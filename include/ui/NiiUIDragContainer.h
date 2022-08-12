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

#ifndef _NII_UI_DRAG_CONTAINER_H_
#define _NII_UI_DRAG_CONTAINER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** 可拖放的容器控件
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI DragContainer : public Container
    {
    public:
        DragContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        virtual ~DragContainer();

        /** 设置拖放时的透明度
        @version NIIEngine 3.0.0
        */
        void setDragAlpha(NIIf a);

        /** 获取拖放时的透明度
        @version NIIEngine 3.0.0
        */
        NIIf getDragAlpha() const               { return mDragAlpha; }

        /** 设置拖放位置阀值
        @version NIIEngine 3.0.0
        */
        void setDragThreshold(NIIf t);

        /** 获取拖放位置阀值
        @version NIIEngine 3.0.0
        */
        NIIf getDragThreshold() const           { return mDragThreshold;  }

        /** 设置是否启用黏贴模式
        @note 一般指按一下拖拽按钮就能黏住,不需要一直按住
        @version NIIEngine 3.0.0
        */
        void setStickyMode(bool b)              { mStickyMode = b;  }

        /** 获取是否启用黏贴模式
        @note 一般指按一下拖拽按钮就能黏住,不需要一直按住
        @version NIIEngine 3.0.0
        */
        bool isStickyMode() const               { return mStickyMode; }

        /** 设置拖放时的游标
        @version NIIEngine 3.0.0
        */
        void setDragCursor(PixelBuffer * pb);

        /** 获取拖放时的游标
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getDragCursor() const;

        /** 设置是否拖放
        @version NIIEngine 3.0.0
        */
        void setDragEnable(bool b);

        /** 当前是否启用拖放
        @version NIIEngine 3.0.0
        */
        bool isDragEnable() const               { return mDragEnable; }

        /** 是否开始拖放
        @version NIIEngine 3.0.0
        */
        bool isDragging() const                 { return mDrag; }

        /** 外部控制
        @version NIIEngine 3.0.0
        */
        bool pickUp(const Vector2f & pos);

        /// @copydetails Widget::getContextImpl
        void getContextImpl(SheetContext & ctx) const;
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID DragEnableEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID DragAlphaEvent;
        
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID DragBeginEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID DragPosEvent;
        
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID DragEndEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID DragCursorEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID DragThresholdEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID DragDropWindowEvent;

        static const EventID EventCount;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** 是否达到拖拽条件
        @version NIIEngine 3.0.0
        */
        bool isReachDrag(const Vector2f & local_mouse);

        /** 通知进入拖拽模式
        @version NIIEngine 3.0.0
        */
        void _notifyDrag();

        /** 拖拽对象
        @version NIIEngine 3.0.0
        */
        void dragging(const Vector2f & start);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);

        /// @copydetails Widget::onAlpha
        virtual void onAlpha(const WidgetEventArgs * arg);

        /// @copydetails Widget::onClipMode
        virtual void onClipMode(const WidgetEventArgs * arg);

        /// @copydetails Widget::onMove
        virtual void onMove(const WidgetEventArgs * arg);

        /** 容器被拖拽开始后触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragBegin(const WidgetEventArgs * arg);

        /** 容器被拖拽结束后触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragEnd(const WidgetEventArgs * arg);

        /** 容器被拖拽位置变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragPosChange(const WidgetEventArgs * arg);

        /** 容器拖拽设置发生变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragEnableChange(const WidgetEventArgs * arg);

        /** 容器拖拽透明度发生变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragAlphaChange(const WidgetEventArgs * arg);

        /** 容器拖拽游标设置变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragCursorChange(const WidgetEventArgs * arg);

        /** 容器拖拽游离阀值变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragThresholdChange(const WidgetEventArgs * arg);

        /** 容器拖拽项变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDragDropWindowChange(const DragDropEventArgs * arg);
    protected:
        NIIf mPreAlpha;
        NIIf mDragAlpha;
        NIIf mDragThreshold;
        DragDropWindow * mTarget;
        PixelBuffer * mDragCursor;
        RelVector2f mDragBeginPos;
        RelVector2f mBeginPos;
        bool mPreClip;
        bool mDragEnable;
        bool mDragButtomDown;
        bool mDrag;
        bool mStickyMode;
        bool mPick;
    };
}
}
#endif