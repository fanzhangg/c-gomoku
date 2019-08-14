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

/* Connect white and black players
 * return: 0 if connection succeeds, 1 if connections fails
 */
int connect_players(struct game_state *game, struct sockaddr_in *server){
    int clientfd;
    struct sockaddr_in connection;

    // zero out stone_mat
    mat_zero_out(&game->stone_mat);

    // init server socket
    int listenfd = set_up_server_socket(server, MAX_QUEUE);

    // allow white and balck palyer to connect to the game
    //TODO: handle more than 3 clients, a client quits in the middle of the game
    while (1){
            printf("A new client is connecting\n");
            clientfd = accept_connection(listenfd);

            printf("ip: %s  fd: %d\n", inet_ntoa(connection.sin_addr), clientfd);
            char *GREET_MSG = "**Welcome to Gomoku**\n";
            if (write(clientfd, GREET_MSG, strlen(GREET_MSG)) == -1){
                fprintf(stderr, "Error: fail to write to client %s\n", inet_ntoa(connection.sin_addr));
            }
            else if (game->black == NULL){   // the first player entering the game plays as black
                char *player_info = "You play as black stone\nWaiting for another player...\n";
                write(clientfd, player_info, strlen(player_info));
                game->black = get_new_client(clientfd, connection.sin_addr);
                game->black->id = 2;
                game->black->stone_name = "black";
                printf("Balck has entered the game\n");
            } else if (game->white == NULL){
                char *PLAYER_INFO = "You play as white stone\n";
                write(clientfd, PLAYER_INFO, strlen(PLAYER_INFO));
                game->white = get_new_client(clientfd, connection.sin_addr);
                game->white->id = 1;
                game->white->stone_name = "white";
                printf("White has entered the game\n");
            }
            if (game->black != NULL && game->white != NULL){  // both black and white have entered the game
                printf("Black: %s, White: %s\n", inet_ntoa(game->black->ipaddr), inet_ntoa(game->white->ipaddr));
                printf("Game starts\n");
                break;
            }
    }
    return 0;
}


/* Play one round of game
 * return: 1 if black wins, 2 if white wins
 */
int play_round(struct game_state *game){

    // balck moves first
    struct client *player = game->black;
    struct client *opponent = game->white;

    // write the board to the client
    write_board(&game->stone_mat, player->fd);
    write_board(&game->stone_mat, opponent->fd);
    mat_print(&game->stone_mat);

    // make a move
    while (1){
        char row_c;
        char col_c;
        int col = 0;
        int row = 0;

        // get input from player & update matrix
        //TODO: read input only when it is the player's turn
        char *TURN_MSG = "It is your turn. usage: <row> <col>\n"; 
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
            } else if (sscanf(player->inbuf, "%c %c", &row_c, &col_c) != 2){ // invalid format
                printf("Error: invalid format %s\n", player->inbuf);
                char *FORMAT_ERR = "Error: invalid format. usage: <row> <col>\n";
                write(player->fd, FORMAT_ERR, strlen(FORMAT_ERR));
            } else if (!(row_c >= 'A' && row_c <= 'Z') && !(row_c >= '0' && row_c <= '9')){   // invalid row_c format
                printf("Error: row %c is not a letter in 'A' and 'Z' or '0' and '9'", row_c);
                char *ROW_LETTER_ERR = "Error: invalid row format. usage: a letter in 'A' and 'Z' or '0' and '9'\n";
                write(player->fd, ROW_LETTER_ERR, strlen(ROW_LETTER_ERR));
            } else if (!(col_c >= 'A' && col_c <= 'Z') && !(col_c >= '0' && col_c <= '9')){   // invalid col format
                printf("Error: col %c is not a letter in 'A' and 'Z' or '0' and '9'", col_c);
                char *COL_LETTER_ERR = "Error: invalid col format. usage: a letter in 'A' and 'Z' or '0' and '9'\n";
                write(player->fd, COL_LETTER_ERR, strlen(COL_LETTER_ERR));
            } else{
                printf("row_c: %c, col_c: %c\n", row_c, col_c);
                // convert col to integer
                if (col_c >= '0' && col_c <= '9'){
                    col = col_c - '0';
                    printf("0-9\n");
                } else {
                    printf("A-Z\n");
                    col = col_c - 'A' + 10;
                }
                // convert row to integer
                if (row_c >= '0' && row_c <= '9'){
                    row = row_c - '0';
                } else {
                    row = row_c - 'A' + 10;
                }
                printf("col: %d, row: %d\n", col, row);

                if (row < 0 || row >= game->stone_mat.rows){    // row out of bound
                    printf("Error: row %d out of bound\n", row);
                    char *ROW_ERR = "Error: row out of bound\n";
                    write(player->fd, ROW_ERR, strlen(ROW_ERR));
                } else if (col < 0 || col >= game->stone_mat.cols){    // col out of bound
                    printf("Error: row %d out of bound\n", col);
                    char *COL_ERR = "Error: column out of bound\n";
                    write(player->fd, COL_ERR, strlen(COL_ERR));
                } else if (game->stone_mat.data[row][col] != 0){    // position occupied
                    printf("Error: (%d, %d) has been occupied\n", row, col);
                    char *OCCUPY_ERR = "Error: position occupied\n";
                    write(player->fd, OCCUPY_ERR, strlen(OCCUPY_ERR));
                } else {
                    mat_add(&game->stone_mat, row, col, player->id);
                    break;
                }
            }
        }

        // write the board to the client
        write_board(&game->stone_mat, player->fd);
        write_board(&game->stone_mat, opponent->fd);
        mat_print(&game->stone_mat);

        // check if the player wins
        int win = check_win(&game->stone_mat, row, col, player->id);

        // write end message
        if (win == 1) {
            printf("%s wins\n", player->stone_name);
            char *WIN_MSG = "You Won!\n";
            write(player->fd, WIN_MSG, strlen(WIN_MSG));
            char *LOST_MSG = "You Lost!";
            write(opponent->fd, LOST_MSG, strlen(LOST_MSG));
            return player->id;
        }

        char *WAIT_MSG = "Wait for another player to make a move...\n";
        write(player->fd, WAIT_MSG, strlen(WAIT_MSG));

        // switch the player
        struct client *tmp = player;
        player = opponent;
        opponent = tmp;
    }
}

// int check_start_new_round(struct client *p1, struct client *p2){
//     char *NEW_MSG = "DO you want to start a new round?(y/n)";
//     write(p1->fd, NEW_MSG, strlen(NEW_MSG));
//     write(p2->fd, NEW_MSG, strlen(NEW_MSG));

//     int p1_start = 0;
//     int p2_start = 0;

//     while(1){
//         char *INPUT_INDICT = ">";
//         if (p1_start == 1){
//             write(p1->fd, INPUT_INDICT, strlen(INPUT_INDICT));

//         }
        
//         write(p2->fd, INPUT_INDICT, strlen(INPUT_INDICT));

//         int p1_str_len = read_from_input(p1->inbuf, p1->fd);
//         if (p1_str_len == -1){ // input error
//             printf("Error: Fail to read from %s player\n", p1->stone_name);
//             exit(1);
//         }

//         int p2_str_len = read_from_input(p2->inbuf, p2->fd);
//         if (p2_str_len == -1){ // input error
//             printf("Error: Fail to read from %s player\n", p2->stone_name);
//             exit(1);
//         }

//         if (p1->inbuf[0] == 'n' || p2->inbuf[0] == 'n'){
//             return 1;
//         } else if (p1->inbuf[0] == 'y'){
//             p1_start = 1;
//         } else if (p2->inbuf[0] == 'y'){
//             p2_start = 1;
//         }

//         if (p1_start == 1 && p2_start == 1){
//             return 0;
//         }
//     }
// }

int main(){
    // init game
    struct game_state game;
    init_game(&game);

    // init socket
    struct sockaddr_in *server = init_server_addr(PORT);

    // connect players
    connect_players(&game, server);

    int black_score = 0;
    int white_score = 0;

    // play rounds
    while(1){
        printf("Black: %d | White: %d\n", black_score, white_score);
        char score_str[124];
        sprintf(score_str, "Game starts!\nblack: %d | white: %d\n", black_score, white_score);
        write(game.black->fd, score_str, strlen(score_str));
        write(game.white->fd, score_str, strlen(score_str));

        int win_id = play_round(&game);
        if (win_id == 1){
            black_score++;
        } else {
            white_score++;
        }
        mat_zero_out(&game.stone_mat);
    }

    free(game.black);
    free(game.white);
    free(server);
}