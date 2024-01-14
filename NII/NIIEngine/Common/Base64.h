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

#ifndef _NII_BASE64_H_
#define _NII_BASE64_H_

#include "NiiPreInclude.h"

namespace NII
{
    class Base64
    {
    public:
        static void Encode(const char * in, NCount length, VString & out);
        static VString Encode(const char * in, NCount length);
        static void Encode(const VString & in, VString & out);
        static VString Encode(const VString & in);
        static void Decode(const char * in, NCount length, VString & out);
        static VString Decode(const char * in, NCount length);
        static void Decode(const VString & in, VString & out);
        static VString Decode(const VString & in);
        
        static const VString & getMap();
    private:
        static const VString mData;
    };
}
#endif