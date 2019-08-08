#include "mat.h"
#include "gomoku.h"
#include <stdio.h>

int main(){
    mat_init(&stone_mat, 6, 6);

    printf("TEST: col win\t");
    mat_add(&stone_mat, 0, 0, 1);
    mat_add(&stone_mat, 1, 0, 1);
    mat_add(&stone_mat, 3, 0, 1);
    mat_add(&stone_mat, 4, 0, 1);
    mat_add(&stone_mat, 2, 0, 1);

    int win = check_win(2, 0, 1);
    if (win == 1){
        printf("SUCCESS\n");
    } else {
        printf("FAILED\n");
    }

    printf("TEST: row win\t");
    mat_zero_out(&stone_mat);
    mat_add(&stone_mat, 0, 0, 1);
    mat_add(&stone_mat, 0, 1, 1);
    mat_add(&stone_mat, 0, 2, 1);
    mat_add(&stone_mat, 0, 3, 1);
    mat_add(&stone_mat, 0, 4, 1);

    win = check_win(0, 3, 1);
    if (win == 1){
        printf("SUCCESS\n");
    } else {
        printf("FAILED\n");
    }

    printf("TEST: right lower win\t");
    mat_zero_out(&stone_mat);
    mat_add(&stone_mat, 0, 0, 1);
    mat_add(&stone_mat, 1, 1, 1);
    mat_add(&stone_mat, 2, 2, 1);
    mat_add(&stone_mat, 3, 3, 1);
    mat_add(&stone_mat, 4, 4, 1);

    win = check_win(3, 3, 1);
    if (win == 1){
        printf("SUCCESS\n");
    } else {
        printf("FAILED\n");
    }

    printf("TEST: right upper win\t");
    mat_zero_out(&stone_mat);
    mat_init(&stone_mat, 6, 6);
    mat_add(&stone_mat, 0, 4, 1);
    mat_add(&stone_mat, 1, 3, 1);
    mat_add(&stone_mat, 2, 2, 1);
    mat_add(&stone_mat, 3, 1, 1);
    mat_add(&stone_mat, 4, 0, 1);

    win = check_win(1, 3, 1);
    if (win == 1){
        printf("SUCCESS\n");
    } else {
        printf("FAILED\n");
    }

    printf("TEST: failed case\t");
    win = check_win(2, 3, 2);
    if (win == 0){
        printf("SUCCESS\n");
    } else {
        printf("FAILED\n");
    }
}