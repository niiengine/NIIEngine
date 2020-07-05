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
#ifndef _NII_WIDGET_View_H_
#define _NII_WIDGET_View_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetList.h"
#include "NiiColourArea.h"
#include "NiiCommandObj.h"
#include "NiiPixelBuffer.h"
#include "NiiUICommon.h"

namespace NII
{
namespace UI
{
    /** UI渲染样式
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetView : public UIAlloc
    {
        friend class Widget;
    public:
        WidgetView(WidgetViewlID wsid, WidgetViewlID type);
        virtual ~WidgetView();

        /** UI渲染形式ID
        @version NIIEngine 3.0.0
        */
		inline WidgetViewlID getID() const { return mID; }

        /** UI渲染形式类型
        @version NIIEngine 3.0.0
        */
		inline WidgetViewlID getType() const { return mType; }

		/** 获取使用的风格
		@version NIIEngine 3.0.0
		*/
		Style * getStyle() const;

        /** 更新
        @param[in] cost 上次调用和这次度过的事件
        @version NIIEngine 3.0.0
        */
        virtual void update(TimeDurMS cost);

        /** 把UI冲刷到缓存中
        @version NIIEngine 3.0.0
        */
        virtual void flush() = 0;

        /** 执行样式排版
        @version NIIEngine 3.0.0
        */
        virtual void layout();

        /** 获取排版区域
        @version NIIEngine 3.0.0
        */
        virtual Rectf getArea() const;

        /** 获取实际画盘
        @version NIIEngine 3.0.0
        */
        virtual void getContext(SheetContext & ctx) const;
    protected:
        /** 附加属性
        @param pcd 属性
        @param serl 是否可序列化
        */
        void add(PropertyCmd * pcd, bool serl = true);

        /** 附加到UI单元时触发
        @version NIIEngine 3.0.0
        */
        virtual void onAttach();

        /** 从UI单元中解除时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDetach();

        /** 附加风格时触发
        @version NIIEngine 3.0.0
        */
        virtual void onStyleAttach();

        /** 解除风格时触发
        @version NIIEngine 3.0.0
        */
        virtual void onStyleDetach();

        /** 字体改变时触发
        @param[in] obj
        @verison NIIEngine 3.0.0
        */
        virtual bool notifyFont(const Font * obj);
    public:
        /** enable/启用 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const StateID EnableState;

        /** disable/禁用 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const StateID DisableState;

        /** 状态总数
        @version NIIEngine 3.0.0
        */
        static const StateID StateCount;
    protected:
        WidgetView & operator=(const WidgetView &);
    protected:
        typedef vector<std::pair<PropertyCmd *, bool> >::type PropertyCmdList;
    protected:
        Widget * mSrc;
        WidgetViewlID mID;
        WidgetViewlID mType;
        PropertyCmdList mCmdList;
    };

    /** 默认UI渲染样式
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DefaultView : public WidgetView
    {
    public:
        DefaultView(WidgetViewlID wsid);

        /// @copydetails WidgetView::flush
        void flush();
    };
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StaticView : public WidgetView
    {
    public:
        StaticView(WidgetViewlID wsid, WidgetViewlID type = N_MODEL_Static);

        /** 
        @version NIIEngine 3.0.0
        */
        void setFrame(bool b);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isFrame() const {return mFrame;}

        /** 
        @version NIIEngine 3.0.0
        */
        void setBackground(bool b);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isBackground() const {return mBackground;}

        /// @copydetails WidgetView::flush
        virtual void flush();
    protected:
        static const StateID WithFrameState;
        static const StateID NoFrameState;
        static const StateID WithFrameAndBgState;
        static const StateID WithFrameNoBgState;
        static const StateID WithBgNoFrameState;
        static const StateID NoFrameAndBgState;
        static const StateID StateCount;
    protected:
        bool mFrame;
        bool mBackground;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StaticTextView : public StaticView, public NII_COMMAND::CommandObj
    {
    public:
        StaticTextView(WidgetViewlID wsid);
        ~StaticTextView();

        /**
        @version NIIEngine 3.0.0
        */
        void setHLayout(HTextLayout layout);

        /**
        @version NIIEngine 3.0.0
        */
        inline HTextLayout getHLayout() const {return mHLayout;}

        /**
        @version NIIEngine 3.0.0
        */
        void setVLayout(VTextLayout layout);

        /**
        @version NIIEngine 3.0.0
        */
        inline VTextLayout getVLayout() const {return mVLayout;}

        /**
        @version NIIEngine 3.0.0
        */
        void setColour(const ColourArea & colour);

        /**
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const {return mColour;}

        /**
        @version NIIEngine 3.0.0
        */
        void setVScrollbar(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isVScrollbar() const {return mVScrollbar;}

        /**
        @version NIIEngine 3.0.0
        */
        void setHScrollbar(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isHScrollbar() const {return mHScrollbar;}

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getHTextExtent() const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getVTextExtent() const;

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails WidgetView::notifyFont
        bool notifyFont(const Font * font);
    public:
        static const StateID FrameAreaState;
        static const StateID FrameHScrollAreaState;
        static const StateID FrameVScrollAreaState;
        static const StateID FrameHVScrollAreaState;
        static const StateID AreaState;
        static const StateID HScrollAreaState;
        static const StateID VScrollAreaState;
        static const StateID HVScrollAreaState;
        static const StateID StateCount;

        static const WidgetID VScrollbarID;
        static const WidgetID HScrollbarID;
    protected:
        /// @copydetails WidgetView::onStyleAttach
        void onStyleAttach();

        /// @copydetails WidgetView::onStyleDetach
        void onStyleDetach();
        
        void updateFormatText(const PlaneSizef & sz) const;        
        
        void updateScrollbar();
        
        Scrollbar * getVScrollbar() const;

        Scrollbar * getHScrollbar() const;

        Rectf getRenderArea() const;

        PlaneSizef getDocumentSize(const Rectf & area) const;

        void onSize(const EventArgs * arg);
        
        void onText(const EventArgs * arg);
        
        void onFont(const FontEventArgs * arg);

        void onCursorDrag(const EventArgs * arg);

        void onScrollbarChange(const EventArgs * e);
    protected:
        SignalPtrs mSignalList;
        HTextLayout mHLayout;
        VTextLayout mVLayout;
        ColourArea mColour;
        bool mVScrollbar;
        bool mHScrollbar;
        mutable PixelLayout * mFormatText;
        mutable bool mFormatValid;
    };
    
    class _EngineAPI StaticImageView : public StaticView
    {
    public:
        StaticImageView(WidgetViewlID wsid);

        void setImage(PixelBuffer * img);
        
        inline PixelBuffer * getImage() const{ return mImage; }

        /// @copydetails StaticView::flush
        void flush();
    protected:
        PixelBuffer * mImage;
    };
}
}
#endif