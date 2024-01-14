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

#ifndef _NII_OverlayGeoFactory_H_
#define _NII_OverlayGeoFactory_H_

#include "NiiPreInclude.h"
#include "NiiOverlay.h"
#include "NiiContainerOverlayView.h"
#include "NiiFrameOverlayView.h"
#include "NiiTextOverlayGeo.h"

namespace NII
{
    /** 定义一个接口，所有组件希望提供OverlayGeo子类必须实现.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI OverlayGeoFactory : public OverlayAlloc
    {
    public:
        virtual ~OverlayGeoFactory() {}

        /** 创建实例
        @version NIIEngine 3.0.0
        */
        virtual OverlayGeo * createGeo(OverlayGeoID id) = 0;

        /** 删除实例
        @version NIIEngine 3.0.0
        */
        virtual void destroyGeo(OverlayGeo * geo);

        /** 获取实例类型
        @version NIIEngine 3.0.0
        */
        virtual OverlayGeoType getTypeID() const = 0;
    };

    /// 工厂类
    class _EngineAPI RectOverlayGeoFactory: public OverlayGeoFactory
    {
    public:
        /// @copydetails OverlayGeoFactory::createGeo
        OverlayGeo * createGeo(OverlayGeoID id);

        /// @copydetails OverlayGeoFactory::getTypeID
        OverlayGeoType getTypeID() const;
    };

    /// 工厂类
    class _EngineAPI ContainerOverlayGeoFactory: public OverlayGeoFactory
    {
    public:
        /// @copydetails OverlayGeoFactory::createGeo
        OverlayGeo * createGeo(OverlayGeoID id);

        /// @copydetails OverlayGeoFactory::getTypeID
        OverlayGeoType getTypeID() const;
    };

    /// 工厂类
    class _EngineAPI FrameOverlayGeoFactory: public OverlayGeoFactory
    {
    public:
        /// @copydetails OverlayGeoFactory::createGeo
        OverlayGeo * createGeo(OverlayGeoID id);

        /// @copydetails OverlayGeoFactory::getTypeID
        OverlayGeoType getTypeID() const;
    };

    /// 工厂类
    class _EngineAPI TextOverlayGeoFactory: public OverlayGeoFactory
    {
    public:
        /// @copydetails OverlayGeoFactory::createGeo
        OverlayGeo * createGeo(OverlayGeoID id);

        /// @copydetails OverlayGeoFactory::getTypeID
        OverlayGeoType getTypeID() const;
    };
}
#endif