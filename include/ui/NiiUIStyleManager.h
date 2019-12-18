/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-8-26

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
	  三维应用软件
		地理信息系统软件(GIS)
		电影背景立体重构软件
		立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_UI_STYLEMANAGER_H_
#define _NII_UI_STYLEMANAGER_H_

#include "NiiUIPreInclude.h"
#include "NiiSingleton.h"
#include "NiiUIException.h"
#include "NiiUIStyle.h"

namespace NII
{
    /** UI风格管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIStyleManager : public Singleton<UIStyleManager>, public UIAlloc
    {
    public:
        typedef map<StyleID, Style *>::type StyleList;
    public:
        UIStyleManager();
        ~UIStyleManager();

        /** 加载文件
        @version NIIEngine 3.0.0
        */
        void load(const String & file, GroupID gid = GroupUnknow);
        
        /** 加载内存流
        @version NIIEngine 3.0.0
        */
        void load(const MemDataStream * data);
        
        /** 添加风格
        @version NIIEngine 3.0.0
        */
        void add(Style * style);

        /** 移去风格
        @version NIIEngine 3.0.0
        */
        void remove(StyleID style);

        /** 移去所有风格
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 是否存在风格
        @version NIIEngine 3.0.0
        */
        bool isExist(StyleID style) const;

        /** 获取风格
        @version NIIEngine 3.0.0
        */
        Style * get(StyleID style) const;

        /** 
        @param[out]
        */
        void write(StyleID style, std::ostream & out) const;

        /** 
        @param[out] out 
        */
        void exportSeries(Nui16 Ser, std::ostream & out) const;

        /** 
        @version NIIEngine 3.0.0
        */
		static void setGroup(GroupID gid)
        {
            ResourceGroup = gid;
        }

        /** 
        @version NIIEngine 3.0.0
        */
		static GroupID getGroup()
        {
            return ResourceGroup;
        }
        
		const StyleList & getWidgetList() const;

       	/// @copydetails Singleton::geOnly
        static UIStyleManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static UIStyleManager * getOnlyPtr();
    private:
		static GroupID ResourceGroup;
        StyleList mStyleList;
    };
}
#endif