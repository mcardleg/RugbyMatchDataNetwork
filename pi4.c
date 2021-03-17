//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include<time.h>

#define TRUE   1
#define FALSE  0
#define PORT 8083	//THIS IS THE ONLY DIFFERENCE BETWEEN EACH PI FILE

//globals
int count =0;
int hb_array[][5];
int impact_array[][5];
int opt;
int master_socket, addrlen, new_socket, client_socket[7], max_clients, clients, player_check[7], activity, i, valread, sd, coach;
int max_sd;
struct sockaddr_in address;
char buffer[1025];  //data buffer of 1K
char player[30];
fd_set readfds;     //set of socket descriptors

void delay(int seconds)
{
    int milli_seconds = 1000 * seconds;
    clock_t start_time = clock();

    while (clock() < start_time + milli_seconds);
}

int concat(int a, int b)
{

    char s1[20];
    char s2[20];

    // Convert both the integers to string
    sprintf(s1, "%d", a);
    sprintf(s2, "%d", b);

    // Concatenate both strings
    strcat(s1, s2);

    // Convert the concatenated string
    // to integer
    int c = atoi(s1);

    // return the formed integer
    return c;
}

int setup(){
    opt = TRUE;
    max_clients = 7;
    clients = 0;

    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;              //initialise all client_socket[] to 0 so not checked
        player_check[i] = 0;               //initialize all sockets to not player.
    }

    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    return 1;
}

int socket_in_out(int i){
    sd = client_socket[i];

    if (FD_ISSET( sd, &readfds))
    {
        //Check if it was for closing , and also read the
        //incoming message
        if ((valread = read( sd, buffer, 1024)) == 0)
        {
            //Somebody disconnected , get his details and print
            getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            //Close the socket and mark as 0 in list for reuse
            close( sd );
            client_socket[i] = 0;
        }
        //If incoming message says "exit", echo it back.
        else if ((strncmp(buffer, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            buffer[valread] = '\0';
            send(sd , buffer , strlen(buffer) , 0 );
            send(coach, buffer, strlen(buffer), 0);
        }
        //Check if the socket is a player
        else if(strcmp(buffer, "player") == 0)
        {
            strcpy(buffer, "ack player\0");
            player_check[i] = 1;                    //track player sockets
            send(sd, buffer, strlen(buffer), 0 );
        }
        //Check if the socket is a coach
        else if(strcmp(buffer, "coach") == 0)
        {
            strcpy(buffer, "ack coach\0");
            coach = client_socket[i];
            send(sd, buffer, strlen(buffer), 0 );
        }
        //Check if it's a message from a player
        else if(player_check[i] == 1)
        {
            char str[3];

            int x =0;
            int y =4;
            sprintf(player, "%d", i);		//DIFFERENT FOR EACH PI
            player[2] = ':';
            player[3] = ' ';
            buffer[valread] = '\0';
            while(buffer[x]!='\0'){
                player[y]=buffer[x];
                x++;
                y++;
            }

            str[0]=player[4];
            str[1]=player[5];
            str[2]=player[7];
            if(player[8]!='\0'){
            str[3]=player[8];
            }
            else{player[8]=' ';}
            player[y++] = '\0';

            int a = str[0];
            int b = str[1];
            int e = concat(a,b);

            if(e>=85){
            count++;
            //store array
            hb_array[e][i];

            }
            else{
            count=0;
            hb_array[e][i];
            //store array
            }

            if(count>5){
            send(coach, player, strlen(player), 0 );
            }

            int c = str[0];
            int d = str[1];
            int f = concat(c,d);

            if(f>=12){
            //alert
            send(coach, player, strlen(player), 0 );
            impact_array[f][i];
            }
            else{
            //store array
            impact_array[f][i];
            }
            strcpy(buffer, "forwarded\0");
            send(sd, buffer, strlen(buffer), 0 );
        }
        //If it's not a player, its a coach requesting data.
        else
        {
            strcpy(buffer, "ack request\0");
            send(sd, buffer, strlen(buffer), 0 );
        }
    }
    return 1;
}

int comms(char *message){
    int e;          //for error handling
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    //add child sockets to set
    for ( i = 0 ; i < max_clients ; i++)
    {
        //socket descriptor
        sd = client_socket[i];

        //if valid socket descriptor then add to read list
        if(sd > 0)
            FD_SET( sd , &readfds);

        //highest file descriptor number, need it for the select function
        if(sd > max_sd)
            max_sd = sd;
        }

    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely
    activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

    if ((activity < 0) && (errno!=EINTR))
    {
        printf("select error");
    }

    //If something happened on the master socket, then its an incoming connection
    if (FD_ISSET(master_socket, &readfds))
    {	if (clients <= max_clients){
        	if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        	{
        	    perror("accept");
        	    exit(EXIT_FAILURE);
        	}
        }
        else{
        	perror("accept");
        	exit(EXIT_FAILURE);
        }

        //inform user of socket number - used in send and receive commands
        printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

        //send new connection greeting message
        if( send(new_socket, message, strlen(message), 0) != strlen(message) )
        {
            perror("send");
        }

        puts("Welcome message sent successfully");

        //add new socket to array of sockets
        for (i = 0; i < max_clients; i++)
        {
            //if position is empty
            if( client_socket[i] == 0 )
            {
                client_socket[i] = new_socket;
                printf("Adding to list of sockets as %d\n" , i);
                break;
            }
        }
    }

    //else its some IO operation on some other socket
    for (i = 0; i < max_clients; i++)
    {
        e = socket_in_out(i);
        if(!e){
        //DO SOME ERROR HANDLING HERE
        }
        e = 0;          //reset
    }
    delay(250);
    return 1;
}

int main(int argc , char *argv[])
{
    //Message that is sent to new connections.
    char *message = "Connected to pi.\r\n";         //maybe add number for which pi
    int e = 0;          //for error checking

    e = setup();
    if (!e){
    // DO SOME ERROR HANDLING HERE
    }
    e = 0;          //reset

    while(TRUE)
    {
        e = comms(message);
        if (!e){
        //DO SOME ERROR HANDLING HERE
        }
        e = 0;      //reset
    }

    return 0;
}
