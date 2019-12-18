/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-8

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

#ifndef _NII_ContainerOverlayGeo_H_
#define _NII_ContainerOverlayGeo_H_

#include "NiiPreInclude.h"
#include "NiiOverlayView.h"

namespace NII
{
    /** 网格覆盖层几何
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ContainerOverlayView : public OverlayView
    {
    public:
        ContainerOverlayView(OverlayGeoID id, ScriptTypeID stid = N_CMD_ContainerOverlayView,
			LangID lid = N_PrimaryLang);
        virtual ~ContainerOverlayView();

        /** 设置是否绘制自身
        @version NIIEngine 3.0.0
        */
        void setSelfView(bool b);

        /** 获取是否绘制自身
        @version NIIEngine 3.0.0
        */
        bool isSelfView() const;

        /** 设置这个面板的纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 返回这个面板的纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /// @copydetails OverlayGeo::init
        virtual void init();

        /// @copydetails OverlayGeo::getTypeID
        virtual OverlayGeoType getTypeID() const;

        /// @copydetails GeometryObj::getGeometry
        void getGeometry(GeometryRaw & op) const;

        /// @copydetails OverlayView::update
        void queue(RenderQueue * queue);
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dest);

        /// @copydetails OverlayGeo::updateGeoPos
        virtual void updateGeoPos();

        /// @copydetails OverlayGeo::updateGeoTex
        virtual void updateGeoTex();
    protected:
        GeometryRaw mGeo;
        NCount mTexCoorCount;
        NIIf mU1;
        NIIf mV1;
        NIIf mU2;
        NIIf mV2;
        bool mSelfView;
    };
}
#endif