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

#ifndef _NII_SPACEOBJ_FACTORY_H_
#define _NII_SPACEOBJ_FACTORY_H_

#include "NiiPreInclude.h"
#include "NiiFactoryObj.h"
#include "NiiSpaceObj.h"

namespace NII
{
    /** 空间对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceObjFactory : public FactoryObj
    {
    public:
        SpaceObjFactory() : mTypeMark(0xFFFFFFFF) {}
        virtual ~SpaceObjFactory() {}

        /** 创建实例
        @param[in] sid
        @param[in] mag
        @param[in] params
        @version NIIEngine 3.0.0
        */
        virtual SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0) = 0;

        /** 设置工厂类的类型掩码
        @version NIIEngine 3.0.0
        */
        void setTypeMark(Nmark mark) { mTypeMark = mark; }

        /** 获取工厂类的类型掩码
        @version NIIEngine 3.0.0
        */
        Nmark getTypeMark() const { return mTypeMark; }
    protected:
        Nmark mTypeMark;
    };
}

#endif