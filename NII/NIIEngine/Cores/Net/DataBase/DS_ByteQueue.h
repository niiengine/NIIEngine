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

#ifndef __BYTE_QUEUE_H
#define __BYTE_QUEUE_H

#include "NiiNetPreInclude.h"

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
    class ByteQueue
    {
    public:
        ByteQueue();
        ~ByteQueue();
        void WriteBytes(const Nui8 * in, NCount size);
        bool ReadBytes(Nui8 * out, Nui32 maxLengthToRead, bool peek);
        Nui32 GetBytesWritten(void) const;
        char * PeekContiguousBytes(Nui32 * outLength) const;
        void IncrementReadOffset(NCount size);
        void DecrementReadOffset(NCount size);
        void Clear();
        void Print();
    protected:
        Nui8 * mData;
        Nui32 readOffset, writeOffset, lengthAllocated;
    };
}

#endif
