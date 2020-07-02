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
#ifndef _NII_UI_SCROLLBAR_H_
#define _NII_UI_SCROLLBAR_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiUIWidgetView.h"

namespace NII
{
namespace UI
{
    /** 滚动条
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Scrollbar : public Container
    {
    public:
        Scrollbar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        ~Scrollbar();

        /** 设置总页大小
        @version NIIEngine 3.0.0
        */
        void setDocSize(NIIf size);

        /** 获取总页大小
        @version NIIEngine 3.0.0
        */
        inline NIIf getDocSize() const{ return mDocSize; }

        /** 设置按键步大小
        @version NIIEngine 3.0.0
        */
        void setStepSize(NIIf size);
        
        /** 获取按键步大小
        @version NIIEngine 3.0.0
        */
        inline NIIf getStepSize() const{ return mStepSize; }

        /** 设置页大小
        @version NIIEngine 3.0.0
        */
        void setPageSize(NIIf size);

        /** 获取页大小
        @version NIIEngine 3.0.0
        */
        inline NIIf getPageSize() const{ return mPageSize; }
        
        /** 设置页余大小
        @version NIIEngine 3.0.0
        */
        void setOverlapSize(NIIf size);
        
        /** 获取页余大小
        @version NIIEngine 3.0.0
        */
        inline NIIf getOverlapSize() const { return mPageRemian; }

        /** 设置当前位置
        @param[in] pos [0 <= pos <= doc - page]
        @version NIIEngine 3.0.0
        */
        void setCurrent(NIIf pos);
        
        /** 获取当前位置
        @param[in] pos [0 <= pos <= doc - page]
        @version NIIEngine 3.0.0
        */
        inline NIIf getCurrent() const{ return mCurrentPos;}

        /** 设置单位化当前位置
        @param[in] upos [0, 1]
        @version NIIEngine 3.0.0
        */
        void setCurrentUnit(NIIf upos);        
        
        /** 获取单位化当前位置
        @param[in] upos [0, 1]
        @version NIIEngine 3.0.0
        */
        NIIf getCurrentUnit() const;

        /** 减向步进
        @version NIIEngine 3.0.0
        */
        void negStep();
        
        /** 加向步进
        @version NIIEngine 3.0.0
        */
        void posStep();

        /** 减向页进
        @version NIIEngine 3.0.0
        */
        void negPage();
        
        /** 加向页进
        @version NIIEngine 3.0.0
        */
        void posPage();
        
        /** 获取位置指标
        @version NIIEngine 3.0.0
        */
        inline PosButton * getThumb() const { return mThumb; }

        /** 获取减向按纽
        @version NIIEngine 3.0.0
        */
        inline PushButton * getNegative() const{ return mNegative; }
        
        /** 获取加向按纽
        @version NIIEngine 3.0.0
        */
        inline PushButton * getPositive() const { return mPositive;}

        /** 设置是否自动末尾
        @version NIIEngine 3.0.0
        */
        inline void setAutoEndPos(bool b){ mAutoEnd = b;}

        /** 获取是否自动末尾
        @version NIIEngine 3.0.0
        */
        inline bool isAutoEndPos() const{ return mAutoEnd; }
        
        /** 综合设置
        @version NIIEngine 3.0.0
        */
        void setConfig(NIIf doc, NIIf page, NIIf step, NIIf pageremain, NIIf pos);
    protected:
        /// @copydetails Container::initChild
        void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** 设置当前位置
        @version NIIEngine 3.0.0
         */
        bool setCurrentImpl(NIIf pos);

        /** 获取最大位置
        @version NIIEngine 3.0.0
        */
        NIIf getCurrentMax() const;
        
        /** 是否处于末尾位置
        @version NIIEngine 3.0.0
        */
        bool isCurrentEnd() const;

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void ThumbMoveMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void PositiveMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void NegativeMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void ThumbBeginMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void ThumbEndMFunc(const EventArgs * e);

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * view) const;

        /** 当前位置变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCurrentChange(const WidgetEventArgs * arg);

        /** 位置指标开始移动时触发
        @version NIIEngine 3.0.0
        */
        virtual void onTumbMoveBegin(const WidgetEventArgs * arg);

        /** 位置指标结束移动时触发
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveEnd(const WidgetEventArgs * arg);

        /** 配置发生变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onConfigChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::disableWrite
        void disableWrite();
    public:
        /** 当前位置变化时
        @version NIIEngine 3.0.0
        */
        static const EventID CurrentChageEvent;

        /** 位置指标开始移动时
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveBeginEvent;

        /** 位置指标结束移动时
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveEndEvent;

        /** 配置发生变化时
        @version NIIEngine 3.0.0
        */
        static const EventID ConfigChangeEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        PosButton * mThumb;
        PushButton * mNegative;
        PushButton * mPositive;
        NIIf mDocSize;
        NIIf mPageSize;
        NIIf mStepSize;
        NIIf mPageRemian;
        NIIf mCurrentPos;
        bool mAutoEnd;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScrollbarView : public WidgetView
    {
        friend class Scrollbar;
    public:
        ScrollbarView(WidgetModelID wsid);

        bool isVertical() const;

        void setVertical(bool b);

        virtual NIIf getThumbValue() const;

        virtual NIIf getDirection(const Vector2f & pt) const;

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails WidgetView::layout
        void layout();
    protected:
        virtual void updateThumb();
    protected:
        bool mYDirection;
    };
}
}
#endif