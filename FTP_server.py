from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler
from pyftpdlib.servers import FTPServer
#"C:/Users/Mohammad/FTP/FTPclient.py"
authorizer = DummyAuthorizer()
authorizer.add_user("user", "00000", "/Users/Mohammad/FTP", perm="elradfmw")
authorizer.add_anonymous("/Users/Mohammad/FTP")
#"/Users/JD/Desktop/CIS457/ftp"
handler = FTPHandler
handler.authorizer = authorizer


server = FTPServer(("0.0.0.0", 2121), handler)
server.serve_forever()