/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-8

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

#ifndef _NII_UI_WIDGET_AIDE_H_
#define _NII_UI_WIDGET_AIDE_H_

#include "NiiUIPreInclude.h"

namespace NII
{
namespace UI
{
    /** UI��Ԫ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetAide : public PropertyCmdObj
    {
    public:
        WidgetAide(Widget * dst, ScriptTypeID stid, LangID lid = N_PrimaryLang) :
			PropertyCmdObj(stid, lid),
			mCurrent(dst){}
        virtual ~WidgetAide() {}
    protected:
        /** ���α����ʱ����
        @remark ����״̬����ʱ����
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onCursorEnter(const CursorEventArgs * arg) {}
        
        /** ���α��뿪ʱ����
        @remark ����״̬����ʱ����
        @remark ����
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onCursorLeave(const CursorEventArgs * arg) {}
        
        /** ���α��ƶ�ʱ����(����״̬��)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onCursorMove(const CursorEventArgs * arg) {}
        
        /** ���α���קʱ����(����״̬)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onCursorDrag(const CursorEventArgs * arg) {}
        
        /** ���α갴���ͷ�ʱ����(����״̬��)
        @remark ����״̬����ʱ����(��һ״̬��Ϊ��������)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onButtonDown(const CursorEventArgs * arg) {}
        
        /** ���α갴������ʱ����(����״̬��)
        @remark ����״̬����ʱ����(��һ״̬��Ϊ�����ͷ�)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onButtonUp(const CursorEventArgs * arg) {}
    protected:
        WidgetAide() : PropertyCmdObj(5001, N_PrimaryLang){}
    protected:
        UISheet * mSheet;
		Widget * mCurrent;
    };
}
}
#endif