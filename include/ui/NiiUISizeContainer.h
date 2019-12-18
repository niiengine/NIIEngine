/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/
#ifndef _NII_UI_SIZEContainer_H_
#define _NII_UI_SIZEContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** ��С����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SizeContainer : public Container
    {
    public:
        SizeContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        ~SizeContainer();

        /** �����Զ���С
        @version NIIEngine 3.0.0
        */
        void setAutoSize(bool b);

        /** �Ƿ��Զ���С
        @version NIIEngine 3.0.0
        */
        bool isAutoSize() const;

        /** �������������С
        @version NIIEngine 3.0.0
        */
        void setContentArea(const Rectf & area);

        /** ��ȡ���������С
        @version NIIEngine 3.0.0
        */
        const Rectf & getContentArea() const;

        /** ��ȡ�Զ����������С
        @version NIIEngine 3.0.0
        */
        Rectf getAutoContentArea() const;

        /// @copydetails Widget::getChildInnerArea
        virtual const CalcRect & getChildInnerArea() const;

        /// @copydetails Widget::getChildArea
        virtual const CalcRect & getChildArea() const;

        /// @copydetails Widget::notifyPosChange
        virtual void notifyPosChange();
    protected:
        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /** ��ȡ�ͻ�����ʵ��
        @version NIIEngine 3.0.0
        */
        Rectf getClientAreaImpl(bool pixelAlign) const;

        /** �Ӷ����С����
        @version NIIEngine 3.0.0
        */
        void ChildSizeMFunc(const EventArgs * e);

        /** �Ӷ���λ�ú���
        @version NIIEngine 3.0.0
        */
		void ChildMoveMFunc(const EventArgs * e);

        /// @copydetails Widget::queuePixelImpl
        void queuePixelImpl();

        /// @copydetails Widget::getClipInnerAreaImpl
        Rectf getClipInnerAreaImpl() const;

        /// @copydetails Widget::getInnerAreaImpl
        virtual Rectf getInnerAreaImpl(bool pixelAlign) const;
        
        /// @copydetails Widget::setAreaImpl
        void setAreaImpl(const RelVector2 & pos, const RelPlaneSize & size, bool event = true);

        /// @copydetails Widget::getIntersectAreaImpl
        Rectf getIntersectAreaImpl() const;

        /// @copydetails Container::onAdd
        void onAdd(const WidgetEventArgs * arg);

        /// @copydetails Container::onRemove
        void onRemove(const WidgetEventArgs * arg);

        /// @copydetails Widget::onParentSize
        void onParentSize(const WidgetEventArgs * arg);
        
        /** ���Զ���С���øı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onAutoSizeChange(const WidgetEventArgs * arg);
        
        /** ����������ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onContentAreaChange(const WidgetEventArgs * arg);
    public:
        /** �����Զ���С�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID AutoSizeChangeEvent;
        
        /** ��������ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ContentAreaChangeEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        typedef multimap<Widget *, SignalPtr>::type SignalList;
    protected:
        Rectf mChildArea;
        CalcRect mClientArea;
        SignalList mSignalList;
        bool mAutoSize;        
    };
}
}
#endif