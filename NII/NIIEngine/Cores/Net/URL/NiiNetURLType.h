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

#ifndef _NII_URL_TYPE_H_
#define _NII_URL_TYPE_H_

#include "NiiNetPreInclude.h"

namespace NII
{
namespace NII_NET
{
    /// 主流传输协议
    enum URLType
    {
        URLT_DICT,
        URLT_FILE,
        URLT_FTP,
        URLT_FTPS,
        URLT_Gopher,
        URLT_HTTP,
        URLT_HTTPS,
        URLT_IMAP,
        URLT_IMAPS,
        URLT_LDAP,
        URLT_LDAPS,
        URLT_POP3,
        URLT_POP3S,
        URLT_RTMP,
        URLT_RTSP,
        URLT_SCP,
        URLT_SFTP,
        URLT_SMTP,
        URLT_SMTPS,
        URLT_Telnet,
        URLT_TFTP
    };
}
}
#endif