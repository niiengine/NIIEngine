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

#ifndef _NII_NET_RPC_H_
#define _NII_NET_RPC_H_

#include "NiiNetPreInclude.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"
#include "RakString.h"
#include "NetworkIDObject.h"
#include "DS_Hash.h"
#include "DS_OrderedList.h"

/// \defgroup RPC_PLUGIN_GROUP RPC
/// \brief Remote procedure calls, without external dependencies.
/// \details This should not be used at the same time as RPC3. This is a less functional version of RPC3, and is here for users that do not want the Boost dependency of RPC3.
/// \ingroup PLUGINS_GROUP

namespace NII
{
namespace NII_NET
{
    /// \brief Error codes returned by a remote system as to why an RPC function call cannot execute
    /// \details Error code follows msg ID ID_RPC_REMOTE_ERROR, that is msg->mData[1]<BR>
    /// Name of the function will be appended starting at msg->mData[2]
    /// \ingroup RPC_PLUGIN_GROUP
    enum RPCErrorCodes
    {
        /// Named function was not registered with RegisterFunction(). Check your spelling.
        RPC_ERROR_FUNCTION_NOT_REGISTERED,
    };

    /// \brief Instantiate this class globally if you want to register a function with RPC at the global space
    class _EngineAPI RPC4GlobalRegistration
    {
    public:
        /// \brief Queue a call to RPC::RegisterFunction() globally. Actual call occurs once RPC is attached to an instance of NetworkSys or NetworkObj.
        RPC4GlobalRegistration(const char * uniqueID, void (*functionPointer)(NetSerializer * userData, SocketMessage * msg));

        /// \brief Queue a call to RPC::RegisterSlot() globally. Actual call occurs once RPC is attached to an instance of NetworkSys or NetworkObj.
        RPC4GlobalRegistration(const char * uniqueID, void (*functionPointer)(NetSerializer * userData, SocketMessage * msg), int callPriority);

        /// \brief Queue a call to RPC::RegisterBlockingFunction() globally. Actual call occurs once RPC is attached to an instance of NetworkSys or NetworkObj.
        RPC4GlobalRegistration(const char * uniqueID, void (*functionPointer)(NetSerializer * userData, NetSerializer * returnData, SocketMessage * msg));

        /// \brief Queue a call to RPC::RegisterLocalCallback() globally. Actual call occurs once RPC is attached to an instance of NetworkSys or NetworkObj.
        RPC4GlobalRegistration(const char * uniqueID, Token messageId);
    };

    /// \brief The RPC plugin is just an association between a C function pointer and a string.
    /// \details It is for users that want to use RPC, but do not want to use boost.
    /// You do not have the automatic serialization or other features of RPC3, and C++ member calls are not supported.
    /// \note You cannot use RPC at the same time as RPC3Plugin
    /// \ingroup RPC_PLUGIN_GROUP
    class _EngineAPI RPC : public Plugin
    {
    public:
        // Constructor
        RPC();

        // Destructor
        virtual ~RPC();

        /// \deprecated Use RegisterSlot
        /// \brief Register a function pointer to be callable from a remote system
        /// \details The hash of the function name will be stored as an association with the function pointer
        /// When a call is made to call this function from the \a Call() or CallLoopback() function, the function pointer will be invoked with the passed bitStream to Call() and the actual SocketMessage that RakNet got.
        /// \sa RegisterPacketCallback()
        /// \param[in] uniqueID Identifier to be associated with \a functionPointer. If this identifier is already in use, the call will return false.
        /// \param[in] functionPointer C function pointer to be called
        /// \return True if the hash of uniqueID is not in use, false otherwise.
        bool RegisterFunction(const char * uniqueID, void (*functionPointer)(NetSerializer * userData, SocketMessage * msg));

        /// Register a slot, which is a function pointer to one or more implementations that supports this function signature
        /// When a signal occurs, all slots with the same identifier are called.
        /// \param[in] sharedIdentifier A string to identify the slot. Recommended to be the same as the name of the function.
        /// \param[in] functionPtr Pointer to the function. For C, just pass the name of the function. For C++, use ARPC_REGISTER_CPP_FUNCTION
        /// \param[in] callPriority Slots are called by order of the highest callPriority first. For slots with the same plevel, they are called in the order they are registered
        void RegisterSlot(const char * sharedIdentifier, void (*functionPointer)(NetSerializer * userData, SocketMessage * msg), int callPriority);

        /// \brief Same as \a RegisterFunction, but is called with CallBlocking() instead of Call() and returns a value to the caller
        bool RegisterBlockingFunction(const char * uniqueID, void (*functionPointer)(NetSerializer *userData, NetSerializer * returnData, SocketMessage * msg));

        /// \deprecated Use RegisterSlot and invoke on self only when the msg you want arrives
        /// When a RakNet SocketMessage with the specified identifier is returned, execute CallLoopback() on a function previously registered with RegisterFunction()
        /// For example, you could call "OnClosedConnection" whenever you get ID_DISCONNECTION_NOTIFICATION or ID_CONNECTION_LOST
        /// \param[in] uniqueID Identifier passed to RegisterFunction()
        /// \param[in] messageId What RakNet msg ID to call on, for example ID_DISCONNECTION_NOTIFICATION or ID_CONNECTION_LOST
        void RegisterLocalCallback(const char * uniqueID, Token messageId);

        /// \brief Unregister a function pointer previously registered with RegisterFunction()
        /// \param[in] Identifier originally passed to RegisterFunction()
        /// \return True if the hash of uniqueID was in use, and hence removed. false otherwise.
        bool UnregisterFunction(const char * uniqueID);

        /// \brief Same as UnregisterFunction, except for a blocking function
        bool UnregisterBlockingFunction(const char * uniqueID);

        /// Remove the association created with RegisterPacketCallback()
        /// \param[in] uniqueID Identifier passed as uniqueID to RegisterLocalCallback()
        /// \param[in] messageId Identifier passed as messageId to RegisterLocalCallback()
        /// \return True if the combination of uniqueID and messageId was in use, and hence removed
        bool UnregisterLocalCallback(const char * uniqueID, Token messageId);

        /// Remove the association created with RegisterSlot()
        /// \param[in] sharedIdentifier Identifier passed as sharedIdentifier to RegisterSlot()
        bool UnregisterSlot(const char * sharedIdentifier);

        /// \deprecated Use RegisterSlot() and Signal() with your own UniqueID as the send target
        /// send to the attached instance of NetworkSys. See LinkBase::send()
        /// \param[in] Identifier originally passed to RegisterFunction() on the local system
        /// \param[in] bitStream bitStream encoded data to send to the function callback
        void CallLoopback(const char * uniqueID, NetSerializer * bitStream);

        /// \deprecated, use Signal()
        /// send to the specified remote instance of NetworkSys.
        /// \param[in] uniqueID Identifier originally passed to RegisterFunction() on the remote system(s)
        /// \param[in] bitStream bitStream encoded data to send to the function callback
        /// \param[in] plevel See LinkBase::send()
        /// \param[in] ptype See LinkBase::send()
        /// \param[in] pchannel See LinkBase::send()
        /// \param[in] usys See LinkBase::send()
        /// \param[in] broadcast See LinkBase::send()
        void Call(const char * uniqueID, NetSerializer * bitStream,
            PacketLevel plevel, PacketType ptype, char pchannel,
                const UniqueAddress usys, bool broadcast);

        /// \brief Same as call, but don't return until the remote system replies.
        /// Broadcasting parameter does not exist, this can only call one remote system
        /// \note This function does not return until the remote system responds, disconnects, or was never connected to begin with
        /// \param[in] Identifier originally passed to RegisterBlockingFunction() on the remote system(s)
        /// \param[in] bitStream bitStream encoded data to send to the function callback
        /// \param[in] plevel See LinkBase::send()
        /// \param[in] ptype See LinkBase::send()
        /// \param[in] pchannel See LinkBase::send()
        /// \param[in] usys See LinkBase::send()
        /// \param[out] returnData Written to by the function registered with RegisterBlockingFunction.
        /// \return true if successfully called. False on disconnect, function not registered, or not connected to begin with
        bool CallBlocking(const char * uniqueID, NetSerializer * bitStream,
            PacketLevel plevel, PacketType ptype, char pchannel,
                const UniqueAddress usys, NetSerializer * returnData);

        /// Calls zero or more functions identified by sharedIdentifier registered with RegisterSlot()
        /// \param[in] sharedIdentifier parameter of the same name passed to RegisterSlot() on the remote system
        /// \param[in] bitStream bitStream encoded data to send to the function callback
        /// \param[in] plevel See LinkBase::send()
        /// \param[in] ptype See LinkBase::send()
        /// \param[in] pchannel See LinkBase::send()
        /// \param[in] usys See LinkBase::send()
        /// \param[in] broadcast See LinkBase::send()
        /// \param[in] invokeLocal If true, also sends to self.
        void Signal(const char * sharedIdentifier, NetSerializer * bitStream,
            PacketLevel plevel, PacketType ptype, char pchannel,
                const UniqueAddress usys, bool broadcast, bool invokeLocal);

        /// If called while processing a slot, no further slots for the currently executing signal will be executed
        void InterruptSignal(void);

        /// \internal
        struct LocalCallback
        {
            Token messageId;
            DataStructures::OrderedList<RakString,RakString> functions;
        };
        static int LocalCallbackComp(const Token & key, LocalCallback * const & data);

        /// \internal
        // Callable object, along with plevel to call relative to other objects
        struct LocalSlotObject
        {
            LocalSlotObject() {}
            LocalSlotObject(Nui32 _registrationCount,int _callPriority, void (*_functionPointer ) ( NetSerializer *userData, SocketMessage *msg ))
            {registrationCount=_registrationCount;callPriority=_callPriority;functionPointer=_functionPointer;}
            ~LocalSlotObject() {}

            // Used so slots are called in the order they are registered
            Nui32 registrationCount;
            int callPriority;
            void (*functionPointer)(NetSerializer * userData, SocketMessage * msg);
        };

        static int LocalSlotObjectComp(const LocalSlotObject & key, const LocalSlotObject & data);

        /// \internal
        struct LocalSlot
        {
            DataStructures::OrderedList<LocalSlotObject,LocalSlotObject,LocalSlotObjectComp> slotObjects;
        };
        DataStructures::Hash<RakString, LocalSlot*,256, RakString::ToInteger> localSlots;
    protected:
        virtual void onAttach();
        virtual ConnectReceiveType onMessage(SocketMessage * msg);

        DataStructures::Hash<RakString, void (*)(NetSerializer *, SocketMessage *),64, RakString::ToInteger> registeredNonblockingFunctions;
        DataStructures::Hash<RakString, void (*)(NetSerializer *, NetSerializer *, SocketMessage * ),64, RakString::ToInteger> registeredBlockingFunctions;
        DataStructures::OrderedList<Token,LocalCallback*,RPC::LocalCallbackComp> localCallbacks;

        NetSerializer blockingReturnValue;
        bool gotBlockingReturnValue;

        DataStructures::HashIndex GetLocalSlotIndex(const char * sharedIdentifier);

        /// Used so slots are called in the order they are registered
        Nui32 nextSlotRegistrationCount;

        bool interruptSignal;

        void InvokeSignal(DataStructures::HashIndex functionIndex, NetSerializer * serializedParameters, SocketMessage *msg);
    };
}
}
#endif

#endif