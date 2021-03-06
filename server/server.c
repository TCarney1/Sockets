#include "server_header.h"


int main() {
    struct stat st = {0};
    int server_socket = 0, client_socket = 0;
    struct sockaddr_in server, client;
    char *client_request, *server_reply;
    pid_t cpid;
    int num_args = 10;

    signal(SIGCHLD, kill_zombie);


    // creating socket for server.
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    printf("--- Socket created ---\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUM);

    // binding socket
    if(bind(server_socket, (struct sockaddr *) &server, sizeof(server)) < 0){
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    printf("--- Bind successful ---\n");

    // BACKLOG is the maximum number of outstanding requests
    // listening for clients to connect.
    if(listen(server_socket, BACKLOG) < 0){
        perror("Error listening to socket");
        exit(EXIT_FAILURE);
    }
    printf("--- Waiting for connection ---\n");

    // accept multiple clients
    while(1){
        client_request = (char*)malloc(BUFF_SIZE * sizeof(char));
        server_reply = (char*)malloc(BUFF_SIZE * sizeof(char));
        if(client_request == NULL || server_reply == NULL){
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        int address_len = sizeof(client);
        if((client_socket = accept(server_socket, (struct sockaddr *) &client, (socklen_t *) &address_len)) < 0){
            perror("Error accepting client");
            exit(EXIT_FAILURE);
        }
        printf("--- Client connected ---\n");
        // creating child process
        if((cpid = fork()) < 0){
            perror("Error forking.");
            exit(1);
        }
        // if child
        else if (cpid == 0){
            // memory alloc to split user input into args.
            // just makes input more manageable
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
            close(server_socket);
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
                    printf("\n");
                    free(client_request);
                    free(server_reply);
                    close(client_socket);
                    for(int i = 0; i < num_args; i++){
                        free(args[i]);
                    }
                    free(args);
                    exit(EXIT_SUCCESS);
                }

                // still breaking input string into sections
                int arg_count = 0;
                while(token != NULL){
                    token = strtok(NULL, " ");
                    args[arg_count] = token;
                    arg_count++;
                }
                int last_index = arg_count - 1;

                // puts client's file on server
                if(strcmp(client_request, "put") == 0) {
                    printf("--- Put request ---\n");

                    bool replace_dir = (strcmp(args[last_index -1], "-f\n")) == 0;
                    // check if directory exists or -f flag
                    if(stat(args[0], &st) == -1 || replace_dir == true){
                        write(client_socket, "0", sizeof(char));
                        // -f flag means replace dir.
                        if(stat(args[0], &st) != -1 && replace_dir == true){
                            char *rm = (char*)malloc(sizeof(char) * BUFF_SIZE);
                            memset(rm, '\0', strlen(rm));
                            strcat(rm, "rm ");
                            strcat(rm, args[0]);
                            strcat(rm, "/*");
                            FILE* f = popen(rm, "r");
                            printf("--- Dir replaced ---\n");
                            pclose(f);
                            free(rm);
                            // we dont want to make a file for the flag. so reduce index by 1.
                            last_index--;
                        }else {
                            //this if else is to handle if the user puts in an unnecessary -f.
                            if(replace_dir == true){
                                last_index--;
                            } else {
                                args[last_index-1][strlen(args[last_index-1]) - 1] = '\0';
                            }
                            // make directory because it doesnt exist.
                            mkdir(args[0], 0777);
                            printf("--- Directory Created ---\n");
                            // if we dont have a flag on the end, we need to remove the '\n' on the last arg

                        }

                        char *file_name = malloc(BUFF_SIZE * sizeof(char));
                        if(file_name == NULL){
                            perror("Error allocating memory");
                            exit(EXIT_FAILURE);
                        }
                        char *dir = strdup(args[0]);

                        for(int i = 1; i < last_index; i++) {
                            memset(file_name, '\0', BUFF_SIZE);
                            memset(file_name, '\0', BUFF_SIZE);

                            // makes 'file_name' = [current directory]/[dir]/[file]
                            strcat(file_name, dir);
                            strcat(file_name, "/");
                            strcat(file_name, args[i]);

                            FILE *fp = fopen(file_name, "w");
                            if (fp == NULL) {
                                perror("Error opening file");
                                exit(EXIT_FAILURE);
                            }
                            printf("--- File opened ---\n");
                            // file copying
                            while (1) {
                                read(client_socket, client_request, sizeof(client_request));
                                if (strcmp(client_request, "-1-1") == 0) {
                                    break;
                                }
                                fputs(client_request, fp);
                            }
                            fclose(fp);
                        }
                        free(file_name);
                        printf("--- copy complete ---\n");

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
                        printf("--- File sent ---\n");

                        fclose(fp);
                        free(file_name);
                    } else {
                        write(client_socket, "1", sizeof(char));
                    }
                }
                // puts client's file on server
                else if(strcmp(client_request, "run") == 0) {
                    printf("--- Run request ---\n");

                    //removing excess '\n' from single argument cases.
                    if(last_index == 1){
                        args[0][strlen(args[0]) - 1] = '\0';

                    }
                    // check if directory exists.
                    if(stat(args[0], &st) >= 0){
                        char *end = "-1-1";
                        char *exe_path = malloc(BUFF_SIZE * sizeof(char));
                        memset(exe_path, '\0', strlen(exe_path));
                        char *file_buffer = malloc(BUFF_SIZE * sizeof(char));
                        ensure_compiled(args[0], st);//check if file has been compiled
                        //making exe_path = ./[dir]/[exe] [args1]...
                        strcat(exe_path, "./");
                        strcat(exe_path, args[0]);
                        strcat(exe_path,"/");
                        // adding args.
                        for(int i = 0; i < last_index; i++){
                            strcat(exe_path, args[i]);
                            strcat(exe_path, " ");
                        }
                        wait(NULL); // need to wait for the popen(compiling) before popen(running).
                        FILE *fp = popen(exe_path, "r");
                        if(fp == NULL){
                            perror("Error");
                            exit(EXIT_FAILURE);
                        }

                        // send program output to client.
                        while((fgets(file_buffer, BUFF_SIZE, fp)) != NULL){
                            write(client_socket, file_buffer, BUFF_SIZE);
                        }
                        write(client_socket, end, BUFF_SIZE);

                        free(exe_path);
                        free(file_buffer);
                        pclose(fp);

                    } else {
                        perror("Error opening program file");
                    }
                }
                // lists contents of server
                else if(strcmp(client_request, "list") == 0){
                    // check for -l flag.
                    printf("--- Getting list ---\n");

                    char *command = (char*)malloc(sizeof(char) * BUFF_SIZE);
                    char *end = "-1-1";
                    memset(server_reply, '\0', strlen(server_reply));
                    memset(command, '\0', strlen(command));
                    if(command == NULL){
                        perror("Failed allocating memory");
                        exit(EXIT_FAILURE);
                    }
                    //if no args are given, we just want ls.
                    strcat(command, "ls ");
                    if(last_index > 0){
                        // if args are given we want to ls [-f] [dir]
                        for(int i = 0; i < last_index; i++){
                            strcat(command, args[i]);
                            strcat(command, " ");
                        }
                    }

                    FILE* fp = popen(command, "r");
                    if(fp == NULL){
                        perror("Error");
                        exit(EXIT_FAILURE);
                    }

                    printf("--- Sending list ---\n");
                    // send ls to client.
                    while((fgets(server_reply, BUFF_SIZE, fp)) != NULL){
                        write(client_socket, server_reply, BUFF_SIZE);
                    }
                    write(client_socket, end, BUFF_SIZE);
                    printf("--- list sent ---\n");

                    pclose(fp);
                    free(command);
                }
                else{
                    // big issues if we get here... client shouldn't send anything
                    // that the server wont understand.
                    perror("Error, client command not defined.");
                    exit(EXIT_FAILURE);
                }
            }
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

void ensure_compiled(char *arg0, struct stat st){
    char *file_name = malloc(BUFF_SIZE * sizeof(char));
    char *buff = malloc(BUFF_SIZE * sizeof(char));
    memset(file_name, '\0', strlen(file_name));

    if(file_name == NULL){
        perror("Failed allocating memory for file name.");
        exit(EXIT_FAILURE);
    }

    // list by last edit to find if the executable is up to date.
    strcat(file_name, "ls -t ");
    strcat(file_name, arg0);
    FILE * fp = NULL;

    if((fp = popen(file_name, "r"))== NULL){
        perror("Error");
        exit(EXIT_FAILURE);
    }

    //grab top of list (newest file) to see if its the exe.
    fgets(buff, BUFF_SIZE, fp);
    //remove buffs trailing \n;
    buff[strlen(buff) - 1] = '\0';

    pclose(fp);
    memset(file_name, '\0', strlen(file_name));

    //make path for executable to check if it exists.
    char * comp_check = strdup(arg0);
    strcat(comp_check, "/");
    strcat(comp_check, arg0);

    // if we have no exe OR the exe is older than a source file
    if(stat(comp_check, &st) == -1 || strcmp(buff, arg0) != 0){
        printf("--- compiling ---\n");
        // making file_name = "cc [dir]/[file].c -o [dir]/out
        // essentially compiling the file inside the dir, and putting the exe in the dir.
        strcat(file_name, "cc ");
        strcat(file_name, arg0);
        strcat(file_name, "/*.c");
        strcat(file_name, " -o ");
        strcat(file_name, arg0);
        strcat(file_name, "/");
        strcat(file_name, arg0);
        popen(file_name, "r");
        free(file_name);
    }
    else {
        // we get here if the exe exists already and its younger than the source files.
        printf("--- executable up to date ---\n");
    }
    free(buff);
}

void kill_zombie(int sig){
    int status;
    printf("--- looking for zombie processes ---\n");
    if(waitpid(-1, &status, WNOHANG) < 0){
        printf("--- zombie process found and removed ---\n");
    } else {
        printf("--- no zombie processes found ---\n");
    }

}
