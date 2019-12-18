/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-7

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

#ifndef _NII_DefaultWindow_H_
#define _NII_DefaultWindow_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWindow.h"

namespace NII
{
namespace UI
{
    /** ����
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