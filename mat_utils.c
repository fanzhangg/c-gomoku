# include <stdio.h>
# include <stdlib.h>
# include "mat.h"

// intialize a matrix of i rows and j cols
// return: 0 if success, 1 if fail
int mat_init(mat_t *mat, int rows, int cols){
    if (rows <= 0 || cols <= 0){
        printf("Error: invalid rows %d or cols %d\n", rows, cols);
        return 1;
    }
    mat->rows = rows;
    mat->cols = cols;
    mat->data = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++){
        mat->data[i] = malloc(cols * sizeof(int));
    }
    mat_zero_out(mat);
    return 0;
}

// return: the value at (row, col) of mat, -1 if the the value does not exist
int mat_get(mat_t *mat, int row, int col){
    if (row < 0 || row >= mat->rows){
        // printf("Error: the row index %d is out of bound\n", row);
        return -1;
    }
    if (col < 0 || col >= mat->cols){
        // printf("Error: the col index %d is out of bound\n", col);
        return -1;
    }
    return mat->data[row][col];
}

// add value to (row, col) of mat
// return: 0 if success, else 1
int mat_add(mat_t *mat, int row, int col, int value){
        if (row < 0 || row >= mat->rows){
        printf("Error: the row index %d is out of bound\n", row);
        return 1;
    }
    if (col < 0 || col >= mat->cols){
        printf("Error: the col index %d is out of bound\n", col);
        return 1;
    }
    mat->data[row][col] = value;
    return 0;
}

// zero out all values in matrix
void mat_zero_out(mat_t *mat){
    for (int i = 0; i < mat->rows; i++){
        for (int j = 0; j < mat->cols; j++){
            mat->data[i][j] = 0;
        }
    }
}

// print the matrix
void mat_print(mat_t *mat){
    for (int i = 0; i < mat->rows; i++){
        for (int j = 0; j < mat->cols; j++){
            printf("%d ", mat->data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}