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

#ifndef _NII_RectOverlayGeo_H_
#define _NII_RectOverlayGeo_H_

#include "NiiPreInclude.h"
#include "NiiOverlay.h"

namespace NII
{
    /** 网格覆盖层几何
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RectOverlayGeo : public OverlayGeo
    {
    public:
        RectOverlayGeo(OverlayGeoID id, LangID lid = N_PrimaryLang);
        virtual ~RectOverlayGeo();

        /** 设置纹理重复
        @param[in] texidx 纹理层
        @param[in] x 重复次数
        @param[in] y 重复次数
        @version NIIEngine 3.0.0
        */
        void setTexRepeat(Nui16 texidx, NIIf x, NIIf y);

        /** x方向重复次数
        @param[in] texidx 纹理层
        @version NIIEngine 3.0.0
        */
        NIIf getTexRepeatX(Nui16 texidx = 0) const;

        /** y方向重复次数
        @param[in] texidx 纹理层
        @version NIIEngine 3.0.0
        */
        NIIf getTexRepeatY(Nui16 texidx = 0) const;

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
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetails OverlayGeo::updateGeoPos
        virtual void updateGeoPos();

        /// @copydetails OverlayGeo::updateGeoTex
        virtual void updateGeoTex();
    protected:
        GeometryRaw mGeo;
        NIIf mTexRepeatX[NII_MAX_TextureLayer];
        NIIf mTexRepeatY[NII_MAX_TextureLayer];
        NIIf mU1;
        NIIf mV1;
        NIIf mU2;
        NIIf mV2;
        NCount mTexCoorCount;
    };
}
#endif