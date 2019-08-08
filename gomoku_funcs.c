#include <stdio.h>
#include "mat.h"
#include "gomoku.h"

// check if the player's stones form a unbreakable chain of 5
// return: 1 if the player wins, else 0
int check_win(int row, int col, int player){
    int total = 0;  // total number of stones in a chain
    // check col
    for (int i = row; mat_get(&stone_mat, i, col) == player; i++){   // upper row
        total++;
    }
    for (int i = row-1; mat_get(&stone_mat, i, col) == player; i--){ // lower row
        total++;
    }

    if (total == 5){
        return 1;
    }
    // printf("col total: %d\n", total);

    total = 0;

    // check row
    for (int j = col; mat_get(&stone_mat, row, j) == player; j++){
        total++;
    }
    for (int j = col-1; mat_get(&stone_mat, row, j) == player; j--){
        total++;
    }

    if (total == 5){
        return 1;
    }
    // printf("row total: %d\n", total);

    // check right lower diagnal
    // 1 0 0
    // 0 1 0
    // 0 0 1
    total = 0;
    for (int k = 0; mat_get(&stone_mat, row+k, col+k) == player; k++){
        total++;
    }
    for (int k = -1; mat_get(&stone_mat, row+k, col+k) == player; k--){
        total++;
    }

    if (total == 5){
        return 1;
    }

    // printf("right lower total: %d\n", total);

    // check right upper diagnal
    // 0 0 1
    // 0 1 0
    // 1 0 0
    total = 0;
    for (int k = 0; mat_get(&stone_mat, row+k, col-k) == player; k++){
        total++;
    }
    for (int k = -1; mat_get(&stone_mat, row+k, col-k) == player; k--){
        total++;
    }

    if (total == 5){
        return 1;
    }

    // printf("left upper total: %d\n", total);

    return 0;
}