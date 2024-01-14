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

#ifndef _NII_StringSerialiser_H_
#define _NII_StringSerialiser_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** ÎÄ±¾Á÷
    @version NIIEngine 3.0.0
    */
    class StringSerialiser
    {
    public:
        StringSerialiser(NCount size = 0);
        ~StringSerialiser();

        String getStr() const;

        StringSerialiser & operator << (const Ntchar * str);
        StringSerialiser & operator << (const String & str);
        StringSerialiser & operator << (Ni8 val);
        StringSerialiser & operator << (Ni16 val);
        StringSerialiser & operator << (Ni32 val);
        StringSerialiser & operator << (Nui8 val);
        StringSerialiser & operator << (Nui16 val);
        StringSerialiser & operator << (Nui32 val);
        StringSerialiser & operator << (NIIf val);
        StringSerialiser & operator << (NIId val);
    private:
        void enlarge(NCount n);
    private:
        Ntchar * mBuffer;
        NCount mBufferOft;
        NCount mTotalSize;
    };
}
#endif