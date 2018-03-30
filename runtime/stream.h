/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _STREAM_H_
#define _STREAM_H_

typedef struct _stream
{
    const unsigned char * ptr;
    int len;
    int offset;
} stream_t;

/**
 * stream_init
 */
void stream_init(stream_t *, const unsigned char *, int);

/**
 * stream_read_line
 */
char * stream_read_line(stream_t *, char *, int);

#endif