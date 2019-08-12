#include <stdio.h>

#include "mat.h"
#include "gomoku.h"

// update the value at (row, col) of stone_mat and score_mat
// (row, col) must be a valid coordinate
// return: the score of the grid
// int place_stone(int row, int col, int player){

//     stone_mat.data[row][col] = player; // update stone_mat
    
//     // // find out the max score of the player's stones around (row, col)
//     // int max_score = 0;
//     // for (int i = row-1; i <= row+1; i++){
//     //     for (int j = col-1; j <= cols+1; j++){
//     //         int temp_value = mat_get(&score_mat, i, j);
//     //         int temp_player = mat_get(&stone_mat, i, j);
//     //         if (temp_value >= max_score && temp_player == palyer){
//     //             max_score = value;
//     //         }
//     //     }
//     // }
//     // max_score++;

//     // //TODO: increase scores of the grid and 8 adjacent grids if it is the same player
    
//     // score_mat.data[row][col] = max_score;    // update score_mat
//     return max_score;
// }

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