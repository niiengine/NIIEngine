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

#ifndef _NII_UI_WIDGET_H_
#define _NII_UI_WIDGET_H_

#include "NiiUIPreInclude.h"
#include "NiiCommandObj.h"
#include "NiiPropertyObj.h"
#include "NiiExtData.h"
#include "NiiUIPixelUnitGrid.h"
#include "NiiUIWidgetAide.h"
#include "NiiUIWidgetList.h"
#include "NiiUIWidgetEventArgs.h"
#include "NiiColour.h"
#include "NiiPixelBuffer.h"
#include "NiiUICommon.h"
#include "NiiUISheet.h"
#include "NiiUICalcRect.h"
#include "NiiClipboard.h"
#include "NiiQuaternion.h"

using namespace NII::NII_MEDIA;
using namespace NII::NII_COMMAND;

#define N_WidgetModeFactory(c, id)  TWidgetViewFactory<c>(id)
#define N_WidgetFactory(c, id)      TWidgetFactory<c>(id)

namespace NII
{
namespace UI
{
    /** 单元风格工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetViewFactory : public FactoryAlloc
    {
    public:
        WidgetViewFactory() {}
        virtual ~WidgetViewFactory() {}

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** 创建单元风格
        @version NIIEngine 3.0.0
        */
        virtual WidgetView * create() = 0;

        /** 删除单元风格
        @version NIIEngine 3.0.0
        */
        virtual void destroy(WidgetView * obj) = 0;

    };

    /** 预定制UI风格工厂类
    @version NIIEngine 3.0.0
    */
    template <typename T> class TWidgetViewFactory : public WidgetViewFactory
    {
    public:
        TWidgetViewFactory(FactoryID fid):mID(fid) {}

        ///@copydetails WidgetViewFactory::getID
        FactoryID getID() const         { return mID; }

        ///@copydetails WidgetViewFactory::create
        WidgetView * create()           { return N_new T(mID); }

        ///@copydetails WidgetViewFactory::destroy
        void destroy(WidgetView * obj)  { N_delete obj; }

        /** 注入到系统中
        @version NIIEngine 3.0.0
        */
        void plugin()
        {
            WidgetViewFactory * factory = N_new TWidgetViewFactory<T>(mID);

            if (N_OnlyPtr(Widget))
            {
                if (N_Only(Widget).addViewFactory(factory) != true)
                {
                    N_delete factory;
                }
            }
        }

        /** 从系统中移出
        @version NIIEngine 3.0.0
        */
        void unplugin()
        {
            if (N_OnlyPtr(Widget))
            {
                WidgetViewFactory * factory = N_Only(Widget).getViewFactory(mID);
                if (factory != 0)
                {
                    N_Only(Widget).removeViewFactory(mID);
                    N_delete factory;
                }
            }
        }
    protected:
        FactoryID mID;
    };

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
        inline WidgetViewlID getID() const      { return mID; }

        /** UI渲染形式类型
        @version NIIEngine 3.0.0
        */
        inline WidgetViewlID getType() const    { return mType; }

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
        /** 基础状态
        @version NIIEngine 3.0.0
        */
        static const StateID NormalState;

        /** 禁用状态
        @version NIIEngine 3.0.0
        */
        static const StateID DisableState;

        /** 悬浮(光标)状态
        @version NIIEngine 3.0.0
        */
        static const StateID HoverState;

        /** 活动状态
        @version NIIEngine 3.0.0
        */
        static const StateID ActiveState;

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
        StaticView(WidgetViewlID wsid, WidgetViewlID type = N_VIEW_Static);

        /**
        @version NIIEngine 3.0.0
        */
        void setFrame(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isFrame() const { return mFrame; }

        /**
        @version NIIEngine 3.0.0
        */
        void setBackground(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isBackground() const { return mBackground; }

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
        inline HTextLayout getHLayout() const { return mHLayout; }

        /**
        @version NIIEngine 3.0.0
        */
        void setVLayout(VTextLayout layout);

        /**
        @version NIIEngine 3.0.0
        */
        inline VTextLayout getVLayout() const { return mVLayout; }

        /**
        @version NIIEngine 3.0.0
        */
        void setVScrollbar(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isVScrollbar() const { return mVScrollbar; }

        /**
        @version NIIEngine 3.0.0
        */
        void setHScrollbar(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isHScrollbar() const { return mHScrollbar; }

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
        bool mVScrollbar;
        bool mHScrollbar;
        mutable PixelLayout * mFormatText;
        mutable bool mFormatValid;
    };

    class _EngineAPI StaticImageView : public StaticView
    {
    public:
        StaticImageView(WidgetViewlID wsid);

        /// @copydetails StaticView::flush
        void flush();
    };

    /** UI单元工厂类
    @versioin NIIEngine 3.0.0
    */
    class _EngineAPI WidgetFactory : public UIAlloc
    {
    public:
        WidgetFactory() {}
        virtual ~WidgetFactory() {}

        /** 获取工厂ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** 创建对象
        @version NIIEngine 3.0.0
        */
        virtual Widget * create(WidgetID wid, Container * own) = 0;

        /** 删除对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(Widget * obj) = 0;
    };

    /** 预定制UI单元工厂类
    @version NIIEngine 3.0.0
    */
    template <typename T> class TWidgetFactory : public WidgetFactory
    {
    public:
        TWidgetFactory(FactoryID fid) : mID(fid) {}

        /// @copydetails WidgetFactory::getID
        FactoryID getID() const                         { return mID; }

        /// @copydetails WidgetFactory::create
        Widget * create(WidgetID wid, Container * own)  { return N_new T(wid, mID, own); }

        /// @copydetails WidgetFactory::destroy
        void destroy(Widget * obj)                      { N_delete obj; }

        /** 注入到系统中
        @version NIIEngine 3.0.0
        */
        void plugin()
        {
            WidgetFactory * factory = N_new TWidgetFactory<T>(mID);
            if (N_OnlyPtr(Widget))
            {
                if (N_Only(Widget).addFactory(factory) != true)
                {
                    N_delete factory;
                }
            }
        }

        /** 从系统中移出
        @version NIIEngine 3.0.0
        */
        void unplugin()
        {
            if (N_OnlyPtr(Widget))
            {
                WidgetFactory * factory = N_Only(Widget).getFactory(mID);
                if (factory != 0)
                {
                    N_Only(Widget).removeFactory(mID);
                    N_delete factory;
                }
            }
        }
    protected:
        FactoryID mID;
    };

    typedef Rectf(Widget::*CalcMethod)(bool) const;
    typedef bool(Widget::*intersect)(const Vector2f &, bool) const;
    /** UI单元基础类
    @remark
        UI单元不可能独立于窗体级别的UI单元存在,也就是说UI单元必须在任意一个窗体,否
        则它是没有效的.这样做是为了避免重复的判断重叠位置关系,而直接通过上级窗体确定
    @note 这个基础类并没有渲染能力
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Widget : public CommandObj, public PropertyCmdObj, 
        public ExtData, public UIAlloc
    {
        friend class WidgetManager;
        friend class WidgetView;
        friend class UISubSheet;
        friend class UISheet;
        friend class Window;
        friend class Container;
        friend class MouseControlItem;
    public:
        Widget(ScriptTypeID stid, WidgetID wid, FactoryID sid, Container * own,
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);
        virtual ~Widget();

        /** 复制
        @param r 包含下级对象
        @return 副本对象
        @version NIIEngine 3.0.0
        */
        Widget * clone(bool r = true) const;

        /** 更新
        @param[in] cost 与上次调用后渡过的时间,用于更新可能出现的动画
        @version NIIEngine 3.0.0
        */
        virtual void update(TimeDurMS cost);

        /** 添加辅助
        @version NIIEngine 3.0.0
        */
        void addAide(WidgetAide * aide);

        /** 移曲辅助
        @version NIIEngine 3.0.0
        */
        void removeAide(WidgetAide * aide);

        /** 活动状态
        @version NIIEngine 3.0.0
        */
        virtual void activate();

        /** 非活动状态
        @version NIIEngine 3.0.0
        */
        virtual void deactivate();

        /** 显示
        @version NIIEngine 3.0.0
        */
        void show()                                     { setVisible(true); }

        /** 隐藏
        @version NIIEngine 3.0.0
        */
        void hide()                                     { setVisible(false); }

        /** 启动状态
        @version NIIEngine 3.0.0
        */
        void enable()                                   { setEnable(true); }

        /** 禁用状态
        @version NIIEngine 3.0.0
        */
        void disable()                                  { setEnable(false); }

        /** 删除内部结构
        @version NIIEngine 3.0.0
        */
        virtual void destroy();

        /** 是否是容器对象
        @note 指的是 NII::UI::Container 类
        @version NIIEngine 3.0.0
        */
        virtual bool isContainer() const;

        /** 返回这个对象的类型
        @version NIIEngine 3.0.0
        */
        inline FactoryID getType() const                { return mType; }

        /** 设置UI单元ID
        @version NIIEngine 3.0.0
        */
        void setID(WidgetID id);

        /** 返回这个这个对象的ID
        @version NIIEngine 3.0.0
        */
        inline WidgetID getID() const                   { return mID; }

        /** 设置名字
        @version NIIEngine 3.0.0
        */
        void setName(const String & n);

        /** 获取名字
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const           { return mName; }

        /** 获取名字路径
        @remark 包含父对象路径的描述
        @version NIIEngine 3.0.0
        */
        String getNamePath() const;

        /** 设置容器对象
        @param c 容器对象
        @version NIIEngine 3.0.0
        */
        void setParent(Container * c);

        /** 获取容器对象
        @version NIIEngine 3.0.0
        */
        inline Container * getParent() const            { return mParent; }

        /** 获取UI单元所在的画盘
        @version NIIEngine 3.0.0
        */
        inline UISheet * getSheet() const               { return mSheet; }

        /** 是否启用UI单元
        @version NIIEngine 3.0.0
        */
        void setEnable(bool b);

        /** 设置UI单元的更新模式
        @remark 这是UI单元内容发生变化时候更新
        @version NIIEngine 3.0.0
        */
        inline void setUpdateMode(RefreshMode m)        { mRefreshMode = m; }

        /** 返回UI单元的更新模式
        @remark 这是UI单元内容发生变化时候更新
        @version NIIEngine 3.0.0
        */
        inline RefreshMode getUpdateMode() const        {  return mRefreshMode;}

        /** 设置UI单元是否可见
        @version NIIEngine 3.0.0
        */
        void setVisible(bool b);

        /** 返回这个元素当前是否可见
        @version NIIEngine 3.0.0
        */
        inline bool isVisible() const                   { return mVisible; }

        /** 设置层是否可以改变
        @version NIIEngine 3.0.0
        */
        inline void setZOrder(bool b)                   { mZOrder = b; }

        /** 获取层是否可以改变
        @version NIIEngine 3.0.0
        */
        inline bool isZOrder() const                    { return mZOrder; }

        /** 设置游标点击是否产生层排序
        @version NIIEngine 3.0.0
        */
        inline void setZOrderClick(bool b)              { mZOrderClick = b; }

        /** 获取游标点击是否产生层排序
        @version NIIEngine 3.0.0
        */
        inline bool isZOrderClick() const               { return mZOrderClick; }
        
        /** 设置边余
        @version NIIEngine 3.0.0
        */
        virtual void setMargin(const RelRectf & rect);

        /** 获取边余
        @version NIIEngine 3.0.0
        */
        inline const RelRectf & getMargin() const       { return mMargin; }

        /** 获取边余偏移
        @version NIIEngine 3.0.0 高级api
        */
        virtual RelVector2f getMarginOft() const;

        /** 获取加上边余的元素大小
        @version NIIEngine 3.0.0 高级api
        */
        virtual RelVector2f getMarginSize() const;

        /** 设置风格
        @param sid 设计风格
        @param wsid 单元风格
        @version NIIEngine 3.0.0
        */
        void setStyle(StyleID sid, WidgetViewlID wsid);

        /** 设置设计风格
        @version NIIEngine 3.0.0
        */
        virtual void setStyle(StyleID sid);

        /** 获取设计风格
        @version NIIEngine 3.0.0
        */
        inline StyleID getStyle() const                 {return mStyleID;}

        /** 设置单元风格
        @version NIIEngine 3.0.0
        */
        void setWidgetView(WidgetViewlID wsid);

        /** 获取单元风格
        @version NIIEngine 3.0.0
        */
        inline WidgetView * getWidgetView() const       {return mWidgetView;}

        /** 设置单元风格
        @version NIIEngine 3.0.0
        */
        void setStyleView(StyleViewComID svcid);

        /** 返回这个对象的类型
        @version NIIEngine 3.0.0
        */
        inline StyleViewID getStyleView() const         { return mViewID & N_Com_EscapeMark; }

        /** 返回这个对象的类型组
        @version NIIEngine 3.0.0
        */
        inline GroupID getStyleViewGroup() const        { return mViewID >> N_Com_Escape; }

        /** 设置提示文本
        @param[in] txt 提示文本
        @version NIIEngine 3.0.0
        */
        void setTooltipText(const String & tip)         { mTipsText = tip; }

        /** 获取提示文本
        @version NIIEngine 3.0.0
        */
        const String & getTooltipText() const;

        /** 设置是否使用父对象提示文本
        @version NIIEngine 3.0.0
        */
        inline void setParentTipsText(bool b)           {mParentTipsText = b;}

        /** 获取是否使用父对象提示文本
        @return NIIEngine 3.0.0
        */
        inline bool isParentTooltipText() const         {return mParentTipsText;}

        /** 设置水平对齐方式
        @remark 这个位置相对于父对象
        @param[in] align 水平对齐方式
        @version NIIEngine 3.0.0
        */
        void setXAlign(HAlign align);

        /** 获取水平对齐方式
        @remark 这个位置相对于父对象
        @return 水平对齐方式
        @version NIIEngine 3.0.0
        */
        inline HAlign getXAlign() const                 { return mHAlign;}

        /*** 设置垂直对齐方式
        @remark 这个位置相对于父对象
        @param align 垂直对齐方式
        @version NIIEngine 3.0.0
        */
        void setYAlign(VAlign align);

        /** 获取垂直对齐方式
        @remark 这个位置相对于父对象
        @return 垂直对齐方式
        @version NIIEngine 3.0.0
        */
        inline VAlign getYAlign() const                 { return mVAlign;}

        /** 设置透明度
        @param a 透明度[0.0-1.0]
        @version NIIEngine 3.0.0
        */
        void setAlpha(NIIf a);

        /** 返回这个对象所设置的透明度
        @version NIIEngine 3.0.0
        */
        inline NIIf getAlpha() const                    { return mAlpha;}

        /** 设置是否使用环境透明度
        @remark 一般是指同个体系中的透明度
        @version NIIEngine 3.0.0
        */
        void setEnvAlpha(bool b);

        /** 获取是否使用环境透明度
        @remark 一般是指同个体系中的透明度
        @version NIIEngine 3.0.0
        */
        inline bool isEnvAlpha() const                  { return mEnvAlpha; }

        /** 获取最终的透明度
        @version NIIEngine 3.0.0
        */
        NIIf getAlphaEffect() const;

        /** 设置背景图片
        @version NIIEngine 3.0.0
        */
        void setBGImage(PixelBuffer * img);

        /** 获取背景图片
        @version NIIEngine 3.0.0
        */
        inline PixelBuffer * getBGImage() const         { return mImage; }

        /** 设置区域大小与显示的缩放模式
        @param mode 模式
        @version NIIEngine 3.0.0
        */
        void setScaleMode(AspectMode mode);

        /** 获取区域大小与显示的缩放模式
        @version NIIEngine 3.0.0
        */
        inline AspectMode getScaleMode() const          { return mScaleMode; }

        /** 设置区域大小与显示的缩放因子
        @param ratio 宽/高的比率
        @note AspectMode 是 AM_IGNORE 时无效.
        @version NIIEngine 3.0.0
        */
        void setScaleFactor(NIIf ratio);

        /** 获取区域大小与显示的缩放因子
        @version NIIEngine 3.0.0
        */
        inline NIIf getScaleFactor() const              { return mScaleFactor;}

        /** 设置是否总在最表层
        @remark
            如果任意2个UI窗体有相同的层别设置,系统将会按实际显示先后进行层排序
        @version NIIEngine 3.0.0
        */
        void setTop(bool b);

        /** 获取是否总在最表层
        @remark
            如果任意2个UI窗体有相同的层别设置,系统将会按实际显示先后进行层排序
        @version NIIEngine 3.0.0
        */
        inline bool isTop() const                       { return mTop; }

        /** 设置元素的位置
        @param pos 相对位置
        @version NIIEngine 3.0.0
        */
        void setPos(const RelVector2f & pos)            { setAreaImpl(pos, mArea.getSize()); }

        /** 获取元素位置
        @return 相对位置
        @version NIIEngine 3.0.0
        */
        inline RelVector2f getPos() const               { return mArea.getPos(); }

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getActualPosX() const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getActualPosY() const;

        /** 设置元素的大小
        @param size 元素大小
        @version NIIEngine 3.0.0
        */
        void setSize(const RelPlaneSizef & size)        { setAreaImpl(mArea.getPos(), size); }

        /** 获取元素的大小
        @return 元素大小
        @version NIIEngine 3.0.0
        */
        inline RelPlaneSizef getSize() const            { return mArea.getSize(); }

        /** 设置位置区域.
        @param pos 相对位置
        @param size 相对大小
        @version NIIEngine 3.0.0
        */
        inline void setRelArea(const RelRectf & area)   { setAreaImpl(area.getPos(), area.getSize());  }

        /** 获取位置区域
        @version NIIEngine 3.0.0
        */
        inline const RelRectf & getRelArea() const      { return mArea; }

        /** 这个UI元素是否在指定的位置
        @param[in] pos 指定位置
        @param[in] disable 可以处于禁用状态
        @version NIIEngine 3.0.0
        */
        virtual bool isPos(const Vector2f & pos, bool disable) const;

        /** 是否最顶层位置
        @version NIIEngine 3.0.0
        */
        bool isTopPos() const;

        /** 获取层位置
        @version NIIEngine 3.0.0
        */
        Nidx getPosLevel() const;

        /** 是否在另一单元的上层
        @version NIIEngine 3.0.0
        */
        bool isFront(const Widget * obj) const;

        /** 是否在另一单元的下层
        @version NIIEngine 3.0.0
        */
        bool isBehind(const Widget * obj) const;

        /** 把这个单元移到另一个前面
        @version NIIEngine 3.0.0
        */
        void moveFront(const Widget * obj);

        /** 把这个单元移到另一个后面
        @version NIIEngine 3.0.0
        */
        void moveBack(const Widget * obj);

        /** 把这个单元移到最表层
        @version NIIEngine 3.0.0
        */
        void moveFront();

        /** 把这个单元移到最底层
        @version NIIEngine 3.0.0
        */
        void moveBack();

        /** 设置宽
        @version NIIEngine 3.0.0
        */
        void setWidth(const RelPosf & w);

        /** 获取宽
        @version NIIEngine 3.0.0
        */
        inline RelPosf getWidth() const                 { return getSize().mWidth;}

        /** 设置高
        @version NIIEngine 3.0.0
        */
        void setHeight(const RelPosf & h);

        /** 获取高
        @version NIIEngine 3.0.0
        */
        inline RelPosf getHeight() const                { return getSize().mHeight;}

        /** 设置元素的最小大小
        @version NIIEngine 3.0.0
        */
        void setMinSize(const RelPlaneSizef & size);

        /** 获取元素的最小大小
        @version NIIEngine 3.0.0
        */
        inline const RelPlaneSizef & getMinSize() const { return mMinSize; }

        /** 设置元素最大大小
        @version NIIEngine 3.0.0
        */
        void setMaxSize(const RelPlaneSizef & size);

        /** 获取元素最大大小
        @version NIIEngine 3.0.0
        */
        inline const RelPlaneSizef & getMaxSize() const { return mMaxSize;}

        /** 设置是否对齐像素
        @version NIIEngine 3.0.0
        */
        void setPixelAlign(bool b);

        /** 获取是否对齐像素
        @version NIIEngine 3.0.0
        */
        inline bool getPixelAlign() const               { return mPixelAlign;}

        /** 获取元素的绝对位置(单位:像素)
        @return Vector2f 元素实际的位置(单位:像素)
        @version NIIEngine 3.0.0
        */
        Vector2f getPixelPos() const;

        /** 获取元素的大小(单位:像素)
        @return 元素的大小(单位:像素)
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getPixelSize() const  { return mPixelSize; }

        /** 设置是否自动裁剪
        @note 一般是被动裁剪/父对象裁减
        @version NIIEngine 3.0.0
        */
        void setAutoClip(bool b);

        /** 获取是否自动裁剪
        @version NIIEngine 3.0.0
        */
        inline bool isAutoClip() const                  { return mAutoClip; }

        /** 是否自动删除
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b);

        /** 是否自动删除
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDestroy() const               { return mAutoDestroy;}

        /** 返回这个元素是否活动
        @version NIIEngine 3.0.0
        */
        inline bool isActive() const                    { return mActive; }

        /** 返回这个元素是否活动
        @remark 意味着这个元素接收用户输入事件
        @version NIIEngine 3.0.0
        */
        bool isFinalActive() const;

        /** 返回这个元素当前是否禁用
        @return true则代表禁止了,false则代表启用
        @version NIIEngine 3.0.0
        */
        inline bool isDisable() const                   { return !mEnable;}

        /** 返回这个元素当前是否禁止(最终效果)
        @return true 则代表禁止了, false则代表启用
        @version NIIEngine 3.0.0
        */
        bool isFinalDisable() const;

        /** 返回这个元素当前是否可见(最终效果)
        @version NIIEngine 3.0.0
        */
        bool isFinalVisible() const;

        /** 指定对象是否是上级对象
        @param id 上级对象ID
        @version NIIEngine 3.0.0
        */
        bool isAncestor(WidgetID id) const;

        /** 指定对象是否是上级对象
        @param e 上级对象
        @version NIIEngine 3.0.0
        */
        bool isAncestor(const Widget * con) const;

        /** 指定对象是否是上级对象
        @param name 上级对象名字
        @version NIIEngine 3.0.0
        */
        bool isAncestor(const String & name) const;

        /** 获取共同的上级对象
        @note 另一个对象的上级对象与这个对象有共同上级对象则返回这个对象的上级对象
        @param[in] o 另一个对象
        @version NIIEngine 3.0.0
        */
        const Widget * getCommonAncestor(const Widget * o) const;

        /** 检测给定像素是否和这个UI单元相交
        @param pos 像素位置
        @param disable true UI单元可以处于禁用状态;false UI单元不能处于禁用状态
        @return 是否存在相交
        @version NIIEngine 3.0.0
        */
        virtual bool intersects(const Vector2f & pos, bool disable = false) const;

        /** 返回同级活动中的对象
        @version NIIEngine 3.0.0
        */
        Widget * getActiveSibling();

        /** 返回这个UI元素的大体区域
        @remark 也就是包含外观(边框或其他)的区域
        @note 和元素在屏幕空间中像素位置相同
        @return 大体区域
        @version NIIEngine 3.0.0
        */
        inline const CalcRect & getArea() const     {  return mRect; }

        /** 返回这个UI元素的有效区域
        @remark 不包含外观(边框或其他)的区域(内容区域)
        @remark 控制区域不包含子对象无法到达的区域
        @return 内部区域
        @version NIIEngine 3.0.0
        */
        inline const CalcRect & getInnerArea() const { return mInnerRect; }

        /** 返回这个UI元素的指定区域
        @param inner
            true 返回这个UI元素的有效区域;false 返回这个UI元素的大体区域
        @version NIIEngine 3.0.0
        */
        inline const CalcRect & getArea(bool inner) const { return inner ? mInnerRect : mRect; }

        /** 返回这个元素的大体裁剪区域
        @remark
            也就是包含外观(边框或其他)的区域,但这个区域会被父对象(渲染面)实际的渲染
            区域所裁减
        @version NIIEngine 3.0.0
        */
        const Rectf & getClipArea() const;

        /** 返回这个UI元素的有效裁剪区域
        @remark
            不包含外观(边框或其他)的区域(内容区域),而且这个区域会被父对象(渲染面)实
            际的渲染区域所裁减
        @version NIIEngine 3.0.0
        */
        const Rectf & getInnerClipArea() const;

        /** 返回这个UI元素的指定裁剪区域
        @param content
            true 返回这个UI元素的有效裁剪区域;false 返回这个元素的形体裁剪区域
        @version NIIEngine 3.0.0
        */
        inline const Rectf & getClipArea(bool content) const{ return content ? getInnerClipArea() : getClipArea(); }

        /** 获取有效的检测区域
        @version NIIEngine 3.0.0
        */
        const Rectf & getIntersectArea() const;

        /** 通知位置已经改变了
        @remark
            对象位置改变说明对象坐标变化了,渲染图形的位置缓存也应该要变化
        @note
            在数据改变(后)才调用了这个函数
        @version NIIEngine 3.0.0
        */
        virtual void notifyPosChange();

        /** 设置是否自动重复生成鼠标按下事件
        @param set true 启动;false 禁用
        */
        void setClickAutoEvent(bool set);

        /** 是否自动重复生成鼠标按下事件
        */
        inline bool isClickAutoEvent() const { return mClickAutoEvent; }

        /** 设置引发自动事件条件需要的时间阀
        @param delay 单位秒
        @return Nothing.
        */
        inline void setClickThreshold(TimeDurMS delay)  { mClickThreshold = delay; }

        /** 或引发自动事件条件需要的时间阀
        @return 单位秒
        */
        inline TimeDurMS getClickThreshold() const  { return mClickThreshold; }

        /** 引发自动事件条件达成后,自动引发的时间间隔
        @param rate 单位秒
        @version NIIEngine 3.0.0
        */
        inline void setClickRate(TimeDurMS rate)    { mClickRate = rate; }

        /** 引发自动事件条件达成后,自动引发的时间间隔
        @return 单位秒
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getClickRate() const       { return mClickRate; }

        /** 是否上传(到父对象)游标事件
        @version NIIEngine 3.0.0
        */
        inline void setCursorSubmitEvent(bool b)    { mCursorSubmitEvent = b; }

        /** 是否上传(到父对象)游标事件
        @return NIIEngine 3.0.0
        */
        inline bool isCursorSubmitEvent() const     { return mCursorSubmitEvent; }

        /** 设置自动重复触发"click"事件
        @version NIIEngine 3.0.0
        */
        inline void setCursorMultiClick(bool b)     { mMultiClickEventEnable = b;}

        /** 是否自动重复触发"click"事件
        @version NIIEngine 3.0.0
        */
        inline bool isCursorMultiClick() const      { return mMultiClickEventEnable;}

        /** 设置是否存储上一次对焦的对象
        @version NIIEngine 3.0.0
        */
        virtual void setRestorePreFocus(bool b);

        /** 获取是否存储上一次对焦的对象
        @version NIIEngine 3.0.0
        */
        virtual bool isRestorePreFocus() const;

        /** 设置是否自动产生的对象
        @remark 如一个对话框,里面默认包含一些按钮,则这些按钮属于自动对象
        @version NIIEngine 3.0.0
        */
        void setAutoWindow(bool b);

        /** 返回是否自动产生的对象
        @remark 如一个对话框,里面默认包含一些按钮,则这些按钮属于自动对象
        @version NIIEngine 3.0.0
        */
        inline bool isAutoWindow() const                    { return mAutoGen; }

        /** 返回这个对象是否被对焦
        @version NIIEngine 3.0.0
        */
        inline bool isFocus() const                         { return mSheet->getFocus() == this; }

        /** 返回这个对象的上级是否被对焦
        @version NIIEngine 3.0.0
        */
        inline bool isAncestorFocus() const                 { return isAncestor(mSheet->getFocus()); }

        /** 返回实际的可以渲染的大小
        @version NIIEngine 3.0.0
        */
        inline PlaneSizei getRenderSize() const             { return mSheet->getArea().getSize(); }

        /** 设置字体
        @version NIIEngine 3.0.0
        */
        void setFont(Font * obj);

        /** 返回使用的字体
        @version NIIEngine 3.0.0
        */
        Font * getFont(FontID fid = 0) const;

        /** 设置基础文本
        @version NIIEngine 3.0.0
        */
        void setText(const String & t);

        /** 获取基础文本
        @version NIIEngine 3.0.0
        */
        inline const String & getText() const               { return mText; }

        /** 设置基础文本颜色
        @version NIIEngine 3.0.0
        */
        void setTextColour(const ColourArea & colour);

        /** 获取基础文本颜色
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getTextColour() const     { return mForegroundColour; }

        /** 插入基础文本
        @version NIIEngine 3.0.0
        */
        void insertText(const String & text, Nidx pos);

        /** 扩充基础文本
        @version NIIEngine 3.0.0
        */
        void appendText(const String & text);

        /** 获取视觉文本
        @version NIIEngine 3.0.0
        */
        const String & getVisualText() const;

        /** 获取渲染文本
        @version NIIEngine 3.0.0
        */
        const PixelUnitGrid * getRenderText() const;

        /** 设置是否使用文本排版器
        @version NIIEngine 3.0.0
        */
        void setTextLayout(bool b);

        /** 获取是否使用文本排版器
        @version NIIEngine 3.0.0
        */
        inline bool isTextLayout() const                    { return mTextLayout;}

        /** 返回双向文本
        @version NIIEngine 3.0.0
        */
        inline const BidiText * getBidiText() const         { return mBidiText; }

        /** 设置文本排版器
        @version NIIEngine 3.0.0
        */
        void setTextView(TextView * obj);

        /** 获取文本排版器
        @version NIIEngine 3.0.0
        */
        inline TextView * getTextView() const               { return mTextView; }

        /** 获取实际使用的排版器
        @version NIIEngine 3.0.0
        */
        virtual TextView * getRenderTextView() const;

        /**
        @version NIIEngine 3.0.0
        */
        inline KeyboardControlItem * getKeyboardItem() const { return mC0; }

        /**
        @version NIIEngine 3.0.0
        */
        inline MouseControlItem * getMouseItem() const      { return mC1; }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline TouchControlItem * getTouchItem() const      { return mC2; }

        /** 设置数据是否可序列化
        @version NIIEngine 3.0.0
        */
        inline void setDataWrite(bool set)                  { mDataWrite = set;}

        /** 返回数据是否可序列化
        @version NIIEngine 3.0.0
        */
        inline bool isDataWrite() const                     { return mDataWrite;}

        /** 序列化到数据中
        @param[in] out 序列器
        @version NIIEngine 3.0.0
        */
        virtual void write(XmlSerializer * out) const;

        /** 设置序列化属性
        @version NIIEngine 3.0.0
        */
        void setWrite(PropertyID pid, bool b);

        /** 是否序列化属性数据
        @version NIIEngine 3.0.0
        */
        bool isWrite(PropertyID pid) const;

        /** 属性是否处于默认值
        @version NIIEngine 3.0.0
        */
        bool isDefault(const PropertyCmd * obj) const;
    public:
        /** 对焦UI单元
        @version NIIEngine 3.0.0
        */
        bool focus();

        /** UI单元失焦
        @version NIIEngine 3.0.0
        */
        void lost();

        /** 返回所在的窗体
        @return
            UI单元的上级是容器对象,容器对象不一定是窗体对象,但所有容器对象必定是属于
            某个窗体.
        @version NIIEngine 3.0.0
        */
        Window * getRoot() const;

        /** 获取几何像素
        @version NIIEngine 3.0.0
        */
        inline GeometryPixel * getPixelData() const     { return mPixelData; }

        /** 计算元素的像素大小
        @param pixelAlign 是否整型化大小
        @version NIIEngine 3.0.0
        */
        PlaneSizef calcArea(bool pixelAlign = true) const;

        /** 获取父对象元素的像素大小
        @param pixelAlign 是否整型化大小
        @version NIIEngine 3.0.0
        */
        PlaneSizef calcParentArea(bool pixelAlign = true) const;

        /** 刷新渲染
        @param[in] r 是否含概下级
        @version NIIEngine 3.0.0
        */
        void refresh(bool r = false);

        /** 执行样式排版
        @version NIIEngine 3.0.0
        */
        virtual void layout();

        /** 执行复制
        @version NIIEngine 3.0.0
        */
        virtual bool onCopyData(Clipboard & clipboard);

        /** 执行剪切
        @version NIIEngine 3.0.0
        */
        virtual bool onCutData(Clipboard & clipboard);

        /** 执行粘贴
        @version NIIEngine 3.0.0
        */
        virtual bool onPasteData(Clipboard & clipboard);
    public:
        /** id变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID IDEvent;

        /** 名字变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID NameEvent;

        /** 尺寸变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID SizeEvent;

        /** 边余属性
        @version NIIEngine 3.0.0
        */
        static const EventID MarginModeEvent;

        /** 上级尺寸变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID ParentSizeEvent;

        /** 移动事件
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEvent;

        /** 透明度变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID AlphaEvent;

        /** 环境透明度变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID EnvAlphaEvent;

        /** 面旋转事件
        @version NIIEngine 3.0.0
        */
        static const EventID RotateEvent;

        /** 更新事件
        @version NIIEngine 3.0.0
        */
        static const EventID UpdataEvent;

        /** 活动状态事件
        @version NIIEngine 3.0.0
        */
        static const EventID ActivateEvent;

        /** 不活动状态事件
        @version NIIEngine 3.0.0
        */
        static const EventID DeactivateEvent;

        /** 显示状态事件
        @version NIIEngine 3.0.0
        */
        static const EventID ShowEvent;

        /** 不显示状态事件
        @version NIIEngine 3.0.0
        */
        static const EventID HideEvent;

        /** 启用事件
        @version NIIEngine 3.0.0
        */
        static const EventID EnableEvent;

        /** 禁用事件
        @version NIIEngine 3.0.0
        */
        static const EventID DisableEvent;

        /** 水平对齐改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID HAlignModeEvent;

        /** 垂直对齐改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID VAlignModeEvent;

        /** 对焦事件
        @version NIIEngine 3.0.0
        */
        static const EventID FocusEvent;

        /** 失焦事件
        @version NIIEngine 3.0.0
        */
        static const EventID LostEvent;

        /** 自动裁剪改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID AutoClipModeEvent;

        /** 自动删除改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID AutoDestroyModeEvent;

        /** 渲染开始事件
        @version NIIEngine 3.0.0
        */
        static const EventID RenderBeginEvent;

        /** 渲染结束事件
        @version NIIEngine 3.0.0
        */
        static const EventID RenderEndEvent;

        /** 实际的渲染面改变(后)触发
        @version NIIEngine 3.0.0
        */
        static const EventID RenderCtxEvent;

        /** 置顶设置改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID TopModeEvent;

        /** 层变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID ZOrderEvent;

        /** 删除开始事件
        @version NIIEngine 3.0.0
        */
        static const EventID DestroyStartEvent;

        /** 一个UI单元风格应用到这个UI对象(后)触发
        @version NIIEngine 3.0.0
        */
        static const EventID ViewAttachEvent;

        /** 一个UI单元风格从这个UI对象中失效(后)触发
        @version NIIEngine 3.0.0
        */
        static const EventID ViewDetachEvent;

        /** 风格应用到这个UI对象(后)触发
        @version NIIEngine 3.0.0
        */
        static const EventID StyleAttachEvent;

        /** 风格从这个UI对象中失效(后)触发
        @version NIIEngine 3.0.0
        */
        static const EventID StyleDetachEvent;

        /** 无效状态事件
        @version NIIEngine 3.0.0
        */
        static const EventID InvalidEvent;

        /** 字体改变时
        @version NIIEngine 3.0.0
        */
        static const EventID FontEvent;

        /** 文本改变时
        @version NIIEngine 3.0.0
        */
        static const EventID TextEvent;

        /** 文本排版改变时
        @version NIIEngine 3.0.0
        */
        static const EventID TextLayoutModeEvent;

        /** 游标进入事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorEnterEvent;

        /** 游标移出事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorLeaveEvent;

        /** 游标移动事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorMoveEvent;

        /** 游标拖拽事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorDragEvent;

        /** 游标触控事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorTouchEvent;

        /** 游标多重触控事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorMultiTouchEvent;

        /** 按键按下事件
        @version NIIEngine 3.0.0
        */
        static const EventID ButtonDownEvent;

        /** 按键释放事件
        @version NIIEngine 3.0.0
        */
        static const EventID ButtonUpEvent;

        /** 键盘按下事件
        @version NIIEngine 3.0.0
        */
        static const EventID KeyDownEvent;

        /** 键盘释放事件
        @version NIIEngine 3.0.0
        */
        static const EventID KeyUpEvent;

        /** 文本输入事件
        @version NIIEngine 3.0.0
        */
        static const EventID CharEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;

        /** 内部事件
        @version NIIEngine 3.0.0
        */
        static const PropertyID InnerPropertyLimit;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /** 是否适用
        @remark 一般是指是否匹配
        @version NIIEngine 3.0.0
        */
        virtual bool validImpl(const WidgetView * view) const;

        /** 获取实际UI面盘
        @param[out] ctx UI面盘
        @version NIIEngine 3.0.0
        */
        void getContext(SheetContext & ctx) const;

        /** 排列渲染
        @version NIIEngine 3.0.0 高级API
        */
        virtual void queuePixelImpl();

        /** 副本实现
        @version NIIEngine 3.0.0
        */
        virtual void cloneImpl(Widget * dest, bool r) const;

        /** 区域实现
        @param[in] pos 相对位置
        @param[in] size 相对大小
        @param[in] event 是否触发事件
        @version NIIEngine 3.0.0
        */
        virtual void setAreaImpl(const RelVector2f & pos, const RelPlaneSizef & size, bool event = true);

        /** 执行更新
        @remark 样式/位置动画等更新
        @param cost 上次调用渡过的时间
        @version NIIEngine 3.0.0 高级api
        */
        virtual void updateImpl(TimeDurMS cost);

        /** 更新渲染数据
        @remark 位置/裁剪面等数据
        @version NIIEngine 3.0.0 高级api
        */
        virtual void updateImpl(SheetContext * rc);

        /** 填充渲染数据
        @remark 一般是内部结构发生变化需要填充
        @version NIIEngine 3.0.0 高级api
        */
        virtual void renderImpl();

        /** 层移动实现
        @version NIIEngine 3.0.0
        */
        virtual void ZOrderImpl();

        /** 移到同范围的最前层
        @return 仅是同范围
        @param[in] click
        @version NIIEngine 3.0.0
        */
        virtual bool moveFrontImpl(bool click);

        /** 获取内部区域实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual Rectf getAreaImpl(bool pixelAlign) const;

        /** 获取整体区域实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual Rectf getInnerAreaImpl(bool pixelAlign) const;

        /** 裁剪区域发生变化
        @version NIIEngine 3.0.0 高级api
        */
        virtual void notifyClipChange();

        /** 获取内部裁减区域实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual Rectf getClipAreaImpl() const;

        /** 获取内部裁减区域实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual Rectf getClipInnerAreaImpl() const;

        /** 有效的交叉区域实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual Rectf getIntersectAreaImpl() const;

        /** 获取获取实际UI面盘实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void getContextImpl(SheetContext & out) const;

        /** 刷新实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void refreshImpl(bool r);

        /** 刷新渲染面
        @version NIIEngine 3.0.0 高级API
        */
        virtual void refreshFaceImpl();

        /** 非序列化数据
        @version NIIEngine 3.0.0 高级api
        */
        virtual void disableWrite();

        /** 写入到序列器
        @param[in] out 序列器
        */
        virtual NCount writeImpl(XmlSerializer * out) const;
    public:
        /** 这个UI对象由外部数据载入时触发
        @remark 外部事件型
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onBeginInit();

        /** 这个UI对象由外部数据结束载入时触发
        @remark 外部事件型
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onEndInit();
    protected:
        /** ID改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onID(const WidgetEventArgs * arg);

        /** 名字改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onName(const WidgetEventArgs * arg);

        /** 尺寸改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onSize(const WidgetEventArgs * arg);

        /** 父对象尺寸改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onParentSize(const WidgetEventArgs * arg);

        /** 当元素的位置改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onMove(const WidgetEventArgs * arg);

        /** 水平对齐方式改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onHAlignMode(const WidgetEventArgs * arg);

        /** 垂直对齐方式改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onVAlignMode(const WidgetEventArgs * arg);

        /** 边余改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onMargin(const WidgetEventArgs * arg);

        /** 透明度改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onAlpha(const WidgetEventArgs * arg);

        /** 环境透明度改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onEnvAlpha(const WidgetEventArgs * arg);

        /** 显示状态改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onShow(const WidgetEventArgs * arg);

        /** 隐藏状态改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onHide(const WidgetEventArgs * arg);

        /** 启用状态改变触发
        @version NIIEngine 3.0.0
        */
        virtual void onEnable(const WidgetEventArgs * arg);

        /** 禁用状态改变触发
        @version NIIEngine 3.0.0
        */
        virtual void onDisable(const WidgetEventArgs * arg);

        /** 裁剪改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onClipMode(const WidgetEventArgs * arg);

        /** 置顶设置改变时触发
        @versioin NIIEngine 3.0.0
        */
        virtual void onTopMode(const WidgetEventArgs * arg);

        /** 层改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onZOrder(const WidgetEventArgs * arg);

        /** 自动删除改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onAutoDestroyMode(const WidgetEventArgs * arg);

        /** 对焦时触发
        @version NIIEngine 3.0.0
        */
        virtual void onFocus(const WidgetEventArgs * arg);

        /** 失焦时触发
        @version NIIEngine 3.0.0
        */
        virtual void onLost(const WidgetEventArgs * arg);

        /** 刷新时触发
        @version NIIEngine 3.0.0
        */
        virtual void onRefresh(const WidgetEventArgs * arg);

        /** 渲染前触发
        @version NIIEngine 3.0.0
        */
        virtual void onPreRender(const WidgetEventArgs * arg);

        /** 渲染后触发
        @version NIIEngine 3.0.0
        */
        virtual void onRenderEnd(const WidgetEventArgs * arg);

        /** 删除时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(const WidgetEventArgs * arg);

        /** 附加单元风格时触发
        @version NIIEngine 3.0.0
        */
        virtual void onViewAttach(const WidgetEventArgs * arg);

        /** 解除单元风格时触发
        @version NIIEngine 3.0.0
        */
        virtual void onViewDetach(const WidgetEventArgs * arg);

        /** 画盘触发
        @version NIIEngine 3.0.0
        */
        virtual void onSheet(const SheetEventArgs * arg);

        /** 活动状态时触发
        @version NIIEngine 3.0.0
        */
        virtual void onActivate(const ActivationEventArgs * arg);

        /** 非活动状态时触发
        @version NIIEngine 3.0.0
        */
        virtual void onInactivate(const ActivationEventArgs * arg);

        /** 附加风格时触发
        @version NIIEngine 3.0.0
        */
        virtual void onStyleAttach(const StyleEventArgs * arg);

        /** 解除风格时触发
        @version NIIEngine 3.0.0
        */
        virtual void onStyleDetach(const StyleEventArgs * arg);

        /** 字体发生改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onFont(const FontEventArgs * arg);

        /** 文本发生改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onText(const WidgetEventArgs * arg);

        /** Bidi发生改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onTextLayoutMode(const WidgetEventArgs * arg);

        /** 游标进入时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorEnter(const CursorEventArgs * arg);

        /** 游标离开后触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorLeave(const CursorEventArgs * arg);

        /** 游标移动时触发
        @remark 游标进入时
        @version NIIEngine 3.0.0
        */
        virtual void onCursorMove(const CursorEventArgs * arg);

        /** 游标拖拽触发
        @rmark 按下状态/进入状态
        @version NIIEngine 3.0.0
        */
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /** 按键按下时触发
        @version NIIEngine 3.0.0
        */
        virtual void onButtonDown(const CursorEventArgs * arg);

        /** 按键释放时触发
        @version NIIEngine 3.0.0
        */
        virtual void onButtonUp(const CursorEventArgs * arg);

        /** 游标按下时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorTouch(const CursorEventArgs * arg);

        /** 游标多重按下时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorMultiTouch(const CursorEventArgs * arg);

        /** 键盘按下时触发
        @param arg
        @version NIIEngine 3.0.0
        */
        virtual void onKeyDown(const KeyEventArgs * arg);

        /** 键盘释放时触发
        @param arg
        @version NIIEngine 3.0.0
        */
        virtual void onKeyUp(const KeyEventArgs * arg);

        /** 文本输入时触发
        @param arg
        @version NIIEngine 3.0.0
        */
        virtual void onChar(const KeyEventArgs * arg);
    protected:
        Widget(const Widget &);

        Widget & operator=(const Widget &);
    protected:
        typedef NII::set<PropertyID>::type NonSerlProperty;
    protected:
        Container * mParent;
        String mText;
        WidgetID mID;
        String mName;
        FactoryID mType;
        StyleID mStyleID;                   ///< UI风格ID
        StyleViewComID mViewID;
        WidgetView * mWidgetView;
        GeometryPixel * mPixelData;
        ColourArea mForegroundColour;
        RelRectf mArea;
        NIIf mAlpha;
        RefreshMode mRefreshMode;
        Font * mFont;
        PixelBuffer * mImage;
        BidiText * mBidiText;
        TextView * mTextView;
        Widget * mPreFocus;
        HAlign mHAlign;
        VAlign mVAlign;
        RelRectf mMargin;
        RelPlaneSizef mMinSize;
        RelPlaneSizef mMaxSize;
        AspectMode mScaleMode;              ///< 非匹配大小时的拉伸模式
        NIIf mScaleFactor;                  ///< 非匹配大小时的拉伸因子
        PlaneSizef mPixelSize;              ///< 像素大小
        Quaternion mRot;
        CalcRect mRect;                     ///< 区域大小(屏幕位置)
        CalcRect mInnerRect;                ///< 内区域大小(屏幕位置)
        mutable PixelUnitGrid mRenderText;
        NonSerlProperty mNonSerlProperty;
        WidgetAideList mAideList;
        CursorWidgetAide * mAide1;
        TooltipWidgetAide * mAide2;
        KeyboardControlItem * mC0;
        MouseControlItem * mC1;
        TouchControlItem * mC2;
        UISheet * mSheet;                   ///< 所在UI窗体画盘
        Nmark mSpaceMark;
        TimeDurMS mClickRate;               ///< 引发自动事件条件达成后,自动引发的时间
        TimeDurMS mClickThreshold;          ///< 引发自动事件条件需要的时间阀
        mutable Rectf mClipRect;            ///< 裁减区域大小(屏幕位置)
        mutable Rectf mClipInnerRect;       ///< 内裁减区域大小(屏幕位置)
        mutable Rectf mIntersectRect;       ///< 相交测试区域(屏幕位置)
        String mTipsText;
        bool mParentTipsText;
        bool mInit;
        bool mDestroy;
        bool mRedraw;                       ///< 是否需要重新计算/绘制几何
        bool mMultiClickEventEnable;        ///< 是否监听双击鼠标事件
        bool mCursorSubmitEvent;            ///< 上传游标事件
        bool mClickAutoEvent;               ///< 是否自动重复生成鼠标按下事件
        mutable bool mValidClipRect;        ///< 裁减区域大小(是否)有效
        mutable bool mValidClipInnerRect;   ///< 内裁减区域大小(是否)有效
        mutable bool mValidIntersectRect;   ///< 相交测试区域(是否)有效
        bool mZOrder;                       ///< 层次可以改变
        bool mZOrderClick;                  ///< 点击事件能改变这个窗体的层次
        bool mRestorePreFocus;              ///< 是否存储前一个被对焦的对象
        bool mTop;                          ///< 在最顶层
        bool mPixelAlign;                   ///< 像素整数化
        bool mAutoGen;                      ///< 系统自动创建的元素
        bool mEnable;                       ///< 是否启用
        bool mVisible;                      ///< 是否可见
        bool mActive;                       ///< 是否活动中
        bool mEnvAlpha;                     ///< 是否从父对象中继承透明度
        bool mDataWrite;                    ///< 序列化数据
        bool mAutoDestroy;                  ///< 是否自动删除
        bool mAutoClip;                     ///< 自动裁剪
        bool mTextLayout;
        mutable bool mValidText;
        mutable bool mValidBidi;
    };
    typedef vector<Widget *>::type Widgets;
}
}
#endif