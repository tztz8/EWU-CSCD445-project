#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include <string>
#include "GameOfLifeLogic.h"
#include "GameOfLifeCube.h"

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



int  getxlinead(int * board, int col, int y, int x) {
    if(x >= DEBUG_GLOBAL_COLS || y >= DEBUG_GLOBAL_ROWS) {
        SPDLOG_ERROR(spdlog::fmt_lib::format("OUT OF BOUNDS: x = {}, y = {}", x, y));
        return 0;
    }
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
void printboard(int * board, int row, int col) {

    std::string printSting{};
    for (int i = 0; i < row; ++i){
        for (int j = 0; j < col; ++j) {
            if (j == rightstart || j == leftstart || j == backstart || j == topstart || j == botstart) {
                printSting.append(" ");
            }
            printSting.append(spdlog::fmt_lib::format("{},", board[(i * col)+j]));
        }
        SPDLOG_INFO(printSting); // \n (newline)
        printSting = std::string {};
    }
}

std::string stringBoard(int * board, int row, int col) {
    std::string printSting{};
    for (int i = 0; i < row; ++i){
        for (int j = 0; j < col; ++j) {
            if (j == rightstart || j == leftstart || j == backstart || j == topstart || j == botstart) {
                printSting.append(" ");
            }
            printSting.append(spdlog::fmt_lib::format("{},", board[(i * col)+j]));
        }
        printSting.append("\n");
    }
    return printSting;
}

int addUpLife(int * board,int col, int j, int i)
{
    return  getxlinead(board, col, j+1,i)+
            getxlinead(board, col, j,i)+
            getxlinead(board, col, j-1,i);
}
void deadorAlive(int * board,int * nextboard, int col, int value, int y, int x)
{

    //y == j , x == start
    if(board[y*col+x]==1)
    {
        if(value-1==2||value-1==3)
        {
            nextboard[y*col+x]=1;
        }
        else
        {
            nextboard[y*col+x]=0;
        }
    }
    else
    {
        if(value==3)
        {
            nextboard[y*col+x]=1;
        }
        else
            nextboard[y*col+x]=0;
    }

/*    if(value <= 1)
        nextboard[y*col+x]=0;
    else if(x>=3)
        if(value==3)
            nextboard[y*col+x]=1;
        else
        if(board[y*col+x]==1)
            nextboard[y*col+x] =0;
        else
            nextboard[y*col+x]=1;*/
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
    //merge top front to front top
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
        // was wrapBoxRow
        deadorAlive(board,nextboard, col,x,start,i+topstart);
    }

    //merge front bot to back top
    for (int i = start+1; i <frontend; ++i) {
        x= getxlinead(board,col,wrapBoxRow,6+i)+
           getxlinead(board,col,start,backstart+i)+
           getxlinead(board,col,start+1,backstart+i);
        deadorAlive(board,nextboard, col, x,start,i+backstart);
    }

    for (int i = start+1; i < frontend; ++i) {
        x= getxlinead(board,col,wrapBoxRow-1,topstart+i)+
           getxlinead(board,col,wrapBoxRow,topstart+i)+
           getxlinead(board,col,start,backstart+i);

        deadorAlive(board,nextboard, col, x,wrapBoxRow,i+topstart);
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
        x=      getxlinead(board,col,wrapBoxRow-1,botstart+i)+
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

    for (int i = start+1; i < row-1; ++i) {
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




//int board[row][column]
//nextboard[row][column]
void runlife(int * board, int * nextboard, int inbound_row, int inbound_col)
{

    int row = inbound_row;
    int column = inbound_col;
    int col = inbound_col;

    int x = 0;
    //body
    for (int i = start; i < row ; ++i) {
        for (int j = start; j < wrapBoxColumn ; ++j) {
            if(i==0){
                mergetop(board,nextboard, row, column);
                // x = addUpLife(board, i, j,column);
            }
            else if(i==row-1)
            {
                mergebot(board, nextboard, row,column);
            }
            else if(j==start||j==wrapBoxColumn){
                addWrapMain(board,nextboard, row,column,i);
                //x = addUpLife(board, i, j);
            }
            else {
                x = addUpLife(board, col, i, j);
                deadorAlive(board, nextboard,column, x, i, j);
            }
        }
    }
/*    for(int j = start; j < row - 1; j++)
    {
        addWrapMain(board,nextboard,j);
    }*/


    for (int i = start+1; i < row-1; ++i) {
        for (int j =topstart+1; j <=topend-1 ; ++j) {
            x = addUpLife(board,column,i,j);
            deadorAlive(board,nextboard,column,x,i,j);
        }
    }



    for (int i = start+1; i < row-1; ++i) {
        for (int j = botstart+1; j <= botend-1; ++j) {
            x= addUpLife(board,column,i,j);
            deadorAlive(board,nextboard,col,x,i,j);
        }
    }
    mergetop(board,nextboard, row,column);

    //printboard(nextboard);
    //mergecorners(board,nextboard, row,column);
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

