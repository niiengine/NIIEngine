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

#ifndef PHPENTITYVARIABLE_H
#define PHPENTITYVARIABLE_H

#include "gsgsPreInclude.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase

class _gsgsAPI PHPEntityVariable : public PHPEntityBase
{
public:
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    virtual void FromResultSet(wxSQLite3ResultSet& res);

protected:
    wxString m_typeHint;

    // Incase this variable is instantiated with an expression
    // keep it
    wxString m_expressionHint;
    wxString m_defaultValue;

public:
    /**
     * @brief
     * @return
     */
    wxString GetScope() const;
    virtual void Store(PHPLookupTable* lookup);
    virtual void PrintStdout(int indent) const;
    
    void FromJSON(const JSONObject& json);
    JSONObject ToJSON() const;
    
    /**
     * @brief format this variable
     */
    wxString ToFuncArgString() const;
    PHPEntityVariable();
    virtual ~PHPEntityVariable();

    void SetExpressionHint(const wxString& expressionHint) { this->m_expressionHint = expressionHint; }
    const wxString& GetExpressionHint() const { return m_expressionHint; }
    void SetVisibility(int visibility);
    void SetTypeHint(const wxString& typeHint) { this->m_typeHint = typeHint; }
    const wxString& GetTypeHint() const { return m_typeHint; }
    void SetDefaultValue(const wxString& defaultValue) { this->m_defaultValue = defaultValue; }
    const wxString& GetDefaultValue() const { return m_defaultValue; }
    wxString GetNameNoDollar() const;
    virtual wxString ToTooltip() const;
    
    // Aliases
    void SetIsReference(bool isReference) { SetFlag(kVar_Reference, isReference); }
    bool IsMember() const { return HasFlag(kVar_Member); }
    bool IsPublic() const { return HasFlag(kVar_Public); }
    bool IsPrivate() const { return HasFlag(kVar_Private); }
    bool IsProtected() const { return HasFlag(kVar_Protected); }
    bool IsFunctionArg() const { return HasFlag(kVar_FunctionArg); }
    bool IsConst() const { return HasFlag(kVar_Const); }
    bool IsReference() const { return HasFlag(kVar_Reference); }
    bool IsStatic() const { return HasFlag(kVar_Static); }
    bool IsDefine() const { return HasFlag(kVar_Define); }
    bool IsBoolean() const { return GetTypeHint() == "boolean" || GetTypeHint() == "bool"; }
};

#endif // PHPENTITYVARIABLE_H
