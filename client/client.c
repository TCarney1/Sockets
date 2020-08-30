#include "client_header.h"

int main(int argc, char *argv[]){
    ////////
    //TODO make ip address be entered from arguments.
    ///////

    // check if the user entered the IP address of the server.
    if(argc < 2){
        perror("Error too few arguments entered");
        exit(EXIT_FAILURE);
    }

    int server_socket;
    struct sockaddr_in server_address;
    char *user_input, *server_reply;
    user_input = (char*)malloc(BUFF_SIZE * sizeof(char));
    server_reply = (char*)malloc(BUFF_SIZE * sizeof(char));

    if(user_input == NULL){
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    if(server_reply == NULL){
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

    for (int i=0; i<num_strings; i++){
        args[i] = malloc(sizeof(char) * BUFF_SIZE);
        if(args[i] == NULL){
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
    }

    // create the socket
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    printf("--- Socket created ---\n");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUM);
    server_address.sin_addr.s_addr = inet_addr("192.168.0.34");

    if(connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("Error connecting");
        exit(EXIT_FAILURE);
    }
    printf("--- Connected to server ---\n");

    while(1){
        // clearing buffers
        memset(user_input, '\0', strlen(user_input));
        memset(server_reply, '\0', strlen(server_reply));
        memset(args, '\0', (num_strings*BUFF_SIZE) * (sizeof *args));

        if(user_input == NULL || server_reply == NULL || args == NULL){
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }


        // prompting user for input
        printf("> ");
        fgets(user_input, BUFF_SIZE, stdin);

        // getting the command from the user (the first word).
        char *user_input_split;
        user_input_split = (char*)malloc(BUFF_SIZE * sizeof(char));
        strcpy(user_input_split, user_input);
        char *token;
        token = strtok(user_input_split, " ,\n");

        // quits the client and server when "quit" is entered
        if(strcmp(user_input_split, "quit") == 0) {
            write(server_socket, user_input_split, sizeof(user_input));
            close(server_socket);
            free(user_input);
            free(server_reply);
            for(int i = 0; i < num_strings; i++){
                free(args[i]);
            }
            free(args);
            free(user_input_split);
            return 0;
        }

        int arg_count = 0;
        while(token != NULL){
            token = strtok(NULL, " ,\n");
            args[arg_count] = token;
            args[arg_count] += '\0';
            arg_count++;
        }

        // puts file on server. file [dir] [file]
        if(strcmp(user_input_split, "put") == 0){
            //start timing
            clock_t begin = clock();

            // put request from server
            write(server_socket, user_input, BUFF_SIZE);
            printf("Sent: %s\n", user_input);
            // server replies 0 if directory created, 1 if directory already exists.
            read(server_socket, server_reply, sizeof(server_reply));

            if(strcmp(server_reply, "0") == 0){
                FILE *fp = NULL;
                fp = fopen(args[1], "r");
                if (fp == NULL) {
                    perror("Error opening file");
                    exit(EXIT_FAILURE);
                }
                char *end = "-1-1";
                char line[256];
                while(fgets(line, sizeof(line), fp) != NULL){
                    write(server_socket, line, sizeof(line));
                }
                write(server_socket, end, sizeof(end));
                fclose(fp);

            }
            else {
                perror("Error directory already exists");
            }
            // stop timing
            clock_t end = clock();
            double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

            // display result and time taken
            printf("Time Taken: %lf seconds\n", time_spent);
        }
        // reads file from server get [dir] [file]
        else if(strcmp(user_input_split, "get") == 0){
            // get request from server
            write(server_socket, user_input, BUFF_SIZE);
            printf("Sent: %s\n", user_input);
            // server replies 0 if directory exists, 1 if directory doesn't exists.
            read(server_socket, server_reply, sizeof(server_reply));

            if(strcmp(server_reply, "0") == 0){
                //start timing
                clock_t begin = clock();

                // start reading from the server.
                while(1){
                    memset(server_reply, '\0', strlen(server_reply));
                    if(print_forty(server_socket) == 1){
                        printf("\n--- End of file ---\n");
                        break;
                    }
                    else {
                        printf("--- Press any key to continue ---\n");
                        getchar();
                        write(server_socket, user_input, sizeof(user_input));
                    }
                }

                clock_t end = clock();
                double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

                // display result and time taken
                printf("Time Taken: %lf seconds\n", time_spent);

            } else {
                perror("Error directory does not exist");
            }
        }
        else if(strcmp(user_input_split, "sys") == 0){
            if(TYPE == 0){
                printf("--- System: Windows ---\n");
            } else if (TYPE == 1){
                printf("--- System: Mac ---\n");
            } else if (TYPE == 2){
                printf("--- System: Unix ---\n");
            } else {
                printf("--- System: Unknown ---\n");
            }

            char cpu_info[BUFF_SIZE];
            size_t size = BUFF_SIZE;
            if(sysctlbyname("machdep.cpu.brand_string", &cpu_info, &size, NULL, 0) < 0){
                perror("Error gathering CPU information");
                exit(EXIT_FAILURE);
            }
            printf("--- CPU: %s ---\n", cpu_info);
        }
        else{
            printf("Error: %s is not defined.\n", user_input_split);
        }
    }
}

// prints 40 lines from server. Returns 1 if file ends. Returns 0 if not.
int print_forty(int server_socket){
    char line[BUFF_SIZE];
    for(int i = 0; i < 40; i++){
        read(server_socket, line, sizeof(line));
        if(strcmp(line, "-1-1") == 0){
            return 1;
        }
        fputs(line, stdout);
    }
    return 0;
}
