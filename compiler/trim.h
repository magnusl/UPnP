/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include <string>

namespace Util
{
    using std::string;

    /**
     * Trim
     *      Trims spaces from a string.
     */
    inline std::string Trim(const std::string & str)
    {
        string tmp;
        size_t left = str.find_first_not_of(" \r\n");
        if (left == string::npos)
            return str;

        /* left trim */
        tmp = str.substr(left);
        size_t right = tmp.find_last_not_of(" \r\n");
        if (right == string::npos)
            return tmp;
        /* right trim */
        tmp = tmp.substr(0, right + 1);
        return tmp;
    }
}