/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#ifndef _ENCODE_H_
#define _ENCODE_H_

#include <stdint.h>
#include <string>

namespace upnp
{
    /**
     * UPnP_TypeEncode
     */
    class UPnP_TypeEncode
    {
    public:
        /* numeric values */
        static std::string EncodeU8(uint8_t);
        static std::string EncodeU16(uint16_t);
        static std::string EncodeU32(uint32_t);
        static std::string EncodeI8(int8_t);
        static std::string EncodeI16(int16_t);
        static std::string EncodeI32(int32_t);
        static std::string EncodeString(const std::string &);
        static std::string EncodeBool(bool);

        static bool DecodeU8(uint8_t &, const std::string &);
        static bool DecodeU16(uint16_t &, const std::string &);
        static bool DecodeU32(uint32_t &, const std::string &);
        static  bool DecodeI8(int8_t &, const std::string &);
        static bool DecodeI16(int16_t &, const std::string &);
        static bool DecodeI32(int32_t &, const std::string &);
        static bool DecodeString(std::string &, const std::string &);
        static bool DecodeBool(bool &, const std::string &);
    };
}

#endif