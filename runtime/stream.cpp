/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "stream.h"
#include <string.h>
#include <stdlib.h>

/**
 * stream_init
 */
void stream_init(stream_t * stream, const unsigned char * ptr, int len)
{
    stream->len = len;
    stream->offset = 0;
    stream->ptr = ptr;
}

/**
 * stream_read_line
 */
char * stream_read_line(stream_t * stream, char * ptr, int len)
{
    int line_length;
    char * pData = (char *) stream->ptr + stream->offset;
    /* find the CR */
    char * cr = strchr((char *) pData, 0x0d);
    if (!cr) {
        return 0;
    }
    /* make sure that CR is followed by LF */
    line_length = cr - pData;
    if ((stream->offset + line_length + 1) > stream->len)
        return 0;

    if (*(pData + line_length + 1) != 0x0a)
        return 0;

    if (ptr) { /* use supplied buffer */
        if ((line_length + 1) > len) {
            return 0;
        }
        memcpy(ptr, pData, line_length);
        ptr[line_length] = 0;
        stream->offset += line_length + 2;          /* skip line plus CR LF */
        return ptr;
    } else { /* allocate a new buffer */
        ptr = (char *) malloc(line_length + 1);
        if (!ptr)
            return 0;
        memcpy(ptr, pData, line_length);
        ptr[line_length] = 0;
        stream->offset += line_length + 2;          /* skip line plus CR LF */
        return ptr;
    }
}