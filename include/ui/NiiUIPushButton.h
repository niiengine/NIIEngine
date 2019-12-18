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

#ifndef _NII_UI_PUSH_BUTTON_H_
#define _NII_UI_PUSH_BUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIButton.h"

namespace NII
{
namespace UI
{
    /** �ɰ���ť
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PushButton : public Button
    {
    public:
        PushButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			ScriptTypeID stid = N_CMD_PushButton, LangID lid = N_PrimaryLang);

        virtual ~PushButton();
    public:
        /** �����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PushEvent;
        
        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /** ����ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onPush(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);
    };
}
}
#endif