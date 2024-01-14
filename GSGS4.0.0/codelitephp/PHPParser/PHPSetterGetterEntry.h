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

#ifndef PHPSETTERGETTERENTRY_H
#define PHPSETTERGETTERENTRY_H

#include <wx/string.h>
#include <vector>
#include "PHPEntityBase.h"

enum eSettersGettersFlags {
    kSG_None = 0,
    kSG_StartWithLowercase = (1 << 0),
    kSG_NameOnly = (1 << 1),
    kSG_NoPrefix = (1 << 2),
    kSG_ReturnThis = (1 << 3),
};

class PHPSetterGetterEntry
{
public:
    typedef std::vector<PHPSetterGetterEntry> Vec_t;

protected:
    PHPEntityBase::Ptr_t m_entry;

protected:
    void FormatName(wxString& name, size_t flags) const;

public:
    PHPSetterGetterEntry();
    PHPSetterGetterEntry(PHPEntityBase::Ptr_t entry);
    virtual ~PHPSetterGetterEntry();

    /**
     * @brief generate getter for the this PHPEntry
     * @param flags generation flags
     */
    wxString GetGetter(size_t flags = kSG_None) const;

    /**
     * @brief generate setter for the this PHPEntry
     * @param flags generation flags
     */
    wxString GetSetter(const wxString& scope, size_t flags = kSG_None) const;
};

#endif // PHPSETTERGETTERENTRY_H
