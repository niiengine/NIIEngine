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

#ifndef PHPENTITYKEYWORD_H
#define PHPENTITYKEYWORD_H

#include "PHPEntityBase.h"
#include <wx/string.h>

class _gsgsAPI PHPEntityKeyword : public PHPEntityBase
{
public:
    PHPEntityKeyword();
    virtual ~PHPEntityKeyword();
    void FromJSON(const JSONObject& json);
    JSONObject ToJSON() const;
    
public:
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual void PrintStdout(int indent) const;
    virtual void Store(PHPLookupTable* lookup);
    virtual wxString Type() const;
};

#endif
