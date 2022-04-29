#include "client.hpp"


client::client(){
    client(0);
}

client::client(int server_socket_){
    server_socket = server_socket_;
}

void client::connect_to_server(const char* ip){
    // create the socket
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    printf("--- Socket created ---\n");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUM);
    server_address.sin_addr.s_addr = inet_addr(ip);

    if(connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("Error connecting");
        exit(EXIT_FAILURE);
    }
    std::cout << "--- Connected to server ---" << std::endl;
}


void client::run(){
    std::vector<std::string> args;
    std::string user_input;
    std::string server_reply;

    while(1){
        // clearing buffers
        args.clear();
        user_input.clear();
        server_reply.clear();

        // prompting user for input
        printf("> ");
        std::cin >> user_input;

        // quits the client and server when "quit" is entered
        if(user_input == "quit") {
            // TODO tell server to quit here.
            //write(server_socket, static_cast<const void*>(user_input), sizeof(user_input));
            close(server_socket);
            return;
        }

        // puts file on server. file [dir] [file]
        if(user_input == "put"){
            put();
        }
        // reads file from server get [dir] [file]
        else if(user_input == "get"){
            get();
        }
        // prints the clients OS and CPU information
        else if(user_input == "put"){
            sys();
        }
        // compiles and runs files on the server. run [dir][*optional* args][*optional*-f localfile]
        else if(user_input == "run"){
            run();
        }
        // lists contents of server. list [*optional* -l][*optional* dir]
        else if(user_input == "list"){
            list();
        }
        else{
            std::cerr << "Error: " << user_input << " is not defined." << std::endl;
        }
    }
}

void client::put(){
    std::cout << "--- put ---" << std::endl;
    // // if put is called with less than 2 args its not valid.
    // if(last_index < 1){
    //     printf("Error: too few arguments entered for put\n");
    // } else {
    //     //start timing
    //     clock_t begin = clock();

    //     // put request from server
    //     write(server_socket, user_input, BUFF_SIZE);
    //     printf("Sent: %s\n", user_input);
    //     // server replies 0 if directory created, 1 if directory already exists (and no flag).
    //     read(server_socket, server_reply, sizeof(server_reply));
    //     bool overwrite = (strcmp(args[last_index], "-f")) == 0;
    //     if (strcmp(server_reply, "0") == 0 || overwrite == true) {
    //         if(overwrite == false){
    //             last_index++;
    //         }
    //         for(int i = 1; i < last_index; i++){
    //             FILE *fp = NULL;
    //             fp = fopen(args[i], "r");
    //             if (fp == NULL) {
    //                 perror("Error opening file");
    //                 exit(EXIT_FAILURE);
    //             }
    //             char *end = "-1-1";
    //             char *buf = (char*)malloc(sizeof(char) *BUFF_SIZE);
    //             memset(buf, '\0', strlen(buf));
    //             while (fgets(buf, sizeof(buf), fp) != NULL) {
    //                 write(server_socket, buf, sizeof(buf));
    //             }
    //             write(server_socket, end, sizeof(buf));
    //             fclose(fp);
    //         }
    //     } else {
    //         printf("Error directory already exists, use -f to replace\n");
    //     }
    //     // stop timing
    //     clock_t end = clock();
    //     double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;

    //     // display result and time taken
    //     printf("Time Taken: %lf seconds\n", time_spent);
    // }
}

void client::get(){
    std::cout << "--- get ---" << std::endl;
    // if(last_index < 1){
    //     printf("Error: too few arguments entered for get\n");

    // }else {
    //     // request get from server
    //     write(server_socket, user_input, BUFF_SIZE);
    //     printf("Sent: %s\n", user_input);
    //     // server replies 0 if directory exists, 1 if directory doesn't exists.
    //     read(server_socket, server_reply, sizeof(server_reply));

    //     if (strcmp(server_reply, "0") == 0) {
    //         //start timing
    //         clock_t begin = clock();

    //         // start reading from the server.
    //         while (1) {
    //             memset(server_reply, '\0', strlen(server_reply));
    //             if (print_forty(server_socket) == 1) {
    //                 printf("\n--- End of file ---\n");
    //                 break;
    //             } else {
    //                 printf("--- Press any key to continue ---\n");
    //                 getchar();
    //                 write(server_socket, user_input, sizeof(user_input));
    //             }
    //         }

    //         clock_t end = clock();
    //         double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;

    //         // display result and time taken
    //         printf("Time Taken: %lf seconds\n", time_spent);

    //     } else {
    //         perror("Error no directory found");
    //     }
    // }
}

void client::sys(){
    std::cout << "--- sys ---" << std::endl;
    // if(TYPE == 0){
    //     printf("--- System: Windows ---\n");
    // } else if (TYPE == 1){
    //     printf("--- System: Mac ---\n");
    // } else if (TYPE == 2){
    //     printf("--- System: Unix ---\n");
    // } else {
    //     printf("--- System: Unknown ---\n");
    // }

    // char cpu_info[BUFF_SIZE];
    // size_t size = BUFF_SIZE;
    // if(sysctlbyname("machdep.cpu.brand_string", &cpu_info, &size, NULL, 0) < 0){
    //     perror("Error gathering CPU information");
    //     exit(EXIT_FAILURE);
    // }
    // printf("--- CPU: %s ---\n", cpu_info);
}


void client::run_command(){
    std::cout << "--- run ---" << std::endl;
    // if(last_index < 0){
    //     printf("Error: too few arguments entered for run\n");

    // }else {
    //     bool loc_file, f_flag = false;

    //     // check if last argument is a file name (for optional localfile arg)
    //     loc_file = (strchr(args[last_index], '.')) != NULL;
    //     if (loc_file == true) {

    //         if (last_index < 1) {
    //             perror("Error performing 'run', too few arguments");
    //             exit(1);
    //         }
    //         // check for -f flag (-f flag wont exists if localfile doesnt.)
    //         f_flag = strcmp(args[last_index - 1], "-f") == 0;
    //     }

    //     // if localfile doesn't exist or we have -f
    //     if (access(args[last_index], F_OK) == -1 || f_flag == true) {
    //         clock_t begin = clock();
    //         // if file doesn't exists OR we have -f flag
    //         // request get from server
    //         write(server_socket, user_input, BUFF_SIZE);
    //         printf("Sent: %s\n", user_input);
    //         // server will reply with program output (or program error).
    //         // if loc_file == true print to localfile, else print to stdout.
    //         if(f_flag || loc_file){
    //             // if client wants output put into a file
    //             FILE *fp = fopen(args[last_index], "w");
    //             while(1){
    //                 memset(server_reply, '\0', strlen(server_reply));
    //                 read(server_socket, server_reply, BUFF_SIZE);
    //                 if(strcmp(server_reply, "-1-1") == 0){
    //                     break;
    //                 }
    //                 fprintf(fp,"%s", server_reply);
    //             }
    //             fclose(fp);
    //         } else {
    //             // if client wants output printed.
    //             while(1){
    //                 memset(server_reply, '\0', strlen(server_reply));
    //                 read(server_socket, server_reply, BUFF_SIZE);
    //                 if(strcmp(server_reply, "-1-1") == 0){
    //                     break;
    //                 }
    //                 printf("%s",server_reply);
    //             }
    //         }
    //         clock_t end = clock();
    //         double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;

    //         // display result and time taken
    //         printf("Time Taken: %lf seconds\n", time_spent);

    //     } else {
    //         // if file exists and we dont have -f flag.
    //         printf("Error, directory already exists. No -f flag present.\n");
    //     }
    // }
}

void client::list(){
    //start timing
    std::cout << "--- list ---" << std::endl;

    // clock_t begin = clock();
    // write(server_socket, user_input, BUFF_SIZE);
    // while(1){
    //     memset(server_reply, '\0', strlen(server_reply));
    //     read(server_socket, server_reply, BUFF_SIZE);
    //     if(strcmp(server_reply, "-1-1") == 0){
    //         break;
    //     }
    //     printf("%s", server_reply);
    // }

    // // stop timing
    // clock_t end = clock();
    // double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;

    // // display result and time taken
    // printf("Time Taken: %lf seconds\n", time_spent);
}