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

/* Init the stone mat, balck and white to NULL */
void init_game(struct game_state *game){
    mat_init(game->stone_mat, MAT_ROW, MAT_COL);
    game->black = NULL;
    game->white = NULL;
}