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
#ifndef __VARIABLE_DELTA_SERIALIZER_H
#define __VARIABLE_DELTA_SERIALIZER_H

#include "NiiNetSerializer.h"
#include "PacketLevel.h"
#include "DS_OrderedList.h"
#include "DS_List.h"

namespace NII
{
namespace NII_NET
{
    /// Class to write a series of variables, copy the contents to memory, and return if the newly written value is different than what was last written
    /// Can also encode the reads, writes, and results directly to/from a bitstream
    class VariableListDeltaTracker
    {
    public:
        VariableListDeltaTracker();
        ~VariableListDeltaTracker();

        // Call before using a series of WriteVar
        void StartWrite(void);

        bool IsPastEndOfList(void) const {return nextWriteIndex>=variableList.Size();}

        /// Records the passed value of the variable to memory, and returns true if the value is different from the write before that (or if it is the first write)
        /// \pre Call StartWrite() before doing the first of a series of calls to WriteVar or other functions that call WriteVar
        /// \note Variables must be of the same type, written in the same order, each time
        template <class VarType>
        bool WriteVar(const VarType &varData)
        {
            NetSerializer temp;
            temp.write(varData);
            if (nextWriteIndex>=variableList.Size())
            {
                variableList.Push(VariableLastValueNode(temp.getData(),temp.getSize()));
                nextWriteIndex++;
                return true; // Different because it's new
            }

            if (temp.getSize()!=variableList[nextWriteIndex].byteLength)
            {
                variableList[nextWriteIndex].lastData = (char *)N_realloc(variableList[nextWriteIndex].lastData, temp.getSize());// ÐèÒªÐÞ¸Ä
                variableList[nextWriteIndex].byteLength=temp.getSize();
                memcpy(variableList[nextWriteIndex].lastData,temp.getData(),temp.getSize());
                nextWriteIndex++;
                variableList[nextWriteIndex].isDirty=false;
                return true; // Different because the serialized size is different
            }
            if (variableList[nextWriteIndex].isDirty==false && memcmp(temp.getData(),variableList[nextWriteIndex].lastData, variableList[nextWriteIndex].byteLength)==0)
            {
                nextWriteIndex++;
                return false; // Same because not dirty and memcmp is the same
            }

            variableList[nextWriteIndex].isDirty=false;
            memcpy(variableList[nextWriteIndex].lastData,temp.getData(),temp.getSize());
            nextWriteIndex++;
            return true; // Different because dirty or memcmp was different
        }
        /// Calls WriteVar. If the variable has changed, writes true, and writes the variable. Otherwise writes false.
        template <class VarType>
        bool WriteVarToBitstream(const VarType &varData, NetSerializer *bitStream)
        {
            bool wasDifferent = WriteVar(varData);
            bitStream->write(wasDifferent);
            if (wasDifferent)
            {
                bitStream->write(varData);
                return true;
            }
            return false;
        }
        /// Calls WriteVarToBitstream(). Additionally, adds the boolean result of WriteVar() to boolean bit array
        template <class VarType>
        bool WriteVarToBitstream(const VarType &varData, NetSerializer *bitStream, Nui8 *bArray, Nui16 writeOffset)
        {
            if (WriteVarToBitstream(varData,bitStream)==true)
            {
                NBitCount numberOfBitsMod8 = writeOffset & 7;

                if ( numberOfBitsMod8 == 0 )
                    bArray[ writeOffset >> 3 ] = 0x80;
                else
                    bArray[ writeOffset >> 3 ] |= 0x80 >> ( numberOfBitsMod8 ); // Set the bit to 1

                return true;
            }
            else
            {
                if ( ( writeOffset & 7 ) == 0 )
                    bArray[ writeOffset >> 3 ] = 0;

                return false;
            }
        }

        /// Paired with a call to WriteVarToBitstream(), will read a variable if it had changed. Otherwise the values remains the same.
        template <class VarType>
        static bool ReadVarFromBitstream(VarType &varData, NetSerializer *bitStream)
        {
            bool wasWritten;
            if (bitStream->read(wasWritten)==false)
                return false;
            if (wasWritten)
            {
                if (bitStream->read(varData)==false)
                    return false;
            }
            return wasWritten;
        }

        /// Variables flagged dirty will cause WriteVar() to return true, even if the variable had not otherwise changed
        /// This updates all the variables in the list, where in each index \a varsWritten is true, so will the variable at the corresponding index be flagged dirty
        void FlagDirtyFromBitArray(Nui8 *bArray);

        /// \internal
        struct VariableLastValueNode
        {
            VariableLastValueNode();
            VariableLastValueNode(const Nui8 *data, int _byteLength);
            ~VariableLastValueNode();
            char *lastData;
            Nui32 byteLength;
            bool isDirty;
        };

    protected:
        /// \internal
        DataStructures::List<VariableLastValueNode> variableList;
        /// \internal
        Nui32 nextWriteIndex;
    };
    
    /// \brief Class to compare memory values of variables in a current state to a prior state
    /// Results of the comparisons will be written to a bitStream, such that only changed variables get written<BR>
    /// Can be used with ReplicaManager3 to export a Replica3 per-variable, rather than comparing the entire object against itself<BR>
    /// Usage:<BR>
    ///<BR>
    /// 1. Call BeginUnreliableAckedSerialize(), BeginUniqueSerialize(), or BeginIdenticalSerialize(). In the case of Replica3, this would be in the export() call<BR>
    /// 2. For each variable of the type in step 1, call export(). The same variables must be serialized every tick()<BR>
    /// 3. Call EndSerialize()<BR>
    /// 4. Repeat step 1 for each of the other categories of how to send varaibles<BR>
    ///<BR>
    /// On the receiver:<BR>
    ///<BR>
    /// 1. Call BeginDeserialize(). In the case of Replica3, this would be in the import() call<BR>
    /// 2. Call DeserializeVariable() for each variable, in the same order as was Serialized()<BR>
    /// 3. Call EndSerialize()<BR>
    /// \sa The ReplicaManager3 sample
    class _EngineAPI VariableDeltaSerializer
    {
    protected:
        struct RemoteSystemVariableHistory;
        struct ChangedVariablesList;

    public:
        VariableDeltaSerializer();
        ~VariableDeltaSerializer();

        struct SerializationContext
        {
            SerializationContext();
            ~SerializationContext();

            UniqueID mID;
            NetSerializer * bitStream;
            Nui32 rakPeerSendReceipt;
            RemoteSystemVariableHistory * variableHistory;
            RemoteSystemVariableHistory * variableHistoryIdentical;
            RemoteSystemVariableHistory * variableHistoryUnique;
            ChangedVariablesList * changedVariables;
            Nui32 sendReceipt;
            PacketType serializationMode;
            bool anyVariablesWritten;
            bool newSystemSend; // Force send all, do not record
        };

        struct DeserializationContext
        {
            NetSerializer *bitStream;
        };

        /// \brief Call before doing one or more SerializeVariable calls when the data will be sent PT_RECEIPT_WILL_LOST
        /// The last value of each variable will be saved per remote system. Additionally, a history of \a _sendReceipts is stored to determine what to resend on packetloss.
        /// When variables are lost, they will be flagged dirty and always resent to the system that lost it
        /// Disadvantages: Every variable for every remote system is copied internally, in addition to a history list of what variables changed for which \a _sendReceipt. Very memory and CPU intensive for multiple connections.
        /// Advantages: When data needs to be resent by RakNet, RakNet can only resend the value it currently has. This allows the application to control the resend, sending the most recent value of the variable. The end result is that bandwidth is used more efficiently because old data is never sent.
        /// \pre Upon getting ID_SND_RECEIPT_LOSS or ID_SND_RECEIPT_ACKED call OnMessageReceipt()
        /// \pre AddRemoteSystemVariableHistory() and RemoveRemoteSystemVariableHistory() must be called for new and lost connections
        /// \param[in] context Holds the context of this group of serialize calls. This can be a stack object just passed to the function.
        /// \param[in] _guid Which system we are sending to
        /// \param[in] _bitSteam Which bitStream to write to
        /// \param[in] _sendReceipt and passed to the send() or SendLists() function. Identifies this update for ID_SND_RECEIPT_LOSS and ID_SND_RECEIPT_ACKED
        void BeginUnreliableAckedSerialize(SerializationContext *context, UniqueID _guid, NetSerializer *_bitStream, Nui32 _sendReceipt);

        /// \brief Call before doing one or more SerializeVariable calls for data that may be sent differently to every remote system (such as an invisibility flag that only teammates can see)
        /// The last value of each variable will be saved per remote system.
        /// Unlike BeginUnreliableAckedSerialize(), send receipts are not necessary
        /// Disadvantages: Every variable for every remote system is copied internally. Very memory and CPU intensive for multiple connections.
        /// Advantages: When data is sent differently depending on the recipient, this system can make things easier to use and is as efficient as it can be.
        /// \pre AddRemoteSystemVariableHistory() and RemoveRemoteSystemVariableHistory() must be called for new and lost connections
        /// \param[in] context Holds the context of this group of serialize calls. This can be a stack object just passed to the function.
        /// \param[in] _guid Which system we are sending to
        /// \param[in] _bitSteam Which bitStream to write to
        void BeginUniqueSerialize(SerializationContext *context, UniqueID _guid, NetSerializer *_bitStream);

        /// \brief Call before doing one or more SerializeVariable calls for data that is sent with the same value to every remote system (such as health, position, etc.)
        /// This is the most common type of serialization, and also the most efficient
        /// Disadvantages: A copy of every variable still needs to be held, although only once
        /// Advantages: After the first serialization, the last serialized bitStream will be used for subsequent sends
        /// \pre Call OnPreSerializeTick() before doing any calls to BeginIdenticalSerialize() for each of your objects, once per game tick
        /// \param[in] context Holds the context of this group of serialize calls. This can be a stack object just passed to the function.
        /// \param[in] _isFirstSerializeToThisSystem Pass true if this is the first time ever serializing to this system (the initial download). This way all variables will be written, rather than checking against prior sent values.
        /// \param[in] _bitSteam Which bitStream to write to
        void BeginIdenticalSerialize(SerializationContext *context, bool _isFirstSerializeToThisSystem, NetSerializer *_bitStream);

        /// \brief Call after BeginUnreliableAckedSerialize(), BeginUniqueSerialize(), or BeginIdenticalSerialize(), then after calling SerializeVariable() one or more times
        /// \param[in] context Same context pointer passed to BeginUnreliableAckedSerialize(), BeginUniqueSerialize(), or BeginIdenticalSerialize()
        void EndSerialize(SerializationContext *context);

        /// \brief Call when you receive the NetSerializer written by SerializeVariable(), before calling DeserializeVariable()
        /// \param[in] context Holds the context of this group of deserialize calls. This can be a stack object just passed to the function.
        /// \param[in] _bitStream Pass the bitStream originally passed to and written to by serialize calls
        void BeginDeserialize(DeserializationContext *context, NetSerializer *_bitStream);

        /// \param[in] context Same context pointer passed to BeginDeserialize()
        void EndDeserialize(DeserializationContext *context);

        /// BeginUnreliableAckedSerialize() and BeginUniqueSerialize() require knowledge of when connections are added and dropped
        /// Call AddRemoteSystemVariableHistory() and RemoveRemoteSystemVariableHistory() to notify the system of these events
        /// \param[in] _guid Which system we are sending to
        void AddRemoteSystemVariableHistory(UniqueID id);

        /// BeginUnreliableAckedSerialize() and BeginUniqueSerialize() require knowledge of when connections are added and dropped
        /// Call AddRemoteSystemVariableHistory() and RemoveRemoteSystemVariableHistory() to notify the system of these events
        /// \param[in] _guid Which system we are sending to
        void RemoveRemoteSystemVariableHistory(UniqueID id);

        /// BeginIdenticalSerialize() requires knowledge of when serialization has started for an object across multiple systems
        /// This way it can setup the flag to do new comparisons against the last sent values, rather than just resending the last sent bitStream
        /// For Replica3, overload and call this from Replica3::OnUserReplicaPreSerializeTick()
        void OnPreSerializeTick(void);

        /// Call when getting ID_SND_RECEIPT_LOSS or ID_SND_RECEIPT_ACKED for a particular system
        /// Example:
        ///
        /// Nui32 msgNumber;
        /// memcpy(&msgNumber, msg->mData + 1, 4);
        /// DataStructures::List<Replica3 *> replicaListOut;
        /// replicaManager.GetReplicasCreatedByMe(replicaListOut);
        /// NCount idx;
        /// for (idx = 0; idx < replicaListOut.GetSize(); ++idx)
        /// {
        /// 	((SampleReplica*)replicaListOut[idx])->NotifyReplicaOfMessageDeliveryStatus(msg->mID,msgNumber, msg->mData[0]==ID_SND_RECEIPT_ACKED);
        /// }
        ///
        /// \param[in] id Which system we are sending to
        /// \param[in] receiptId Encoded in bytes 1-4 inclusive of ID_SND_RECEIPT_LOSS and ID_SND_RECEIPT_ACKED
        /// \param[in] messageArrived True for ID_SND_RECEIPT_ACKED, false otherwise
        void OnMessageReceipt(UniqueID id, Nui32 receiptId, bool messageArrived);

        /// Call to export a variable
        /// Will write to the bitSteam passed to \a context true, variableValue if the variable has changed or has never been written. Otherwise will write false.
        /// \pre You have called BeginUnreliableAckedSerialize(), BeginUniqueSerialize(), or BeginIdenticalSerialize()
        /// \pre Will also require calling OnPreSerializeTick() if using BeginIdenticalSerialize()
        /// \note Be sure to call EndSerialize() after finishing all serializations
        /// \param[in] context Same context pointer passed to BeginUnreliableAckedSerialize(), BeginUniqueSerialize(), or BeginIdenticalSerialize()
        /// \param[in] variable A variable to write to the bitStream passed to \a context
        template <class VarType>
        void SerializeVariable(SerializationContext *context, const VarType &variable)
        {
            if (context->newSystemSend)
            {
                if (context->variableHistory->variableListDeltaTracker.IsPastEndOfList()==false)
                {
                    // previously sent data to another system
                    context->bitStream->write1();
                    context->bitStream->write(variable);
                    context->anyVariablesWritten=true;
                }
                else
                {
                    // never sent data to another system
                    context->variableHistory->variableListDeltaTracker.WriteVarToBitstream(variable, context->bitStream);
                    context->anyVariablesWritten = true;
                }
            }
            else if (context->serializationMode == PT_RECEIPT_WILL_LOST)
            {
                context->anyVariablesWritten |=
                context->variableHistory->variableListDeltaTracker.WriteVarToBitstream(variable, context->bitStream, context->changedVariables->bitField, context->changedVariables->bitWriteIndex++);
            }
            else
            {
                if (context->variableHistoryIdentical)
                {
                    // Identical serialization to a number of systems
                    if (didComparisonThisTick == false)
                        context->anyVariablesWritten |=
                        context->variableHistory->variableListDeltaTracker.WriteVarToBitstream(variable, context->bitStream);
                    // Else bitstream is written to at the end
                }
                else
                {
                    // Per-system serialization
                    context->anyVariablesWritten |=
                        context->variableHistory->variableListDeltaTracker.WriteVarToBitstream(variable, context->bitStream);
                }
            }
        }

        /// Call to deserialize into a variable
        /// \pre You have called BeginDeserialize()
        /// \note Be sure to call EndDeserialize() after finishing all deserializations
        /// \param[in] context Same context pointer passed to BeginDeserialize()
        /// \param[in] variable A variable to write to the bitStream passed to \a context
        template <class VarType>
        bool DeserializeVariable(DeserializationContext * context, VarType & variable)
        {
            return VariableListDeltaTracker::ReadVarFromBitstream(variable, context->bitStream);
        }
    protected:

        // For a given send receipt from NetworkSys::send() track which variables we updated
        // That way if that send does not arrive (ID_SND_RECEIPT_LOSS) we can mark those variables as dirty to resend them with current values
        struct ChangedVariablesList
        {
            Nui32 sendReceipt;
            Nui16 bitWriteIndex;
            Nui8 bitField[56];
        };

        static int UpdatedVariablesListPtrComp(const Nui32 & key, ChangedVariablesList * const & data);

        // For each remote system, track the last values of variables we sent to them, and the history of what values changed per call to send()
        // Every serialize if a variable changes from its last value, send it out again
        // Also if a send does not arrive (ID_SND_RECEIPT_LOSS) we use updatedVariablesHistory to mark those variables as dirty, to resend them unreliably with the current values
        struct RemoteSystemVariableHistory
        {
            UniqueID mID;
            VariableListDeltaTracker variableListDeltaTracker;
            DataStructures::OrderedList<Nui32,ChangedVariablesList *,VariableDeltaSerializer::UpdatedVariablesListPtrComp> updatedVariablesHistory;
        };
        /// A list of RemoteSystemVariableHistory indexed by guid, one per connection that we serialize to
        /// List is added to when SerializeConstruction is called, and removed from when SerializeDestruction is called, or when a given connection is dropped
        DataStructures::List<RemoteSystemVariableHistory *> remoteSystemVariableHistoryList;

        bool didComparisonThisTick;
        NetSerializer identicalSerializationBs;

        void FreeVarsAssociatedWithReceipt(UniqueID id, Nui32 receiptId);
        void DirtyAndFreeVarsAssociatedWithReceipt(UniqueID id, Nui32 receiptId);
        Nui32 GetVarsWrittenPerRemoteSystemListIndex(UniqueID id);
        void RemoveRemoteSystemVariableHistory(void);

        RemoteSystemVariableHistory* GetRemoteSystemVariableHistory(UniqueID id);

        ChangedVariablesList *AllocChangedVariablesList(void);
        void FreeChangedVariablesList(ChangedVariablesList *changedVariables);
        void StoreChangedVariablesList(RemoteSystemVariableHistory *variableHistory, ChangedVariablesList *changedVariables, Nui32 sendReceipt);

        RemoteSystemVariableHistory *StartVariableHistoryWrite(UniqueID id);
        Nui32 GetRemoteSystemHistoryListIndex(UniqueID id);

    };
}
}
#endif