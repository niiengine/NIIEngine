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
#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_CloudClient==1 || _RAKNET_SUPPORT_CloudServer==1

#include "CloudCommon.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    int CloudKeyComp(const CloudKey &key, const CloudKey &data)
    {
        if (key.primaryKey < data.primaryKey)
            return -1;
        if (key.primaryKey > data.primaryKey)
            return 1;
        if (key.secondaryKey < data.secondaryKey)
            return -1;
        if (key.secondaryKey > data.secondaryKey)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    CloudQueryRow* CloudAllocator::AllocateCloudQueryRow(void)
    {
        return N_new CloudQueryRow();
    }
    //------------------------------------------------------------------------
    void CloudAllocator::DeallocateCloudQueryRow(CloudQueryRow * row)
    {
        N_delete row;
    }
    //------------------------------------------------------------------------
    Nui8 *CloudAllocator::AllocateRowData(Nui32 bytesNeededForData)
    {
        return (Nui8*) N_alloc(bytesNeededForData);
    }
    //------------------------------------------------------------------------
    void CloudAllocator::DeallocateRowData(void * data)
    {
        N_free(data);
    }
    //------------------------------------------------------------------------
    void CloudKey::write(NetSerializer * in)
    {
        in->read(primaryKey);
        in->read(secondaryKey);
    }
    //------------------------------------------------------------------------
    void CloudKey::read(NetSerializer * out)
    {
        out->write(primaryKey);
        out->write(secondaryKey);
    }
    //------------------------------------------------------------------------
    void CloudQuery::write(NetSerializer * out)
    {
        bool startingRowIndexIsZero = 0;
        bool maxRowsToReturnIsZero = 0;
        startingRowIndexIsZero = startingRowIndex == 0;
        maxRowsToReturnIsZero = maxRowsToReturn == 0;
        out->read(startingRowIndexIsZero);
        out->read(maxRowsToReturnIsZero);
        out->read(subscribeToResults);
        if(startingRowIndexIsZero == false)
            out->read(startingRowIndex);
        if(maxRowsToReturnIsZero == false)
            out->read(maxRowsToReturn);

        Nui16 numKeys;
        out->read(numKeys);

        CloudKey cmdk;
        for(Nui16 i = 0; i < numKeys; ++i)
        {
            cmdk.read(false,out);
            keys.Push(cmdk);
        }
    }
    //------------------------------------------------------------------------
    void CloudQuery::read(NetSerializer * out)
    {
        bool startingRowIndexIsZero = 0;
        bool maxRowsToReturnIsZero = 0;
        startingRowIndexIsZero = startingRowIndex == 0;
        maxRowsToReturnIsZero = maxRowsToReturn == 0;
        out->write(startingRowIndexIsZero);
        out->write(maxRowsToReturnIsZero);
        out->write(subscribeToResults);
        if(startingRowIndexIsZero == false)
            out->write(startingRowIndex);
        if(maxRowsToReturnIsZero == false)
            out->write(maxRowsToReturn);
        N_assert(keys.Size() < (Nui16)-1);
        Nui16 numKeys = (Nui16) keys.Size();
        out->write(numKeys);
        for(Nui16 i = 0; i < numKeys; ++i)
        {
            keys[i].write(true,out);
        }
    }
    //------------------------------------------------------------------------
    void CloudQueryRow::write(NetSerializer * in, CloudAllocator * allocator)
    {
        key.write(in);
        in->read(serverSystemAddress);
        in->read(clientSystemAddress);
        in->read(serverGUID);
        in->read(clientGUID);
        in->read(mLength);

        if (mLength > 0)
        {
            mData = allocator->AllocateRowData(mLength);
            if (mData)
            {
                in->read((Nui8 *)mData, mLength);
            }
            else
            {
                return;
            }
        }
        else
        {
            mData=0;
        }
    }
    //------------------------------------------------------------------------
    void CloudQueryRow::read(NetSerializer * out, CloudAllocator * allocator)
    {
        key.read(out);
        out->write(serverSystemAddress);
        out->write(clientSystemAddress);
        out->write(serverGUID);
        out->write(clientGUID);
        out->write(mLength);
        out->write((const Nui8 *)mData, mLength);

    }
    //------------------------------------------------------------------------
    void CloudQueryResult::writeHeader(NetSerializer * in)
    {
        cloudQuery.write(in);
        in->read(subscribeToResults);
    }
    //------------------------------------------------------------------------
    void CloudQueryResult::readHeader(NetSerializer * out)
    {
        cloudQuery.read(out);
        out->write(subscribeToResults);
    }
    //------------------------------------------------------------------------
    void CloudQueryResult::writeNumRows(Nui32 & numRows, NetSerializer * in)
    {
        in->read(numRows);
    }
    //------------------------------------------------------------------------
    void CloudQueryResult::readNumRows(Nui32 & numRows, NetSerializer * out)
    {
        out->write(numRows);
    }
    //------------------------------------------------------------------------
    void CloudQueryResult::writeCloudQueryRows(Nui32 & numRows, NetSerializer * in, CloudAllocator *allocator)
    {
        CloudQueryRow * cmdr;
        for(Nui16 i = 0; i < numRows; ++i)
        {
            cmdr = allocator->AllocateCloudQueryRow();
            if(cmdr)
            {
                cmdr->write(in, allocator);
                if(cmdr->mData == 0 && cmdr->mLength > 0)
                {
                    allocator->DeallocateCloudQueryRow(cmdr);
                    numRows = i;
                    return;
                }
                rowsReturned.Push(cmdr);
            }
            else
            {
                numRows = i;
                return;
            }
        }
    }
    //------------------------------------------------------------------------
    void CloudQueryResult::readCloudQueryRows(Nui32 & numRows, NetSerializer * out, CloudAllocator *allocator)
    {
        for(Nui16 i = 0; i < numRows; ++i)
        {
            rowsReturned[i]->read(out, allocator);
        }
    }
    //------------------------------------------------------------------------
    void CloudQueryResult::write(NetSerializer * in, CloudAllocator * allocator)
    {
        writeHeader(in);
        Nui32 numRows = (Nui32) rowsReturned.Size();
        writeNumRows(numRows, in);
        writeCloudQueryRows(numRows, in, allocator);
    }
    //------------------------------------------------------------------------
    void CloudQueryResult::read(NetSerializer * out, CloudAllocator * allocator)
    {
        readHeader(out);
        Nui32 numRows = (Nui32) rowsReturned.Size();
        readNumRows(numRows, out);
        readCloudQueryRows(numRows, out, allocator);
    }
    //------------------------------------------------------------------------
}
}
#endif // #if _RAKNET_SUPPORT_CloudMemoryClient==1 || _RAKNET_SUPPORT_CloudMemoryServer==1
