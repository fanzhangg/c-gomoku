# include <stdio.h>
# include "mat.h"
# include "gomoku.h"

int main(){
    stone_mat_init(&stone_mat, 6, 10);
    stone_mat_print(&stone_mat);
    stone_mat_add(&stone_mat, 1, 2, 1);
    int value = stone_mat_get(&stone_mat, 1, 2);
    stone_mat_print(&stone_mat);
    printf("%d\n", value);
}