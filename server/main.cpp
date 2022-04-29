#include "server.hpp"
#include "server.cpp"


int main() {
    struct stat st = {0};
    server server;

    server.set_up();
    server.run();
}

// TODO updating this to cpp later

// void make_file_path(char *file_name, char *arg0, char *arg1){

//     strcat(file_name, arg0);
//     strcat(file_name, "/");
//     strcat(file_name, arg1); // path is now "[dir]/[file]"

//     file_name[strlen(file_name) - 1] = '\0';
// }

// // prints 40 lines of file. returns 1 if EOF. otherwise 0
// int give_forty(int client_socket, FILE* fp){
//     char line[BUFF_SIZE];
//     for(int i = 0; i < 40; i++){
//         if(fgets(line, sizeof(line), fp) != NULL){
//             write(client_socket, line, sizeof(line));
//         } else {
//             return 1;
//         }
//     }
//     return 0;
// }

// void ensure_compiled(char *arg0, struct stat st){
//     std::shared_ptr<std::string> file_name = "ls -t " + static_cast<std::string>(arg0);
//     std::shared_ptr<std::string> buffer;

//     if(file_name == nullptr){
//         perror("Failed allocating memory for file name.");
//         exit(EXIT_FAILURE);
//     }

//     // list by last edit to find if the executable is up to date.
//     file_name = "ls -t " + static_cast<std::string>(arg0);

//     FILE * fp = NULL;

//     if((fp = popen(file_name, "r"))== NULL){
//         perror("Error");
//         exit(EXIT_FAILURE);
//     }

//     //grab top of list (newest file) to see if its the exe.
//     fgets(buff, BUFF_SIZE, fp);
//     //remove buffs trailing \n;
//     buff[strlen(buff) - 1] = '\0';

//     pclose(fp);
//     memset(file_name, '\0', strlen(file_name));

//     //make path for executable to check if it exists.
//     char * comp_check = strdup(arg0);
//     strcat(comp_check, "/");
//     strcat(comp_check, arg0);

//     // if we have no exe OR the exe is older than a source file
//     if(stat(comp_check, &st) == -1 || strcmp(buff, arg0) != 0){
//         printf("--- compiling ---\n");
//         // making file_name = "cc [dir]/[file].c -o [dir]/out
//         // essentially compiling the file inside the dir, and putting the exe in the dir.
//         strcat(file_name, "cc ");
//         strcat(file_name, arg0);
//         strcat(file_name, "/*.c");
//         strcat(file_name, " -o ");
//         strcat(file_name, arg0);
//         strcat(file_name, "/");
//         strcat(file_name, arg0);
//         popen(file_name, "r");
//         free(file_name);
//     }
//     else {
//         // we get here if the exe exists already and its younger than the source files.
//         printf("--- executable up to date ---\n");
//     }
//     free(buff);
// }

// void kill_zombie(int sig){
//     int status;
//     printf("--- looking for zombie processes ---\n");
//     if(waitpid(-1, &status, WNOHANG) < 0){
//         printf("--- zombie process found and removed ---\n");
//     } else {
//         printf("--- no zombie processes found ---\n");
//     }

// }
