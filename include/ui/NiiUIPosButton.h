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

#ifndef _NII_UI_POSBUTTON_H_
#define _NII_UI_POSBUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIPushButton.h"

namespace NII
{
namespace UI
{
    /** 记录按纽
    @remark 记录位置状态的按纽
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosButton : public PushButton
    {
    public:
        PosButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~PosButton();

        /** 设置是否同步位置事件
        @remark 
        @version NIIEngine 3.0.0
        */
        void setPosEvent(bool b);

        /** 获取是否同步位置事件
        @version NIIEngine 3.0.0
        */
        bool isPosEvent() const;

        /** 设置y方向移动
        @version NIIEngine 3.0.0
        */
        void setYMove(bool b);

        /** 获取y方向移动
        @version NIIEngine 3.0.0
        */
        bool isYMove() const;

        /** 设置x方向移动
        @version NIIEngine 3.0.0
        */
        void setXMove(bool b);

        /** 获取x方向移动
        @version NIIEngine 3.0.0
        */
        bool isXMove() const;

        /** 设置y方向范围
        @version NIIEngine 3.0.0
        */
        void setYRange(NIIf min, NIIf max);

        /** 获取y方向范围
        @version NIIEngine 3.0.0
        */
        void getYRange(NIIf & min, NIIf & max) const;

        /** 设置x方向范围
        @version NIIEngine 3.0.0
        */
        void setXRange(NIIf min, NIIf max);

        /** 获取x方向范围
        @version NIIEngine 3.0.0
        */
        void getXRange(NIIf & min, NIIf & max) const;
    protected:
        /** 这个UI对象开始拖拽时触发
        @remark
            达成拖拽条件一般是指游标在这个UI对象的位置区域里,而且按下触发拖拽的按键
        @version NIIEngine 3.0.0
        */
        virtual void onMoveBegin(const WidgetEventArgs * arg);

        /** 这个UI对象结束拖拽时触发
        @remark
            达成拖拽条件后,又达成了拖拽结束的条件后触发,一般是指释放了游标的按键
        @version NIIEngine 3.0.0
        */
        virtual void onMoveEnd(const WidgetEventArgs * arg);

        /** 这个UI对象的位置发生变化时触发
        @remark
            依赖相应的是否同步位置变化事件的设计,如果同步则实时触发(存在多次),否则
            是以在每次开始拖拽事件到结束拖拽事件后触发一次.
        @version NIIEngine 3.0.0
        */
        virtual void onPosChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::disableWrite
        void disableWrite();
    public:
        /** 移动开始事件
        @version NIIEngine 3.0.0
        */
        static const EventID MoveBeginEvent;

        /** 移动结束事件
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEndEvent;

        /** 位置改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID PosChangeEvent;
        
        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Vector2f mDragPos;
        NIIf mVMin;
        NIIf mVMax;
        NIIf mHMin;
        NIIf mHMax;
        bool mSynEvent;
        bool mDrag;
        bool mYMove;
        bool mXMove;
    };
}
}
#endif