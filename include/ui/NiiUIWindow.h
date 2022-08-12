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

#ifndef _NII_UI_WINDOW_H_
#define _NII_UI_WINDOW_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiUISheet.h"

namespace NII
{
namespace UI
{
    /** ���弶UI��Ԫ
    @remark ֻ�д��弶UI��Ԫ���ܱ�Sheet����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Window : public Container
    {
        friend class UISheet;
    public:
        Window(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet = 0, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_Window, LangID lid = N_PrimaryLang);
        virtual ~Window();

        /** ������ת
        @version NIIEngine 3.0.0
        */
        void setRotation(const Quaternion & q);

        /** ��ȡ��ת
        @version NIIEngine 3.0.0
        */
        const Quaternion & getRotation() const          { return mRot; }

        /** �����Ƿ���Ҫ�ȴ�����
        @remark
            �ȴ�����ָ���Ǵ�UI������ʾ��ʼ��UI������ʾ���������ʱ��
        @version NIIEngine 3.0.0
        */
        void setModal(bool b);

        /** �����Ƿ���Ҫ�ȴ�����
        @remark
            �ȴ�����ָ���Ǵ�UI������ʾ��ʼ��UI������ʾ���������ʱ��
        @version NIIEngine 3.0.0
        */
        bool isModal() const;

        /** ��������¼��Ƿ񴩹��������������䱳�����
        @param b �ǻ��
        */
        inline void setOvergo(bool b)                   { mOvergo = b;}

        /** ��������¼��Ƿ񴩹��������������䱳�����
        @return �ǻ��
        @version NIIEngine 3.0.0
        */
        inline bool isOvergo() const                    { return mOvergo;}

        /** �����Զ�������Ⱦ��
        @version NIIEngine 3.0.0
        */
        void setAutoRender(bool b);

        /** �Ƿ��Զ�������Ⱦ��
        @version NIIEngine 3.0.0 �߼�api
        */
        bool isAutoRender() const                       { return mAutoCtx; }

        /** ���ö�����Ⱦ��
        @version NIIEngine 3.0.0 �߼�api
        */
        void setRender(UISubSheet * o);

        /** ��ȡ������Ⱦ��
        @version NIIEngine 3.0.0 �߼�api
        */
        inline UISubSheet * getRender() const           { return mRender;}

        /** ����UI����
        @version NIIEngine 3.0.0 �߼�api
        */
        void setSheet(UISheet * obj);

        /** ��ȡUI����
        @version NIIEngine 3.0.0 �߼�api
        */
        inline UISheet * getSheet() const               { return mSheet;}

        /** ��ȡһ��ͶӰ���λ���ڻ����е�ʵ��λ��
        @param[in] pos ͶӰ��(�ɼ����)��λ��
        @return �ڲ�����ʵ�ʵ�λ��
        @version NIIEngine 3.0.0
        */
        Vector2f affine(const Vector2f & pos) const;

        /// @copydetails Widget::isPos
        bool isPos(const Vector2f & pos, bool disable) const;

        /// @copydetails Widget::activate
        void activate();
    protected:
        /** ��תʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onRotate(const WidgetEventArgs * arg);

        /// @copydetails Widget::onSize
        virtual void onSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onDestroy
        virtual void onDestroy(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onRemove
        virtual void onRemove(const WidgetEventArgs * arg);

        /// @copydetails Container::onChildMove
        virtual void onChildMove(const WidgetEventArgs * arg);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::getClipAreaImpl
        virtual Rectf getClipAreaImpl() const;

        /// @copydetails Widget::getClipInnerAreaImpl
        virtual Rectf getClipInnerAreaImpl() const;

        /// @copydetails Widget::updateImpl
        virtual void updateImpl(TimeDurMS t);

        /// @copydetails Widget::updateImpl
        virtual void updateImpl(SheetContext * rc);

        /// @copydetails Widget::getContextImpl
        virtual void getContextImpl(SheetContext & ctx) const;

        /// @copydetails Widget::refreshFaceImpl
        virtual void refreshFaceImpl();

        /// @copydetails Widget::queuePixelImpl
        virtual void queuePixelImpl();

        /** ������Ⱦ��
        @version NIIEngine 3.0.0
        */
        void allocRenderCtx();

        /** ɾ����Ⱦ��
        @version NIIEngine 3.0.0
        */
        void freeRenderCtx();
    protected:
        UISubSheet * mRender;
        bool mAutoCtx;
        bool mOvergo;           ///< ������¼���͸�������
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DefaultWindow : public Window
    {
    public:
        DefaultWindow(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet = 0, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~DefaultWindow();
    protected:
        /// @copydetails Window::moveFrontImpl
        bool moveFrontImpl(bool click);
    protected:
        /// @copydetails Widget::onCursorMove
        void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorTouch
        void onCursorTouch(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMultiTouch
        void onCursorMultiTouch(const CursorEventArgs * arg);
    };

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
        bool isRollUp() const                   { return mRollUp; }

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
        const String & getTitleBarText() const  { return mText; }
        
        /** �����Ϸű߿������С
        @version NIIEngine 3.0.0
        */
        void setDragResize(bool b)              { mResizeEnable = b; }

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
        bool isFrameEnable() const              { return mFrameEnable; }

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
        bool isDragMove() const         { return mDragTitleBarMove; }

        /** ����������С��
        @version NIIEngine 3.0.0
        */
        void setRollUpEnable(bool b);

        /** �Ƿ�������С��
        @version NIIEngine 3.0.0
        */
        bool isRollUpEnable() const     { return mRollUpEnable; }

        /** ���õ����߿�Ĵ�С
        @version NIIEngine 3.0.0
        */
        void setBorderSize(NIIf size)   { mBorderSize = size; }

        /** ��ȡ�����߿�Ĵ�С
        @version NIIEngine 3.0.0
        */
        NIIf getBorderSize() const      { return mBorderSize; }

        /** �����Ϸ�ͼ��
        @version NIIEngine 3.0.0
        */
        void setCursor(ResizeType type, PixelBuffer * pb);

        /** ��ȡ�Ϸ�ͼ��
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getCursor(ResizeType type) const;

        /** ���ñ�����
        @version NIIEngine 3.0.0
        */
        void setTitlebarView(StyleViewID id)    { mTitlebarView = id; }

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        StyleViewID getTitlebarView() const     { return mTitlebarView; }

        /** ��ȡ�رհ�Ŧ
        @version NIIEngine 3.0.0
        */
        void setCloseButtonView(StyleViewID id) { mCloseButtonView = id; }

        /** ���ùرհ�Ŧ
        @version NIIEngine 3.0.0
        */
        StyleViewID getCloseButtonView() const  { return mCloseButtonView; }

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        Titlebar * getTitlebar() const          { return mTitlebar; }

        /** ��ȡ�رհ�Ŧ
        @version NIIEngine 3.0.0
        */
        PushButton * getCloseButton() const     { return mPushButton; }

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
        StyleViewID mTitlebarView;
        StyleViewID mCloseButtonView;
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