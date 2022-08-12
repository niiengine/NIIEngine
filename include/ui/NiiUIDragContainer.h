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
    /** ���Ϸŵ������ؼ�
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI DragContainer : public Container
    {
    public:
        DragContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        virtual ~DragContainer();

        /** �����Ϸ�ʱ��͸����
        @version NIIEngine 3.0.0
        */
        void setDragAlpha(NIIf a);

        /** ��ȡ�Ϸ�ʱ��͸����
        @version NIIEngine 3.0.0
        */
        NIIf getDragAlpha() const               { return mDragAlpha; }

        /** �����Ϸ�λ�÷�ֵ
        @version NIIEngine 3.0.0
        */
        void setDragThreshold(NIIf t);

        /** ��ȡ�Ϸ�λ�÷�ֵ
        @version NIIEngine 3.0.0
        */
        NIIf getDragThreshold() const           { return mDragThreshold;  }

        /** �����Ƿ��������ģʽ
        @note һ��ָ��һ����ק��ť�����ס,����Ҫһֱ��ס
        @version NIIEngine 3.0.0
        */
        void setStickyMode(bool b)              { mStickyMode = b;  }

        /** ��ȡ�Ƿ��������ģʽ
        @note һ��ָ��һ����ק��ť�����ס,����Ҫһֱ��ס
        @version NIIEngine 3.0.0
        */
        bool isStickyMode() const               { return mStickyMode; }

        /** �����Ϸ�ʱ���α�
        @version NIIEngine 3.0.0
        */
        void setDragCursor(PixelBuffer * pb);

        /** ��ȡ�Ϸ�ʱ���α�
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getDragCursor() const;

        /** �����Ƿ��Ϸ�
        @version NIIEngine 3.0.0
        */
        void setDragEnable(bool b);

        /** ��ǰ�Ƿ������Ϸ�
        @version NIIEngine 3.0.0
        */
        bool isDragEnable() const               { return mDragEnable; }

        /** �Ƿ�ʼ�Ϸ�
        @version NIIEngine 3.0.0
        */
        bool isDragging() const                 { return mDrag; }

        /** �ⲿ����
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

        /** �Ƿ�ﵽ��ק����
        @version NIIEngine 3.0.0
        */
        bool isReachDrag(const Vector2f & local_mouse);

        /** ֪ͨ������קģʽ
        @version NIIEngine 3.0.0
        */
        void _notifyDrag();

        /** ��ק����
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

        /** ��������ק��ʼ�󴥷�
        @version NIIEngine 3.0.0
        */
        virtual void onDragBegin(const WidgetEventArgs * arg);

        /** ��������ק�����󴥷�
        @version NIIEngine 3.0.0
        */
        virtual void onDragEnd(const WidgetEventArgs * arg);

        /** ��������קλ�ñ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDragPosChange(const WidgetEventArgs * arg);

        /** ������ק���÷����仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDragEnableChange(const WidgetEventArgs * arg);

        /** ������ק͸���ȷ����仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDragAlphaChange(const WidgetEventArgs * arg);

        /** ������ק�α����ñ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDragCursorChange(const WidgetEventArgs * arg);

        /** ������ק���뷧ֵ�仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDragThresholdChange(const WidgetEventArgs * arg);

        /** ������ק��仯ʱ����
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