#include "header.h"
#define BACKLOG 3

int main() {
    int server_socket = 0, client_socket = 0;
    struct sockaddr_in server, client;
    int address_len = sizeof(client);
    char client_request[BUFF_SIZE], server_reply[BUFF_SIZE];

    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("ERROR: Failed to create socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUM);

    if(bind(server_socket, (struct sockaddr *) &server, sizeof(server)) < 0){
        perror("ERROR: Failed to bind");
        exit(EXIT_FAILURE);
    }

    // BACKLOG is the maximum number of clients allowed.
    if(listen(server_socket, BACKLOG) < 0){
        perror("ERROR: Could not listen");
        exit(EXIT_FAILURE);
    }

    if((client_socket = accept(server_socket, (struct sockaddr *) &client, (socklen_t *) &address_len)) < 0){
        perror("ERROR: Could not accept");
        exit(EXIT_FAILURE);
    }

    while(1){
        // clearing buffers
        memset(client_request, '\0', sizeof(client_request));
        memset(server_reply, '\0', sizeof(server_reply));

        read(client_socket, client_request, BUFF_SIZE);
        printf("Received: %s\n", client_request);

        if(strcmp(client_request, "time") == 0){
            char cur_time[100];
            memset(cur_time, '\0', sizeof(cur_time));
            time_t ticks = time(NULL);
            strcpy(cur_time, ctime(&ticks));
            write(client_socket, cur_time, sizeof(cur_time));
        }


        if(strcmp(client_request, "quit") == 0){
            close(client_socket);
            close(server_socket);
            return 0;
        }
        sleep(1);
    }
}
