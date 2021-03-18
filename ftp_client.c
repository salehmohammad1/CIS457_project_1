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
#include <fcntl.h>              //used for file control

#define SIZE 1024

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
    //declare variables
    int sockfd, portno, size, status, filehandler, choice;
    struct sockaddr_in serv_addr;
    struct stat obj;
    struct hostent *server;
    char buffer[1000], command[5], filename[20], *f, data[1024] = {0};
    FILE *fp;
    
    int i = 1;

    while(1) 
    {
        //Print the options list for the user to the terminal
        printf("Enter a choice to perform:\n\n");
        printf("(1) CONNECT - allows to client to connect to server w/ IP address & port #\n");
        printf("(2) LIST - allows server to return a list of files in current directory\n");
        printf("(3) RETRIEVE - allows a client to get a file specified by filename from server\n");
        printf("(4) STORE - allows a client to send a file specified by filename to server\n");
        printf("(5) QUIT - allows the client to terminate the control connection");
        scanf("%d", &choice);       //accept the choice

        //performs whichever choice was chosen
        switch(choice)      
        {
            //CONNECT option
            case 1:

                if (argc < 3) {
                    fprintf(stderr,"usage %s hostname port\n", argv[0]);
                    exit(0);
                }
                //save the port number and open socket
                portno = atoi(argv[2]);

                //initiate the connection
                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd < 0) 
                    error("ERROR opening socket");
                printf("Server socket created successfully.");
                //get the port number of the server
                server = gethostbyname(argv[1]);

                //error check if the server is available
                if (server == NULL) {
                    fprintf(stderr,"ERROR, no such host\n");
                    exit(0);
                }
                
                bzero((char *) &serv_addr, sizeof(serv_addr));
                serv_addr.sin_family = AF_INET;
                bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
                serv_addr.sin_port = htons(portno);
                //error checking
                if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
                    error("ERROR connecting");
                printf("Socket Connection Successful\n");
                break;

            //LIST option
            case 2:
                
                //copy string "list" to buffer to let server know what to perform
                strcpy(buffer, "list ");  

                //send data to the server and receive data back     
                send(sockfd, buffer, 100, 0);  
                recv(sockfd, &size, sizeof(int), 0);  

                //allocates the exact amount of memory needed for the size of the received data
                f = malloc(size);

                //receive only the data of the size allocated and create a file to hold data retrieved
                recv(sockfd, f, size, 0);
                filehandler = creat("temp.txt", O_WRONLY);   //open file for writing only

                //write to the file handler and then close the file to print the contents
                write(filehandler, f, size);
                close(filehandler);
                printf("Listing:\n");
                system("cat temp.txt");
                break;

            //RETRIEVE option
            case 3: 

                //Copy string "retrieve" to the buffer to tell server what to perform
                strcpy(buffer, "retrieve ");

                //Retrieve the filename from the user and add to the buffer
                printf("Enter filename to retrieve:" );
                scanf("%s", filename);
                strcat(buffer, filename);

                //Send the data to the server and retrieve the information sent back to client
                send(sockfd, buffer, 100, 0);
                recv(sockfd, &size, sizeof(int), 0);

                //if the size is 0 then the file doesn't exist
                if(!size)
                {
                    printf("ERROR: FILE DOES NOT EXIST\n\n");
                    break;
                }

                //allocate memory for the file being received from server
                f = malloc(size);
                recv(sockfd, f, size, 0);
                while(1)
                {
                    //open the file of the filename 
                    //(O_CREAT: create file if doesn't exist)
                    //(O_EXCL: exclusive use flag)
                    //(O_WRONLY: open as writing permissions only)
                    filehandler = open(filename, O_CREAT | O_EXCL | O_WRONLY, portno);
                    if(filehandler < 0)
                        sprintf(filename + strlen(filename), "%d ", i);
                    else   
                        break;
                }

                //write to the file and close the file
                write(filehandler, f, size);
                close(filehandler);
                strcpy(buffer, "cat ");
                strcat(buffer, filename);
                system(buffer);
                break;

            //STORE option
            case 4: 
                //Copy string "store" to the buffer to tell server what to perform
                strcpy(buffer, "store ");

                //Retrieve the filename to be stored to the server and open the file as read only
                printf("Enter filename to store to server:");
                scanf("%s", filename);
                fp = fopen(filename, "r");
                if(fp == NULL)
                    error("ERROR: File not able to read.");

                //Concatenate the filename to the buffer and send to the server    
                strcat(buffer, filename);
                send(sockfd, buffer, 100, 0);

                while(fgets(data, SIZE, fp) != NULL)
                {
                    if(send(sockfd, data, sizeof(data), 0) == -1)
                        error("ERROR: Could not send file.\n");
                    bzero(data, SIZE);    
                }
                break;

            //QUIT option
            case 5:

                //Copy string "quit " to the buffer to tell server what to perform
                strcpy(buffer, "quit ");
                
                //send the buffer and receive the status from the server
                send(sockfd, buffer, 100, 0);
                recv(sockfd, &status, 100, 0);
                if(status)
                {
                    printf("Server Closed\n Quitting Now...\n\n");
                    exit(0);
                }
                else
                    printf("Closing Connection Failed...Please Try Again\n\n");
                break;

        }

    }
}