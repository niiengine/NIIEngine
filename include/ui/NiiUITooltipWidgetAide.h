/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-9-8

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

#ifndef _NII_UI_TOOLTIP_WIDGET_AIDE_H_
#define _NII_UI_TOOLTIP_WIDGET_AIDE_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetAide.h"

namespace NII
{
namespace UI
{
    /** UI��ʾ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TooltipWidgetAide : public WidgetAide
    {
    public:
        TooltipWidgetAide(Widget * dst, LangID lid = N_PrimaryLang);
        ~TooltipWidgetAide();

        /** ������ʾ�ı�
        @param[in] txt ��ʾ�ı�
        @version NIIEngine 3.0.0
        */
        void setTooltipText(const String & txt);

        /** ��ȡ��ʾ�ı�
        @version NIIEngine 3.0.0
        */
        const String & getTooltipText() const;

        /** ������ʾ��������
        @param type �Ѿ�ע���Tooltip������
        @version NIIEngine 3.0.0
        */
        void setTooltipType(StyleID type);

        /** ��ȡ��ʾ��������
        @return �Ѿ�ע���Tooltip������
        @version NIIEngine 3.0.0
        */
        StyleID getTooltipType() const;

        /** ������ʾ����
        @param obj Tooltip����
        @version NIIEngine 3.0.0
        */
        void setTooltip(Tooltip * obj);

        /** ��ȡ��ʾ����
        @return Tooltip����
        @version NIIEngine 3.0.0
        */
        Tooltip * getTooltip() const;

        /** �Ƿ�ʹ��Ĭ�ϵ���ʾ����
        @version NIIEngine 3.0.0
        */
        bool isDefault() const;

        /** �����Ƿ�ʹ�ø�������ʾ�ı�
        @version NIIEngine 3.0.0
        */
        void setParentTooltipText(bool b);

        /** ��ȡ�Ƿ�ʹ�ø�������ʾ�ı�
        @return NIIEngine 3.0.0
        */
        bool isParentTooltipText() const;
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dst);
    protected:
        /// @copydetails WidgetAide::onCursorEnter
        void onCursorEnter(const CursorEventArgs * arg);

        /// @copydetails WidgetAide::onCursorLeave
        void onCursorLeave(const CursorEventArgs * arg);

        /// @copydetails WidgetAide::onCursorMove
        void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails WidgetAide::onButtonDown
        void onButtonDown(const CursorEventArgs * arg);
    protected:
        String mTipText;        ///< ��ʾ�ı�
        mutable StyleID mType;  ///< Tooltip����
        Tooltip * mCustomTip;   ///< �Զ�����ʾ�������
        bool mGenTip;           ///< �Զ������
        bool mInheritTipText;   ///< ʹ�ø������ı�
    };
}
}
#endif