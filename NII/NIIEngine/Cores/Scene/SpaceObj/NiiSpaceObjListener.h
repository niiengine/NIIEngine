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

#ifndef _NII_SPACEOBJ_LISTENER_H_
#define _NII_SPACEOBJ_LISTENER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 事件监听
    @remark 可用于物理引擎/阴影渲染器控制
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI SpaceObjListener
    {
    public:
        SpaceObjListener();
        virtual ~SpaceObjListener();

        /** 创建后时
        @version NIIEngine 3.0.0
        */
        virtual void create(SpaceObj * obj);
        
        /** 删除时
        @version NIIEngine 3.0.0
        */
        virtual void destroy(SpaceObj * obj);
        
        /** 位置发生改变
        @version NIIEngine 3.0.0
        */
        virtual void notify(SpaceObj * obj);

        /** 附加到节点时
        @note 仅在参数2非0时触发
        @version NIIEngine 3.0.0
        */
        virtual void attach(SpaceObj * obj, const PosNode * np);

        /** 从节点中分离时
        @note 仅在参数2非0时触发
        @version NIIEngine 3.0.0
        */
        virtual void detach(SpaceObj * obj, const PosNode * op);

        /** 使用指定摄像机绘制空间
        @return 是否有实际绘制效果,没有效果则过滤
        @version NIIEngine 3.0.0
        */
        virtual bool render(SpaceObj * obj, const Camera * view);

        /** 查询实际影响空间对象的灯光
        @version NIIEngine 3.0.0
        */
        virtual void query(const SpaceObj * obj, LightList & out);
    };
}

#endif