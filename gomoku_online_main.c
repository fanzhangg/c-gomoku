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

    // 2 players connects to the server
    while (1){
        rset = allset;  // a copy of the set
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready == -1){  // error occurs
            perror("select");
            continue;
        }

        if (FD_ISSET(listenfd, &rset)){
            printf("Socket: a new client is connecting\n");
            clientfd = accept_connection(listenfd);
            printf("Socket: clientfd: %d", clientfd);
            FD_SET(clientfd, &allset);
            if (clientfd > maxfd){
                maxfd = clientfd;
            }
            printf("Connection from %s\n", inet_ntoa(connection.sin_addr));
            add_player(&new_players, clientfd, connection.sin_addr);
            char *greeting = "Welcome to Gomoku.\n";
            if (write(clientfd, greeting, strlen(greeting)) == -1){
                fprintf(stderr, "Error: fail to write to client %s\n", inet_ntoa(connection.sin_addr));
                remove_player(&new_players, clientfd);
            }
            else if (game.black == NULL){
                char *player_info = "You play as black stone\n";
                write(clientfd, player_info, strlen(player_info));
                game.black = get_new_client(clientfd, connection.sin_addr);
                printf("Balck has entered the game\n");
            } else if (game.white == NULL){
                char *player_info = "You play as white stone\n";
                write(clientfd, player_info, strlen(player_info));
                game.white = get_new_client(clientfd, connection.sin_addr);
                printf("White has entered the game\n");
            }
            if (game.black != NULL && game.white != NULL){  // both black and white have entered the game
                printf("Black: %s, White: %s\n", inet_ntoa(game.black->ipaddr), inet_ntoa(game.white->ipaddr));
                printf("Game starts\n");
                break;
            }
        }
    }

    // start with black
    struct client *player = game.black;
    char *player_str = "black";
    int player_id = 1;
    int row = 0;
    int col = 0;

    while (1){

        // get input from player
        char *TURN_MESSAGE = "It is your turn\n";
        write(player->fd, TURN_MESSAGE, strlen(TURN_MESSAGE));

        while(1){
            char *INPUT_INDICT = ">";
            write(player->fd, INPUT_INDICT, strlen(INPUT_INDICT));

            int str_len = read_from_input(player->inbuf, player->fd);
            if (str_len == -1){ // input error
                printf("Error: Fail to read from %s player\n", player_str);
                exit(1);
            }

            if (str_len > 0 && sscanf(player->inbuf, "%d %d", &row, &col) == 2){
                printf("Update %s: %d %d\n", player_str, row, col);
                break;
            } else if (str_len == -2){
                // newline, do nothing
            } else {
                printf("Error: invalid input from %s: %s\n", player_str, player->inbuf);
                char *INPUT_ERROR = "Error: invalid input. Try again:\n";
                // send warning to player
                write(player->fd, INPUT_ERROR, strlen(INPUT_ERROR));
            }
        }

        //TODO: handler inputs out of bound
        //TODO: the second input shows 2 '>'

        // update board
        mat_add(&game.stone_mat, row, col, player_id);
        mat_print(&game.stone_mat);

        // check if the player wins
        int win = check_win(&game.stone_mat, row, col, player_id);

        if (win == 1) {
            printf("%s wins\n", player_str);
            exit(0);
        }

        // switch the player
        if (player_id == 1){
            player_id = 2;
            player_str = "white";
            player = game.white;
        } else {
            player_id = 1;
            player_str = "black";
            player = game.black;
        }
    }
}