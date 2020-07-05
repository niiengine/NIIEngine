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

#ifndef _NII_UI_FRAMEWINDOW_H_
#define _NII_UI_FRAMEWINDOW_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWindow.h"
#include "NiiUIWidgetView.h"

namespace NII
{
namespace UI
{
    /** �߿���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameWindow : public Window
    {
    public:
        /** ������С����
        @version NIIEngine 3.0.0
        */
        enum ResizeType
        {
            RT_Top,
            RT_TopLeft,
            RT_Left,
            RT_TopRight,
            RT_Bottom,
            RT_BottomLeft,
            RT_Right,
            RT_BottomRight,
            RT_None
        };
    public:
        FrameWindow(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet = 0, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~FrameWindow();

        /** �л���С��
        @version NIIEngine 3.0.0
        */
        void rollup();

        /** ��С��
        @version NIIEngine 3.0.0
        */
        void setRollUp(bool b);

        /** ��С��
        @version NIIEngine 3.0.0
        */
        bool isRollUp() const;

        /** �������ñ�����
        @version NIIEngine 3.0.0
        */
        void setTitleBarEnable(bool b);

        /** �Ƿ����ñ�����
        @version NIIEngine 3.0.0
        */
        bool isTitleBarEnable() const;

        /** ���ñ������ı�
        @version NIIEngine 3.0.0
        */
        void setTitleBarText(const String & str);
        
        /** ��ȡ�������ı�
        @versioin NIIEngine 3.0.0
        */
        const String & getTitleBarText() const;
        
        /** �����Ϸű߿������С
        @version NIIEngine 3.0.0
        */
        void setDragResize(bool b);

        /** �Ƿ��Ϸű߿������С
        @version NIIEngine 3.0.0
        */
        bool isDragResize() const;

        /** ���������߿�
        @version NIIEngine 3.0.0
        */
        void setFrameEnable(bool b);

        /** �Ƿ������߿�
        @version NIIEngine 3.0.0
        */
        bool isFrameEnable() const;

        /** ���������رհ�Ŧ
        @version NIIEngine 3.0.0
        */
        void setCloseButtonEnable(bool b);

        /** �Ƿ������رհ�Ŧ
        @version NIIEngine 3.0.0
        */
        bool isCloseButtonEnable() const;

        /** ������ק���������ƶ�
        @version NIIEngine 3.0.0
        */
        void setDragMove(bool b);

        /** �Ƿ���ק���������ƶ�
        @version NIIEngine 3.0.0
        */
        bool isDragMove() const;

        /** ����������С��
        @version NIIEngine 3.0.0
        */
        void setRollUpEnable(bool b);

        /** �Ƿ�������С��
        @version NIIEngine 3.0.0
        */
        bool isRollUpEnable() const;

        /** ���õ����߿�Ĵ�С
        @version NIIEngine 3.0.0
        */
        void setBorderSize(NIIf size);

        /** ��ȡ�����߿�Ĵ�С
        @version NIIEngine 3.0.0
        */
        NIIf getBorderSize() const;

        /** �����Ϸ�ͼ��
        @version NIIEngine 3.0.0
        */
        void setCursor(ResizeType type, PixelBuffer * pb);

        /** ��ȡ�Ϸ�ͼ��
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getCursor(ResizeType type) const;

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        Titlebar * getTitlebar() const;

        /** ��ȡ�رհ�Ŧ
        @version NIIEngine 3.0.0
        */
        PushButton * getCloseButton() const;

        /// @copydetails Widget::intersects
        bool intersects(const Vector2f & pos, bool disable) const;
    public:
        /** ��С���л��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID RollUpEvent;

        /** �ϷŴ�С��ʼ�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID DragResizeBeginEvent;

        /** �ϷŴ�С�����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID DragResizeEndEvent;

        /** �رհ�Ŧ�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CloseButtonEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /// @copydetails Container::initChild
        virtual void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** ת���α�
        @version NIIEngine 3.0.0 api
        */
        void alterCursor(const Vector2f & cursorpos) const;

        /** ��ȡ������С����
        @version NIIEngine 3.0.0 api
        */
        ResizeType getBorderResizeType(const Vector2f & pt) const;

        /** ����������С
        @version NIIEngine 3.0.0
        */
        bool resizeLeft(NIIf delta, RelRectf & out_area);

        /** ���Ҳ������С
        @version NIIEngine 3.0.0
        */
        bool resizeRight(NIIf delta, RelRectf & out_area);

        /** ���ϲ������С
        @version NIIEngine 3.0.0
        */
        bool resizeTop(NIIf delta, RelRectf & out_area);

        /** ���²������С
        @version NIIEngine 3.0.0
        */
        bool resizeBottom(NIIf delta, RelRectf & out_area);
    protected:
        void CloseButtonMFunc(const EventArgs * arg);

        /** ����С���л�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onRollUp(const WidgetEventArgs * arg);

        /** �رհ�Ŧ����ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCloseButton(const WidgetEventArgs * arg);

        /** �ϷŴ�С��ʼʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDragResizeBegin(const WidgetEventArgs * arg);

        /** �ϷŴ�С����ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDragResizeEnd(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);

        /// @copydetails Widget::onActivate
        virtual void onActivate(const ActivationEventArgs * e);

        /// @copydetails Widget::onInactivate
        virtual void onInactivate(const ActivationEventArgs * e);
    protected:
        String mText;
        Titlebar * mTitlebar;
        PushButton * mPushButton;
        PixelBuffer * mResizeCursor[4];
        Vector2f mDragPos;
        NIIf mBorderSize;
        
        bool mRollUp;
        bool mSizing;
        bool mDragTitleBarMove;
        bool mFrameEnable;
        bool mRollUpEnable;
        bool mResizeEnable;
    };

    /**
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI FrameWindowView : public WidgetView
    {
    public:
        FrameWindowView(WidgetViewlID type);

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails WidgetView::getArea
        Rectf getArea() const;
    public:
        static const StateID ActiveTitleFrameState;
        static const StateID InactiveTitleFrameState;
        static const StateID DisableTitleFrameState;
        static const StateID ActiveTitleState;
        static const StateID InactiveTitleState;
        static const StateID DisableTitleState;
        static const StateID ActiveFrameState;
        static const StateID InactiveFrameState;
        static const StateID DisableFrameState;
        static const StateID ActiveState;
        static const StateID InactiveState;
        static const StateID StateCount;
    };
}
}
#endif