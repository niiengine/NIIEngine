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

#ifndef _NII_ScriptSys_H_
#define _NII_ScriptSys_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"
#include "NiiDataStream.h"
#include "NiiString.h"

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ProgramScriptLanguage : public ScriptAlloc
    {
    public:
        ProgramScriptLanguage();
        virtual ~ProgramScriptLanguage();
    };

    /** 能执行托管编程语言(高阶编程语言)
    @remark python lua javascript
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ProgramScriptSys : public ScriptAlloc
    {
    public:
        ProgramScriptSys();
        virtual ~ProgramScriptSys();

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void init();

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void shutdown();

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void add(ProgramScriptLanguage * engine);

        /** 执行本地脚本文件
        @remark
        @param[in] file 文件名字
        @param[in] gid 所属的资源组
        @version NIIEngine 3.0.0
        */
        void excute(const String & file, GroupID gid);

        /** 执行代码
        @param[in] function 函数名字
        @version NIIEngine 3.0.0
        */
        bool excute(const String & code);

        /**
        @param[in] hander
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        int excute(const String & hander, const EventArgs & arg);
    };

    /** 译成属性数据
    @remark json xml
    @note
        引擎默认配备了一种文件序列格式,这种序列是复合结构出现的,还配备了一种结构很像
        json的基础脚本.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyScriptSys
    {
    public:
        virtual ~PropertyScriptSys();

        /** 解析
        @param[in] stream
        @param[in] gid
        @param[in] out
        @version NIIEngine 3.0.0
        */
        virtual void parse(DataStream * stream, GroupID gid, PtrList & out);

        /** 获取
        @version NIIEngine 3.0.0
        */
        virtual NIIf getScriptLevel() const;

        /** 添加脚本文件匹配
        @version NIIEngine 3.0.0
        */
        void addScriptPattern(const String & pattern);

        /** 获取脚本文件匹配
        @version NIIEngine 3.0.0
        */
        virtual const StringList & getScriptPattern() const;
    protected:
        Serializer * mSerializer;
        NIIf mScriptLevel;
        StringList mScriptPattern;
    };
}
#endif