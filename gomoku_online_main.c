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
    int clientfd;
    struct sockaddr_in connection;

    // init game
    struct game_state game;
    init_game(&game);
    printf("Init game\n");
    mat_print(&game.stone_mat);

    // init server socket
    struct sockaddr_in *server = init_server_addr(PORT);
    int listenfd = set_up_server_socket(server, MAX_QUEUE);

    // allow white and balck palyer to connect to the game
    while (1){
            printf("A new client is connecting\n");
            clientfd = accept_connection(listenfd);

            printf("ip: %s  fd: %d\n", inet_ntoa(connection.sin_addr), clientfd);
            char *GREET_MSG = "**Welcome to Gomoku**\n";
            if (write(clientfd, GREET_MSG, strlen(GREET_MSG)) == -1){
                fprintf(stderr, "Error: fail to write to client %s\n", inet_ntoa(connection.sin_addr));
            }
            else if (game.black == NULL){   // the first player entering the game plays as black
                char *player_info = "You play as black stone\nWaiting for another player...\n";
                write(clientfd, player_info, strlen(player_info));
                game.black = get_new_client(clientfd, connection.sin_addr);
                game.black->id = 2;
                game.black->stone_name = "black";
                printf("Balck has entered the game\n");
            } else if (game.white == NULL){
                char *PLAYER_INFO = "You play as white stone\n";
                write(clientfd, PLAYER_INFO, strlen(PLAYER_INFO));
                game.white = get_new_client(clientfd, connection.sin_addr);
                game.white->id = 1;
                game.white->stone_name = "white";
                printf("White has entered the game\n");
            }
            if (game.black != NULL && game.white != NULL){  // both black and white have entered the game
                printf("Black: %s, White: %s\n", inet_ntoa(game.black->ipaddr), inet_ntoa(game.white->ipaddr));
                printf("Game starts\n");
                char *START_MSG = "Game starts!\n";
                write(game.black->fd, START_MSG, strlen(START_MSG));
                write(game.white->fd, START_MSG, strlen(START_MSG));
                break;
            }
    }

    // balck moves first
    struct client *player = game.black;
    struct client *opponent = game.white;
    // char *player_str = "black";
    //int player_id = 1;


    // make a move
    while (1){
        int row = 0;
        int col = 0;

        // get input from player & update matrix
        char *TURN_MSG = "It is your turn\n"; 
        write(player->fd, TURN_MSG, strlen(TURN_MSG)); 

        while(1){
            char *INPUT_INDICT = ">";
            write(player->fd, INPUT_INDICT, strlen(INPUT_INDICT));

            int str_len = read_from_input(player->inbuf, player->fd);
            if (str_len == -1){ // input error
                printf("Error: Fail to read from %s player\n", player->stone_name);
                exit(1);
            }
            
            if (str_len == 0){  // empty string
                printf("Error: empty string\n");
                char *EMPTY_STR_ERR = "Error: empty string\n";
                write(player->fd, EMPTY_STR_ERR, strlen(EMPTY_STR_ERR));
            } else if (sscanf(player->inbuf, "%d %d", &row, &col) != 2){ // invalid format
                printf("Error: invalid format %s\n", player->inbuf);
                char *FORMAT_ERR = "Error: invalid format. usage: <row> <col>\n";
                write(player->fd, FORMAT_ERR, strlen(FORMAT_ERR));
            } else if (row < 0 | row > game.stone_mat.rows){    // row out of bound
                printf("Error: row %d out of bound\n", row);
                char *ROW_ERR = "Error: row out of bound\n";
                write(player->fd, ROW_ERR, strlen(ROW_ERR));
            } else if (col < 0 | col > game.stone_mat.cols){    // col out of bound
                printf("Error: row %d out of bound\n", col);
                char *COL_ERR = "Error: column out of bound\n";
                write(player->fd, COL_ERR, strlen(COL_ERR));
            } else if (game.stone_mat.data[row][col] != 0){    // position occupied
                printf("Error: (%d, %d) has been occupied\n", row, col);
                char *OCCUPY_ERR = "Error: position occupied\n";
                write(player->fd, OCCUPY_ERR, strlen(OCCUPY_ERR));
            } else {
                mat_add(&game.stone_mat, row, col, player->id);
                break;
            }
        }

        // write the board to the client
        write_board(&game.stone_mat, player->fd);
        write_board(&game.stone_mat, opponent->fd);
        mat_print(&game.stone_mat);

        // check if the player wins
        int win = check_win(&game.stone_mat, row, col, player->id);

        // write end message
        if (win == 1) {
            printf("%s wins\n", player->stone_name);
            char *WIN_MSG = "You Won!\n";
            write(player->fd, WIN_MSG, strlen(WIN_MSG));
            char *LOST_MSG = "You Lost!";
            write(opponent->fd, LOST_MSG, strlen(LOST_MSG));
            exit(0);
        }

        char *WAIT_MSG = "Wait for another player to make a move...\n";
        write(player->fd, WAIT_MSG, strlen(WAIT_MSG));

        // switch the player
        struct client *tmp = player;
        player = opponent;
        opponent = tmp;
    }
}