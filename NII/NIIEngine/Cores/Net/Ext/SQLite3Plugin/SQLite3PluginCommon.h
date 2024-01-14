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
#ifndef __SQL_LITE_3_PLUGIN_COMMON_H
#define __SQL_LITE_3_PLUGIN_COMMON_H

#include "DS_Multilist.h"
#include "RakString.h"
#include "NiiNetSerializer.h"

/// \defgroup SQL_LITE_3_PLUGIN SQLite3Plugin
/// \brief Code to transmit SQLite3 commands across the network
/// \details
/// \ingroup PLUGINS_GROUP

/// Contains a result row, which is just an array of strings
/// \ingroup SQL_LITE_3_PLUGIN
struct SQLite3Row
{
	DataStructures::List<RakString> entries;
};

/// Contains a result table, which is an array of column name strings, followed by an array of SQLite3Row
/// \ingroup SQL_LITE_3_PLUGIN
struct SQLite3Table
{
	SQLite3Table();
	~SQLite3Table();
	void read(NetSerializer * out);
	void write(NetSerializer * in);

	DataStructures::List<RakString> columnNames;
	DataStructures::List<SQLite3Row*> rows;
};

#endif
