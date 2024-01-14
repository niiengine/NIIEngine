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


#ifndef __RPC_3_H
#define __RPC_3_H

// Most of the internals of the boost code to make this work
#include "RPC3_Boost.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"
#include "RakString.h"
#include "NetworkIDObject.h"
#include "DS_Hash.h"
#include "DS_OrderedList.h"

/// \defgroup RPC_3_GROUP RPC3
/// \brief Remote procedure calls, powered by the 3rd party library Boost
/// \details
/// \ingroup PLUGINS_GROUP

namespace NII
{
namespace NII_NET
{
    class LinkBase;
    class NetworkIDManager;

    /// \ingroup RPC_3_GROUP
    #define RPC3_REGISTER_FUNCTION(RPC3Instance, _FUNCTION_PTR_ ) (RPC3Instance)->RegisterFunction((#_FUNCTION_PTR_), (_FUNCTION_PTR_))

    /// \brief Error codes returned by a remote system as to why an RPC function call cannot execute
    /// \details Error code follows msg ID ID_RPC_REMOTE_ERROR, that is msg->data[1]<BR>
    /// Name of the function will be appended starting at msg->data[2]
    /// \ingroup RPC_3_GROUP
    enum RPCErrorCodes
    {
        /// RPC3::SetNetworkIDManager() was not called, and it must be called to call a C++ object member
        RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE,

        /// Cannot execute C++ object member call because the object specified by SetRecipientObject() does not exist on this system
        RPC_ERROR_OBJECT_DOES_NOT_EXIST,

        /// Internal error, index optimization for function lookup does not exist
        RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE,

        /// Named function was not registered with RegisterFunction(). Check your spelling.
        RPC_ERROR_FUNCTION_NOT_REGISTERED,

        /// Named function was registered, but later unregistered with UnregisterFunction() and can no longer be called.
        RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED,

        /// SetRecipientObject() was not called before Call(), but the registered pointer is a class member
        /// If you intended to call a class member function, call SetRecipientObject() with a valid object first.
        RPC_ERROR_CALLING_CPP_AS_C,

        /// SetRecipientObject() was called before Call(), but RegisterFunction() was called with isObjectMember=false
        /// If you intended to call a C function, call SetRecipientObject(UNASSIGNED_NETWORK_ID) first.
        RPC_ERROR_CALLING_C_AS_CPP,
    };

    /// \brief The RPC3 plugin allows you to call remote functions as if they were local functions, using the standard function call syntax
    /// \details No serialization or deserialization is needed.<BR>
    /// As of this writing, the system is not threadsafe.<BR>
    /// Features:<BR>
    /// <LI>Pointers to classes that derive from NetworkID are automatically looked up using NetworkIDManager
    /// <LI>Types are written to NetSerializer, meaning built-in serialization operations are performed, including endian swapping
    /// <LI>Types can customize autoserialization by providing an implementation of operator << and operator >> to and from NetSerializer
    /// \note You cannot use RPC at the same time as RPC3
    class RPC3 : public Plugin
    {
    public:
        // Constructor
        RPC3();

        // Destructor
        virtual ~RPC3();

        /// Sets the network ID manager to use for object lookup
        /// Required to call C++ object member functions via SetRecipientObject()
        /// \param[in] idMan Pointer to the network ID manager to use
        void SetNetworkIDManager(NetworkIDManager *idMan);

        /// Register a function pointer as callable using RPC()
        /// \param[in] uniqueIdentifier String identifying the function. Recommended that this is the name of the function
        /// \param[in] functionPtr Pointer to the function. For C, just pass the name of the function. For C++, use ARPC_REGISTER_CPP_FUNCTION
        /// \return True on success, false on uniqueIdentifier already used
        template<typename Function>
        bool RegisterFunction(const char * uniqueIdentifier, Function functionPtr)
        {
            if (IsFunctionRegistered(uniqueIdentifier)) return false;
            _RPC3::FunctionPointer fp;
            fp= _RPC3::GetBoundPointer(functionPtr);
            localFunctions.Push(uniqueIdentifier,OP_NEW_1<LocalRPCFunction>(fp));
            return true;
        }

        /// \internal
        // Callable object, along with plevel to call relative to other objects
        struct LocalSlotObject
        {
            LocalSlotObject() {}
            LocalSlotObject(NetworkID _associatedObject,unsigned int _registrationCount,int _callPriority,_RPC3::FunctionPointer _functionPointer)
            {associatedObject=_associatedObject;registrationCount=_registrationCount;callPriority=_callPriority;functionPointer=_functionPointer;}
            ~LocalSlotObject() {}

            // Used so slots are called in the order they are registered
            NetworkID associatedObject;
            unsigned int registrationCount;
            int callPriority;
            _RPC3::FunctionPointer functionPointer;
        };
        /// \internal
        /// Identifies an RPC function, by string identifier and if it is a C or C++ function
        typedef RakString RPCIdentifier;
        static int LocalSlotObjectComp(const LocalSlotObject & key,
            const LocalSlotObject & data);
        /// \internal
        struct LocalSlot
        {
            DataStructures::OrderedList<LocalSlotObject,LocalSlotObject,LocalSlotObjectComp> slotObjects;
        };
        /// Register a slot, which is a function pointer to one or more instances of a class that supports this function signature
        /// When a signal occurs, all slots with the same identifier are called.
        /// \param[in] sharedIdentifier A string to identify the slot. Recommended to be the same as the name of the function.
        /// \param[in] functionPtr Pointer to the function. For C, just pass the name of the function. For C++, use ARPC_REGISTER_CPP_FUNCTION
        /// \param[in] objectInstance If 0, then this slot is just a regular C function. Otherwise, this is a member of the given class instance.
        /// \param[in] callPriority Slots are called by order of the highest callPriority first. For slots with the same plevel, they are called in the order they are registered
        template<typename Function>
        void RegisterSlot(const char * sharedIdentifier, Function functionPtr, NetworkID objectInstanceId, int callPriority)
        {
            _RPC3::FunctionPointer fp;
            fp= _RPC3::GetBoundPointer(functionPtr);
            LocalSlotObject lso(objectInstanceId, nextSlotRegistrationCount++, callPriority, _RPC3::GetBoundPointer(functionPtr));
            DataStructures::HashIndex idx = GetLocalSlotIndex(sharedIdentifier);
            LocalSlot * localSlot;
            if (idx.IsInvalid())
            {
                localSlot = OP_NEW<LocalSlot>();
                localSlots.Push(sharedIdentifier, localSlot);
            }
            else
            {
                localSlot = localSlots.ItemAtIndex(idx);
            }
            localSlot->slotObjects.Insert(lso,lso,true);
        }

        /// Unregisters a function pointer to be callable given an identifier for the pointer
        /// \param[in] uniqueIdentifier String identifying the function.
        /// \return True on success, false on function was not previously or is not currently registered.
        bool UnregisterFunction(const char * uniqueIdentifier);

        /// Returns if a function identifier was previously registered on this system with RegisterFunction(), and not unregistered with UnregisterFunction()
        /// \param[in] uniqueIdentifier String identifying the function.
        /// \return True if the function was registered, false otherwise
        bool IsFunctionRegistered(const char * uniqueIdentifier);

        /// send or stop sending a timestamp with all following calls to Call()
        /// Use GetLastSenderTimestamp() to read the timestamp.
        /// \param[in] timeStamp Non-zero to pass this timestamp using the ID_TIMESTAMP system. 0 to clear passing a timestamp.
        void SetTimestamp(TimeDurMS timeStamp);

        /// Set parameters to pass to NetworkSys::send() for all following calls to Call()
        /// Deafults to HIGH_PRIORITY, RELIABLE_ORDERED, ordering channel 0
        /// \param[in] plevel See NetworkSys::send()
        /// \param[in] ptype See NetworkSys::send()
        /// \param[in] pchannel See NetworkSys::send()
        void SetSendParams(PacketLevel plevel, PacketType ptype, char pchannel);

        /// Set system to send to for all following calls to Call()
        /// Defaults to Address::invalid, broadcast=true
        /// \param[in] address See NetworkSys::send()
        /// \param[in] broadcast See NetworkSys::send()
        void SetRecipientAddress(const Address &address, bool broadcast);

        /// Set the NetworkID to pass for all following calls to Call()
        /// Defaults to UNASSIGNED_NETWORK_ID (none)
        /// If set, the remote function will be considered a C++ function, e.g. an object member function
        /// If set to UNASSIGNED_NETWORK_ID (none), the remote function will be considered a C function
        /// If this is set incorrectly, you will get back either RPC_ERROR_CALLING_C_AS_CPP or RPC_ERROR_CALLING_CPP_AS_C
        /// \sa NetworkIDManager
        /// \param[in] networkID Returned from NetworkIDObject::GetNetworkID()
        void SetRecipientObject(NetworkID networkID);

        /// If the last received function call has a timestamp included, it is stored and can be retrieved with this function.
        /// \return 0 if the last call did not have a timestamp, else non-zero
        TimeDurMS GetLastSenderTimestamp(void) const;

        /// Returns the system address of the last system to send us a received function call
        /// Equivalent to the old system RPCParameters::sender
        /// \return Last system to send an RPC call using this system
        Address * GetLastSenderAddress(void) const;

        /// If called while processing a slot, no further slots for the currently executing signal will be executed
        void InterruptSignal(void);

        /// Returns the instance of NetworkSys this plugin was attached to
        LinkBase *GetRakPeer(void) const;

        /// Returns the currently running RPC call identifier, set from RegisterFunction::uniqueIdentifier
        /// Returns an empty string "" if none
        /// \return which RPC call is currently running
        const char *GetCurrentExecution(void) const;

        /// Calls a remote function, using as send parameters whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
        /// If you call a C++ class member function, don't forget to first call SetRecipientObject(). You can use CallExplicit() instead of Call() to force yourself not to forget.
        ///
        /// Parameters passed to Call are processed as follows:
        /// 1. If the parameter is not a pointer
        /// 2. - And you overloaded NetSerializer& operator<<(NetSerializer& out, MyClass& in) then that will be used to do the serialization
        /// 3. - Otherwise, it will use bitStream.write(myClass); NetSerializer already defines specializations for NetworkIDObject, Address, other Serializer
        /// 4. If the parameter is a pointer
        /// 5. - And the pointer can be converted to NetworkIDObject, then it will write bitStream.write(myClass->GetNetworkID()); To make it also dereference the pointer, use _RPC3::Deref(myClass)
        /// 6. - And the pointer can not be converted to NetworkID, but it is a pointer to RPC3, then it is skipped
        /// 7. Otherwise, the pointer is dereferenced and written as in step 2 and 3.
        ///
        /// \note If you need endian swapping (Mac talking to PC for example), you pretty much need to define operator << and operator >> for all classes you want to serialize. Otherwise the member variables will not be endian swapped.
        /// \note If the call fails on the remote system, you will get back ID_RPC_REMOTE_ERROR. msg->data[1] will contain one of the values of RPCErrorCodes. msg->data[2] and on will contain the name of the function.
        ///
        /// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
        bool Call(const char *uniqueIdentifier){
            NetSerializer bitStream;
            return SendCallOrSignal(uniqueIdentifier, 0, &bitStream, true);
        }
        template <class P1>
        bool Call(const char *uniqueIdentifier, P1 &p1)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            return SendCallOrSignal(uniqueIdentifier, 1, &bitStream, true);
        }
        template <class P1, class P2>
        bool Call(const char *uniqueIdentifier, P1 &p1, P2 &p2)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            return SendCallOrSignal(uniqueIdentifier, 2, &bitStream, true);
        }
        template <class P1, class P2, class P3>
        bool Call(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            return SendCallOrSignal(uniqueIdentifier, 3, &bitStream, true);
        }
        template <class P1, class P2, class P3, class P4>
        bool Call(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            return SendCallOrSignal(uniqueIdentifier, 4, &bitStream, true);
        }
        template <class P1, class P2, class P3, class P4, class P5>
        bool Call(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            return SendCallOrSignal(uniqueIdentifier, 5, &bitStream, true);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6>
        bool Call(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            return SendCallOrSignal(uniqueIdentifier, 6, &bitStream, true);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
        bool Call(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            _RPC3::SerializeCallParameterBranch<P7>::type::apply(bitStream, p7);
            return SendCallOrSignal(uniqueIdentifier, 7, &bitStream, true);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
        bool Call(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            _RPC3::SerializeCallParameterBranch<P7>::type::apply(bitStream, p7);
            _RPC3::SerializeCallParameterBranch<P8>::type::apply(bitStream, p8);
            return SendCallOrSignal(uniqueIdentifier, 8, &bitStream, true);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
        bool Call(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            _RPC3::SerializeCallParameterBranch<P7>::type::apply(bitStream, p7);
            _RPC3::SerializeCallParameterBranch<P8>::type::apply(bitStream, p8);
            _RPC3::SerializeCallParameterBranch<P9>::type::apply(bitStream, p9);
            return SendCallOrSignal(uniqueIdentifier, 9, &bitStream, true);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
        bool Call(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9, P10 &p10)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            _RPC3::SerializeCallParameterBranch<P7>::type::apply(bitStream, p7);
            _RPC3::SerializeCallParameterBranch<P8>::type::apply(bitStream, p8);
            _RPC3::SerializeCallParameterBranch<P9>::type::apply(bitStream, p9);
            _RPC3::SerializeCallParameterBranch<P10>::type::apply(bitStream, p10);
            return SendCallOrSignal(uniqueIdentifier, 10, &bitStream, true);
        }

        struct CallExplicitParameters
        {
            CallExplicitParameters(
                NetworkID _networkID=UNASSIGNED_NETWORK_ID, Address * address=0,
                bool _broadcast=true, TimeDurMS _timeStamp=0, PacketLevel plevel=HIGH_PRIORITY,
                PacketType ptype=RELIABLE_ORDERED, char pchannel = 0) :
                    networkID(_networkID),
                    mAddress(address),
                    broadcast(_broadcast),
                    timeStamp(_timeStamp),
                    mPacketLevel(plevel),
                    mPacketType(ptype),
                    mPacketCh(pchannel){}
            NetworkID networkID;
            Address * mAddress;
            bool broadcast;
            TimeDurMS timeStamp;
            PacketLevel mPacketLevel;
            PacketType mPacketType;
            char mPacketCh;
        };

        /// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
        /// Passed parameter(s), if any, are serialized using operator << with NetSerializer. If you provide an overload it will be used, otherwise the seriailzation is equivalent to memcpy except for native RakNet types (NetworkIDObject, Address, etc.)
        /// If the type is a pointer to a type deriving from NetworkIDObject, then only the NetworkID is sent, and the object looked up on the remote system. Otherwise, the pointer is dereferenced and the contents serialized as usual.
        /// \note The this pointer, for this instance of RPC3, is pushed as the last parameter on the stack. See RPC3Sample.cpp for an example of this
        /// \note If the call fails on the remote system, you will get back ID_RPC_REMOTE_ERROR. msg->data[1] will contain one of the values of RPCErrorCodes. msg->data[2] and on will contain the name of the function.
        /// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
        /// \param[in] timeStamp See SetTimestamp()
        /// \param[in] plevel See SetSendParams()
        /// \param[in] mPacketType See SetSendParams()
        /// \param[in] orderingChannel See SetSendParams()
        /// \param[in] mAddress See SetRecipientAddress()
        /// \param[in] broadcast See SetRecipientAddress()
        /// \param[in] networkID See SetRecipientObject()
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters){
            SetTimestamp(callExplicitParameters->timeStamp);
            SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
            SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
            SetRecipientObject(callExplicitParameters->networkID);
            return Call(uniqueIdentifier);
        }
        template <class P1 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1);
        }
        template <class P1, class P2 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1, P2 &p2
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1, p2);
        }
        template <class P1, class P2, class P3 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1, p2, p3);
        }
        template <class P1, class P2, class P3, class P4 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1, p2, p3, p4);
        }
        template <class P1, class P2, class P3, class P4, class P5 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1, p2, p3, p4, p5);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1, p2, p3, p4, p5, p6);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1, p2, p3, p4, p5, p6, p7);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1, p2, p3, p4, p5, p6, p7, p8);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1, p2, p3, p4, p5, p6, p7, p8, p9);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10 >
        bool CallExplicit(const char *uniqueIdentifier, const CallExplicitParameters * const callExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9, P10 &p10
            )	{
                SetTimestamp(callExplicitParameters->timeStamp);
                SetSendParams(callExplicitParameters->mPacketLevel, callExplicitParameters->mPacketType, callExplicitParameters->orderingChannel);
                SetRecipientAddress(callExplicitParameters->mAddress, callExplicitParameters->broadcast);
                SetRecipientObject(callExplicitParameters->networkID);
                return Call(uniqueIdentifier, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        }

        bool CallC(const char *uniqueIdentifier) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier);}

        template <class P1>
        bool CallC(const char *uniqueIdentifier, P1 &p1) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1);}

        template <class P1, class P2>
        bool CallC(const char *uniqueIdentifier, P1 &p1, P2 &p2) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1,p2);}

        template <class P1, class P2, class P3>
        bool CallC(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1,p2,p3);}

        template <class P1, class P2, class P3, class P4>
        bool CallC(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1,p2,p3,p4);}

        template <class P1, class P2, class P3, class P4, class P5>
        bool CallC(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1,p2,p3,p4,p5);}

        template <class P1, class P2, class P3, class P4, class P5, class P6>
        bool CallC(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6);}

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
        bool CallC(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6,p7);}

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
        bool CallC(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6,p7,p8);}

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
        bool CallC(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6,p7,p8,p9);}

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
        bool CallC(const char *uniqueIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9, P10 &p10) {SetRecipientObject(UNASSIGNED_NETWORK_ID); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);}

        bool CallCPP(const char *uniqueIdentifier, NetworkID nid) { SetRecipientObject(nid); return Call(uniqueIdentifier); }

        template <class P1>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1) { SetRecipientObject(nid); return Call(uniqueIdentifier,p1); }

        template <class P1, class P2>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1, P2 &p2) { SetRecipientObject(nid); return Call(uniqueIdentifier,p1,p2); }

        template <class P1, class P2, class P3>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1, P2 &p2, P3 &p3) { SetRecipientObject(nid); return Call(uniqueIdentifier,p1,p2,p3); }

        template <class P1, class P2, class P3, class P4>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1, P2 &p2, P3 &p3, P4 &p4) { SetRecipientObject(nid); return Call(uniqueIdentifier,p1,p2,p3,p4); }

        template <class P1, class P2, class P3, class P4, class P5>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5) { SetRecipientObject(nid); return Call(uniqueIdentifier,p1,p2,p3,p4,p5); }

        template <class P1, class P2, class P3, class P4, class P5, class P6>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6) { SetRecipientObject(nid); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6); }

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7) { SetRecipientObject(nid); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6,p7); }

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8) { SetRecipientObject(nid); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6,p7,p8); }

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9) { SetRecipientObject(nid); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6,p7,p8,p9); }

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
        bool CallCPP(const char *uniqueIdentifier, NetworkID nid, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9, P10 &p10)	{ SetRecipientObject(nid); return Call(uniqueIdentifier,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10); }

        // ---------------------------- Signals and slots ----------------------------------

        /// Calls zero or more functions identified by sharedIdentifier.
        /// Uses as send parameters whatever was last passed to SetTimestamp(), SetSendParams(), and SetRecipientAddress()
        /// You can use CallExplicit() instead of Call() to force yourself not to forget to set parameters
        ///
        /// See the Call() function for a description of parameters
        ///
        /// \param[in] sharedIdentifier parameter of the same name passed to RegisterSlot() on the remote system
        bool Signal(const char *sharedIdentifier){
            NetSerializer bitStream;
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 0, &bitStream, false);
        }
        template <class P1>
        bool Signal(const char *sharedIdentifier, P1 &p1)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 1, &bitStream, false);
        }
        template <class P1, class P2>
        bool Signal(const char *sharedIdentifier, P1 &p1, P2 &p2)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 2, &bitStream, false);
        }
        template <class P1, class P2, class P3>
        bool Signal(const char *sharedIdentifier, P1 &p1, P2 &p2, P3 &p3)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 3, &bitStream, false);
        }
        template <class P1, class P2, class P3, class P4>
        bool Signal(const char *sharedIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 4, &bitStream, false);
        }
        template <class P1, class P2, class P3, class P4, class P5>
        bool Signal(const char *sharedIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 5, &bitStream, false);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6>
        bool Signal(const char *sharedIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 6, &bitStream, false);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
        bool Signal(const char *sharedIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            _RPC3::SerializeCallParameterBranch<P7>::type::apply(bitStream, p7);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 7, &bitStream, false);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
        bool Signal(const char *sharedIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            _RPC3::SerializeCallParameterBranch<P7>::type::apply(bitStream, p7);
            _RPC3::SerializeCallParameterBranch<P8>::type::apply(bitStream, p8);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 8, &bitStream, false);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
        bool Signal(const char *sharedIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            _RPC3::SerializeCallParameterBranch<P7>::type::apply(bitStream, p7);
            _RPC3::SerializeCallParameterBranch<P8>::type::apply(bitStream, p8);
            _RPC3::SerializeCallParameterBranch<P9>::type::apply(bitStream, p9);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 9, &bitStream, false);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10>
        bool Signal(const char *sharedIdentifier, P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7, P8 &p8, P9 &p9, P10 &p10)	{
            NetSerializer bitStream;
            _RPC3::SerializeCallParameterBranch<P1>::type::apply(bitStream, p1);
            _RPC3::SerializeCallParameterBranch<P2>::type::apply(bitStream, p2);
            _RPC3::SerializeCallParameterBranch<P3>::type::apply(bitStream, p3);
            _RPC3::SerializeCallParameterBranch<P4>::type::apply(bitStream, p4);
            _RPC3::SerializeCallParameterBranch<P5>::type::apply(bitStream, p5);
            _RPC3::SerializeCallParameterBranch<P6>::type::apply(bitStream, p6);
            _RPC3::SerializeCallParameterBranch<P7>::type::apply(bitStream, p7);
            _RPC3::SerializeCallParameterBranch<P8>::type::apply(bitStream, p8);
            _RPC3::SerializeCallParameterBranch<P9>::type::apply(bitStream, p9);
            _RPC3::SerializeCallParameterBranch<P10>::type::apply(bitStream, p10);
            InvokeSignal(GetLocalSlotIndex(sharedIdentifier), &bitStream, true);
            return SendCallOrSignal(sharedIdentifier, 10, &bitStream, false);
        }

        struct SignalExplicitParameters
        {
            SignalExplicitParameters(Address * address = 0, bool _broadcast=true, 
                TimeDurMS _timeStamp=0, PacketLevel plevel = HIGH_PRIORITY,
                PacketType ptype=RELIABLE_ORDERED, char pchannel = 0): 
                    mAddress(address), broadcast(_broadcast), timeStamp(_timeStamp), mPacketLevel(plevel), mPacketType(ptype), orderingChannel(pchannel)
            {}
            Address * mAddress;
            bool broadcast;
            TimeDurMS timeStamp;
            PacketLevel mPacketLevel;
            PacketType mPacketType;
            char orderingChannel;
        };

        /// Same as Signal(), but you are forced to specify the remote system parameters
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters){
            SetTimestamp(signalExplicitParameters->timeStamp);
            SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
            SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
            return Signal(sharedIdentifier);
        }
        template <class P1 >
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 &p1
            )	{
                SetTimestamp(signalExplicitParameters->timeStamp);
                SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
                SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
                return Signal(sharedIdentifier, p1);
        }
        template <class P1, class P2 >
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 &p1, P2 &p2
            )	{
                SetTimestamp(signalExplicitParameters->timeStamp);
                SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
                SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
                return Signal(sharedIdentifier, p1, p2);
        }
        template <class P1, class P2, class P3 >
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3
            )	{
                SetTimestamp(signalExplicitParameters->timeStamp);
                SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
                SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
                return Signal(sharedIdentifier, p1, p2, p3);
        }
        template <class P1, class P2, class P3, class P4 >
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4
            )	{
                SetTimestamp(signalExplicitParameters->timeStamp);
                SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
                SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
                return Signal(sharedIdentifier, p1, p2, p3, p4);
        }
        template <class P1, class P2, class P3, class P4, class P5 >
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5
            )	{
                SetTimestamp(signalExplicitParameters->timeStamp);
                SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
                SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
                return Signal(sharedIdentifier, p1, p2, p3, p4, p5);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6 >
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6
            )	{
                SetTimestamp(signalExplicitParameters->timeStamp);
                SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
                SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
                return Signal(sharedIdentifier, p1, p2, p3, p4, p5, p6);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7 >
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 &p1, P2 &p2, P3 &p3, P4 &p4, P5 &p5, P6 &p6, P7 &p7
            )	{
                SetTimestamp(signalExplicitParameters->timeStamp);
                SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
                SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
                return Signal(sharedIdentifier, p1, p2, p3, p4, p5, p6, p7);
        }
        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8 >
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 & p1, P2 & p2, P3 & p3, P4 & p4, P5 & p5, P6 & p6, P7 & p7, P8 & p8)
        {
                SetTimestamp(signalExplicitParameters->timeStamp);
                SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
                SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
                return Signal(sharedIdentifier, p1, p2, p3, p4, p5, p6, p7, p8);
        }

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9 >
        bool SignalExplicit(const char *sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 & p1, P2 & p2, P3 & p3, P4 & p4, P5 & p5, P6 & p6, P7 & p7, P8 & p8, P9 & p9)
        {
            SetTimestamp(signalExplicitParameters->timeStamp);
            SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
            SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
            return Signal(sharedIdentifier, p1, p2, p3, p4, p5, p6, p7, p8, p9);
        }

        template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9, class P10 >
        bool SignalExplicit(const char * sharedIdentifier, const SignalExplicitParameters * const signalExplicitParameters,
            P1 & p1, P2 & p2, P3 & p3, P4 & p4, P5 & p5, P6 & p6, P7 & p7, P8 & p8, P9 & p9, P10 &p10)
        {
            SetTimestamp(signalExplicitParameters->timeStamp);
            SetSendParams(signalExplicitParameters->mPacketLevel, signalExplicitParameters->mPacketType, signalExplicitParameters->orderingChannel);
            SetRecipientAddress(signalExplicitParameters->mAddress, signalExplicitParameters->broadcast);
            return Signal(sharedIdentifier, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        }

        /// \internal
        /// The RPC identifier, and a pointer to the function
        struct LocalRPCFunction
        {
            LocalRPCFunction() {}
            LocalRPCFunction(_RPC3::FunctionPointer _functionPointer)
            {
                functionPointer = _functionPointer;
            };
            _RPC3::FunctionPointer functionPointer;
        };

        /// \internal
        /// Sends the RPC call, with a given serialized function
        bool SendCallOrSignal(RakString uniqueIdentifier, char parameterCount,
            NetSerializer * serializedParameters, bool isCall);

        /// Call a given signal with a bitstream representing the parameter list
        void InvokeSignal(DataStructures::HashIndex functionIndex,
            NetSerializer * serializedParameters, bool temporarilySetUSA);
    protected:
        void onAttach();
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        virtual void OnRPC3Call(const Address &address, unsigned char *data, unsigned int lengthInBytes);
        virtual void onConnectClose(const Address &address, UniqueID id, ConnectCloseType lostConnectionReason );
        virtual void shutdown();

        void Clear();

        void SendError(Address * target, unsigned char errorCode, const char *functionName);
        DataStructures::HashIndex GetLocalFunctionIndex(RPCIdentifier identifier);
        DataStructures::HashIndex GetLocalSlotIndex(const char *sharedIdentifier);

        DataStructures::Hash<RakString, LocalSlot*,256, RakString::ToInteger> localSlots;
        DataStructures::Hash<RakString, LocalRPCFunction*,256, RakString::ToInteger> localFunctions;

        TimeDurMS outgoingTimestamp;
        PacketLevel outgoingPriority;
        PacketType outgoingReliability;
        char outgoingOrderingChannel;
        Address * outgoingSystemAddress;
        bool outgoingBroadcast;
        NetworkID outgoingNetworkID;
        NetSerializer outgoingExtraData;

        TimeDurMS incomingTimeStamp;
        Address * incomingSystemAddress;
        NetSerializer incomingExtraData;

        NetworkIDManager *networkIdManager;
        char currentExecution[512];

        /// Used so slots are called in the order they are registered
        unsigned int nextSlotRegistrationCount;

        bool interruptSignal;
    };
}
}
#endif