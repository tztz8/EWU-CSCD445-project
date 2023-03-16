#include <stdio.h>
#include <math.h>


int const row = 6;
int const column = 6*6;
int const start = 0;
int wrapBoxRow = row - 1;
int wrapBoxColumn = 2*(column/3) - 1;
int topstart = (column/3)*2;
int topend = (column/6)*5-1;
int botstart = (column/6)*5;
int botend = column-1;
int frontstart = start;
int frontend= column/6-1;
int rightstart = column/6;
int rightend = 2*(column/6)-1;
int backstart = 2*(column/6);
int backend = 3*(column/6)-1;
int leftstart = 3*(column/6);
int leftend = 4*(column/6)-1;

void printboard(int  board[row][column]){

    printf("\n");
    for (int i = start; i < row; ++i){
        for (int j = start; j < column; ++j) {
            if(j==rightstart || j==leftstart || j==backstart || j==topstart || j==botstart)
                printf(" ");

            printf("%d", board[i][j]);}
        printf("\n");}}

int addUpLife(int board[row][column], int i, int j)
{
    return  board[i+1][j-1]+
            board[i+1][j]+
            board[i+1][j+1]+
            board[i][j-1]+
            board[i][j]+
            board[i][j+1]+
            board[i-1][j-1]+
            board[i-1][j]+
            board[i-1][j+1];
}
void deadorAlive(int board[row][column],int nextboard[row][column], int x, int i, int j)
{
    if(x <= 2)
        nextboard[i][j]=0;
    else
        if(board[i][j]==1)
        {
            if(x>=3 && x<=4)
                nextboard[i][j] =1;
            else
                nextboard[i][j] =0;
        }
        else
        {
            if(board[i][j]==0&&x==3)
                nextboard[i][j] =1;
            else
                nextboard[i][j] =0;
        }
}
void addWrapMain(int board[row][column],int nextboard[row][column],int j)
{
    printf("%d, %d , %d", j-1, j,j+1);
    int x=0;
    if(j==0|| j== row-1)
        return;
     x = board[j-1][wrapBoxColumn]+
            board[j-1][start]+
            board[j-1][start+1]+
            board[j][wrapBoxColumn]+
            board[j][start]+
            board[j][start+1]+
            board[j+1][wrapBoxColumn]+
            board[j+1][start]+
            board[j+1][start+1];
    //printf(" , %d ",x);
        deadorAlive(board, nextboard, x, j, start);

        x = board[j - 1][start] +
            board[j - 1][wrapBoxColumn] +
            board[j - 1][wrapBoxColumn - 1] +
            board[j][start] +
            board[j][wrapBoxColumn] +
            board[j][wrapBoxColumn - 1] +
            board[j + 1][start] +
            board[j + 1][wrapBoxColumn] +
            board[j + 1][wrapBoxColumn - 1];
        deadorAlive(board, nextboard, x, j, wrapBoxColumn);

}

void mergetop(int board[row][column], int nextboard[row][column])
{
    int x=0;
    //merge front to topf
    for (int i = start; i <= frontend; ++i) {
        x= board[start][topstart-1+i]+
           board[start][topstart+i]+
           board[start][topstart+1+i]+
           board[start][i-1]+
           board[start][i]+
           board[start][i+1]+
           board[start+1][i-1]+
           board[start+1][i]+
           board[start+1][i+1];
        deadorAlive(board,nextboard,x,start,i);
    }

    for (int i = start; i <= frontend; ++i) {
        x= board[start+1][topstart-1+i]+
           board[start+1][topstart+i]+
           board[start+1][topstart+1+i]+
           board[start][topstart-1+i]+
           board[start][topstart+i]+
           board[start][topstart+1+i]+
           board[start][i-1]+
           board[start][i]+
           board[start][i+1];
        deadorAlive(board,nextboard,x,wrapBoxRow,i+topstart);
    }

    //merge back
    for (int i = start; i <=frontend; ++i) {
        x= board[wrapBoxRow][topstart-1+i]+
           board[wrapBoxRow][topstart+i]+
           board[wrapBoxRow][topstart+1+i]+
           board[start][backstart+i-1]+
           board[start][backstart+i]+
           board[start][backstart+i+1]+
           board[start+1][backstart+i-1]+
           board[start+1][backstart+i]+
           board[start+1][backstart+i+1];
        deadorAlive(board,nextboard,x,start,i+backstart);
    }

    for (int i = start; i <= frontend; ++i) {
        x= board[wrapBoxRow-1][topstart-1+i]+
           board[wrapBoxRow-1][topstart+i]+
           board[wrapBoxRow-1][topstart+1+i]+
           board[wrapBoxRow][topstart+i-1]+
           board[wrapBoxRow][topstart+i]+
           board[wrapBoxRow][topstart+i+1]+
           board[start][backstart+i-1]+
           board[start][backstart+i]+
           board[start][backstart+i+1];
        deadorAlive(board,nextboard,x,start,i+topstart);
    }

    //right
    for (int i = start; i <= row-1; ++i) {
        x= board[start*i-1][topstart-1]+
           board[start*i][topstart]+
           board[start*i+1][topstart+1]+
           board[start][leftstart+i-1]+
           board[start][leftstart+i]+
           board[start][leftstart+i+1]+
           board[start+1][leftstart+i-1]+
           board[start+1][leftstart+i]+
           board[start+1][leftstart+i+1];
        deadorAlive(board,nextboard,x,start,leftstart+i);
    }

    for (int i = start; i <= row-1; ++i) {
        x= board[start*i-1+1][topstart-1]+
           board[start*i+1][topstart]+
           board[start*i+1+1][topstart+1]+
           board[start*i-1][leftstart-1]+
           board[start*i][leftstart]+
           board[start*i+1][leftstart+1]+
           board[start+1][leftstart+i-1]+
           board[start+1][leftstart+i]+
           board[start+1][leftstart+i+1];
        deadorAlive(board,nextboard,x,start*i,leftstart);
    }

    //left

    for (int i = start; i <= row-1; ++i) {
        x= board[start*i-1][topstart-1]+
           board[start*i][topstart]+
           board[start*i+1][topstart+1]+
           board[start][rightstart+i-1]+
           board[start][rightstart+i]+
           board[start][rightstart+i+1]+
           board[start+1][rightstart+i-1]+
           board[start+1][rightstart+i]+
           board[start+1][rightstart+i+1];
        deadorAlive(board,nextboard,x,start,rightstart+i);
    }

    for (int i = start; i <= row-1; ++i) {
        x= board[start*i-1+1][topstart-1]+
           board[start*i+1][topstart]+
           board[start*i+1+1][topstart+1]+
           board[start*i-1][topstart-1]+
           board[start*i][topstart]+
           board[start*i+1][topstart+1]+
           board[start+1][rightstart+i-1]+
           board[start+1][rightstart+i]+
           board[start+1][rightstart+i+1];
        deadorAlive(board,nextboard,x,start*i,rightstart);
    }



}

void runlife(int board[row][column], int nextboard[row][column])
{
    int x = 0;
    //body
    for (int i = start; i < row ; ++i) {
        for (int j = start; j < wrapBoxColumn ; ++j) {
            if(i==0){
                mergetop(board,nextboard);
                //x = addUpLife(board, i, j);
            }
            else if(i==row-1)
                x=0;
                //mergebot(board,nextboard);
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


    for (int i = start; i < row-1; ++i) {
        for (int j =topstart; j <=topend ; ++j) {
        x = addUpLife(board,i,j);
            deadorAlive(board,nextboard,x,i,j);
        }
    }



    for (int i = start; i < row-1; ++i) {
        for (int j = botstart; j <= botend; ++j) {
            x= addUpLife(board,i,j);
            deadorAlive(board,nextboard,x,i,j);
        }
    }
    mergetop(board,nextboard);


}
int main()
{
    printf("Hello, World!\n");

    int board[row][column];
    int pboard[row][column];

    printf("Setup\n");

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < column; ++j) {
            board[i][j] = 0;
            pboard[i][j] = 0;
//            printf(" %d ",board[i][j]);
        }
        //printf("\n");
    }
    //printboard(board);

//    board[5][5] = 1;
//    board[4][5] = 1;
//    board[3][5] = 1;
    for(int i = start; i <= wrapBoxColumn;i++)
    {
        board[5][i] = 1;
    }

    for(int i = topstart; i <= topend;i++)
    {
       // board[5][i] = 1;
    }

    for(int i = botstart; i <= botend;i++)
    {
       // board[5][i] = 1;
    }

    printf("\n\nState 0:\n");
    printboard(board);

    runlife(board, pboard);
    printf("\n\nState 1:\n");
    printboard(pboard);

    runlife(pboard, board);
    printf("\n\nState 2:\n");
    printboard(board);

    return 0;

}


