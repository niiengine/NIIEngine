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
#if _RAKNET_SUPPORT_EmailSender == 1 && _RAKNET_SUPPORT_TCPInterface == 1 && _RAKNET_SUPPORT_FileOperations==1

// Useful sites
// http://www.faqs.org\rfcs\rfc2821.html
// http://www2.rad.com\networks/1995/mime/examples.htm

#include "NiiPreProcess.h"
#include "EmailSender.h"
#include "NetworkObj.h"
#include "Rand.h"
#include "FileList.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    const char * EmailSender::send(const char * hostAddress, Nui16 hostPort,
        const char * sender, const char * recipient, const char * senderName,
            const char * recipientName, const char * subject, const char * body,
                FileList * attachedFiles, bool doPrintf, const char * password)
    {
        SocketMessage * msg;
        char query[1024];
        NetworkObj tcpInterface;

        if(tcpInterface.Start(0, 0) == false)
            return "Unknown error starting TCP";
        Address emailServer = tcpInterface.Connect(hostAddress, hostPort, true);
        if(emailServer == 0)
            return "Failed to connect to host";

        tcpInterface.setSSL(emailServer);

        TimeDurMS timeout = N_Engine().getTimer().getMS()+3000;
        msg = 0;
        while(N_Engine().getTimer().getMS() < timeout)
        {
            msg = tcpInterface.receive();
            if (msg)
            {
                if (doPrintf)
                    N_printf("%s", msg->mData);
                break;
            }
            Nsleep(250);
        }

        if(msg == 0)
            return "Timeout while waiting for initial data from server.";

        tcpInterface.send("EHLO\r\n", 6, emailServer, false);
        const char * response;
        bool authenticate = false;
    #ifdef _MSC_VER
    #pragma warning(disable:4127)   // conditional expression is constant
    #endif
        while(1)
        {
            response = GetResponse(&tcpInterface, emailServer, doPrintf);

            if(response != 0 && strcmp(response, "AUTHENTICATE") == 0)
            {
                authenticate=true;
                break;
            }

            // Something other than continue?
            if(response != 0 && strcmp(response, "CONTINUE")!=0)
                return response;

            // Success?
            if(response == 0)
                break;
        }

        if(authenticate)
        {
            sprintf(query, "EHLO %s\r\n", sender);
            tcpInterface.send(query, (Nui32)strlen(query), emailServer, false);
            response = GetResponse(&tcpInterface, emailServer, doPrintf);
            if(response != 0)
                return response;
            if(password == 0)
                return "Password needed";
            VString outputData; //= N_alloc_t(char, (strlen(sender) + strlen(password) + 2) * 3);
            NetSerializer bs;
            char zero=0;
            bs.write(&zero, 1);
            bs.write(sender,(const Nui32)strlen(sender));
            //bs.write("jms1@jms1.net",(const Nui32)strlen("jms1@jms1.net"));
            bs.write(&zero, 1);
            bs.write(password,(const Nui32)strlen(password));
            bs.write(&zero, 1);
            //bs.write("not.my.real.password",(const Nui32)strlen("not.my.real.password"));
            Base64::Encode((const Nui8*)bs.getData(), bs.getSize(), outputData);
            sprintf(query, "AUTH PLAIN %s", outputData.c_str());
            tcpInterface.send(query, (Nui32)strlen(query), emailServer, false);
            response = GetResponse(&tcpInterface, emailServer, doPrintf);
            if(response != 0)
                return response;
        }

        if(sender)
            sprintf(query, "MAIL From: <%s>\r\n", sender);
        else
            sprintf(query, "MAIL From: <>\r\n");
        tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);
        response = GetResponse(&tcpInterface, emailServer, doPrintf);
        if(response != 0)
            return response;

        if(recipient)
            sprintf(query, "RCPT TO: <%s>\r\n", recipient);
        else
            sprintf(query, "RCPT TO: <>\r\n");
        tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);
        response = GetResponse(&tcpInterface, emailServer, doPrintf);
        if (response != 0)
            return response;

        tcpInterface.send("DATA\r\n", (Nui32)strlen("DATA\r\n"), emailServer,false);

        // Wait for 354...

        response = GetResponse(&tcpInterface, emailServer, doPrintf);
        if (response != 0)
            return response;

        if (subject)
        {
            sprintf(query, "Subject: %s\r\n", subject);
            tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);
        }
        if (senderName)
        {
            sprintf(query, "From: %s\r\n", senderName);
            tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);
        }
        if (recipientName)
        {
            sprintf(query, "To: %s\r\n", recipientName);
            tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);
        }

        const int boundarySize=60;
        char boundary[boundarySize+1];
        int i,j;
        if (attachedFiles && attachedFiles->fileList.Size())
        {
            rakNetRandom.SeedMT((Nui32) N_Engine().getTimer().getMS());
            // Random multipart message boundary
            for (i=0; i < boundarySize; i++)
                boundary[i] = Base64::getMap()[rakNetRandom.RandomMT() % 64];
            boundary[boundarySize]=0;
        }

        sprintf(query, "MIME-version: 1.0\r\n");
        tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);

        if (attachedFiles && attachedFiles->fileList.Size())
        {
            sprintf(query, "Content-type: multipart/mixed; BOUNDARY=\"%s\"\r\n\r\n", boundary);
            tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);

            sprintf(query, "This is a multi-part message in MIME format.\r\n\r\n--%s\r\n", boundary);
            tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);
        }

        sprintf(query, "Content-Type: text/plain; charset=\"US-ASCII\"\r\n\r\n");
        tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);

        // write the body of the email, doing some lame shitty shit where I have to make periods at the start of a newline have a second period.
        char *newBody;
        int bodyLength;
        bodyLength=(int)strlen(body);
        newBody = (char *)N_alloc(bodyLength * 3);
        if (bodyLength>0)
            newBody[0]=body[0];
        for (i=1, j=1; i < bodyLength; i++)
        {
            // Transform \n . \r \n into \n . . \r \n
            if (i < bodyLength-2 &&
                body[i-1]=='\n' &&
                body[i+0]=='.' &&
                body[i+1]=='\r' &&
                body[i+2]=='\n')
            {
                newBody[j++]='.';
                newBody[j++]='.';
                newBody[j++]='\r';
                newBody[j++]='\n';
                i+=2;
            }
            // Transform \n . . \r \n into \n . . . \r \n
            // Having to process .. is a bug in the mail server - the spec says ONLY \r\n.\r\n should be transformed
            else if (i <= bodyLength-3 &&
                body[i-1]=='\n' &&
                body[i+0]=='.' &&
                body[i+1]=='.' &&
                body[i+2]=='\r' &&
                body[i+3]=='\n')
            {
                newBody[j++]='.';
                newBody[j++]='.';
                newBody[j++]='.';
                newBody[j++]='\r';
                newBody[j++]='\n';
                i+=3;
            }
            // Transform \n . \n into \n . . \r \n (this is a bug in the mail server - the spec says do not count \n alone but it does)
            else if (i < bodyLength-1 &&
                body[i-1]=='\n' &&
                body[i+0]=='.' &&
                body[i+1]=='\n')
            {
                newBody[j++]='.';
                newBody[j++]='.';
                newBody[j++]='\r';
                newBody[j++]='\n';
                i+=1;
            }
            // Transform \n . . \n into \n . . . \r \n (this is a bug in the mail server - the spec says do not count \n alone but it does)
            // In fact having to process .. is a bug too - because the spec says ONLY \r\n.\r\n should be transformed
            else if (i <= bodyLength-2 &&
                body[i-1]=='\n' &&
                body[i+0]=='.' &&
                body[i+1]=='.' &&
                body[i+2]=='\n')
            {
                newBody[j++]='.';
                newBody[j++]='.';
                newBody[j++]='.';
                newBody[j++]='\r';
                newBody[j++]='\n';
                i+=2;
            }
            else
                newBody[j++]=body[i];
        }

        newBody[j++]='\r';
        newBody[j++]='\n';
        tcpInterface.send(newBody, j, emailServer,false);

        N_free(newBody);
        int outputOffset;

        // What a pain in the rear.  I have to map the binary to printable characters using 6 bits per character.
        if (attachedFiles && attachedFiles->fileList.Size())
        {
            for (i=0; i < (int) attachedFiles->fileList.Size(); i++)
            {
                // write boundary
                sprintf(query, "\r\n--%s\r\n", boundary);
                tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);

                sprintf(query, "Content-Type: APPLICATION/Octet-Stream; SizeOnDisk=%i; name=\"%s\"\r\nContent-Transfer-Encoding: BASE64\r\nContent-Description: %s\r\n\r\n", attachedFiles->fileList[i].dataLengthBytes, attachedFiles->fileList[i].filename.C_String(), attachedFiles->fileList[i].filename.C_String());
                tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);

                //newBody = (char*) N_alloc((size_t) (attachedFiles->fileList[i].dataLengthBytes*3)/2);
                VString newBody1;
                //outputOffset=Base64::Encode((const Nui8*) attachedFiles->fileList[i].mData, (int) attachedFiles->fileList[i].dataLengthBytes, newBody);
                Base64::Encode((const Nui8*) attachedFiles->fileList[i].mData, (int) attachedFiles->fileList[i].dataLengthBytes, newBody1);
                // send the base64 mapped file.
                tcpInterface.send(newBody1.c_str(), outputOffset, emailServer,false);
                //N_free(newBody);

            }

            // write last boundary
            sprintf(query, "\r\n--%s--\r\n", boundary);
            tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);
        }


        sprintf(query, "\r\n.\r\n");
        tcpInterface.send(query, (Nui32)strlen(query), emailServer,false);
        response=GetResponse(&tcpInterface, emailServer, doPrintf);
        if (response!=0)
            return response;

        tcpInterface.send("QUIT\r\n", (Nui32)strlen("QUIT\r\n"), emailServer,false);

        Nsleep(30);
        if (doPrintf)
        {
            msg = tcpInterface.receive();
            while (msg)
            {
                N_printf("%s", msg->mData);
                msg = tcpInterface.receive();
            }
        }
        tcpInterface.Stop();
        return 0; // Success
    }
    //------------------------------------------------------------------------
    const char * EmailSender::GetResponse(NetworkObj * tcpInterface, const Address * emailServer, bool doPrintf)
    {
        TimeDurMS timeout = N_Engine().getTimer().getMS() + 5000;
    #ifdef _MSC_VER
        #pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
    #endif
        while(1)
        {
            if(tcpInterface->onConnectClose() == *emailServer)
                return "Connection to server lost.";
            SocketMessage * msg = tcpInterface->receive();
            if(msg)
            {
                if(doPrintf)
                {
                    N_printf("%s", msg->mData);
                }

                if(strstr((const char *)msg->mData, "220"))
                {
                    tcpInterface->setSSL(msg->mAddress);
                    return "AUTHENTICATE"; // OK
                }

                if(strstr((const char *)msg->mData, "235"))
                    return 0; // Authentication accepted
                if(strstr((const char *)msg->mData, "354"))
                    return 0; // Go ahead
    #if N_OpenSSLSupport==1
                if(strstr((const char *)msg->mData, "250-STARTTLS"))
                {
                    tcpInterface->send("STARTTLS\r\n", (Nui32) strlen("STARTTLS\r\n"), msg->mAddress, false);
                    return "CONTINUE";
                }
    #endif
                if(strstr((const char *)msg->mData, "250"))
                    return 0; // OK
                if(strstr((const char *)msg->mData, "550"))
                    return "Failed on error code 550";
                if(strstr((const char *)msg->mData, "553"))
                    return "Failed on error code 553";
            }
            if(N_Engine().getTimer().getMS() > timeout)
                return "Timed out";
            Nsleep(100);
        }
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*