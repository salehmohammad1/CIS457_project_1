/*********************************************************************
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

void connect(int);

void list(int);
void front(int);
void sending (int);
void storage(int); //stores files in the directory of the server, pass scoket number


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
int i;
int clientcon;

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, n, filehandle, size, c, i;
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
      
	i=1;
	while(i) {

if(!clientcon)	{	

     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 
    


      if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,1000);
     
}
     
else {
	clientcon = 1; 
}}

   pid = fork();
        if (pid < 0) 
	{
           error("ERROR on fork");
	}
        if (pid == 0)  
	{
           close(sockfd);
           dostuff(newsockfd);
           exit(1);
        }
        else
	{
	   wait (&status);
	   clientcon = status;
	}
}


}


void connect(int sk ) {

 int n;
   char buffer[256];
      
   bzero(buffer,256);
   n = read(sk,buffer,255);
   if (n < 0) error("ERROR reading from socket");

   if(strlen(buffer)<1)
   {
	close(sk);
	exit(0);
   }
   printf("BUFFER:%s.\n",buffer);
   //Call the list function if LIST is selected
   if(strcmp(buffer,"LIST")==0)
   {
	list(sk);
   }
   //Call the retrieve function if retrieve is selected
   if (strcmp(buffer,"RETR")==0) 
   {
        write(sk, "RETR received",13);
        fSend(sk);
   }
   //store command:receive a file from client and store in server directory
   if (strcmp(buffer,"STORE")==0) {
        write(sk, "STORE received",14); //ack command(fixes consecutive write short message merge)
        storage(sk); //run function to store file, pass socket number
   }
   //Disconnect from the server if QUIT is selected
   if(strcmp(buffer,"QUIT")==0){
	close(sk);
	clientcon=0;
	exit(0);
   }

}
		}

void list(int sk) {

	char files[1000];
	int n;
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	if(d){
		while((dir = readdir(d)) != NULL){
			strcat(files, dir->d_name );
			strcat(files,"\n");
		} 
		closedir(d);
	}
	n= write(sk, strlen(files));
	if (n<0){
		error("error establishing scoket ");
	}}
//Send file to server
void sending  (int sk)
{
  char cBuff[256];//communication buffer
  FILE *fPoint; //file pointer
  char fName[256];//file name
  unsigned long fSize=0;//file size

  bzero(fName,256);
  read(sk, fName, 255);//read file name
  fPoint = fopen(fName,"rb");//open text file
  
  if (fPoint == NULL)
  {
    printf("Error opening file.\n");
  }
  else
  {
    //send size of file
    fseek(fPoint,0, SEEK_END);
    fSize = ftell(fPoint);//get curent file pointer
    fseek(fPoint, 0, SEEK_SET);
    sprintf(cBuff,"%ld",fSize);
    write(sk, cBuff, strlen(cBuff));//send size of file
    bzero(cBuff,256);
    read(sk,cBuff,255);//get ack
    while(fgets(cBuff,255,fPoint) != NULL)
    {
      write(sk, cBuff, strlen(cBuff));//send file data
    }
    printf("File Sent.\n");
    fclose(fPoint);//close file
  }
}

//store file from client
void storage (int sk)
{
  char fName[256];//file name buffer
  char rBuff[256];//receive buffer
  FILE *fPoint;//file pointer
  long int fSize = 0; //size of file
  long int count = 0;//count bytes of file delivered
  bzero(fName,256);//clear file name buffer
  read(sk, fName, 255);//read file name
  fPoint = fopen(fName, "wb");//open file with name in write binary mode
  if (fPoint == NULL) //if file open error
  {
    printf("Error opening file.\n"); //notify user of file open error
    write(sk, "Server file error",17);//file open error
  }
  else
  {
    write(sk, "Name received",13);//file name read/open ack
    bzero(rBuff,256);//clear receive buffer
    read(sk, rBuff, 255);//read file length
    //file length ack(fixes consecutive write short message merge)
    write(sk, "File length received",20);
    fSize = atol(rBuff);//convert file size from buffer into number
    //write through file until length of file completed.
    while(count<fSize)
    {
      bzero(rBuff,256);//clear receive buffer
      read(sk,rBuff,255);//read data
      count+=strlen(rBuff);//count data length
      fprintf(fPoint, "%s", rBuff);//put data in new file
    }
    printf("File received.\n");//notify user of completion
    fclose (fPoint);//close file
  }

