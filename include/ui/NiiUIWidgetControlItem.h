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

#ifndef _NII_UI_Widget_CONTROL_ITEM_H_
#define _NII_UI_Widget_CONTROL_ITEM_H_

#include "NiiUIPreInclude.h"
#include "NiiKeyBoardControlItem.h"
#include "NiiMouseControlItem.h"
#include "NiiJoyDevControlItem.h"
#include "NiiTouchControlItem.h"

using namespace NII::NII_MEDIA;
namespace NII
{
namespace UI
{
    /** UI��Ԫר�õļ��̿��Ƶ�Ԫ
    @remark 
        ��UI��ϵ�о����ѻ���ת�����������ʹ��,�ܻ�ȡ����Ĺ���
    */
    class _EngineAPI KeyBoardControlItem : public NII_MEDIA::KeyboardControlItem
    {
    public:
        KeyBoardControlItem();
        virtual ~KeyBoardControlItem();
    };
    
    /** UI��Ԫר�õ������Ƶ�Ԫ
    @remark
        ��UI��ϵ�о����ѻ���ת�����������ʹ��,�ܻ�ȡ����Ĺ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlItem : public NII_MEDIA::MouseControlItem, public PropertyCmdObj
    {
    public:
        MouseControlItem(LangID lid = N_PrimaryLang);
        virtual ~MouseControlItem();

        /** Ӧ�õ���Ԫ
        @version NIIEngine 3.0.0
        */
        void apply(Widget * dst);

        /** ����ʱ��״̬
        @param[in] cost
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** ���������Զ��¼�������Ҫ��ʱ�䷧
        @param delay ��λ��
        @return Nothing.
        */
        void setThreshold(TimeDurMS delay);

        /** �������Զ��¼�������Ҫ��ʱ�䷧
        @return ��λ��
        */
        TimeDurMS getThreshold() const;

        /** �����Զ��¼�������ɺ�,�Զ�������ʱ����
        @param rate ��λ��
        @version NIIEngine 3.0.0
        */
        void setRate(TimeDurMS rate);

        /** �����Զ��¼�������ɺ�,�Զ�������ʱ����
        @return ��λ��
        @version NIIEngine 3.0.0
        */
        TimeDurMS getRate() const;

        /** �����Ƿ��Զ��ظ�������갴���¼�
        @param set true ����;false ����
        */
        void setAutoEvent(bool set);

        /** �Ƿ��Զ��ظ�������갴���¼�
        */
        bool isAutoEvent() const;

        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dst);
    protected:
        /// @copydetails MouseControlItem::onPress
        virtual void onPress(const MousePressArgs * args);

        /// @copydetails MouseControlItem::onRelease
        virtual void onRelease(const MousePressArgs * args);

        /// @copydetails MouseControlItem::onMove
        virtual void onMove(const MousePressArgs * args);
    private:
        /**
        @brief Fires off a repeated mouse button down event for this window.
        */
        void rise(MouseButton button);
    protected:
        Widget * mDest;             ///< ��ǰ��Ӧ��Ŀ��
        TimeDurMS mRate;            ///< �����Զ��¼�������ɺ�,�Զ�������ʱ��
        TimeDurMS mThreshold;       ///< �����Զ��¼�������Ҫ��ʱ�䷧
        MouseButton mRepeatButton;  ///< �����Զ��¼��İ���
        TimeDurMS mRepeatElapse;    ///< ��ǰ��ɵ�ʱ�䷧
        bool mAutoEvent;            ///< �Ƿ��Զ��ظ�������갴���¼�
        bool mRepeat;               ///< �����Զ��¼����
    };
    
    /** UI��Ԫר�õ��ֱ����Ƶ�Ԫ
    @remark
        ��UI��ϵ�о����ѻ���ת�����������ʹ��,�ܻ�ȡ����Ĺ���
    */
    class _EngineAPI JoyPadControlItem : public NII_MEDIA::JoyPadControlItem
    {
    public:
        JoyPadControlItem();
        virtual ~JoyPadControlItem();
    };
    
    /** UI��Ԫר�õĴ������Ƶ�Ԫ
    @remark 
        ��UI��ϵ�о����ѻ���ת�����������ʹ��,�ܻ�ȡ����Ĺ���
    */
    class _EngineAPI TouchControlItem : public NII_MEDIA::TouchControlItem
    {
    public:
        TouchControlItem();
        virtual ~TouchControlItem();
    };
}
}
#endif