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
    /** �ű���ʶ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptHelperManager : public Singleton<ScriptHelperManager>,
        public ScriptAlloc
    {
    public:
        ScriptHelperManager();
        ~ScriptHelperManager();

        /** ���ָ���ű��������԰汾
        @param[in] sid
        @param[in] sl
        @version NIIEngine 3.0.0
        */
        void add(ScriptTypeID sid, ScriptLangMap * sl);

        /** ��ȡָ���ű������������԰汾
        @param[in] sid
        @param[in] lid
        @version NIIEngine 3.0.0
        */
        ScriptLangMap * get(ScriptTypeID sid, LangID lid) const;

        /** ��ȥָ���ű�����ָ�����԰汾
        @param[in] sid
        @version NIIEngine 3.0.0
        */
        void remove(ScriptTypeID sid, LangID lid);

        /** ��ȥָ���ű������������԰汾
        @param[in] sid
        @version NIIEngine 3.0.0
        */
        void remove(ScriptTypeID sid);

        /** ��ȥ�ű������������԰汾
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** ��ӽű�����ӳ��
        @param[in] lid
        @param[in] sid
        @param[in] m
        @version NIIEngine 3.0.0
        */
        void addUnit(LangID lid, ScriptTypeID sid, const String & m);

        /** ��ȥ�ű�����ӳ��
        @param[in] lid
        @param[in] sid
        @version NIIEngine 3.0.0
        */
        void removeUnit(LangID lid, ScriptTypeID sid);

        /** ��ȡ�ű�����ӳ��
        @param[in] lid
        @param[in] m
        @version NIIEngine 3.0.0
        */
        ScriptTypeID getUnit(LangID lid, const String & m) const;

        /** ��ȡ�ű�����ӳ��
        @param[in] lid
        @version NIIEngine 3.0.0
        */
        const String & getUnit(LangID lid, ScriptTypeID tid) const;

        /** ��ȥ���нű�ӳ��
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

    /** �ű��﷨ӳ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptLangMap
    {
    public:
        ScriptLangMap(LangID lid);
        ~ScriptLangMap();

        /** ���ӳ��
        @param[in] pid
        @param[in] token
        @version NIIEngine 3.0.0
        */
        void add(PropertyID pid, const String & token);

        /** ��ȥӳ��
        @param[in] pid
        @version NIIEngine 3.0.0
        */
        void remove(PropertyID pid);

        /** ��ȡӳ��
        @param[in] name
        @note name->pid
        @version NIIEngine 3.0.0
        */
        PropertyID get(const String & name) const;

        /** ��ȡӳ��
        @param[in] pid
        @note pid->name
        @version NIIEngine 3.0.0
        */
        const String & get(PropertyID pid) const;

        /** ��ȡ��������
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