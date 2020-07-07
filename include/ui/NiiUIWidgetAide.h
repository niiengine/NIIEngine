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
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails WidgetAide::onCursorEnter
        void onCursorEnter(const CursorEventArgs * arg);
    protected:
        PixelBuffer * mImage;        ///< ���ָ��ͼƬ
    };
    
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
        inline void setParentTipsText(bool b){ mParentTipsText = b; }

        /** ��ȡ�Ƿ�ʹ�ø�������ʾ�ı�
        @return NIIEngine 3.0.0
        */
        inline bool isParentTooltipText() const{return mParentTipsText;}
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dst);
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
        String mTipsText;
        mutable StyleID mType;  ///< Tooltip����
        Tooltip * mCustomTip;
        bool mGenTip;           ///< �Զ������
        bool mParentTipsText;
    };
}
}
#endif