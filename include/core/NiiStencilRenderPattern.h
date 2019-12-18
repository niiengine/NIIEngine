/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-3-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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
        NCount getIndexSize() const;

        /** 是否调试阴影
        @version NIIEngine 3.0.0
        */
        void setDebugShadow(bool b);

        /** 是否调试阴影
        @version NIIEngine 3.0.0
        */
        bool isDebugShadow() const;
    protected:
        /** 确定能投射阴影的列表
        @version NIIEngine 3.0.0
        */
        const ShadowVolumeGenList & findCastList(const Light * light, const Camera * camera);

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
            ShadowVolumeObj * caster, NIIf extrude, Nmark & mark);

        /** 渲染阴影
        @version NIIEngine 3.0.0
        */
        void renderShadows(const ShadowVolumeList & src, ShaderCh * ch, 
            const LightList * lights, bool secondpass, bool zfail, bool twosided, Nmark mark);
    private:
        StencilShaderData * mShaderData;
    protected:
        IndexBuffer * mIndexBuffer;
        ViewRectGeo * mScreen;
        ShaderCh * mDebugCh;
        ShaderCh * mStencilCh;
        ShaderCh * mModulateCh;
        ShadowVolumeGenList mCasterList;
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