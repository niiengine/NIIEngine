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

#ifndef PHPENTITYFUNCTION_H
#define PHPENTITYFUNCTION_H

#include "gsgsPreInclude.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase

class _gsgsAPI PHPEntityFunction : public PHPEntityBase
{
public:
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual void PrintStdout(int indent) const;
    
    void FromJSON(const JSONObject& json);
    JSONObject ToJSON() const;
    
protected:
    // The local variabels defined in this function of type
    // PHPEntityVariable
    PHPEntityBase::List_t m_locals;

    // the return value as read from the database
    wxString m_strReturnValue;

    // the signature
    wxString m_strSignature;

public:
    PHPEntityFunction();
    virtual ~PHPEntityFunction();
    const wxString& GetReturnValue() const { return m_strReturnValue; }
    void SetReturnValue(const wxString& strReturnValue) { this->m_strReturnValue = strReturnValue; }

    wxString GetScope() const;
    /**
     * @brief format function signature
     */
    wxString GetSignature() const;
    
    /**
     * @brief return the full path for this function
     * Example: \path\toClass::functionName($a, $b)
     */
    wxString GetFullPath() const;
    
    /**
     * @brief write this object into the database
     * @param db
     */
    virtual void Store(PHPLookupTable* lookup);
    
    virtual wxString ToTooltip() const;
};

#endif // PHPENTITYFUNCTION_H
