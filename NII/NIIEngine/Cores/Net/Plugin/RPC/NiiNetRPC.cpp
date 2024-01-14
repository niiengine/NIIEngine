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
#if _RAKNET_SUPPORT_RPC4Plugin == 1

#include "NiiNetRPC.h"
#include "NiiNetCommon.h"
#include "NiiNetLinkBase.h"
#include "NetworkObj.h"
#include "Nsleep.h"
#include "NiiNetPlat.h"
#include "DS_Queue.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    struct GlobalRegistration
    {
        void (*registerFunctionPointer) (NetSerializer * userData, SocketMessage * msg);
        void (*registerBlockingFunctionPointer) (NetSerializer * userData, NetSerializer *returnData, SocketMessage *msg );
        char functionName[N_Socket_MaxRPCFuncName];
        Token messageId;
        int callPriority;
    };
    //------------------------------------------------------------------------
    static GlobalRegistration globalRegistrationBuffer[N_Socket_MaxRPCFuncCount];
    static Nui32 globalRegistrationIndex = 0;
    //------------------------------------------------------------------------
    RPC4GlobalRegistration::RPC4GlobalRegistration(const char * uniqueID, void (*functionPointer) (NetSerializer * userData, SocketMessage * msg))
    {
        N_assert(globalRegistrationIndex != N_Socket_MaxRPCFuncCount);
        NCount i;
        for (i = 0; uniqueID[i]; ++i)
        {
            N_assert(i<=N_Socket_MaxRPCFuncName-1);
            globalRegistrationBuffer[globalRegistrationIndex].functionName[i]=uniqueID[i];
        }
        globalRegistrationBuffer[globalRegistrationIndex].registerFunctionPointer=functionPointer;
        globalRegistrationBuffer[globalRegistrationIndex].registerBlockingFunctionPointer=0;
        globalRegistrationBuffer[globalRegistrationIndex].callPriority=0xFFFFFFFF;
        globalRegistrationIndex++;
    }
    //------------------------------------------------------------------------
    RPC4GlobalRegistration::RPC4GlobalRegistration(const char* uniqueID, void (*functionPointer) (NetSerializer * userData, SocketMessage * msg), int callPriority)
    {
        N_assert(globalRegistrationIndex != N_Socket_MaxRPCFuncCount);
        NCount i;
        for (i = 0; uniqueID[i]; ++i)
        {
            N_assert(i<=N_Socket_MaxRPCFuncName-1);
            globalRegistrationBuffer[globalRegistrationIndex].functionName[i]=uniqueID[i];
        }
        globalRegistrationBuffer[globalRegistrationIndex].registerFunctionPointer=functionPointer;
        globalRegistrationBuffer[globalRegistrationIndex].registerBlockingFunctionPointer=0;
        N_assert(callPriority!=(int) 0xFFFFFFFF);
        globalRegistrationBuffer[globalRegistrationIndex].callPriority=callPriority;
        globalRegistrationIndex++;
    }
    //------------------------------------------------------------------------
    RPC4GlobalRegistration::RPC4GlobalRegistration(const char * uniqueID, void (*functionPointer ) (NetSerializer *userData, NetSerializer *returnData, SocketMessage *msg))
    {
        N_assert(globalRegistrationIndex != N_Socket_MaxRPCFuncCount);
        Nui32 i;
        for (i = 0; uniqueID[i]; ++i)
        {
            N_assert(i<=N_Socket_MaxRPCFuncName-1);
            globalRegistrationBuffer[globalRegistrationIndex].functionName[i]=uniqueID[i];
        }
        globalRegistrationBuffer[globalRegistrationIndex].registerFunctionPointer=0;
        globalRegistrationBuffer[globalRegistrationIndex].registerBlockingFunctionPointer=functionPointer;
        globalRegistrationIndex++;
    }
    //------------------------------------------------------------------------
    RPC4GlobalRegistration::RPC4GlobalRegistration(const char* uniqueID, Token messageId)
    {
        N_assert(globalRegistrationIndex != N_Socket_MaxRPCFuncCount);
        Nui32 i;
        for (i = 0; uniqueID[i]; ++i)
        {
            N_assert(i<=N_Socket_MaxRPCFuncName-1);
            globalRegistrationBuffer[globalRegistrationIndex].functionName[i]=uniqueID[i];
        }
        globalRegistrationBuffer[globalRegistrationIndex].registerFunctionPointer=0;
        globalRegistrationBuffer[globalRegistrationIndex].registerBlockingFunctionPointer=0;
        globalRegistrationBuffer[globalRegistrationIndex].messageId=messageId;
        globalRegistrationIndex++;
    }
    //------------------------------------------------------------------------
    enum RPC4Identifiers
    {
        ID_RPC4_CALL,
        ID_RPC4_RETURN,
        ID_RPC4_SIGNAL,
    };
    //------------------------------------------------------------------------
    int RPC::LocalSlotObjectComp(const LocalSlotObject & key, const LocalSlotObject & data)
    {
        if (key.callPriority>data.callPriority)
            return -1;
        if (key.callPriority==data.callPriority)
        {
            if (key.registrationCount<data.registrationCount)
                return -1;
            if (key.registrationCount==data.registrationCount)
                return 0;
            return 1;
        }

        return 1;
    }
    //------------------------------------------------------------------------
    int RPC::LocalCallbackComp(const Token & key, RPC::LocalCallback * const & data)
    {
        if (key < data->messageId)
            return -1;
        if (key > data->messageId)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    RPC::RPC()
    {
        gotBlockingReturnValue=false;
        nextSlotRegistrationCount=0;
        interruptSignal=false;
    }
    //------------------------------------------------------------------------
    RPC::~RPC()
    {
        for (Nui32 i = 0; i < localCallbacks.Size(); ++i)
        {
            N_delete localCallbacks[i];
        }

        DataStructures::List<RakString> keyList;
        DataStructures::List<LocalSlot *> outputList;
        localSlots.GetAsList(outputList,keyList);

        for (Nui32 j = 0; j < outputList.Size(); ++j)
        {
            N_delete outputList[j];
        }
        localSlots.Clear();
    }
    //------------------------------------------------------------------------
    bool RPC::RegisterFunction(const char * uniqueID, void (*functionPointer)(NetSerializer * userData, SocketMessage * msg))
    {
        DataStructures::HashIndex skhi = registeredNonblockingFunctions.GetIndexOf(uniqueID);
        if (skhi.IsInvalid()==false)
            return false;

        registeredNonblockingFunctions.Push(uniqueID, functionPointer);
        return true;
    }
    //------------------------------------------------------------------------
    void RPC::RegisterSlot(const char *sharedIdentifier, void (*functionPointer)(NetSerializer * userData, SocketMessage * msg), int callPriority)
    {
        LocalSlotObject lso(nextSlotRegistrationCount++, callPriority, functionPointer);
        DataStructures::HashIndex idx = GetLocalSlotIndex(sharedIdentifier);
        LocalSlot *localSlot;
        if (idx.IsInvalid())
        {
            localSlot = N_new LocalSlot();
            localSlots.Push(sharedIdentifier, localSlot);
        }
        else
        {
            localSlot=localSlots.ItemAtIndex(idx);
        }
        localSlot->slotObjects.Insert(lso, lso, true);
    }
    //------------------------------------------------------------------------
    bool RPC::RegisterBlockingFunction(const char* uniqueID, void ( *functionPointer ) ( NetSerializer *userData, NetSerializer *returnData, SocketMessage *msg ))
    {
        DataStructures::HashIndex skhi = registeredBlockingFunctions.GetIndexOf(uniqueID);
        if (skhi.IsInvalid()==false)
            return false;

        registeredBlockingFunctions.Push(uniqueID,functionPointer);
        return true;
    }
    //------------------------------------------------------------------------
    void RPC::RegisterLocalCallback(const char * uniqueID, Token messageId)
    {
        bool objectExists;
        Nui32 index;
        LocalCallback *lc;
        RakString str;
        str = uniqueID;
        index = localCallbacks.GetIndexFromKey(messageId,&objectExists);
        if (objectExists)
        {
            lc = localCallbacks[index];
            index = lc->functions.GetIndexFromKey(str,&objectExists);
            if (objectExists == false)
                lc->functions.InsertAtIndex(str, index);
        }
        else
        {
            lc = N_new LocalCallback();
            lc->messageId=messageId;
            lc->functions.Insert(str, str, false);
            localCallbacks.InsertAtIndex(lc, index);
        }
    }
    //------------------------------------------------------------------------
    bool RPC::UnregisterFunction(const char * uniqueID)
    {
        void (*f) (NetSerializer *, SocketMessage *);
        return registeredNonblockingFunctions.Pop(f, uniqueID);
    }
    //------------------------------------------------------------------------
    bool RPC::UnregisterBlockingFunction(const char * uniqueID)
    {
        void (*f) (NetSerializer *, NetSerializer *, SocketMessage *);
        return registeredBlockingFunctions.Pop(f, uniqueID);
    }
    //------------------------------------------------------------------------
    bool RPC::UnregisterLocalCallback(const char * uniqueID, Token messageId)
    {
        bool objectExists;
        Nui32 index, index2;
        LocalCallback * lc;
        RakString str;
        str=uniqueID;
        index = localCallbacks.GetIndexFromKey(messageId, &objectExists);
        if (objectExists)
        {
            lc = localCallbacks[index];
            index2 = lc->functions.GetIndexFromKey(str, &objectExists);
            if (objectExists)
            {
                lc->functions.RemoveAtIndex(index2);
                if (lc->functions.Size()==0)
                {
                    N_delete lc;
                    localCallbacks.RemoveAtIndex(index);
                    return true;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool RPC::UnregisterSlot(const char * sharedIdentifier)
    {
        DataStructures::HashIndex hi = localSlots.GetIndexOf(sharedIdentifier);
        if (hi.IsInvalid()==false)
        {
            LocalSlot * ls = localSlots.ItemAtIndex(hi);
            N_delete ls;
            localSlots.RemoveAtIndex(hi);
            return true;
        }

        return false;
    }
    //------------------------------------------------------------------------
    void RPC::CallLoopback(const char * uniqueID, NetSerializer * bitStream)
    {
        SocketMessage * msg = 0;

        DataStructures::HashIndex skhi = registeredNonblockingFunctions.GetIndexOf(uniqueID);

        if(skhi.IsInvalid() == true)
        {
            if(mPrc2)
                msg = createMessage(sizeof(Token)+sizeof(Nui8)+(Nui32) strlen(uniqueID)+1);
    #if _RAKNET_SUPPORT_NiiNetworkObj == 1 && _RAKNET_SUPPORT_TCPInterface == 1
            else
                msg = mPrc->createMessage(sizeof(Token)+sizeof(Nui8)+(Nui32) strlen(uniqueID)+1);
    #endif

            if(mPrc2)
                msg->mID = mPrc2->getGUID((Address *)0);
    #if _RAKNET_SUPPORT_NiiNetworkObj == 1 && _RAKNET_SUPPORT_TCPInterface == 1
            else
                msg->mID = UniqueID::INVALID;
    #endif

            msg->mAddress=0;
            msg->mAddress.mIndex=(Nindex)-1;
            msg->mData[0]=ID_RPC_REMOTE_ERROR;
            msg->mData[1]=RPC_ERROR_FUNCTION_NOT_REGISTERED;
            strcpy((char*) msg->mData+2, uniqueID);

            addMessage(msg, false);

            return;
        }

        NetSerializer out;
        out.write((Token) ID_RPC_PLUGIN);
        out.write((Token) ID_RPC4_CALL);
        out.writeCompress(uniqueID);
        out.write0(); // nonblocking
        if(bitStream)
        {
            bitStream->seekRead(0);
            out.writeAlign();
            out.write(bitStream);
        }
        if(mPrc2)
            msg = createMessage(out.getSize());
        else
            msg = mPrc->createMessage(out.getSize());

        if (mPrc2)
            msg->mID = mPrc2->getGUID((Address *)0);
        else
            msg->mID = UniqueID::INVALID;
        
        msg->mAddress = 0;
        msg->mAddress.mIndex = (Nindex)-1;
        memcpy(msg->mData, out.getData(), out.getSize());
        addMessage(msg, false);
        return;
    }
    //------------------------------------------------------------------------
    void RPC::Call(const char * uniqueID, NetSerializer * bitStream,
        PacketLevel plevel, PacketType ptype, char pchannel, const UniqueAddress usys, bool broadcast)
    {
        NetSerializer out;
        out.write((Token) ID_RPC_PLUGIN);
        out.write((Token) ID_RPC4_CALL);
        out.writeCompress(uniqueID);
        out.write0(); // Nonblocking
        if (bitStream)
        {
            bitStream->seekRead(0);
            out.writeAlign();
            out.write(bitStream);
        }
        send(&out, plevel, ptype, pchannel, usys, broadcast);
    }
    //------------------------------------------------------------------------
    bool RPC::CallBlocking(const char * uniqueID, NetSerializer * bitStream,
        PacketLevel plevel, PacketType ptype, char pchannel, const UniqueAddress usys, 
            NetSerializer * returnData)
    {
        NetSerializer out;
        out.write((Token) ID_RPC_PLUGIN);
        out.write((Token) ID_RPC4_CALL);
        out.writeCompress(uniqueID);
        out.write1(); // Blocking
        if (bitStream)
        {
            bitStream->seekRead(0);
            out.writeAlign();
            out.write(bitStream);
        }
        N_assert(returnData);
        N_assert(mPrc2);
        ConnectionState cs;
        cs = mPrc2->getState(usys);
        if (cs!=IS_CONNECTED)
            return false;

        send(&out, plevel, ptype, pchannel, usys, false);

        returnData->seekRead(0);
        returnData->seekWrite(0);
        blockingReturnValue.seekRead(0);
        blockingReturnValue.seekWrite(0);
        gotBlockingReturnValue=false;
        SocketMessage * msg;
        DataStructures::Queue<SocketMessage *> packetQueue;
        while (gotBlockingReturnValue==false)
        {
            // TODO - block, filter until gotBlockingReturnValue==true or ID_CONNECTION_LOST or ID_DISCONNECTION_NOTIFICXATION or ID_RPC_REMOTE_ERROR/RPC_ERROR_FUNCTION_NOT_REGISTERED
            Nsleep(30);

            msg = mPrc2->receive();
            if(msg)
            {
                if ((msg->mData[0]==ID_CONNECTION_LOST || msg->mData[0]==ID_DISCONNECTION_NOTIFICATION) &&
                    ((usys.mID!=UniqueID::INVALID && msg->mID==usys.mID) ||
                    (usys.mAddress && !usys.mAddress->isInvalid() && msg->mAddress==usys.mAddress)))
                {
                    // Push back to head in reverse order
                    mPrc2->addMessage(msg, true);
                    while (packetQueue.Size())
                        mPrc2->addMessage(packetQueue.Pop(), true);
                    return false;
                }
                else if(msg->mData[0] == ID_RPC_REMOTE_ERROR && msg->mData[1]==RPC_ERROR_FUNCTION_NOT_REGISTERED)
                {
                    RakString functionName;
                    NetSerializer bsIn(msg->mData, msg->mLength, false);
                    bsIn.skipRead(2, 0);
                    bsIn.read(functionName);
                    if (functionName == uniqueID)
                    {
                        // Push back to head in reverse order
                        mPrc2->addMessage(msg, true);
                        while (packetQueue.Size())
                            mPrc2->addMessage(packetQueue.Pop(), true);
                        return false;
                    }
                    else
                    {
                        packetQueue.PushAtHead(msg, 0);
                    }
                }
                else
                {
                    packetQueue.PushAtHead(msg, 0);
                }
            }
        }

        returnData->write(blockingReturnValue);
        returnData->seekRead(0);
        return true;
    }
    //------------------------------------------------------------------------
    void RPC::Signal(const char * sharedIdentifier, NetSerializer * bitStream,
        PacketLevel plevel, PacketType ptype, char pchannel,
            const UniqueAddress usys, bool broadcast, bool invokeLocal)
    {
        NetSerializer out;
        out.write((Token) ID_RPC_PLUGIN);
        out.write((Token) ID_RPC4_SIGNAL);
        out.writeCompress(sharedIdentifier);
        if (bitStream)
        {
            bitStream->seekRead(0);
            out.writeAlign();
            out.write(bitStream);
        }
        send(&out, plevel, ptype, pchannel, usys, broadcast);

        if (invokeLocal)
        {
            DataStructures::HashIndex functionIndex;
            functionIndex = localSlots.GetIndexOf(sharedIdentifier);
            if (functionIndex.IsInvalid())
                return;

            SocketMessage p;
            p.mID = mPrc2->getGuid();
            p.mAddress = mPrc2->getLAN(0, 0);
            p.mLocal = true;
            NetSerializer * bsptr, bstemp;
            if(bitStream)
            {
                bitStream->seekRead(0);
                p.mLength = bitStream->getSize();
                p.mBitCount = bitStream->getWriteOffset();
                bsptr = bitStream;
            }
            else
            {
                p.mLength = 0;
                p.mBitCount = 0;
                bsptr = &bstemp;
            }

            InvokeSignal(functionIndex, bsptr, &p);
        }
    }
    //------------------------------------------------------------------------
    void RPC::InvokeSignal(DataStructures::HashIndex functionIndex,
        NetSerializer * serializedParameters, SocketMessage * msg)
    {
        if (functionIndex.IsInvalid())
            return;

        interruptSignal=false;
        LocalSlot *localSlot = localSlots.ItemAtIndex(functionIndex);
        Nui32 i;
        i=0;
        while (i < localSlot->slotObjects.Size())
        {
            localSlot->slotObjects[i].functionPointer(serializedParameters, msg);

            // Not threadsafe
            if (interruptSignal==true)
                break;

            serializedParameters->seekRead(0);

            i++;
        }
    }
    //------------------------------------------------------------------------
    void RPC::InterruptSignal(void)
    {
        interruptSignal=true;
    }
    //------------------------------------------------------------------------
    void RPC::onAttach()
    {
        Nui32 i;
        for (i=0; i < globalRegistrationIndex; i++)
        {
            if (globalRegistrationBuffer[i].registerFunctionPointer)
            {
                if (globalRegistrationBuffer[i].callPriority==(int)0xFFFFFFFF)
                    RegisterFunction(globalRegistrationBuffer[i].functionName, globalRegistrationBuffer[i].registerFunctionPointer);
                else
                    RegisterSlot(globalRegistrationBuffer[i].functionName, globalRegistrationBuffer[i].registerFunctionPointer, globalRegistrationBuffer[i].callPriority);
            }
            else if (globalRegistrationBuffer[i].registerBlockingFunctionPointer)
                RegisterBlockingFunction(globalRegistrationBuffer[i].functionName, globalRegistrationBuffer[i].registerBlockingFunctionPointer);
            else
                RegisterLocalCallback(globalRegistrationBuffer[i].functionName, globalRegistrationBuffer[i].messageId);
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType RPC::onMessage(SocketMessage * msg)
    {
        if (msg->mData[0]==ID_RPC_PLUGIN)
        {
            NetSerializer bsIn(msg->mData, msg->mLength, false);
            bsIn.skipRead(2, 0);
            if (msg->mData[1]==ID_RPC4_CALL)
            {
                RakString functionName;
                bsIn.readCompress(functionName);
                bool isBlocking=false;
                bsIn.read(isBlocking);
                if (isBlocking==false)
                {
                    DataStructures::HashIndex skhi = registeredNonblockingFunctions.GetIndexOf(functionName.C_String());
                    if (skhi.IsInvalid())
                    {
                        NetSerializer bsOut;
                        bsOut.write((Nui8) ID_RPC_REMOTE_ERROR);
                        bsOut.write((Nui8) RPC_ERROR_FUNCTION_NOT_REGISTERED);
                        bsOut.write(functionName.C_String(),(Nui32) functionName.GetLength()+1);
                        send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,msg->mAddress,false);
                        return CRT_Destroy;
                    }

                    void ( *fp ) ( NetSerializer *, SocketMessage * );
                    fp = registeredNonblockingFunctions.ItemAtIndex(skhi);
                    bsIn.readAlign();
                    fp(&bsIn,msg);
                }
                else
                {
                    DataStructures::HashIndex skhi = registeredBlockingFunctions.GetIndexOf(functionName.C_String());
                    if (skhi.IsInvalid())
                    {
                        NetSerializer bsOut;
                        bsOut.write((Nui8) ID_RPC_REMOTE_ERROR);
                        bsOut.write((Nui8) RPC_ERROR_FUNCTION_NOT_REGISTERED);
                        bsOut.write(functionName.C_String(),(Nui32) functionName.GetLength()+1);
                        send(&bsOut,PL_HIGH,PT_ORDER_MAKE_SURE,0,msg->mAddress,false);
                        return CRT_Destroy;
                    }

                    void ( *fp ) ( NetSerializer *, NetSerializer *, SocketMessage * );
                    fp = registeredBlockingFunctions.ItemAtIndex(skhi);
                    NetSerializer returnData;
                    bsIn.readAlign();
                    fp(&bsIn, &returnData, msg);

                    NetSerializer out;
                    out.write((Token) ID_RPC_PLUGIN);
                    out.write((Token) ID_RPC4_RETURN);
                    returnData.seekRead(0);
                    out.writeAlign();
                    out.write(returnData);
                    send(&out, PL_RAPID, PT_ORDER_MAKE_SURE, 0, msg->mAddress, false);
                }
            }
            else if (msg->mData[1] == ID_RPC4_SIGNAL)
            {
                RakString sharedIdentifier;
                bsIn.readCompress(sharedIdentifier);
                DataStructures::HashIndex functionIndex;
                functionIndex = localSlots.GetIndexOf(sharedIdentifier);
                NetSerializer serializedParameters;
                bsIn.readAlign();
                bsIn.read(&serializedParameters);
                InvokeSignal(functionIndex, &serializedParameters, msg);
            }
            else
            {
                N_assert(msg->mData[1] == ID_RPC4_RETURN);
                blockingReturnValue.seekWrite(0);
                blockingReturnValue.seekRead(0);
                blockingReturnValue.write(bsIn);
                gotBlockingReturnValue = true;
            }

            return CRT_Destroy;
        }

        bool objectExists;
        Nui32 index, index2;
        index = localCallbacks.GetIndexFromKey(msg->mData[0], &objectExists);
        if (objectExists)
        {
            LocalCallback *lc;
            lc = localCallbacks[index];
            for (index2=0; index2 < lc->functions.Size(); index2++)
            {
                NetSerializer bsIn(msg->mData, msg->mLength, false);

                DataStructures::HashIndex skhi = registeredNonblockingFunctions.GetIndexOf(lc->functions[index2].C_String());
                if (skhi.IsInvalid() == false)
                {
                    void (* fp) (NetSerializer *, SocketMessage *);
                    fp = registeredNonblockingFunctions.ItemAtIndex(skhi);
                    bsIn.readAlign();
                    fp(&bsIn,msg);
                }
            }
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    DataStructures::HashIndex RPC::GetLocalSlotIndex(const char * sharedIdentifier)
    {
        return localSlots.GetIndexOf(sharedIdentifier);
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*
