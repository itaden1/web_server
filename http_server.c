#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int status;
    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr, "usage: PORT\n");
        return 1;
    }

    memset(&hints, 0, sizeof(hints));     // Remove garbage values by setting all hint values to 0
    hints.ai_family = AF_UNSPEC;          // Unspecified, dont care if IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;      // TCP stream socket
    hints.ai_flags = AI_PASSIVE;          // Use local ie. am a server

    printf("argv == %s\n", argv[1]);

    status = getaddrinfo(NULL, argv[1], &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddessinfo: %s\n", gai_strerror(status));
    }

    for (p = res; p != NULL; p = p->ai_next)                                    // iterate through the constructed socket info until we find one we can bind to
    {
        int sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);         // Create a socket file descriptor
        if (sfd == -1)                                                          // If socket binding fails go to next address
        {
            continue;
        }
        if (bind(sfd, p->ai_addr, p->ai_addrlen))                               // Bind the socket to an address
        {
            printf("bound to socket at %s\n", p->ai_addr->sa_data);
            close(sfd);
            break;
        }
    }
    if (p == NULL)
    {
        fprintf(stderr, "Failed to bind to a socket..\n");
    }


    freeaddrinfo(res);
    return 0;
}