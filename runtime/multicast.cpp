/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#endif

/**
 * multicast_socket_bind
 */
#ifdef WIN32
SOCKET multicast_socket_bind(const char * local_addr, const char * multicast_addr, unsigned short port)
#else
int multicast_socket_bind(const char * local_addr, const char * multicast_addr, unsigned short port)
#endif
{
#ifdef WIN32
    sockaddr_in server;
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == SOCKET_ERROR)
        return -1;

    BOOL reuse = TRUE;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(BOOL)) != 0)
        return -1;

    u_long iMode=1;
    ioctlsocket(sock,FIONBIO,&iMode);

    int ttl  = 2;
    setsockopt( sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(local_addr);

    if( bind(sock, (sockaddr *) &server, sizeof(server)) != 0 ) {
        closesocket(sock);
        return -1;
    }

        /* join multicast group */
    //struct ip_mreq_source imr;
    struct ip_mreq imr;
#if 0
    imr.imr_multiaddr.s_addr  = inet_addr(multicast_addr);
    imr.imr_sourceaddr.s_addr = inet_addr(local_addr);
    imr.imr_interface.s_addr  = inet_addr("0.0.0.0");
#endif
#if 1
    imr.imr_interface.s_addr = inet_addr("0.0.0.0");
    imr.imr_multiaddr.s_addr = inet_addr(multicast_addr);
    if( setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&imr, sizeof(imr)) != 0 )
    {
        closesocket(sock);
        return -1;
    }
#endif
    return sock;
#endif
}

/**
 *
 */
#ifdef WIN32
int read_multicast_udp_packet(SOCKET sock, unsigned char * buf, int buflen, int timeout)
#else
int read_multicast_udp_packet(int sock, unsigned char * buf, int buflen, int timeout)
#endif
{
#ifdef WIN32
    sockaddr_in addr;
    int fromlength = sizeof(addr);
    int res;

    memset(&addr, 0, sizeof(sockaddr_in));
    res = recvfrom(sock, (char *)buf, buflen, 0, 0, 0);
    if (res == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            fd_set readfs;
            timeval val;
            val.tv_usec = timeout * 1000;
            val.tv_sec = 0;
            FD_ZERO(&readfs);
            FD_SET(sock, &readfs);
            res = select(0, &readfs, 0, 0, (timeout == -1 ? 0 : &val));
            if (res == 1) {
                res = recvfrom(sock, (char *)buf, buflen, 0, 0, 0);
                return res;
            }   
            return -1;
        }
        return -1;
    }
    return res;
#endif
}

#ifdef WIN32
int send_multicast_udp_packet(SOCKET sock, const char * dst, unsigned short port, const unsigned char * buf, int buflen)
#else
int send_multicast_udp_packet(int sock, unsigned char * buf, int buflen, int timeout)
#endif
{
#ifdef WIN32
    int res;

    sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(port);
    recvAddr.sin_addr.s_addr = inet_addr(dst);

    res = sendto(sock, (char *)buf, buflen, 0, (SOCKADDR *)&recvAddr, sizeof(recvAddr));
    if (res == SOCKET_ERROR)
        return -1;
#endif
    return res;
}

/**
 * unicast_socket_bind
 */
int unicast_socket_bind(const char * local_addr, unsigned short port)
{
#ifdef WIN32
    sockaddr_in server;
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == SOCKET_ERROR)
        return -1;

    BOOL reuse = TRUE;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(BOOL)) != 0)
        return -1;

    u_long iMode=1;
    //ioctlsocket(sock,FIONBIO,&iMode);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
//inet_addr(local_addr);

    if( bind(sock, (sockaddr *) &server, sizeof(server)) != 0 ) {
        closesocket(sock);
        return -1;
    }
#endif
    return sock;
}

/**
 *
 */
int UPnP_SocketListen(int sock)
{
    if (listen(sock, 100))
        return -1;
    
    return 0;
}

int UPnP_SocketAccept(int sock, int timeout)
{
    int s, res;
    if ((s = accept(sock, 0, 0)) == SOCKET_ERROR) {
        res = WSAGetLastError();
        if (res == WSAEWOULDBLOCK) {
            fd_set readfs;
            timeval val;
            val.tv_usec = timeout * 1000;
            val.tv_sec = 0;
            FD_ZERO(&readfs);
            FD_SET(sock, &readfs);
            res = select(0, &readfs, 0, 0, (timeout == -1 ? 0 : &val));
            if (res == 1) {
                return accept(sock, 0, 0);
            }
            return -1;  
        }
    }
    return s;
}

int UPnP_SocketClose(int sock)
{
    closesocket(sock);
    return 0;
}