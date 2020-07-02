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
#include "NiiUIWidgetView.h"
#include "NiiUIWidgetFactory.h"
#include "NiiUIWidgetModelFactory.h"

using namespace NII::UI;

namespace NII
{
    /** UI单元管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIWidgetManager : public Singleton<UIWidgetManager>, public UIAlloc
    {
        friend class UIManager;
    public:
        /** 风格定义
        @version NIIEngine 3.0.0
        */
        struct _EngineAPI WidgetModelDefine
        {
            StyleID mStyle;
            EffectID mEffect;
            FactoryID mFactory;
            WidgetModelID mWidgetModel;
        };

        typedef map<FactoryID, WidgetModelFactory *>::type WidgetModelList;
        typedef    map<FactoryID, WidgetFactory *>::type WidgetFactoryList;
        typedef map<FactoryID, WidgetModelDefine>::type StyleDefineList;
    public:
        UIWidgetManager();
        ~UIWidgetManager();

        /** 设置资源组
        @version NIIEngine 3.0.0
        */
        static void setGroup(GroupID gid);

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        static GroupID getGroup();

        /** 创建UI单元
        @version NIIEngine 3.0.0
        */
        Widget * create(FactoryID sid, WidgetID wid = 0, Container * parent = 0);
        
        /** 是否存在UI单元
        @version NIIEngine 3.0.0
        */
        bool isExist(const Widget * obj) const;

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
        const WidgetFactoryList & getWidgetFactoryList() const;

        /** 获取风格列表
        @version NIIEngine 3.0.0
        */
        const StyleDefineList & getStyleDefineList() const;

        /** 添加UI模型工厂
        @version NIIEngine 3.0.0
        */
        bool addModelFactory(WidgetModelFactory * obj);

        /** 移去UI模型工厂
        @version NIIEngine 3.0.0
        */
        void removeModelFactory(FactoryID fid);

        /** 获取UI模型工厂
        @version NIIEngine 3.0.0
        */
        WidgetModelFactory * getModelFactory(FactoryID fid) const;

        /** 是否存在UI模型工厂
        @version NIIEngine 3.0.0
        */
        bool isModelFactoryExist(FactoryID fid) const;

        /** 创建UI模型
        @version NIIEngine 3.0.0
        */
        WidgetView * createModel(FactoryID wid);

        /** 删除UI模型
        @version NIIEngine 3.0.0
        */
        void destroyModel(WidgetView * obj);

        /** 添加风格定义
        @version NIIEngine 3.0.0
        */
        void addStyle(FactoryID fid, StyleID sid, WidgetModelID wsid, EffectID eid);

        /** 风格定义是否存在
        @version NIIEngine 3.0.0
        */
        bool isStyleExist(FactoryID fid) const;

        /** 获取风格定义
        @version NIIEngine 3.0.0
        */
        const WidgetModelDefine & getStyle(FactoryID fid) const;

        /** 移去风格定义
        @version NIIEngine 3.0.0
        */
        void removeStyle(FactoryID fid);

        /** 移去所有风格定义
        @version NIIEngine 3.0.0
        */
        void removeAllStyle();

        /** 加载
        @note xml级
        @version NIIEngine 3.0.0
        */
        Widget * load(const String & file, GroupID gid = GroupUnknow);

        /** 加载
        @note xml级
        @version NIIEngine 3.0.0
        */
        Widget * load(const MemDataStream * source);

        /** 保存
        @param[in] out 输出流
        @note xml级
        @version NIIEngine 3.0.0
        */
        void save(const Widget & obj, Nostream & out) const;

        /** 保存
        @param[in] file 文件名
        @note xml级
        @version NIIEngine 3.0.0
        */
        void save(const Widget & obj, const String & file) const;

        /** 回收
        @version NIIEngine 3.0.0
        */
        void recover();

        /** 获取未使用的ID
        @version NIIEngine 3.0.0 高级api
        */
        WidgetID genValidID();

        /// @copydetails Singleton::getOnly
        static UIWidgetManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static UIWidgetManager * getOnlyPtr();
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
    private:
        Widgets mCreateWidgets;
        Widgets mDestroyWidgets;
        WidgetFactoryList mWidgetFactoryList;
        StyleDefineList mStyleDefineList;
        WidgetModelList mWidgetModelList;
        WidgetID mValidWidgetID;
        static GroupID ResourceGroup;
    };
}
#endif