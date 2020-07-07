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
        OverlayView(OverlayGeoID id, ScriptTypeID stid = N_CmdObj_OverlayView, LangID lid = N_PrimaryLang);
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