/**********************************************************************
* Course:       CIS 457 - Project 1
* Author(s):    Taylor Rieger & Mohammed Saleh
* Date:         March 16, 2021
* Description:  This program creates an FTP client to connect to an 
*               FTP server that allows 5 different commands: Connect,
*               List, Retrieve, Store, and Quit.
* References:   Initial code was utilized from Homework 1 provided code
*               to start the client and server connections.   
* Professor:    Dr. Vijay Bhuse
* Filename:     ftp_client.c            
**********************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>           //gets file size using stat()
#include <sys/sendfile.h>       //used for sending a file
#include <fcntl.h>              //used for file control

/**********************************************************************
* Name:     error
* Author:   Dr. Vijay Bhuse
* Date:     March 16, 2021
* Description: Provides an error message and exits the program.
* @param *msg - the message to provide the screen for the error
**********************************************************************/
void error(char *msg)
{
    perror(msg);
    exit(0);
}

/**********************************************************************
* Name:     main
* Author:   Dr. Vijay Bhuse, additions Taylor Rieger
* Date:     March 16, 2021
* Description: Connects the server to the client and responds to the 
               client depending what option they choose.
* @param argc 
* @param *argv 
**********************************************************************/
int main(int argc, char *argv[])
{
    int sockfd, portno, n, size, status, filehandle, choice;
    struct sockaddr_in serv_addr;
    struct stat obj;
    struct hostent *server;
    char buffer[1000], command[5], filename[20], *f;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    server = gethostbyname(argv[1]);
   
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
       error("ERROR connecting");
    
    int i = 1;

    while(1) 
    {
        printf("Enter a choice to perform:\n\n");
        printf("(1) CONNECT - allows to client to connect to server w/ IP address & port #\n");
        printf("(2) LIST - allows server to return a list of files in current directory\n");
        printf("(3) RETRIEVE - allows a client to get a file specified by filename from server\n");
        printf("(4) STORE - allows a client to send a file specified by filename to server\n");
        printf("(5) QUIT - allows the client to terminate the control connection")
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                break;

            case 2:
                strcpy(buffer, "list ");        //copy string "list" to buffer to let server know what to perform
                send(sockfd, buffer, 100, 0);  //send the buffer to the server
                recv(sockfd, &size, sizeof(int), 0);    
                f = malloc(size);
                recv(sockfd, f, size, 0);
                filehandle = creat("temp.txt", O_WRONLY);   //open file for writing only
                write(filehandle, f, size, 0);
                close(filehandle);
                printf("Listing:\n");
                system("cat temp.txt");
                break;

            case 3: 
                strcpy(buffer, "retrieve ")
                printf("Enter filename to retrieve:" );
                scanf("%s", filename);
                strcat(buffer, filename)
                send(sockfd, buffer, 100, 0);
                recv(sockfd, &size, sizeof(int), 0);
                if(!size)
                {
                    printf("ERROR: FILE DOES NOT EXIST\n\n");
                    break;
                }
                f = malloc(size);
                recv(sockfd, f, size, 0);
                while(1)
                {
                    
                }
        }

        if((n = send(sockfd,buffer,strlen(buffer),0) < 0))
            error("ERROR writing to socket");
        bzero(buffer,1000);
        if((n = read(sockfd,buffer,999))<0)
            error("ERROR reading from socket");
        printf("%s\n",buffer);
    }
    
 
    close(sockfd);
    return 0;
}