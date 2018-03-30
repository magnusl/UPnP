/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _DIDL_PARSER_H_
#define _DIDL_PARSER_H_

#include "DIDL_Object.h"
#include <list>
#include <cstdint>
#include <vector>

namespace didl
{
    /**
     * DIDL_Parser
     *      Parses a DIDL-LITE XML document.
     */
    class DIDL_Parser
    {
    public:
        /**
         * ParseDIDL
         *      Parses a in-memory DIDL-LITE XML document.
         */
        static int ParseMemoryDIDL(const std::string & str, std::list<DIDL_Object *> &);
    };
}

#endif