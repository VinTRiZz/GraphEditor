#pragma once

#include <string>
#include <vector>
#include <string.h>

namespace ExtraClasses
{

struct StringList
{
    std::vector<std::string> stringVect;
    inline StringList & operator <<(const std::string & str)
    {
        stringVect.push_back(str);
        return *this;
    }

    inline std::string join(const char * delim = " ")
    {
        std::string result = "";
        for (const auto & str : stringVect)
        {
            result += str + delim;
        }
        result.erase(result.end() - strlen(delim), result.end());
        return result;
    }
};

}
