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
#include "RPC3.h"
#include "RakMemoryOverride.h"
#include "N_assert.h"
#include "StringTable.h"
#include "NiiNetSerializer.h"
#include "NiiNetLinkBase.h"
#include "NiiNetCommon.h"
#include "NetworkIDManager.h"
#include <stdlib.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    int RPC3::LocalSlotObjectComp(const LocalSlotObject & key, const LocalSlotObject & data)
    {
        if (key.callPriority > data.callPriority)
            return -1;
        if (key.callPriority == data.callPriority)
        {
            if (key.registrationCount < data.registrationCount)
                return -1;
            if (key.registrationCount == data.registrationCount)
                return 0;
            return 1;
        }
        return 1;
    }
    //------------------------------------------------------------------------
    RPC3::RPC3()
    {
        currentExecution[0] = 0;
        networkIdManager = 0;
        outgoingTimestamp = 0;
        outgoingPriority = HIGH_PRIORITY;
        outgoingReliability = RELIABLE_ORDERED;
        outgoingOrderingChannel = 0;
        outgoingBroadcast = true;
        incomingTimeStamp = 0;
        nextSlotRegistrationCount = 0;
    }
    //------------------------------------------------------------------------
    RPC3::~RPC3()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void RPC3::SetNetworkIDManager(NetworkIDManager * idMan)
    {
        networkIdManager=idMan;
    }
    //------------------------------------------------------------------------
    bool RPC3::UnregisterFunction(const char * uniqueIdentifier)
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool RPC3::IsFunctionRegistered(const char * uniqueIdentifier)
    {
        DataStructures::HashIndex i = GetLocalFunctionIndex(uniqueIdentifier);
        return i.IsInvalid()==false;
    }
    //------------------------------------------------------------------------
    void RPC3::SetTimestamp(TimeDurMS timeStamp)
    {
        outgoingTimestamp=timeStamp;
    }
    //------------------------------------------------------------------------
    void RPC3::SetSendParams(PacketLevel plevel, PacketType ptype, char pchannel)
    {
        outgoingPriority = plevel;
        outgoingReliability = ptype;
        outgoingOrderingChannel = pchannel;
    }
    //------------------------------------------------------------------------
    void RPC3::SetRecipientAddress(const Address & address, bool broadcast)
    {
        outgoingSystemAddress = address;
        outgoingBroadcast = broadcast;
    }
    //------------------------------------------------------------------------
    void RPC3::SetRecipientObject(NetworkID networkID)
    {
        outgoingNetworkID = networkID;
    }
    //------------------------------------------------------------------------
    TimeDurMS RPC3::GetLastSenderTimestamp() const
    {
        return incomingTimeStamp;
    }
    //------------------------------------------------------------------------
    Address * RPC3::GetLastSenderAddress() const
    {
        return incomingSystemAddress;
    }
    //------------------------------------------------------------------------
    LinkBase * RPC3::GetRakPeer() const
    {
        return mPrc2;
    }
    //------------------------------------------------------------------------
    const char * RPC3::GetCurrentExecution() const
    {
        return (const char *)currentExecution;
    }
    //------------------------------------------------------------------------
    bool RPC3::SendCallOrSignal(RakString uniqueIdentifier, char parameterCount,
        NetSerializer * serializedParameters, bool isCall)
    {
        Address * systemAddr;

        if(uniqueIdentifier.IsEmpty())
            return false;

        NetSerializer bs;
        if(outgoingTimestamp != 0)
        {
            bs.write((Token)ID_TIMESTAMP);
            bs.write(outgoingTimestamp);
        }
        bs.write((Token)ID_RPC_PLUGIN);
        bs.write(parameterCount);
        if(outgoingNetworkID != UNASSIGNED_NETWORK_ID && isCall)
        {
            bs.write1();
            bs.write(outgoingNetworkID);
        }
        else
        {
            bs.write0();
        }
        bs.write(isCall);
        // This is so the call seekWrite works
        bs.writeAlign();
        NBitCount writeBOffset = bs.getWriteOffset();
        if (outgoingBroadcast)
        {
            Nindex systemIndex;
            for (systemIndex = 0; systemIndex < mPrc2->GetMaximumNumberOfPeers(); systemIndex++)
            {
                systemAddr = mPrc2->GetSystemAddressFromIndex(systemIndex);
                if (systemAddr !=0 && systemAddr != *outgoingSystemAddress)
                {
                    StringCompressor::Instance()->EncodeString(uniqueIdentifier, 512, &bs, 0);

                    bs.writeCompress(serializedParameters->getWriteOffset());

                    bs.write((const unsigned char*) serializedParameters->getData(), serializedParameters->getSize());
                    send(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);

                    // Start writing again after ID_AUTO_RPC_CALL
                    bs.seekWrite(writeBOffset);
                }
            }
        }
        else
        {
            systemAddr = outgoingSystemAddress;
            if (systemAddr && !systemAddr->isInvalid())
            {
                StringCompressor::Instance()->EncodeString(uniqueIdentifier, 512, &bs, 0);

                bs.writeCompress(serializedParameters->getWriteOffset());
                bs.write((const Nui8 *)serializedParameters->getData(), serializedParameters->getSize());
                send(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);
            }
            else
                return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void RPC3::onAttach()
    {
        outgoingSystemAddress=0;
        outgoingNetworkID=UNASSIGNED_NETWORK_ID;
        incomingSystemAddress=0;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType RPC3::onMessage(SocketMessage *msg)
    {
        TimeDurMS timestamp=0;
        unsigned char packetIdentifier, packetDataOffset;
        if ( ( unsigned char ) msg->data[ 0 ] == ID_TIMESTAMP )
        {
            if ( msg->length > sizeof( unsigned char ) + sizeof( TimeDurMS ) )
            {
                packetIdentifier = ( unsigned char ) msg->data[ sizeof( unsigned char ) + sizeof( TimeDurMS ) ];
                // Required for proper endian swapping
                NetSerializer tsBs(msg->data+sizeof(Token),msg->length-1,false);
                tsBs.read(timestamp);
                packetDataOffset=sizeof( unsigned char )*2 + sizeof( TimeDurMS );
            }
            else
                return CRT_Destroy;
        }
        else
        {
            packetIdentifier = ( unsigned char ) msg->data[ 0 ];
            packetDataOffset=sizeof( unsigned char );
        }

        switch (packetIdentifier)
        {
        case ID_RPC_PLUGIN:
            incomingTimeStamp=timestamp;
            incomingSystemAddress=msg->mAddress;
            OnRPC3Call(msg->mAddress, msg->data+packetDataOffset, msg->length-packetDataOffset);
            return CRT_Destroy;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void RPC3::OnRPC3Call(const Address &address, unsigned char * data,
        unsigned int lengthInBytes)
    {
        NetSerializer bs(data,lengthInBytes,false);

        DataStructures::HashIndex functionIndex;
        LocalRPCFunction * lrpcf;
        bool hasParameterCount = false;
        char parameterCount;
        NetworkIDObject * networkIdObject;
        NetworkID networkId;
        bool hasNetworkId = false;
        NCount bytesOnStack;
        NBitCount bitsOnStack;
        char strIdentifier[512];
        incomingExtraData.Reset();
        bs.read(parameterCount);
        bs.read(hasNetworkId);
        if (hasNetworkId)
        {
            bool readSuccess = bs.read(networkId);
            N_assert(readSuccess);
            N_assert(networkId!=UNASSIGNED_NETWORK_ID);
            if (networkIdManager==0)
            {
                // Failed - Tried to call object member, however, networkIDManager system was never registered
                SendError(address, RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE, "");
                return;
            }
            networkIdObject = networkIdManager->GET_OBJECT_FROM_ID<NetworkIDObject*>(networkId);
            if (networkIdObject==0)
            {
                // Failed - Tried to call object member, object does not exist (deleted?)
                SendError(address, RPC_ERROR_OBJECT_DOES_NOT_EXIST, "");
                return;
            }
        }
        else
        {
            networkIdObject = 0;
        }
        bool isCall;
        bs.read(isCall);
        bs.readAlign();
        StringCompressor::Instance()->DecodeString(strIdentifier,512,&bs,0);
        bs.readCompress(bytesOnStack);
        bs.readCompress(bitsOnStack);
        NetSerializer serializedParameters;
        if (bytesOnStack > 0 || bitsOnStack > 0)
        {
            serializedParameters.enlargeByte(bytesOnStack);
            serializedParameters.enlargeBit(bitsOnStack);
            // BITS_TO_BYTES is correct, why did I change this?
            bs.read(serializedParameters.getData(), bytesOnStack, BITS_TO_BYTES(bitsOnStack));
            serializedParameters.seekWrite(bytesOnStack, bitsOnStack);
        }
        {
            // Find the registered function with this str
            if (isCall)
            {
                functionIndex = localFunctions.GetIndexOf(strIdentifier);
                if (functionIndex.IsInvalid())
                {
                    SendError(address, RPC_ERROR_FUNCTION_NOT_REGISTERED, strIdentifier);
                    return;
                }
                lrpcf = localFunctions.ItemAtIndex(functionIndex);

                bool isObjectMember = boost::fusion::get<0>(lrpcf->functionPointer);
                if (isObjectMember==true && networkIdObject==0)
                {
                    // Failed - Calling C++ function as C function
                    SendError(address, RPC_ERROR_CALLING_CPP_AS_C, strIdentifier);
                    return;
                }

                if (isObjectMember==false && networkIdObject!=0)
                {
                    // Failed - Calling C function as C++ function
                    SendError(address, RPC_ERROR_CALLING_C_AS_CPP, strIdentifier);
                    return;
                }
            }
            else
            {
                functionIndex = localSlots.GetIndexOf(strIdentifier);
                if (functionIndex.IsInvalid())
                {
                    SendError(address, RPC_ERROR_FUNCTION_NOT_REGISTERED, strIdentifier);
                    return;
                }
            }
        }

        if (isCall)
        {
            bool isObjectMember = boost::fusion::get<0>(lrpcf->functionPointer);
            boost::function<_RPC3::InvokeResultCodes (_RPC3::InvokeArgs)> functionPtr = boost::fusion::get<1>(lrpcf->functionPointer);

            if (functionPtr == 0)
            {
                // Failed - Function was previously registered, but isn't registered any longer
                SendError(address, RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED, strIdentifier);
                return;
            }

            _RPC3::InvokeArgs functionArgs;
            functionArgs.bitStream = &serializedParameters;
            functionArgs.networkIDManager = networkIdManager;
            functionArgs.caller = this;
            functionArgs.thisPtr = networkIdObject;

            _RPC3::InvokeResultCodes res2 = functionPtr(functionArgs);
        }
        else
        {
            InvokeSignal(functionIndex, &serializedParameters, false);
        }

    }
    //------------------------------------------------------------------------
    void RPC3::InterruptSignal(void)
    {
        interruptSignal=true;
    }
    //------------------------------------------------------------------------
    void RPC3::InvokeSignal(DataStructures::HashIndex functionIndex,
        NetSerializer * serializedParameters, bool temporarilySetUSA)
    {
        if (functionIndex.IsInvalid())
            return;

        Address * lastIncomingAddress=incomingSystemAddress;
        if (temporarilySetUSA)
            incomingSystemAddress=0;
        interruptSignal=false;
        LocalSlot *localSlot = localSlots.ItemAtIndex(functionIndex);
        unsigned int i;
        _RPC3::InvokeArgs functionArgs;
        functionArgs.bitStream=serializedParameters;
        functionArgs.networkIDManager=networkIdManager;
        functionArgs.caller=this;
        i=0;
        while (i < localSlot->slotObjects.Size())
        {
            if (localSlot->slotObjects[i].associatedObject!=UNASSIGNED_NETWORK_ID)
            {
                functionArgs.thisPtr = networkIdManager->GET_OBJECT_FROM_ID<NetworkIDObject*>(localSlot->slotObjects[i].associatedObject);
                if (functionArgs.thisPtr==0)
                {
                    localSlot->slotObjects.RemoveAtIndex(i);
                    continue;
                }
            }
            else
                functionArgs.thisPtr = 0;
            functionArgs.bitStream->seekRead(0);

            bool isObjectMember = boost::fusion::get<0>(localSlot->slotObjects[i].functionPointer);
            boost::function<_RPC3::InvokeResultCodes (_RPC3::InvokeArgs)> functionPtr = boost::fusion::get<1>(localSlot->slotObjects[i].functionPointer);
            if (functionPtr==0)
            {
                if (temporarilySetUSA==false)
                {
                    // Failed - Function was previously registered, but isn't registered any longer
                    SendError(lastIncomingAddress, RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED, localSlots.KeyAtIndex(functionIndex).C_String());
                }
                return;
            }
            _RPC3::InvokeResultCodes res2 = functionPtr(functionArgs);

            // Not threadsafe
            if (interruptSignal==true)
                break;

            i++;
        }
        if (temporarilySetUSA)
            incomingSystemAddress=lastIncomingAddress;
    }
    //------------------------------------------------------------------------
    void RPC3::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {

    }
    //------------------------------------------------------------------------
    void RPC3::shutdown(void)
    {

    }
    //------------------------------------------------------------------------
    void RPC3::Clear(void)
    {
        unsigned j;
        DataStructures::List<RakString> keyList;
        DataStructures::List<LocalSlot*> outputList;
        localSlots.GetAsList(outputList,keyList);
        for (j=0; j < outputList.Size(); ++j)
        {
            OP_DELETE(outputList[j]);
        }
        localSlots.Clear();

        DataStructures::List<LocalRPCFunction *> outputList2;
        localFunctions.GetAsList(outputList2,keyList);
        for (j = 0; j < outputList2.Size(); ++j)
        {
            OP_DELETE(outputList2[j]);
        }
        localFunctions.Clear();
        outgoingExtraData.Reset();
        incomingExtraData.Reset();
    }
    //------------------------------------------------------------------------
    void RPC3::SendError(Address * target, unsigned char errorCode, 
        const char * functionName)
    {
        NetSerializer bs;
        bs.write((Token)ID_RPC_REMOTE_ERROR);
        bs.write(errorCode);
        bs.write((const Nui8 *)functionName,(NCount)strlen(functionName)+1);
        send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, target, false);
    }
    //------------------------------------------------------------------------
    DataStructures::HashIndex RPC3::GetLocalSlotIndex(const char * sharedIdentifier)
    {
        return localSlots.GetIndexOf(sharedIdentifier);
    }
    //------------------------------------------------------------------------
    DataStructures::HashIndex RPC3::GetLocalFunctionIndex(RPC3::RPCIdentifier identifier)
    {
        return localFunctions.GetIndexOf(identifier.C_String());
    }
    //------------------------------------------------------------------------
}
}