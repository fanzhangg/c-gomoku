#include <netinet/in.h>

#define MAX_NAME 30
#define MAX_BUF 256
#define WELCOME_MSG "Welcome to the world of Gomoku. What is your name?\n";

#ifndef PORT
#define PORT 4000   // port is 4000 by default
#endif
#define MAX_QUEUE 5

#define MAT_ROW 15
#define MAT_COL 15

struct client {
    int fd;
    struct in_addr ipaddr;  // ip address
    struct client *next;
    char name[MAX_NAME];
    char inbuf[MAX_BUF];    // hold input from client
    char *in_ptr;   // a pointer into inbuf for reading
};

struct game_state {
    int stone_mat;
    struct client *white;   // white player, NULL if white does not exist/leave
    struct client *black;   // black player, NULL if black does not exist/leave
};

// socket helper functions
void add_player(struct client **top, int fd, struct in_addr addr);
void remove_player(struct client **top, int fd);
struct client *get_new_client(int fd, struct in_addr ipaaddr);


// game functions
void init_game(struct game_state *game);