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

#ifndef PHPDOCVISITOR_H
#define PHPDOCVISITOR_H

#include "gsgsPreInclude.h"
#include "PHPEntityVisitor.h" // Base class: PHPEntityVisitor
#include "PhpLexerAPI.h"
#include "PHPSourceFile.h"

class _gsgsAPI PHPDocVisitor : public PHPEntityVisitor
{
    std::map<int, phpLexerToken> m_comments;
    PHPSourceFile& m_sourceFile;
    
public:
    PHPDocVisitor(PHPSourceFile& sourceFile, const std::vector<phpLexerToken>& comments);
    virtual ~PHPDocVisitor();

public:
    virtual void OnEntity(PHPEntityBase::Ptr_t entity);
};

#endif
