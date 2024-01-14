#ifndef PHPDOCPROPERTY_H
#define PHPDOCPROPERTY_H

#include "gsgsPreInclude.h"
#include "PHPSourceFile.h"
#include <tuple>

class _gsgsAPI PHPDocProperty
{
public:
    typedef std::vector<std::tuple<wxString, wxString, wxString> > Tuple_t;

protected:
    PHPSourceFile& m_sourceFile;
    const wxString& m_comment;
    PHPDocProperty::Tuple_t m_params;
    
    bool NextWord(const wxString& str, size_t &offset, wxString& word);
    
public:
    PHPDocProperty(PHPSourceFile& sourceFile, const wxString& comment);
    ~PHPDocProperty();

    /**
     * @brief parse comment for @params
     */
    const PHPDocProperty::Tuple_t& ParseParams();
    /**
     * @brief parse comment for @method
     */
    const PHPDocProperty::Tuple_t& ParseMethods();
};

#endif
