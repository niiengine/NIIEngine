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

        /** 获取设置(第一个)
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
        const SetList & getValueList(const String & section = StrUtil::WBLANK) const;

        /** 获取设置列表
        @version NIIEngine 3.0.0
        */
        const SectionSetList & getSectionList() const;

        /** 清除设置
        @version NIIEngine 3.0.0
        */
        void clear();
    protected:
        SectionSetList mSettings;
    };
}
#endif