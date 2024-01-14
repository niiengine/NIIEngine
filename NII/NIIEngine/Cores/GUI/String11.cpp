/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#include "CEGUI/String.h"

#if NII_STRING == CEGUI_STRING_CLASS_UNICODE

#include <iostream>

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    // definition of 'no position' value
    const String::size_type String::npos = (String::size_type)(-1);
    //------------------------------------------------------------------------
    String::~String(void)
    {
        if (d_reserve > CEGUI_STR_QUICKBUFF_SIZE)
        {
            delete[] d_buffer;
        }
            if (d_encodedbufflen > 0)
        {
            delete[] d_encodedbuff;
        }
    }
    //------------------------------------------------------------------------
    bool String::grow(size_type new_size)
    {
        // check for too big
        if (max_size() <= new_size)
            throw(
                std::length_error("Resulting CEGUI::String would be too big"));

        // increase, as we always null-terminate the buffer.
        ++new_size;

        if (new_size > d_reserve)
        {
            utf32* temp = N_new_array_t(utf32, new_size);

            if (d_reserve > CEGUI_STR_QUICKBUFF_SIZE)
            {
                memcpy(temp, d_buffer, (d_cplength + 1) * sizeof(utf32));
                N_delete_array_t(d_buffer, utf32, d_reserve);
            }
            else
            {
                memcpy(temp, d_quickbuff, (d_cplength + 1) * sizeof(utf32));
            }

            d_buffer = temp;
            d_reserve = new_size;

            return true;
        }

        return false;
    }
    //------------------------------------------------------------------------
    void String::trim(void)
    {
        size_type min_size = d_cplength + 1;

        // only re-allocate when not using quick-buffer, and when size can be trimmed
        if ((d_reserve > CEGUI_STR_QUICKBUFF_SIZE) && (d_reserve > min_size))
        {
                // see if we can trim to quick-buffer
            if (min_size <= CEGUI_STR_QUICKBUFF_SIZE)
            {
                memcpy(d_quickbuff, d_buffer, min_size * sizeof(utf32));
                N_delete_array_t(d_buffer, utf32, d_reserve);
                d_reserve = CEGUI_STR_QUICKBUFF_SIZE;
            }
            // re-allocate buffer
            else
            {
                utf32* temp = N_new_array_t(utf32, min_size);
                memcpy(temp, d_buffer, min_size * sizeof(utf32));
                N_delete_array_t(d_buffer, utf32, d_reserve);
                d_buffer = temp;
                d_reserve = min_size;
            }

        }

    }
    //------------------------------------------------------------------------
    utf8 * String::build_utf8_buff(void) const
    {
        size_type buffsize = encoded_size(ptr(), d_cplength) + 1;

        if(buffsize > d_encodedbufflen) 
        {
            if(d_encodedbufflen > 0)
            {
                N_delete_array_t(d_encodedbuff, utf8, d_encodedbufflen);
            }

            d_encodedbuff = N_new_array_t(utf8, buffsize);
            d_encodedbufflen = buffsize;
        }

        encode(ptr(), d_encodedbuff, buffsize, d_cplength);

        // always add a null at end
        d_encodedbuff[buffsize-1] = ((utf8)0);
        d_encodeddatlen = buffsize;

        return d_encodedbuff;
    }
    //------------------------------------------------------------------------
    bool operator==(const String & str1, const String & str2)
    {
        return (str1.compare(str2) == 0);
    }
    //------------------------------------------------------------------------
    bool operator==(const String & str, const std::string & std_str)
    {
        return (str.compare(std_str) == 0);
    }
    //------------------------------------------------------------------------
    bool operator==(const std::string& std_str, const String& str)
    {
        return (str.compare(std_str) == 0);
    }
    //------------------------------------------------------------------------
    bool operator==(const String& str, const utf8* utf8_str)
    {
        return (str.compare(utf8_str) == 0);
    }
    //------------------------------------------------------------------------
    bool operator==(const utf8* utf8_str, const String& str)
    {
        return (str.compare(utf8_str) == 0);
    }
    //------------------------------------------------------------------------
    bool operator!=(const String& str1, const String& str2)
    {
        return (str1.compare(str2) != 0);
    }
    //------------------------------------------------------------------------
    bool operator!=(const String& str, const std::string& std_str)
    {
        return (str.compare(std_str) != 0);
    }
    //------------------------------------------------------------------------
    bool operator!=(const std::string& std_str, const String& str)
    {
        return (str.compare(std_str) != 0);
    }
    //------------------------------------------------------------------------
    bool operator!=(const String& str, const utf8* utf8_str)
    {
        return (str.compare(utf8_str) != 0);
    }
    //------------------------------------------------------------------------
    bool operator!=(const utf8* utf8_str, const String& str)
    {
        return (str.compare(utf8_str) != 0);
    }
    //------------------------------------------------------------------------
    bool operator<(const String& str1, const String& str2)
    {
        return (str1.compare(str2) < 0);
    }
    //------------------------------------------------------------------------
    bool operator<(const String& str, const std::string& std_str)
    {
        return (str.compare(std_str) < 0);
    }
    //------------------------------------------------------------------------
    bool operator<(const std::string& std_str, const String& str)
    {
        return (str.compare(std_str) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator<(const String& str, const utf8* utf8_str)
    {
        return (str.compare(utf8_str) < 0);
    }
    //------------------------------------------------------------------------
    bool operator<(const utf8* utf8_str, const String& str)
    {
        return (str.compare(utf8_str) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator>(const String& str1, const String& str2)
    {
        return (str1.compare(str2) > 0);
    }
    //------------------------------------------------------------------------
    bool operator>(const String& str, const std::string& std_str)
    {
        return (str.compare(std_str) > 0);
    }
    //------------------------------------------------------------------------
    bool operator>(const std::string& std_str, const String& str)
    {
        return (str.compare(std_str) <= 0);
    }
    //------------------------------------------------------------------------
    bool operator>(const String& str, const utf8* utf8_str)
    {
        return (str.compare(utf8_str) > 0);
    }
    //------------------------------------------------------------------------
    bool operator>(const utf8* utf8_str, const String& str)
    {
        return (str.compare(utf8_str) <= 0);
    }
    //------------------------------------------------------------------------
    bool operator<=(const String& str1, const String& str2)
    {
        return (str1.compare(str2) <= 0);
    }
    //------------------------------------------------------------------------
    bool operator<=(const String& str, const std::string& std_str)
    {
        return (str.compare(std_str) <= 0);
    }
    //------------------------------------------------------------------------
    bool operator<=(const std::string& std_str, const String& str)
    {
        return (str.compare(std_str) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator<=(const String & str, const utf8 * utf8_str)
    {
        return (str.compare(utf8_str) <= 0);
    }
    //------------------------------------------------------------------------
    bool operator<=(const utf8 * utf8_str, const String & str)
    {
        return (str.compare(utf8_str) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator>=(const String & str1, const String & str2)
    {
        return (str1.compare(str2) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator>=(const String & str, const std::string & std_str)
    {
        return (str.compare(std_str) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator>=(const std::string & std_str, const String & str)
    {
        return (str.compare(std_str) <= 0);
    }
    //------------------------------------------------------------------------
    bool operator>=(const String & str, const utf8 * utf8_str)
    {
        return (str.compare(utf8_str) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator>=(const utf8 * utf8_str, const String & str)
    {
        return (str.compare(utf8_str) <= 0);
    }
    //------------------------------------------------------------------------
    bool operator==(const String & str, const char * c_str)
    {
        return (str.compare(c_str) == 0);
    }
    //------------------------------------------------------------------------
    bool operator==(const char* c_str, const String& str)
    {
        return (str.compare(c_str) == 0);
    }
    //------------------------------------------------------------------------
    bool operator!=(const String & str, const char * c_str)
    {
        return (str.compare(c_str) != 0);
    }
    //------------------------------------------------------------------------
    bool operator!=(const char * c_str, const String & str)
    {
        return (str.compare(c_str) != 0);
    }
    //------------------------------------------------------------------------
    bool operator<(const String & str, const char * c_str)
    {
        return (str.compare(c_str) < 0);
    }
    //------------------------------------------------------------------------
    bool operator<(const char * c_str, const String & str)
    {
        return (str.compare(c_str) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator>(const String & str, const char * c_str)
    {
        return (str.compare(c_str) > 0);
    }
    //------------------------------------------------------------------------
    bool operator>(const char * c_str, const String & str)
    {
        return (str.compare(c_str) <= 0);
    }
    //------------------------------------------------------------------------
    bool operator<=(const String & str, const char * c_str)
    {
        return (str.compare(c_str) <= 0);
    }
    //------------------------------------------------------------------------
    bool operator<=(const char * c_str, const String & str)
    {
        return (str.compare(c_str) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator>=(const String & str, const char* c_str)
    {
        return (str.compare(c_str) >= 0);
    }
    //------------------------------------------------------------------------
    bool operator>=(const char * c_str, const String & str)
    {
        return (str.compare(c_str) <= 0);
    }
    //------------------------------------------------------------------------
    String	operator+(const String & str1, const String & str2)
    {
        String temp(str1);
        temp.append(str2);
        return temp;
    }
    //------------------------------------------------------------------------
    String operator+(const String & str, const std::string & std_str)
    {
        String temp(str);
        temp.append(std_str);
        return temp;
    }
    //------------------------------------------------------------------------
    String operator+(const std::string & std_str, const String & str)
    {
        String temp(std_str);
        temp.append(str);
        return temp;
    }
    //------------------------------------------------------------------------
    String operator+(const String & str, const utf8 * utf8_str)
    {
        String temp(str);
        temp.append(utf8_str);
        return temp;
    }
    //------------------------------------------------------------------------
    String operator+(const utf8 * utf8_str, const String & str)
    {
        String temp(utf8_str);
        temp.append(str);
        return temp;
    }
    //------------------------------------------------------------------------
    String	operator+(const String & str, utf32 code_point)
    {
        String temp(str);
        temp.append(1, code_point);
        return temp;
    }
    //------------------------------------------------------------------------
    String	operator+(utf32 code_point, const String& str)
    {
        String temp(1, code_point);
        temp.append(str);
        return temp;
    }
    //------------------------------------------------------------------------
    String operator+(const String& str, const char* c_str)
    {
        String tmp(str);
        tmp.append(c_str);
        return tmp;
    }
    //------------------------------------------------------------------------
    String operator+(const char * c_str, const String & str)
    {
        String tmp(c_str);
        tmp.append(str);
        return tmp;
    }
    //------------------------------------------------------------------------
    std::ostream & operator<<(std::ostream & s, const String & str)
    {
        return s << str.c_str();
    }
    //------------------------------------------------------------------------
    void swap(String & str1, String & str2)
    {
        str1.swap(str2);
    }
    //------------------------------------------------------------------------
}
}
}
#endif