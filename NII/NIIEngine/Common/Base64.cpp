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

#include "NiiPreProcess.h"
#include "Base64.h"

#define PADDING '='

using namespace std;

namespace NII
{
    //------------------------------------------------------------------------
    const VString Base64::mData =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    //------------------------------------------------------------------------
    const VString & Base64::getMap()
    {
        return mData;
    }
    //------------------------------------------------------------------------
    void Base64::Encode(const char * in, NCount length, VString & out)
    {
        if (in == NULL || length == 0)
            return;

        long l;
        out.clear();
        out.reserve(((length + 2) / 3) * 4);

        for (NCount i = 0; i < length; i += 3)
        {
            l = ((((unsigned long) in[i]) << 16) & 0xFFFFFF) |
                ((((i + 1) < length) ? (((unsigned long) in[i + 1]) << 8) : 0) & 0xFFFF) |
                ((((i + 2) < length) ? (((unsigned long) in[i + 2]) << 0) : 0) & 0x00FF);

            out.push_back(mData[(l >> 18) & 0x3F]);
            out.push_back(mData[(l >> 12) & 0x3F]);

            if (i + 1 < length)
                out.push_back(mData[(l >> 6) & 0x3F]);
            if (i + 2 < length)
                out.push_back(mData[(l >> 0) & 0x3F]);
        }

        int left = 3 - (length % 3);

        if (length % 3)
        {
            for(int i = 0; i < left; ++i)
                out.push_back(PADDING);
        }
    }
    //------------------------------------------------------------------------
    VString Base64::Encode(const char * in, NCount length)
    {
        VString out;
        Encode(in, length, out);

        return out;
    }
    //------------------------------------------------------------------------
    void Base64::Encode(const VString & in, VString & out)
    {
        Encode(in.c_str(), in.size(), out);
    }
    //------------------------------------------------------------------------
    VString Base64::Encode(const VString & in)
    {
        VString out;
        Encode(in, out);

        return out;
    }
    //------------------------------------------------------------------------
    void Base64::Decode(const char * in, NCount length, VString & out)
    {
        if (in == NULL || length == 0)
            return;

        long l;
        out.clear();

        for (NCount index = 0; index < length; index++)
        {
            if (in[index] == '=')
            {
                length = index;
                break;
            }
        }

        out.reserve(length - ((length + 2) / 4));

        for (NCount i = 0; i < length; i += 4)
        {
            l = ((((unsigned long) mData.find(in[i])) & 0x3F) << 18);
            l |= (((i + 1) < length) ? ((((unsigned long) mData.find(in[i + 1])) & 0x3F) << 12) : 0);
            l |= (((i + 2) < length) ? ((((unsigned long) mData.find(in[i + 2])) & 0x3F) <<  6) : 0);
            l |= (((i + 3) < length) ? ((((unsigned long) mData.find(in[i + 3])) & 0x3F) <<  0) : 0);

            out.push_back((char)((l >> 16) & 0xFF));
            if (i + 2 < length)
                out.push_back((char)((l >> 8) & 0xFF));
            if (i + 3 < length)
                out.push_back((char)((l >> 0) & 0xFF));
        }
    }
    //------------------------------------------------------------------------
    VString Base64::Decode(const char * in, NCount length)
    {
        VString out;
        Decode(in, length, out);

        return out;
    }
    //------------------------------------------------------------------------
    void Base64::Decode(const VString & in, VString & out)
    {
        size_t length = in.find_first_of(PADDING);
        if (length == string::npos)
            length = in.size();

        Decode(in.c_str(), length, out);
    }
    //------------------------------------------------------------------------
    VString Base64::Decode(const VString & in)
    {
        VString out;
        Decode(in, out);

        return out;
    }
    //------------------------------------------------------------------------
}