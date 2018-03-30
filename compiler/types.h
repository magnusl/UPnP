/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _TYPES_H_
#define _TYPES_H_

namespace upnp
{
    typedef enum {
        TYPE_UI1 = 0,
        TYPE_UI2,
        TYPE_UI4,
        TYPE_I1,
        TYPE_I2,
        TYPE_I4,
        TYPE_INT,
        TYPE_R4,
        TYPE_R8,
        TYPE_NUMBER,
        TYPE_FIXED_14_4,
        TYPE_FLOAT,
        TYPE_CHAR,
        TYPE_STRING,
        TYPE_DATE,
        TYPE_DATETIME,
        TYPE_DATETIME_TZ,
        TYPE_TIME,
        TYPE_TIME_TZ,
        TYPE_BOOLEAN,
        TYPE_BIN_BASE64,
        TYPE_BIN_HEX,
        TYPE_URI,
        TYPE_UUID,
        TYPE_INVALID
    } UPnP_DataType;

    UPnP_DataType UPnP_GetTypeByName(const char * name);

    const char * UPnP_GetNameByType(UPnP_DataType type);
    const char * UPnP_GetImplType(UPnP_DataType type);
    const char * UPnP_GetSerializeMethod(UPnP_DataType type);

};

#endif