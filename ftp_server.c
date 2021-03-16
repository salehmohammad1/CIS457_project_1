/**********************************************************************
* Course:       CIS 457 - Project 1
* Author(s):    Taylor Rieger & Mohammed Saleh
* Date:         March 16, 2021
* Description:  This program creates an FTP server to connect to an 
*               FTP client that allows 5 different commands: Connect,
*               List, Retrieve, Store, and Quit.
* References:   Initial code was utilized from Homework 1 provided code
*               to start the client and server connections.   
* Professor:    Dr. Vijay Bhuse
* Filename:     ftp_server.c            
**********************************************************************/

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>       //gets file size using stat()
#include <sys/sendfile.h>   //used for sending a file
#include <fcntl.h>          //used for file control options

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
    exit(1);
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
     int sockfd, newsockfd, portno, clilen, n, filehandle, size, c;
     char buffer[100], command[5], filename[20];
     struct sockaddr_in serv_addr, cli_addr;
     struct stat obj;
   
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
   
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 
     if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,1000);
     
     while((n = recv(newsockfd, buffer, 999, 0)) > 0)
     {
         printf("\n");
         printf("Here is the message: %s\n", buffer);
         n = write(newsockfd,"I got your message!", 18);
         bzero(buffer, 1000);
     }
    
     if (n == 0) 
     {
         error("Disconnected\n");
         bzero(buffer,1000);
     }

     else if (n < 0) error("ERROR writing to socket");
     
     return 0; 
}