#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include <string>
#include "GameOfLifeLogic.h"

#define start 0
//int wrapBoxRow = row - 1;
#define wrapBoxRow (row - 1)
//int wrapBoxColumn = 2*(column/3) - 1;
#define wrapBoxColumn (2*(col/3) - 1)
//int topstart = (column/3)*2;
#define topstart ((col/3)*2)
//int topend = (column/6)*5-1;
#define topend ((col/6)*5-1)
//int botstart = (column/6)*5;
#define botstart ((col/6)*5)
//int botend = column-1;
#define botend (col - 1)
//int frontstart = start;
#define frontstart 0
//int frontend= column/6-1;
#define frontend (col/6-1)
//int rightstart = column/6;
#define rightstart (col/6)
//int rightend = 2*(column/6)-1;
#define rightend (2*(col/6)-1)
//int backstart = 2*(column/6);
#define backstart (2*(col/6))
//int backend = 3*(column/6)-1;
#define backend (3*(col/6)-1)
//int leftstart = 3*(column/6);
#define leftstart (3*(col/6))
//int leftend = 4*(column/6)-1;
#define leftend (4*(col/6)-1)

int  getxlinead(int * board, int col,int y,int x)
{
    return   board[(y * col) + x-1]+
             board[(y * col) + x]+
             board[(y * col) + x+1];
}

int  getylinead(int * board, int col,int y,int x)
{
    return   board[(y * col+1) + x]+
             board[(y * col) + x]+
             board[(y * col-1) + x];
}
void printboard(int * board, int row, int col){

    std::string printSting{};
    for (int i = 0; i < row; ++i){
        for (int j = 0; j < col; ++j) {
            if (j == rightstart || j == leftstart || j == backstart || j == topstart || j == botstart) {
                printSting.append(" ");
            }
            printSting.append(spdlog::fmt_lib::format("{}", board[(i * col)+j]));
        }
        SPDLOG_INFO(printSting);
        printSting = std::string {};
    }
}

int addUpLife(int * board,int col, int i, int j)
{
    return  getxlinead(board, col, i+1,j)+
            getxlinead(board, col, i,j)+
            getxlinead(board, col, i-1,j);
}
void deadorAlive(int * board,int * nextboard, int cols, int value, int y, int x)
{
    if(value <= 2)
        nextboard[y*cols+x]=0;
    else if(x>=3)
        if(value==3)
            nextboard[y*cols+x]=1;
        else
        if(board[y*cols+x]==1)
            nextboard[y*cols+x] =0;
        else
            nextboard[y*cols+x]=1;
}
void addWrapMain(int * board,int * nextboard, int row, int col, int j)
{
    //printf("%d, %d , %d", j-1, j,j+1);
    int x=0;
    if(j==0|| j== row-1)
        return;
    x = board[(j-1)*col+wrapBoxColumn]+
        board[(j-1)*col+start]+
        board[(j-1)*col+start+1]+
        board[(j)*col+wrapBoxColumn]+
        board[(j)*col+start]+
        board[(j)*col+start+1]+
        board[(j+1)*col+wrapBoxColumn]+
        board[(j+1)*col+start]+
        board[(j+1)*col+start+1];
    //printf(" , %d ",x);
    deadorAlive(board, nextboard, col, x, j, start);

    x = board[(j-1)*col+start] +
        board[(j-1)*col+wrapBoxColumn] +
        board[(j-1)*col+wrapBoxColumn - 1] +
        board[(j)*col+start] +
        board[(j)*col+wrapBoxColumn] +
        board[(j)*col+wrapBoxColumn - 1] +
        board[(j+1)*col+start] +
        board[(j+1)*col+wrapBoxColumn] +
        board[(j+1)*col+wrapBoxColumn - 1];
    deadorAlive(board, nextboard, col, x, j, wrapBoxColumn);

}

void mergetop(int * board, int * nextboard, int row, int col)
{
    int x=0;
    //merge front to topf
    for (int i = start+1; i < frontend; ++i) {
        x= getxlinead(board,col,start,topstart+i)+
           getxlinead(board,col,start,i)+
           getxlinead(board,col,start+1,i);

        deadorAlive(board,nextboard, col ,x,start,i);
    }

    for (int i = start+1; i < frontend; ++i) {
        x= getxlinead(board,col,start+1,topstart+i)+
           getxlinead(board,col,start,topstart+i)+
           getxlinead(board,col,start,i);
        //printf("%d  \n",topstart+i);
        deadorAlive(board,nextboard, col,x,wrapBoxRow,i+topstart);
    }

    //merge back
    for (int i = start+1; i <frontend; ++i) {
        x= getxlinead(board,col,wrapBoxRow,topstart+i)+
           getxlinead(board,col,start,backstart+i)+
           getxlinead(board,col,start+1,backstart+i);
        deadorAlive(board,nextboard, col, x,start,i+backstart);
    }

    for (int i = start+1; i < frontend; ++i) {
        x= getxlinead(board,col,wrapBoxRow-1,topstart+i)+
           getxlinead(board,col,wrapBoxRow,topstart+i)+
           getxlinead(board,col,start,backstart+i);

        deadorAlive(board,nextboard, col, x,start,i+topstart);
    }

    //left
    for (int i = start+1; i < row-1; ++i) {
        x=  getylinead(board,col,i,topstart)+
            getxlinead(board,col,start,leftstart+i)+
            getxlinead(board,col,start+1,leftstart+i);

        deadorAlive(board,nextboard, col, x,start,leftstart+i);
    }

    for (int i = start+1; i < row-1; ++i) {
        x=getylinead(board,col,i,topstart+1)+
          getylinead(board,col,i,topstart)+
          getxlinead(board,col,start,leftstart+i);

        deadorAlive(board,nextboard, col, x,i,topstart);
    }

    //right


    for (int i = start+1; i < row-1; ++i) {
        x= board[i-1][topend]+
           board[i][topend]+
           board[i+1][topend]+
           board[start][rightstart+i-1]+
           board[start][rightstart+i]+
           board[start][rightstart+i+1]+
           board[start+1][rightstart+i-1]+
           board[start+1][rightstart+i]+
           board[start+1][rightstart+i+1];
        deadorAlive(board,nextboard, col, x,start,rightstart+i);
    }

    for (int i = start+1; i < row-1; ++i) {
        x= board[i-1][topend-1]+
           board[i][topend-1]+
           board[i+1][topend-1]+
           board[i-1][topend]+
           board[i][topend]+
           board[i+1][topend]+
           board[start][rightstart+i-1]+
           board[start][rightstart+i]+
           board[start][rightstart+i+1];
        deadorAlive(board,nextboard, col, x,i,topend);
    }
}
void mergebot(int board[row][column], int nextboard[row][column])
{
    int x=0;
    //merge back to bot
    for (int i = start+1; i < frontend; ++i) {
        x= board[wrapBoxRow][botstart-1+i]+
           board[wrapBoxRow][botstart+i]+
           board[wrapBoxRow][botstart+1+i]+
           board[wrapBoxRow][backstart+i-1]+
           board[wrapBoxRow][backstart+i]+
           board[wrapBoxRow][backstart+i+1]+
           board[wrapBoxRow-1][backstart+i-1]+
           board[wrapBoxRow-1][backstart+i]+
           board[wrapBoxRow-1][backstart+i+1];
        deadorAlive(board,nextboard,x,wrapBoxRow,backstart+i);
    }

    for (int i = start+1; i < frontend; ++i) {
        x= board[start+1][botstart-1+i]+
           board[start+1][botstart+i]+
           board[start+1][botstart+1+i]+
           board[start][botstart-1+i]+
           board[start][botstart+i]+
           board[start][botstart+1+i]+
           board[wrapBoxRow][i-1]+
           board[wrapBoxRow][i]+
           board[wrapBoxRow][i+1];
        deadorAlive(board,nextboard,x,wrapBoxRow,i+botstart);
    }

    //merge front bot
    for (int i = start+1; i <frontend; ++i) {
        x= board[wrapBoxRow+1][botstart+i-1]+
           board[wrapBoxRow+1][botstart+i]+
           board[wrapBoxRow+1][botstart+i+1]+
           board[wrapBoxRow][i-1]+
           board[wrapBoxRow][i]+
           board[wrapBoxRow][i+1]+
           board[start+1][i-1]+
           board[start+1][i]+
           board[start+1][i+1];
        deadorAlive(board,nextboard,x,wrapBoxRow,i);
    }

    for (int i = start+1; i < frontend; ++i) {
        x= board[start+1][botstart-1+i]+
           board[start+1][botstart+i]+
           board[start+1][botstart+1+i]+
           board[start][botstart+i-1]+
           board[start][botstart+i]+
           board[start][botstart+i+1]+
           board[wrapBoxRow][i-1]+
           board[wrapBoxRow][i]+
           board[wrapBoxRow][i+1];
        deadorAlive(board,nextboard,x,start,i+botstart);
    }

    //left
    for (int i = start+1; i < row-1; ++i) {
        x= board[i-1][botstart]+
           board[i][botstart]+
           board[i+1][botstart]+
           board[wrapBoxRow][leftstart+i-1]+
           board[wrapBoxRow][leftstart+i]+
           board[wrapBoxRow][leftstart+i+1]+
           board[wrapBoxRow-1][leftstart+i-1]+
           board[wrapBoxRow-1][leftstart+i]+
           board[wrapBoxRow-1][leftstart+i+1];
        deadorAlive(board,nextboard,x,wrapBoxRow,leftstart+i);
    }

    for (int i = start+1; i < row; ++i) {
        x= board[i+1][botstart+1]+
           board[i][botstart+1]+
           board[i-1][botstart+1]+
           board[i+1][botstart]+
           board[i][botstart]+
           board[i-1][botstart]+
           board[wrapBoxRow][leftstart+i-1]+
           board[wrapBoxRow][leftstart+i]+
           board[wrapBoxRow][leftstart+i+1];
        deadorAlive(board,nextboard,x,i,botstart);
    }

    //right

    for (int i = start+1; i < row-1; ++i) {

        x= board[(i-1)][botend]+
           board[(i)][botend]+
           board[(i+1)][botend]+
           board[wrapBoxRow][rightstart+i-1]+
           board[wrapBoxRow][rightstart+i]+
           board[wrapBoxRow][rightstart+i+1]+
           board[wrapBoxRow-1][rightstart+i-1]+
           board[wrapBoxRow-1][rightstart+i]+
           board[wrapBoxRow-1][rightstart+i+1];
        deadorAlive(board,nextboard,x,wrapBoxRow,rightstart+i);
    }

    for (int i = start+1; i < row-1; ++i) {

        x= board[i-1][botend-1]+
           board[i][botend-1]+
           board[i+1][botend-1]+
           board[i-1][botend]+
           board[i][botend]+
           board[i+1][botend]+
           board[wrapBoxRow][rightstart+i-1]+
           board[wrapBoxRow][rightstart+i]+
           board[wrapBoxRow][rightstart+i+1];
        deadorAlive(board,nextboard,x,i,botend);
        //printf("i is %d, i+1 %d, i-1 %d \n", i, i+1, i-1);
    }
    //printboard(nextboard);
}



void mergecorners(int board[row][column],int nextboard[row][column])
{
    int x =0;

    // left top corner front with warp

    x =     board[start][topstart]+
            board[start][topstart+1]+

            board[start][leftend]+
            board[start][start]+
            board[start][start+1]+

            board[start+1][leftend]+
            board[start+1][start]+
            board[start+1][start+1];
    deadorAlive(board,nextboard,x,start,start);

    // right top corner of left with warp
    x =     board[wrapBoxRow-1][topstart]+
            board[wrapBoxRow][topstart]+

            board[start][leftend-1]+
            board[start][leftend]+
            board[start][start]+

            board[start+1][leftend-1]+
            board[start+1][leftend]+
            board[start+1][start+1];

    // left bot corner of front with warp

    deadorAlive(board,nextboard,x,start,leftend);

    x =     board[wrapBoxRow-1][leftend]+
            board[wrapBoxRow-1][topstart]+
            board[wrapBoxRow-1][topstart+1]+

            board[wrapBoxRow][leftend]+
            board[wrapBoxRow][start]+
            board[wrapBoxRow][start+1]+

            board[start][botstart]+
            board[start][botstart+1];

    deadorAlive(board,nextboard,x,wrapBoxRow,start);

    // right bot corner of left with warp
    x =     board[wrapBoxRow-1][leftend-1]+
            board[wrapBoxRow-1][leftend]+
            board[wrapBoxRow-1][start]+

            board[wrapBoxRow][leftend-1]+
            board[wrapBoxRow][leftend]+
            board[wrapBoxRow][start]+

            board[start][botstart]+
            board[start+1][botstart];

    deadorAlive(board,nextboard,x,wrapBoxRow,leftend);


    // left top corners with no warps main
    // left corner right
    x =     board[start][topend]+
            board[start+1][topend]+

            board[start][rightstart-1]+
            board[start][rightstart]+
            board[start][rightstart+1]+

            board[start+1][rightstart-1]+
            board[start+1][rightstart]+
            board[start+1][rightstart+1];

    deadorAlive(board,nextboard,x,start,rightstart);
    //printboard(nextboard);

    // left corner back
    x =     board[wrapBoxRow][topstart]+
            board[wrapBoxRow][topstart+1]+

            board[start][backstart-1]+
            board[start][backstart]+
            board[start][backstart+1]+

            board[start+1][backstart-1]+
            board[start+1][backstart]+
            board[start+1][backstart+1];

    deadorAlive(board,nextboard,x,start,backstart);
    //printboard(nextboard);

    // left corner left
    x =     board[start][topstart]+
            board[start+1][topstart]+

            board[start][leftstart-1]+
            board[start][leftstart]+
            board[start][leftstart+1]+

            board[start+1][leftstart-1]+
            board[start+1][leftstart]+
            board[start+1][leftstart+1];

    deadorAlive(board,nextboard,x,start,leftstart);
    //printboard(nextboard);

    // right corner front
    x =     board[start][topend]+
            board[start][topend-1]+

            board[start][frontend-1]+
            board[start][frontend]+
            board[start][frontend+1]+

            board[start+1][frontend-1]+
            board[start+1][frontend]+
            board[start+1][frontend+1];

    deadorAlive(board,nextboard,x,start,frontend);

    // right corner front
    x =     board[start][topend]+
            board[start][topend-1]+

            getxlinead(board,start,frontend)+
            /*           board[start][frontend-1]+
                       board[start][frontend]+
                       board[start][frontend+1]+*/
            getxlinead(board,start+1,frontend);
    /*  board[start+1][frontend-1]+
      board[start+1][frontend]+
      board[start+1][frontend+1];
*/
    deadorAlive(board,nextboard,x,start,frontend);

    // right corner right
    x =     board[wrapBoxRow][topend]+
            board[wrapBoxRow-1][topend]+
            getxlinead(board,start,rightend)+
            getxlinead(board,start+1,rightend);

    deadorAlive(board,nextboard,x,start,rightend);

    // right corner back
    x =     board[wrapBoxRow][topend]+
            board[wrapBoxRow][topend-1]+
            getxlinead(board,start,backend)+
            getxlinead(board,start+1,backend);

    deadorAlive(board,nextboard,x,start,backend);


    // left bot corners with no warps main
    // right corner front
    x =     board[wrapBoxRow][botend]+
            board[wrapBoxRow][botend-1]+
            getxlinead(board,wrapBoxRow,frontend)+
            getxlinead(board,wrapBoxRow-1,frontend);

    deadorAlive(board,nextboard,x,wrapBoxRow,frontend);

    // right corner right
    x =     board[start][botend]+
            board[start+1][botend]+
            getxlinead(board,wrapBoxRow,rightend)+
            getxlinead(board,wrapBoxRow-1,rightend);

    deadorAlive(board,nextboard,x,wrapBoxRow,rightend);

    // right corner back
    x =     board[start][botstart]+
            board[start][botstart+1]+
            getxlinead(board,wrapBoxRow,backend)+
            getxlinead(board,wrapBoxRow-1,backend);

    deadorAlive(board,nextboard,x,wrapBoxRow,backend);

    // bot left corners of main
    // left corner right
    x =     board[start][botstart]+
            board[start+1][botstart]+
            getxlinead(board,wrapBoxRow,rightstart)+
            getxlinead(board,wrapBoxRow-1,rightstart);
    deadorAlive(board,nextboard,x,wrapBoxRow,rightstart);

    //printboard(nextboard);

    // bot left corners of main
    // left corner back
    x =     board[start][botend]+
            board[start][botend-1]+
            getxlinead(board,wrapBoxRow,backstart)+
            getxlinead(board,wrapBoxRow-1,backstart);
    deadorAlive(board,nextboard,x,wrapBoxRow,backstart);

    //printboard(nextboard);
    // bot left corners of main
    // left corner left
    x =     board[start][botstart]+
            board[start+1][botstart]+
            getxlinead(board,wrapBoxRow,leftstart)+
            getxlinead(board,wrapBoxRow-1,backstart);
    deadorAlive(board,nextboard,x,wrapBoxRow,leftstart);

    //printboard(nextboard);

    //topmerge corner
    //top front
    x =     board[start][start]+
            board[start][start+1]+

            board[start][leftstart]+
            board[start][topstart]+
            board[start][topstart+1]+

            board[start+1][leftstart]+
            board[start+1][topstart]+
            board[start+1][topstart+1];

    deadorAlive(board,nextboard,x,start,topstart);

    //top right
    x =     board[start][frontend-1]+
            board[start][frontend]+

            board[start][topend-1]+
            board[start][topend]+
            board[start][rightstart]+

            board[start+1][topend-1]+
            board[start+1][topend]+
            board[start+1][rightstart];

    deadorAlive(board,nextboard,x,start,topend);

    //top left
    x =     board[start][leftstart+1]+
            board[wrapBoxRow-1][topstart]+
            board[wrapBoxRow-1][topstart+1]+

            board[start][leftstart]+
            board[wrapBoxRow][topstart]+
            board[wrapBoxRow][topstart+1]+

            board[start][backstart]+
            board[start][backstart+1];


    //printf("%d", frontstart);
    deadorAlive(board,nextboard,x,wrapBoxRow,topstart);

    x =     board[start][rightend-1]+
            board[wrapBoxRow-1][topend]+
            board[wrapBoxRow-1][topend-1]+

            board[start][rightend]+
            board[wrapBoxRow][topend]+
            board[wrapBoxRow][topend-1]+

            board[start][backstart]+
            board[start][backstart+1];


    //printf("%d", frontstart);
    deadorAlive(board,nextboard,x,wrapBoxRow,topend);


    //botmerge corner
    //bot front
    x =     board[wrapBoxRow][start]+
            board[wrapBoxRow][start+1]+

            board[start][leftstart]+
            board[start][botstart]+
            board[start][botstart+1]+

            board[wrapBoxRow+1][leftstart]+
            board[start+1][botstart]+
            board[start+1][botstart+1];

    deadorAlive(board,nextboard,x,start,botstart);

    //bot right
    x =     board[wrapBoxRow][frontend-1]+
            board[wrapBoxRow][frontend]+

            board[start][botend-1]+
            board[start][botend]+
            board[wrapBoxRow][rightstart]+

            board[start+1][botend-1]+
            board[start+1][botend]+
            board[wrapBoxRow-1][rightstart];

    deadorAlive(board,nextboard,x,start,botend);

    //top left
    x =     board[wrapBoxRow][leftstart+1]+
            board[wrapBoxRow-1][botstart]+
            board[wrapBoxRow-1][botstart+1]+

            board[wrapBoxRow][leftstart]+
            board[wrapBoxRow][botstart]+
            board[wrapBoxRow][botstart+1]+

            board[wrapBoxRow][backstart]+
            board[wrapBoxRow][backstart+1];


    //printf("%d", frontstart);
    deadorAlive(board,nextboard,x,wrapBoxRow,botstart);

    x =     board[wrapBoxRow][rightend-1]+
            board[wrapBoxRow-1][botend]+
            board[wrapBoxRow-1][botend-1]+

            board[wrapBoxRow][rightend]+
            board[wrapBoxRow][botend]+
            board[wrapBoxRow][botend-1]+

            board[wrapBoxRow][backstart]+
            board[wrapBoxRow][backstart+1];


    //printf("%d", frontstart);
    deadorAlive(board,nextboard,x,wrapBoxRow,botend);
}
//int board[row][column]
//nextboard[row][column]
void runlife(int * board, int * nextboard, int inbound_row, int inbound_col)
{

    int row = inbound_row;
    int column = inbound_col;

    int x = 0;
    //body
    for (int i = start; i < row ; ++i) {
        for (int j = start; j < wrapBoxColumn ; ++j) {
            if(i==0){
                mergetop(board,nextboard);
                x = addUpLife(board, i, j);
            }
            else if(i==row-1)
            {
                mergebot(board, nextboard);
            }
            else if(j==start||j==wrapBoxColumn){
                addWrapMain(board,nextboard,i);
                //x = addUpLife(board, i, j);
            }
            else {
                x = addUpLife(board, i, j);
                deadorAlive(board, nextboard, x, i, j);
            }
        }
    }
/*    for(int j = start; j < row - 1; j++)
    {
        addWrapMain(board,nextboard,j);
    }*/


    for (int i = start+1; i < row-1; ++i) {
        for (int j =topstart+1; j <=topend-1 ; ++j) {
            x = addUpLife(board,i,j);
            deadorAlive(board,nextboard,x,i,j);
        }
    }



    for (int i = start+1; i < row-1; ++i) {
        for (int j = botstart+1; j <= botend-1; ++j) {
            x= addUpLife(board,i,j);
            deadorAlive(board,nextboard,x,i,j);
        }
    }
    mergetop(board,nextboard);

    //printboard(nextboard);
    mergecorners(board,nextboard);
    /*printf("\n");
    printboard(nextboard);
*/
}
//int main()
//{
//    printf("Hello, World!\n");
//
//    int board[row][column];
//    int pboard[row][column];
//
//    printf("Setup\n");
//
//    for (int i = 0; i < row; ++i) {
//        for (int j = 0; j < column; ++j) {
//            board[i][j] = 0;
//            pboard[i][j] = 0;
////            printf(" %d ",board[i][j]);
//        }
//        //printf("\n");
//    }
//    //printboard(board);
//
////    board[5][5] = 1;
////    board[4][5] = 1;
////    board[3][5] = 1;
//    for(int i = start; i <= wrapBoxColumn;i++)
//    {
//        board[start][i] = 1;
//        board[wrapBoxRow][i]=1;
//    }
//
//    for(int i = topstart; i <= topend;i++)
//    {
//        //board[5][i] = 1;
//    }
//
//    for(int i = botstart; i <= botend;i++)
//    {
//        //board[5][i] = 1;
//    }
//
//    printf("\n\nState 0:\n");
//    printboard(board);
//
//    runlife(board, pboard);
//    printf("\n\nState 1:\n");
//    printboard(pboard);
//
//    runlife(pboard, board);
//    printf("\n\nState 2:\n");
//    printboard(board);
//
//    return 0;
//
//}

