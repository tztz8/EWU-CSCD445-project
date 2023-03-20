//
// Created by tztz8 on 3/19/23.
//

#ifndef EWU_CSCD445_PROJECT_GAMEOFLIFELOGIC_H
#define EWU_CSCD445_PROJECT_GAMEOFLIFELOGIC_H

// TODO: fix this (Ex. hava row and col passed in)
//void runlife(int board[6][6*6], int nextboard[6][6*6]);
void runlife(int * board, int * nextboard, int inbound_row, int inbound_col);

void printboard(int * board, int row, int col);

#endif //EWU_CSCD445_PROJECT_GAMEOFLIFELOGIC_H
