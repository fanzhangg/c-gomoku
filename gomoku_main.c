#include <stdio.h>

#include "mat.h"
#include "gomoku.h"

int main(){
    mat_init(&stone_mat, rows, cols);
    mat_init(&score_mat, rows, cols);
    int player = 1; // 1 if black stone, 2 if white stone

    mat_print(&stone_mat);

    while(1){

        int row = 0;
        int col = 0;
        // get the input of stone
        while(1){
            // check whose turn
            if (player == 1){
                printf("Black stone's turn:");
            } else {
                printf("White stone's turn:");
            }

            char line[1024];
            fgets(line, 1023, stdin);

            // check if the input if valid
            if (sscanf(line, "%d %d", &row, &col) != 2){
                printf("Error: invalid input format (must be <row> <col>)\n");
            }
            else if (row < 0 || row >= rows || col < 0 || col >= cols){
                printf("Error: the row index %d or col index %d is out of bound\n", row, col);
            }
            else if (stone_mat.data[row][col] != 0){ 
                printf("Error: the position (%d, %d) has been occupied\n", row, col);
            } else {
                break;
            }
        }
        mat_add(&stone_mat, row, col, player);
        mat_print(&stone_mat);

        int win = check_win(row, col, player);
        // check if the player wins
        if (win == 1){
            if (player == 1){
                printf("Black wins!\n");
            } else {
                printf("White wins!\n");
            }
        }

        // switch the player
        if (player == 1){
            player = 2;
        } else {
            player = 1;
        }

    }
    return 0;
}