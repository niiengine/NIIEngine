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
#include "NiiUIInstance.h"

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
        const Quaternion & getRotation() const;

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
        inline void setOvergo(bool b) { mOvergo = b;}

        /** ��������¼��Ƿ񴩹��������������䱳�����
        @return �ǻ��
        @version NIIEngine 3.0.0
        */
        inline bool isOvergo() const { return mOvergo;}

        /** �����Զ�������Ⱦ��
        @version NIIEngine 3.0.0
        */
        void setAutoRender(bool b);

        /** �Ƿ��Զ�������Ⱦ��
        @version NIIEngine 3.0.0 �߼�api
        */
        bool isAutoRender() const;

        /** ���ö�����Ⱦ��
        @version NIIEngine 3.0.0 �߼�api
        */
        void setRender(UIInstance * o);

        /** ��ȡ������Ⱦ��
        @version NIIEngine 3.0.0 �߼�api
        */
        inline UIInstance * getRender() const { return mRender;}

        /** ����UI����
        @version NIIEngine 3.0.0 �߼�api
        */
        void setSheet(UISheet * obj);

        /** ��ȡUI����
        @version NIIEngine 3.0.0 �߼�api
        */
        inline UISheet * getSheet() const { return mSheet;}

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
        UIInstance * mRender;
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
}
}
#endif