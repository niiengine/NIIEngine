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

#ifndef _NII_UI_WIDGET_MANAGER_H_
#define _NII_UI_WIDGET_MANAGER_H_

#include "NiiUIPreInclude.h"
#include "NiiSingleton.h"
#include "NiiUIWidget.h"
#include "NiiUIException.h"
#include "NiiUIStyle.h"
#include "NiiUIPixelUnitGrid.h"

using namespace NII::UI;

namespace NII
{
    /** UI单元管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetManager : public Singleton<WidgetManager>, public UIAlloc
    {
        friend class UIManager;
    public:
        /** 风格定义
        @version NIIEngine 3.0.0
        */
        struct _EngineAPI StyleView
        {
            String mName;
            StyleViewID mID;
            GroupID mGroup;
            FactoryID mModel;
            StyleID mStyle;
            WidgetViewlID mView;
            EffectID mEffect;
        };

        typedef map<FactoryID, WidgetViewFactory *>::type WidgetViewList;
        typedef map<FactoryID, WidgetFactory *>::type WidgetFactoryList;
        typedef map<StyleViewID, StyleView>::type StyleViewList;
        typedef map<StyleID, Style *>::type StyleList;
    public:
        WidgetManager();
        ~WidgetManager();

        /** 设置资源组
        @version NIIEngine 3.0.0
        */
        static inline void setGroup(GroupID gid)    { ResourceGroup = gid; }

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        static inline GroupID getGroup()            { return ResourceGroup; }

        /**
        @version NIIEngine 3.0.0
        */
        static inline void setStyleGroup(GroupID gid) { StyleResourceGroup = gid; }

        /**
        @version NIIEngine 3.0.0
        */
        static inline GroupID getStyleGroup()       { return StyleResourceGroup; }

        /** 创建UI单元
        @version NIIEngine 3.0.0
        */
        Widget * create(StyleViewID id, WidgetID wid = 0, Container * parent = 0, FactoryID modelhint = 0);

        /** 创建UI单元
        @version NIIEngine 3.0.0
        */
        Widget * create(GroupID gid, FactoryID mid, WidgetViewlID vid, WidgetID wid = 0, Container * parent = 0);
        
        /** 是否存在UI单元
        @version NIIEngine 3.0.0
        */
        bool isExist(const Widget * obj) const;

        /** 是否存在UI单元
        @version NIIEngine 3.0.0
        */
        bool isExist(const String & name, bool path = false) const;

        /** 删除UI单元
        @version NIIEngine 3.0.0
        */
        void destroy(Widget * obj);

        /** 删除UI单元
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 添加工厂类
        @version NIIEngine 3.0.0
        */
        bool addFactory(WidgetFactory * obj);

        /** 移去工厂类
        @version NIIEngine 3.0.0
        */
        void removeFactory(FactoryID fid);

        /** 获取工厂类
        @version NIIEngine 3.0.0
        */
        WidgetFactory * getFactory(FactoryID fid) const;

        /** 工厂类是否存在
        @version NIIEngine 3.0.0
        */
        bool isFactoryExist(FactoryID fid) const;

        /** 移去所有工厂类
        @version NIIEngine 3.0.0
        */
        void removeAllFactory();

        /** 获取工厂列表
        @version NIIEngine 3.0.0 高级 api
        */
        const WidgetFactoryList & getWidgetFactoryList() const  { return mWidgetFactoryList;  }

        /** 添加UI模型工厂
        @version NIIEngine 3.0.0
        */
        bool addViewFactory(WidgetViewFactory * obj);

        /** 移去UI模型工厂
        @version NIIEngine 3.0.0
        */
        void removeViewFactory(FactoryID fid);

        /** 获取UI模型工厂
        @version NIIEngine 3.0.0
        */
        WidgetViewFactory * getViewFactory(FactoryID fid) const;

        /** 是否存在UI模型工厂
        @version NIIEngine 3.0.0
        */
        bool isViewFactoryExist(FactoryID fid) const;

        /** 加载文件
        @version NIIEngine 3.0.0
        */
        void loadStyle(const String & file, GroupID gid = GroupUnknow);

        /** 加载内存流
        @version NIIEngine 3.0.0
        */
        void loadStyle(const MemStream * data);

        /** 添加风格
        @version NIIEngine 3.0.0
        */
        void addStyle(Style * style);

        /** 移去风格
        @version NIIEngine 3.0.0
        */
        void removeStyle(StyleID style);

        /** 移去所有风格
        @version NIIEngine 3.0.0
        */
        void removeAllStyle();

        /** 是否存在风格
        @version NIIEngine 3.0.0
        */
        bool isStyleExist(StyleID style) const;

        /** 获取风格
        @version NIIEngine 3.0.0
        */
        Style * getStyle(StyleID style) const;

        /** 获取风格列表
        @version NIIEngine 3.0.0
        */
        inline const StyleList & getStyleList() const           { return mStyleList; }

        /** 创建UI视图
        @version NIIEngine 3.0.0
        */
        WidgetView * createView(FactoryID wid);

        /** 删除UI视图
        @version NIIEngine 3.0.0
        */
        void destroyView(WidgetView * obj);

        /** 添加风格定义
        @version NIIEngine 3.0.0
        */
        void addStyleView(StyleViewID id, FactoryID mid, WidgetViewlID vid, StyleID sid, EffectID eid, const String & name = N_StrBlank);

        /** 风格定义是否存在
        @version NIIEngine 3.0.0
        */
        bool isStyleViewExist(FactoryID mid, WidgetViewlID vid, StyleID sid) const;

        /** 风格定义是否存在
        @version NIIEngine 3.0.0
        */
        bool isStyleViewExist(StyleViewID fid) const;

        /** 获取风格定义
        @version NIIEngine 3.0.0
        */
        const StyleView * getStyleView(FactoryID mid, WidgetViewlID vid, StyleID sid) const;

        /** 获取风格定义
        @version NIIEngine 3.0.0
        */
        const StyleView * getStyleView(StyleViewID fid) const;

        /** 移去风格定义
        @version NIIEngine 3.0.0
        */
        void removeStyleView(StyleViewID fid);

        /** 移去所有风格定义
        @version NIIEngine 3.0.0
        */
        void removeAllStyleView();

        /** 获取风格列表
        @version NIIEngine 3.0.0
        */
        const StyleViewList & getStyleViewList() const          { return mStyleViewList; }

        /** 加载
        @note xml级
        @version NIIEngine 3.0.0
        */
        Widget * load(const String & file, GroupID gid = GroupUnknow);

        /** 加载
        @note xml级
        @version NIIEngine 3.0.0
        */
        Widget * load(const MemStream * source);

        /** 获取列表
        @version NIIEngine 3.0.0 高级api
        */
        const Widgets & getList() const                         { return mCreateWidgets; }

        /** 写入UI单元
        @param[in] out 输出流
        @note xml级
        @version NIIEngine 3.0.0
        */
        void writeWidget(const Widget * obj, Nostream & out) const;

        /** 写入UI单元
        @param[in] file 文件名
        @note xml级
        @version NIIEngine 3.0.0
        */
        void writeWidget(const Widget * obj, const String & file) const;

        /** 写入UI风格
        @param[out]
        @version NIIEngine 3.0.0
        */
        void writeStyle(StyleID style, Nostream & out) const;

        /** 写入UI风格
        @param[out] out
        @version NIIEngine 3.0.0
        */
        void writeStyleList(Nui16 Ser, Nostream & out) const;

        /** 回收
        @version NIIEngine 3.0.0
        */
        void recover();

        /** 获取未使用的ID
        @version NIIEngine 3.0.0 高级api
        */
        WidgetID genValidWidgetID();

        /** 获取未使用的ID
        @version NIIEngine 3.0.0 高级api
        */
        StyleViewID genValidStyleViewID();
    public:
        /** 公共Tooltip
        @version NIIEngine 3.0.0
        */
        static TooltipWidgetAide * mTooltip;
        
        /** 公共Cursor
        @version NIIEngine 3.0.0
        */
        static CursorWidgetAide * mCursor;
    protected:
        /** 创建单元时
        @version NIIEngine 3.0.0 高级api
        */
        void onCreate(const Widget * obj);
        
        /** 删除单元时
        @version NIIEngine 3.0.0 高级api
        */
        void onDestroy(const Widget * obj);
    public:
        static const Colour DefaultTextColour;
        static const Colour DefaultSelectionColour;

        static EffectTextView * gDefaultTextView;
        static TextView * gNormalTextView;
    private:
        static GroupID StyleResourceGroup;
        StyleList mStyleList;
        Widgets mCreateWidgets;
        Widgets mDestroyWidgets;
        WidgetFactoryList mWidgetFactoryList;
        StyleViewList mStyleViewList;
        WidgetViewList mWidgetViewList;
        WidgetID mValidWidgetID;
        WidgetID mValidStyleViewID;
        static GroupID ResourceGroup;
    };
}
#endif