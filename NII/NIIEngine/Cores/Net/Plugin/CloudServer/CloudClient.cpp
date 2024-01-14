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
#if _RAKNET_SUPPORT_CloudClient == 1

#include "NiiPreProcess.h"
#include "CloudClient.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"
#include "NiiNetLinkBase.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    CloudClient::CloudClient()
    {
        callback = 0;
        allocator = &unsetDefaultAllocator;
    }
    //------------------------------------------------------------------------
    CloudClient::~CloudClient()
    {
    }
    //------------------------------------------------------------------------
    void CloudClient::SetDefaultCallbacks(CloudAllocator * _allocator,
        CloudClientCallback *_callback)
    {
        callback=_callback;
        allocator=_allocator;
    }
    //------------------------------------------------------------------------
    void CloudClient::Post(CloudKey * cloudKey, const Nui8 * data,
        Nui32 dataLengthBytes, UniqueID id)
    {
        N_assert(cloudKey);

        NetSerializer bsOut;
        bsOut.write((Token)ID_CLOUD_POST_REQUEST);
        cloudKey->read(&bsOut);
        if(data == 0)
            dataLengthBytes = 0;
        bsOut.write(dataLengthBytes);
        if(dataLengthBytes > 0)
            bsOut.write((const Nui8 *)data, dataLengthBytes);
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, id, false);
    }
    //------------------------------------------------------------------------
    void CloudClient::Release(DataStructures::List<CloudKey> & keys, UniqueID id)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_CLOUD_RELEASE_REQUEST);
        N_assert(keys.Size() < (Nui16)-1);
        bsOut.write((Nui16)keys.Size());
        for(NCount i = 0; i < keys.Size(); ++i)
        {
            keys[i].read(&bsOut);
        }
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, id, false);
    }
    //------------------------------------------------------------------------
    bool CloudClient::Get(CloudQuery * keyQuery, UniqueID id)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_CLOUD_GET_REQUEST);
        keyQuery->read(&bsOut);
        bsOut.write((Nui16)0); // Specific systems
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, id, false);
        return true;
    }
    //------------------------------------------------------------------------
    bool CloudClient::Get(CloudQuery * keyQuery, DataStructures::List<UniqueID> & specificSystems, UniqueID id)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_CLOUD_GET_REQUEST);
        keyQuery->read(&bsOut);
        bsOut.write((Nui16)specificSystems.Size());
        N_assert(specificSystems.Size() < (Nui16)-1);
        for(Nui16 i = 0; i < specificSystems.Size(); ++i)
        {
            bsOut.write(specificSystems[i]);
        }
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, id, false);
        return true;
    }
    //------------------------------------------------------------------------
    bool CloudClient::Get(CloudQuery * keyQuery, DataStructures::List<CloudQueryRow *> &specificSystems,
        UniqueID id)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_CLOUD_GET_REQUEST);
        keyQuery->read(&bsOut);
        bsOut.write((Nui16)specificSystems.Size());
        N_assert(specificSystems.Size() < (Nui16)-1 );
        for(Nui16 i = 0; i < specificSystems.Size(); ++i)
        {
            if(specificSystems[i]->clientGUID != UniqueID::INVALID)
            {
                bsOut.write1();
                bsOut.write(specificSystems[i]->clientGUID);
            }
            else
            {
                bsOut.write0();
                bsOut.write(specificSystems[i]->clientSystemAddress);
            }
        }
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, id, false);
        return true;
    }
    //------------------------------------------------------------------------
    void CloudClient::Unsubscribe(DataStructures::List<CloudKey> & keys, UniqueID id)
    {
        NetSerializer out;
        out.write((Token)ID_CLOUD_UNSUBSCRIBE_REQUEST);
        N_assert(keys.Size() < (Nui16)-1 );
        out.write((Nui16)keys.Size());
        for(Nui16 i = 0; i < keys.Size(); ++i)
        {
            keys[i].read(&out);
        }
        out.write((Nui16)0);
        send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, id, false);
    }
    //------------------------------------------------------------------------
    void CloudClient::Unsubscribe(DataStructures::List<CloudKey> & keys,
        DataStructures::List<UniqueID> & specificSystems, UniqueID id)
    {
        NetSerializer out;
        out.write((Token)ID_CLOUD_UNSUBSCRIBE_REQUEST);
        N_assert(keys.Size() < (Nui16)-1 );
        out.write((Nui16)keys.Size());
        for (Nui16 i=0; i < keys.Size(); i++)
        {
            keys[i].read(&out);
        }
        out.write((Nui16)specificSystems.Size());
        N_assert(specificSystems.Size() < (Nui16)-1 );
        for (Nui16 i=0; i < specificSystems.Size(); i++)
        {
            out.write(specificSystems[i]);
        }
        send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, id, false);
    }
    //------------------------------------------------------------------------
    void CloudClient::Unsubscribe(DataStructures::List<CloudKey> &keys,
        DataStructures::List<CloudQueryRow *> &specificSystems, UniqueID id)
    {
        NetSerializer out;
        out.write((Token)ID_CLOUD_UNSUBSCRIBE_REQUEST);
        N_assert(keys.Size() < (Nui16)-1 );
        out.write((Nui16)keys.Size());
        for(NCount i=0; i < keys.Size(); i++)
        {
            keys[i].read(&out);
        }
        out.write((Nui16)specificSystems.Size());
        N_assert(specificSystems.Size() < (Nui16)-1 );
        for(Nui16 i=0; i < specificSystems.Size(); i++)
        {
            if(specificSystems[i]->clientGUID != UniqueID::INVALID)
            {
                out.write1();
                out.write(specificSystems[i]->clientGUID);
            }
            else
            {
                out.write0();
                out.write(specificSystems[i]->clientSystemAddress);
            }
        }
        send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, id, false);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType CloudClient::onMessage(SocketMessage * msg)
    {
        (void) msg;

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void CloudClient::OnGetReponse(SocketMessage * msg, CloudClientCallback * _callback,
        CloudAllocator * _allocator)
    {
        if (_callback==0)
            _callback=callback;
        if (_allocator==0)
            _allocator=allocator;

        CloudQueryResult cloudQueryResult;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        cloudQueryResult.write(&bsIn, _allocator);
        bool deallocateRowsAfterReturn=true;
        _callback->OnGet(&cloudQueryResult, &deallocateRowsAfterReturn);
        if (deallocateRowsAfterReturn)
        {
            Nui32 i;
            for (i=0; i < cloudQueryResult.rowsReturned.Size(); i++)
            {
                _allocator->DeallocateRowData(cloudQueryResult.rowsReturned[i]->mData);
                _allocator->DeallocateCloudQueryRow(cloudQueryResult.rowsReturned[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    void CloudClient::OnGetReponse(CloudQueryResult * cloudQueryResult, SocketMessage * msg,
        CloudAllocator * _allocator)
    {
        if (_allocator==0)
            _allocator=allocator;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        cloudQueryResult->write(&bsIn, _allocator);
    }
    //------------------------------------------------------------------------
    void CloudClient::OnSubscriptionNotification(SocketMessage * msg,
        CloudClientCallback * _callback, CloudAllocator *_allocator)
    {
        if(_callback == 0)
            _callback = callback;
        if(_allocator == 0)
            _allocator = allocator;

        bool wasUpdated = false;
        CloudQueryRow row;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        bsIn.read(wasUpdated);
        row.write(&bsIn, _allocator);
        bool deallocateRowAfterReturn = true;
        _callback->OnSubscriptionNotification(&row, wasUpdated, &deallocateRowAfterReturn);
        if(deallocateRowAfterReturn)
        {
            _allocator->DeallocateRowData(row.mData);
        }
    }
    //------------------------------------------------------------------------
    void CloudClient::OnSubscriptionNotification(bool * wasUpdated, CloudQueryRow * row,
        SocketMessage * msg, CloudAllocator * _allocator)
    {
        if(_allocator == 0)
            _allocator = allocator;

        NetSerializer in(msg->mData, msg->mLength, false);
        in.skipRead(1, 0);
        bool b = false;
        in.read(b);
        *wasUpdated = b;
        row->write(&in, _allocator);
    }
    //------------------------------------------------------------------------
    void CloudClient::DeallocateWithDefaultAllocator(CloudQueryResult * cloudQueryResult)
    {
        Nui32 i;
        for(i = 0; i < cloudQueryResult->rowsReturned.Size(); ++i)
        {
            allocator->DeallocateRowData(cloudQueryResult->rowsReturned[i]->mData);
            allocator->DeallocateCloudQueryRow(cloudQueryResult->rowsReturned[i]);
        }

        cloudQueryResult->rowsReturned.Clear(false);
        cloudQueryResult->resultKeyIndices.Clear(false);
        cloudQueryResult->cloudQuery.keys.Clear(false);
    }
    //------------------------------------------------------------------------
    void CloudClient::DeallocateWithDefaultAllocator(CloudQueryRow *row)
    {
        allocator->DeallocateRowData(row->mData);
    }
    //------------------------------------------------------------------------
}
}
#endif
