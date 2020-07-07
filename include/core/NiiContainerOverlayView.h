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
        ContainerOverlayView(OverlayGeoID id, ScriptTypeID stid = N_CmdObj_ContainerOverlayView,
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
        virtual bool initCmd(PropertyCmdSet * dest);

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