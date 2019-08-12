CFLAGS = -Wall -g
CC	   = gcc $(CFLAGS)

# mat
mat_utils.o : mat_utils.c
	$(CC) -c $<

test_mat.o : test_mat.c
	$(CC) -c $<

test_mat : test_mat.o	mat_utils.o
	$(CC) -o $@ $^

# gomoku
gomoku_main.o : gomoku_main.c
	$(CC) -c $<

gomoku_funcs.o : gomoku_funcs.c
	$(CC) -c $<

gomoku : gomoku_main.o gomoku_funcs.o	mat_utils.o
	$(CC) -o $@ $^

test_gomoku.o : test_gomoku.c
	$(CC) -c $<

test_gomoku : gomoku_funcs.o mat_utils.o test_gomoku.o
	$(CC) -o $@ $^

# gomoku_online
gomoku_online_funcs.o : gomoku_online_funcs.c
	$(CC) -c $<

gomoku_online_main.o : gomoku_online_main.c
	$(CC) -c $<

socket.o : socket.c
	$(CC) -c $<

gomoku_online : gomoku_online_funcs.o gomoku_online_main.o socket.o mat_utils.o
	$(CC) -o $@ $^
