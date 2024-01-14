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

#ifndef IDBADAPTER_H
#define IDBADAPTER_H
#include <wx/wx.h>
#include "gsgsSharedPtr.h"
#include <wx/dynarray.h>
#include <wx/dblayer/include/DatabaseLayer.h>
#include "IDbType.h"
//#include "columncol.h"
//#include "tablecol.h"
//#include "databasecol.h"
class DbConnection;
class Database;
class Table;
class View;
class IDbType;


typedef gsgs::SharedPtr<DatabaseLayer> DatabaseLayerPtr;

/*! \brief Basic virtual class for creating universal interface between different database servers. */
class IDbAdapter
{

public:
    enum TYPE {
        atUNKNOWN = 0,
        atSQLITE,
        atMYSQL,
        atPOSTGRES
    };
    
    IDbAdapter()
        : m_adapterType(atUNKNOWN)
    {
    }

    virtual ~IDbAdapter() {}
    
    /*! \brief Return opened DatabaseLayer for selected database. If dbName is empty, DatabaseLayer will be opend without defalut database. */
    virtual DatabaseLayerPtr GetDatabaseLayer(const wxString& dbName) = 0;

    /*! \brief Return true if dbAdapter is connected. DEPRECATED!!! */
    virtual bool IsConnected() = 0;

    /*! \brief Clone DbAdapter and return new object */
    virtual IDbAdapter* Clone() = 0;

    /*! \brief Return true if adapter settings are set and adapter can connect to the database */
    virtual bool CanConnect() = 0;

    /*! \brief Function connect databases to the DbConnection */
    virtual void GetDatabases(DbConnection* dbCon) = 0;

    /*! \brief Function connect tables to the Database */
    virtual void GetTables(Database* db, bool includeViews) = 0;

    /*! \brief Function connect views to the Database */
    virtual void GetViews(Database* db) = 0;

    /*! \brief Function connect columns to the DBETable */
    virtual bool GetColumns(Table* pTab) = 0;


    /*! \brief Return wxString with USE DB statement for dbName */
    virtual wxString GetUseDb(const wxString& dbName) = 0;
    /*! \brief Return wxString with defalut SELECT for defined table and db */
    virtual wxString GetDefaultSelect(const wxString& dbName, const wxString& tableName) = 0;
    virtual wxString GetDefaultSelect(const wxString& cols, const wxString& dbName, const wxString& tableName) = 0;
    /*! \brief Return Create table sql statement */
    virtual wxString GetCreateTableSql(Table* tab, bool dropTable) = 0;
    /*! \brief Return Create view sql statement */
    virtual wxString GetCreateViewSql(View* view, bool dropView) = 0;
    /*! \brief Return Create update table SQL statement for adding constraints */
    virtual wxString GetAlterTableConstraintSql(Table* tab) = 0;
    /*! \brief Return Create database SQL statement. */
    virtual wxString GetCreateDatabaseSql(const wxString& dbName) = 0;
    /*! \brief Return Drop table SQL statement. */
    virtual wxString GetDropTableSql(Table* pTab) = 0;
    /*! \brief Return Drop view SQL statement. */
    virtual wxString GetDropViewSql(View* pView) = 0;
    /*! \brief Return Drop database SQL statement. */
    virtual wxString GetDropDatabaseSql(Database* pDb) = 0;

    virtual void CloseConnection() = 0;

    /*! \brief Return IDbType by name string */
    virtual IDbType* GetDbTypeByName(const wxString& typeName) = 0;
    /*! \brief Return IDbType by name universal name */
    virtual IDbType* GetDbTypeByUniversalName(IDbType::UNIVERSAL_TYPE type) = 0;
    /*! \brief Return wxArrayString of possible db types */
    virtual wxArrayString* GetDbTypes () = 0;

    /*! \brief Convert IDbType to the special db type. (!!! Old type is deleted !!!) */
    virtual IDbType* ConvertType (IDbType* pType) = 0;
    /*! \brief Return wxArrayString of possible db types */
    virtual void ConvertTable(Table* pTab) = 0;

    const TYPE& GetAdapterType() const {
        return m_adapterType;
    }

protected:
    TYPE m_adapterType;

};

#endif // IDBADAPTER_H
