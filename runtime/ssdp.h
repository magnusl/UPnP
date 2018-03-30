/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _SSDP_H_
#define _SSDP_H_

#include "uuid.h"
#include <list>
/**
 * ssdp_hdr_t
 */
typedef struct _ssdp_hdr_t {
    std::string     name;
    std::string     contents;
} ssdp_hdr_t;

/**
 * ssdp_request_t
 */
class ssdp_request_t
{
public:

    ssdp_request_t()
    {
        type = SSDP_TYPE_UNKNOWN;
        startLine = 0;
        version = 0;
    }

    ~ssdp_request_t()
    {
        if (startLine)
            free((void *) startLine);
    }

    enum {
        SSDP_TYPE_UNKNOWN,
        SSDP_TYPE_ROOTDEVICE,       /* upnp:rootdevice */
        SSDP_TYPE_DEVICE,           /* urn:schemas-upnp-org:device:deviceType:ver */
        SSDP_TYPE_SERVICE           /* urn:schemas-upnp-org:service:serviceType:ver */
    } type;

    char *              startLine;
    std::list<ssdp_hdr_t>   hdrs;
    uuid_t              uuid;
    std::string         deviceType;
    std::string         schema;
    int                 version;
};

/**
 * ssdp_add_header
 */
int ssdp_add_header(ssdp_request_t *, const char * name, const char * contents);

/**
 * ssdp_parse_header
 *      Parses a SSDP header.
 */
int ssdp_parse_header(ssdp_request_t *, const char *);

/**
 * ssdp_field_contents
 *      Returns the contents of a header field.
 */
const char * ssdp_field_contents(const ssdp_request_t *, const char * field_name);


#endif