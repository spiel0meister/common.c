#ifndef NETSOCK_H_
#define NETSOCK_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef union {
    struct {
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
    } as_parts;
    uint32_t as_int;
}IPv4Addr;

typedef struct {
    int socket;
    IPv4Addr addr;
    uint16_t port;
}Net;

#define IPv4_FMT "%d.%d.%d.%d"
#define IPv4_F(ipv4) (ipv4).as_parts.d, (ipv4).as_parts.c, (ipv4).as_parts.b, (ipv4).as_parts.a

IPv4Addr ipv4_from_cstr(const char* cstr);

bool netsocket_bind(Net* netsock, IPv4Addr addr, uint16_t port);
bool netsocket_listen(Net* netsock, int n);
bool netsocket_accept(Net* netsock, Net* netconn);
bool netsocket_recv(Net* netconn, char* buf, size_t maxlen, int* n, int flags);
bool netsocket_send(Net* conn, char* buf, size_t len, int flags);
bool netsocket_close(Net* netsock);

#endif // NETSOCK_H_

#ifdef NETSOCK_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

IPv4Addr ipv4_from_cstr(const char* cstr) {
    char* end;
    long d = strtol(cstr, &end, 10);
    cstr = end;
    assert(*cstr++ == '.');
    long c = strtol(cstr, &end, 10);
    cstr = end;
    assert(*cstr++ == '.');
    long b = strtol(cstr, &end, 10);
    cstr = end;
    assert(*cstr++ == '.');
    long a = strtol(cstr, &end, 10);

    return (IPv4Addr) {
        .as_parts = {
            .a = a,
            .b = b,
            .c = c,
            .d = d,
        }
    };
}

bool netsocket_bind(Net* netsock, IPv4Addr addr, uint16_t port) {
    netsock->addr = addr;

    netsock->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (netsock->socket < 0) {
        fprintf(stderr, "Couldn't create socket: %s\n", strerror(errno));
        return false;
    }

    struct sockaddr_in addr_in = {0};
    addr_in.sin_addr.s_addr = htonl(netsock->addr.as_int);
    addr_in.sin_port = htons(port);
    addr_in.sin_family = AF_INET;

    socklen_t len = sizeof(addr_in);

    if (bind(netsock->socket, (void*)&addr_in, len) < 0) {
        fprintf(stderr, "Couldn't bind socket: %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool netsocket_listen(Net* netsock, int n) {
    if (listen(netsock->socket, n) < 0) {
        fprintf(stderr, "Couldn't listen on socket: %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool netsocket_accept(Net* netsock, Net* netconn) {
    struct sockaddr_in addr_in = {0};
    socklen_t len = sizeof(addr_in);

    netconn->socket = accept(netsock->socket, (void*)&addr_in, &len);
    if (netconn->socket < 0) {
        fprintf(stderr, "Couldn't accept on socket: %s\n", strerror(errno));
        return false;
    }

    netconn->addr.as_int = addr_in.sin_addr.s_addr;
    netconn->port = ntohs(addr_in.sin_port);

    return true;
}

bool netsocket_recv(Net* netconn, char* buf, size_t maxlen, int* n, int flags) {
    *n = recv(netconn->socket, buf, maxlen, flags);
    if (*n < 0) {
        fprintf(stderr, "Couldn't recv on socket: %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool netsocket_send(Net* conn, char* buf, size_t len, int flags) {
    if (send(conn->socket, buf, len, flags) < 0) {
        fprintf(stderr, "Couldn't send on socket: %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool netsocket_close(Net* netsock) {
    if (close(netsock->socket) < 0) {
        fprintf(stderr, "Couldn't close socket: %s\n", strerror(errno));
        return false;
    }

    return true;
}

#endif // NETSOCK_IMPLEMENTATION
