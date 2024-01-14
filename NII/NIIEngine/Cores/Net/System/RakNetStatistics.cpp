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

#include "NetStats.h"
#include "RakString.h"
    
namespace NII
{
namespace NII_NET
{
    // Verbosity level currently supports 0 (low), 1 (medium), 2 (high)
    // Buffer must be hold enough to hold the output string.  See the source to get an idea of how many bytes will be output
    void _EngineAPI StatisticsToString( NetStats *s, char *buffer, int verbosityLevel )
    {
        if ( s == 0 )
        {
            sprintf( buffer, "stats is a NULL pointer in statsToString\n" );
            return ;
        }

        if (verbosityLevel==0)
        {
            sprintf(buffer,
                "Bytes per second sent     %" N_64BIT_MOD "u\n"
                "Bytes per second received %" N_64BIT_MOD "u\n"
                "Current packetloss        %.1f%%\n",
                (Nui64) s->valueOverLastSecond[ACTUAL_BYTES_SENT],
                (Nui64) s->valueOverLastSecond[ACTUAL_BYTES_RECEIVED],
                s->packetlossLastSecond*100.0f
                );
        }
        else if (verbosityLevel==1)
        {
            sprintf(buffer,
                "Actual bytes per second sent       %" N_64BIT_MOD "u\n"
                "Actual bytes per second received   %" N_64BIT_MOD "u\n"
                "SocketMessage bytes per second pushed    %" N_64BIT_MOD "u\n"
                "Total actual bytes sent            %" N_64BIT_MOD "u\n"
                "Total actual bytes received        %" N_64BIT_MOD "u\n"
                "Total message bytes pushed         %" N_64BIT_MOD "u\n"
                "Current packetloss                 %.1f%%\n"
                "Average packetloss                 %.1f%%\n"
                "Elapsed connection time in seconds %" N_64BIT_MOD "u\n",
                (Nui64) s->valueOverLastSecond[ACTUAL_BYTES_SENT],
                (Nui64) s->valueOverLastSecond[ACTUAL_BYTES_RECEIVED],
                (Nui64) s->valueOverLastSecond[USER_MESSAGE_BYTES_PUSHED],
                (Nui64) s->runningTotal[ACTUAL_BYTES_SENT],
                (Nui64) s->runningTotal[ACTUAL_BYTES_RECEIVED],
                (Nui64) s->runningTotal[USER_MESSAGE_BYTES_PUSHED],
                s->packetlossLastSecond*100.0f,
                s->packetlossTotal*100.0f,
                (Nui64) (Nui64)((N_Engine().getTimer().getUS()-s->connectionStartTime)/1000000)
                );

            if (s->BPSLimitByCongestionControl != 0)
            {
                char buff2[128];
                sprintf(buff2,
                    "send capacity                    %" N_64BIT_MOD "u bytes per second (%.0f%%)\n",
                    (Nui64) s->BPSLimitByCongestionControl,
                    100.0f * s->valueOverLastSecond[ACTUAL_BYTES_SENT] / s->BPSLimitByCongestionControl
                    );
                strcat(buffer,buff2);
            }
            if (s->mUploadLimitValue!=0)
            {
                char buff2[128];
                sprintf(buff2,
                    "send limit                       %" N_64BIT_MOD "u (%.0f%%)\n",
                    (Nui64) s->mUploadLimitValue,
                    100.0f * s->valueOverLastSecond[ACTUAL_BYTES_SENT] / s->mUploadLimitValue
                    );
                strcat(buffer,buff2);
            }
        }
        else
        {
            sprintf(buffer,
                "Actual bytes per second sent         %" N_64BIT_MOD "u\n"
                "Actual bytes per second received     %" N_64BIT_MOD "u\n"
                "SocketMessage bytes per second sent        %" N_64BIT_MOD "u\n"
                "SocketMessage bytes per second resent      %" N_64BIT_MOD "u\n"
                "SocketMessage bytes per second pushed      %" N_64BIT_MOD "u\n"
                "SocketMessage bytes per second returned      %" N_64BIT_MOD "u\n"
                "SocketMessage bytes per second ignored     %" N_64BIT_MOD "u\n"
                "Total bytes sent                     %" N_64BIT_MOD "u\n"
                "Total bytes received                 %" N_64BIT_MOD "u\n"
                "Total message bytes sent             %" N_64BIT_MOD "u\n"
                "Total message bytes resent           %" N_64BIT_MOD "u\n"
                "Total message bytes pushed           %" N_64BIT_MOD "u\n"
                "Total message bytes returned          %" N_64BIT_MOD "u\n"
                "Total message bytes ignored          %" N_64BIT_MOD "u\n"
                "Messages in send buffer, by plevel %i,%i,%i,%i\n"
                "Bytes in send buffer, by plevel    %i,%i,%i,%i\n"
                "Messages in resend buffer            %i\n"
                "Bytes in resend buffer               %" N_64BIT_MOD "u\n"
                "Current packetloss                   %.1f%%\n"
                "Average packetloss                   %.1f%%\n"
                "Elapsed connection time in seconds   %" N_64BIT_MOD "u\n",
                (Nui64) s->valueOverLastSecond[ACTUAL_BYTES_SENT],
                (Nui64) s->valueOverLastSecond[ACTUAL_BYTES_RECEIVED],
                (Nui64) s->valueOverLastSecond[USER_MESSAGE_BYTES_SENT],
                (Nui64) s->valueOverLastSecond[USER_MESSAGE_BYTES_RESENT],
                (Nui64) s->valueOverLastSecond[USER_MESSAGE_BYTES_PUSHED],
                (Nui64) s->valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_PROCESSED],
                (Nui64) s->valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_IGNORED],
                (Nui64) s->runningTotal[ACTUAL_BYTES_SENT],
                (Nui64) s->runningTotal[ACTUAL_BYTES_RECEIVED],
                (Nui64) s->runningTotal[USER_MESSAGE_BYTES_SENT],
                (Nui64) s->runningTotal[USER_MESSAGE_BYTES_RESENT],
                (Nui64) s->runningTotal[USER_MESSAGE_BYTES_PUSHED],
                (Nui64) s->runningTotal[USER_MESSAGE_BYTES_RECEIVED_PROCESSED],
                (Nui64) s->runningTotal[USER_MESSAGE_BYTES_RECEIVED_IGNORED],
                s->messageInSendBuffer[PL_RAPID],s->messageInSendBuffer[PL_HIGH],s->messageInSendBuffer[PL_MEDIUM],s->messageInSendBuffer[PL_LOW],
                (Nui32) s->bytesInSendBuffer[PL_RAPID],(Nui32) s->bytesInSendBuffer[PL_HIGH],(Nui32) s->bytesInSendBuffer[PL_MEDIUM],(Nui32) s->bytesInSendBuffer[PL_LOW],
                s->messagesInResendBuffer,
                (Nui64) s->bytesInResendBuffer,
                s->packetlossLastSecond*100.0f,
                s->packetlossTotal*100.0f,
                (Nui64) (Nui64)((N_Engine().getTimer().getUS()-s->connectionStartTime)/1000000)
                );

            if (s->BPSLimitByCongestionControl != 0)
            {
                char buff2[128];
                sprintf(buff2,
                    "send capacity                    %" N_64BIT_MOD "u bytes per second (%.0f%%)\n",
                    (Nui64) s->BPSLimitByCongestionControl,
                    100.0f * s->valueOverLastSecond[ACTUAL_BYTES_SENT] / s->BPSLimitByCongestionControl
                    );
                strcat(buffer,buff2);
            }
            if (s->mUploadLimitValue!=0)
            {
                char buff2[128];
                sprintf(buff2,
                    "send limit                       %" N_64BIT_MOD "u (%.0f%%)\n",
                    (Nui64) s->mUploadLimitValue,
                    100.0f * s->valueOverLastSecond[ACTUAL_BYTES_SENT] / s->mUploadLimitValue
                    );
                strcat(buffer,buff2);
            }
        }
    }
}
}