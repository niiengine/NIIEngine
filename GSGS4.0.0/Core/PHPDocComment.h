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

#ifndef PHPDOCCOMMENT_H
#define PHPDOCCOMMENT_H

#include "gsgsPreInclude.h"
#include "PHPSourceFile.h"
#include "PHPDocParam.h"

class _gsgsAPI PHPDocComment
{
public:
    struct Property
    {
        wxString name;
        wxString type;
        wxString desc;
        typedef std::unordered_map<wxString, PHPDocComment::Property> Map_t;
    };

protected:
    PHPSourceFile& m_sourceFile;
    wxString m_comment;
    std::unordered_map<wxString, wxString> m_params;
    wxArrayString m_paramsArr;
    wxString m_returnValue;
    wxString m_varType;
    wxString m_varName;
    PHPDocComment::Property::Map_t m_properties; // @property, @property-read, @property-write
    PHPEntityBase::List_t m_methods;

    /**
     * @brief process @method php doc
     */
    void ProcessMethods();

public:
    PHPDocComment(PHPSourceFile& sourceFile, const wxString& comment);
    virtual ~PHPDocComment();

    void SetComment(const wxString& comment) { this->m_comment = comment; }

    const wxString& GetVar() const;
    const wxString& GetReturn() const;
    const wxString& GetParam(const wxString& name) const;
    const wxString& GetParam(size_t index) const;
    const PHPDocComment::Property::Map_t& GetProperties() const { return m_properties; }
    PHPDocComment::Property::Map_t& GetProperties() { return m_properties; }
    const PHPEntityBase::List_t& GetMethods() const { return m_methods; }
};

#endif
