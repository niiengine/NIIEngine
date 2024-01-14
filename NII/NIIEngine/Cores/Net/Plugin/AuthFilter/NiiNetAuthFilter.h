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
#if _RAKNET_SUPPORT_TwoWayAuthentication == 1

#ifndef __TWO_WAY_AUTHENTICATION_H
#define __TWO_WAY_AUTHENTICATION_H

// How often to change the nonce.
#define NONCE_TIMEOUT_MS 10000
// How often to check for ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT, and the minimum timeout time. Maximum is NIId this value.
#define CHALLENGE_MINIMUM_TIMEOUT 3000

#if LIBCAT_SECURITY == 1
    #define TWO_WAY_AUTHENTICATION_NONCE_LENGTH 32
    #define HASHED_NONCE_AND_PW_LENGTH 32
#else
    #include "DR_SHA1.h"
    #define TWO_WAY_AUTHENTICATION_NONCE_LENGTH 20
    #define HASHED_NONCE_AND_PW_LENGTH SHA1_LENGTH
#endif

#include "NiiNetPreInclude.h"
#include "NiiNetPlugin.h"
#include "RakString.h"
#include "DS_Hash.h"
#include "DS_Queue.h"

typedef Ni64 FCM2Guid;

namespace NII
{
namespace NII_NET
{
    /// \brief Implements two way authentication
    /// \details Given two systems, each of whom known a common password / identifier pair, verify the password without transmitting it
    /// This can be used to determine what permissions are should be allowed to the other system
    /// If the other system should not send any data until authentication passes, you can use the MsgFilter plugin for this. Call MsgFilter::SetAllowMessageID() including ID_TWO_WAY_AUTHENTICATION_NEGOTIATION when doing so. Also attach MsgFilter first in the list of plugins
    /// \note If other systems challenges us, and fails, you will get ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILED.
    /// \ingroup PLUGINS_GROUP
    class _EngineAPI AuthFilter : public Plugin
    {
    public:
        AuthFilter();
        virtual ~AuthFilter();

        /// \brief Adds a password to the list of passwords the system will accept
        /// \details Each password, which is secret and not transmitted, is identified by \a identifier.
        /// \a identifier is transmitted in plaintext with the request. It is only needed because the system supports multiple password.
        /// It is used to only hash against once password on the remote system, rather than having to hash against every known password.
        /// \param[in] identifier A unique identifier representing this password. This is transmitted in plaintext and should be considered insecure
        /// \param[in] password The password to add
        /// \return True on success, false on identifier == password, either identifier or password is blank, or identifier is already in use
        bool AddPassword(RakString identifier, RakString password);

        /// \brief Challenge another system for the specified identifier
        /// \details After calling Challenge, you will get back ID_TWO_WAY_AUTHENTICATION_SUCCESS, ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT, or ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILED
        /// ID_TWO_WAY_AUTHENTICATION_SUCCESS will be returned if and only if the other system has called AddPassword() with the same identifier\password pair as this system.
        /// \param[in] identifier A unique identifier representing this password. This is transmitted in plaintext and should be considered insecure
        /// \return True on success, false on remote system not connected, or identifier not previously added with AddPassword()
        bool Challenge(RakString identifier, UniqueAddress remote);

        /// \brief Free all memory
        void Clear(void);

        /// \internal
        virtual void update();
        /// \internal
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        /// \internal
        virtual void shutdown();
        /// \internal
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );

        /// \internal
        struct PendingChallenge
        {
            RakString identifier;
            UniqueAddress mRemote;
            TimeDurMS time;
            bool sentHash;
        };

        DataStructures::Queue<PendingChallenge> outgoingChallenges;

        /// \internal
        struct NonceAndRemoteSystemRequest
        {
            char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
            UniqueAddress mRemote;
            Nui16 requestId;
            TimeDurMS whenGenerated;
        };
        /// \internal
        struct _EngineAPI NonceGenerator
        {
            NonceGenerator();
            ~NonceGenerator();
            void GetNonce(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH], Nui16 *requestId, UniqueAddress remote);
            void GenerateNonce(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH]);
            bool GetNonceById(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH], Nui16 requestId, UniqueAddress remote, bool popIfFound);
            void Clear(void);
            void ClearByAddress(UniqueAddress remote);
            void update(TimeDurMS curTime);

            DataStructures::List<AuthFilter::NonceAndRemoteSystemRequest *> generatedNonces;
            Nui16 nextRequestId;
        };
    protected:
        void PushToUser(Token messageId, RakString password, UniqueAddress remote);
        // Key is identifier, data is password
        DataStructures::Hash<RakString, RakString, 16, RakString::ToInteger> passwords;

        TimeDurMS whenLastTimeoutCheck;

        NonceGenerator nonceGenerator;

        void OnNonceRequest(SocketMessage * msg);
        void OnNonceReply(SocketMessage * msg);
        ConnectReceiveType OnHashedNonceAndPassword(SocketMessage * msg);
        void OnPasswordResult(SocketMessage * msg);
        void Hash(char thierNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH], RakString password, char out[HASHED_NONCE_AND_PW_LENGTH]);
    };
}
}
#endif
#endif