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

#ifndef PHPENTITYNAMESPACE_H
#define PHPENTITYNAMESPACE_H

#include "gsgsPreInclude.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase

class _gsgsAPI PHPEntityNamespace : public PHPEntityBase
{
protected:
    void DoEnsureNamespacePathExists(wxSQLite3Database& db, const wxString& path);

public:
    void FromJSON(const JSONObject& json);
    JSONObject ToJSON() const;
    
    wxString GetParentNamespace() const;
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual void Store(PHPLookupTable* lookup);
    virtual void PrintStdout(int indent) const;
    PHPEntityNamespace();
    virtual ~PHPEntityNamespace();
    
    /**
     * @brief build namespace from 2 strings
     */
    static wxString BuildNamespace(const wxString &part1, const wxString &part2);
};

#endif // PHPENTITYNAMESPACE_H
