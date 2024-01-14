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

#ifndef _NII_Script_Analyze_H_
#define _NII_Script_Analyze_H_

#include "NiiPreInclude.h"
#include "NiiScriptCompiler.h"
#include "NiiScriptProperty.h"

namespace NII
{
    /** 脚本分析类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptAnalyze : public ScriptProperty, public ScriptAlloc
    {
        friend class ScriptCompilerManager;
    public:
        ScriptAnalyze(ScriptCompiler * cpl, ScriptTypeID stid, LangID lid);
        virtual ~ScriptAnalyze();

        /** 使用通用方法分析节点
        @version NIIEngine 3.0.0
        */
        virtual void analyze(const ScriptNode * in);

        /** 使用子系方法分析节点
        @version NIIEngine 3.0.0
        */
        virtual bool analyzeCom(const ScriptNode * in);

        /** 获取一个布尔
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNode * in, bool & out);

        /** 获取一个整形
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNode * in, Ni32 & out);

        /** 获取一个无符号整形
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNode * in, Nui32 & out);

        /** 获取一个浮点
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNode * in, NIIf & out);

        /** 获取一个双精浮点
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNode * in, NIId & out);

        /** 获取一个字符串
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNode * in, String & out);

        /** 获取一个字符串
        @param[in] oft 数组偏移
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, String & out);
        
        /** 获取一个整形
        @param[in] oft 数组偏移
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, bool & out);
        
        /** 获取一个整形
        @param[in] oft 数组偏移
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, Ni32 & out);

        /** 获取一个整形
        @param[in] oft 数组偏移
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, Nui32 & out);

        /** 获取一个浮点
        @param[in] oft 数组偏移
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, NIIf & out);

        /** 获取一个双精浮点
        @param[in] oft 数组偏移
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, NIId & out);

        /** 获取一个颜色
        @param[in] oft 数组偏移
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, Colour & out);

        /** 获取一个距阵
        @param[in] oft 数组偏移
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, Matrix4f & out);

        /** 获取多个字符串
        @param[in] oft 数组偏移
        @param[in] cnt 个数
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, NCount cnt, StringList & out);

        /** 获取多个整形
        @param[in] oft 数组偏移
        @param[in] cnt 个数
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, NCount cnt, Ni32 * out);

        /** 获取多个整形
        @param[in] oft 数组偏移
        @param[in] cnt 个数
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, NCount cnt, Nui32 * out);

        /** 获取多个浮点
        @param[in] oft 数组偏移
        @param[in] cnt 个数
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, NCount cnt, NIIf * out);

        /** 获取多个双精浮点
        @param[in] oft 数组偏移
        @param[in] cnt 个数
        @version NIIEngine 3.0.0
        */
        static bool getValue(const ScriptNodeList & in, NCount oft, NCount cnt, NIId * out);

        /** 获取节点
        @version NIIEngine 3.0.0
        */
        static const ScriptNode * getNode(const ScriptNodeList & nodes, Nidx index);

        /** 获取节点迭代
        @version NIIEngine 3.0.0
        */
        static ScriptNodeList::const_iterator getIterator(const ScriptNodeList & nodes, Nidx index);

        /** 获取分析成分ID
        @note 子类必须实现
        @version NIIEngine 3.0.0
        */
        virtual ScriptUnitID getUnit() = 0;

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        virtual GroupID getGroup();
    protected:
        ScriptCompiler * mCompiler;
    };
}
#endif