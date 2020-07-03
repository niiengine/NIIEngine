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

#ifndef _NII_RenderObjectListener_H_
#define _NII_RenderObjectListener_H_

#include "NiiPreInclude.h"
#include "NiiGeometryObj.h"
#include "NiiShaderCh.h"

namespace NII
{
    /** 抽象类接口,类必须实现它如果它们希望当单个对象渲染后从场景管理器接收事件 
    */
    class _EngineAPI RenderObjListener
    {
    public:
        virtual ~RenderObjListener() {}

        /** 当开始渲染单个对象事引发的事件
        @remark
        */
        virtual void notifyRenderSingleObject(GeometryObj * obj, const ShaderCh * ch,
            const SysSyncParam * source, const LightList & lights, bool lockch) = 0;
    };
}
#endif