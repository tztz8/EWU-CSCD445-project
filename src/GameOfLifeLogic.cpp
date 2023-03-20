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
        x= getylinead(board,col,i,topend)+
           getxlinead(board,col,start,rightstart+1)+
           getxlinead(board,col,start+1,rightstart+i);

        deadorAlive(board,nextboard, col, x,start,rightstart+i);
    }

    for (int i = start+1; i < row-1; ++i) {
        x= getylinead(board,col,i,topend-1)+
           getylinead(board,col,i,topend)+
           getxlinead(board,col,start,rightstart+i);

        deadorAlive(board,nextboard, col, x,i,topend);
    }
}
void mergebot(int * board, int * nextboard,int row, int col)
{
    int x=0;
    //merge back to bot
    for (int i = start+1; i < frontend; ++i) {
        x=
                getxlinead(board,col,wrapBoxRow,botstart+i)+
                getxlinead(board,col,wrapBoxRow,backstart+i)+
                getxlinead(board,col,wrapBoxRow-1,backstart+i);

        deadorAlive(board,nextboard,col,x,wrapBoxRow,backstart+i);
    }
    ///possible problem here
    for (int i = start+1; i < frontend; ++i) {
        x=      getxlinead(board,col,wrapBoxRow+1,botstart+i)+
                getxlinead(board,col,wrapBoxRow,botstart+i)+
                getxlinead(board,col,wrapBoxRow,backstart+i);

        deadorAlive(board,nextboard,col,x,wrapBoxRow,i+botstart);
    }

    //merge front bot reworked.
    for (int i = start+1; i <frontend; ++i) {
        x=      getxlinead(board,col, start ,botstart+i)+
                getxlinead(board,col,wrapBoxRow, i)+
                getxlinead(board,col,wrapBoxRow-1,i);

        deadorAlive(board,nextboard,col,x,wrapBoxRow,i);
    }

    for (int i = start+1; i < frontend; ++i) {
        x=      getxlinead(board,col,start+1,botstart+i)+
                getxlinead(board,col,start, botstart+i)+
                getxlinead(board,col,wrapBoxRow,i);

        deadorAlive(board,nextboard,col,x,start,i+botstart);
    }

    //left
    for (int i = start+1; i < row-1; ++i) {
        x= getylinead(board,col,i,botstart)+
           getxlinead(board,col,wrapBoxRow, leftstart+i)+
           getxlinead(board,col,wrapBoxRow-1,leftstart+i);

        deadorAlive(board,nextboard,col,x,wrapBoxRow,leftstart+i);
    }

    for (int i = start+1; i < row; ++i) {
        x=  getylinead(board,col,i,botstart+1)+
            getylinead(board,col,i,botstart)+
            getxlinead(board,col,wrapBoxRow,leftstart+i);

        deadorAlive(board,nextboard,col,x,i,botstart);
    }

    //right

    for (int i = start+1; i < row-1; ++i) {

        x= getylinead(board,col,i,botend)+
           getxlinead(board,col,wrapBoxRow,rightstart+i)+
           getxlinead(board,col,wrapBoxRow-1,rightstart+i);

        deadorAlive(board,nextboard,col,x,wrapBoxRow,rightstart+i);
    }

    for (int i = start+1; i < row-1; ++i) {

        x=  getylinead(board,col,i,botend-1)+
            getylinead(board,col,i,botend)+
            getxlinead(board,col,wrapBoxRow,rightstart+i);

        deadorAlive(board,nextboard,col,x,i,botend);
        //printf("i is %d, i+1 %d, i-1 %d \n", i, i+1, i-1);
    }
    //printboard(nextboard);
}



void mergecorners(int * board,int * nextboard,int col)
{
    int x =0;

    // left top corner front with warp

    x =     board[topstart]+
            board[topstart+1]+

            board[leftend]+
            board[start]+
            board[start+1]+

            board[(start+1)*cols+leftend]+
            board[(start+1)*cols+start]+
            board[(start+1)*cols+start+1];
    deadorAlive(board,nextboard,col,x,start,start);

    // right top corner of left with warp
    x =     board[cols*(wrapBoxRow-1)+topstart]+
            board[cols*(wrapBoxRow)+topstart]+

            board[start*cols+leftend-1]+
            board[start*cols+leftend]+
            board[start*cols+start]+

            board[(start+1)*cols+leftend-1]+
            board[(start+1)*cols+leftend]+
            board[(start+1*)cols+start+1];

    // left bot corner of front with warp

    deadorAlive(board,nextboard,col,x,start,leftend);

    x =     board[(wrapBoxRow-1)*cols+leftend]+
            board[(wrapBoxRow-1)*cols+topstart]+
            board[(wrapBoxRow-1)*cols+topstart+1]+

            board[wrapBoxRow*cols+leftend]+
            board[wrapBoxRow*cols+start]+
            board[wrapBoxRow*cols+start+1]+

            board[start*cols+botstart]+
            board[start*cols+botstart+1];

    deadorAlive(board,nextboard,col,x,wrapBoxRow,start);

    // right bot corner of left with warp
    x =     board[(wrapBoxRow-1)*cols+leftend-1]+
            board[(wrapBoxRow-1)*cols+leftend]+
            board[(wrapBoxRow-1)*cols+start]+

            board[(wrapBoxRow)*cols+leftend-1]+
            board[(wrapBoxRow)*cols+leftend]+
            board[(wrapBoxRow)*cols+start]+

            board[start*cols+botstart]+
            board[(start+1)*cols+botstart];

    deadorAlive(board,nextboard,col,x,wrapBoxRow,leftend);


    // left top corners with no warps main
    // left corner right
    x =     board[start*cols+topend]+
            board[(start+1)*cols+topend]+

            board[start*cols+rightstart-1]+
            board[start*cols+rightstart]+
            board[start*cols+rightstart+1]+

            board[(start+1)*cols+rightstart-1]+
            board[(start+1)*cols+rightstart]+
            board[(start+1)*cols+rightstart+1];

    deadorAlive(board,nextboard,col,x,start,rightstart);
    //printboard(nextboard);

    // left corner back
    x =     board[wrapBoxRow*cols+topstart]+
            board[wrapBoxRow*cols+topstart+1]+

            board[start*cols+backstart-1]+
            board[start*cols+backstart]+
            board[start*cols+backstart+1]+

            board[(start+1)*cols+backstart-1]+
            board[(start+1)*cols+backstart]+
            board[(start+1)*cols+backstart+1];

    deadorAlive(board,nextboard,col,x,start,backstart);
    //printboard(nextboard);

    // left corner left
    x =     board[start*cols+topstart]+
            board[(start+1)*cols+topstart]+

            board[start*cols+leftstart-1]+
            board[start*cols+leftstart]+
            board[start*cols+leftstart+1]+

            board[(start+1)*cols+leftstart-1]+
            board[(start+1)*cols+leftstart]+
            board[(start+1)*cols+leftstart+1];

    deadorAlive(board,nextboard,col,x,start,leftstart);
    //printboard(nextboard);

    // right corner front
    x =     board[start*cols+topend]+
            board[start*cols+topend-1]+

            board[start*cols+frontend-1]+
            board[start*cols+frontend]+
            board[start*cols+frontend+1]+

            board[(start+1)*cols+frontend-1]+
            board[(start+1)*cols+frontend]+
            board[(start+1)*cols+frontend+1];

    deadorAlive(board,nextboard,col,x,start,frontend);

    // right corner front
    x =     board[start*cols+topend]+
            board[start*cols+topend-1]+

            getxlinead(board,col,start,frontend)+
            /*           board[start][frontend-1]+
                       board[start][frontend]+
                       board[start][frontend+1]+*/
            getxlinead(board,col,start+1,frontend);
    /*  board[start+1][frontend-1]+
      board[start+1][frontend]+
      board[start+1][frontend+1];
*/
    deadorAlive(board,nextboard,col,x,start,frontend);

    // right corner right
    x =     board[wrapBoxRow*cols+topend]+
            board[(wrapBoxRow-1)*cols+topend]+
            getxlinead(board,start,rightend)+
            getxlinead(board,start+1,rightend);

    deadorAlive(board,nextboard,col,x,start,rightend);

    // right corner back
    x =     board[wrapBoxRow*cols+topend]+
            board[wrapBoxRow*cols+topend-1]+
            getxlinead(board,start,backend)+
            getxlinead(board,start+1,backend);

    deadorAlive(board,nextboard,col,x,start,backend);


    // left bot corners with no warps main
    // right corner front
    x =     board[wrapBoxRow*cols+botend]+
            board[wrapBoxRow*cols+botend-1]+
            getxlinead(board,wrapBoxRow,frontend)+
            getxlinead(board,wrapBoxRow-1,frontend);

    deadorAlive(board,nextboard,col,x,wrapBoxRow,frontend);

    // right corner right
    x =     board[start][botend]+
            board[start+1][botend]+
            getxlinead(board,wrapBoxRow,rightend)+
            getxlinead(board,wrapBoxRow-1,rightend);

    deadorAlive(board,nextboard,col,x,wrapBoxRow,rightend);

    // right corner back
    x =     board[start*cols+botstart]+
            board[start*cols+botstart+1]+
            getxlinead(board,wrapBoxRow,backend)+
            getxlinead(board,wrapBoxRow-1,backend);

    deadorAlive(board,nextboard,col,x,wrapBoxRow,backend);

    // bot left corners of main
    // left corner right
    x =     board[start*cols+botstart]+
            board[(start+1)*cols+botstart]+
            getxlinead(board,wrapBoxRow,rightstart)+
            getxlinead(board,wrapBoxRow-1,rightstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,rightstart);

    //printboard(nextboard);

    // bot left corners of main
    // left corner back
    x =     board[start*cols+botend]+
            board[start*cols+botend-1]+
            getxlinead(board,wrapBoxRow,backstart)+
            getxlinead(board,wrapBoxRow-1,backstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,backstart);

    //printboard(nextboard);
    // bot left corners of main
    // left corner left
    x =     board[start*cols+botstart]+
            board[(start+1)*cols+botstart]+
            getxlinead(board,wrapBoxRow,leftstart)+
            getxlinead(board,wrapBoxRow-1,backstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,leftstart);

    //printboard(nextboard);

    //topmerge corner
    //top front
    x =     board[start*cols+start]+
            board[start*cols+start+1]+

            board[start*cols+leftstart]+
            board[start*cols+topstart]+
            board[start*cols+topstart+1]+

            board[(start+1)*cols+leftstart]+
            board[(start+1)*cols+topstart]+
            board[(start+1)*cols+topstart+1];

    deadorAlive(board,nextboard,col,x,start,topstart);

    //top right
    x =     board[start*cols+frontend-1]+
            board[start*cols+frontend]+

            board[start*cols+topend-1]+
            board[start*cols+topend]+
            board[start*cols+rightstart]+

            board[(start+1)*cols+topend-1]+
            board[(start+1)*cols+topend]+
            board[(start+1)*cols+rightstart];

    deadorAlive(board,nextboard,col,x,start,topend);

    //top left
    x =     board[start*cols+leftstart+1]+
            board[(wrapBoxRow-1)*cols+topstart]+
            board[(wrapBoxRow-1)*cols+topstart+1]+

            board[start*cols+leftstart]+
            board[wrapBoxRow*cols+topstart]+
            board[wrapBoxRow*cols+topstart+1]+

            board[start*cols+backstart]+
            board[start*cols+backstart+1];


    //printf("%d", frontstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,topstart);

    x =     board[start*cols+rightend-1]+
            board[(wrapBoxRow-1)*cols+topend]+
            board[(wrapBoxRow-1)*cols+topend-1]+

            board[start*cols+rightend]+
            board[wrapBoxRow*cols+topend]+
            board[wrapBoxRow*cols+topend-1]+

            board[start*cols+backstart]+
            board[start*cols+backstart+1];


    //printf("%d", frontstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,topend);


    //botmerge corner
    //bot front
    x =     board[wrapBoxRow*cols+start]+
            board[wrapBoxRow*cols+start+1]+

            board[start*cols+leftstart]+
            board[start*cols+botstart]+
            board[start*cols+botstart+1]+

            board[(wrapBoxRow+1)][leftstart]+
            board[(start+1)*cols+botstart]+
            board[(start+1)*cols+botstart+1];

    deadorAlive(board,nextboard,col,x,start,botstart);

    //bot right
    x =     board[wrapBoxRow*cols+frontend-1]+
            board[wrapBoxRow*cols+frontend]+

            board[start*cols+botend-1]+
            board[start*cols+botend]+
            board[wrapBoxRow*cols+rightstart]+

            board[(start+1)*cols+botend-1]+
            board[(start+1)*cols+botend]+
            board[(wrapBoxRow-1)*cols+rightstart];

    deadorAlive(board,nextboard,col,x,start,botend);

    //top left
    x =     board[wrapBoxRow*cols+leftstart+1]+
            board[(wrapBoxRow-1)*cols+botstart]+
            board[(wrapBoxRow-1)*cols+botstart+1]+

            board[wrapBoxRow*cols+leftstart]+
            board[wrapBoxRow*cols+botstart]+
            board[wrapBoxRow*cols+botstart+1]+

            board[wrapBoxRow*cols+backstart]+
            board[wrapBoxRow*cols+backstart+1];


    //printf("%d", frontstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,botstart);

    x =     board[wrapBoxRow*cols+rightend-1]+
            board[(wrapBoxRow-1)*cols+botend]+
            board[(wrapBoxRow-1)*cols+botend-1]+

            board[wrapBoxRow*cols+rightend]+
            board[wrapBoxRow*cols+botend]+
            board[wrapBoxRow*cols+botend-1]+

            board[wrapBoxRow*cols+backstart]+
            board[wrapBoxRow*cols+backstart+1];


    //printf("%d", frontstart);
    deadorAlive(board,nextboard,col,x,wrapBoxRow,botend);
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
                deadorAlive(board, nextboard,col, x, i, j);
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
            deadorAlive(board,nextboard,col,x,i,j);
        }
    }



    for (int i = start+1; i < row-1; ++i) {
        for (int j = botstart+1; j <= botend-1; ++j) {
            x= addUpLife(board,i,j);
            deadorAlive(board,nextboard,col,x,i,j);
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

