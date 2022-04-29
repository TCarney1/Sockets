#include "server.hpp"


server::server(){
    server(0, 0);
}

server::server(int server_socket_, int client_socket_){
    server_socket = server_socket_;
    client_socket = client_socket_;
}

void server::set_up(){
    //signal(SIGCHLD, kill_zombie);

    // creating socket for server.
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    std::cout << "--- Socket created ---" << std::endl;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT_NUM);

    // binding socket
    if(bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "--- Bind successful ---" << std::endl;

    // BACKLOG is the maximum number of outstanding requests
    // listening for clients to connect.
    if(listen(server_socket, BACKLOG) < 0){
        perror("Error listening to socket");
        exit(EXIT_FAILURE);
    }
}

void server::run(){
    std::cout << "--- Waiting for connection ---" << std::endl;;
        // accept multiple clients
    while(1){
        std::string client_request;
        std::string server_reply;

        int address_len = sizeof(client_address);
        if((client_socket = accept(server_socket, (struct sockaddr *) &client_address, (socklen_t *) &address_len)) < 0){
            perror("Error accepting client");
            exit(EXIT_FAILURE);
        }
        std::cout << "--- Client connected ---" << std::endl;
        // creating child process
        if((cpid = fork()) < 0){
            perror("Error forking.");
            exit(1);
        }
        // if child
        else if (cpid == 0){
            // memory alloc to split user input into args.
            // just makes input more manageable
            std::vector<std::string> args;

            close(server_socket);
            while(1){
                // clearing buffers
                client_request.clear();
                server_reply.clear();
                args.clear();

                // get whole line of client request.
                // read(client_socket, client_request, BUFF_SIZE);
                std::cout << "Received: " << client_request << std::endl;

                // check for quit request
                if(client_request == "quit"){
                    std::cout << std::endl;
                    exit(EXIT_SUCCESS);
                }

                // puts client's file on server
                if(client_request == "put") {
                    put();
                }
                // sends contents of file to client
                else if(client_request == "get"){
                    get();
                }
                // puts client's file on server
                else if(client_request == "run") {
                    run_command();
                }
                // lists contents of server
                else if(client_request == "list"){
                    list();
                }
            }
        }
    }
}

void server::put(){
    std::cout << "--- Put request ---" << std::endl;

    // bool replace_dir = (strcmp(args[last_index -1], "-f\n")) == 0;
    // // check if directory exists or -f flag
    // if(stat(args[0], &st) == -1 || replace_dir == true){
    //     write(client_socket, "0", sizeof(char));
    //     // -f flag means replace dir.
    //     if(stat(args[0], &st) != -1 && replace_dir == true){
    //         char *rm = (char*)malloc(sizeof(char) * BUFF_SIZE);
    //         memset(rm, '\0', strlen(rm));
    //         strcat(rm, "rm ");
    //         strcat(rm, args[0]);
    //         strcat(rm, "/*");
    //         FILE* f = popen(rm, "r");
    //         printf("--- Dir replaced ---\n");
    //         pclose(f);
    //         free(rm);
    //         // we dont want to make a file for the flag. so reduce index by 1.
    //         last_index--;
    //     }else {
    //         //this if else is to handle if the user puts in an unnecessary -f.
    //         if(replace_dir == true){
    //             last_index--;
    //         } else {
    //             args[last_index-1][strlen(args[last_index-1]) - 1] = '\0';
    //         }
    //         // make directory because it doesnt exist.
    //         mkdir(args[0], 0777);
    //         printf("--- Directory Created ---\n");
    //         // if we dont have a flag on the end, we need to remove the '\n' on the last arg

    //     }

    //     char *file_name = malloc(BUFF_SIZE * sizeof(char));
    //     if(file_name == NULL){
    //         perror("Error allocating memory");
    //         exit(EXIT_FAILURE);
    //     }
    //     char *dir = strdup(args[0]);

    //     for(int i = 1; i < last_index; i++) {
    //         memset(file_name, '\0', BUFF_SIZE);
    //         memset(file_name, '\0', BUFF_SIZE);

    //         // makes 'file_name' = [current directory]/[dir]/[file]
    //         strcat(file_name, dir);
    //         strcat(file_name, "/");
    //         strcat(file_name, args[i]);

    //         FILE *fp = fopen(file_name, "w");
    //         if (fp == NULL) {
    //             perror("Error opening file");
    //             exit(EXIT_FAILURE);
    //         }
    //         printf("--- File opened ---\n");
    //         // file copying
    //         while (1) {
    //             read(client_socket, client_request, sizeof(client_request));
    //             if (strcmp(client_request, "-1-1") == 0) {
    //                 break;
    //             }
    //             fputs(client_request, fp);
    //         }
    //         fclose(fp);
    //     }
    //     free(file_name);
    //     printf("--- copy complete ---\n");

    // } else {
    //     printf("Error directory already exists. Use -f flag to replace directory.\n");
    //     write(client_socket, "1", sizeof(char));

    // }
}

void server::get(){
    std::cout << "--- get request ---" << std::endl;
    // // check if directory exists.
    // if(stat(args[0], &st) >= 0){
    //     printf("--- Directory Found ---\n");
    //     write(client_socket, "0", sizeof(char)); // tell the client we found dir
    //     char *file_name = malloc(BUFF_SIZE * sizeof(char));
    //     if(file_name == NULL){
    //         perror("Error allocating memory");
    //         exit(EXIT_FAILURE);
    //     }
    //     make_file_path(file_name, args[0], args[1]); //format file name correctly
    //     // file stuff
    //     FILE *fp = NULL;
    //     fp = fopen(file_name, "r");
    //     if(fp == NULL){
    //         perror("Error opening file");
    //         exit(EXIT_FAILURE);
    //     }
    //     char *end = "-1-1";
    //     printf("--- Sending file ---\n");
    //     while(1){
    //         if(give_forty(client_socket, fp) == 1){
    //             write(client_socket, end, sizeof(end));
    //             printf("\n");
    //             break;
    //         }
    //         // just wait till client sends something. (asking for another 40 lines)
    //         read(client_socket, client_request, sizeof(client_request));
    //     }
    //     printf("--- File sent ---\n");

    //     fclose(fp);
    //     free(file_name);
    // } else {
    //     write(client_socket, "1", sizeof(char));
    // }
}

void server::run_command(){
    std::cout << "--- run request ---" << std::endl;

    // //removing excess '\n' from single argument cases.
    // if(last_index == 1){
    //     args[0][strlen(args[0]) - 1] = '\0';

    // }
    // // check if directory exists.
    // if(stat(args[0], &st) >= 0){
    //     char *end = "-1-1";
    //     char *exe_path = malloc(BUFF_SIZE * sizeof(char));
    //     memset(exe_path, '\0', strlen(exe_path));
    //     char *file_buffer = malloc(BUFF_SIZE * sizeof(char));
    //     ensure_compiled(args[0], st);//check if file has been compiled
    //     //making exe_path = ./[dir]/[exe] [args1]...
    //     strcat(exe_path, "./");
    //     strcat(exe_path, args[0]);
    //     strcat(exe_path,"/");
    //     // adding args.
    //     for(int i = 0; i < last_index; i++){
    //         strcat(exe_path, args[i]);
    //         strcat(exe_path, " ");
    //     }
    //     wait(NULL); // need to wait for the popen(compiling) before popen(running).
    //     FILE *fp = popen(exe_path, "r");
    //     if(fp == NULL){
    //         perror("Error");
    //         exit(EXIT_FAILURE);
    //     }

    //     // send program output to client.
    //     while((fgets(file_buffer, BUFF_SIZE, fp)) != NULL){
    //         write(client_socket, file_buffer, BUFF_SIZE);
    //     }
    //     write(client_socket, end, BUFF_SIZE);

    //     free(exe_path);
    //     free(file_buffer);
    //     pclose(fp);

    // } else {
    //     perror("Error opening program file");
    // }
}

void server::list(){
    std::cout << "--- list request ---" << std::endl;
    // // check for -l flag.
    // printf("--- Getting list ---\n");

    // char *command = (char*)malloc(sizeof(char) * BUFF_SIZE);
    // char *end = "-1-1";
    // memset(server_reply, '\0', strlen(server_reply));
    // memset(command, '\0', strlen(command));
    // if(command == NULL){
    //     perror("Failed allocating memory");
    //     exit(EXIT_FAILURE);
    // }
    // //if no args are given, we just want ls.
    // strcat(command, "ls ");
    // if(last_index > 0){
    //     // if args are given we want to ls [-f] [dir]
    //     for(int i = 0; i < last_index; i++){
    //         strcat(command, args[i]);
    //         strcat(command, " ");
    //     }
    // }

    // FILE* fp = popen(command, "r");
    // if(fp == NULL){
    //     perror("Error");
    //     exit(EXIT_FAILURE);
    // }

    // printf("--- Sending list ---\n");
    // // send ls to client.
    // while((fgets(server_reply, BUFF_SIZE, fp)) != NULL){
    //     write(client_socket, server_reply, BUFF_SIZE);
    // }
    // write(client_socket, end, BUFF_SIZE);
    // printf("--- list sent ---\n");

    // pclose(fp);
    // free(command);
    // }
    // else{
    // // big issues if we get here... client shouldn't send anything
    // // that the server wont understand.
    // perror("Error, client command not defined.");
    // exit(EXIT_FAILURE);
    // }
}