/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include "types.h"
#include <cstring>

namespace upnp
{
    const char * types[] = {
        "ui1",
        "ui2",
        "ui4",
        "i1",
        "i2",
        "i4",
        "int",
        "r4",
        "r8",
        "number",
        "fixed.14.4",
        "float",
        "char",
        "string",
        "date",
        "dateTime",
        "dateTime.tz",
        "time",
        "time.tz",
        "boolean",
        "bin.base64",
        "bin.hex",
        "uri",
        "uuid"
    };

    const char * impl_types[] = {
        "uin8_t",
        "uint16_t",
        "uint32_t",
        "int8_t",
        "int16_t",
        "int32_t",
        "int",
        "float",
        "double",
        "double",
        "double",
        "float",
        "wchar_t",
        "std::string",
        "upnp::date_t",
        "upnp::dateTime",
        "upnp::dateTime",
        "time_t",
        "time_t",
        "bool",
        "vector<uint8_t>",
        "vector<uint8_t>",
        "string",
        "uuid_t",
    };

    const char * implSerialize[] = {
        "U8",
        "U16",
        "U32",
        "I8",
        "I16",
        "I32",
        "I32",  /* int */
        "Float",
        "Double",
        "Double",
        "Double",
        "Float",
    };

    UPnP_DataType UPnP_GetTypeByName(const char * name)
    {
        for(size_t i = 0; i < sizeof(types) / sizeof(void *); i++)
        {
            if (!strcmp(name, types[i]))
                return (UPnP_DataType) i;
        }
        return upnp::TYPE_INVALID;
    }

    const char * UPnP_GetNameByType(UPnP_DataType type)
    {
        if (type >= TYPE_INVALID)
            return 0;

        return types[type];
    }

    const char * UPnP_GetImplType(UPnP_DataType type)
    {
        if (type >= TYPE_INVALID)
            return 0;

        return impl_types[type];
    }

    const char * UPnP_GetSerializeMethod(UPnP_DataType type)
    {
        if (type == TYPE_BOOLEAN) {
            return "Bool";
        }
        if (type >= sizeof(implSerialize)/sizeof(char *))
            return "String";
        else
            return implSerialize[type];
    }

};