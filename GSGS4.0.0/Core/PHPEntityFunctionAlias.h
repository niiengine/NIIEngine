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

#ifndef PHPENTITYFUNCTIONALIAS_H
#define PHPENTITYFUNCTIONALIAS_H

#include "gsgsPreInclude.h"
#include "PHPEntityBase.h"

class _gsgsAPI PHPEntityFunctionAlias : public PHPEntityBase
{
    wxString m_realname;
    wxString m_scope;

    PHPEntityBase::Ptr_t m_func;

public:
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual void PrintStdout(int indent) const;
    virtual wxString Type() const;
    PHPEntityFunctionAlias();
    virtual ~PHPEntityFunctionAlias();
    
    void FromJSON(const JSONObject &json);
    JSONObject ToJSON() const;
    
public:
    virtual bool Is(eEntityType type) const;
    virtual void Store(PHPLookupTable* lookup);
    virtual void FromResultSet(wxSQLite3ResultSet& res);

    void SetScope(const wxString& scope) { this->m_scope = scope; }
    const wxString& GetScope() const { return m_scope; }
    void SetRealname(const wxString& realname) { this->m_realname = realname; }
    const wxString& GetRealname() const { return m_realname; }
    void SetFunc(PHPEntityBase::Ptr_t func) { this->m_func = func; }
    PHPEntityBase::Ptr_t GetFunc() const { return m_func; }
};

#endif // PHPENTITYFUNCTIONALIAS_H
