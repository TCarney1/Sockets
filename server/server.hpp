//
// Created by Tom Carney on 28/8/20.
//


#ifndef SOCKETS_HEADER_H
#define SOCKETS_HEADER_H


#define PORT_NUM 80
#define BUFF_SIZE 1024
#define BACKLOG 10


#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <memory>
#include <string>
#include <vector>

struct server {
private:
    int server_socket;
    int client_socket;
    int num_args = 10;
    pid_t cpid;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    char *client_request;
    char *server_reply;

    void list();
    void run_command();
    void get();
    void put();

public:
    server();
    server(int server_socket, int client_socket);
    void set_up();
    void run();

} typedef server;


void make_file_path(char *file_name, char *arg0, char *arg1);
int give_forty(int client_socket, FILE* fp);
void ensure_compiled(char *arg0, struct stat st);
void kill_zombie(int sig);




#endif //SOCKETS_HEADER_H
