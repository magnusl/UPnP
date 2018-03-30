/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _DIDL_RESOURCE_H_
#define _DIDL_RESOURCE_H_

#include <string>

namespace didl
{
    /**
     * DIDL_Resource
     */
    struct DIDL_Resource
    {
        DIDL_Resource()
        {
            size = 0;
            bitrate = 0;
            bitsPerSample = 0;
        }

        std::string     protocolInfo;
        unsigned long   size;
        std::string     duration;
        std::string     protection;
        unsigned int    bitrate;
        unsigned int    bitsPerSample;
        std::string     resolution;
        std::string     addr;
    };
}

#endif
