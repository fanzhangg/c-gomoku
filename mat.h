typedef struct {
    int rows;
    int cols;
    int **data;
    int total; // total stones in the matrix
} mat_t;

int mat_init(mat_t *mat, int rows, int cols);
void mat_print(mat_t *mat);
int mat_get(mat_t *mat, int row, int col);
int mat_add(mat_t *mat, int row, int col, int value);
void mat_zero_out(mat_t *mat);