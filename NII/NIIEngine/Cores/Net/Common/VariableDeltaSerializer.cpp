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
#include "VariableDeltaSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    VariableListDeltaTracker::VariableListDeltaTracker() {nextWriteIndex=0;}
    VariableListDeltaTracker::~VariableListDeltaTracker()
    {
        Nui32 i;
        for (i=0; i < variableList.Size(); i++)
            N_free(variableList[i].lastData);
    }
    //------------------------------------------------------------------------
    // Call before using a series of WriteVar
    void VariableListDeltaTracker::StartWrite(void) {nextWriteIndex=0;}
    //------------------------------------------------------------------------
    void VariableListDeltaTracker::FlagDirtyFromBitArray(Nui8 *bArray)
    {
        Nui16 readOffset = 0;
        for(readOffset = 0; readOffset < variableList.Size(); readOffset++)
        {
            bool result = (bArray[readOffset >> 3] & ( 0x80 >> ( readOffset & 7 ) ) ) !=0;

            if(result == true)
                variableList[readOffset].isDirty=true;
        }
    }
    //------------------------------------------------------------------------
    VariableListDeltaTracker::VariableLastValueNode::VariableLastValueNode()
    {
        lastData=0;
    }
    //------------------------------------------------------------------------
    VariableListDeltaTracker::VariableLastValueNode::VariableLastValueNode(
        const Nui8 * data, int _byteLength)
    {
        lastData = (char *)N_alloc(_byteLength);
        memcpy(lastData,data,_byteLength);
        byteLength=_byteLength;
        isDirty=false;
    }
    //------------------------------------------------------------------------
    VariableListDeltaTracker::VariableLastValueNode::~VariableLastValueNode()
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    VariableDeltaSerializer::VariableDeltaSerializer() {didComparisonThisTick=false;}
    VariableDeltaSerializer::~VariableDeltaSerializer() {RemoveRemoteSystemVariableHistory();}
    //------------------------------------------------------------------------
    VariableDeltaSerializer::SerializationContext::SerializationContext() 
    {
        variableHistoryIdentical = 0; 
        variableHistoryUnique = 0;
    }
    //------------------------------------------------------------------------
    VariableDeltaSerializer::SerializationContext::~SerializationContext() 
    {
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::OnMessageReceipt(UniqueID id, Nui32 receiptId,
        bool messageArrived)
    {
        // Module?
        if (messageArrived)
            FreeVarsAssociatedWithReceipt(id, receiptId);
        else
            DirtyAndFreeVarsAssociatedWithReceipt(id, receiptId);

    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::BeginUnreliableAckedSerialize(SerializationContext * context,
        UniqueID _guid, NetSerializer * _bitStream, Nui32 _sendReceipt)
    {
        N_assert(_guid!=UniqueID::INVALID);
        context->anyVariablesWritten=false;
        context->mID=_guid;
        context->bitStream=_bitStream;
        if (context->variableHistoryUnique==0)
            context->variableHistoryUnique=StartVariableHistoryWrite(_guid);
        context->variableHistory=context->variableHistoryUnique;
        context->sendReceipt=_sendReceipt;
        context->changedVariables = AllocChangedVariablesList();
        context->newSystemSend=false;
        context->serializationMode=PT_RECEIPT_WILL_LOST;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::BeginUniqueSerialize(SerializationContext * context,
        UniqueID _guid, NetSerializer * _bitStream)
    {
        N_assert(_guid != UniqueID::INVALID);
        context->anyVariablesWritten = false;
        context->mID = _guid;
        context->bitStream = _bitStream;
        if (context->variableHistoryUnique == 0)
            context->variableHistoryUnique = StartVariableHistoryWrite(_guid);
        context->variableHistory = context->variableHistoryUnique;
        context->newSystemSend = false;

        context->serializationMode = PT_BE_SURE;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::BeginIdenticalSerialize(SerializationContext * context,
        bool _isFirstSendToRemoteSystem, NetSerializer * _bitStream)
    {
        context->anyVariablesWritten = false;
        context->mID = UniqueID::INVALID;
        context->bitStream = _bitStream;
        context->serializationMode = PT_BE_SURE;
        if(context->variableHistoryIdentical == 0)
            context->variableHistoryIdentical = StartVariableHistoryWrite(UniqueID::INVALID);
        context->variableHistory = context->variableHistoryIdentical;
        context->newSystemSend = _isFirstSendToRemoteSystem;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::EndSerialize(SerializationContext * context)
    {
        if (context->serializationMode == PT_RECEIPT_WILL_LOST)
        {
            if (context->anyVariablesWritten == false)
            {
                context->bitStream->seekWrite(0);
                context->bitStream->seekRead(0);
                FreeChangedVariablesList(context->changedVariables);
                return;
            }

            StoreChangedVariablesList(context->variableHistory, context->changedVariables, context->sendReceipt);
        }
        else
        {
            if (context->variableHistoryIdentical)
            {
                if (didComparisonThisTick == false)
                {
                    didComparisonThisTick = true;
                    identicalSerializationBs.seekWrite(0);
                    identicalSerializationBs.seekRead(0);
                    if (context->anyVariablesWritten == false)
                    {
                        context->bitStream->seekWrite(0);
                        context->bitStream->seekRead(0);
                        return;
                    }

                    identicalSerializationBs.write(context->bitStream);
                    context->bitStream->seekRead(0);
                }
                else
                {
                    context->bitStream->write(&identicalSerializationBs);
                    identicalSerializationBs.seekRead(0);
                }
            }
            else if (context->anyVariablesWritten == false)
            {
                context->bitStream->seekWrite(0);
                context->bitStream->seekRead(0);
                return;
            }
        }
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::BeginDeserialize(DeserializationContext * context,
        NetSerializer * _bitStream)
    {
        context->bitStream=_bitStream;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::EndDeserialize(DeserializationContext *context)
    {
        (void) context;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::AddRemoteSystemVariableHistory(UniqueID id)
    {
        (void) id;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::RemoveRemoteSystemVariableHistory(UniqueID id)
    {
        Nui32 idx,idx2;
        idx = GetVarsWrittenPerRemoteSystemListIndex(id);
        if (idx==(Nui32)-1)
            return;

        if (remoteSystemVariableHistoryList[idx]->mID == id)
        {
            // Memory pool doesn't call destructor
            for (idx2=0; idx2 < remoteSystemVariableHistoryList[idx]->updatedVariablesHistory.Size(); idx2++)
            {
                FreeChangedVariablesList(remoteSystemVariableHistoryList[idx]->updatedVariablesHistory[idx2]);
            }

            delete remoteSystemVariableHistoryList[idx];
            remoteSystemVariableHistoryList.RemoveAtIndexFast(idx);
            return;
        }
    }
    //------------------------------------------------------------------------
    int VariableDeltaSerializer::UpdatedVariablesListPtrComp(const Nui32 & key,
        ChangedVariablesList * const & data)
    {
        if (key<data->sendReceipt)
            return -1;
        if (key==data->sendReceipt)
            return 0;
        return 1;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::FreeVarsAssociatedWithReceipt(UniqueID id, Nui32 receiptId)
    {
        Nui32 idx, idx2;
        idx = GetVarsWrittenPerRemoteSystemListIndex(id);
        if (idx==(Nui32)-1)
            return;

        RemoteSystemVariableHistory* vprs = remoteSystemVariableHistoryList[idx];
        bool objectExists;
        idx2=vprs->updatedVariablesHistory.GetIndexFromKey(receiptId,&objectExists);
        if (objectExists)
        {
            // Free this history node
            FreeChangedVariablesList(vprs->updatedVariablesHistory[idx2]);
            vprs->updatedVariablesHistory.RemoveAtIndex(idx2);
        }
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::DirtyAndFreeVarsAssociatedWithReceipt(UniqueID id, Nui32 receiptId)
    {
        Nui32 idx, idx2;
        idx = GetVarsWrittenPerRemoteSystemListIndex(id);
        if (idx==(Nui32)-1)
            return;

        RemoteSystemVariableHistory* vprs = remoteSystemVariableHistoryList[idx];
        bool objectExists;
        idx2=vprs->updatedVariablesHistory.GetIndexFromKey(receiptId,&objectExists);
        if (objectExists)
        {
            // 'Dirty' all variables sent this update, meaning they will be resent the next time export() is called
            vprs->variableListDeltaTracker.FlagDirtyFromBitArray(vprs->updatedVariablesHistory[idx2]->bitField);

            // Free this history node
            FreeChangedVariablesList(vprs->updatedVariablesHistory[idx2]);
            vprs->updatedVariablesHistory.RemoveAtIndex(idx2);
        }
    }
    //------------------------------------------------------------------------
    Nui32 VariableDeltaSerializer::GetVarsWrittenPerRemoteSystemListIndex(UniqueID id)
    {
        Nui32 idx;
        for (idx=0; idx < remoteSystemVariableHistoryList.Size(); idx++)
        {
            if (remoteSystemVariableHistoryList[idx]->mID == id)
                return idx;
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::RemoveRemoteSystemVariableHistory(void)
    {
        Nui32 idx,idx2;
        for (idx=0; idx < remoteSystemVariableHistoryList.Size(); idx++)
        {
            for (idx2=0; idx2 < remoteSystemVariableHistoryList[idx]->updatedVariablesHistory.Size(); idx2++)
            {
                FreeChangedVariablesList(remoteSystemVariableHistoryList[idx]->updatedVariablesHistory[idx2]);
            }

            delete remoteSystemVariableHistoryList[idx];
        }
        remoteSystemVariableHistoryList.Clear(false);
    }
    //------------------------------------------------------------------------
    VariableDeltaSerializer::RemoteSystemVariableHistory* VariableDeltaSerializer::GetRemoteSystemVariableHistory(UniqueID id)
    {
        Nui32 rshli = GetRemoteSystemHistoryListIndex(id);
        return remoteSystemVariableHistoryList[rshli];
    }
    //------------------------------------------------------------------------
    VariableDeltaSerializer::ChangedVariablesList *VariableDeltaSerializer::AllocChangedVariablesList(void)
    {
        VariableDeltaSerializer::ChangedVariablesList * p = 
            N_new VariableDeltaSerializer::ChangedVariablesList;
        p->bitWriteIndex = 0;
        p->bitField[0] = 0;
        return p;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::FreeChangedVariablesList(ChangedVariablesList * changedVariables)
    {
        N_delete changedVariables;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::StoreChangedVariablesList(RemoteSystemVariableHistory *variableHistory, ChangedVariablesList *changedVariables, Nui32 sendReceipt)
    {
        changedVariables->sendReceipt=sendReceipt;
        variableHistory->updatedVariablesHistory.Insert(changedVariables->sendReceipt,changedVariables,true);
    }
    //------------------------------------------------------------------------
    VariableDeltaSerializer::RemoteSystemVariableHistory *VariableDeltaSerializer::StartVariableHistoryWrite(UniqueID id)
    {
        RemoteSystemVariableHistory *variableHistory;

        Nui32 rshli = GetRemoteSystemHistoryListIndex(id);
        if (rshli==(Nui32) -1)
        {
            variableHistory = new RemoteSystemVariableHistory;
            variableHistory->mID = id;
            remoteSystemVariableHistoryList.Push(variableHistory);
        }
        else
        {
            variableHistory=remoteSystemVariableHistoryList[rshli];
        }

        variableHistory->variableListDeltaTracker.StartWrite();
        return variableHistory;
    }
    //------------------------------------------------------------------------
    Nui32 VariableDeltaSerializer::GetRemoteSystemHistoryListIndex(UniqueID id)
    {
        // Find the variable tracker for the target system
        Nui32 idx;
        for (idx=0; idx < remoteSystemVariableHistoryList.Size(); idx++)
        {
            if (remoteSystemVariableHistoryList[idx]->mID == id)
            {
                return idx;
            }
        }
        return (Nui32) -1;
    }
    //------------------------------------------------------------------------
    void VariableDeltaSerializer::OnPreSerializeTick(void)
    {
        didComparisonThisTick=false;
    }
    //------------------------------------------------------------------------
}
}