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

#ifndef _NII_UI_CURSOR_WIDGET_AIDE_H_
#define _NII_UI_CURSOR_WIDGET_AIDE_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetAide.h"

namespace NII
{
namespace UI
{
    /** �α긨������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CursorWidgetAide : public WidgetAide
    {
    public:
        CursorWidgetAide(Widget * dst, LangID lid = N_PrimaryLang);
        ~CursorWidgetAide();

        /** ���ͼƬ����
        @param[in] pb ����
        @version NIIEngine 3.0.0
        */
        void setMouseCursor(PixelBuffer * pb);
        
        /** ��ȡʵ�ʵ����ػ���
        @param _default
        @vesion NIIEngine 3.0.0
        */
		PixelBuffer * getMouseCursor(bool _default = true) const;
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dest);

        /// @copydetails WidgetAide::onCursorEnter
        void onCursorEnter(const CursorEventArgs * arg);
    protected:
		PixelBuffer * mImage;        ///< ���ָ��ͼƬ
    };
}
}
#endif