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

#ifndef _NII_GEOMETRY_SPACE_FACTORY_H_
#define _NII_GEOMETRY_SPACE_FACTORY_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObjFactory.h"

namespace NII
{
    /** 创建实体实例的工厂对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometrySpaceFactory : public SpaceObjFactory
    {
    public:
        GeometrySpaceFactory();
        ~GeometrySpaceFactory();

        /// @copydetails SpaceObjFactory::getID
        FactoryID getID() const;
        
        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);        
        
        /// @copydetails SpaceObjFactory::destroy
        void destroy(void * obj);
    };
}

#endif