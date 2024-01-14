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
#include "NiiPreProcess.h"
#include "NiiScriptCompilerManager.h"
#include "NiiResourceSchemeManager.h"
#include "NiiScriptAnalyzeFactory.h"
#include "NiiBuiltinScriptAnalyze.h"
#include "NiiScriptAnalyzeList.h"
#include "NiiDefaultStream.h"
namespace NII
{
    //-----------------------------------------------------------------------
    template<> ScriptCompilerManager * Singleton<ScriptCompilerManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    /*ScriptCompilerManager * ScriptCompilerManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    ScriptCompilerManager & ScriptCompilerManager::getOnly()
    {
        N_assert(mOnly, "error");
        return(*mOnly);
    }*/
    //-----------------------------------------------------------------------
    ScriptCompilerManager::ScriptCompilerManager()
    {
#if 1 == 1
        mScriptPattern.push_back(_T("*.program"));
        mScriptPattern.push_back(_T("*.material"));
        mScriptPattern.push_back(_T("*.particle"));
        mScriptPattern.push_back(_T("*.compositor"));
#endif
        mScriptPattern.push_back(_T("*.os"));

        ScriptError::mErrorMap.push_back(_T("script error"));
        ScriptError::mErrorMap.push_back(_T("string error"));
        ScriptError::mErrorMap.push_back(_T("number error"));
        ScriptError::mErrorMap.push_back(_T("variable error"));
        ScriptError::mErrorMap.push_back(_T("undefined error"));
        ScriptError::mErrorMap.push_back(_T("obj id error"));
        ScriptError::mErrorMap.push_back(_T("unique object error"));
        ScriptError::mErrorMap.push_back(_T("param count error"));
        ScriptError::mErrorMap.push_back(_T("invalid param error"));
        ScriptError::mErrorMap.push_back(_T("obj exist error"));

        N_Only(ResourceScheme).add(this);
        mScriptCompiler = N_new ScriptCompiler();
        addFactory(N_new PreDefineScriptAnalyzeFactory<MaterialScriptAnalyze>(N_Script_Unit_Material));
        addFactory(N_new PreDefineScriptAnalyzeFactory<GpuProgramScriptAnalyze>(N_Script_Unit_GpuProgramVP));
        addFactory(N_new PreDefineScriptAnalyzeFactory<GpuProgramScriptAnalyze>(N_Script_Unit_GpuProgramFP));
        addFactory(N_new PreDefineScriptAnalyzeFactory<GpuProgramScriptAnalyze>(N_Script_Unit_GpuProgramGP));
        addFactory(N_new PreDefineScriptAnalyzeFactory<GpuProgramParamScriptAnalyze>(N_Script_Unit_GpuProgramParam));
        addFactory(N_new PreDefineScriptAnalyzeFactory<ParticleSpaceScriptAnalyze>(N_Script_Unit_ParticleSpace));
        addFactory(N_new PreDefineScriptAnalyzeFactory<SpreadParticleScriptAnalyze>(N_Script_Unit_SpreadParticle));
        addFactory(N_new PreDefineScriptAnalyzeFactory<ParticleEffectScriptAnalyze>(N_Script_Unit_ParticleEffect));
        addFactory(N_new PreDefineScriptAnalyzeFactory<FontScriptAnalyze>(N_Script_Unit_Font));
        addFactory(N_new PreDefineScriptAnalyzeFactory<CompositorTranslator>(N_Script_Unit_Compositor));
    }
    //-----------------------------------------------------------------------
    ScriptCompilerManager::~ScriptCompilerManager()
    {
        N_delete mScriptCompiler;
    }
    //-----------------------------------------------------------------------
    bool ScriptCompilerManager::addFactory(ScriptAnalyzeFactory * obj)
    {
        FactoryList::iterator i = mFactory.find(obj->getID());
        if(i == mFactory.end())
        {
            mFactory.insert(Npair(obj->getID(), obj));
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void ScriptCompilerManager::removeFactory(ScriptAnalyzeFactory * obj)
    {
        FactoryList::iterator i = mFactory.find(obj->getID());
        if(i != mFactory.end())
        {
            mFactory.erase(i);
        }
    }
    //----------------------------------------------------------------------
    ScriptAnalyzeFactory * ScriptCompilerManager::getFactory(FactoryID fid) const
    {
        ScriptAnalyzeFactory * analyze = 0;
        {
            FactoryList::const_iterator i = mFactory.find(fid);
            if(i != mFactory.end())
            {
                analyze = i->second;
            }
        }
        return analyze;
    }
    //----------------------------------------------------------------------
    ScriptAnalyze * ScriptCompilerManager::create(ScriptCompiler * cpl, FactoryID fid, LangID lid) const
    {
        FactoryList::const_iterator i = mFactory.find(fid);
        if(i != mFactory.end())
        {
            return i->second->create(cpl, lid);
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void ScriptCompilerManager::destroy(ScriptAnalyze * obj)
    {
        FactoryList::iterator i = mFactory.find(obj->getUnit());
        if(i != mFactory.end())
        {
            i->second->destroy(obj);
        }
    }
    //-----------------------------------------------------------------------
    NIIf ScriptCompilerManager::getScriptLevel() const
    {
        // Load relatively early, before most script loaders run
        return 90.0f;
    }
    //-----------------------------------------------------------------------
    void ScriptCompilerManager::parse(DataStream * stream, GroupID gid, PtrList & out)
    {
        Nui8 * data;
        NCount size;
        if(stream->isSysMemory())
        {
            data = static_cast<MemStream *>(stream)->getData();
            size = stream->size();
        }
        else
        {
            size = stream->cloneData(data);
        }
        VString tempstr((char *)data, size);
        String temp;
        StrConv::conv(tempstr, temp);
        mScriptCompiler->compile(temp, stream->getName(), gid);

        if(!stream->isSysMemory())
            N_free(data);
    }
    //-------------------------------------------------------------------------
}