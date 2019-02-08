#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    int status;
    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr, "usage: showip hostname\n");
        return 1;
    }

    memset(&hints, 0, sizeof(hints));     // Remove garbage values by setting all hint values to 0
    hints.ai_family = AF_UNSPEC;          // UNspecified, dont care if IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;      // TCP stream socket

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddressinfo: %s\n", gai_strerror(status));
    }

    printf("IP address for : %s\n\n", argv[1]);

    // iterate through the constructed socket info
    for (p = res; p != NULL; p = p->ai_next)
    {
        void *addr;
        char *ipver;

        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // translate IP address info to printable format
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("%s: %s\n", ipver, ipstr);

    }

    freeaddrinfo(res);
    return 0;
}