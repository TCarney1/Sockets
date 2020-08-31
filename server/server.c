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


    int num_args = 10;
    char **args = NULL;
    args = malloc(num_args * (sizeof(char *)));
    if(args == NULL){
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }
    for (int i=0; i<num_args; i++) {
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
        memset(args, '\0', (num_args*BUFF_SIZE) * (sizeof *args));

        // get whole line of client request.
        read(client_socket, client_request, BUFF_SIZE);
        printf("Received: %s", client_request);

        // break client request into [arg1] [arg2] [arg3]...
        char *token;
        token = strtok(client_request, " \n");

        // check for quit request
        if(strcmp(client_request, "quit") == 0){
            free(client_request);
            free(server_reply);
            close(client_socket);
            close(server_socket);
            for(int i = 0; i < num_args; i++){
                free(args[i]);
            }
            free(args);
            return 0;
        }

        // still breaking input string into sections
        int arg_count = 0;
        while(token != NULL){
            token = strtok(NULL, " ");
            args[arg_count] = token;
            arg_count++;
        }
        int last_index = arg_count - 2;

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
        // sends contents of file to client
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
        // puts client's file on server
        else if(strcmp(client_request, "run") == 0) {

            printf("--- Run request ---\n");
            //removing excess '\n' from single argument cases.
            if(last_index < 1){
                args[0][strlen(args[0]) - 1] = '\0';
            }

            // check if directory exists.
            if(stat(args[0], &st) >= 0){
                char *file_name = malloc(BUFF_SIZE * sizeof(char));
                char *exe_path = malloc(BUFF_SIZE * sizeof(char));
                if(file_name == NULL){
                    perror("Error allocating memory");
                    exit(EXIT_FAILURE);
                }
                //check if file has been compiled
                ensure_compiled(file_name, args[0], args[1], st);
                strcat(exe_path, "./");
                strcat(exe_path, args[0]);
                strcat(exe_path, "/out");
                //execl(exe_path, NULL);
                char *a[]={"./out",NULL};
                execvp(a[0],a);
                // if no executable is in file, make one.
                // if executable is older than last edit on file compile it.
                // run executable with all command line args.
                // give program output to client.
                free(file_name);
                free(exe_path);

            } else {
                perror("Error opening program file");
            }
        }
        else if(strcmp(client_request, "list") == 0){
            bool l_flag = strcmp(args[last_index - 1], "-l") == 0;
            // need to finish this
        }
        else{
            perror("Error, client command not defined.");
            exit(EXIT_FAILURE);
        }
    }
}


void make_file_path(char *file_name, char *arg0, char *arg1){

    strcat(file_name, arg0);
    strcat(file_name, "/");
    strcat(file_name, arg1); // path is now "[dir]/[file]"

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

void ensure_compiled(char *file_name, char *arg0, char *arg1, struct stat st){
    char * comp_check = strdup(arg0);
    strcat(comp_check, "/out");
    if(stat(comp_check, &st) == -1){
        // making file_name = "cc [dir]/[file].c -o [dir]/out]
        // essentially compiling the file inside the dir, and putting the exe in the dir.
        strcat(file_name, "cc ");
        make_file_path(file_name, arg0, arg1);
        strcat(file_name, " -o ");
        strcat(file_name, arg0);
        strcat(file_name, "/");
        strcat(file_name, "out");


        FILE* fp = popen(file_name, "w");
        if(fp == NULL){
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        fclose(fp);
    }
    else {
        perror("Error executable exists\n");
        exit(EXIT_FAILURE);
    }
}
