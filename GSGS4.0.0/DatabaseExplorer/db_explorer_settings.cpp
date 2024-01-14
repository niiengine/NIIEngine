//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : db_explorer_settings.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "db_explorer_settings.h"

// -----------------------------------------------------------------
// DbExplorerSettings
// -----------------------------------------------------------------

DbExplorerSettings::DbExplorerSettings()
    : ObjectData(DBE_CONFIG)
{
}

DbExplorerSettings::~DbExplorerSettings()
{
}

void DbExplorerSettings::SetRecentFiles(const wxArrayString& recentFiles)
{
    m_recentFiles.Clear();
    for(size_t i=0; i<recentFiles.Count(); i++) {
        if(m_recentFiles.Index(recentFiles.Item(i)) == wxNOT_FOUND) {
            m_recentFiles.Add(recentFiles.Item(i));
        }
    }
}

DbConnectionInfoVec DbExplorerSettings::GetMySQLConnections()
{
    DbConnectionInfoVec conns;
    for(size_t i=0; i<m_connections.size(); i++) {
        if(m_connections.at(i).GetConnectionType() == DbConnectionInfo::DbConnTypeMySQL) {
            conns.push_back(m_connections.at(i));
        }
    }
    return conns;
}

DbConnectionInfoVec DbExplorerSettings::GetPgSQLConnections()
{
    DbConnectionInfoVec conns;
    for(size_t i=0; i<m_connections.size(); i++) {
        if(m_connections.at(i).GetConnectionType() == DbConnectionInfo::DbConnTypePgSQL) {
            conns.push_back(m_connections.at(i));
        }
    }
    return conns;
}

void DbExplorerSettings::SetMySQLConnections(const DbConnectionInfoVec& conns)
{
    DbConnectionInfoVec pgconns = GetPgSQLConnections();
    m_connections.clear();

    m_connections.insert(m_connections.end(), pgconns.begin(), pgconns.end());
    m_connections.insert(m_connections.end(), conns.begin(), conns.end());
}

void DbExplorerSettings::SetPgSQLConnections(const DbConnectionInfoVec& conns)
{
    DbConnectionInfoVec myconns = GetMySQLConnections();
    m_connections.clear();

    m_connections.insert(m_connections.end(), myconns.begin(), myconns.end());
    m_connections.insert(m_connections.end(), conns.begin(), conns.end());
}

void DbExplorerSettings::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_recentFiles").getValue(m_recentFiles);
    json.getObject("m_sqlHistory").getValue(m_sqlHistory);
    
    // read the connections
    gsgs::JSONObject arrConnections = json.getObject("connections");
    for(int i=0; i<arrConnections.getArraySize(); ++i) {
        DbConnectionInfo ci;
        ci.FromJSON( arrConnections.getArray(i) );
        m_connections.push_back( ci );
    }
}

gsgs::JSONObject DbExplorerSettings::ToJSON() const
{
    gsgs::JSONObject element = gsgs::JSONObject::createObject(GetName());
    element.addProperty("m_recentFiles", m_recentFiles);
    element.addProperty("m_sqlHistory",  m_sqlHistory);
    
    // add the connections array
    gsgs::JSONObject arrConnections = gsgs::JSONObject::createArray("connections");
    element.arrayAppend(arrConnections);
    DbConnectionInfoVec::const_iterator iter = m_connections.begin();
    for(; iter != m_connections.end(); ++iter) {
        arrConnections.arrayAppend( iter->ToJSON() );
    }
    return element;
}

//---------------------------------------------------
// Database Connection info
//---------------------------------------------------

DbConnectionInfo::DbConnectionInfo()
    : ObjectData("connection-info")
    , m_connectionType(DbConnTypeMySQL)
{
}

DbConnectionInfo::~DbConnectionInfo()
{
}

gsgs::JSONObject DbConnectionInfo::ToJSON() const
{
    gsgs::JSONObject element = gsgs::JSONObject::createObject(GetName());
    element.addProperty("m_connectionName",  m_connectionName );
    element.addProperty("m_connectionType",  m_connectionType );
    element.addProperty("m_defaultDatabase", m_defaultDatabase);
    element.addProperty("m_password"       , m_password       );
    element.addProperty("m_server"         , m_server         );
    element.addProperty("m_port"           , m_port           );
    element.addProperty("m_username"       , m_username       );
    return element;
}

void DbConnectionInfo::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_connectionName").getValue(m_connectionName, m_connectionName);
    json.getObject("m_connectionType").getValue(m_connectionType, m_connectionType);
    json.getObject("m_defaultDatabase").getValue(m_defaultDatabase, m_defaultDatabase);
    json.getObject("m_password").getValue(m_password, m_password);
    json.getObject("m_server").getValue(m_server, m_server);
    json.getObject("m_port").getValue(m_port, m_port);
    json.getObject("m_username").getValue(m_username, m_username);
}
