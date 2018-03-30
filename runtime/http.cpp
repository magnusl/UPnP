/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "http.h"
#include <sstream>
#include "socket.h"
#include <iostream>

using namespace std;

void splitString(const std::string & str, char delim, std::vector<std::string> & tokens);

namespace upnp
{

    /**
     *
     */
    void UPnP_HttpRequest::setPayload(const unsigned char * src, size_t len)
    {
        if (len == 0)
            return;

        m_payload.resize(len);
        memcpy(&m_payload[0], src, len); 
    }

    /**
     * UPnP_HttpRequest::addHeader
     */
    void UPnP_HttpRequest::addHeader(const std::string & str1, const std::string & str2)
    {
        m_headers.push_back(pair<string,string>(str1, str2));
    }

    /**
     * UPnP_HttpRequest::addHeader
     */
    void UPnP_HttpRequest::addHeader(const std::string & name, size_t num)
    {
        std::stringstream ss;
        ss << num;
        m_headers.push_back(pair<string,string>(name, ss.str()));
    }

    /**
     * UPnP_HttpRequest::addHeader
     */
    void UPnP_HttpRequest::addHeader(const char * str1, const char * str2)
    {
        if (str1) {
            m_headers.push_back(pair<string,string>(string(str1), str2 ? string(str2) : string("")));
        }
    }

    /**
     * UPnP_HttpRequest::getHeader
     */
    bool UPnP_HttpRequest::getHeader(const std::string & name, std::string &)
    {
        return false;
    }

    bool UPnP_HttpRequest::getHeader(const std::string & name, size_t & value)
    {
        for(list<pair<string, string >>::iterator it = m_headers.begin(); it != m_headers.end(); it++)
        {
            if (!stricmp(it->first.c_str(), name.c_str())) {
                std::stringstream ss;
                ss << it->second;
                if (!(ss >> value))
                    return false;
                
                return true;
            }
        }
        return false;
    }

    /**
     * UPnP_HttpRequest::GetRequest
     */
    UPnP_HttpRequest * UPnP_HttpRequest::GetRequest(const char * host, unsigned short port, const char * uri, int & error)
    {
        int res;
        TCPSocket sock;

        if (sock.Connect(host, port)) {
            printf("%s() failed to connect\r\n", __FUNCTION__);
            return NULL;
        }

        printf("%s() connected\r\n", __FUNCTION__);

        method = UPnP_HttpRequest::HTTP_GET;
        /* write request */
        res = WriteRequest(&sock, uri);
        if (res) {
            /* failed to write request */
            printf("%s() failed to write request.\r\n");
            return 0;
        }

        /* read response */
        UPnP_HttpRequest * response = new UPnP_HttpRequest;
        if (!response)
            return 0;

        if (response->ReadResponse(&sock)) {
            printf("%s() failed to read response.\r\n", __FUNCTION__);
            delete response;
            return 0;
        }

        return response;
    }

    /**
     * UPnP_HttpRequest::PostRequest
     */
    UPnP_HttpRequest * UPnP_HttpRequest::PostRequest(const char * host, unsigned short port, const char * uri, int & error)
    {
        int res;
        TCPSocket sock;
        method = UPnP_HttpRequest::HTTP_POST;

        if (!uri)
            return NULL;
        
        if (sock.Connect(host, port)) {
            printf("%s() failed to connect\r\n", __FUNCTION__);
            return NULL;
        }

        res = WriteRequest(&sock, uri);
        if (res) {
            /* failed to write request */
            printf("%s() failed to write request.\r\n", __FUNCTION__);
            return 0;
        }

        /* read response */
        UPnP_HttpRequest * response = new UPnP_HttpRequest;
        if (!response)
            return 0;

        if (response->ReadResponse(&sock)) {
            printf("%s() failed to read response.\r\n", __FUNCTION__);
            delete response;
            return 0;
        }

        return response;
    }

    /**
     * UPnP_HttpRequest::WriteRequest
     *      Writes the HTTP request to the socket.
     */
    int UPnP_HttpRequest::WriteRequest(TCPSocket * sock, const char * uri)
    {
        std::stringstream ss;

        if (method == UPnP_HttpRequest::HTTP_GET) {
            ss << "GET " << uri << " HTTP/1.1" << "\r\n";
        } else if(method == UPnP_HttpRequest::HTTP_POST) {
            ss << "POST " << uri << " HTTP/1.1" << "\r\n";
        }

        ss << "Accept-Charset: utf-8\r\n";
        ss << "Accept-Encoding: identity\r\n";
        ss << "Host: www.example.com\r\n";
        for(list<pair<string, string> >::const_iterator it = m_headers.begin(); it != m_headers.end(); it++) {      /* write headers */
            ss << it->first << ": " << it->second << "\r\n";
        }
        if(method == UPnP_HttpRequest::HTTP_POST) {
            ss << "Content-Length: " << m_payload.size() << "\r\n";
        }

        ss << "\r\n";                                                   /* end with a empty line */
        const std::string & str = ss.str();
        if (sock->Write((const unsigned char *) str.c_str(), str.size()))   /* write data to socket */
            return -1;

        if(method == UPnP_HttpRequest::HTTP_POST) {
            /* write payload */
            if (sock->Write(&m_payload[0], m_payload.size()))
                return -1;
        }
    
        return 0;
    }

    /**
     * UPnP_HttpRequest::ReadResponse
     */
    int UPnP_HttpRequest::ReadResponse(TCPSocket * sock)
    {
        int status;
        std::string line;

        if (sock->ReadLine(line))
            return -1;

        sscanf(line.c_str(), "HTTP/1.1 %d", &status);

        printf("%s()\r\n", line.c_str());

        if (status != 200) { /* HTTP/1.1 200 OK */
            return -1;
        }

        printf("%s(): 200 OK\r\n", __FUNCTION__);
        
        /* read headers */
        if (ReadHeaders(sock)) {
            printf("%s(): failed to read headers\r\n", __FUNCTION__);
            return -1;
        }

        /* all headers read, payload remaining */
        if (ReadPayload(sock)) {
            printf("%s(): failed to read payload\r\n", __FUNCTION__);
            return -1;
        }
        
        return 0;
    }

    /**
     * UPnP_HttpRequest::ReadHeaders
     */
    int UPnP_HttpRequest::ReadHeaders(TCPSocket * sock)
    {
        std::string line;
        std::vector<string> tokens;

        while(!sock->ReadLine(line))
        {
            if (line.empty() || line == "")
                return 0;

            if (!ParseHeader(line))
                return -1;
        }

        return 0;
    }

    /**
     * UPnP_HttpRequest::ParseHeader
     */
    bool UPnP_HttpRequest::ParseHeader(const std::string & header)
    {
        std::string name, value;
        size_t delim = header.find_first_of(':');
        if (delim == header.npos || !delim)
            return false;

        name = header.substr(0, delim);
        if (delim < (header.size() - 1)) {
            value = header.substr(delim + 1);
        }

        m_headers.push_back(pair<string,string>(name, value));
        
        return true;
    }

    /**
     * UPnP_HttpRequest::ReadPayload
     */
    int UPnP_HttpRequest::ReadPayload(TCPSocket * sock)
    {
        int res;
        size_t len, count = 0;
        if (!getHeader("Content-Length", len))
            return -1;

        if (len > 0xffff)
            return -1;

        m_payload.resize(len);
        while(count < len)
        {
            res = sock->Read(&m_payload[count], len - count);
            if (res <= 0)
                return -1;
            count += res;
        }
        return 0;
    }



}