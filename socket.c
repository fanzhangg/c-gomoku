#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>  // get host name
#include <sys/socket.h>

#include "socket.h"

// initialize a server address assocaited with the given port
struct sockaddr_in *init_server_addr(int port){
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = PF_INET; // allow sockets across machines
    addr->sin_port = htons(port);   // the port for the process to listen on
    memset(&(addr->sin_zero), 0, 8);    // clear the field  * sin_zero is for padding for the struct
    addr->sin_addr.s_addr = INADDR_ANY; // listen on all network interfaces
    return addr;
}

// create and set up a socket for a server to listen on
int set_up_server_socket(struct sockaddr_in *self, int num_queue){
    int soc = socket(PF_INET, SOCK_STREAM, 0);
    if (soc < 0){
        perror("socket");
        exit(1);
    }
    // Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    int status = setsockopt(soc, SOL_SOCKET, SO_REUSEADDR,
        (const char *) &on, sizeof(on));
    if (status < 0){
        perror("setsockopt");
        exit(1);
    }
    // associate the process with the address and a port
    if (bind(soc, (struct sockaddr *)self, sizeof(*self)) < 0) {
        perror("bind");
        exit(1);
    }
    // set up a queue in the kernel to hold pending coneections
    if (listen(soc, num_queue) < 0){
        perror("listen");   // listen fails
        exit(1);
    }
    return soc;
}

int accept_connection(int listenfd){
    struct sockaddr_in peer;
    unsigned int peer_len = sizeof(peer);
    peer.sin_family = PF_INET;

    printf("Waiting for a new connection...\n");
    int client_socket = accept(listenfd, (struct sockaddr *)&peer, &peer_len);
    if (client_socket < 0){
        perror("accept");
        exit(1);
    } else {
        printf("New connection accepted from %s:%d\n",
            inet_ntoa(peer.sin_addr),
            ntohs(peer.sin_port));
        return client_socket;
    }
}