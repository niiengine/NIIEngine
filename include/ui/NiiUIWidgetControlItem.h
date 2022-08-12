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
#include "NiiKeyBoardControlPattern.h"
#include "NiiMouseControlPattern.h"
#include "NiiJoyPadControlPattern.h"
#include "NiiTouchControlPattern.h"

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
    class _EngineAPI MouseControlItem : public NII_MEDIA::MouseControlItem
    {
    public:
        MouseControlItem();
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
    protected:
        /// @copydetails MouseControlItem::onPress
        virtual void onPress(const MouseArgs * args);

        /// @copydetails MouseControlItem::onRelease
        virtual void onRelease(const MouseArgs * args);

        /// @copydetails MouseControlItem::onMove
        virtual void onMove(const MouseArgs * args);
    private:
        /**
        @brief Fires off a repeated mouse button down event for this window.
        */
        void rise(MouseButton button);
    protected:
        Widget * mDest;             ///< ��ǰ��Ӧ��Ŀ��
        MouseButton mRepeatButton;
        TimeDurMS mRepeatElapse;
        bool mRepeat;
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