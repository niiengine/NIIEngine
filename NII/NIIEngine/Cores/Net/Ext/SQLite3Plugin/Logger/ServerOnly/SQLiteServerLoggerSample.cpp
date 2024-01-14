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
#include "NiiNetLinkBase.h"
#include "SQLiteServerLoggerPlugin.h"
#include "NiiNetSerializer.h"
#include "Nsleep.h"

#include "Kbhit.h"
#include "NetworkObj.h"


int main(void)
{
	printf("Demonstration of SQLiteServerLoggerPlugin.\n");

	ClusterNetworkObj packetizedTCP;
	SQLiteServerLoggerPlugin loggerPlugin;
	loggerPlugin.SetEnableDXTCompression(true);
	loggerPlugin.SetSessionManagementMode(SQLiteServerLoggerPlugin::CREATE_SHARED_NAMED_DB_HANDLE, true, "");

	packetizedTCP.AttachPlugin(&loggerPlugin);
	packetizedTCP.Start(38123,8);
	printf("\nStarted.\n");
	bool quit = false;
	bool isProcessing = false;

	SQLiteServerLoggerPlugin::ProcessingStatus processingStatusNew;
	SQLiteServerLoggerPlugin::ProcessingStatus processingStatusOld;
	memset(&processingStatusOld,0,sizeof(processingStatusOld));

	Address * sa;
	while (quit == false || isProcessing == true)
	{
		SocketMessage * p;
		for(p = packetizedTCP.recevie(); p; N_delete p, p = packetizedTCP.recevie())
		{
			;
		}
        String temp;
		sa = packetizedTCP.onServerConnect(sa);
		if (sa && !sa->isInvalid())
        {
            sa->read(temp, true);
			printf("New incoming connection from %s\n", temp);
		}
        sa = packetizedTCP.onConnectClose(sa);
		if (sa && !sa->isInvalid())
        {
            sa->read(temp, true);
			printf("Lost connection from %s\n", temp);
		}
        sa = packetizedTCP.onConnectFail(sa);
		sa = packetizedTCP.onClientConnect(sa);
		Nsleep(0);

		if (kbhit())
		{
			if (getch() == 'q')
			{
				printf("Quitting as soon as threads finish.\n");
				packetizedTCP.Stop();
				quit = true;
			}
		}

		loggerPlugin.GetProcessingStatus(&processingStatusNew);
		if (memcmp(&processingStatusNew,&processingStatusOld,sizeof(processingStatusOld))!=0)
		{
			printf("buffered=%i cpuWait=%i cpuDo=%i cpuDone=%i sqlWait=%i sqlDo=%i sqlDone=%i\n",
				processingStatusNew.packetsBuffered,
				processingStatusNew.cpuPendingProcessing,processingStatusNew.cpuNumThreadsWorking,processingStatusNew.cpuProcessedAwaitingDeallocation,
				processingStatusNew.sqlPendingProcessing,processingStatusNew.sqlNumThreadsWorking,processingStatusNew.sqlProcessedAwaitingDeallocation
				);
			memcpy(&processingStatusOld,&processingStatusNew,sizeof(processingStatusOld));
		}

		if (processingStatusNew.cpuNumThreadsWorking==processingStatusNew.cpuPendingProcessing==processingStatusNew.cpuProcessedAwaitingDeallocation==
			processingStatusNew.packetsBuffered==processingStatusNew.sqlNumThreadsWorking==processingStatusNew.sqlPendingProcessing==processingStatusNew.sqlProcessedAwaitingDeallocation==0)
			isProcessing=false;
		else
			isProcessing=true;
	}

	loggerPlugin.CloseAllSessions();
	return 1;
}