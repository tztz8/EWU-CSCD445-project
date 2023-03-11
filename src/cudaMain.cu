// Logger Lib
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include "cudaMain.cuh"

double timeStart;

int device = 0;

float* h_dataA;
float* d_dataA;
float* d_dataB;
int width;
int height;
int threadsPerBlock;

size_t pitch;

unsigned int shared_mem_size;
dim3  grid;
dim3  threads;

void initializeArrays(float *a1, float *a2, int width, int height){
    int i, j;

    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            if(i==0 || j ==0 || i==height-1 || j==width-1){
                a1[i*width + j] = 5.0;
                a2[i*width + j] = 5.0;
            } else {
                a1[i*width + j] = 1.0;
            }
        }
    }
}

// Called when setting things up before graphs loop
__host__ void cudaMainInitialize() {
    SPDLOG_INFO("Initialize Cuda");
    timeStart = 0;

    threadsPerBlock = 32;
    width = 3200;
    height = 3200;

    h_dataA= (float *)malloc(width * height * sizeof(float));
    float * h_dataB= (float *)malloc(width * height * sizeof(float));

    initializeArrays(h_dataA, h_dataB, width, height);

    // Use card 0  (See top of file to make sure you are using your assigned device.)
    cudaSetDevice(device);

    // To ensure alignment, we'll use the code below to pad rows of the arrays when they are
    // allocated on the device.
    // allocate device memory for data A
    cudaMallocPitch( (void**) &d_dataA, &pitch, width * sizeof(float), height);

    // copy host memory to device memory for image A
    cudaMemcpy2D( d_dataA, pitch, h_dataA, width * sizeof(float), width * sizeof(float), height,
                  cudaMemcpyHostToDevice);

    // repeat for second device array
    cudaMallocPitch( (void**) &d_dataB, &pitch, width * sizeof(float), height);

    // copy host memory to device memory for image B
    cudaMemcpy2D( d_dataB, pitch, h_dataB, width * sizeof(float), width * sizeof(float), height,
                  cudaMemcpyHostToDevice) ;

    //***************************
    // setup CUDA execution parameters

    int blockHeight;
    int blockWidth;

    // When testing with small arrays, this code might be useful. Feel free to change it.
    if (threadsPerBlock > width - 2 ){
        blockWidth = 16 * (int) ceil((width - 2) / 16.0);
        blockHeight = 1;
    } else {

        blockWidth = threadsPerBlock;
        blockHeight = 1;
    }

    int gridWidth = (int) ceil( (width - 2) / (float) blockWidth);
    int gridHeight = (int) ceil( (height - 2) / (float) blockHeight);

    // number of blocks required to process all the data.
    int numBlocks =   gridWidth * gridHeight;

    // Each block gets a shared memory region of this size.
    shared_mem_size = ((blockWidth + 2) * 4) * sizeof(float);

    SPDLOG_INFO(spdlog::fmt_lib::format("blockDim.x={} blockDim.y={}    grid = {} x {}", blockWidth, blockHeight, gridWidth, gridHeight));
    SPDLOG_INFO(spdlog::fmt_lib::format("numBlocks = {},  threadsPerBlock = {}   shared_mem_size = {}", numBlocks, threadsPerBlock,  shared_mem_size));

    if(gridWidth > 65536 || gridHeight > 65536) {
        SPDLOG_ERROR("a block dimension is too large.");
    }

    if(threadsPerBlock > 1024) {
        SPDLOG_ERROR("number of threads per block is too large.");
    }

    if(shared_mem_size > 49152) {
        SPDLOG_ERROR("shared memory per block is too large.");
    }

    // Format the grid, which is a collection of blocks.
    grid = dim3( gridWidth, gridHeight, 1);

    // Format the blocks.
    threads = dim3( blockWidth, blockHeight, 1);


    // time the kernel launches using CUDA events
    cudaEvent_t launch_begin, launch_end;
    cudaEventCreate(&launch_begin);
    cudaEventCreate(&launch_end);

    cudaEventRecord(launch_begin,0);
}

__global__ void k1( float* g_dataA, float* g_dataB, int floatpitch, int width)
{
    extern __shared__ float s_data[];
    //Write this kernel to achieve the same output as the provided k0, but you will have to use
    // shared memory.

    // global thread(data) row index
    unsigned int i = blockIdx.y * blockDim.y + threadIdx.y;
    i = i + 1; //because the edge of the data is not processed

    // global thread(data) column index
    unsigned int j = blockIdx.x * blockDim.x + threadIdx.x;
    j = j + 1; //because the edge of the data is not processed

    int threadID = threadIdx.x;

    int s_rowwidth = blockDim.x + 2;

    // Index's

    // -- Global
    int g_ind_0 = (i-1) * floatpitch +  j;
    int g_ind_1 = i * floatpitch + j;
    int g_ind_2 = (i+1) * floatpitch +  j;

    // -- Shared
    int s_ind_0 = threadID + 1 + (s_rowwidth * 0);
    int s_ind_1 = threadID + 1 + (s_rowwidth * 1);
    int s_ind_2 = threadID + 1 + (s_rowwidth * 2);
    int s_index_result = threadID + 1 + (s_rowwidth * 3);

    //Check the boundary. DO NOT copy data from out of bounds, but the thread MUST remain alive for syncthreads
    //Each thread should copy in 3 values
    if( i >= width - 1|| j >= width || i < 1 || j < 1 ) {
        // Do Nothing (Keep thread for __syncthreads)
    } else {
        s_data[s_ind_0-1] = g_dataA[g_ind_0-1];
        s_data[s_ind_1-1] = g_dataA[g_ind_1-1];
        s_data[s_ind_2-1] = g_dataA[g_ind_2-1];
        s_data[s_ind_0+1] = g_dataA[g_ind_0+1];
        s_data[s_ind_1+1] = g_dataA[g_ind_1+1];
        s_data[s_ind_2+1] = g_dataA[g_ind_2+1];
    }
    __syncthreads();

    if( i >= width - 1|| j >= width - 1 || i < 1 || j < 1 ) {
        // Do Nothing (Keep thread for __syncthreads)
    } else {
        const int s_i = 1;
        const int s_j = threadID + 1;
        const int pitch = s_rowwidth;

        //Calculate our cell's result using a LOCAL variable, then write that variable to the result
        s_data[s_index_result] = (
                                         0.2f * s_data[s_i * pitch + s_j] +               //itself
                                         0.1f * s_data[(s_i-1) * pitch +  s_j   ] +       //N
                                         0.1f * s_data[(s_i-1) * pitch + (s_j+1)] +       //NE
                                         0.1f * s_data[ s_i    * pitch + (s_j+1)] +       //E
                                         0.1f * s_data[(s_i+1) * pitch + (s_j+1)] +       //SE
                                         0.1f * s_data[(s_i+1) * pitch +  s_j   ] +       //S
                                         0.1f * s_data[(s_i+1) * pitch + (s_j-1)] +       //SW
                                         0.1f * s_data[ s_i    * pitch + (s_j-1)] +       //W
                                         0.1f * s_data[(s_i-1) * pitch + (s_j-1)]         //NW
                                 ) * 0.95f;
    }

    __syncthreads();

    if( i >= width - 1|| j >= width - 1 || i < 1 || j < 1 ) {
        return;
    }

    g_dataB[i * floatpitch + j] = s_data[s_index_result];
}

// Called for every frame
__host__ void cudaMainUpdate(double time) {
    bool error = false;
    if (error) {
        // Avoid logging messages
        SPDLOG_ERROR("Problem Happen");
    }
//    if (time-timeStart > 0.5F) {
//        SPDLOG_INFO("Start New CUDA Kernel");
//        timeStart = time;
//        cudaError_t code = cudaDeviceSynchronize();
//        if (code != cudaSuccess){
//            SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Device Synchronize error -- {}", cudaGetErrorString(code)));
//        }
//        // check if kernel execution generated an error
//        code = cudaGetLastError();
//        if (code != cudaSuccess){
//            SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Kernel Launch error -- {}", cudaGetErrorString(code)));
//        }
//        k1 <<< grid, threads, shared_mem_size >>>( d_dataA, d_dataB, pitch/sizeof(float), width);
//    }
    cudaError_t code = cudaDeviceSynchronize();
    if (code != cudaSuccess){
        SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Device Synchronize error -- {}", cudaGetErrorString(code)));
    }
    // check if kernel execution generated an error
    code = cudaGetLastError();
    if (code != cudaSuccess){
        SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Kernel Launch error -- {}", cudaGetErrorString(code)));
    }
    k1 <<< grid, threads, shared_mem_size >>>( d_dataA, d_dataB, pitch/sizeof(float), width);
}

// Called as the program close
__host__ void cudaMainCleanUp() {
    SPDLOG_INFO("Clean Up Cuda");

    // check if kernel execution generated an error
    cudaError_t code = cudaGetLastError();
    if (code != cudaSuccess){
        SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Kernel Launch error -- {}", cudaGetErrorString(code)));
    }
    // copy result from device to host
    cudaMemcpy2D( h_dataA, width * sizeof(float), d_dataA, pitch, width * sizeof(float), height,cudaMemcpyDeviceToHost );
}
