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
#ifndef __LOBBY_2_SERVER_H
#define __LOBBY_2_SERVER_H

#include "Export.h"
#include "NiiNetCommon.h"
#include "Lobby2Plugin.h"
#include "DS_OrderedList.h"
#include "NiiThreadManager.h"
#include "Lobby2Presence.h"

namespace NII
{
namespace NII_NET
{
    struct Lobby2Message;
    class RoomsPlugin;

    /// Commands are either messages from remote systems, or can be run by the local system
    /// \internal
    struct Lobby2ServerCommand
    {
        Lobby2Message *lobby2Message;
        bool deallocMsgWhenDone;
        bool returnToSender;
        unsigned int callerUserId;
        RakString callingUserName;
        DataStructures::List<Address *> callerSystemAddresses;
        DataStructures::List<UniqueID> callerGuids;
        //Address requiredConnectionAddress;
        Lobby2Server *server;
    };
    
    class Lobby2ServerJob : public Job
    {
    public:
        Lobby2ServerJob(const Lobby2ServerCommand & cmd) : 
            Job(0), 
            mCommand(cmd){}
            
        ~Lobby2ServerJob()
        {
            if (mCommand.deallocMsgWhenDone && mCommand.lobby2Message)
            {
                OP_DELETE(mCommand.lobby2Message);
                mCommand.lobby2Message = 0;
            }
        }
        
        class Lobby2ServerJobIdentifier : public Job::Identifier
        {
        public : 
            Lobby2ServerJobIdentifier(User * usr) : mUser(usr){} ;
            
            User * mUser;
        };
        
        bool equal(const Identifier * id)
        {
            Lobby2ServerJobIdentifier * temp = static_cast<Lobby2ServerJobIdentifier *>(id);
            User * user = temp->mUser;
            if (mCommand.lobby2Message->CancelOnDisconnect()&& mCommand.callerSystemAddresses.Size()>0 && user->systemAddresses.GetIndexOf(mCommand.callerSystemAddresses[0])!=(NCount)-1)
            {
                return true;
            }
            return false;
        }
    public:
        Lobby2ServerCommand mCommand;
    };
    
    class Lobby2ServerJobResult : public JobResult
    {
    public:
        Lobby2ServerJobResult(Lobby2ServerCommand * cmd, Job * src,
            bool complete, const String & msg = StrUtil::WBLANK) : 
                JobResult(src, complete, msg),
                mResult(cmd){}

        Lobby2ServerCommand * mResult;
    };
    
    class _EngineAPI Lobby2JobPrc : public JobPrc
    {
        friend class ThreadManager;
    public:
        Lobby2JobPrc(Lobby2Server * server) : JobPrc(true), mServer(server) {}
        virtual ~Lobby2JobPrc() {}

        /** ¥¶¿Ìªÿ¿°
        @version NIIEngine 3.0.0
        */
        virtual void handle(JobResult * result)
        {
            Lobby2ServerCommand * c = static_cast<Lobby2ServerJobResult *>(result)->mResult;
            c->lobby2Message->ServerPostDBMemoryImpl(this, c->callingUserName);
            //if (c->returnToSender)
            //{
                for (unsigned long i = 0; i < callbacks.Size(); i++)
                {
                    if (c->lobby2Message->callbackId == (uint32_t)-1 || c->lobby2Message->callbackId==callbacks[i]->callbackId)
                        c->lobby2Message->CallCallback(callbacks[i]);
                }

                NetSerializer bs;
                bs.write((Token)ID_LOBBY2_SEND_MESSAGE);
                bs.write((Token)c->lobby2Message->GetID());
                c->lobby2Message->read(true, &bs);
                // Have the ID to send to, but not the address. The ID came from the thread, such as notifying another user
                if (c->callerSystemAddresses.Size()==0)
                {
                    NCount i;
                    if (c->callerUserId!=0)
                    {
                        for (i=0; i < users.Size(); i++)
                        {
                            if (users[i]->callerUserId==c->callerUserId)
                            {
                                c->callerSystemAddresses=users[i]->systemAddresses;
                                c->callerGuids=users[i]->guids;
                                break;
                            }
                        }
                    }
                    if (c->callerSystemAddresses.Size()==0 && c->callingUserName.IsEmpty()==false)
                    {
                        for (i=0; i < users.Size(); i++)
                        {
                            if (users[i]->callerUserId==c->callerUserId)
                            {
                                c->callerSystemAddresses=users[i]->systemAddresses;
                                c->callerGuids=users[i]->guids;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    bool objectExists;
                    NCount idx;
                    idx = users.GetIndexFromKey(c->callingUserName,&objectExists);
                    if (objectExists && c->callingUserName.IsEmpty()==false && users[idx]->userName!=c->callingUserName)
                    {
                        // Different user, same IP address. Abort the send.
                        if (c->deallocMsgWhenDone)
                            OP_DELETE(c->lobby2Message);
                        return;
                    }
                }
                mServer->SendUnifiedToMultiple(&bs,packetPriority, RELIABLE_ORDERED, mPacketCh, c->callerSystemAddresses);
            //}
            if (c->deallocMsgWhenDone)
            {
                OP_DELETE(c->lobby2Message);
            }
        }
    protected:
        Lobby2Server * mServer;
    };

    /// \brief The base class for the lobby server, without database specific functionality
    /// \details This is a plugin which will take incoming messages via Lobby2Client_PC::SendMsg(), process them, and send back the same messages with output and a result code
    /// Unlike the first implementation of the lobby server, this is a thin plugin that mostly just sends messages to threads and sends back the results.
    /// \ingroup LOBBY_2_SERVER
    class RAK_DLL_EXPORT Lobby2Server : public Lobby2Plugin/*,  public ThreadPoolData*/
    {
    public:
        Lobby2Server();
        virtual ~Lobby2Server();

        /// \brief Connect to the database \a numWorkerThreads times using the connection string
        /// \param[in] conninfo See the postgre docs
        /// \return True on success, false on failure.
        virtual bool ConnectToDB(const char * conninfo, int numWorkerThreads)=0;
        /// \internal
        virtual void AddInputFromThread(Lobby2Message * msg, unsigned int targetUserId, RakString targetUserHandle)=0;
        /// \internal
        virtual void AddOutputFromThread(Lobby2Message * msg, unsigned int targetUserId, RakString targetUserHandle)=0;

        /// \brief Lobby2Message encapsulates a user command, containing both input and output data
        /// \details This will serialize and transmit that command
        void SendMsg(Lobby2Message *msg, const DataStructures::List<Address *> &recipients);

        /// \brief Add a command, which contains a message and other data such as who send the message.
        /// \details The command will be processed according to its implemented virtual functions. Most likely it will be processed in a thread to run database commands
        void ExecuteCommand(Lobby2ServerCommand *command);

        /// \brief If Lobby2Message::RequiresAdmin() returns true, the message can only be processed from a remote system if the sender's system address is first added()
        /// \details This is useful if you want to administrate the server remotely
        void AddAdminAddress(Address * addr);
        
        /// \brief If AddAdminAddress() was previously called with \a addr then this returns true.
        bool HasAdminAddress(const DataStructures::List<Address *> &addresses);
        
        /// \brief Removes a system address previously added with AddAdminAddress()
        void RemoveAdminAddress(Address * addr);
        
        /// \brief Removes all system addresses previously added with AddAdminAddress()
        void ClearAdminAddresses(void);

        /// \brief If Lobby2Message::RequiresRankingPermission() returns true, then the system that sent the command must be registered with AddRankingAddress()
        /// \param[in] addr Address to allow
        void AddRankingAddress(Address * addr);

        /// Returns if an address was previously added with AddRankingAddress()
        /// \param[in] addr Address to check
        bool HasRankingAddress(const DataStructures::List<Address *> &addresses);

        /// Removes an addressed added with AddRankingAddress()
        /// \param[in] addr Address to check
        void RemoveRankingAddress(Address * addr);

        /// \brief Clears all addresses added with AddRankingAddress()
        void ClearRankingAddresses(void);

        /// \brief To use RoomsPlugin and Lobby2Server together, register RoomsPlugin with this funcrtion
        /// \details The rooms plugin does not automatically handle users logging in and logging off, or renaming users.
        /// You can have Lobby2Server manage this for you by calling SetRoomsPlugin() with a pointer to the rooms plugin, if it is on the local system.
        /// This will call RoomsPlugin::LoginRoomsParticipant() and RoomsPlugin::LogoffRoomsParticipant() as the messages L2MID_Client_Login and L2MID_Client_Logoff arrive
        /// This will use the pointer to RoomsPlugin directly. Setting this will disable SetRoomsPluginAddress()
        /// \note This is an empty function. You must #define __INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN and link with RoomsPlugin.h to use it()
        void SetRoomsPlugin(RoomsPlugin *rp);

        /// \brief This is similar to SetRoomsPlugin(), except the plugin is on another system.
        /// \details This will set the system address of that system to send the login and logoff commands to.
        /// For this function to work, you must first call RoomsPlugin::AddLoginServerAddress() so that RoomsPlugin will accept the incoming login and logoff messages.
        /// \note This is an empty function. You must #define __INTEGRATE_LOBBY2_WITH_ROOMS_PLUGIN and link with RoomsPlugin.h to use it()
        void SetRoomsPluginAddress(Address * address);

        /// \brief Server configuration properties, to customize how the server runs specific commands
        struct ConfigurationProperties
        {
            ConfigurationProperties() {requiresEmailAddressValidationToLogin=false; requiresTitleToLogin=false; accountRegistrationRequiredAgeYears=0;}

            /// \brief If true, Client_Login will fail with Client_Login_EMAIL_ADDRESS_NOT_VALIDATED unless the email address registered with Client_RegisterAccount is verified with the command System_SetEmailAddressValidated
            bool requiresEmailAddressValidationToLogin;

            /// \brief If true Client_Login::titleName and Client_Login::titleSecretKey must be previously registered with System_CreateTitle or Client_Login will fail with L2RC_Client_Login_BAD_TITLE_OR_TITLE_SECRET_KEY
            bool requiresTitleToLogin;

            /// \brief If true, Client_RegisterAccount::cdKey must be previously registered with CDKey_Add or Client_RegisterAccount will fail with L2RC_Client_RegisterAccount_REQUIRES_CD_KEY or a related error message
            bool accountRegistrationRequiresCDKey;

            /// \brief The minimum age needed to register accounts. If Client_RegisterAccount::createAccountParameters::ageInDays is less than this, then the account registration will fail with L2RC_Client_RegisterAccount_REQUIRED_AGE_NOT_MET
            /// \details Per-title age requirements can be handled client-side with System_CreateTitle::requiredAge and System_GetTitleRequiredAge
            unsigned int accountRegistrationRequiredAgeYears;
        };

        /// \brief Set the desired configuration properties. This is read during runtime from threads.
        void SetConfigurationProperties(ConfigurationProperties c);
        /// \brief Get the previously set configuration properties.
        const ConfigurationProperties *GetConfigurationProperties(void) const;

        /// Set the presence of a logged in user
        /// \param[in] presence Presence info of this user
        void SetPresence(const Lobby2Presence & presence, RakString userHandle);

        /// Get the presence of a logged in user, by handle
        /// \param[out] presence Presence info of requested user
        /// \param[in] userHandle Handle of the user
        void GetPresence(Lobby2Presence & presence, RakString userHandle);

        /// \internal Lets the plugin know that a user has logged on, so this user can be tracked and the message forwarded to RoomsPlugin
        void OnLogin(Lobby2ServerCommand *command, bool calledFromThread);
        /// \internal Lets the plugin know that a user has logged off, so this user can be tracked and the message forwarded to RoomsPlugin
        void OnLogoff(Lobby2ServerCommand *command, bool calledFromThread);
        /// \internal Lets the plugin know that a user has been renamed, so this user can be tracked and the message forwarded to RoomsPlugin
        void OnChangeHandle(Lobby2ServerCommand *command, bool calledFromThread);

        /// \internal
        struct User
        {
            DataStructures::List<Address *> systemAddresses;
            DataStructures::List<UniqueID> guids;
            unsigned int callerUserId;
            RakString userName;
            Lobby2Presence presence;
            bool allowMultipleLogins;
        };

        /// \internal
        static int UserCompByUsername( const RakString &key, Lobby2Server::User * const &data );

        /// \internal
        struct ThreadAction
        {
            Lobby2MessageID action;
            Lobby2ServerCommand command;
        };

        const DataStructures::OrderedList<RakString, User *, Lobby2Server::UserCompByUsername>& GetUsers(void) const {return users;}
        void GetUserOnlineStatus(UsernameAndOnlineStatus & userInfo) const;
    protected:
        void update();
        ConnectReceiveType onMessage(SocketMessage * msg);
        void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason);
        void shutdown(void);
        void OnMessage1(SocketMessage * msg);
        void Clear(void);
        void ClearUsers(void);
        unsigned int GetUserIndexBySystemAddress(Address * address) const;
        unsigned int GetUserIndexByGUID(UniqueID guid) const;
        unsigned int GetUserIndexByUsername(RakString userName) const;
        void StopThreads(void);
        void SendRemoteLoginNotification(RakString handle, const DataStructures::List<Address *>& recipients);

        /// \internal
        void RemoveUser(RakString userName);
        /// \internal
        void RemoveUser(unsigned int index);
        void LogoffFromRooms(User *user);

        //virtual void* PerThreadFactory(void *context)=0;
        //virtual void PerThreadDestructor(void* factoryResult, void *context)=0;
        virtual void AddInputCommand(Lobby2ServerCommand command)=0;
        virtual void ClearConnections(void) {};

        DataStructures::OrderedList<Address *, Address *> adminAddresses;
        DataStructures::OrderedList<Address *, Address *> rankingAddresses;
        DataStructures::OrderedList<RakString, User*, Lobby2Server::UserCompByUsername> users;
        RoomsPlugin *roomsPlugin;
        Address * roomsPluginAddress;
        ThreadManager<Lobby2ServerCommand,Lobby2ServerCommand> threadPool;
        ThreadMutex connectionPoolMutex;
        ConfigurationProperties configurationProperties;
        DataStructures::Queue<ThreadAction> threadActionQueue;
        ThreadMutex threadActionQueueMutex;

        //DataStructures::List<PostgreSQLInterface *> connectionPool;
        void SendUnifiedToMultiple( const NetSerializer * bitStream, PacketLevel plevel, PacketType ptype, char pchannel, const DataStructures::List<Address *> systemAddresses );
    };
}
}
#endif