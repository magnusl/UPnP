/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "socket.h"

namespace upnp
{
    /**
     * Socket::Socket
     */
    Socket::Socket()
    {
    }

    /**
     * Socket::Socket
     */
    Socket::~Socket()
    {
        if (m_sock)
            closesocket(m_sock);
        m_sock = 0;
    }

    /**
     * Socket::Connect
     */
    int TCPSocket::Connect(const char * src, unsigned short port)
    {
        sockaddr_in service;
        service.sin_addr.S_un.S_addr = inet_addr(src);
        service.sin_port = htons(port);
        service.sin_family = AF_INET;

        if (connect(m_sock, (SOCKADDR*) &service, sizeof(service)))
            return -1;

        return 0;
    }

    /**
     * Socket::Bind
     */
    int Socket::Bind(const char * addr, unsigned short port)
    {
        sockaddr_in service;
        service.sin_addr.S_un.S_addr = inet_addr(addr);
        service.sin_port = htons(port);
        service.sin_family = AF_INET;

        if (bind(m_sock, (SOCKADDR*) &service, sizeof(service))) {
            return -1;
        }

        return 0;
    }

    /**
     *
     */
    int Socket::Listen()
    {
        if (listen(m_sock, SOMAXCONN)) {
            printf("%s() WSAGetLastError returned %d\r\n", __FUNCTION__, WSAGetLastError());
            return -1;
        }

        return 0;
    }

    /**
     * Socket::Listen
     */
    int Socket::Accept(int timeout)
    {
        return (int) accept(m_sock, NULL, NULL);
    }

    /************************************************************************/
    /*                                  UDP                                 */
    /************************************************************************/
    UDPSocket::UDPSocket()
    {
        m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    /*************************************************************************/
    /*                                  TCP                                  */
    /*************************************************************************/

    /**
     * TCPSocket::TCPSocket
     */
    TCPSocket::TCPSocket()
    {
        m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }

    /**
     * Socket::Read
     *      Reads up to count bytes.
     */
    int TCPSocket::Read(unsigned char * src, int count)
    {
        int res, rcount = 0;
        while(rcount < count) {
            res = recv(m_sock, (char *) src + rcount, count - rcount, 0);
            if (res < 0) {
                return res;
            } else if(res == 0) {
                return rcount;
            }
            rcount += res;
        }
        return rcount;
    }

    /**
     * Socket::ReadLine
     *
     */
    int TCPSocket::ReadLine(std::string & str)
    {
        char c;
        int res, count = 0;
        char line[512];
        bool readCr = false;

        while(count < 511)
        {
            if (Read((unsigned char *) &c, 1) != 1)
                return -1;

            if (readCr) {   /* waiting for '\n' */
                if (c == '\n') {
                    line[count] = 0;
                    str = line;
                    return 0;
                }
                return -1;
            } else {
                if (c == '\r')
                    readCr = true;
                else if(c == '\n')
                    return -1;
                else 
                line[count++] = c;
            }
        }

        return -1;
    }

    /**
     * Socket::Write
     */
    int TCPSocket::Write(const unsigned char * src, int count)
    {
        int res, written = 0;
        while(written < count)
        {
            res = send(m_sock, (const char *) src + written, count-written, 0);
            if (res <= 0)
                return -1;

            written += res;
        }
        return 0;
    }

}