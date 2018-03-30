/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _HTTP_H_
#define _HTTP_H_

#include <string>
#include <list>
#include <vector>
#include "socket.h"

namespace upnp
{
    /**
     * UPnP_HttpRequest
     *      A HTTP request.
     */
    class UPnP_HttpRequest
    {
    public:
        enum {
            HTTP_GET,
            HTTP_POST,
            HTTP_PUT
        } method;

        /* performs a HTTP GET request */
        UPnP_HttpRequest * UPnP_HttpRequest::GetRequest(const char * host, unsigned short port, const char * uri, int & error);
        /* performs a HTTP POST request */
        UPnP_HttpRequest * UPnP_HttpRequest::PostRequest(const char * host, unsigned short port, const char * uri, int & error);

        /* sets the payload, used for POST */
        void setPayload(const unsigned char *, size_t);
        /* adds a http header */
        void addHeader(const std::string &, const std::string &);
        void addHeader(const std::string &, size_t);
        void addHeader(const char *, const char *);
        bool getHeader(const std::string &, std::string &);
        bool getHeader(const std::string &, size_t &);

        const std::vector<unsigned char> & GetPayload()     {return m_payload;}

    protected:

        int WriteRequest(TCPSocket * sock, const char * uri);
        int ReadResponse(TCPSocket * sock);
        int ReadHeaders(TCPSocket * sock);
        bool ParseHeader(const std::string &);
        int ReadPayload(TCPSocket * sock);

        std::list<std::pair<std::string, std::string> > m_headers;
        std::vector<unsigned char> m_payload;
    };
}

#endif