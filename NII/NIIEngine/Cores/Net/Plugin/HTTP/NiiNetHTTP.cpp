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
#if _RAKNET_SUPPORT_HTTPConnection2 == 1 && _RAKNET_SUPPORT_TCPInterface == 1

#include "NiiPreProcess.h"
#include "NiiNetHTTP.h"
#include "NetworkObj.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    HTTP::HTTP()
    {
    }
    //------------------------------------------------------------------------
    HTTP::~HTTP()
    {

    }
    //------------------------------------------------------------------------
    bool HTTP::TransmitRequest(const char* stringToTransmit, const char* host,
        Nui16 port, bool useSSL, int ipVersion, Address * useAddress)
    {
        Request *request = N_new Request();
        request->host=host;
        request->chunked = false;
        if (useAddress && !useAddress->isInvalid())
        {
            request->hostEstimatedAddress=*useAddress;
            if (IsConnected(useAddress)==false)
            {
                N_delete request;
                return false;
            }
        }
        else
        {
            if (request->hostEstimatedAddress.write(host, '|', ipVersion)==false)
            {
                N_delete request;
                return false;
            }
        }
        request->hostEstimatedAddress.setHSBPort(port);
        request->port=port;
        request->stringToTransmit=stringToTransmit;
        request->contentLength=-1;
        request->contentOffset=0;
        request->useSSL=useSSL;
        request->ipVersion=ipVersion;

        if (IsConnected(request->hostEstimatedAddress))
        {
            sentRequestsMutex.Lock();
            if (sentRequests.Size()==0)
            {
                request->hostCompletedAddress=request->hostEstimatedAddress;
                sentRequests.Push(request);
                sentRequestsMutex.Unlock();

                SendRequest(request);
            }
            else
            {
                // Request pending, push it
                pendingRequestsMutex.Lock();
                pendingRequests.Push(request);
                pendingRequestsMutex.Unlock();

                sentRequestsMutex.Unlock();
            }
        }
        else
        {
            pendingRequestsMutex.Lock();
            pendingRequests.Push(request);
            pendingRequestsMutex.Unlock();

            if (ipVersion!=6)
            {
                mPrc->Connect(host, port, false, AF_INET);
            }
            else
            {
                #if N_NET_PROTOCOL_VER == 6
                    mPrc->Connect(host, port, false, AF_INET6);
                #else
                    N_assert("HTTP::TransmitRequest needs define  N_NET_PROTOCOL_VER" && 0);
                #endif
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool HTTP::GetResponse(RakString & stringTransmitted, RakString & hostTransmitted, RakString & responseReceived, Address & hostReceived, int & contentOffset)
    {
        completedRequestsMutex.Lock();
        if (completedRequests.Size() > 0)
        {
            Request *completedRequest = completedRequests[0];
            completedRequests.RemoveAtIndexFast(0);
            completedRequestsMutex.Unlock();

            responseReceived = completedRequest->stringReceived;
            hostReceived = completedRequest->hostCompletedAddress;
            stringTransmitted = completedRequest->stringToTransmit;
            hostTransmitted = completedRequest->host;
            contentOffset = completedRequest->contentOffset;

            N_delete completedRequest;
            return true;
        }
        else
        {
            completedRequestsMutex.Unlock();
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool HTTP::IsBusy(void) const
    {
        return pendingRequests.Size()>0 || sentRequests.Size()>0;
    }
    //------------------------------------------------------------------------
    bool HTTP::HasResponse(void) const
    {
        return completedRequests.Size()>0;
    }
    //------------------------------------------------------------------------
    int ReadChunkSize(char * txtStart, char ** txtEnd)
    {
        return strtoul(txtStart, txtEnd,16);
    }
    //------------------------------------------------------------------------
    void ReadChunkBlock( size_t &currentChunkSize, size_t &bytesReadSoFar, char *txtIn, RakString &txtOut)
    {
        size_t bytesToRead;
        size_t sLen;

        do
        {
            bytesToRead = currentChunkSize - bytesReadSoFar;
            sLen = strlen(txtIn);
            if (sLen < bytesToRead)
                bytesToRead = sLen;
            txtOut.AppendBytes(txtIn, bytesToRead);
            txtIn += bytesToRead;
            bytesReadSoFar += bytesToRead;
            if (*txtIn == 0)
            {
            //	currentChunkSize=0;
                return;
            }
            // char *newLine = strstr(txtIn, "\r\n");
            if (txtIn[0] && txtIn[0]=='\r' && txtIn[1] && txtIn[1]=='\n' )
                txtIn += 2; // Newline
            char *newLine;
            currentChunkSize = ReadChunkSize(txtIn, &newLine);
            N_assert(currentChunkSize < 50000); // Sanity check
            if (currentChunkSize == 0)
                return;
            if (newLine == 0)
                return;
            bytesReadSoFar=0;
            txtIn = newLine + 2;
        } while (txtIn);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType HTTP::onMessage(SocketMessage *msg)
    {
        Nui32 i;

        bool locked=true;
        sentRequestsMutex.Lock();
        for (i = 0; i < sentRequests.Size(); ++i)
        {
            Request * sentRequest = sentRequests[i];
            if(sentRequest->hostCompletedAddress == msg->mAddress)
            {
                sentRequests.RemoveAtIndexFast(i);
                locked = false;
                sentRequestsMutex.Unlock();

                const char * isFirstChunk = strstr((char*) msg->mData, "Transfer-Encoding: chunked");
                if(isFirstChunk)
                {
                    locked=false;
                    sentRequestsMutex.Unlock();

                    sentRequest->chunked = true;
                    char *chunkStrStart = strstr((char*) msg->mData, "\r\n\r\n");
                    N_assert(chunkStrStart);

                    chunkStrStart += 4; // strlen("\r\n\r\n");
                    char *body_header; // = strstr(chunkStrStart, "\r\n");
                    sentRequest->thisChunkSize = ReadChunkSize(chunkStrStart, &body_header);
                    sentRequest->bytesReadForThisChunk = 0;
                    sentRequest->contentOffset = 0;

                    if (sentRequest->thisChunkSize == 0)
                    {
                        // Done
                        completedRequestsMutex.Lock();
                        completedRequests.Push(sentRequest);
                        completedRequestsMutex.Unlock();

                        // If there is another command waiting for this server, send it
                        SendPendingRequestToConnectedSystem(msg->mAddress);
                    }
                    else
                    {
                        body_header+=2;
                        ReadChunkBlock(sentRequest->thisChunkSize, sentRequest->bytesReadForThisChunk, body_header, sentRequest->stringReceived);

                        if (sentRequest->thisChunkSize==0)
                        {
                            // Done
                            completedRequestsMutex.Lock();
                            completedRequests.Push(sentRequest);
                            completedRequestsMutex.Unlock();

                            // If there is another command waiting for this server, send it
                            SendPendingRequestToConnectedSystem(msg->mAddress);
                        }
                        else
                        {
                            // Not done
                            sentRequestsMutex.Lock();
                            sentRequests.Push(sentRequest);
                            sentRequestsMutex.Unlock();
                        }
                    }
                }
                else if (sentRequest->chunked)
                {
                    ReadChunkBlock(sentRequest->thisChunkSize, sentRequest->bytesReadForThisChunk, (char*) msg->mData, sentRequest->stringReceived);

                    if (sentRequest->thisChunkSize==0)
                    {
                        // Done
                        completedRequestsMutex.Lock();
                        completedRequests.Push(sentRequest);
                        completedRequestsMutex.Unlock();

                        // If there is another command waiting for this server, send it
                        SendPendingRequestToConnectedSystem(msg->mAddress);
                    }
                    else
                    {
                        // Not done
                        sentRequestsMutex.Lock();
                        sentRequests.Push(sentRequest);
                        sentRequestsMutex.Unlock();
                    }

                }
                else
                {
                    sentRequest->stringReceived+=msg->mData;

                    if (sentRequest->contentLength==-1)
                    {
                        const char *length_header = strstr(sentRequest->stringReceived.C_String(), "Content-Length: ");
                        if(length_header)
                        {
                            length_header += 16; // strlen("Content-Length: ");

                            Nui32 clLength;
                            for (clLength=0; length_header[clLength] && length_header[clLength] >= '0' && length_header[clLength] <= '9'; clLength++)
                                ;
                            if (clLength>0 && (length_header[clLength]=='\r' || length_header[clLength]=='\n'))
                            {
                                sentRequest->contentLength = RakString::ReadIntFromSubstring(length_header, 0, clLength);
                            }
                        }
                    }

                    // If we know the content length, find \r\n\r\n
                    if (sentRequest->contentLength != -1)
                    {
                        if (sentRequest->contentLength > 0)
                        {
                            const char *body_header = strstr(sentRequest->stringReceived.C_String(), "\r\n\r\n");
                            if (body_header)
                            {
                                body_header += 4; // strlen("\r\n\r\n");
                                size_t slen = strlen(body_header);
                                //N_assert(slen <= (size_t) sentRequest->contentLength);
                                if (slen >= (size_t) sentRequest->contentLength)
                                {
                                    sentRequest->contentOffset = body_header - sentRequest->stringReceived.C_String();
                                    completedRequestsMutex.Lock();
                                    completedRequests.Push(sentRequest);
                                    completedRequestsMutex.Unlock();

                                    // If there is another command waiting for this server, send it
                                    SendPendingRequestToConnectedSystem(msg->mAddress);
                                }
                                else
                                {
                                    sentRequestsMutex.Lock();
                                    sentRequests.Push(sentRequest);
                                    sentRequestsMutex.Unlock();
                                }
                            }

                            else
                            {
                                sentRequestsMutex.Lock();
                                sentRequests.Push(sentRequest);
                                sentRequestsMutex.Unlock();
                            }
                        }
                        else
                        {
                            sentRequest->contentOffset=-1;
                            completedRequestsMutex.Lock();
                            completedRequests.Push(sentRequest);
                            completedRequestsMutex.Unlock();

                            // If there is another command waiting for this server, send it
                            SendPendingRequestToConnectedSystem(msg->mAddress);
                        }
                    }
                    else
                    {
                        const char *firstNewlineSet = strstr(sentRequest->stringReceived.C_String(), "\r\n\r\n");
                        if (firstNewlineSet!=0)
                        {
                            int offset = firstNewlineSet - sentRequest->stringReceived.C_String();
                            if (sentRequest->stringReceived.C_String()[offset+4]==0)
                                sentRequest->contentOffset=-1;
                            else
                                sentRequest->contentOffset=offset+4;
                            completedRequestsMutex.Lock();
                            completedRequests.Push(sentRequest);
                            completedRequestsMutex.Unlock();

                            // If there is another command waiting for this server, send it
                            SendPendingRequestToConnectedSystem(msg->mAddress);
                        }
                        else
                        {
                            sentRequestsMutex.Lock();
                            sentRequests.Push(sentRequest);
                            sentRequestsMutex.Unlock();
                        }
                    }
                }


                break;
            }
        }

        if (locked==true)
            sentRequestsMutex.Unlock();

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void HTTP::onConnect(const Address &address, UniqueID id)
    {
        (void) id;
        SendPendingRequestToConnectedSystem(address);
    }
    //------------------------------------------------------------------------
    void HTTP::SendPendingRequestToConnectedSystem(Address * sa)
    {
        if (sa == 0 || sa->isInvalid())
            return;

        Nui32 requestsSent=0;

        // Search through requests to find a match for this instance of NetworkObj and Address
        Nui32 i;
        i=0;
        pendingRequestsMutex.Lock();
        while (i < pendingRequests.Size())
        {
            Request * request = pendingRequests[i];
            if (request->hostEstimatedAddress==sa)
            {
                pendingRequests.RemoveAtIndex(i);
                // send this request
                request->hostCompletedAddress=sa;

                sentRequestsMutex.Lock();
                sentRequests.Push(request);
                sentRequestsMutex.Unlock();

                pendingRequestsMutex.Unlock();

                if(request->useSSL)
                    mPrc->setSSL(sa);
                
                SendRequest(request);
                requestsSent++;
                pendingRequestsMutex.Lock();
                break;
            }
            else
            {
                i++;
            }
        }
        pendingRequestsMutex.Unlock();

        if(requestsSent == 0)
        {
            pendingRequestsMutex.Lock();
            if(pendingRequests.Size() > 0)
            {
                // Just assign
                Request * request = pendingRequests[0];
                pendingRequests.RemoveAtIndex(0);

                request->hostCompletedAddress = sa;

                sentRequestsMutex.Lock();
                sentRequests.Push(request);
                sentRequestsMutex.Unlock();
                pendingRequestsMutex.Unlock();

                // send
                if(request->useSSL)
                    mPrc->setSSL(sa);
                
                SendRequest(request);
            }
            else
            {
                pendingRequestsMutex.Unlock();
            }
        }
    }
    //------------------------------------------------------------------------
    void HTTP::RemovePendingRequest(Address * sa)
    {
        Nui32 i = 0;
        pendingRequestsMutex.Lock();
        for (i = 0; i < pendingRequests.Size(); i++)
        {
            Request * request = pendingRequests[i];
            if (request->hostEstimatedAddress==sa)
            {
                pendingRequests.RemoveAtIndex(i);
                N_delete request;
            }
            else
                i++;
        }

        pendingRequestsMutex.Unlock();
    }
    //------------------------------------------------------------------------
    void HTTP::SendNextPendingRequest(void)
    {
        // send a pending request
        pendingRequestsMutex.Lock();
        if (pendingRequests.Size()>0)
        {
            Request *pendingRequest = pendingRequests.Peek();
            pendingRequestsMutex.Unlock();

            if (pendingRequest->ipVersion!=6)
            {
                mPrc->Connect(pendingRequest->host.C_String(), pendingRequest->port, false, AF_INET);
            }
            else
            {
    #if N_NET_PROTOCOL_VER == 6
                mPrc->Connect(pendingRequest->host.C_String(), pendingRequest->port, false, AF_INET6);
    #else
                N_assert("HTTP::TransmitRequest needs define  N_NET_PROTOCOL_VER" && 0);
    #endif
            }
        }
        else
        {
            pendingRequestsMutex.Unlock();
        }
    }
    //------------------------------------------------------------------------
    void HTTP::onConnectFail(SocketMessage * msg, ConnectFailType failedConnectionAttemptReason)
    {
        (void) failedConnectionAttemptReason;
        if (msg->mAddress == 0 ||msg->mAddress->isInvalid())
            return;

        RemovePendingRequest(msg->mAddress);

        SendNextPendingRequest();
    }
    //------------------------------------------------------------------------
    void HTTP::onConnectClose(const Address &address, UniqueID id, ConnectCloseType lostConnectionReason )
    {
        (void) lostConnectionReason;
        (void) id;

        if(address.isInvalid())
            return;

        // update sent requests to completed requests
        Nui32 i = 0;
        sentRequestsMutex.Lock();
        while (i < sentRequests.Size())
        {
            if (sentRequests[i]->hostCompletedAddress==address)
            {
                Request *sentRequest = sentRequests[i];
                if (sentRequest->chunked==false && sentRequest->stringReceived.IsEmpty()==false)
                {
                    if (strstr(sentRequest->stringReceived.C_String(), "Content-Length: "))
                    {
                        char *body_header = strstr((char*) sentRequest->stringReceived.C_String(), "\r\n\r\n");
                        if (body_header)
                        {
                            body_header += 4; // strlen("\r\n\r\n");
                            sentRequest->contentOffset = body_header - sentRequest->stringReceived.C_String();
                        }
                        else
                        {
                            sentRequest->contentOffset = 0;
                        }

                    }
                    else
                    {
                        sentRequest->contentOffset = 0;
                    }
                }
                completedRequestsMutex.Lock();
                completedRequests.Push(sentRequests[i]);
                completedRequestsMutex.Unlock();

                sentRequests.RemoveAtIndexFast(i);
            }
            else
            {
                i++;
            }
        }
        sentRequestsMutex.Unlock();

        SendNextPendingRequest();
    }
    //------------------------------------------------------------------------
    bool HTTP::IsConnected(Address * sa)
    {
        Address * remotes[64];
        Nui16 numberOfSystems = 64;
        mPrc->getConnect(remotes, &numberOfSystems);
        for (Nui32 i = 0; i < numberOfSystems; ++i)
        {
            if (remotes[i] == sa)
            {
                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void HTTP::SendRequest(Request * request)
    {
        mPrc->send(request->stringToTransmit.C_String(), (Nui32) request->stringToTransmit.GetLength(), request->hostCompletedAddress, false);
    }
    //------------------------------------------------------------------------
#endif