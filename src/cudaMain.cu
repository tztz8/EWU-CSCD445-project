// Logger Lib
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <GL/gl.h>

#include "cudaMain.cuh"

// Texture ID
GLuint cudaTexID;

double timeStart;

int device = 0;

int *h_board;
int *d_board;
int *d_nextboard;
int rows;
int cols;
int size;
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

__host__ int  cpu_get_x_lined(int * board, int col,int y,int x)
{
    return   board[(y * col) + x-1]+
             board[(y * col) + x]+
             board[(y * col) + x+1];
}

__host__ int  cpu_get_y_lined(int * board, int col,int y,int x) {
    return board[(y * col + 1) + x] +
           board[(y * col) + x] +
           board[(y * col - 1) + x];
}

__host__ int cpuDeadorAlive(int value, int currentLocation){
    if (currentLocation == 1)
        if (value - 1 == 2 || value - 1 == 3)
            return 1;
        else
            return 0;
    else if (value == 3)
        return 1;
    else
        return 0;
}

// the y and x values should be the max-1 and are static for this
// function
__host__ void cornerHost(int* board, int * nextboard,int y, int x)
{
    int value=0;
    // the first four or the corners of the main box (warps)
    //left corner of front to top and left (warps)
    value = board[0]+
            board[0+1]+

            board[4*(x/6)-1]+
            board[0]+
            board[1]+

            board[(1)*x+(4*(x/6)-1)]+
            board[(1)*x+0]+
            board[(1)*x+0+1];

    nextboard[0]= cpuDeadorAlive(value,board[0]);

    // Right top corner of Left to top and front (warps)
    value = board[x*(y-1)+((x/3)*2)]+
            board[x*(y)+((x/3)*2)]+

            board[0*x+(4*(x/6)-1)-1]+
            board[0*x+(4*(x/6)-1)]+
            board[0*x+0]+

            board[(0+1)*x+(4*(x/6)-1)-1]+
            board[(0+1)*x+(4*(x/6)-1)]+
            board[(0+1)*x+0+1];

    nextboard[0*x+(4*(x/6)-1)]= cpuDeadorAlive(value,board[0*x+(4*(x/6)-1)]);

    // left bot corner of front with left and bot (warps)

    value = board[(y-1)*x+(4*(x/6)-1)]+
            board[(y-1)*x+((x/3)*2)]+
            board[(y-1)*x+((x/3)*2)+1]+

            board[y*x+(4*(x/6)-1)]+
            board[y*x+0]+
            board[y*x+0+1]+

            board[0*x+((x/6)*5)]+
            board[0*x+((x/6)*5)+1];

            nextboard[y*x+0]= cpuDeadorAlive(value,
                                             board[y*x+0]);
    // right bot corner of left with top and bot (warps)
    value = board[(y-1)*x+(4*(x/6)-1)-1]+
            board[(y-1)*x+(4*(x/6)-1)]+
            board[(y-1)*x+0]+

            board[(y)*x+(4*(x/6)-1)-1]+
            board[(y)*x+(4*(x/6)-1)]+
            board[(y)*x+0]+

            board[0*x+((x/6)*5)]+
            board[(0+1)*x+((x/6)*5)];

    nextboard[(y)*x+(4*(x/6)-1)]= cpuDeadorAlive(value,
                                     board[(y)*x+(4*(x/6)-1)]);

    // left top corners with no wraps
    // so the next set are all left top corners
    // right with front and top
    value =     board[0*x+((x/6)*5-1)]+
            board[(0+1)*x+((x/6)*5-1)]+
            cpu_get_x_lined(board,x,0,(x/6))+
            cpu_get_x_lined(board,x,1,(x/6));

    nextboard[0*x+(x/6)]= cpuDeadorAlive(value,
                                         board[0*x+(x/6)]);

    // back with right and top
    value =     board[y*x+((x/3)*2)]+
                board[y*x+((x/3)*2)+1]+

    cpu_get_x_lined(board,x,0,(2*(x/6)))+
    cpu_get_x_lined(board,x,1,(2*(x/6)));

    nextboard[0*x+(2*(x/6))]= cpuDeadorAlive(value,
                                         board[0*x+(2*(x/6))]);

    // left with back and top
    value = board[0*x+((x/3)*2)]+
            board[(0+1)*x+((x/3)*2)]+

    cpu_get_x_lined(board,x,0,(3*(x/6)))+
    cpu_get_x_lined(board,x,1,(3*(x/6)));

    nextboard[0*x+(3*(x/6))]= cpuDeadorAlive(value,
                                             board[0*x+(3*(x/6))]);

    // the next set is the right top corners
    // front with right and top
    value =     board[0*x+((x/6)*5-1)]+
            board[0*x+((x/6)*5-1)-1]+

    cpu_get_x_lined(board,x,0,(x/6-1))+
    cpu_get_x_lined(board,x,1,(x/6-1));

    nextboard[0*x+(x/6-1)]= cpuDeadorAlive(value,
                                             board[0*x+(x/6-1)]);

    // right with top and back
    value =     board[y*x+((x/6)*5-1)]+
            board[(y-1)*x+((x/6)*5-1)]+

    cpu_get_x_lined(board,x,0,(2*(x/6)-1))+
    cpu_get_x_lined(board, x,0+1,(2*(x/6)-1));

    nextboard[0*x+(2*(x/6)-1)]= cpuDeadorAlive(value,
                                           board[0*x+(2*(x/6)-1)]);


    // right corner back
    value =     board[y*x+((x/6)*5-1)]+
            board[y*x+((x/6)*5-1)-1]+

    cpu_get_x_lined(board,x,0,(3*(x/6)-1))+
    cpu_get_x_lined(board,x,0+1,(3*(x/6)-1));

    nextboard[0*x+(2*(x/6)-1)]= cpuDeadorAlive(value,
                                               board[0*x+(3*(x/6)-1)]);

    // the next set handles the left bots corners
    // right with front and bot
    value = board[0*x+(x - 1)]+
            board[0*x+(x - 1)-1]+

    cpu_get_x_lined(board,x,y,(x/6))+
    cpu_get_x_lined(board,x,y-1,(x/6));

    nextboard[y*x+(x/6-1)]= cpuDeadorAlive(value,
                                               board[y*x+(x/6-1)]);

    // back with right and bot
    value = board[(y*x) + (x - 1)]+
            board[((y-1)*x) + (x - 1)]+

    cpu_get_x_lined(board,x,y,(2*(x/6)))+
    cpu_get_x_lined(board,x,y-1,(2*(x/6)));

    nextboard[y*x+(2*(x/6)-1)]= cpuDeadorAlive(value,
                                           board[y*x+(2*(x/6)-1)]);
    // left with back and bot
    value =     board[y*x+((x/6)*5)]+
            board[(y-1)*x+((x/6)*5)+1]+

    cpu_get_x_lined(board,x,y,(3*(x/6)))+
    cpu_get_x_lined(board,x,y-1,(3*(x/6)));

    nextboard[y*x+(3*(x/6))]= cpuDeadorAlive(value,
                                               board[y*x+(3*(x/6))]);

    // bot right corners of main
    // front with right and bot

    value = board[0*x+(5*(x/6))]+
    board[(0+1)*x+(5*(x/6))]+

    cpu_get_x_lined(board,x,y,(x/6))+
    cpu_get_x_lined(board,x,y-1,(x/6));

    nextboard[y*x+(x/6)]= cpuDeadorAlive(value,
                                             board[y*x+(x/6)]);

    // right corner back
    value =     board[y*x+(x - 1)]+
            board[(y)*x+(x - 1)-1]+

            getxlinead(board, col,wrapBoxRow,backstart)+
            getxlinead(board, col,wrapBoxRow-1,backstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,backstart);

    cpu_get_x_lined(board,x,y,(x/6))+
    cpu_get_x_lined(board,x,y-1,(x/6));

    nextboard[y*x+(x/6)]= cpuDeadorAlive(value,
                                         board[y*x+(x/6)]);
    /*
     *
    x =

    //printboard(nextboard);

    // bot left corners of main
    // left corner back
    x =     board[start*col+botend]+
            board[start*col+botend-1]+
            getxlinead(board, col,wrapBoxRow,backstart)+
            getxlinead(board, col,wrapBoxRow-1,backstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,backstart);

    //printboard(nextboard);
    // bot left corners of main
    // left corner left
    x =     board[start*col+botstart]+
            board[(start+1)*col+botstart]+
            getxlinead(board, col,wrapBoxRow,leftstart)+
            getxlinead(board, col,wrapBoxRow-1,backstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,leftstart);

     */

}

__device__ int get_x_lined(int * board, int col,int x,int y)
{
    return   board[(x * col) + y-1]+
             board[(x * col) + y]+
             board[(x * col) + y+1];
}

__device__ int  get_y_lined(int * board, int col,int x,int y)
{
    return   board[(x * col+1) + y]+
             board[(x * col) + y]+
             board[(x * col-1) + y];
}
__device__ int cudaAddUpLife(int * board,int col, int x, int y)
{
    return  get_x_lined(board, col, x,y+1)+
            get_x_lined(board, col, x,y)+
            get_x_lined(board, col, x,y-1);
}
__device__ int deadorAlive(int value, int currentLocation)
{
    if (currentLocation == 1)
        if (value - 1 == 2 || value - 1 == 3)
            return 1;
        else
            return 0;
    else if (value == 3)
        return 1;
    else
        return 0;
}
__device__ int addUpLife(int * board,int col, int j, int i)
{
    return  get_x_lined(board, col, j+1,i)+
            get_x_lined(board, col, j,i)+
            get_x_lined(board, col, j-1,i);
}

__global__ void two_d_conway_block(int *board, int *next_board, int cols, int x_size, int y_size)
{
    // global thread(data) row index
    unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

    // global thread(data) column index
    unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned int index = x*cols+y;
    int total = 0;

    if(y < x_size && x < y_size)
    {
        total = addUpLife(board,cols,x,y);
        next_board[index] = deadorAlive(total,board[index]);
    }
}

__global__ void conway_edges(int *self, int *self_next, int *other, int self_idx_step, int other_idx_step, int adjacent_offset, int len) {
    //Determine the index
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx>=len) return;

    //Rebase the indexes for this thread
    self = self + self_idx_step*idx;
    other = other + other_idx_step*idx;
    self_next = self_next + self_idx_step*idx;

    //Get the complete sum
    int sum =   *(other - other_idx_step) + (*other) + *(other + other_idx_step) +
                *(self  - self_idx_step ) + (*self ) + *(self  + self_idx_step ) +
                *(self + adjacent_offset - self_idx_step) + *(self + adjacent_offset) + *(self + adjacent_offset + self_idx_step);

    //Write the result
    *self_next = (sum == 3) || (sum-(*self) == 3);
}

//temporarily working on my launches here
void launchit () {
    const int edge_length = size - 2;
    const dim3 numBlocks(edge_length/blocksize + (1&&(edge_length%blocksize)), 1, 1);

    conway_edges(int *self, int *self_next, int *other, int self_idx_step, int other_idx_step, int adjacent_offset, int len);

    conway_edges(board + cols, next_board + cols, board + size*4 - 1 + cols, cols, cols, 1, edge_length); // body wrap
    conway_edges(board + size*4 - 1 + cols, next_board + size*4 - 1 + cols, board + cols, cols, cols, -1, edge_length);

    conway_edges <<<numBlocks, blocksize>>>(board + 1, next_board + 1, board + (4 * size)+1, 1, 1, cols, edge_length); //top front
    conway_edges <<<numBlocks, blocksize>>>(board + 1 + size*2, next_board + 1 + size*2, board + 5*size - 2 + cols*(rows-1), 1, -1, cols, edge_length); //top back
    conway_edges <<<numBlocks, blocksize>>>(board + 1 + size, next_board + 1 + size, board + 5*size - 1 + cols, 1, cols, cols, edge_length); //top right
    conway_edges <<<numBlocks, blocksize>>>(board + 1 + size*3, next_board + 1 + size*3, board + 4*size + cols*(rows-2), 1, (-cols), cols, edge_length); //top left

    conway_edges <<<numBlocks, blocksize>>>(board+cols*(rows-1)+1, next_board+cols*(rows-1)+1, board + size*5 + 1, 1, 1, (-cols), edge_length); // bottom front

}
// Called when setting things up before graphs loop
__host__ void cudaMainInitialize(int size) {
    SPDLOG_INFO("Initialize Cuda");
    timeStart = 0;

    //memory allocation
    cudaTexID = genCudaTexImage();
    h_board = (int *)malloc(rows*cols*sizeof(int));

    //cuda memory allocation
    cudaMalloc((void**) &d_board, sizeof(int)*size);
    cudaMalloc((void**) &d_nextboard, sizeof(int)*size);

    cudaMemset(d_board,0,sizeof(int)*size);

    cudaMemcpy(d_board,h_board, sizeof(int)*size, cudaMemcpyHostToDevice);

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

    glDeleteTextures(1, &cudaTexID);
    cudaTexID = genCudaTexImage();


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
