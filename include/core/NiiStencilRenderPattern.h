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

#ifndef _NII_STENCIL_RENDERPATTERN_H
#define _NII_STENCIL_RENDERPATTERN_H

#include "NiiPreInclude.h"
#include "NiiRenderPattern.h"

namespace NII
{
    class StencilShaderData;
    /** 蒙板渲染处理器
    @version NIIEngine 3.0.0
    */
    class StencilRenderPattern : public RenderPattern
    {
    public :
        StencilRenderPattern(SpaceManager * sm, RenderSys * rsys, SysSyncParam * param);
        virtual ~StencilRenderPattern();

        /// @copydetails RenderPattern::init
        void init();

        /// @copydetails RenderPattern::isValid
        bool isValid();

        /// @copydetails RenderPattern::createQueue
        RenderQueue * createQueue() const;
        
        /// @copydetails RenderPattern::renderShadow
        virtual void renderShadow(Camera * cam, const Viewport * vp, const LightList & lights);

        /// @copydetails RenderPattern::isStencilShadow
        bool isStencilShadow() const;

        /// @copydetails RenderPattern::setShadowColour
        void setShadowColour(const Colour & c);

        /// @copydetails RenderPattern::sort
        void sortLight(LightList & dest);
    public:
        /** 设置索引数量
        @param[in] size 顶点索引数量
        @version NIIEngine 3.0.0
        */
        void setIndexSize(NCount size);

        /** 获取设置索引数量
        @version NIIEngine 3.0.0
        */
        inline NCount getIndexSize() const      { return mIndexBufferSize; }

        /** 是否调试阴影
        @version NIIEngine 3.0.0
        */
        inline void setDebugShadow(bool b)      { mDebugShadows = b; }

        /** 是否调试阴影
        @version NIIEngine 3.0.0
        */
        inline bool isDebugShadow() const       { return mDebugShadows; }
    protected:
        /** 确定能投射阴影的列表
        @version NIIEngine 3.0.0
        */
        const StencilObjList & findCastList(const Light * light, const Camera * camera);

        /** 摄象机和灯光的近裁剪
        @version NIIEngine 3.0.0
        */
        const PlaneSpace & getNearClip(const Light * light, const Camera * cam) const;

        /** 摄象机和灯光的裁剪容积
        @version NIIEngine 3.0.0
        */
        const PlaneSpaceList & getFrustumClip(const Light * light, const Camera * cam) const;

        /** 为阴影容积设置蒙板状态的内部实用方法.
        @param[in] two 这是第二次
        @param[in] zfail 使用 zfail
        */
        void applyStencil(bool two, bool zfail);

        /** 渲染蒙板
        @param[in] light 灯光资源
        @param[in] cam 正在观看的摄象机
        @param[in] clip 建立裁减状态
        */
        void renderStencil(const Light * light, const Camera * cam, bool clip);

        /** 应用暗罩
        @version NIIEngine 3.0.0
        */
        virtual void applyDarkCap(const Light * light, const Camera * camera,
            ShadowObj * caster, NIIf extrude, Nmark & mark);

        /** 渲染阴影
        @version NIIEngine 3.0.0
        */
        void renderShadows(const ShadowVolumeList & src, ShaderCh * ch, const LightList * lights, 
            bool secondpass, bool zfail, bool twosided, Nmark mark);
    private:
        StencilShaderData * mShaderData;
    protected:
        IndexBuffer * mIndexBuffer;
        RectGeo * mScreen;
        ShaderCh * mDebugCh;
        ShaderCh * mStencilCh;
        ShaderCh * mModulateCh;
        StencilObjList mCasterList;
        mutable PlaneSpace mNearClipSpace;
        mutable PlaneSpaceList mFrustumClipSpace;
        NCount mIndexBufferSize;
        ResourceID mDebugChRID;
        ResourceID mStencilChRID;
        ResourceID mModulationChRID;
        bool mDebugShadows;
        bool mBothSideStencil;
    };
}
#endif