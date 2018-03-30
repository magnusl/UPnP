/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "uuid.h"
#include <cstdio>
#include <cstring>

/**
 * uuid_t::parse
 */
bool uuid_t::parse(const char * id, int str_len)
{
    char c;
    size_t index = 0;
    while(*id && str_len)
    {
        --str_len;
        c = *id;
        if (!(c >= '0' && c <= '9') &&
            !(c >= 'a' && c <= 'f') &&
            !(c >= 'A' && c <= 'F')) {
            id++;
            continue;   /* not a hex character, continue */
        }
        ident[index++] = c;
        id++;
        if (index == 32) {
            return true;
        }
    }

    return false;
}