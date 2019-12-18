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

#ifndef _NII_FrameOverlayGeo_H_
#define _NII_FrameOverlayGeo_H_

#include "NiiPreInclude.h"
#include "NiiContainerOverlayView.h"

namespace NII
{
    class FrameBorderGeo;

    /** 边框覆盖层几何
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameOverlayView : public ContainerOverlayView
    {
        friend class FrameBorderGeo;
    public:
        enum CellType
        {
            CT_Top_Left     = 0,
            CT_Top          = 1,
            CT_Top_Right    = 2,
            CT_Left         = 3,
            CT_Centre       = 4,
            CT_Right        = 5,
            CT_Bottom_Left  = 6,
            CT_Bottom       = 7,
            CT_Bottom_Right = 8
        };
    public:
        FrameOverlayView(OverlayGeoID id, LangID lid = N_PrimaryLang);
        virtual ~FrameOverlayView();

        /** 设置边框大小
        @version NIIEngine 3.0.0
        */
        void setBorderSize(NIIf size);

        /** 设置边框大小
        @version NIIEngine 3.0.0
        */
        void setBorderSize(NIIf left, NIIf right, NIIf top, NIIf bottom);

        /** 设置左边框大小
        @version NIIEngine 3.0.0
        */
        void setLeftBorderSize(NIIf size);

        /** 获取左边框大小
        @version NIIEngine 3.0.0
        */
        NIIf getLeftBorderSize() const;

        /** 设置右边框大小
        @version NIIEngine 3.0.0
        */
        void setRightBorderSize(NIIf size);

        /** 获取右边框大小
        @version NIIEngine 3.0.0
        */
        NIIf getRightBorderSize() const;

        /** 设置上边框大小
        @version NIIEngine 3.0.0
        */
        void setTopBorderSize(NIIf size);

        /** 获取上边框大小
        @version NIIEngine 3.0.0
        */
        NIIf getTopBorderSize() const;

        /** 设置下边框大小
        @version NIIEngine 3.0.0
        */
        void setBottomBorderSize(NIIf size);

        /** 获取下边框大小
        @version NIIEngine 3.0.0
        */
        NIIf getBottomBorderSize() const;

        /** 设置中区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setCentreCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 返回中区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getCentreCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /** 设置左区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setLeftCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 获取左区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getLeftCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /** 设置右区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setRightCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 获取右区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getRightCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /** 设置上区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setTopCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 获取上区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getTopCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /** 设置下区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setBottomCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 设置下区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getBottomCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /** 设置左上区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setTopLeftCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 获取左上区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getTopLeftCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /** 设置右上区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setTopRightCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 获取右上区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getTopRightCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /** 设置左下区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setBottomLeftCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 获取左下区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getBottomLeftCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /** 设置右下区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void setBottomRightCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2);

        /** 设置右下区域纹理坐标
        @note 一般情况就是整个纹理(0.0, 0.0, 1.0, 1.0)
        @version NIIEngine 3.0.0
        */
        void getBottomRightCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const;

        /// @copydetails OverlayView::getTypeID
        OverlayGeoType getTypeID() const;

        /// @copydetails OverlayView::init
        virtual void init();

        /// @copydetails OverlayView::queue
        void queue(RenderQueue * queue);

        /// @copydetails OverlayGeo::setSizeMode
        void setSizeMode(SizeMode gmm);

        /// @copydetails OverlayGeo::_update
        void _update(void);
    protected:
        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /// @copydetails OverlayGeo::update
        void updateGeoPos();

        /// @copydetails OverlayGeo::update
        void updateGeoTex();
	protected:
		struct RectUV
		{
			NIIf u1, v1, u2, v2;
		};
    protected:
        FrameBorderGeo * mFrameGeo;
        NIIf mLeftBorderSize;
        NIIf mRightBorderSize;
        NIIf mTopBorderSize;
        NIIf mBottomBorderSize;
        Nui16 mPixelLeftBorderSize;
        Nui16 mPixelRightBorderSize;
        Nui16 mPixelTopBorderSize;
        Nui16 mPixelBottomBorderSize;
        RectUV mBorderUV[8];
    };
}
#endif
