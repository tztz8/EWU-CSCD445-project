// Logger Lib
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <GL/gl.h>

#include "cudaMain.cuh"
#include "string.h"
#include "stdlib.h"

int *h_board;
int *h_next_board;
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
__host__ void cornerHost(int* h_board, int * h_next_board,int y, int x)
{
    int *board =h_board;
    int *nextboard = h_next_board;
    int value=0;
    y--;
    // the first four or the corners of the main box (warps)
    //left corner of front to top and left (warps)
    value = board[((x/3)*2)]+
            board[((x/3)*2)+1]+
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
    value =  board[0*x+(3*(x/6)-1)]+
             board[0*x+(3*(x/6)-1)-1]+

            board[0*x+(3*(x/6))]+
            board[(y)*x+((x/3)*2)]+
            board[(y)*x+((x/3)*2)+1]+

            board[0*x+(3*(x/6))+1]+
            board[(y-1)*x+((x/3)*2)]+
            board[(y-1)*x+((x/3)*2)+1];

    nextboard[(y)*x+((x/3)*2)] = cpuDeadorAlive(value,
                                                board[(y)*x+((x/3)*2)]);


    //botmerge corner
    // bot with front and left
        value = board[y*x]+
                board[y*x+1]+

                board[y*x+(4*(x/6)-1)]+
                board[0*x+((x/6)*5)]+
                board[0*x+((x/6)*5)+1]+

                //was wrapBoxRow+1
                board[(y) * x + (4*(x/6))]+
                board[(1)*x+((x/6)*5)]+
                board[(1)*x+((x/6)*5)+1];

     nextboard[0*x+((x/6)*5)] = cpuDeadorAlive(value,
                                                board[0*x+((x/6)*5)]);

    //bot with front and right
    x =     board[y*x+ (x/6-1)-1]+
            board[y*x+ (x/6-1)]+

            board[0*x+(x - 1)-1]+
            board[0*x+(x - 1)]+
            board[y*x+(x/6)]+

            board[(1)*x+(x - 1)-1]+
            board[(1)*x+(x - 1)]+
            board[(y-1)*x+ (x/6)];
    nextboard[0*x+(x - 1)] = cpuDeadorAlive(value,
                                              board[0*x+(x - 1)]);



    //bot with right and back
    x =     board[y*x+(2*(x/6))]+
            board[y*x+(2*(x/6))+1]+

            board[y*x+(2*(x/6)-1)]+
            board[(y)*x+(x - 1)]+
            board[(y)*x+(x - 1)-1]+

            board[y*x+(2*(x/6)-1)-1]+
            board[(y-1)*x+(x - 1)]+
            board[(y-1)*x+(x - 1)-1];

        nextboard[(y)*x+(x - 1)] = cpuDeadorAlive(value,
                                              board[(y)*x+(x - 1)]);



    //bot with back and left

    x =     board[y*x+(2*(x/6))]+
            board[y*x+(2*(x/6))]+

            board[y*x+(3*(x/6))]+
            board[y*x+((x/6)*5)]+
            board[y*x+((x/6)*5)-1]+

            board[y*x+(3*(x/6))+1]+
            board[(y-1)*x+((x/6)*5)-1]+
            board[(y-1)*x+((x/6)*5)-1];
    nextboard[y*x+((x/6)*5)] = cpuDeadorAlive(value,
                                              board[y*x+((x/6)*5)]);


}

__device__ int get_x_lined(int * board, int col,int x,int y)
{
    return  (board[(y * col) + x-1])+
            (board[(y * col) + x])+
            (board[(y * col) + x+1]);
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
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // global thread(data) column index
    int x = blockIdx.x * blockDim.x + threadIdx.x;

    int index = y*cols+x;
    int total = 0;

    if((y < y_size) && (x < x_size)) {

        total = cudaAddUpLife(board,cols,x,y);
//        next_board[index] = deadorAlive(total,board[index]);

//        total += board[index-1-cols] + board[index-cols] + board[index + 1 - cols] +
//                 board[index - 1] + board[index] + board[index+1] +
//                 board[index + cols - 1] + board[index + cols] + board[index + cols +1];
        next_board[index] = (total == 3) || (total-(board[index]) == 3);
    }

}


void launch_two_d_conway_block(dim3 blocksize)
{
    const dim3 numBlocksMain((cols)/blocksize.x +1 ,rows/blocksize.y + 1,1);
    const dim3 numBlocks(size/blocksize.x + 1,size/blocksize.y + 1,1);
    two_d_conway_block<<<numBlocksMain, blocksize>>>(d_board + 1 + cols, d_nextboard + 1 + cols, cols, (size*4)-2, size-2);
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

    conway_edges<<<numBlocks, blocksize>>>(d_board + cols, d_nextboard + cols, d_board + size*4 - 1 + cols, cols, cols, 1, edge_length); // front to left
    conway_edges<<<numBlocks, blocksize>>>(d_board + size*4 - 1 + cols, d_nextboard + size*4 - 1 + cols, d_board + cols, cols, cols, -1, edge_length); //left to front

    conway_edges <<<numBlocks, blocksize>>>(d_board + 1, d_nextboard + 1, d_board + (4 * size)+1, 1, 1, cols, edge_length); //top to front
    conway_edges <<<numBlocks, blocksize>>>(d_board + (4 * size)+1, d_nextboard + (4 * size)+1, d_board + 1, 1, 1, cols, edge_length); //front to top

    conway_edges <<<numBlocks, blocksize>>>(d_board + 1 + size*2, d_nextboard + 1 + size*2, d_board + 5*size - 2 + cols*(rows-1), 1, -1, cols, edge_length); //top to back
    conway_edges <<<numBlocks, blocksize>>>(d_board + 5*size - 2 + cols*(rows-1), d_nextboard + 5*size - 2 + cols*(rows-1), d_board + 1 + size*2, -1, 1, -cols, edge_length); //back to top

    conway_edges <<<numBlocks, blocksize>>>(d_board + 1 + size, d_nextboard + 1 + size, d_board + 5*size - 1 + cols, 1, cols, cols, edge_length); //top to right
    conway_edges <<<numBlocks, blocksize>>>(d_board + 5*size - 1 + cols, d_nextboard + 5*size - 1 + cols, d_board + 1 + size, cols, 1, -1, edge_length); //right to top

    conway_edges <<<numBlocks, blocksize>>>(d_board + 1 + size*3, d_nextboard + 1 + size*3, d_board + 4*size + cols*(rows-2), 1, (-cols), cols, edge_length); //top to left
    conway_edges <<<numBlocks, blocksize>>>(d_board + 4*size + cols*(rows-2), d_nextboard + 4*size + cols*(rows-2), d_board + 1 + size*3, (-cols), 1, 1, edge_length); //left to top

    conway_edges <<<numBlocks, blocksize>>>(d_board+cols*(rows-1)+1, d_nextboard+cols*(rows-1)+1, d_board + size*5 + 1, 1, 1, (-cols), edge_length); // bottom to front
    conway_edges <<<numBlocks, blocksize>>>(d_board + size*5 + 1, d_nextboard + size*5 + 1, d_board+cols*(rows-1)+1, 1, 1, (-cols), edge_length); // front to bottom

    conway_edges <<<numBlocks, blocksize>>>(d_board + 1 + size*2, d_nextboard + 1 + size*2, d_board + size*6 - 2 + cols*rows-1, 1, -1, (-cols), edge_length); // bottom to back
    conway_edges <<<numBlocks, blocksize>>>(d_board + size*6 - 2 + cols*rows-1, d_nextboard + size*6 - 2 + cols*rows-1, d_board + 1 + size*2, -1, 1, (-cols), edge_length); // back to bottom
}

// Called when setting things up before graphs loop
__host__ void cudaMainInitialize(int size_set) {
    SPDLOG_INFO("Initialize Cuda");
    float timeStart = 0;
    size = size_set;
    rows = size;
    cols = size*6;

    h_board = (int *) calloc(rows*cols, sizeof(int));
    h_next_board = (int *) calloc(rows*cols, sizeof(int));

    //Initialize a pattern in the conway grid
    /*for (int i = 0; i < cols; ++i) {
        h_board[(3 * cols) + i] = 1;
    }*/

    h_board[3*cols + 10] = 1;
    h_board[3*cols + 11] = 1;
    h_board[3*cols + 12] = 1;
    h_board[4*cols + 10] = 1;
    h_board[5*cols + 11] = 1;

    h_board[3*cols + 10 + size] = 1;
    h_board[3*cols + 11 + size] = 1;
    h_board[3*cols + 12 + size] = 1;
    h_board[4*cols + 10 + size] = 1;
    h_board[5*cols + 11 + size] = 1;

    //cuda memory allocation
    cudaMalloc(&d_board, sizeof(int)*rows*cols);
    cudaMalloc(&d_nextboard, sizeof(int)*rows*cols);

    cudaMemcpy(d_board, h_board, sizeof(int)*rows*cols, cudaMemcpyHostToDevice);
}

// Called for every frame
int *cudaMainUpdate() {
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

    memcpy(h_next_board, h_board, rows*cols*sizeof(*h_board));

    // #### Corners ####
    //cornerHost(h_board, h_next_board, rows,cols);

    cudaMemcpy(d_nextboard, h_next_board, rows*cols*sizeof(*d_nextboard), cudaMemcpyHostToDevice);
    cudaMemcpy(d_board, h_board, rows*cols*sizeof(*d_nextboard), cudaMemcpyHostToDevice);
    // #### Field ####
    launch_two_d_conway_block(blockdim);

    // #### Edges ####
    const int edge_length = size - 2;
    const dim3 numBlocks(edge_length/blocksize + (1&&(edge_length%blocksize)), 1, 1);

    cuda_launch_conway_edges(blocksize);

    cudaMemcpy(h_board, d_nextboard, rows*cols*sizeof(*d_nextboard), cudaMemcpyDeviceToHost);
    {
        int * temp = d_board;
        d_board = d_nextboard;
        d_nextboard = temp;
    }
    return h_board;
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
    free(h_next_board);
}