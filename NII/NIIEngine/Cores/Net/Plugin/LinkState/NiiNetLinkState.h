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
#if NII_LinkState == 1

#ifndef _NII_NET_LINK_STATE_H_
#define _NII_NET_LINK_STATE_H_

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "DS_List.h"
#include "DS_WeightedGraph.h"

namespace NII
{
namespace NII_NET
{
    /// \brief A one hop connection graph.
    /// \details Sends ID_REMOTE_CONNECTION_LOST, ID_REMOTE_DISCONNECTION_NOTIFICATION, ID_REMOTE_NEW_INCOMING_CONNECTION<BR>
    /// All identifiers are followed by Address, then UniqueID
    /// Also stores the list for you, which you can access with GetConnectionListForRemoteSystem
    /// \ingroup CONNECTION_GRAPH_GROUP
    class _EngineAPI LinkState : public Plugin
    {
    public:
        LinkState();
        ~LinkState();

        /// \brief Given a remote system identified by UniqueID, return the list of SystemAddresses and GUIDs they are connected to
        /// \param[in] remoteSystemGuid Which system we are referring to. This only works for remote systems, not ourselves.
        /// \param[out] saOut A preallocated array to hold the output list of Address. Can be 0 if you don't care.
        /// \param[out] guidOut A preallocated array to hold the output list of UniqueID. Can be 0 if you don't care.
        /// \param[in,out] outLength On input, the size of \a saOut and \a guidOut. On output, modified to reflect the number of elements actually written
        /// \return True if \a remoteSystemGuid was found. Otherwise false, and \a saOut, \a guidOut remain unchanged. \a outLength will be set to 0.
        bool GetConnectionListForRemoteSystem(UniqueID remoteSystemGuid, Address * saOut,
            UniqueID * guidOut, Nui32 * outLength);

        /// Returns if g1 is connected to g2
        bool ConnectionExists(UniqueID g1, UniqueID g2);

        /// Returns the average ping between two systems in the connection graph. Returns -1 if no connection exists between those systems
        Nui16 GetPingBetweenSystems(UniqueID g1, UniqueID g2) const;

        /// Returns the system with the lowest average ping among all its connections.
        /// If you need one system in the peer to peer group to relay data, have the FullyConnected host call this function after host migration, and use that system
        UniqueID GetLowestAveragePingSystem(void) const;

        /// \brief If called with false, then new connections are only added to the connection graph when you call ProcessNewConnection();
        /// \details This is useful if you want to perform validation before connecting a system to a mesh, or if you want a submesh (for example a server cloud)
        /// \param[in] b True to automatically call ProcessNewConnection() on any new connection, false to not do so. Defaults to true.
        void SetAutoProcessNewConnections(bool b);

        /// \brief Returns value passed to SetAutoProcessNewConnections()
        /// \return Value passed to SetAutoProcessNewConnections(), or the default of true if it was never called
        bool GetAutoProcessNewConnections(void) const;

        /// \brief If you call SetAutoProcessNewConnections(false);, then you will need to manually call ProcessNewConnection() on new connections
        /// \details On ID_NEW_INCOMING_CONNECTION or ID_CONNECTION_REQUEST_ACCEPTED, adds that system to the graph
        /// Do not call ProcessNewConnection() manually otherwise
        /// \param[in] address The msg->Address member
        /// \param[in] id The msg->mID member
        void AddParticipant(const Address & address, UniqueID id);

        /// Get the participants added with AddParticipant()
        /// \param[out] participantList Participants added with AddParticipant();
        void GetParticipantList(DataStructures::OrderedList<UniqueID, UniqueID> &participantList);

        /// \internal
        struct SystemAddressAndGuid
        {
            Address * mAddress;
            UniqueID mID;
            Nui16 sendersPingToThatSystem;
        };
        /// \internal
        static int SystemAddressAndGuidComp(const SystemAddressAndGuid & key, const SystemAddressAndGuid & data);

        /// \internal
        struct RemoteSystem
        {
            DataStructures::OrderedList<SystemAddressAndGuid,SystemAddressAndGuid,LinkState::SystemAddressAndGuidComp> remoteConnections;
            UniqueID mID;
        };
        /// \internal
        static int RemoteSystemComp(const UniqueID & key, RemoteSystem * const & data);
    protected:
        /// \internal
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );
        /// \internal
        virtual void onConnect(const Address & address, UniqueID id);
        /// \internal
        virtual ConnectReceiveType onMessage(SocketMessage * msg);

        // List of systems I am connected to, which in turn stores which systems they are connected to
        DataStructures::OrderedList<UniqueID, RemoteSystem *, LinkState::RemoteSystemComp> mRemotes;

        bool autoProcessNewConnections;
    };
}
}
#endif
#endif