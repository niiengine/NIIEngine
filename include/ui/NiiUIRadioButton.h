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

#ifndef _NII_UI_RADIOBUTTON_H_
#define _NII_UI_RADIOBUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIButton.h"

namespace NII
{
namespace UI
{
    /** ��ѡ��ť
    @remark ��������ͬһȺ���еĽ���һѡ�еİ�ťȺ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RadioButton : public Button
    {
    public:
        RadioButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
			LangID lid = N_PrimaryLang);

        /** ����������Ⱥ��
        @param[in] gid
        @version NIIEngine 3.0.0
        */
        void setGroup(GroupID gid);

        /** ��ȡ������Ⱥ��
        @version NIIEngine 3.0.0
        */
        GroupID getGroup() const;

        /** ��ȡ������Ⱥ��������ѡ�еĵ�ѡ��ť
        @version NIIEngine 3.0.0
        */
        RadioButton * getGroupSelect() const;
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dest);

		// @copydetails Button::onSelectChange
		void onSelectChange(const WidgetEventArgs * arg);
    protected:
        void verify() const;
    protected:
        GroupID mGID;
    };
}
}
#endif