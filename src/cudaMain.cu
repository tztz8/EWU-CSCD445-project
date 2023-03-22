// Logger Lib
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <GL/gl.h>

#include "cudaMain.cuh"

int *h_board;
int *d_board;
int *d_nextboard;
int rows;
int cols;
int size;

__host__ int  cpu_get_x_lined(int * board, int col,int y,int x)
{
    return   board[(y * col) + x-1]+
             board[(y * col) + x]+
             board[(y * col) + x+1];
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

//TODO need to check if the y needs to be "-1"
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

    // right with back and bot
    value =     board[y*x+(x - 1)]+
            board[(y)*x+(x - 1)-1]+

    cpu_get_x_lined(board,x,y,2*(x/6)-1)+
    cpu_get_x_lined(board,x,y-1,2*(x/6)-1);

    nextboard[y*x+2*(x/6)-1]= cpuDeadorAlive(value,
                                         board[y*x+2*(x/6)-1]);

    // back with left and bot
    value =     board[y*x+5*(x/6)]+
            board[y*x+5*(x/6)+1]+

    cpu_get_x_lined(board,x,y,(3*(x/6)-1))+
    cpu_get_x_lined(board,x,y-1,(3*(x/6)-1));

    nextboard[y*x+(3*(x/6)-1)]= cpuDeadorAlive(value,
                                             board[y*x+(3*(x/6)-1)]);

// For the top
// top with left and front

    value   =   board[0*x+0]+
                board[0*x+0+1]+

                board[0*x+(3*(x/6))]+
                board[0*x+((x/3)*2)]+
                board[0*x+((x/3)*2)+1]+

                board[(1)*x+(3*(x/6))]+
                board[(1)*x+((x/3)*2)]+
                board[(1)*x+((x/3)*2)+1];

    nextboard[0*x+((x/3)*2)]= cpuDeadorAlive(value,
                                               board[0*x+((x/3)*2)]);

    //top with right and front
    value =     board[0*x+(x/6-1)-1]+
            board[0*x+(x/6-1)]+

            board[0*x+((x/6)*5-1)-1]+
            board[0*x+((x/6)*5-1)]+
            board[0*x+(x/6)]+

            board[(1)*x+((x/6)*5-1)-1]+
            board[(1)*x+((x/6)*5-1)]+
            board[(0)*x+(x/6)+1];

    nextboard[0*x+((x/6)*5-1)]= cpuDeadorAlive(value,
                                             board[0*x+((x/6)*5-1)]);

    //Top with back and right
    value = board[0*x+(2*(x/6))]+
            board[0*x+(2*(x/6))+1];

            board[0*x+(2*(x/6)-1)]+
            board[y*x+((x/6)*5-1)]+
            board[y*x+((x/6)*5-1)+1]+

            board[1*x+(2*(x/6)-1)]+
            board[(y-1)*x+((x/6)*5-1)]+
            board[(y-1)*x+((x/6)*5-1)-1];

            nextboard[y*x+((x/6)*5-1)] = cpuDeadorAlive(value,
                                                       board[y*x+((x/6)*5-1)]);


    // Top with back left
/*    value =     board[start*col+rightend-1]+
            board[(wrapBoxRow-1)*col+topend]+
            board[(wrapBoxRow-1)*col+topend-1]+

            board[start*col+rightend]+
            board[wrapBoxRow*col+topend]+
            board[wrapBoxRow*col+topend-1]+

            board[start*col+backstart]+
            board[start*col+backstart+1];*/

/*
 *


    //printf("%d", frontstart);



 */
}

__device__ int get_x_lined(int * board, int col,int x,int y)
{
    return   board[(x * col) + y-1]+
             board[(x * col) + y]+
             board[(x * col) + y+1];
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


__global__ void two_d_conway_block(int *board, int *next_board, int cols, int x_size, int y_size)
{
    // global thread(data) row index
    unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

    // global thread(data) column index
    unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned int index = x*cols+y;
    int total = 0;

    if(y < x_size && x < y_size) {
        total = cudaAddUpLife(board, cols, x, y);
        next_board[index] = deadorAlive(total, board[index]);
    }
}


void launch_two_d_conway_block(dim3 blocksize)
{
    //edge_length = size - 2
    //const dim3 numBlocks(edge_length/blocksize + (1&&(edge_length%blocksize)), 1, 1);
    const dim3 numBlocksMain(cols*rows + size*4-2,1,1);
    const dim3 numBlocks(cols*rows + size-2,1,1);
    two_d_conway_block<<<numBlocksMain, blocksize>>>(d_board + 1 + cols, d_nextboard + 1 + cols, cols, (rows * size*4)-2, cols-2);
    two_d_conway_block<<<numBlocks, blocksize>>>(d_board + 1 + size*4 + cols, d_nextboard + 1 + size*4 + cols, cols, size-2, size-2);
    two_d_conway_block<<<numBlocks, blocksize>>>(d_board + 1 + size*5 + cols, d_nextboard + 1 + size*5 + cols, cols, size-2, size-2);
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


void cuda_launch_conway_edges(int blocksize) {
    const int edge_length = size - 2;
    const dim3 numBlocks(edge_length/blocksize + (1&&(edge_length%blocksize)), 1, 1);

//    conway_edges(d_board + cols, d_nextboard + cols, d_board + size*4 - 1 + cols, cols, cols, 1, edge_length); // body wrap
//    conway_edges(d_board + size*4 - 1 + cols, d_nextboard + size*4 - 1 + cols, d_board + cols, cols, cols, -1, edge_length);

    conway_edges <<<numBlocks, blocksize>>>(d_board + 1, d_nextboard + 1, d_board + (4 * size)+1, 1, 1, cols, edge_length); //top front
    conway_edges <<<numBlocks, blocksize>>>(d_board + 1 + size*2, d_nextboard + 1 + size*2, d_board + 5*size - 2 + cols*(rows-1), 1, -1, cols, edge_length); //top back
    conway_edges <<<numBlocks, blocksize>>>(d_board + 1 + size, d_nextboard + 1 + size, d_board + 5*size - 1 + cols, 1, cols, cols, edge_length); //top right
    conway_edges <<<numBlocks, blocksize>>>(d_board + 1 + size*3, d_nextboard + 1 + size*3, d_board + 4*size + cols*(rows-2), 1, (-cols), cols, edge_length); //top left

    conway_edges <<<numBlocks, blocksize>>>(d_board+cols*(rows-1)+1, d_nextboard+cols*(rows-1)+1, d_board + size*5 + 1, 1, 1, (-cols), edge_length); // bottom front
}
// Called when setting things up before graphs loop
__host__ void cudaMainInitialize(int size) {
    SPDLOG_INFO("Initialize Cuda");
    float timeStart = 0;

    h_board = (int *)malloc(rows*cols*sizeof(int));

    //cuda memory allocation
    cudaMalloc((void**) &d_board, sizeof(int)*size);
    cudaMalloc((void**) &d_nextboard, sizeof(int)*size);

    cudaMemset(d_board,0,sizeof(int)*size);

    cudaMemcpy(d_board,h_board, sizeof(int)*size, cudaMemcpyHostToDevice);


}

// Called for every frame
int * cudaMainUpdate() {
    cudaError_t code = cudaDeviceSynchronize();
    if (code != cudaSuccess){
        SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Device Synchronize error -- {}", cudaGetErrorString(code)));
    }
    // check if kernel execution generated an error
    code = cudaGetLastError();
    if (code != cudaSuccess){
        SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Kernel Launch error -- {}", cudaGetErrorString(code)));
    }
    const int blocksize = 32;
    const dim3 blockdim(blocksize, blocksize, 1);

    // #### Corners ####
    cornerHost(d_board, d_nextboard, rows,cols);

    // #### Field ####
    launch_two_d_conway_block(blockdim);

    // #### Edges ####
    const int edge_length = size - 2;
    const dim3 numBlocks(edge_length/blocksize + (1&&(edge_length%blocksize)), 1, 1);

    cuda_launch_conway_edges(blocksize);
//    conway_edges(board + cols, next_board + cols, board + size*4 - 1 + cols, cols, cols, 1, edge_length); // body wrap
//    conway_edges(board + size*4 - 1 + cols, next_board + size*4 - 1 + cols, board + cols, cols, cols, -1, edge_length);
//
//    conway_edges <<<numBlocks, blocksize>>>(board + 1, next_board + 1, board + (4 * size)+1, 1, 1, cols, edge_length); //top front
//    conway_edges <<<numBlocks, blocksize>>>(board + 1 + size*2, next_board + 1 + size*2, board + 5*size - 2 + cols*(rows-1), 1, -1, cols, edge_length); //top back
//    conway_edges <<<numBlocks, blocksize>>>(board + 1 + size, next_board + 1 + size, board + 5*size - 1 + cols, 1, cols, cols, edge_length); //top right
//    conway_edges <<<numBlocks, blocksize>>>(board + 1 + size*3, next_board + 1 + size*3, board + 4*size + cols*(rows-2), 1, (-cols), cols, edge_length); //top left
//
//    conway_edges <<<numBlocks, blocksize>>>(board+cols*(rows-1)+1, next_board+cols*(rows-1)+1, board + size*5 + 1, 1, 1, (-cols), edge_length); // bottom front


    // copy result from device to host

}

// Called as the program close
__host__ void cudaMainCleanUp() {
    SPDLOG_INFO("Clean Up Cuda");

    // check if kernel execution generated an error
    cudaError_t code = cudaGetLastError();
    if (code != cudaSuccess){
        SPDLOG_ERROR(spdlog::fmt_lib::format("Cuda Kernel Launch error -- {}", cudaGetErrorString(code)));
    }

    cudaFree(d_board);
    cudaFree(d_nextboard);
    free(h_board);
}