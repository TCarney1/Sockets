#include "client.hpp"
#include "client.cpp"

// argv[1] is ip address of server.
int main(int argc, char *argv[]){

    // check if the user entered the IP address of the server.
    if(argc < 2){
        perror("Error too few arguments entered");
        exit(EXIT_FAILURE);
    }

    client client;

    client.connect_to_server(argv[1]);
    client.run();

    return 0;
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
