#include "client_header.h"


int main(int argc, char *argv[]){
    ////////
    //TODO make ip address be entered from arguments.
    ///////
    int server_socket;
    struct sockaddr_in server_address;
    char *user_input, *server_reply;
    user_input = (char*)malloc(BUFF_SIZE * sizeof(char));
    server_reply = (char*)malloc(BUFF_SIZE * sizeof(char));

    // check if the user entered the IP address of the server.
    if(argc < 2){
        printf("ERROR: Too few arguments entered. IP address needed.\n");
        exit(1);
    }

    // create the socket
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("ERROR: Failed to create socket\n");
        exit(1);
    }
    printf("--- Socket created ---\n");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUM);
    server_address.sin_addr.s_addr = inet_addr("192.168.0.34");

    if(connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        printf("ERROR: Failed to connected.\n");
        exit(1);
    }
    printf("--- Connected to server ---\n");

    while(1){
        // clearing buffers
        memset(user_input, '\0', strlen(user_input));
        memset(server_reply, '\0', strlen(server_reply));

        // prompting user for input
        printf("> ");
        fgets(user_input, BUFF_SIZE, stdin);

        // getting the command from the user (the first word).
        char *user_input_split; // this is so user_input doesnt get edited by strtok.
        char *token;
        strcpy(user_input_split, user_input);
        token = strtok(user_input_split, " ");

        if(strcmp(token, "put") == 0){
            //start timing
            clock_t begin = clock();

            // request from server
            write(server_socket, user_input, BUFF_SIZE);
            printf("Sent: %s\n", user_input);


            // server replies 0 if directory created, 1 if directory already exists.
            read(server_socket, server_reply, sizeof(server_reply));
            if(strcmp(server_reply, "0") == 0){
                FILE *fp = NULL;
                fp = open()
            }
            printf("%s", server_reply);
            // stop timing
            clock_t end = clock();
            double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

            // display result and time taken
            // printf("Reply: %s\n", server_reply);
            printf("Time Taken: %lf seconds\n", time_spent);
        }
        else if(strcmp(token, "quit\n") == 0) {
            write(server_socket, user_input, sizeof(user_input));
            close(server_socket);
            free(user_input);
            free(server_reply);
            return 0;
        }
        else{
            printf("ERROR: %s is not defined.\n", token);
        }
    }
}
