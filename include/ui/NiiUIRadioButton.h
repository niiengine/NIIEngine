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
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Button::onSelect
        void onSelect(const WidgetEventArgs * arg);
    protected:
        void verify() const;
    protected:
        GroupID mGID;
    };

    /**
    */
    class _EngineAPI RadioButtonView : public ButtonView
    {
    public:
        RadioButtonView(WidgetModelID wsid);

        static const StateID SelectEnableState;
        static const StateID SelectHoverState;
        static const StateID SelectPushState;
        static const StateID SelectPushOffState;
        static const StateID SelectDisableState;
        static const StateID StateCount;
    protected:
        StateID alter(StateID pid) const;
    };
}
}
#endif