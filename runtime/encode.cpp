/*
 * Copyright (c) 2010-2011 Magnus Leksell, all rights reserved.
 */

#include "encode.h"
#include <sstream>

namespace upnp
{
    /*************************************************************************/
    /*                                  ENCODE                               */
    /*************************************************************************/
    /**
     * UPnP_TypeEncode::EncodeU8
     */
    std::string UPnP_TypeEncode::EncodeU8(uint8_t num)
    {
        std::stringstream ss;
        ss << num;
        return ss.str();
    }

    /**
     * UPnP_TypeEncode::EncodeU16
     */
    std::string UPnP_TypeEncode::EncodeU16(uint16_t num)
    {
        std::stringstream ss;
        ss << num;
        return ss.str();
    }

    /**
     * UPnP_TypeEncode::EncodeU32
     */
    std::string UPnP_TypeEncode::EncodeU32(uint32_t num)
    {
        std::stringstream ss;
        ss << num;
        return ss.str();
    }

    /**
     * UPnP_TypeEncode::EncodeI8
     */
    std::string UPnP_TypeEncode::EncodeI8(int8_t num)
    {
        std::stringstream ss;
        ss << num;
        return ss.str();
    }

    /**
     * UPnP_TypeEncode::EncodeI16
     */
    std::string UPnP_TypeEncode::EncodeI16(int16_t num)
    {
        std::stringstream ss;
        ss << num;
        return ss.str();
    }

    /**
     * UPnP_TypeEncode::EncodeI32
     */
    std::string UPnP_TypeEncode::EncodeI32(int32_t num)
    {
        std::stringstream ss;
        ss << num;
        return ss.str();
    }

    std::string UPnP_TypeEncode::EncodeString(const std::string & str)
    {
        return str;
    }

    std::string UPnP_TypeEncode::EncodeBool(bool value)
    {
        return "1";
    }

    /*************************************************************************/
    /*                                  DECODE                               */
    /*************************************************************************/

    bool UPnP_TypeEncode::DecodeU8(uint8_t & num, const std::string & str)
    {
        std::stringstream ss;
        ss << str;
        return (ss >> num);
    }

    bool UPnP_TypeEncode::DecodeU16(uint16_t & num, const std::string & str)
    {
        std::stringstream ss;
        ss << str;
        return (ss >> num);
    }

    bool UPnP_TypeEncode::DecodeU32(uint32_t & num, const std::string & str)
    {
        std::stringstream ss;
        ss << str;
        return (ss >> num);
    }

    bool UPnP_TypeEncode::DecodeI8(int8_t & num, const std::string & str)
    {
        std::stringstream ss;
        ss << str;
        return (ss >> num);
    }

    bool UPnP_TypeEncode::DecodeI16(int16_t & num, const std::string & str)
    {
        std::stringstream ss;
        ss << str;
        return (ss >> num);
    }

    bool UPnP_TypeEncode::DecodeI32(int32_t & num, const std::string & str)
    {
        std::stringstream ss;
        ss << str;
        return (ss >> num);
    }

    bool UPnP_TypeEncode::DecodeString(std::string & str, const std::string & cstr)
    {
        str = cstr;
        return true;
    }

    bool UPnP_TypeEncode::DecodeBool(bool & value, const std::string & cstr)
    {
        if (cstr == "1" || cstr == "yes" || cstr == "true") {
            value = true;
        } else if(cstr == "0" || cstr == "no" || cstr == "false") {
            value = false;
        } else {
            return false;
        }
        return true;
    }
}