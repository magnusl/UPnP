/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "upnp.h"
#include "ssdp.h"
#include "stream.h"
#include "search.h"

#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include <stdio.h>
#include <vector>
#include <string>

/*****************************************************************************/
/*                          FORWARD DECLARATIONS                             */
/*****************************************************************************/
int upnp_parse_ssdp(ssdp_request_t * request, const unsigned char * src, int len);
int upnp_parse_ssdp_header(ssdp_request_t * request, const char * line);
int upnp_parse_ssdp_info(ssdp_request_t * request);

void splitString(const std::string & str, char delim, std::vector<std::string> & tokens);

/**
 * ssdp_field_contents
 */
const char * ssdp_field_contents(const ssdp_request_t * request, const char * field_name)
{
    for(std::list<ssdp_hdr_t>::const_iterator it = request->hdrs.begin();
        it != request->hdrs.end();
        it++)
    {
        if (!stricmp(field_name, it->name.c_str()))
            return it->contents.c_str();
    }
    return 0;
}

/**
 * upnp_read_multicast_ssdp
 *      Reads a UPnP SSDP request.
 */
int upnp_read_multicast_ssdp(int socket, ssdp_request_t * request, int timeout)
{
    int len;
    unsigned char buf[4096];

    len = read_multicast_udp_packet(socket, buf, sizeof(buf), timeout);
    if (len == -1) {
        int error = WSAGetLastError();
        return -1;
    }

    /* parse the read ssdp packet */
    if (upnp_parse_ssdp(request, buf, len)) 
        return -1;

    /* skip searches */
    if (!strcmp(request->startLine, "M-SEARCH * HTTP/1.1"))
        return 0;

    if (upnp_parse_ssdp_info(request)) {
        printf("%s() failed to parse ssdp info.\r\n", __FUNCTION__);
        return -1;
    }

    return 0;
}

int upnp_read_search_response(int socket, ssdp_request_t * request, int timeout)
{
    int len;
    unsigned char buf[4096];

    len = read_multicast_udp_packet(socket, buf, sizeof(buf), timeout);
    if (len == -1) {
        int error = WSAGetLastError();
        return -1;
    }
    if (upnp_parse_ssdp(request, buf, len)) 
        return -1;

    return 0;
}

/**
 * upnp_parse_ssdp
 */
int upnp_parse_ssdp(ssdp_request_t * request, const unsigned char * src, int len)
{
    stream_t stream;
    char line[256];

    stream_init(&stream, src, len);
    request->type = ssdp_request_t::SSDP_TYPE_UNKNOWN;

    /* read startline */
    request->startLine = stream_read_line(&stream, 0, 0);
    if (!request->startLine) {
        printf("%s() failed to read startline.\r\n", __FUNCTION__);
        return -1;
    }
    /* read headers */
    while(stream_read_line(&stream, line, sizeof(line)))
    {
        if (!strlen(line))  /* empty line */
            break;
        if (upnp_parse_ssdp_header(request, line)) {
            printf("%s() failed to read ssdp header.\r\n", __FUNCTION__);
            return -1;
        }
    }
    return 0;
}



/**
 * upnp_parse_ssdp_header
 *      Parses a UPnP SSDP header.
 */
int upnp_parse_ssdp_header(ssdp_request_t * request, const char * line)
{
    ssdp_hdr_t hdr;
    if (!line)
        return -1;

    std::string str = line;

    size_t delim = str.find_first_of(':');
    if (delim == 0 || delim == str.npos)
        return -1;

    hdr.name = str.substr(0, delim);

    if (delim < (str.length() - 1)) {
        str = str.substr(delim + 1);
        delim = str.find_first_not_of(' ');
        if (delim == str.npos)
            return -1;
        hdr.contents = str.substr(delim);
    }
    request->hdrs.push_back(hdr);

    return 0;
}

int upnp_parse_root_info(ssdp_request_t * request)
{
    const char * usn = ssdp_field_contents(request, "USN");
    if (!usn)
        return false;
    if (strncmp(usn, "uuid:", 5)) /* does not start with "uuid:" */
        return -1;

    size_t uuid_len = strstr(usn, "::") - (usn + 5);
    if (!request->uuid.parse(usn + 5, uuid_len))
        return -1;

    request->type = ssdp_request_t::SSDP_TYPE_ROOTDEVICE;
    return 0;
}

int upnp_parse_uuid_info(ssdp_request_t * request)
{
    const char * usn = ssdp_field_contents(request, "USN");
    if (!usn)
        return -1;
    if (strncmp(usn, "uuid:", 5) || (strlen(usn) < 6))
        return -1;
    if (!request->uuid.parse(usn + 5))
        return -1;

    return 0;
}

/**
 * upnp_parse_urn_info
 */
int upnp_parse_urn_info(ssdp_request_t * request)
{
    std::vector<std::string> tokens;
    const char * nt = ssdp_field_contents(request, "NT");
    if (!nt)
        return -1;

    std::string str = nt;
    splitString(str, ':', tokens);

    if (tokens.size() != 5)
        return -1;

    request->version    = atoi(tokens[4].c_str());
    request->schema     = tokens[1];
    request->deviceType = tokens[3];

    if (upnp_parse_uuid_info(request))
        return -1;

    if (tokens[2] == "device") {
        request->type = ssdp_request_t::SSDP_TYPE_DEVICE;
    } else if (tokens[2] == "service") {
        request->type = ssdp_request_t::SSDP_TYPE_SERVICE;
    } else {
        return -1;
    }

    return 0;
}

/**
 * upnp_parse_uuid
 *      Parses a UUID specified using the UPnP v1.1 format. 
 */
int upnp_parse_ssdp_info(ssdp_request_t * request)
{
    static const size_t device_uuid_length = 36;
    const char * nt = ssdp_field_contents(request, "NT");
    if (!nt) {
        printf("%s() missing NT field\r\n", __FUNCTION__);
        return -1;
    }

    if (!strcmp(nt, "upnp:rootdevice")) {
        return upnp_parse_root_info(request);
    } else if (!strncmp(nt, "uuid:", 5)) {
        return upnp_parse_uuid_info(request);
    } else if (!strncmp(nt, "urn:", 4)) {
        /* device or service */
        return upnp_parse_urn_info(request);
    }

    printf("%s() unknown NT: %s\r\n", __FUNCTION__, nt);
    return -1;
}