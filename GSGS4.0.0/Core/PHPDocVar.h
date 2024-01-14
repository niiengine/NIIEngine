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
#ifndef PHPDOCVAR_H
#define PHPDOCVAR_H

#include "gsgsPreInclude.h"
#include "PHPSourceFile.h"
#include "gsgsSharedPtr.h"
#include "wx/wxsqlite3.h"

class _gsgsAPI PHPDocVar
{
    bool m_isOk;
    wxString m_name;
    wxString m_type;
    wxLongLong m_dbId;
    wxLongLong m_parentDbId;
    wxFileName m_filename;
    int m_lineNumber;

private:
    void Parse(PHPSourceFile& sourceFile, const wxString& doc);

public:
    typedef SharedPtr<PHPDocVar> Ptr_t;
    typedef std::list<PHPDocVar::Ptr_t> List_t;
    typedef std::map<wxString, PHPDocVar::Ptr_t> Map_t;

public:
    PHPDocVar(PHPSourceFile& sourceFile, const wxString& doc);
    PHPDocVar();
    virtual ~PHPDocVar();

    bool IsOk() const { return m_isOk; }

    void SetName(const wxString& name) { this->m_name = name; }
    void SetType(const wxString& type) { this->m_type = type; }

    const wxString& GetName() const { return m_name; }
    const wxString& GetType() const { return m_type; }

    void SetDbId(const wxLongLong& dbId) { this->m_dbId = dbId; }
    const wxLongLong& GetDbId() const { return m_dbId; }

    void SetParentDbId(const wxLongLong& parentDbId) { this->m_parentDbId = parentDbId; }
    const wxLongLong& GetParentDbId() const { return m_parentDbId; }

    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    void SetIsOk(bool isValid) { this->m_isOk = isValid; }
    void SetLineNumber(int lineNumber) { this->m_lineNumber = lineNumber; }
    const wxFileName& GetFilename() const { return m_filename; }
    int GetLineNumber() const { return m_lineNumber; }

    // Database API
    void Store(wxSQLite3Database& db, wxLongLong parentDdId);
    void FromResultSet(wxSQLite3ResultSet& res);
};

#endif
