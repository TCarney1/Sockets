#include "server_header.h"
#define BACKLOG 3

int main() {
    struct stat st = {0};
    int server_socket = 0, client_socket = 0;
    struct sockaddr_in server, client;
    int address_len = sizeof(client);
    char *client_request, *server_reply;
    client_request = (char*)malloc(BUFF_SIZE * sizeof(char));
    server_reply = (char*)malloc(BUFF_SIZE * sizeof(char));


    int num_strings = 10;
    char **args = NULL;
    args = malloc(num_strings * (sizeof(char *)));
    if(args == NULL){
        printf("ERROR: Failed to allocate memory.\n");
        exit(1);
    }
    for (int i=0; i<num_strings; i++) {
        args[i] = malloc(sizeof(char) * BUFF_SIZE);
        if(args[i] == NULL){
            printf("ERROR: Failed to allocate memory.\n");
            exit(1);
        }
    }

    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("ERROR: Failed to create socket");
        exit(EXIT_FAILURE);
    }

    printf("--- Socket created ---\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUM);

    if(bind(server_socket, (struct sockaddr *) &server, sizeof(server)) < 0){
        perror("ERROR: Failed to bind");
        exit(EXIT_FAILURE);
    }
    printf("--- Bind successful ---\n");

    // BACKLOG is the maximum number of clients allowed.
    if(listen(server_socket, BACKLOG) < 0){
        perror("ERROR: Could not listen");
        exit(EXIT_FAILURE);
    }
    printf("--- Listening ---\n");

    if((client_socket = accept(server_socket, (struct sockaddr *) &client, (socklen_t *) &address_len)) < 0){
        perror("ERROR: Could not accept");
        exit(EXIT_FAILURE);
    }
    printf("--- Client connected ---\n");

    while(1){
        // clearing buffers
        memset(client_request, '\0', strlen(client_request));
        memset(server_reply, '\0', strlen(server_reply));
        memset(args, '\0', (num_strings*BUFF_SIZE) * (sizeof *args));

        if(client_request == NULL || server_reply == NULL || args == NULL){
            printf("ERROR: Could not allocate memory.\n");
            exit(1);
        }

        // get whole line of client request.
        read(client_socket, client_request, BUFF_SIZE);
        printf("Received: %s", client_request);

        // break client request into [arg1] [arg2] [arg3]...
        char *token;
        token = strtok(client_request, " ,\n");

        // check for quit request
        if(strcmp(client_request, "quit") == 0){
            free(client_request);
            free(server_reply);
            close(client_socket);
            close(server_socket);
            for(int i = 0; i < num_strings; i++){
                free(args[i]);
            }
            free(args);
            return 0;
        }
        // still breaking input string into sections
        int arg_count = 0;
        while(token != NULL){
            token = strtok(NULL, " ,");
            args[arg_count] = token;
            arg_count++;
        }

        if(strcmp(client_request, "put") == 0) {
            printf("--- Put request ---\n");

            // check if directory exists.
            if(stat(args[0], &st) == -1){

                printf("--- Directory Created ---\n");
                write(client_socket, "0", sizeof(char));
                // make directory because it doesnt exist.
                mkdir(args[0], 0777);

                // file handling
                FILE *fp = NULL;
                args[1][strlen(args[1]) - 1] = '\0'; //getting ride of the '\n' for the file name
                fp = fopen(args[1], "w");

                if(fp == NULL){
                    printf("ERROR: Failed to open file.\n");
                    exit(1);
                }
                printf("--- File opened ---\n");

                // file copying
                char line[256];
                while(1){
                    read(client_socket, client_request, sizeof(line));
                    if(strcmp(client_request, "-1-1") == 0){
                        break;
                    }
                    fputs(client_request, fp);

                }
                fclose(fp);

            } else {
                printf("ERROR: Directory already exists. Use -f flag to replace directory.\n");
                write(client_socket, "1", sizeof(char));

            }
        }
    }
}
