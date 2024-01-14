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
#include "NiiClipboard.h"
#include "NiiString.h"

namespace NII
{
    //------------------------------------------------------------------------
    ClipboardData::~ClipboardData()
    {
    }
    //------------------------------------------------------------------------
    Clipboard::Clipboard():
        mMimeType(_T("text/plain")), // reasonable default I think
        mBuffer(0),
        mCount(0),
        mData(0)
    {
    }
    //------------------------------------------------------------------------
    Clipboard::~Clipboard()
    {
        if(mBuffer != 0)
        {
            N_delete_array_t(mBuffer, Nui8, mCount);
        }
    }
    //------------------------------------------------------------------------
    void Clipboard::set(ClipboardData * obj)
    {
        mData = obj;
    }
    //------------------------------------------------------------------------
    void Clipboard::get(ClipboardData *& obj) const
    {
        obj = mData;
    }
    //------------------------------------------------------------------------
    void Clipboard::setData(const String & mime, const Nui8 * buffer, NCount size)
    {
        mMimeType = mime;

        if(size != mCount)
        {
            if (mBuffer != 0)
            {
                N_delete_array_t(mBuffer, Nui8, mCount);
                mBuffer = 0;
            }

            mCount = size;
            mBuffer = N_new_array_t(Nui8, mCount);
        }

        memcpy(mBuffer, buffer, mCount);

        // we have set the data to the internal clipboard, now sync it with the
        // system-wide native clipboard if possible
        if (mData)
        {
            mData->set(mMimeType, mBuffer, mCount);
        }
    }
    //------------------------------------------------------------------------
    void Clipboard::getData(String & mime, const Nui8 *& buffer, NCount & size)
    {
        // first make sure we are in sync with system-wide native clipboard
        // (if possible)
        if(mData)
        {
            NCount retrievedSize;
            void * retrievedBuffer;

            mData->get(mMimeType, retrievedBuffer, retrievedSize);

            if(retrievedSize != mCount)
            {
                if(mBuffer != 0)
                {
                    N_delete_array_t(mBuffer, Nui8, mCount);
                    mBuffer = 0;
                }

                mCount = retrievedSize;
                mBuffer = N_new_array_t(Nui8, mCount);
            }

            memcpy(mBuffer, retrievedBuffer, retrievedSize);
        }

        mime = mMimeType;
        buffer = mBuffer;
        size = mCount;
    }

    //------------------------------------------------------------------------
    void Clipboard::setText(const String & text)
    {
        // could be just ASCII if std::string is used as String
        const Ntchar * utf8_bytes = text.c_str();

        // we don't want the actual string length, that might not be the buffer size
        // in case of utf8!
        // this gets us the number of bytes until \0 is encountered
        NCount size = Nstrlen(utf8_bytes) * sizeof(Ntchar);

        setData(_T("text/plain"), (const Nui8 *)utf8_bytes, size);
    }

    //------------------------------------------------------------------------
    String Clipboard::getText()
    {
        String mime;
        const Nui8 * buffer;
        NCount size;

        // we have to use this, can't use the member variables directly because of
        // the native clipboard provider!
        getData(mime, buffer, size);

        if(mime == _T("text/plain") && size != 0)
        {
            // mBuffer an utf8 or ASCII C string (ASCII if std::string is used)

            // !!! However it is not null terminated !!! So we have to tell String
            // how many code units (not code points!) there are.
            return String((const Ntchar *)mBuffer, size);
        }
        else
        {
            // the held mime type differs, it's not plain text so we can't
            // return it as just string
            return String();
        }
    }
    //-------------------------------------------------------------------------
}