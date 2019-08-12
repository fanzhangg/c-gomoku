#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include "socket.h"
#include "gomoku_online.h"

/* The set of socket descriptors for select to monitor
 * Remove socket descrptions from allset when a write to a socket fails
 */
fd_set allset;

/* A list of client who have not yet entered their name
 * Keep separate from the list of active players in the game
 * A player can play or receive broadcast messages until he enters his name
 */
struct client *new_players = NULL;



int main(int argc, char **argv){
    int maxfd, nready, clientfd;
    struct sockaddr_in connection;
    fd_set rset;

    // init game
    struct game_state game;
    init_game(&game);

    // init server socket
    struct sockaddr_in *server = init_server_addr(PORT);
    int listenfd = set_up_server_socket(server, MAX_QUEUE);

    FD_ZERO(&allset);   // initialize allset
    FD_SET(listenfd, &allset);  // add listenfd to the et of file descriptors    
    maxfd = listenfd; // how far into the set to search

    // add black player
    while (1){
        rset = allset;  // a copy of the set
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready == -1){  // error occurs
            perror("select");
            continue;
        }

        if (FD_ISSET(listenfd, &rset)){
            printf("A new client is connecting\n");
            clientfd = accept_connection(listenfd);

            FD_SET(clientfd, &allset);
            if (clientfd > maxfd){
                maxfd = clientfd;
            }
            printf("Connection from %s\n", inet_ntoa(connection.sin_addr));
            add_player(&new_players, clientfd, connection.sin_addr);
            char *greeting = "Welcome to Gomoku. You are the black palyer.\nPlease enter your name:\n>";
            if (write(clientfd, greeting, strlen(greeting)) == -1){
                fprintf(stderr, "Error: fail to write to client %s\n", inet_ntoa(connection.sin_addr));
                remove_player(&new_players, clientfd);
            }
            else if (game.black == NULL){
                game.black = get_new_client(clientfd, connection.sin_addr);
                printf("Balck has entered the game\n");
            } else if (game.white == NULL){
                game.white = get_new_client(clientfd, connection.sin_addr);
                printf("White has entered the game\n");
            } else {
                printf("Black: %s, White: %s\n", inet_ntoa(game.black->ipaddr), inet_ntoa(game.white->ipaddr));
                printf("Game starts\n");
                break;
            }
        }
        // int cur_fd;
        // for (cur_fd = 0; cur_fd < 2; cur_fd ++){
        //     if (FD_ISSET(cur_fd, &rset)){
        //         // check if the socket descriptor is an active player
        //         for (p = game.head; p != NULL;)
        //     }
    }
}