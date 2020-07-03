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

#ifndef _NII_SHADOW_RENDER_TEST_H_
#define _NII_SHADOW_RENDER_TEST_H_

#include "NiiPreInclude.h"
#include "NiiRenderFilter.h"

namespace NII
{
    /** 阴影测试器
    @version NIIEngine 3.0.0
    */
    class _EngineInner ShadowRenderTest : public RenderFilter
    {
    public:
        ShadowRenderTest(SpaceManager * gm, RenderPattern * sp);
        ~ShadowRenderTest();

        ///@copydetail RenderFilter::access
        void render(const ShaderCh * ch, GeometryObj * obj);

        ///@copydetail RenderFilter::access
        void render(const ShaderCh * ch, const GeometryObjList & obj);

        /** 设置参数
        @param[in] alphacast 透明物也投射阴影
        @param[in] lightclip 剪裁灯光
        @param[in] chlight 使用通道灯光
        @param[in] lights 灯光列表
        @version NIIEngine 3.0.0
        */
        void setParam(bool alphacast, bool lightclip, bool light, const LightList * lights);
    protected:
        SpaceManager * mScene;
        RenderPattern * mPattern;
        const LightList * mLights;
        const ShaderCh * mCh;
        bool mAlphaShadowCast;
        bool mLightEnable;
        bool mLightClip;
    };
}

#endif