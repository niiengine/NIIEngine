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

#ifndef _NII_ScriptAnalyzeFactory_H_
#define _NII_ScriptAnalyzeFactory_H_

#include "NiiPreInclude.h"

#define N_ScriptAnalyzeFactory(c, id) PreDefineScriptAnalyze<c>(id)

namespace NII
{
    /** UI单元工厂类
    @versioin NIIEngine 3.0.0
    */
    class _EngineAPI ScriptAnalyzeFactory : public ScriptAlloc
    {
    public:
        ScriptAnalyzeFactory(){}
        virtual ~ScriptAnalyzeFactory(){}

        /** 获取工厂ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;
        
        /** 创建对象
        @version NIIEngine 3.0.0
        */
        virtual ScriptAnalyze * create(ScriptCompiler * obj, LangID lid) = 0;

        /** 删除对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(ScriptAnalyze * obj) = 0;
    };

    /** 预定制UI单元工厂类
    @version NIIEngine 3.0.0
    */
    template <typename T> class PreDefineScriptAnalyzeFactory : public ScriptAnalyzeFactory
    {
    public:
        PreDefineScriptAnalyzeFactory(FactoryID fid);

        /// @copydetails ScriptAnalyzeFactory::getID
        FactoryID getID() const;
        
        /// @copydetails ScriptAnalyzeFactory::create
        ScriptAnalyze * create(ScriptCompiler * obj, LangID lid);

        /// @copydetails ScriptAnalyzeFactory::destroy
        void destroy(ScriptAnalyze * obj);

        /** 注入到系统中
        @version NIIEngine 3.0.0
        */
        static void plugin();

        /** 从系统中移出
        @version NIIEngine 3.0.0
        */
        static void unplugin();
    protected:
        FactoryID mID;
    };
    //------------------------------------------------------------------------
    template <typename T> PreDefineScriptAnalyzeFactory<T>::PreDefineScriptAnalyzeFactory(FactoryID fid) :
        mID(fid)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> FactoryID PreDefineScriptAnalyzeFactory<T>::getID() const
    {
        return mID;
    }
    //------------------------------------------------------------------------
    template <typename T> ScriptAnalyze * PreDefineScriptAnalyzeFactory<T>::create(ScriptCompiler * obj, LangID lid)
    {
        return N_new T(obj, lid);
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineScriptAnalyzeFactory<T>::destroy(ScriptAnalyze * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineScriptAnalyzeFactory<T>::plugin()
    {
        if(N_OnlyPtr(ScriptAnalyze))
        {
            ScriptAnalyzeFactory * factory = N_new PreDefineScriptAnalyzeFactory<T>(mID);            
            if(N_Only(ScriptAnalyze).addFactory(factory) != true)
                N_delete factory;
        }
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineScriptAnalyzeFactory<T>::unplugin()
    {
        if(N_OnlyPtr(ScriptAnalyze))
        {
            ScriptAnalyzeFactory * factory = N_Only(ScriptAnalyze).getFactory(mID);
            if(factory != 0)
            {
                N_Only(ScriptAnalyze).removeFactory(mID);
                N_delete factory;
            }
        }
    }
    //------------------------------------------------------------------------
}
#endif