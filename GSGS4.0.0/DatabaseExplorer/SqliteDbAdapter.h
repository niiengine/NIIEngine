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

#ifndef SQLITEDBADAPTER_H
#define SQLITEDBADAPTER_H

#include <wx/wx.h>
#include "IDbAdapter.h"
#include "table.h"
#include "view.h"
#include "database.h"


#ifdef DBL_USE_SQLITE
#include <wx/dblayer/include/SqliteDatabaseLayer.h>
#endif

#include <wx/dblayer/include/DatabaseLayer.h>
#include "SqliteType.h"

class SQLiteDbAdapter : public IDbAdapter {
public:
	SQLiteDbAdapter();
	SQLiteDbAdapter(const wxString& fileName) ;
	~SQLiteDbAdapter();
	virtual void CloseConnection();
	virtual DatabaseLayerPtr GetDatabaseLayer(const wxString& dbName) ;


	virtual void GetDatabases(DbConnection* dbCon);
	virtual void GetTables(Database* db, bool includeViews);
	virtual bool GetColumns(Table* pTab);
	virtual void GetViews(Database* db);



	virtual IDbType* GetDbTypeByName(const wxString& typeName);
	virtual wxArrayString* GetDbTypes();

	virtual IDbAdapter* Clone();
	virtual bool CanConnect();
	virtual bool IsConnected();

	virtual wxString GetUseDb(const wxString& dbName);

	virtual wxString GetDefaultSelect(const wxString& dbName, const wxString& tableName);
	virtual wxString GetDefaultSelect(const wxString& cols, const wxString& dbName, const wxString& tableName);
	virtual wxString GetCreateTableSql(Table* tab, bool dropTable);
	virtual wxString GetCreateViewSql(View* view, bool dropView);
	virtual wxString GetAlterTableConstraintSql(Table* tab);
	virtual wxString GetCreateDatabaseSql(const wxString& dbName);
	virtual wxString GetDropTableSql(Table* pTab);
	virtual wxString GetDropViewSql(View* pView);
	virtual wxString GetDropDatabaseSql(Database* pDb);


	virtual IDbType* GetDbTypeByUniversalName(IDbType::UNIVERSAL_TYPE type);
	virtual void ConvertTable(Table* pTab);
	virtual IDbType* ConvertType(IDbType* pType);


protected:
	wxString m_sFileName;
};
#endif // SQLITEDBADAPTER_H
