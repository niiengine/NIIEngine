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

#ifndef PHPENTITYCLASSIMPL_H
#define PHPENTITYCLASSIMPL_H

#include "gsgsPreInclude.h"
#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include "PHPDocVar.h"

class _gsgsAPI PHPEntityClass : public PHPEntityBase
{
    wxString m_extends;
    wxArrayString m_implements;
    wxArrayString m_traits;
    PHPDocVar::List_t m_varPhpDocs; // List of @var defined inside the class body

public:
    virtual wxString FormatPhpDoc(const CommentConfigData& data) const;
    virtual wxString GetDisplayName() const;
    virtual bool Is(eEntityType type) const;
    virtual wxString Type() const;
    // Save the class into teh database
    virtual void Store(PHPLookupTable* lookup);
    virtual void FromResultSet(wxSQLite3ResultSet& res);
    virtual void PrintStdout(int indent) const;
    
    void FromJSON(const JSONObject& json);
    JSONObject ToJSON() const;
    
    /**
     * @brief return an array of inheritance (extends, implementes and traits)
     */
    wxArrayString GetInheritanceArray() const;

    PHPEntityClass();
    virtual ~PHPEntityClass();
    void SetExtends(const wxString& extends) { this->m_extends = extends; }
    const wxString& GetExtends() const { return m_extends; }
    void SetImplements(const wxArrayString& implements) { this->m_implements = implements; }
    const wxArrayString& GetImplements() const { return m_implements; }
    void SetTraits(const wxArrayString& traits) { this->m_traits = traits; }
    const wxArrayString& GetTraits() const { return m_traits; }
    wxString GetImplementsAsString() const { return ::wxJoin(m_implements, ';'); }
    wxString GetTraitsAsString() const { return ::wxJoin(m_traits, ';'); }

    const PHPDocVar::List_t& GetVarPhpDocs() const { return m_varPhpDocs; }
    PHPDocVar::List_t& GetVarPhpDocs() { return m_varPhpDocs; }
    void AddVarPhpDoc(PHPDocVar::Ptr_t doc) { GetVarPhpDocs().push_back(doc); }

    // Aliases
    void SetIsInterface(bool b) { SetFlag(kClass_Interface, b); }
    bool IsInterface() const { return HasFlag(kClass_Interface); }
    void SetIsTrait(bool b) { SetFlag(kClass_Trait, b); }
    bool IsTrait() const { return HasFlag(kClass_Trait); }
    void SetIsAbstractClass(bool b) { SetFlag(kClass_Abstract, b); }
    bool IsAbstractClass() const { return HasFlag(kClass_Abstract); }
};

#endif // PHPENTITYCLASSIMPL_H
