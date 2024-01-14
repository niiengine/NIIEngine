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
#include "NiiStringSerialiser.h"

#include <cstdio>

namespace NII
{
    //------------------------------------------------------------------
    StringSerialiser::StringSerialiser(NCount size) :
        mBufferOft(0)
    {
        if(size > 0)
        {
            size = size * sizeof(Ntchar);
            mBuffer = N_alloc_t(Ntchar, size);
            mTotalSize = size;
        }
        else
        {
            mBuffer = 0;
            mTotalSize = 0;
        }
    }
    //------------------------------------------------------------------
    StringSerialiser::~StringSerialiser()
    {
        if(mBuffer)
            N_free(mBuffer);
    }
    //------------------------------------------------------------------
    String StringSerialiser::getStr() const
    {
        return mBuffer ? String(mBuffer, mBufferOft) : String();
    }
    //------------------------------------------------------------------
    StringSerialiser & StringSerialiser::operator << (const Ntchar * str)
    {
        NCount n = Nstrlen(str);
        enlarge(n);

        Ntchar * dst = mBuffer + mBufferOft;
        memcpy(dst, str, n * sizeof(Ntchar));
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    StringSerialiser & StringSerialiser::operator << (const String & str)
    {
        NCount n = str.size();
        enlarge(n);

        Ntchar * dst = mBuffer + mBufferOft;
        memcpy(dst, str.c_str(), n * sizeof(String::value_type));
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    StringSerialiser & StringSerialiser::operator << (Ni8 val)
    {
        NCount n = 1;
        enlarge(n);

        Ntchar * dst = mBuffer + mBufferOft;
        n = Nsntprintf(dst, n, _T("%c"), val);
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    StringSerialiser &StringSerialiser::operator << (Ni16 val)
    {
        NCount n = 5;
        enlarge(n);

        Ntchar *dst = mBuffer + mBufferOft;
        n = Nsntprintf(dst, n, _T("%hd"), val);
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    StringSerialiser &StringSerialiser::operator << (Ni32 val)
    {
        NCount n = 10;
        enlarge(n);

        Ntchar * dst = mBuffer + mBufferOft;
        n = Nsntprintf(dst, n, _T("%d"), val);
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    StringSerialiser &StringSerialiser::operator << (Nui8 val)
    {
        NCount n = 3;
        enlarge(n);

        Ntchar * dst = mBuffer + mBufferOft;
        n = Nsntprintf(dst, n, _T("%u"), val);
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    StringSerialiser &StringSerialiser::operator << (Nui16 val)
    {
        NCount n = 5;
        enlarge(n);

        Ntchar * dst = mBuffer + mBufferOft;
        n = Nsntprintf(dst, n, _T("%hu"), val);
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    StringSerialiser & StringSerialiser::operator << (Nui32 val)
    {
        NCount n = 10;
        enlarge(n);

        Ntchar * dst = mBuffer + mBufferOft;
        n = Nsntprintf(dst, n, _T("%u"), val);
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    StringSerialiser & StringSerialiser::operator << (NIIf val)
    {
        NCount n = 100;
        enlarge(n);

        Ntchar * dst = mBuffer + mBufferOft;
        n = Nsntprintf(dst, n, _T("%f"), val);
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    StringSerialiser & StringSerialiser::operator << (NIId val)
    {
        NCount n = 100;
        enlarge(n);

        Ntchar * dst = mBuffer + mBufferOft;
        n = Nsntprintf(dst, n, _T("%f"), val);
        mBufferOft += n;

        return *this;
    }
    //------------------------------------------------------------------
    void StringSerialiser::enlarge(NCount n)
    {
        if(mBuffer == 0)
        {
            mBuffer = N_alloc_t(Ntchar, 128);
            mTotalSize = 128 * sizeof(Ntchar);
        }
        else
        {
            if(mBufferOft + n > (mTotalSize / sizeof(Ntchar)))
            {
                n = n > 128 ? n : 128;
                n = n * sizeof(Ntchar);

                Ntchar * temp = N_alloc_t(Ntchar, mTotalSize + n);
                memcpy(temp, mBuffer, mTotalSize);
                N_free(mBuffer);
                mBuffer = temp;

                mTotalSize += n;
            }
        }
    }
    //------------------------------------------------------------------
}