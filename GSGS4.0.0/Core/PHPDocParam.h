#ifndef PHPDOCPARAM_H
#define PHPDOCPARAM_H

#include "gsgsPreInclude.h"
#include "PHPSourceFile.h"

class _gsgsAPI PHPDocParam
{
public:
    typedef std::vector<std::pair<wxString, wxString> > Vec_t;

protected:
    PHPSourceFile& m_sourceFile;
    const wxString& m_comment;
    PHPDocParam::Vec_t m_params;

public:
    PHPDocParam(PHPSourceFile& sourceFile, const wxString& comment);
    ~PHPDocParam();

    /**
     * @brief parse comment and map of params found in it
     */
    const PHPDocParam::Vec_t& Parse();
};

#endif
