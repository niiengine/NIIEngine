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

#ifndef _NII_SCRIPT_PROPERTY_H_
#define _NII_SCRIPT_PROPERTY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 文本级脚本属性
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptProperty
    {
    public:
        ScriptProperty(ScriptTypeID stid, LangID lid = N_PrimaryLang);
        virtual ~ScriptProperty();
        
        /** 获取实际的属性
        @param name 节点文本名字
        @return 节点文本按具体语言版本解析后的属性
        @version NIIEngine 3.0.0
        */
        PropertyID getProperty(const String & name) const;

        /** 获取实际属性文本
        @param[in] pid 属性
        @version NIIEngine 3.0.0
        */
        const String & getLang(PropertyID pid) const;

        /** 获取语言类型
        @version NIIEngine 3.0.0
        */
        LangID getLangID() const;
    public:
        /** 标签中的元素(恒值)
        @version NIIEngine 3.0.0
        */
        static const PropertyID PropertyDefine;

        /** id/标识号码 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const PropertyID IDProperty;

        /** name/名称 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const PropertyID NameProperty;

        /** gid/群组 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const PropertyID GroupProperty;

        /** type/类型 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const PropertyID TypeProperty;

        /** src/来源 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const PropertyID SourceProperty;

        /** value/值 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const PropertyID ValueProperty;

        /** ver/版本 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const PropertyID VersionProperty;
        
        /** true 标签中的元素(恒值)
        @version NIIEngine 3.0.0
        */
        static const PropertyID TrueProperty;
        
        /** false 标签中的元素(恒值)
        @version NIIEngine 3.0.0
        */
        static const PropertyID FalseProperty;
        
        /** 此类的标签中的个数
        @version NIIEngine 3.0.0
        */
        static const PropertyID PropertyCount;
    protected:
        ScriptLangMap * mLangMap;
        ScriptTypeID mScriptTypeID;
    };
}

#endif