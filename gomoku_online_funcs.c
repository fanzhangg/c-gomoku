#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#include "socket.h"
#include "gomoku_online.h"

extern fd_set allset;

struct client *get_new_client(int fd, struct in_addr ipaaddr){
    struct client *cl = malloc(sizeof(struct client));
    cl->fd = fd;
    cl->ipaddr = ipaaddr;
    cl->name[0] = '\0';
    cl->inbuf[0] = '\0';
    cl->in_ptr = cl->inbuf;
    return cl;
}

/* Add a client to the head of the linked list */
void add_player(struct client **top, int fd, struct in_addr addr){
    struct client *client = malloc(sizeof(struct client));
    if (client == NULL){
        perror("malloc");
        exit(1);
    }

    client->fd = fd;
    client->ipaddr = addr;
    client->name[0] = '\0';
    client->in_ptr = client->inbuf;
    client->inbuf[0] = '\0';
    client->next = *top;
    *top = client;
}

/* remove client from the linked list and close the client's socket
 * remove socket descriptor from allset
 */
void remove_player(struct client **top, int fd){    // loop through all clients
    struct client **c;
    for (c = top; *c && (*c)->fd != fd; c = &(*c)->next){
        if (*c == NULL) {   // reach end of the list
            fprintf(stderr, "Error: fail to remove fd %d\n", fd);
        }
        struct client *tmp = (*c)->next;
        printf("Removing client %d %s\n", fd, inet_ntoa((*c)->ipaddr));
        FD_CLR((*c)->fd, &allset);
        close((*c)->fd);
        free(*c);
        *c = tmp;
    }
}

/* init the stone mat, balck and white to NULL */
void init_game(struct game_state *game){
    mat_init(&game->stone_mat, MAT_ROW, MAT_COL);
    game->black = NULL;
    game->white = NULL;
}

/* read the input, handle network newline
 * return: total number of characters read, -1 if an error errors
 * return: -2 if the line is empty (only contain escape character) and assign lien to be an empty string
*/
int read_from_input(char *line, int fd){
    line[0] = '\0';

    int char_len = read(fd, line, MAX_BUF);
    if (char_len == -1){  
        perror("read");
        return -1;
    }
    if (line[1] == '\n' && line[0] == '\r'){
        printf("Warning: the line only contains escape char\n");
        printf("line[0]: %c", line[0]);
        return -2;
    }
    line[char_len] = '\0';
    printf("[%d] Read %d bytes: %s\n", fd, char_len, line);
    return char_len;
}

/* check if the player wins (form a unbreakable chain of 5)
 * return: 1 if the player wins, else 0
*/
int check_win(mat_t *stone_mat, int row, int col, int player){
        int total = 0;  // total number of stones in a chain
    // check col
    for (int i = row; mat_get(stone_mat, i, col) == player; i++){   // upper row
        total++;
    }
    for (int i = row-1; mat_get(stone_mat, i, col) == player; i--){ // lower row
        total++;
    }

    if (total == 5){
        return 1;
    }

    total = 0;

    // check row
    for (int j = col; mat_get(stone_mat, row, j) == player; j++){
        total++;
    }
    for (int j = col-1; mat_get(stone_mat, row, j) == player; j--){
        total++;
    }

    if (total == 5){
        return 1;
    }

    // check right lower diagnal
    // 1 0 0
    // 0 1 0
    // 0 0 1
    total = 0;
    for (int k = 0; mat_get(stone_mat, row+k, col+k) == player; k++){
        total++;
    }
    for (int k = -1; mat_get(stone_mat, row+k, col+k) == player; k--){
        total++;
    }

    if (total == 5){
        return 1;
    }

    // check right upper diagnal
    // 0 0 1
    // 0 1 0
    // 1 0 0
    total = 0;
    for (int k = 0; mat_get(stone_mat, row+k, col-k) == player; k++){
        total++;
    }
    for (int k = -1; mat_get(stone_mat, row+k, col-k) == player; k--){
        total++;
    }

    if (total == 5){
        return 1;
    }
    return 0;
}