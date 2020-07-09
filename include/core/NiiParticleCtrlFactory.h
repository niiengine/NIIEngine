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

#ifndef _NII_ParticleCtrl_Factory_H_
#define _NII_ParticleCtrl_Factory_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObjFactory.h"

namespace NII
{
    /** 创建粒子控制工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleCtrlFactory : public SpaceObjFactory
    {
    public:
        ParticleCtrlFactory();
        ~ParticleCtrlFactory();

        /// @copydetails SpaceObjFactory::getID
        FactoryID getID() const;

        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params);

        /// @copydetails SpaceObjFactory::destroy
        void destroy(void * obj);
    };
}
#endif