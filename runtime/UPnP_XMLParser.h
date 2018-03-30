/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _UPNP_XMLPARSER_H_
#define _UPNP_XMLPARSER_H_

#include "upnp.h"
#include "soap.h"
#include <vector>

namespace upnp
{
    /**
     * UPnP_XMLParser
     */
    class UPnP_XMLParser
    {
    public:
        static bool init();
        /**
         * ParseDeviceDescription
         *      Parses the device description from a in-memory XML file.
         */
        static bool ParseDeviceDescription(UPnP_Device * dev, const std::vector<unsigned char> &);
        /**
         * ParseSOAPResponse
         *      Parses a SOAP response from a in-memory XML file.
         */
        static bool ParseSOAPResponse(UPnP_SOAPAction * response, const std::vector<unsigned char> &);
    };
}

#endif