#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFF_SIZE 5000
#define BACKLOG 10

int main(int argc, char *argv[])
{
    int sfd, nsfd;                                  // Int for describing a Socket File Descriptor
    char buf[BUFF_SIZE];                            // Buffer for reading incoming messages of size BUFF_SIZE
    int status;                                     // int to store status returned by getaddrinfo
    struct sockaddr my_addr, peer_addr;             // For storing conected peer address
    socklen_t peer_addr_len, my_addr_len;           // For storing length of connected peer address
    ssize_t nread;                                  // For storing incoming messages for reading
    struct addrinfo hints, *res, *p;                // Struct to store addrinfo
    char peerip[INET6_ADDRSTRLEN];                  // for holding ip addresses returned by inet_ntop
    char myip[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr, "usage: PORT\n");
        return 1;
    }

    memset(&hints, 0, sizeof(hints));     // Remove garbage values by setting all hint values to 0
    memset(&buf, 0, BUFF_SIZE);
    hints.ai_family = AF_UNSPEC;          // Unspecified, dont care if IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;      // TCP stream socket
    hints.ai_flags = AI_PASSIVE;          // Use local ip. am a server

    status = getaddrinfo(NULL, argv[1], &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddessinfo: %s\n", gai_strerror(status));
    }

    for (p = res; p != NULL; p = p->ai_next)                                    // iterate through the constructed socket info until we find one we can bind to
    {
        sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);             // Create a socket file descriptor
        if (sfd == -1)                                                          // If socket binding fails go to next address
        {
            continue;
        }
        if (bind(sfd, p->ai_addr, p->ai_addrlen) == 0)                         // Bind the socket to an address
        {
            inet_ntop(p->ai_addr->sa_family, p->ai_addr->sa_data, myip, my_addr_len);
            printf("bound to socket at %s\n", myip);
            break;
        }
        close(sfd);
    }
    if (p == NULL)
    {
        fprintf(stderr, "Failed to bind to a socket..\n");
        exit(1);
    }

    freeaddrinfo(res);                                                          // free res as we no longer need it

    if (listen(sfd, BACKLOG) == -1)                                             // Attempt to listen to the socket exit if fail
    {
        perror("listen");
        exit(1);
    }

    while(1)
    {

        nsfd = accept(sfd, &peer_addr, &peer_addr_len);                         // Accept a connection and create addrinfo for the client
        if (nsfd == -1)
        {
            perror("accept");
            continue;
        }
        inet_ntop(peer_addr.sa_family, &peer_addr, peerip, peer_addr_len);       // get addrinfo of client and transform from binary to character string
        printf("server: recieved connection from %s\n", peerip);

        nread = read(nsfd, &buf, BUFF_SIZE);
        printf("%zd bytes read:\n%s\n", nread, buf);


        char delim[2] = " ";                    // assuming http header contains a space between method, path, and protocol
        char* buf_tmp = malloc(sizeof(buf));
        strcpy(buf_tmp, buf);
        char *method = strtok(buf_tmp, delim);
        char *start_path = strchr(buf, '/') - 1;

        char *path = strchr(buf, '/');
        

        printf("method: %s\npath: %s\n", method, path);
        free(buf_tmp);

    }

    return 0;
}