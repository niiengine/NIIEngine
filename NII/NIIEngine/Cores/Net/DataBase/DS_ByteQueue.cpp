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

#include "DS_ByteQueue.h"
#include <string.h> // Memmove
#include <stdlib.h> // realloc
#include <stdio.h>

using namespace DataStructures;
    //------------------------------------------------------------------------
    ByteQueue::ByteQueue()
    {
        readOffset = writeOffset=lengthAllocated=0;
        mData = 0;
    }
    //------------------------------------------------------------------------
    ByteQueue::~ByteQueue()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void ByteQueue::WriteBytes(const Nui8 * in, NCount length)
    {
        Nui32 bytesWritten;
        bytesWritten = GetBytesWritten();
        if (lengthAllocated == 0 || length > lengthAllocated-bytesWritten-1)
        {
            Nui32 oldLengthAllocated=lengthAllocated;
            // Always need to waste 1 byte for the math to work, else writeoffset==readoffset
            Nui32 newAmountToAllocate=length+oldLengthAllocated+1;
            if(newAmountToAllocate < 256)
                newAmountToAllocate = 256;
            lengthAllocated = lengthAllocated + newAmountToAllocate;
            mData = (Nui8 *)N_realloc(mData, lengthAllocated);// ÐèÒªÐÞ¸Ä
            if (writeOffset < readOffset)
            {
                if (writeOffset <= newAmountToAllocate)
                {
                    memcpy(mData + oldLengthAllocated, mData, writeOffset);
                    writeOffset=readOffset+bytesWritten;
                }
                else
                {
                    memcpy(mData + oldLengthAllocated, mData, newAmountToAllocate);
                    memmove(mData, mData+newAmountToAllocate, writeOffset-newAmountToAllocate);
                    writeOffset-=newAmountToAllocate;
                }
            }
        }

        if (length <= lengthAllocated-writeOffset)
            memcpy(mData+writeOffset, in, length);
        else
        {
            // Wrap
            memcpy(mData+writeOffset, in, lengthAllocated-writeOffset);
            memcpy(mData, in+(lengthAllocated-writeOffset), length-(lengthAllocated-writeOffset));
        }
        writeOffset=(writeOffset+length) % lengthAllocated;
    }
    //------------------------------------------------------------------------
    bool ByteQueue::ReadBytes(Nui8 * out, Nui32 maxLengthToRead, bool peek)
    {
        Nui32 bytesWritten = GetBytesWritten();
        Nui32 bytesToRead = bytesWritten < maxLengthToRead ? bytesWritten : maxLengthToRead;
        if(bytesToRead == 0)
            return false;
        if(writeOffset >= readOffset)
        {
            memcpy(out, mData+readOffset, bytesToRead);
        }
        else
        {
            Nui32 availableUntilWrap = lengthAllocated-readOffset;
            if(bytesToRead <= availableUntilWrap)
            {
                memcpy(out, mData + readOffset, bytesToRead);
            }
            else
            {
                memcpy(out, mData + readOffset, availableUntilWrap);
                memcpy(out + availableUntilWrap, mData, bytesToRead - availableUntilWrap);
            }
        }

        if(peek == false)
            IncrementReadOffset(bytesToRead);

        return true;
    }
    //------------------------------------------------------------------------
    char * ByteQueue::PeekContiguousBytes(Nui32 * outLength) const
    {
        if (writeOffset>=readOffset)
            *outLength=writeOffset-readOffset;
        else
            *outLength=lengthAllocated-readOffset;
        return mData+readOffset;
    }
    //------------------------------------------------------------------------
    void ByteQueue::Clear()
    {
        if (lengthAllocated)
            N_free(mData);
        readOffset=writeOffset=lengthAllocated=0;
        mData=0;
    }
    //------------------------------------------------------------------------
    Nui32 ByteQueue::GetBytesWritten(void) const
    {
        if (writeOffset>=readOffset)
            return writeOffset-readOffset;
        else
            return writeOffset+(lengthAllocated-readOffset);
    }
    //------------------------------------------------------------------------
    void ByteQueue::IncrementReadOffset(NCount length)
    {
        readOffset=(readOffset+length) % lengthAllocated;
    }
    //------------------------------------------------------------------------
    void ByteQueue::DecrementReadOffset(NCount length)
    {
        if (length>readOffset)
            readOffset=lengthAllocated-(length-readOffset);
        else
            readOffset-=length;
    }
    //------------------------------------------------------------------------
    void ByteQueue::Print(void)
    {
        Nui32 i;
        for (i=readOffset; i!=writeOffset; i++)
            N_printf("%i ", mData[i]);
        N_printf("\n");
    }
    //------------------------------------------------------------------------