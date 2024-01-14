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

#include "NiiNetAuthFilter.h"
#include "Rand.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"
#include "NiiNetLinkBase.h"

#if LIBCAT_SECURITY == 1
    static const int HASH_BITS = 256;
    static const int HASH_BYTES = HASH_BITS / 8;
    static const int STRENGTHENING_FACTOR = 256;
    #include <cat/crypt/hash/Skein.hpp>
#endif

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    enum NegotiationIdentifiers
    {
        ID_NONCE_REQUEST,
        ID_NONCE_REPLY,
        ID_HASHED_NONCE_AND_PASSWORD,
    };
    //------------------------------------------------------------------------
    AuthFilter::NonceGenerator::NonceGenerator() {nextRequestId=0;}
    //------------------------------------------------------------------------
    AuthFilter::NonceGenerator::~NonceGenerator()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void AuthFilter::NonceGenerator::GetNonce(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH],
        Nui16 *requestId, UniqueAddress remote)
    {
        AuthFilter::NonceAndRemoteSystemRequest *narsr = N_new AuthFilter::NonceAndRemoteSystemRequest();
        narsr->mRemote=remote;
        GenerateNonce(narsr->nonce);
        narsr->requestId=nextRequestId++;
        *requestId=narsr->requestId;
        memcpy(nonce,narsr->nonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
        narsr->whenGenerated=N_Engine().getTimer().getMS();
        generatedNonces.Push(narsr);
    }
    //------------------------------------------------------------------------
    void AuthFilter::NonceGenerator::GenerateNonce(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH])
    {
        fillBufferMT(nonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
    }
    //------------------------------------------------------------------------
    bool AuthFilter::NonceGenerator::GetNonceById(char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH],
        Nui16 requestId, UniqueAddress remote, bool popIfFound)
    {
        Nui32 i;
        for (i=0; i < generatedNonces.Size(); i++)
        {
            if (generatedNonces[i]->requestId==requestId)
            {
                if (remote==generatedNonces[i]->mRemote)
                {
                    memcpy(nonce,generatedNonces[i]->nonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
                    if (popIfFound)
                    {
                        N_delete generatedNonces[i];
                        generatedNonces.RemoveAtIndex(i);
                    }
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void AuthFilter::NonceGenerator::Clear(void)
    {
        Nui32 i;
        for (i=0; i < generatedNonces.Size(); i++)
            N_delete generatedNonces[i];
        generatedNonces.Clear(true);
    }
    //------------------------------------------------------------------------
    void AuthFilter::NonceGenerator::ClearByAddress(UniqueAddress remote)
    {
        Nui32 i=0;
        while (i < generatedNonces.Size())
        {
            if (generatedNonces[i]->mRemote==remote)
            {
                N_delete generatedNonces[i];
                generatedNonces.RemoveAtIndex(i);
            }
            else
            {
                i++;
            }
        }
    }
    //------------------------------------------------------------------------
    void AuthFilter::NonceGenerator::update(TimeDurMS curTime)
    {
        if(generatedNonces.Size() > 0 && Timer::greater(curTime-5000, generatedNonces[0]->whenGenerated))
        {
            N_delete generatedNonces[0];
            generatedNonces.RemoveAtIndex(0);
        }
    }
    //------------------------------------------------------------------------
    AuthFilter::AuthFilter()
    {
        whenLastTimeoutCheck = N_Engine().getTimer().getMS();
        seedMT(N_Engine().getTimer().getMS());
    }
    //------------------------------------------------------------------------
    AuthFilter::~AuthFilter()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    bool AuthFilter::AddPassword(RakString identifier, RakString password)
    {
        if (password.IsEmpty())
            return false;

        if (identifier.IsEmpty())
            return false;

        if (password==identifier)
            return false; // Insecure

        if (passwords.GetIndexOf(identifier.C_String()).IsInvalid()==false)
            return false; // This identifier already in use

        passwords.Push(identifier, password);
        return true;
    }
    //------------------------------------------------------------------------
    bool AuthFilter::Challenge(RakString identifier, UniqueAddress remote)
    {
        DataStructures::HashIndex skhi = passwords.GetIndexOf(identifier.C_String());
        if (skhi.IsInvalid())
            return false;

        NetSerializer out;
        out.write((Token)ID_TWO_WAY_AUTHENTICATION_NEGOTIATION);
        out.write((Token)ID_NONCE_REQUEST);
        send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, remote, false);

        PendingChallenge pc;
        pc.identifier = identifier;
        pc.mRemote = remote;
        pc.time = N_Engine().getTimer().getMS();
        pc.sentHash = false;
        outgoingChallenges.Push(pc);

        return true;
    }
    //------------------------------------------------------------------------
    void AuthFilter::update()
    {
        TimeDurMS curTime = N_Engine().getTimer().getMS();
        nonceGenerator.update(curTime);
        if (Timer::greater(curTime - CHALLENGE_MINIMUM_TIMEOUT, whenLastTimeoutCheck))
        {
            while (outgoingChallenges.Size() && Timer::greater(curTime - CHALLENGE_MINIMUM_TIMEOUT, outgoingChallenges.Peek().time))
            {
                PendingChallenge pc = outgoingChallenges.Pop();

                // Tell the user about the timeout
                PushToUser(ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT,
                    pc.identifier, pc.mRemote);
            }

            whenLastTimeoutCheck=curTime+CHALLENGE_MINIMUM_TIMEOUT;
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType AuthFilter::onMessage(SocketMessage * msg)
    {
        switch (msg->mData[0])
        {
            case ID_TWO_WAY_AUTHENTICATION_NEGOTIATION:
            {
                if (msg->mLength>=sizeof(Token)*2)
                {
                    switch (msg->mData[sizeof(Token)])
                    {
                    case ID_NONCE_REQUEST:
                        {
                            OnNonceRequest(msg);
                        }
                        break;
                    case ID_NONCE_REPLY:
                        {
                            OnNonceReply(msg);
                        }
                        break;
                    case ID_HASHED_NONCE_AND_PASSWORD:
                        {
                            return OnHashedNonceAndPassword(msg);
                        }
                        break;
                    }
                }
                return CRT_Destroy;
            }
            case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILURE:
            case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_SUCCESS:
            {
                if (msg->mLocal == false)
                {
                    OnPasswordResult(msg);
                    return CRT_Destroy;
                }
                else
                    break;
            }
            break;
            // These should only be generated locally
            case ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS:
            case ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILURE:
            case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT:
                if (msg->mLocal == false)
                    return CRT_Destroy;
                break;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void AuthFilter::shutdown()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void AuthFilter::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;

        // Remove from pending challenges
        Nui32 i = 0;
        while(i < outgoingChallenges.Size())
        {
            if((id != UniqueID::INVALID && outgoingChallenges[i].mRemote.mID == id) ||
                (address && !address->isInvalid() && outgoingChallenges[i].mRemote.mAddress==address))
            {
                outgoingChallenges.RemoveAtIndex(i);
            }
            else
            {
                i++;
            }
        }

        if(id != UniqueID::INVALID)
            nonceGenerator.ClearByAddress(id);
        else
            nonceGenerator.ClearByAddress(address);
    }
    //------------------------------------------------------------------------
    void AuthFilter::Clear(void)
    {
        outgoingChallenges.Clear();
        passwords.Clear();
        nonceGenerator.Clear();
    }
    //------------------------------------------------------------------------
    void AuthFilter::PushToUser(Token messageId, RakString password, UniqueAddress remote)
    {
        NetSerializer output;
        output.write(messageId);
        if (password.IsEmpty() == false)
            output.write(password);
        SocketMessage * p = createMessage(output.getSize());
        p->mAddress = remote.mAddress;
        p->mAddress.mIndex = (Nindex)-1;
        p->mID = remote.mID;
        p->mLocal = true;
        memcpy(p->mData, output.getData(), output.getSize());
        mPrc2->addMessage(p, true);
    }
    //------------------------------------------------------------------------
    void AuthFilter::OnNonceRequest(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        char nonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
        Nui16 requestId;
        nonceGenerator.GetNonce(nonce,&requestId,msg);

        NetSerializer out;
        out.write((Token)ID_TWO_WAY_AUTHENTICATION_NEGOTIATION);
        out.write((Token)ID_NONCE_REPLY);
        out.write(requestId);
        out.write((const Nui8 *)nonce, TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
        send(&out,PL_HIGH,PT_ORDER_MAKE_SURE,0,msg,false);
    }
    //------------------------------------------------------------------------
    void AuthFilter::OnNonceReply(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        char thierNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
        Nui16 requestId;
        bsIn.read(requestId);
        bsIn.read((Nui8 *) thierNonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);

        // Lookup one of the negotiations for this guid/system address
        UniqueAddress aog(msg);
        Nui32 i;
        for (i=0; i < outgoingChallenges.Size(); i++)
        {
            if (outgoingChallenges[i].mRemote == aog && outgoingChallenges[i].sentHash==false)
            {
                outgoingChallenges[i].sentHash = true;

                // Get the password for this identifier
                DataStructures::HashIndex skhi = passwords.GetIndexOf(outgoingChallenges[i].identifier.C_String());
                if (skhi.IsInvalid() == false)
                {
                    RakString password = passwords.ItemAtIndex(skhi);

                    // Hash their nonce with password and reply
                    char hashedNonceAndPw[HASHED_NONCE_AND_PW_LENGTH];
                    Hash(thierNonce, password, hashedNonceAndPw);

                    // send
                    NetSerializer out;
                    out.write((Token)ID_TWO_WAY_AUTHENTICATION_NEGOTIATION);
                    out.write((Token)ID_HASHED_NONCE_AND_PASSWORD);
                    out.write(requestId);
                    out.write(outgoingChallenges[i].identifier); // Identifier helps the other system lookup the password quickly.
                    out.write((const Nui8 *)hashedNonceAndPw, HASHED_NONCE_AND_PW_LENGTH);
                    send(&out, PL_HIGH,PT_ORDER_MAKE_SURE, 0, msg, false);
                }

                return;
            }
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType AuthFilter::OnHashedNonceAndPassword(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        char remoteHashedNonceAndPw[HASHED_NONCE_AND_PW_LENGTH];
        Nui16 requestId;
        bsIn.read(requestId);
        RakString passwordIdentifier;
        bsIn.read(passwordIdentifier);
        bsIn.read((Nui8 *) remoteHashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH);

        // Look up used nonce from requestId
        char usedNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
        if (nonceGenerator.GetNonceById(usedNonce, requestId, msg, true)==false)
            return CRT_Destroy;

        DataStructures::HashIndex skhi = passwords.GetIndexOf(passwordIdentifier.C_String());
        if (skhi.IsInvalid()==false)
        {
            char hashedThisNonceAndPw[HASHED_NONCE_AND_PW_LENGTH];
            Hash(usedNonce, passwords.ItemAtIndex(skhi), hashedThisNonceAndPw);
            if (memcmp(hashedThisNonceAndPw, remoteHashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH)==0)
            {
                // Pass
                NetSerializer out;
                out.write((Token)ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_SUCCESS);
                out.write((const Nui8 *)usedNonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
                out.write((const Nui8 *)remoteHashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH);
                out.write(passwordIdentifier);
                send(&out,PL_HIGH,PT_ORDER_MAKE_SURE,0,msg,false);

                // Incoming success, modify msg header to tell user
                PushToUser(ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS, passwordIdentifier, msg);

                return CRT_Destroy;
            }
        }

        // Incoming failure, modify arrived msg header to tell user
        msg->mData[0] = (Token) ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILURE;

        NetSerializer out;
        out.write((Token)ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILURE);
        out.write((const Nui8 *)usedNonce, TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
        out.write((const Nui8 *)remoteHashedNonceAndPw, HASHED_NONCE_AND_PW_LENGTH);
        out.write(passwordIdentifier);
        send(&out,PL_HIGH,PT_ORDER_MAKE_SURE,0,msg,false);

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void AuthFilter::OnPasswordResult(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        char usedNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH];
        bsIn.read((Nui8 *)usedNonce,TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
        char hashedNonceAndPw[HASHED_NONCE_AND_PW_LENGTH];
        bsIn.read((Nui8 *)hashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH);
        RakString passwordIdentifier;
        bsIn.read(passwordIdentifier);

        DataStructures::HashIndex skhi = passwords.GetIndexOf(passwordIdentifier.C_String());
        if (skhi.IsInvalid()==false)
        {
            RakString password = passwords.ItemAtIndex(skhi);
            char testHash[HASHED_NONCE_AND_PW_LENGTH];
            Hash(usedNonce, password, testHash);
            if (memcmp(testHash,hashedNonceAndPw,HASHED_NONCE_AND_PW_LENGTH)==0)
            {
                // Lookup the outgoing challenge and remove it from the list
                NCount i;
                UniqueAddress aog(msg);
                for (i=0; i < outgoingChallenges.Size(); ++i)
                {
                    if (outgoingChallenges[i].identifier == passwordIdentifier &&
                        outgoingChallenges[i].mRemote == aog &&
                        outgoingChallenges[i].sentHash == true)
                    {
                        outgoingChallenges.RemoveAtIndex(i);

                        PushToUser(msg->mData[0], passwordIdentifier, msg);
                        return;
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void AuthFilter::Hash(char thierNonce[TWO_WAY_AUTHENTICATION_NONCE_LENGTH],
        RakString password, char out[HASHED_NONCE_AND_PW_LENGTH])
    {
    #if LIBCAT_SECURITY == 1
        cat::Skein hash;
        if (!hash.BeginKey(HASH_BITS)) return;
        hash.Crunch(thierNonce, TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
        hash.Crunch(password.C_String(), (int) password.GetLength());
        hash.End();
        hash.Generate(out, HASH_BYTES, STRENGTHENING_FACTOR);
    #else
        CSHA1 sha1;
        sha1.update((Nui8 *) thierNonce, TWO_WAY_AUTHENTICATION_NONCE_LENGTH);
        sha1.update((Nui8 *) password.C_String(), (Nui32) password.GetLength());
        sha1.Final();
        sha1.GetHash((Nui8 *) out);
    #endif
    }
    //------------------------------------------------------------------------
#endif
