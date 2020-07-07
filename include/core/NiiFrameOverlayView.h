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
        bool initCmd(PropertyCmdSet * dest);

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
