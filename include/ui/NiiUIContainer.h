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

#ifndef _NII_UI_CONTAINER_H_
#define _NII_UI_CONTAINER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace UI
{
    /** UI������Ԫ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Container : public Widget
    {
        friend class UISheet;
        friend class Window;
        friend class Widget;
    public:
        Container(WidgetID wid, FactoryID sid, Container * own, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_Container, LangID lid = N_PrimaryLang);
        virtual ~Container();

        using PropertyCmdObj::get;

        /** ����һ���Ӷ��󲢰������ӵ����������
        @param[in] wid UI��Ԫid
        @param[in] fid UI��Ԫ����
        @param[in] name ��������
        @version NIIEngine 3.0.0
        */
        Widget * create(WidgetID wid, FactoryID fid, const String & name = _T(""));

        /** ɾ�����������Ӷ���
        @param sub �Ӷ���
        @version NIIEngine 3.0.0
        */
        void destroy(Widget * sub);

        /** ɾ����������ָ��·���Ӷ���
        @param path �Ӷ���·��(�������������)
        @version NIIEngine 3.0.0
        */
        void destroy(const String & path);

        /** ���ָ���Ӷ���
        @remark �Զ������Ӷ���ԭ��ϵ
        @param[in] sub �Ӷ���
        @version NIIEngine 3.0.0
        */
        void add(Widget * sub);

        /** ��ȥָ���Ӷ���
        @param[in] sub �Ӷ���
        @version NIIEngine 3.0.0
        */
        void remove(Widget * sub);

        /** ��ȥָ��id���Ӷ���
        @param id �Ӷ���id
        @version NIIEngine 3.0.0
        */
        void remove(WidgetID id);

        /** ��ȥָ��·�����Ӷ���
        @param path �Ӷ���·��(�������������)
        @version NIIEngine 3.0.0
        */
        void remove(const String & path);

        /** ��ȡָ���±����UI��Ԫ����
        @remark ��������Ƚ��ڲ�
        @param index �±�
        @return ָ���±����UI��Ԫ����
        @version NIIEngine 3.0.0
        */
        Widget * get(Nidx index) const;

        /** ��ȡָ��id�Ӽ�����
        @param[in] �����ID
        @param[in] r �ݹ�
        @version NIIEngine 3.0.0
        */
        Widget * get(WidgetID id, bool r) const;

        /** ����ָ��·�����Ӷ���
        @param str �Ӷ�������ֻ���·��
        @param n_path ��һ�����Ƿ�Ϊ·��
        @version NIIEngine 3.0.0
        */
        Widget * get(const String & str, bool nonpath = false) const;

        /** ָ��id�Ӽ������Ƿ����
        @param[in] id �Ӷ���id
        @param[in] r �ݹ�
        @version NIIEngine 3.0.0
        */
        bool isExist(WidgetID id, bool r = false) const;

        /** �Ƿ�����Ӷ���
        @param[in] sub �Ӷ���
        @version NIIEngine 3.0.0
        */
        bool isExist(const Widget * sub) const;

        /** ָ��·���Ӷ����Ƿ����
        @param[in] str Ԫ�����ֻ�·��
        @param[in] n_path ��һ�����Ƿ�Ϊ·��
        @version NIIEngine 3.0.0
        */
        bool isExist(const String & str, bool n_path = false) const;

        /** ���ӵ�Ԫ�Ƶ���ǰ��
        @version NIIEngine 3.0.0
        */
        void moveChildFront(Widget * sub);

        /** ���ӵ�Ԫ�Ƶ������
        @version NIIEngine 3.0.0
        */
        void moveChildBack(Widget * sub);

        /** ��ȡ�Ӷ��������
        @version NIIEngine 3.0.0
        */
        NCount getIndex(Widget * sub) const;

        /** �Ӷ����Ƿ񱻶Խ�
        @version NIIEngine 3.0.0
        */
        bool isChildFocus() const;

        /** �α��Ƿ�����������ڲ�
        @version NIIEngine 3.0.0
        */
        bool isCursorEnter() const;

        /** �Ӷ���ĸ���
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** ��ȡ���������λ���ཻ��UI��Ԫ
        @param pos ָ������λ��
        @return ������λ���ཻ��UI��Ԫ,�������򷵻�NULL(0)
        @version NIIEngine 3.0.0
        */
        Widget * get(const Vector2f & pos) const;

        /** ��ȡָ��λ�õ��Ӷ���
        @param pos λ��
        @param disable �Ƿ���Դ��ڽ���״̬
        @version NIIEngine 3.0.0
        */
        Widget * getTarget(const Vector2f & pos, bool disable = false) const;

        /** ��ȡ����Ӷ���
        @version NIIEngine 3.0.0
        */
        Widget * getActiveChild() const;

        /** �����Ƿ��·��α��¼�
        @version NIIEngine 3.0.0
        */
        void setTransmitCursorEvent(bool b);

        /** ��ȡ�Ƿ��·��α��¼�
        @version NIIEngine 3.0.0
        */
        bool isTransmitCursorEvent() const;

        /** ��ȡ������������
        @version NIIEngine 3.0.0
        */
        virtual const CalcRect & getChildArea() const;

        /** ��ȡ����������
        @version NIIEngine 3.0.0
        */
        virtual const CalcRect & getChildInnerArea() const;

        /** ��������
        @param content
        @version NIIEngine 3.0.0
        */
        const CalcRect & getChildArea(bool content) const;

        /** �������ڵĴ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void setWindow(Window * win);

        /** ��ȡ���ڵĴ���
        @version NIIEngine 3.0.0 �߼�api
        */
        Window * getWindow() const;

        /** �����UIԪ��
        @remark �����UIԪ�صľ���ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void addImpl(Widget * e);

        /** ��ȥ��UIԪ��
        @remark ��ȥ��UIԪ�صľ���ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void removeImpl(Widget * e);

        /** ��ȡ������UIԪ��
        @remark ��ȡ������UIԪ�صľ���ʵ��
        @param[in] str Ԫ�����ֻ�·��
        @param[in] nonpath ��һ�����Ƿ�Ϊ·��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual Widget * getImpl(const String & str, bool nonpath = false) const;

        /// @copydetails Widget::update
        virtual void update(TimeDurMS cost);

        /// @copydetails Widget::layout
        virtual void layout();

        /// @copydetails Widget::isContainer
        virtual bool isContainer() const;
        
        /// @copydetails Widget::setRestorePreFocus
        virtual void setRestorePreFocus(bool set);
    public:
        /** ����Ӷ���ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID AddEvent;

        /** ��ȥ�Ӷ���ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID RemoveEvent;

        /** �Ӷ��ƶ�ʱ����
        @version NIIEngine 3.0.0
        */
        static const EventID ChildMoveEvent;

        /** �α����ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID CursorEnterAreaEvent;

        /** �α���ȥʱ
        @version NIIEngine 3.0.0
        */
        static const EventID CursorLeaveAreaEvent;

        /** �����¼�������
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /// @copydetails Widget::onSize
        virtual void onSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onEnable
        virtual void onEnable(const WidgetEventArgs * arg);

        /// @copydetails Widget::onDisable
        virtual void onDisable(const WidgetEventArgs * arg);

        /// @copydetails Widget::onAlpha
        virtual void onAlpha(const WidgetEventArgs * arg);

        /// @copydetails Widget::onInactivate
        virtual void onInactivate(const ActivationEventArgs * arg);

        /// @copydetails Widget::onStyleAttach
        virtual void onStyleAttach(const StyleEventArgs * arg);

        /// @copydetails Widget::onDestroy
        virtual void onDestroy(const WidgetEventArgs * arg);

        /// @copydetails Widget::onSheet
        virtual void onSheet(const SheetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /** ���һ����UI��Ԫ�������UI��Ԫ����(��)����
        @note ע�ⴥ����ǰ��˳��
        @version NIIEngine 3.0.0
        */
        virtual void onAdd(const WidgetEventArgs * arg);

        /** �����UI��Ԫ��������ȥһ����UI��Ԫ����(��)����
        @note ע�ⴥ����ǰ��˳��
        @version NIIEngine 3.0.0
        */
        virtual void onRemove(const WidgetEventArgs * arg);

        /** �Ӷ��ƶ�ʱ����
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        virtual void onChildMove(const WidgetEventArgs * arg);

        /** �α����󴥷�
        @version NIIEngine 3.0.0
        */
        virtual void onCursorEnterArea(const CursorEventArgs * arg);

        /** �α��Ƴ��󴥷�
        @version NIIEngine 3.0.0
        */
        virtual void onCursorLeaveArea(const CursorEventArgs * arg);
    protected:
        /** ��ʼ����UI��Ԫ
        @remark һ����ָ����Ŀɼ��Ľṹ
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void initChild();

        /** ������UI��Ԫ
        @remark һ����ָ����Ŀɼ��Ľṹ
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void clearChild();

        /** ��ȡ�Ӷ���
        @param[in] pos λ��
        @param[in] f ���Ժ���
        @param[in] disable �Ƿ���Դ��ڽ���״̬
        @version NIIEngine 3.0.0 �߼�api
        */
        Widget * get(const Vector2f & pos, intersect f, bool disable = false) const;

        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::queuePixelImpl
        virtual void queuePixelImpl();

        /// @copydetails Widget::refreshImpl
        virtual void refreshImpl(bool r);

        /// @copydetails Widget::cloneImpl
        virtual void cloneImpl(Widget * dest, bool r) const;

        /// @copydetails Widget::notifyPosChange
        virtual void notifyPosChange();

        /// @copydetails Widget::notifyClipChange
        virtual void notifyClipChange();

        /// @copydetails Widget::writeImpl
        virtual NCount writeImpl(XmlSerializer * dest) const;
    protected:
        bool writeChild(XmlSerializer * dest) const;
    protected:
        Window * mWindow;
        WidgetList mChilds;
        bool mCursorEnter;          ///< �α��Ƿ����
        bool mTransmitCursorEvent;  ///< �·��α��¼�
    };
}
}
#endif