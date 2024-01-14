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

#ifndef PHPENTITYVISITOR_H
#define PHPENTITYVISITOR_H

#include "gsgsPreInclude.h"
#include "PHPEntityBase.h"

/**
 * @class PHPEntityVisitor
 * Accept as input a PHPEntitBase and visit all of its children
 */
class _gsgsAPI PHPEntityVisitor
{
public:
    PHPEntityVisitor();
    virtual ~PHPEntityVisitor();
    
    void Visit(PHPEntityBase::Ptr_t parent);
    
    /**
     * @brief called whenver an entity is visited
     * @param entity
     */
    virtual void OnEntity(PHPEntityBase::Ptr_t entity) = 0;
};

#endif // PHPENTITYVISITOR_H
