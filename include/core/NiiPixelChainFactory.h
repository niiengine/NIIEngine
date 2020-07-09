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

#ifndef _NII_PixelChain_Factory_H_
#define _NII_PixelChain_Factory_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObjFactory.h"

namespace NII
{
    /** 创建PixelChain实例的工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelChainFactory : public SpaceObjFactory
    {
    public:
        PixelChainFactory() {}
        ~PixelChainFactory() {}

        /// @copydetails SpaceObjFactory::getID
        FactoryID getID() const;

        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);

        /// @copydetails SpaceObjFactory::destroy
        void destroy(void * obj);
    };
}

#endif