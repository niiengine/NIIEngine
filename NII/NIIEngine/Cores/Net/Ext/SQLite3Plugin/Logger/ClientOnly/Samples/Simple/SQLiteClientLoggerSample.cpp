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
#include "SQLiteClientLoggerPlugin.h"
#include "NiiNetSerializer.h"
#include "Kbhit.h"
#include "NetworkObj.h"
#include "NiiNetCommon.h"
#include "rand.h"
#define M_PI 3.14159265358979323846

int main(void)
{
	printf("Demonstration of SQLiteClientLoggerPlugin.\n");

	ClusterNetworkObj packetizedTCP;
	SQLiteClientLoggerPlugin loggerPlugin;
	packetizedTCP.AttachPlugin(&loggerPlugin);
	packetizedTCP.Start(0,0);
	printf("Connecting.\n");
	Address * serverAddress = packetizedTCP.Connect("127.0.0.1", 38123, true);
	printf("Connected.\n");

	/*
	int *heap1=1234, *heap2=2000;
	loggerPlugin.SetServerParameters(serverAddress, "memoryReport.sqlite");
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("Heaps/Heap1", "new", 1234, "Heap.cpp", heap1, 15000));
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("Heaps/Heap2", "new", 1234, "Heap.cpp", heap2, 10000));
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("General/Graphics/3D", "new", 1234, "3DRenderer.cpp", heap1, 500));
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("General/Graphics/3D", "new", 1235, "3DRenderer.cpp", heap1+500, 500));
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("General/Graphics/2D", "new", 1234, "3DRenderer.cpp", heap1+500*2, 500));
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("General/Graphics/2D", "new", 666, "2DRenderer.cpp", heap2, 1000));
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("General/Graphics/3D", "new", 668, "2DRenderer.cpp", heap2+1000, 1000));
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("Uncategorized", "realloc", 50, "Main.cpp", heap1, -200));
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("Uncategorized", "free", 50, "Main.cpp", heap2, -1000));
	rakSqlLog("memoryReport", "Category,Operation,Line,File,Address,Amount", ("General/Graphics/3D", "free", 800, "3DRenderer.cpp", heap2+1000, -1000));
	*/


	loggerPlugin.SetServerParameters(serverAddress, "functionLog.sqlite");

	SQLLogResult res;
	int x = 1;
	unsigned short y = 2;
	float c = 3;
	NIId d = 4;
	char * e = "HI";
	res = rakFnLog("My func", (x,y,c,d,e,&loggerPlugin));
	N_assert(res == SQLLR_OK);
	res = rakSqlLog("sqlLog", "handle, mapName, positionX, positionY, positionZ, gameMode, connectedPlayers", ("handle1", "mapname1", 1,2,3,"",4));
	N_assert(res == SQLLR_OK);
	res = rakSqlLog("sqlLog", "handle, mapName, positionX, positionY, positionZ, gameMode, connectedPlayers", ("handle2", "mapname2", 5,6,7,"gameMode2",8));
	N_assert(res == SQLLR_OK);
	res = rakSqlLog("sqlLog", "x", (999));
	N_assert(res == SQLLR_OK);
	res = rakFnLog("My func2", ("cat", "carrot", ""));
	N_assert(res == SQLLR_OK);
	loggerPlugin.IncrementAutoTickCount();

	loggerPlugin.SetServerParameters(serverAddress, "scatterPlot.sqlite");

	for (NCount i = 0; i < 1000; ++i)
	{
		res = rakSqlLog("ScatterPlot", "x, y, z, Color, Intensity", (i, (cosf((float)i/30.0)+1.0)*500.0, (sinf((float)i/30.0)+1.0)*500.0, (i%2)==0 ? "red" : "blue", frandomMT()));
		N_assert(res==SQLLR_OK);
		Nsleep(1);
		// Calling recevie() is something you should do anyway, and increments autotick count
		loggerPlugin.IncrementAutoTickCount();
	}

	loggerPlugin.IncrementAutoTickCount();
	loggerPlugin.SetServerParameters(serverAddress, "gradient.sqlite");

	NIId s;
	unsigned int *bytes = new unsigned int[256*256*256];
	for (int i = 0; i < 4096; ++i)
	{
		for (int j = 0; j < 4096; ++j)
		{
			int r,g,b;
			float intensity = 1.0 - (NIId)i/4096.0;
			s = 2.0 * sin(((NIId)j/4096.0)*(2.0*M_PI));
			if (s>0.0)
			{
				if (s>1.0)
					s=1.0;
				r = 255.0 * s;
			}
			else
				r = 0;
			s = 2.0 * sin(((NIId)j/4096.0)*(2.0*M_PI)+2.0*M_PI/3.0);
			if (s > 0.0)
			{
				if (s > 1.0)
					s = 1.0;
				g = 255.0 * s;
			}
			else
				g = 0;
			s = 2.0 * sin(((NIId)j/4096.0)*(2.0*M_PI)+4.0*M_PI/3.0);
			if(s > 0.0)
			{
				if (s>1.0)
					s=1.0;
				b = 255.0 * s;
			}
			else
				b = 0;

			if (intensity>.5)
			{
				r = 255.0-((255.0-(NIId)r)*(255-(2*(intensity-.5))*255.0)) /256.0;
				g = 255.0-((255.0-(NIId)g)*(255-(2*(intensity-.5))*255.0)) /256.0;
				b = 255.0-((255.0-(NIId)b)*(255-(2*(intensity-.5))*255.0)) /256.0;
			}
			else
			{
				r = (intensity*(NIId)r*2.0);
				g = (intensity*(NIId)g*2.0);
				b = (intensity*(NIId)b*2.0);
			}
			bytes[i*4096+j]=(r)|(g<<8)|(b<<16);
		}
	}

	rakSqlLog("gradient", "gradientImage", ( &RGBImageBlob(bytes,4096,4096,4096*4,4) ));
	delete [] bytes;

	Nsleep(5000);

	return 1;
}
