#include "header.h"


int main(){
    int server_socket;

    struct sockaddr_in server_address;
    char *message = "Hello from client";
    char user_input[BUFF_SIZE], server_reply[BUFF_SIZE];

    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("ERROR: Failed to create socket");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUM);
    server_address.sin_addr.s_addr = inet_addr("192.168.0.34");

     /*
    if(inet_pton(AF_INET, "192.168.0.34", &server_address.sin_addr) <= 0){
        perror("ERROR: Invalid address");
        exit(EXIT_FAILURE);
    }*/

    if(connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("ERROR: Failed to connected.");
        exit(EXIT_FAILURE);
    }


    while(1){
        // clearing buffers
        memset(user_input, '\0', sizeof(user_input));
        memset(server_reply, '\0', sizeof(server_reply));

        printf("> ");
        scanf("%s", user_input);

        if(strcmp(user_input, "time") == 0){
            write(server_socket, user_input, sizeof(user_input));
            printf("Sent: %s\n", user_input);

            read(server_socket, server_reply, sizeof(server_reply));
            printf("Reply: %s\n", server_reply);
        }
        else if(strcmp(user_input, "quit") == 0) {
            write(server_socket, user_input, sizeof(user_input));
            close(server_socket);
            return 0;
        }
        else{
            write(server_socket, user_input, sizeof(user_input));
            printf("Sent: %s\n", user_input);
        }
    }
}
