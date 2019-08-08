int check_win(int row, int col, int player);

# define rows 13
# define cols 13
// a matrix to keep track of the stone type of each grid (0 if no stone, 1 if black stone, 2 if white stone)
mat_t stone_mat; 

// a matrix of score of the max stone number of an unbreakable chain in each grid
// 0 if the stone is not whithin any chain
mat_t score_mat;