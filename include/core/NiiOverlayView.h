/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-6-7

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

#ifndef _NII_OverlayView_H_
#define _NII_OverlayView_H_

#include "NiiPreInclude.h"
#include "NiiOverlayGeo.h"

namespace NII
{
    /** 表层面视图
    @version NIIEngine 3.0.0
    */
    class _EngineAPI OverlayView : public OverlayGeo
    {
    public:
        typedef map<OverlayGeoID, OverlayGeo *>::type ChildMap;
    public:
        OverlayView(OverlayGeoID id, ScriptTypeID stid = N_CMD_OverlayView, LangID lid = N_PrimaryLang);
        virtual ~OverlayView();

        /** 添加覆盖层几何
        @version NIIEngine 3.0.0
        */
        void add(OverlayGeo * obj);

        /** 移去覆盖层几何
        @version NIIEngine 3.0.0
        */
        void remove(OverlayGeoID id);

        /** 获取覆盖层几何
        @version NIIEngine 3.0.0
        */
        OverlayGeo * get(OverlayGeoID id);

        /** 获取覆盖层几何列表
        @version NIIEngine 3.0.0
        */
        const ChildMap & getList();

        /** 设置是否检测子对象
        @version NIIEngine 3.0.0
        */
        void setChildPosCheck(bool b);

        /** 获取是否检测子对象
        @version NIIEngine 3.0.0
        */
        bool isChildPosCheck() const;

		using OverlayGeo::init;

        /// @copydetails OverlayGeo::init
        virtual void init();

        /// @copydetails OverlayGeo::_update
        virtual void _update();

        /// @copydetails OverlayGeo::_notifyPos
        virtual void _notifyPos();

        /// @copydetails OverlayGeo::_notifyZOrder
        virtual Nidx _notifyZOrder(Nidx level);

        /// @copydetails OverlayGeo::_notifyViewport
        virtual void _notifyViewport();

        /// @copydetails OverlayGeo::_notifyParent
        virtual void _notifyParent(OverlayView * parent, Overlay * overlay);

        /// @copydetails OverlayGeo::queue
        virtual void queue(RenderQueue * queue);

        /// @copydetails OverlayGeo::getGeo
        virtual OverlayGeo * getGeo(NIIf x, NIIf y) const;

        /// @copydetails OverlayGeo::setTemplate
        virtual void setTemplate(OverlayGeo * obj);

        /// @copydetails OverlayGeo::clone
        virtual OverlayGeo * clone(OverlayGeoID newid);
    protected:
        ChildMap mChildren;
        bool mChildPosCheck;
    };
}
#endif