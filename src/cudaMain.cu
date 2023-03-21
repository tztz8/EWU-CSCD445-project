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

    cudaTexID = genCudaTexImage();

}
__device__ int  get_x_lined(int * board, int col,int y,int x)
{
    return   board[(y * col) + x-1]+
             board[(y * col) + x]+
             board[(y * col) + x+1];
}

__device__ int  get_y_lined(int * board, int col,int y,int x)
{
    return   board[(y * col+1) + x]+
             board[(y * col) + x]+
             board[(y * col-1) + x];
}
__device__ int cudaAddUpLife(int * board,int col, int i, int j)
{
    return  get_x_lined(board, col, i+1,j)+
            get_x_lined(board, col, i,j)+
            get_x_lined(board, col, i-1,j);
}
__device__ int deadorAlive(int value, int currentLocation)
{
    if(currentLocation==1)
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
//add WrapMain from CPU but should include to
__global__ void wrapMain( int *board, int *nextboard, int col, int row, int start, int wrapBoxColumn)
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

        nextboard[j*col+start] = deadorAlive(x, board[j*col+start]);

        x = board[(j-1)*col+start] +
            board[(j-1)*col+wrapBoxColumn] +
            board[(j-1)*col+wrapBoxColumn - 1] +
            board[(j)*col+start] +
            board[(j)*col+wrapBoxColumn] +
            board[(j)*col+wrapBoxColumn - 1] +
            board[(j+1)*col+start] +
            board[(j+1)*col+wrapBoxColumn] +
            board[(j+1)*col+wrapBoxColumn - 1];
        nextboard[j*col+wrapBoxColumn] = deadorAlive(x, board[j*col+wrapBoxColumn]);
    }
    
}
__global__ void merge_top_front( int *board, int *nextboard, int col, int row, int start, int topstart)
{
    // global thread(data) row index
    unsigned int i = blockIdx.y * blockDim.y + threadIdx.y;

    // global thread(data) column index
    unsigned int j = blockIdx.x * blockDim.x + threadIdx.x;

    int x;
    x= get_x_lined(board,col,start,topstart+i)+
       get_x_lined(board,col,start,i)+
       get_x_lined(board,col,start+1,i);
    // y = start, x = i
    nextboard[start*col+i] = deadorAlive(x,board[start*col+i]);
    
}
__global__ void merge_top_back( int *board, int *nextboard, int col, int row, int start, int topstart, int wrapBoxRow, int backstart)
{
    // global thread(data) row index
    unsigned int i = blockIdx.y * blockDim.y + threadIdx.y;

    // global thread(data) column index
    unsigned int j = blockIdx.x * blockDim.x + threadIdx.x;

    int x;
    x= get_x_lined(board,col,wrapBoxRow,topstart+i)+
       get_x_lined(board,col,start,backstart+i)+
       get_x_lined(board,col,start+1,backstart+i);
    // y = start, x = backstart+i
    nextboard[start*col+(i+backstart)] = deadorAlive(x,board[start*col+(i+backstart)]);
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
    //cudaMemcpy2D( h_dataA, width * sizeof(float), d_dataA, pitch, width * sizeof(float), height,cudaMemcpyDeviceToHost );
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
    //cudaMemcpy2D( h_dataA, width * sizeof(float), d_dataA, pitch, width * sizeof(float), height,cudaMemcpyDeviceToHost );

    free(h_dataA);
}
