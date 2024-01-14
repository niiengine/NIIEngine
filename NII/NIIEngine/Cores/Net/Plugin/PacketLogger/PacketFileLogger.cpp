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
#if _RAKNET_SUPPORT_PacketLogger==1
#include "PacketFileLogger.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    PacketFileLogger::PacketFileLogger()
    {
        packetLogFile = 0;
    }
    //------------------------------------------------------------------------
    PacketFileLogger::~PacketFileLogger()
    {
        if (packetLogFile)
        {
            fflush(packetLogFile);
            fclose(packetLogFile);
        }
    }
    //------------------------------------------------------------------------
    void PacketFileLogger::StartLog(const String & filenamePrefix)
    {
        // Open file for writing
        char filename[256];
        if (filenamePrefix)
            sprintf(filename, "%s_%i.csv", filenamePrefix.c_str(), N_Engine().getTimer().getMS());
        else
            sprintf(filename, "PacketLog_%i.csv", N_Engine().getTimer().getMS());
        packetLogFile = fopen(filename, "wt");
        LogHeader();
        if (packetLogFile)
        {
            fflush(packetLogFile);
        }
    }
    //------------------------------------------------------------------------
    void PacketFileLogger::WriteLog(const String & str)
    {
        if (packetLogFile)
        {
            fprintf(packetLogFile, "%s\n", str.c_str());
            fflush(packetLogFile);
        }
    }
    //------------------------------------------------------------------------
}
}
#endif