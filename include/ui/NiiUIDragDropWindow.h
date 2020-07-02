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

#ifndef _NII_UI_DRAG_DROP_WINDOW_H_
#define _NII_UI_DRAG_DROP_WINDOW_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** 可放拖拽容器的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DragDropWindow : public Container
    {
    public:
        DragDropWindow(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~DragDropWindow();

        /** 设置是否启用可拖放
        @version NIIEngine 3.0.0
        */
        void setDragDropEnable(bool b);

        /** 获取是否启用可拖放
        @version NIIEngine 3.0.0
        */
        bool isDragDropEnable() const;

        /** 通知有容器移入
        @version NIIEngine 3.0.0
        */
        void notifyEnter(DragContainer * c);

        /** 通知有容器放入
        @version NIIEngine 3.0.0
        */
        void notifyDrop(DragContainer * c);

        /** 通知有容器移出
        @version NIIEngine 3.0.0
        */
        void notifyLeave(DragContainer * c);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);
    protected:
        /** 有容器移入时触发
        @version NIIEngine 3.0.0
        */
        virtual void onEnter(const DragDropEventArgs * arg);

        /** 有容器放入时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDrop(const DragDropEventArgs * arg);

        /** 有容器移出时触发
        @version NIIEngine 3.0.0
        */
        virtual void onLeave(const DragDropEventArgs * arg);
    public:
        /** 有容器移入事件
        @version NIIEngine 3.0.0
        */
        static const EventID EnterEvent;

        /** 有容器放入事件
        @version NIIEngine 3.0.0
        */
        static const EventID DropEvent;

        /** 有容器移出事件
        @version NIIEngine 3.0.0
        */
        static const EventID LeaveEvent;

        /// 事件总数
        static const EventID EventCount;
    protected:
        bool mDragDropEnable;
    };
}
}
#endif