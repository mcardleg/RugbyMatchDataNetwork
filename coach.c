#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

int max_clients, server_socket[4], i;
int socket1, socket2, socket3, socket4, connfd;
struct sockaddr_in serv1addr, serv2addr, serv3addr, serv4addr, cli;


void setup(){
    max_clients = 4;

    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        server_socket[i] = 0;
    }

    //create socket
    socket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket1 == -1) {
        printf("socket 1 creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
        bzero(&serv1addr, sizeof(serv1addr));

    serv1addr.sin_family = AF_INET;
    serv1addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv1addr.sin_port = htons(8080);

    // connect the client socket to server socket
    if (connect(socket1, (SA*)&serv1addr, sizeof(serv1addr)) != 0) {
        printf("connection with server 1 failed...\n");
        exit(0);
    }
    else
        printf("connected to server 1.\n");

    socket2 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket2 == -1) {
        printf("socket 2 creation failed...\n");
        exit(0);
    }
    else
        printf("Socket 2 successfully created..\n");
        bzero(&serv2addr, sizeof(serv2addr));
    serv2addr.sin_family = AF_INET;
    serv2addr.sin_addr.s_addr = inet_addr("127.0.0.2");
    serv2addr.sin_port = htons(8081);

    // connect the client socket to server socket
    if (connect(socket2, (SA*)&serv2addr, sizeof(serv2addr)) != 0) {
        printf("connection with server 2 failed...\n");
        exit(0);
    }
    else
        printf("connected to server 2.\n");

    socket3 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket3 == -1) {
        printf("socket 3 creation failed...\n");
        exit(0);
    }
    else
        printf("Socket 3 successfully created..\n");
        bzero(&serv3addr, sizeof(serv3addr));
    serv3addr.sin_family = AF_INET;
    serv3addr.sin_addr.s_addr = inet_addr("127.0.0.3");
    serv3addr.sin_port = htons(8082);

    // connect the client socket to server socket
    if (connect(socket3, (SA*)&serv3addr, sizeof(serv3addr)) != 0) {
        printf("connection with server 3 failed...\n");
        exit(0);
    }
    else
        printf("connected to server 3.\n");

    socket4 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket4 == -1) {
        printf("socket 4 creation failed...\n");
        exit(0);
    }
    else
        printf("Socket 4 successfully created..\n");
        bzero(&serv4addr, sizeof(serv4addr));
        // assign IP, PORT
    serv4addr.sin_family = AF_INET;
    serv4addr.sin_addr.s_addr = inet_addr("127.0.0.4");
    serv4addr.sin_port = htons(8083);

    // connect the client socket to server socket
    if (connect(socket4, (SA*)&serv4addr, sizeof(serv4addr)) != 0) {
        printf("connection with server 4 failed...\n");
        exit(0);
    }
    else
        printf("connected to server 4.\n");


}

void coach_tell(int sockfd)
{
    char buff[MAX];
    int n;
    bzero(buff, sizeof(buff));
    printf("Telling server this is a coach.\n");
    strcpy(buff, "coach\0");
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server: %s", buff);
}


/*void func(int sockfd)           //send requests for data periodically
{
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Requesting data.\n");
        strcpy(buff, "request");
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server: %s\n", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
        delay(500);
    }
}*/

int func(int socket)           //send requests for data periodically
{
    char buff[MAX];
        bzero(buff, sizeof(buff));
        read(socket, buff, sizeof(buff));
        printf("From Pi: %s\n", buff);

        if ((strncmp(buff, "exit", 4)) == 0) {
            return 1;
        }
    return 0;
}

int main()
{
    int exit = 0, exit1 = 0, exit2 = 0, exit3 = 0, exit4 = 0;
    int soc = 1;

    // socket create and varification
/*    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));
*/
    // assign IP, PORT
/*    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
*/
    setup();
    // tell server this is a coach
    coach_tell(socket1);
    coach_tell(socket2);
    coach_tell(socket3);
    coach_tell(socket4);

    // function for chat
    //func(sockfd);

    while (exit == 0){

        switch(soc){
            case 1: if(exit1 !=1)exit1 = func(socket1);
                    else if(exit1 == 1){
                        printf("Closing socket 1\n");
                        close(socket1);
                    }
                break;
            case 2: if(exit2 !=1)exit2 = func(socket2);
                    else if(exit2 == 1){
                        printf("Closing socket 1\n");
                        close(socket2);
                    }

                break;
            case 3: if(exit3 !=1)exit3 = func(socket3);
                    else if(exit3 == 1){
                        printf("Closing socket 1\n");
                        close(socket3);
                    }

                break;
            case 4: if(exit4 !=1)exit4 = func(socket4);
                    else if(exit4 == 1){
                        printf("Closing socket 1\n");
                        close(socket4);
                    }
                    printf("\n\n");
                    soc = 0;
                break;
        }
        if(exit1 == 1 && exit2 ==1 && exit3 == 1 && exit4 ==1){
        exit = 1;
        }
        soc++;
    }
    return 0;
}
