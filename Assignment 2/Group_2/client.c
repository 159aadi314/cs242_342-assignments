#include <stdio.h>
#include <stdlib.h> // for malloc, free etc
#include <unistd.h> // for get pid, etc, used for forking, only available in unix
#include <string.h>
#include <sys/types.h>  // pid_t
#include <sys/socket.h> // all socket related stuff
#include <netinet/in.h> // in_addr, sockaddr_in, internet address related stuff
#include <netdb.h>      // in_port_t, and some gethostbyname, hostent etc kinda functions

#define  true 1
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;   // sockaddr_in - socket address, ie having (transport layer) address and port number for AF_INET (IPv4) address family, many others are there like sockaddr_in6, sockaddr_un for AF_INET6, AF_UNIX. used to point to server's address

    struct hostent *server;         // hostent means a host. used to point to server as a host

    char buffer[2049];
    if (argc < 3) {                 // if incorrect format of code execution
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);             // port number of server socket to be contacted
    sockfd = socket(AF_INET, SOCK_STREAM, 0);   // create a TCP (SOCK_STREAM means tcp, SOCK_DGRAM means udp) socket, whose communication domain would be IPv4, ie to allow comm b/w 2 hosts connected by IPv4 (coz AF_INET means IPv4, AF_INET6 is for IPv6, AF_LOCAL for connecting two processes on same host), and the protocol used is automatic (because 0) (this protocol refers to the protocol to be run on top of IP, eg tcp udp etc, tcp is 6, udp 17, etc). a socket descriptor, much like a file descriptor is assigned to sockfd

    if (sockfd < 0)                         // if error creating socket
        error("ERROR opening socket");
    
    server = gethostbyname(argv[1]);           // gethostbyname gives ip address of corresp to argv[1], ie server address, so server has server IP

    if (server == NULL) {                       
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));      // reset serv_addr to 0

    serv_addr.sin_family = AF_INET;         // setting address family of server as AF_INET for IPv4
    bcopy((char *)server->h_addr,           // h_addr stores ip address of server
         (char *)&serv_addr.sin_addr.s_addr,        // sin_addr is union data type,
         server->h_length);         /* sin_addr is the IP address in the socket (the socket structure also contains other data, such as a port). The type of sin_addr is a union, so it can be accessed in three different ways : as s_un_b (four 1-byte integers), s_un_w (two 2-bytes integers) or as s_addr (one 4-bytes integer). inet_addr converts an IPv4 address from a string in dotted decimal representation to an integer. This function is deprecated because it does not support IPv6, use inet_pton instead. So basically, the line  loads into the socket the IP address 127.0.0.1, meaning the local host.*/
    serv_addr.sin_port = htons(portno);     // setting port number (first using htons to convert host byte type (little/big endian) address to network byte type address (big endian) which is used in tcp connections, to avoid complications due to different endianness of two machines)

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) // connect socket file descriptor to the address in serv_addr. sockfd just tells that it is a tcp/udp etc socket running on ipv4/ipv6 etc. serv_addr (type sockaddr_in) tells actual info of a particular ipv4 socket, ie address, port number. serv_addr gives details of particular socket, sockfd knows type of socket and is used to read and write.
        error("ERROR connecting");
    
    bzero(buffer,2049);                     // authentication part
    read(sockfd,buffer,2048);               // giving client welcome message, asking for login id
    printf("%s",buffer);

    scanf("%[^\n]%*c",buffer);
    write(sockfd,buffer,strlen(buffer));    // writing clients response to socket
    
    bzero(buffer,2049);
    read(sockfd,buffer,2048);           // server asking for password
    printf("%s",buffer);
    
    scanf("%[^\n]%*c",buffer);              // client giving password
    write(sockfd,buffer,strlen(buffer));

    bzero(buffer,2049);                     // server giving final response
    read(sockfd,buffer,2048);
    printf("%s",buffer);

    if(strncmp("Authentication successful\n", buffer, 26))      // if server didn't say authentication successful, close the socket, exit program
    {
        close(sockfd);
        return 0;
    }

    else{                                                   // else start the trading
        while(1)
        {
            bzero(buffer,2049);
            fgets(buffer,2048,stdin);                   // get command from client
            n = write(sockfd,buffer,strlen(buffer));    // write it to socket
            if (n < 0) 
                 error("ERROR writing to socket");
            bzero(buffer,2049);
            n = read(sockfd,buffer,2048);               // read response of server
            if (n < 0) 
                 error("ERROR reading from socket");
            printf("%s",buffer);                        // print it on the terminal of client
            int i = strncmp("Connection closed\n" , buffer , 18);   // if server response is connection closed, break and close the socket connection and exit
            if(i == 0)
                   break;
        }
    }

    close(sockfd);
    return 0;
}
