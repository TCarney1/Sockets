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

    if(client_request == NULL || server_reply == NULL){
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }


    int num_strings = 10;
    char **args = NULL;
    args = malloc(num_strings * (sizeof(char *)));
    if(args == NULL){
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }
    for (int i=0; i<num_strings; i++) {
        args[i] = malloc(sizeof(char) * BUFF_SIZE);
        if(args[i] == NULL){
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
    }

    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    printf("--- Socket created ---\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUM);

    if(bind(server_socket, (struct sockaddr *) &server, sizeof(server)) < 0){
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    printf("--- Bind successful ---\n");

    // BACKLOG is the maximum number of clients allowed.
    if(listen(server_socket, BACKLOG) < 0){
        perror("Error listening to socket");
        exit(EXIT_FAILURE);
    }
    printf("--- Listening ---\n");

    if((client_socket = accept(server_socket, (struct sockaddr *) &client, (socklen_t *) &address_len)) < 0){
        perror("Error accepting client");
        exit(EXIT_FAILURE);
    }
    printf("--- Client connected ---\n");

    while(1){
        // clearing buffers
        memset(client_request, '\0', strlen(client_request));
        memset(server_reply, '\0', strlen(server_reply));
        memset(args, '\0', (num_strings*BUFF_SIZE) * (sizeof *args));

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

        // puts client's file on server
        if(strcmp(client_request, "put") == 0) {
            printf("--- Put request ---\n");

            // check if directory exists.
            if(stat(args[0], &st) == -1){

                printf("--- Directory Created ---\n");
                write(client_socket, "0", sizeof(char));
                // make directory because it doesnt exist.
                mkdir(args[0], 0777);

                // file handling and making of file path
                FILE *fp = NULL;
                char *file_name = malloc(BUFF_SIZE * sizeof(char));
                if(file_name == NULL){
                    perror("Error allocating memory");
                    exit(EXIT_FAILURE);
                }

                // makes 'file_name' = [current directory]/[dir]/[file]
                make_file_path(file_name, args[0], args[1]);
                fp = fopen(file_name, "w");
                if(fp == NULL){
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }
                printf("--- File opened ---\n");

                // file copying
                while(1){
                    read(client_socket, client_request, sizeof(char) * 256);
                    if(strcmp(client_request, "-1-1") == 0){
                        break;
                    }
                    fputs(client_request, fp);

                }
                printf("--- copy complete ---\n");

                free(file_name);
                fclose(fp);

            } else {
                printf("Error directory already exists. Use -f flag to replace directory.\n");
                write(client_socket, "1", sizeof(char));

            }
        }
        else if(strcmp(client_request, "get") == 0){
            // check if directory exists.
            if(stat(args[0], &st) >= 0){
                printf("--- Directory Found ---\n");
                write(client_socket, "0", sizeof(char)); // tell the client we found dir
                char *file_name = malloc(BUFF_SIZE * sizeof(char));
                if(file_name == NULL){
                    perror("Error allocating memory");
                    exit(EXIT_FAILURE);
                }
                make_file_path(file_name, args[0], args[1]); //format file name correctly

                // file stuff
                FILE *fp = NULL;
                fp = fopen(file_name, "r");
                if(fp == NULL){
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }
                char *end = "-1-1";
                printf("--- Sending file ---\n");
                while(1){
                    if(give_forty(client_socket, fp) == 1){
                        write(client_socket, end, sizeof(end));
                        printf("\n");
                        break;
                    }
                    // just wait till client sends something. (asking for another 40 lines)
                    read(client_socket, client_request, sizeof(client_request));
                }

                fclose(fp);
                free(file_name);
            } else {
                write(client_socket, "1", sizeof(char));
                perror("Error opening directory");
            }
        }
        else{
            perror("Error, client command not defined.");
            exit(EXIT_FAILURE);
        }
    }
}


void make_file_path(char *file_name, char *arg0, char *arg1){
    //arg0[strlen(arg0) - 1] = '\0'; //getting ride of the '\n' for the file name
    char path[BUFF_SIZE] = "/";
    strcat(path, arg0);
    strcat(path, "/");
    strcat(path, arg1); // path is now "/[dir]/[file]"

    if(getcwd(file_name, BUFF_SIZE) != NULL){
        strcat(file_name, path); // cwd is now "[current_directory]/[dir]/[file]"
    } else {
        perror("Error getting current working directory");
        exit(EXIT_FAILURE);
    }
    file_name[strlen(file_name) - 1] = '\0';
}

// prints 40 lines of file. returns 1 if EOF. otherwise 0
int give_forty(int client_socket, FILE* fp){
    char line[BUFF_SIZE];
    for(int i = 0; i < 40; i++){
        if(fgets(line, sizeof(line), fp) != NULL){
            write(client_socket, line, sizeof(line));
        } else {
            return 1;
        }
    }
    return 0;
}
