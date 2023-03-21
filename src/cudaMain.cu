// Logger Lib
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <GL/gl.h>

#include "cudaMain.cuh"

// Texture ID
GLuint cudaTexID;

double timeStart;

int device = 0;

float* h_dataA;
float* d_dataA;
float* d_dataB;
int width;
int height;
int threadsPerBlock;

GLuint genCudaTexImage() {
    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,  GL_RED,
                 GL_FLOAT, h_dataA);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, tid);
    return tid;
}

// Called when setting things up before graphs loop
__host__ void cudaMainInitialize(int size) {
    SPDLOG_INFO("Initialize Cuda");
    timeStart = 0;

    threadsPerBlock = 32;
    width = size * 6;
    height = size;

    h_dataA= (float *)malloc(width * height * sizeof(float));
    float * h_dataB= (float *)malloc(width * height * sizeof(float));

    initializeArrays(h_dataA, h_dataB, width, height);

    cudaTexID = genCudaTexImage();

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

    free(h_dataB);

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

__device__ int deadorAlive(int value, int current)
{
    if(current==1)
    {
        if(value-1==2||value-1==3)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if(value==3)
        {
            return 1;
        }
        else
            return 0;
    }
}
//add WrapMain from CPU
__global__ void k1( int *board, int *nextboard, int col, int row, int start, int wrapBoxColumn)
{
    // global thread(data) row index
    unsigned int i = blockIdx.y * blockDim.y + threadIdx.y;

    // global thread(data) column index
    unsigned int j = blockIdx.x * blockDim.x + threadIdx.x;

    int x;
    if(i < row - 1 && j < col - 1)
    {
        x = board[(j-1)*col+wrapBoxColumn]+
            board[(j-1)*col+start]+
            board[(j-1)*col+start+1]+
            board[(j)*col+wrapBoxColumn]+
            board[(j)*col+start]+
            board[(j)*col+start+1]+
            board[(j+1)*col+wrapBoxColumn]+
            board[(j+1)*col+start]+
            board[(j+1)*col+start+1];

        nextboard[i*col+x] = deadorAlive(x, board[i*col+x]);

        x = board[(j-1)*col+start] +
            board[(j-1)*col+wrapBoxColumn] +
            board[(j-1)*col+wrapBoxColumn - 1] +
            board[(j)*col+start] +
            board[(j)*col+wrapBoxColumn] +
            board[(j)*col+wrapBoxColumn - 1] +
            board[(j+1)*col+start] +
            board[(j+1)*col+wrapBoxColumn] +
            board[(j+1)*col+wrapBoxColumn - 1];
        nextboard[i*col+x] = deadorAlive(x, board[i*col+x]);
    }
    
}
//kept just in case it has usful info
__global__ void oldk1_from_hmwk( float* g_dataA, float* g_dataB, int floatpitch, int width)
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
__host__ void cudaMainUpdate() {
    cudaError_t code = cudaDeviceSynchronize();
    if (code != cudaSuccess){
        SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Device Synchronize error -- {}", cudaGetErrorString(code)));
    }
    // check if kernel execution generated an error
    code = cudaGetLastError();
    if (code != cudaSuccess){
        SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Kernel Launch error -- {}", cudaGetErrorString(code)));
    }

    // copy result from device to host
    cudaMemcpy2D( h_dataA, width * sizeof(float), d_dataA, pitch, width * sizeof(float), height,cudaMemcpyDeviceToHost );
    glDeleteTextures(1, &cudaTexID);
    cudaTexID = genCudaTexImage();

    //k1 <<< grid, threads, shared_mem_size >>>( d_dataA, d_dataB, pitch/sizeof(float), width);
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

    free(h_dataA);
}
