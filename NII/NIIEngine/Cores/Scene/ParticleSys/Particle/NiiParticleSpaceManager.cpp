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
#include "NiiParticleSpaceManager.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"
#include "NiiParticleCtrlGeo.h"
#include "NiiSpreadParticle.h"
#include "NiiParticleEffect.h"
#include "NiiStrConv.h"

#if 1 == 1
    #include "NiiScriptCompiler.h"
    #include "NiiScriptCompilerManager.h"
#endif

namespace NII
{
    //-----------------------------------------------------------------------
    void parseAttrib(const String & line, ParticleSpace * sys)
    {
        // Split params on space
        StringList vecparams;
        StrUtil::split(line, vecparams, _T("\t "), 1);

        // Look up first param (command setting)
        if (!sys->set(vecparams[0], vecparams[1]))
        {
            // Attribute not supported by particle system, try the renderer
            ParticleGeo * renderer = sys->getGeo();
            if (renderer)
            {
                if (!renderer->set(vecparams[0], vecparams[1]))
                {
                    N_Only(Log).log(_I18n("Bad particle system attribute line: '") +
                        line + _T("' in ") + N_conv(sys->getID()) + _I18n(" (tried renderer)"));
                }
            }
            else
            {
                N_Only(Log).log(_I18n("Bad particle system attribute line: '") +
                    line + _T("' in ") + N_conv(sys->getID()) + _I18n(" (no renderer)"));
            }
        }
    }
    //-----------------------------------------------------------------------
    void parseEmitterAttrib(const String & line, SpreadParticle * emit)
    {
        // Split params on first space
        StringList vecparams;
        StrUtil::split(line, vecparams, _T("\t "), 1);

        // Look up first param (command setting)
        if (!emit->set(vecparams[0], vecparams[1]))
        {
            N_Only(Log).log(_I18n("Bad particle emitter attribute line: '") +
                line + _I18n("' for emitter ") + N_conv(emit->getType()));
        }
    }
    //-----------------------------------------------------------------------
    void parseAffectorAttrib(const String & line, ParticleEffect * aff)
    {
        // Split params on space
        StringList vecparams;
        StrUtil::split(line, vecparams, _T("\t "), 1);

        // Look up first param (command setting)
        if (!aff->set(vecparams[0], vecparams[1]))
        {
            N_Only(Log).log(_I18n("Bad particle affector attribute line: '") +
                line + _I18n("' for affector ") + N_conv(aff->getType()));
        }
    }
    //-----------------------------------------------------------------------
    void skipToNextOpenBrace(DataStream * stream)
    {
        String line;
        while (!stream->end() && line != _T("{"))
        {
            line = stream->readLine();
        }
    }
    //-----------------------------------------------------------------------
    void skipToNextCloseBrace(DataStream * stream)
    {
        String line;
        while (!stream->end() && line != _T("}"))
        {
            line = stream->readLine();
        }
    }
    //-----------------------------------------------------------------------
    void parseNewAffector(Nid type, DataStream * stream, ParticleSpace * sys)
    {
        // Create new affector
        ParticleEffect* pAff = sys->addEffect(type);
        // Parse affector details
        String line;

        while (!stream->end())
        {
            line = stream->readLine();
            // Ignore comments & blanks
            if (!(line.length() == 0 || line.substr(0, 2) == _T("//")))
            {
                if (line == _T("}"))
                {
                    // Finished affector
                    break;
                }
                else
                {
                    // Attribute
                    StrUtil::toLower(line);
                    parseAffectorAttrib(line, pAff);
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    ParticleCtrlGeoFactory * mBillboardRendererFactory = 0;
    //-----------------------------------------------------------------------
    TimeDurMS ParticleSpaceManager::mTimeInterval = 0;
    TimeDurMS ParticleSpaceManager::mTimeThreshold = 0;
    template<> ParticleSpaceManager * Singleton<ParticleSpaceManager>::mOnly = 0;
    /*ParticleSpaceManager * ParticleSpaceManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    ParticleSpaceManager & ParticleSpaceManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //-----------------------------------------------------------------------
    ParticleSpaceManager::ParticleSpaceManager()
    {
        N_mutex1_lock
#if 1 == 0
        mScriptPattern.push_back("*.particle");
        N_Only(ResourceScheme).add(this);
#endif
        mFactory = N_new ParticleSpaceFactory();
        N_Engine().addFactory(mFactory);
    }
    //-----------------------------------------------------------------------
    ParticleSpaceManager::~ParticleSpaceManager()
    {
        N_mutex1_lock

        SpaceList::iterator t, tend = mSpaceList.end();
        for (t = mSpaceList.begin(); t != tend; ++t)
        {
            N_delete t->second;
        }
        mSpaceList.clear();
        N_Only(ResourceScheme).remove(this);

        if(mBillboardRendererFactory)
        {
            N_delete mBillboardRendererFactory;
            mBillboardRendererFactory = 0;
        }

        if(mFactory)
        {
            N_Engine().removeFactory(mFactory->getID());
            N_delete mFactory;
            mFactory = 0;
        }
    }
    //-----------------------------------------------------------------------
    NIIf ParticleSpaceManager::getScriptLevel() const
    {
        return 1000.0f;
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::parse(DataStream * stream, GroupID gid, PtrList & out)
    {
#if 1 == 1
        N_Only(ScriptCompiler).parse(stream, gid, out);
#else
        String line;
        ParticleSpace * pSys;
        StringList vecparams;

        pSys = 0;

        while(!stream->end())
        {
            line = stream->readLine();
            // Ignore comments & blanks
            if (!(line.length() == 0 || line.substr(0,2) == "//"))
            {
                if (pSys == 0)
                {
                    // No current system
                    // So first valid data should be a system name
                    if (StrUtil::beginsWith(line, "particle_system "))
                    {
                        // chop off the 'particle_system ' needed by new compilers
                        line = line.substr(16);
                    }
                    pSys = createTemplate(line, gid);
                    pSys->setSrc(stream->getName());
                    // Skip to and over next {
                    skipToNextOpenBrace(stream);
                }
                else
                {
                    // Already in a system
                    if (line == "}")
                    {
                        // Finished system
                        pSys = 0;
                    }
                    else if (line.substr(0,7) == "emitter")
                    {
                        // new emitter
                        // Get typename
                        StrUtil::split(line, vecparams,"\t ");
                        if (vecparams.size() < 2)
                        {
                            // Oops, bad emitter
                            N_Only(Log).log(_I18n("Bad particle system emitter line: '")
                                + line + "' in " + pSys->getName());
                            skipToNextCloseBrace(stream);

                        }
                        skipToNextOpenBrace(stream);
                        parse(vecparams[1], stream, pSys);

                    }
                    else if (line.substr(0,8) == "affector")
                    {
                        // new affector
                        // Get typename
                        StrUtil::split(line, vecparams, "\t ");
                        if (vecparams.size() < 2)
                        {
                            // Oops, bad affector
                            N_Only(Log).log(_I18n("Bad particle system affector line: '")
                                + line + "' in " + pSys->getName());
                            skipToNextCloseBrace(stream);

                        }
                        skipToNextOpenBrace(stream);
                        parseNewAffector(vecparams[1],stream, pSys);
                    }
                    else
                    {
                        // Attribute
                        parseAttrib(line, pSys);
                    }
                }
            }
        }
#endif
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::add(SpreadParticleFactory * factory)
    {
        N_mutex1_lock
        Nid name = factory->getID();
        mEmitterFactories[name] = factory;
        N_Only(Log).log(_I18n("Particle Emitter Type '") + N_conv(name) + _I18n("' registered"));
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::add(ParticleEffectFactory * factory)
    {
        N_mutex1_lock
        Nid sid = factory->getID();
        mAffectorFactories[sid] = factory;
        N_Only(Log).log(_I18n("Particle Affector Type '") + N_conv(sid) + _I18n("' registered"));
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::add(ParticleGeoFactory * factory)
    {
        N_mutex1_lock
        Nid id = factory->getID();
        mGeoFactoryList[id] = factory;
        N_Only(Log).log(_I18n("Particle Renderer Type '") + N_conv(id) + _I18n("' registered"));
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::addTemplate(SpaceID sid, ParticleSpace * sysTemplate)
    {
        N_mutex1_lock
        if (mSpaceList.find(sid) != mSpaceList.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("ParticleSpace template with sid '") + 
                N_conv(sid) + _I18n("' already exists."));
        }

        mSpaceList[sid] = sysTemplate;
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::removeTemplate(SpaceID sid, bool deleteTemplate)
    {
        N_mutex1_lock
        SpaceList::iterator itr = mSpaceList.find(sid);
        if (itr != mSpaceList.end())
        {
            if (deleteTemplate)
            N_delete itr->second;

            mSpaceList.erase(itr);
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::removeAllTemplate(bool deleteTemplate)
    {
        N_mutex1_lock
        if (deleteTemplate)
        {
            SpaceList::iterator itr;
            for (itr = mSpaceList.begin(); itr != mSpaceList.end(); ++itr)
                N_delete itr->second;
        }
        mSpaceList.clear();
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::removeAllTemplate(GroupID gid)
    {
        N_mutex1_lock

        SpaceList::iterator i = mSpaceList.begin();
        SpaceList::iterator iend = mSpaceList.end();
        while (i != iend)
        {
            SpaceList::iterator icur = i++;

            if(icur->second->getResourceGroup() == gid)
            {
                delete icur->second;
                mSpaceList.erase(icur);
            }
        }
    }
    //-----------------------------------------------------------------------
    ParticleSpace * ParticleSpaceManager::createTemplate(SpaceID sid, GroupID rgid)
    {
        N_mutex1_lock
        if (mSpaceList.find(sid) != mSpaceList.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("ParticleSpace template with id '") + 
                N_conv(sid) + _I18n("' already exists."));
        }

        ParticleSpace* tpl = N_new ParticleSpace(sid, rgid);
        addTemplate(sid, tpl);
        return tpl;
    }
    //-----------------------------------------------------------------------
    ParticleSpace * ParticleSpaceManager::getTemplate(SpaceID name)
    {
        N_mutex1_lock
        SpaceList::iterator i = mSpaceList.find(name);
        if (i != mSpaceList.end())
        {
            return i->second;
        }
        else
        {
            return 0;
        }
    }
    //-----------------------------------------------------------------------
    ParticleSpace * ParticleSpaceManager::createImpl(SpaceID name, NCount max, GroupID gid)
    {
        ParticleSpace * sys = N_new ParticleSpace(name, gid);
        sys->setPoolSize(max);
        return sys;
    }
    //-----------------------------------------------------------------------
    ParticleSpace * ParticleSpaceManager::createImpl(SpaceID name, SpaceID templateid)
    {
        ParticleSpace * pTemplate = getTemplate(templateid);
        if (!pTemplate)
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot find required template '") +
                N_conv(templateid) + _T("'"));
        }

        ParticleSpace * sys = createImpl(name, pTemplate->getPoolSize(),
            pTemplate->getResourceGroup());
        *sys = *pTemplate;
        return sys;
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::destroyImpl(ParticleSpace * sys)
    {
        N_delete sys;
    }
    //-----------------------------------------------------------------------
    SpreadParticle * ParticleSpaceManager::createSpread(Nid type, ParticleSpace * psys)
    {
        N_mutex1_lock
        SpreadFactoryList::iterator i = mEmitterFactories.find(type);
        if(i == mEmitterFactories.end())
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot find requested emitter type."));
        }
        return i->second->create(psys);
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::destroySpread(SpreadParticle * emitter)
    {
        N_mutex1_lock
        SpreadFactoryList::iterator i = mEmitterFactories.find(emitter->getType());
        if(i == mEmitterFactories.end())
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot find emitter factory to destroy emitter."));
        }
        i->second->destroy(emitter);
    }
    //-----------------------------------------------------------------------
    ParticleEffect * ParticleSpaceManager::createEffect(Nid type,
        ParticleSpace * psys)
    {
        N_mutex1_lock
        EffectFactoryList::iterator i = mAffectorFactories.find(type);
        if(i == mAffectorFactories.end())
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot find requested affector type."));
        }
        return i->second->create(psys);
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::destroyEffect(ParticleEffect * affector)
    {
        N_mutex1_lock
        EffectFactoryList::iterator pFact = mAffectorFactories.find(affector->getType());
        if(pFact == mAffectorFactories.end())
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot find affector factory to destroy affector."));
        }
        pFact->second->destroy(affector);
    }
    //-----------------------------------------------------------------------
    ParticleGeo* ParticleSpaceManager::createGeo(Nid rendererType)
    {
        N_mutex1_lock
        // Locate affector type
        GeoFactoryList::iterator i = mGeoFactoryList.find(rendererType);

        if (i == mGeoFactoryList.end())
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot find requested renderer type."));
        }
        return static_cast<ParticleGeo*>(i->second->create(N_StrBlank));
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::destroyGeo(ParticleGeo * geo)
    {
        N_mutex1_lock
        // Destroy using the factory which created it
        GeoFactoryList::iterator pFact = mGeoFactoryList.find(geo->getType());

        if (pFact == mGeoFactoryList.end())
        {
            N_EXCEPT(InvalidParam, _I18n("Cannot find geo factory to destroy geo."));
        }

        pFact->second->destroy(geo);
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::init()
    {
        N_mutex1_lock
        // Create SyncParticle renderer factory
        mBillboardRendererFactory = N_new ParticleCtrlGeoFactory();
        add(mBillboardRendererFactory);
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceManager::parse(Nid type, DataStream * stream, ParticleSpace * sys)
    {
        // Create new emitter
        SpreadParticle * pEmit = sys->addSpread(type);
        // Parse emitter details
        String line;

        while(!stream->end())
        {
            line = stream->readLine();
            // Ignore comments & blanks
            if (!(line.length() == 0 || line.substr(0,2) == _T("//")))
            {
                if (line == _T("}"))
                {
                    // Finished emitter
                    break;
                }
                else
                {
                    // Attribute
                    StrUtil::toLower(line);
                    parseEmitterAttrib(line, pEmit);
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    const ParticleSpaceManager::EffectFactoryList & ParticleSpaceManager::getEffectFactoryList() const
    {
        return mAffectorFactories;
    }
    //-----------------------------------------------------------------------
    const ParticleSpaceManager::SpreadFactoryList & ParticleSpaceManager::getSpreadFactoryList() const
    {
        return mEmitterFactories;
    }
    //-----------------------------------------------------------------------
    const ParticleSpaceManager::GeoFactoryList & ParticleSpaceManager::getGeoFactoryList() const
    {
        return mGeoFactoryList;
    }
    //-----------------------------------------------------------------------
    const ParticleSpaceManager::SpaceList & ParticleSpaceManager::getSpaceList() const
    {
        return mSpaceList;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ParticleSpaceFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    SpaceObj * ParticleSpaceFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
    {
        if(params != 0)
        {
            Nid templateName;
            params->get(N_PropertyParticleSpace_Template, templateName, 0);
            if(templateName != 0)
            {
                // create using manager
                return N_Only(ParticleSpace).createImpl(sid, templateName);
            }
        }
        // Not template based, look for quota & resource name
        Nui32 quota;
        GroupID gid;
        if(params != 0)
        {
            params->get(N_PropertyParticleSpace_PoolSize, quota, 500);
            params->get(N_PropertyResource_Group, gid, GroupDefault);
        }
        // create using manager
        SpaceObj * re = N_Only(ParticleSpace).createImpl(sid, quota, gid);
        re->setFactory(this);
        re->setManager(mag);

        return re;
    }
    //-----------------------------------------------------------------------
    FactoryID ParticleSpaceFactory::getID() const
    {
        return N_FACTORY_ParticleSpace;
    }
    //-----------------------------------------------------------------------
    void ParticleSpaceFactory::destroy(void * obj)
    {
        N_Only(ParticleSpace).destroyImpl(static_cast<ParticleSpace *>(obj));
    }
    //-----------------------------------------------------------------------
}