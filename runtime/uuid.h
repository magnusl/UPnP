/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _UUID_H_
#define _UUID_H_

#include <stdint.h>
#include <string>

/**
 * UUID
 *      Universal Unique Identifier
 */

class uuid_t
{
public:

    bool parse(const char *, int length = -1);

    bool operator==(const uuid_t & uuid) const
    {
        return memcmp(ident, uuid.ident, 32) == 0;
    }

    char ident[32];
#if 0
    uint64_t        low;
    uint64_t        high;
#endif
};

/**
 * uuid_comparse
 */
inline bool uuid_compare(uuid_t & id1, uuid_t & id2)
{
    return (memcmp(id1.ident, id2.ident, sizeof(id1.ident)) == 0);
}

#endif