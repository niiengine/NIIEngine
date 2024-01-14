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
#if _RAKNET_SUPPORT_MessageFilter==1

#ifndef __MESSAGE_FILTER_PLUGIN_H
#define __MESSAGE_FILTER_PLUGIN_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "DS_OrderedList.h"
#include "DS_Hash.h"

/// MessageIdentifier (ID_*) values shoudln't go higher than this.  Change it if you do.
#define MESSAGE_FILTER_MAX_MESSAGE_ID 256

namespace NII
{
namespace NII_NET
{
    /// \internal Has to be public so some of the shittier compilers can use it.
    int _EngineAPI MessageFilterStrComp(char * const & key,char * const & data);

    /// \internal Has to be public so some of the shittier compilers can use it.
    struct FilterSet
    {
        bool banOnFilterTimeExceed;
        bool kickOnDisallowedMessage;
        bool banOnDisallowedMessage;
        TimeDurMS disallowedMessageBanTimeMS;
        TimeDurMS timeExceedBanTimeMS;
        TimeDurMS maxMemberTimeMS;
        void (*invalidMessageCallback)(LinkBase *peer, UniqueAddress address, int filterSetID, void *userData, Nui8 messageID);
        void *disallowedCallbackUserData;
        void (*timeoutCallback)(LinkBase *peer, UniqueAddress address, int filterSetID, void *userData);
        void *timeoutUserData;
        int filterSetID;
        bool allowedIDs[MESSAGE_FILTER_MAX_MESSAGE_ID];
        DataStructures::OrderedList<RakString,RakString> allowedRPC4;
    };

    /// \internal Has to be public so some of the shittier compilers can use it.
    int _EngineAPI FilterSetComp(const int & key, FilterSet * const & data);

    /// \internal Has to be public so some of the shittier compilers can use it.
    struct FilteredSystem
    {
        FilterSet * filter;
        TimeDurMS timeEnteredThisSet;
    };

    /// \defgroup MESSAGEFILTER_GROUP MsgFilter
    /// \brief Remote incoming packets from unauthorized systems
    /// \details
    /// \ingroup PLUGINS_GROUP

    /// \brief Assigns systems to FilterSets.  Each FilterSet limits what kinds of messages are allowed.
    /// \details The MsgFilter plugin is used for security where you limit what systems can send what kind of messages.<BR>
    /// You implicitly define FilterSets, and add allowed message IDs to these FilterSets.<BR>
    /// You then add systems to these filters, such that those systems are limited to sending what the filters allows.<BR>
    /// You can automatically assign systems to a filter.<BR>
    /// You can automatically kick and possibly ban users that stay in a filter too long, or send the wrong message.<BR>
    /// Each system is a member of either zero or one filters.<BR>
    /// Add this plugin before any plugin you wish to filter (most likely just add this plugin before any other).
    /// \ingroup MESSAGEFILTER_GROUP
    class _EngineAPI MsgFilter : public Plugin
    {
    public:
        MsgFilter();
        virtual ~MsgFilter();

        /// Automatically add all new systems to a particular filter
        /// Defaults to -1
        /// \param[in] filterSetID Which filter to add new systems to.  <0 for do not add.
        void SetAutoAddNewConnectionsToFilter(int filterSetID);

        /// Allow a range of message IDs
        /// Always allowed by default: ID_CONNECTION_REQUEST_ACCEPTED through ID_DOWNLOAD_PROGRESS
        /// Usually you specify a range to make it easier to add new enumerations without having to constantly refer back to this function.
        /// \param[in] allow True to allow this message ID, false to disallow. By default, all messageIDs except the noted types are disallowed.  This includes messages from other plugins!
        /// \param[in] messageIDStart The first ID_* message to allow in the range.  Inclusive.
        /// \param[in] messageIDEnd The last ID_* message to allow in the range.  Inclusive.
        /// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
        void SetAllowMessageID(bool allow, int messageIDStart, int messageIDEnd, int filterSetID);

        /// Allow a specific RPC call
        /// \pre MsgFilter must be attached before RPC
        /// \param[in] uniqueID Identifier passed to RegisterFunction()
        /// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
        void SetAllowRPC4(bool allow, const char * uniqueID, int filterSetID);

        /// What action to take on a disallowed message.  You can kick or not.  You can add them to the ban list for some time
        /// By default no action is taken.  The message is simply ignored.
        /// param[in] 0 for permanent ban, >0 for ban time in milliseconds.
        /// \param[in] kickOnDisallowed kick the system that sent a disallowed message.
        /// \param[in] banOnDisallowed ban the system that sent a disallowed message.  See \a banTimeMS for the ban duration
        /// \param[in] banTimeMS Passed to the milliseconds parameter of Ban::add.
        /// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
        void SetActionOnDisallowedMessage(bool kickOnDisallowed, bool banOnDisallowed,
            TimeDurMS banTimeMS, int filterSetID);

        /// Set a user callback to be called on an invalid message for a particular filterSet
        /// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
        /// \param[in] userData A pointer passed with the callback
        /// \param[in] invalidMessageCallback A pointer to a C function to be called back with the specified parameters.
        void SetDisallowedMessageCallback(int filterSetID, void * userData,
            void (*invalidMessageCallback)(LinkBase * peer, UniqueAddress addressOrGUID, int filterSetID, void *userData, Nui8 messageID));

        /// Set a user callback to be called when a user is disconnected due to SetFilterMaxTime
        /// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
        /// \param[in] userData A pointer passed with the callback
        /// \param[in] invalidMessageCallback A pointer to a C function to be called back with the specified parameters.
        void SetTimeoutCallback(int filterSetID, void * userData, void (*invalidMessageCallback)(LinkBase * peer, UniqueAddress addressOrGUID, int filterSetID, void *userData));

        /// Limit how long a connection can stay in a particular filterSetID. After this time, the connection is kicked and possibly banned.
        /// By default there is no limit to how long a connection can stay in a particular filter set.
        /// \param[in] allowedTimeMS How many milliseconds to allow a connection to stay in this filter set.
        /// \param[in] banOnExceed True or false to ban the system, or not, when \a allowedTimeMS is exceeded
        /// \param[in] banTimeMS Passed to the milliseconds parameter of Ban::add.
        /// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
        void SetFilterMaxTime(int allowedTimeMS, bool banOnExceed, TimeDurMS banTimeMS,
            int filterSetID);

        /// Get the filterSetID a system is using.  Returns -1 for none.
        /// \param[in] addressOrGUID The system we are referring to
        int GetSystemFilterSet(UniqueAddress addressOrGUID);

        /// Assign a system to a filter set.
        /// Systems are automatically added to filter sets (or not) based on SetAutoAddNewConnectionsToFilter()
        /// This function is used to change the filter set a system is using, to add it to a new filter set, or to remove it from all existin filter sets.
        /// \param[in] addressOrGUID The system we are referring to
        /// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.  If -1, the system will be removed from all filter sets.
        void SetSystemFilterSet(UniqueAddress addressOrGUID, int filterSetID);

        /// Returns the number of systems subscribed to a particular filter set
        /// Using anything other than -1 for \a filterSetID is slow, so you should store the returned value.
        /// \param[in] filterSetID The filter set to limit to.  Use -1 for none (just returns the total number of filter systems in that case).
        Nui32 GetSystemCount(int filterSetID) const;

        /// Returns the total number of filter sets.
        /// \return The total number of filter sets.
        Nui32 GetFilterSetCount(void) const;

        /// Returns the ID of a filter set, by index
        /// \param[in] An index between 0 and GetFilterSetCount()-1 inclusive
        int GetFilterSetIDByIndex(Nui32 index);

        /// Delete a FilterSet.  All systems formerly subscribed to this filter are now unrestricted.
        /// \param[in] filterSetID The ID of the filter set to delete.
        void DeleteFilterSet(int filterSetID);

        virtual void update();
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        virtual void onConnect(const Address & address, UniqueID id);
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );
    protected:
        void Clear(void);
        void DeallocateFilterSet(FilterSet *filterSet);
        FilterSet* GetFilterSetByID(int filterSetID);
        void OnInvalidMessage(FilterSet *filterSet, UniqueAddress address, Nui8 messageID);

        DataStructures::OrderedList<int, FilterSet*, FilterSetComp> filterList;
        // Change to guid
        DataStructures::Hash<UniqueAddress, FilteredSystem, 2048, UniqueAddress::toNum> systemList;

        int autoAddNewConnectionsToFilter;
        TimeDurMS whenLastTimeoutCheck;
    };
}
}
#endif
#endif // _RAKNET_SUPPORT_*