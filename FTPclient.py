import ftplib


# Function to get FTP connection information from user
# return IP of server and port number
def welcome():
    print("Welcome to FTP client app\n")
    server_name = input("Please enter the server name:\n")
    port_number = input("please enter the port number:\n")
    return server_name, port_number


# Create client connection to server
# Param - IP and port of server
# return ftp connection
# To Do: Add try/catch for connection
def create_client(ip, port):
    ftp = ftplib.FTP('')
    # ftp.connect("127.0.0.1", 8080)
    ftp.connect(ip, int(port))
    ftp.login()

    return ftp


# List directories on server
# param - ftp connection
def list_files(ftp):
    print("List files in directory: ")
    print(ftp.retrlines('List'))
    print("\n\n")


# Retrieve a file from the server
# param - ftp connection
def retrieve(ftp):
    filename = input("Enter filename of file to retrieve\n")
    # create file to store retrieved data in
    try:
        localfile = open(filename, 'wb')
        ftp.retrbinary('RETR ' + filename, localfile.write, 1024)
        localfile.close()
        print("File Retrieved \n\n")
    except IOError:
        print("Failure to retrieve file\n\n")
    except ftplib.all_errors:
        print("Error: ftp error \n")


# Store file in server
# param - ftp connection
def store(ftp):
    filename = input("Enter filename to store \n")
    try:
        ftp.storbinary('STOR ' + filename, open(filename, 'rb'))
    except IOError:
        print("Error: file does not appear to exist \n")
    except ftplib.all_errors:
        print("Error: ftp error \n")


# End client connection to server
# param - ftp connection
def quit_connection(ftp):
    print("quit")
    ftp.quit()


def main():
    ftp_connection = True
    while ftp_connection is True:
        #server_name, port_number = welcome()
       # try:
           #ftp_connection = create_client(server_name, port_number)
        #except ftplib.all_errors:
            print("Could not connect to server, try again\n")
            ftp_connection = None

    command = None
    while command != "quit":
        command = input("Enter Command: LIST, RETRIEVE, STORE, QUIT: ")
        if command.lower() == "list":
            list_files(ftp_connection)
        elif command.lower() == "retrieve":
            retrieve(ftp_connection)
        elif command.lower() == "store":
            store(ftp_connection)
        elif command.lower() == "quit":
            quit_connection(ftp_connection)
            command = "quit"
        else:
            print("Invalid command, please try again\n\n")


if __name__ == "__main__":
    main()
