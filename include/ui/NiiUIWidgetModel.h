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
#ifndef _NII_WIDGET_STYLE_H_
#define _NII_WIDGET_STYLE_H_

#include "NiiUIPreInclude.h"
#include "NiiUICommon.h"

namespace NII
{
namespace UI
{
    /** UI��Ⱦ��ʽ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetModel : public UIAlloc
    {
        friend class Widget;
    public:
        WidgetModel(WidgetModelID wsid, WidgetModelID type);
        virtual ~WidgetModel();

        /** UI��Ⱦ��ʽID
        @version NIIEngine 3.0.0
        */
        WidgetModelID getID() const;

        /** UI��Ⱦ��ʽ����
        @version NIIEngine 3.0.0
        */
        WidgetModelID getType() const;

        /** ����
        @param[in] cost �ϴε��ú���ζȹ����¼�
        @version NIIEngine 3.0.0
        */
        virtual void update(TimeDurMS cost);

        /** ��UI��ˢ��������
        @version NIIEngine 3.0.0
        */
        virtual void flush() = 0;

        /** ִ����ʽ�Ű�
        @version NIIEngine 3.0.0
        */
        virtual void layout();

        /** ��ȡ�Ű�����
        @version NIIEngine 3.0.0
        */
        virtual Rectf getArea() const;

        /** ��ȡʹ�õķ��
        @version NIIEngine 3.0.0
        */
		Style * getStyle() const;
        
        /** ��ȡʵ�ʻ���
        @version NIIEngine 3.0.0
        */
        virtual void getContext(SheetContext *& ctx) const;
    protected:
        /** ��������
        @param pcd ����
        @param serl �Ƿ�����л�
        */
        void add(PropertyCmd * pcd, bool serl = true);

        /** ���ӵ�UI��Ԫʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onAttach();

        /** ��UI��Ԫ�н��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDetach();

        /** ���ӷ��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onStyleAttach();

        /** ������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onStyleDetach();

        /** ����ı�ʱ����
        @param[in] obj
        @verison NIIEngine 3.0.0
        */
        virtual bool notifyFont(const Font * obj);
    public:
        /** enable/���� ��ǩ�е�Ԫ��
        @version NIIEngine 3.0.0
        */
        static const StateID EnableState;

        /** disable/���� ��ǩ�е�Ԫ��
        @version NIIEngine 3.0.0
        */
        static const StateID DisableState;

        /** ״̬����
        @version NIIEngine 3.0.0
        */
        static const StateID StateCount;
    protected:
        WidgetModel & operator=(const WidgetModel &);
    protected:
        typedef vector<std::pair<PropertyCmd *, bool> >::type PropertyCmdList;
    protected:
        Widget * mSrc;
        WidgetModelID mID;
        WidgetModelID mType;
        PropertyCmdList mCmdList;
    };
}
}
#endif