/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#endif

namespace upnp
{
    /**
     * Socket
     */
    class Socket
    {
    public:
        Socket();
        virtual ~Socket();

        int Bind(const char *, unsigned short port);
        int Listen();
        int Accept(int timeout = -1);

#ifdef WIN32
        SOCKET m_sock;
#endif
    };

    /**
     * TCPSocket
     *      Transport Control Protocol Socket.
     */
    class TCPSocket : public Socket
    {
    public:
        TCPSocket();
        int Connect(const char *, unsigned short);
        int Read(unsigned char *, int);
        int ReadLine(std::string &);
        int Write(const unsigned char *, int);
    };

    /**
     * UDPSocket
     *      User Datagram Protocol socket
     */
    class UDPSocket : public Socket
    {
    public:
        UDPSocket();
    };
}

#endif