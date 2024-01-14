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

#ifndef __SQL_LITE_CLIENT_LOGGER_RAKNET_STATISTICS_H_
#define __SQL_LITE_CLIENT_LOGGER_RAKNET_STATISTICS_H_

#include "NiiNetPlugin.h"

namespace NII
{
namespace NII_NET
{
	/// \ingroup PACKETLOGGER_GROUP
	/// \brief Packetlogger that outputs to a file
	class RAK_DLL_EXPORT SQLiteClientLogger_RakNetStatistics : public Plugin
	{
	public:
		SQLiteClientLogger_RakNetStatistics();
		virtual ~SQLiteClientLogger_RakNetStatistics();
		virtual void update();
	protected:
		TimeDurUS lastUpdate;
	};
}
}
#endif
