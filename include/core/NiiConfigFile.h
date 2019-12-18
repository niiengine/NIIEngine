/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_ConfigFile_H_
#define _NII_ConfigFile_H_

#include "NiiPreInclude.h"
#include "NiiString.h"
#include "NiiDataStream.h"

namespace NII
{
    /** 加载.cfg文件
    @note 冒号分隔符(:)或等于(=)字符.每键每值必须用一个回车分割.
    @note 在头部加上[SectionName]可分组
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ConfigFile : public ConfigAlloc
    {
    public:
        typedef multimap<String, String>::type SetList;
        typedef map<String, SetList *>::type SectionSetList;
    public:
        ConfigFile();
        virtual ~ConfigFile();
            
        /** 从文件中加载
        @version NIIEngine 3.0.0
        */
        void load(const String & filename, const String & sep = _T("\t:="), bool trim = true);
            
        /** 从文件中加载
        @version NIIEngine 3.0.0
        */
        void load(const String & filename, GroupID gid, const String & sep = _T("\t:="), bool trim = true);

        /** 从数据流中中加载
        @version NIIEngine 3.0.0
        */
        void load(const DataStream * stream, const String & sep = _T("\t:="), bool trim = true);

        /** 获取第一个设置
        @param[in] key 设置的名字
        @param[in] section 部件的名字,它必须在 (如果有的话)
        @param[in] init 没有找到时,返回的值
        @version NIIEngine 3.0.0
        */
        String getValue(const String & key, const String & section = StrUtil::WBLANK, const String & init = StrUtil::WBLANK) const;

        /** 获取多个设置
        @version NIIEngine 3.0.0
        */
        StringList getValues(const String & key, const String & section = StrUtil::WBLANK) const;

        /** 获取在区域中的多个设置
        @version NIIEngine 3.0.0
        */
        const SetList & getValueList(const String & section = StrUtil::WBLANK);

        /** 获取设置列表
        @version NIIEngine 3.0.0
        */
        const SectionSetList & getSectionList() const;

        /** 清除设置
        @version NIIEngine 3.0.0
        */
        void clear(void);
    protected:
        SectionSetList mSettings;
    };
}
#endif