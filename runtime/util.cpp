/**
 * Copyright (c) 2011 Magnus Leksell, all rights reserved.
 */

#include <string>
#include <vector>

/**
* splitString
*/
void splitString(const std::string & str, char delim, std::vector<std::string> & tokens)
{
    std::string tmp = str;
    while(1)
    {
        size_t d = tmp.find_first_of(delim);
        if (d == std::string::npos) {
            if (!tmp.empty())
                tokens.push_back(tmp);
            return;
        }

        tokens.push_back( tmp.substr(0, d) );
        tmp = tmp.substr(d + 1);
    }
}
