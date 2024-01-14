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

#ifndef _NII_SCRIPT_HELPER_MANAGER_H_
#define _NII_SCRIPT_HELPER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"

namespace NII
{
    /** 脚本标识辅助类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptHelperManager : public Singleton<ScriptHelperManager>,
        public ScriptAlloc
    {
    public:
        ScriptHelperManager();
        ~ScriptHelperManager();

        /** 添加指定脚本类型语言版本
        @param[in] sid
        @param[in] sl
        @version NIIEngine 3.0.0
        */
        void add(ScriptTypeID sid, ScriptLangMap * sl);

        /** 获取指定脚本类型所有语言版本
        @param[in] sid
        @param[in] lid
        @version NIIEngine 3.0.0
        */
        ScriptLangMap * get(ScriptTypeID sid, LangID lid) const;

        /** 移去指定脚本类型指定语言版本
        @param[in] sid
        @version NIIEngine 3.0.0
        */
        void remove(ScriptTypeID sid, LangID lid);

        /** 移去指定脚本类型所有语言版本
        @param[in] sid
        @version NIIEngine 3.0.0
        */
        void remove(ScriptTypeID sid);

        /** 移去脚本类型所有语言版本
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 添加脚本类型映射
        @param[in] lid
        @param[in] sid
        @param[in] m
        @version NIIEngine 3.0.0
        */
        void addUnit(LangID lid, ScriptTypeID sid, const String & m);

        /** 移去脚本类型映射
        @param[in] lid
        @param[in] sid
        @version NIIEngine 3.0.0
        */
        void removeUnit(LangID lid, ScriptTypeID sid);

        /** 获取脚本类型映射
        @param[in] lid
        @param[in] m
        @version NIIEngine 3.0.0
        */
        ScriptTypeID getUnit(LangID lid, const String & m) const;

        /** 获取脚本类型映射
        @param[in] lid
        @version NIIEngine 3.0.0
        */
        const String & getUnit(LangID lid, ScriptTypeID tid) const;

        /** 移去所有脚本映射
        @version NIIEngine 3.0.0
        */
        void removeAllUnit();
    public:
        static const PropertyID UnitPropertyStart;
    private:
        typedef map<LangID, ScriptLangMap *>::type LangList;
        typedef map<ScriptTypeID, LangList>::type ScriptLangs;

        typedef map<String, ScriptTypeID>::type TokenList;
        typedef map<LangID, TokenList>::type TokenLangs;

        ScriptLangs mScriptLangs;
        TokenLangs mTokenLangs;
    };

    /** 脚本语法映射
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptLangMap
    {
    public:
        ScriptLangMap(LangID lid);
        ~ScriptLangMap();

        /** 添加映射
        @param[in] pid
        @param[in] token
        @version NIIEngine 3.0.0
        */
        void add(PropertyID pid, const String & token);

        /** 移去映射
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        void remove(PropertyID pid);

        /** 获取映射
        @param[in] name
        @note name->pid
        @version NIIEngine 3.0.0
        */
        PropertyID get(const String & name) const;

        /** 获取映射
        @param[in] pid
        @note pid->name
        @version NIIEngine 3.0.0
        */
        const String & get(PropertyID pid) const;

        /** 获取语言类型
        @version NIIEngine 3.0.0
        */
        inline LangID getLangID() const                 { return mLangID;  }
    protected:
        ScriptLangMap();
    private:
        typedef HashMap<String, PropertyID> PropertyMap;
    private:
        PropertyMap mPropertyMap;
        LangID mLangID;
    };
}
#endif