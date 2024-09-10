#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <signal.h> // for handling SIGCHLD
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

// Function designed for chat between client and server. 
void func(int connfd) 
{ 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 

        // read the message from client and copy it in buffer 
        read(connfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        while ((buff[n++] = getchar()) != '\n') 
            ; 

        // and send that buffer to client 
        write(connfd, buff, sizeof(buff)); 

        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
} 

// Signal handler to reap zombie processes
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Driver function 
int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 

    // Install SIGCHLD handler to avoid zombie processes
    signal(SIGCHLD, sigchld_handler);

    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("Socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 

    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 

    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 

    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 

    // Infinite loop to accept multiple clients 
    while (1) {
        // Accept the data packet from client and verification 
        connfd = accept(sockfd, (SA*)&cli, &len); 
        if (connfd < 0) { 
            printf("Server accept failed...\n"); 
            exit(0); 
        } 
        else
            printf("Server accepted the client...\n"); 

        // Fork a child process to handle the client 
        if (fork() == 0) { 
            // Child process 
            close(sockfd); // Close the listening socket in the child process
            func(connfd); 
            close(connfd); 
            exit(0); 
        } 
        else {
            close(connfd); // Parent process closes the connected socket
        }
    }

    // After chatting close the socket 
    close(sockfd); 
}
