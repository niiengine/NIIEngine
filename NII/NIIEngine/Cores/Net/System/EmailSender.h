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
#if _RAKNET_SUPPORT_EmailSender==1 && _RAKNET_SUPPORT_TCPInterface==1 && _RAKNET_SUPPORT_FileOperations==1

#ifndef __EMAIL_SENDER_H
#define __EMAIL_SENDER_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "Rand.h"
#include "NetworkObj.h"

namespace NII
{
namespace NII_NET
{
    /// \brief Rudimentary class to send email from code.
    class _EngineAPI EmailSender
    {
    public:
        /// \brief Sends an email.
        /// \param[in] hostAddress The address of the email server.
        /// \param[in] hostPort The port of the email server (usually 25)
        /// \param[in] sender The email address you are sending from.
        /// \param[in] recipient The email address you are sending to.
        /// \param[in] senderName The email address you claim to be sending from
        /// \param[in] recipientName The email address you claim to be sending to
        /// \param[in] subject Email subject
        /// \param[in] body Email body
        /// \param[in] attachedFiles List of files to attach to the email. (Can be 0 to send none).
        /// \param[in] doPrintf true to output SMTP info to console(for debugging?)
        /// \param[in] password Used if the server uses AUTHENTICATE PLAIN over TLS (such as gmail)
        /// \return 0 on success, otherwise a string indicating the error message
        const char * send(const String & host, Nui16 port, const char * sender, const char * recipient, 
            const char * senderName, const char * recipientName, const char * subject, const char * body,
                FileList *attachedFiles, bool doPrintf, const char * password);
    protected:
        const char * GetResponse(NetworkObj * tcpInterface, const Address * emailServer, bool doPrintf);
        RakNetRandom rakNetRandom;
    };
}
}
#endif
#endif