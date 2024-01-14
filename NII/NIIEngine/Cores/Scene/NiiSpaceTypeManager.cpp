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
#include "NiiSpaceTypeManager.h"
#include "NiiConfigFile.h"
#include "NiiMaterial.h"
#include "NiiException.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // DefaultSceneManager
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    /*class _EngineAPI DefaultSceneManager : public SpaceManager
    {
    public:
        DefaultSceneManager(const String & name, TypeID tid) :
            SpaceManager(name, tid)
        {

        }
        ~DefaultSceneManager()
        {
        }
    };*/
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // DefaultSceneManagerFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    /** 默认场景管理器工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DefaultSceneManagerFactory : public SpaceTypeFactory
    {
    public:
        DefaultSceneManagerFactory()
        {
            mVerbose.mTypeID = N_FACTORY_NormalSpaceType;
            mVerbose.mVerbose = _T("The default scene manager");
            mVerbose.mMark = SpaceTypeVerbose::ST_Normal;
        }
        ~DefaultSceneManagerFactory()
        {
        }

        FactoryID getID()
        {
            return N_FACTORY_NormalSpaceType;
        }

        SpaceManager * create(const String & name)
        {
            return N_new SpaceManager(name, N_FACTORY_NormalSpaceType);
        }

        void destroy(SpaceManager * obj)
        {
            N_delete obj;
        }
    protected:
        const SpaceTypeVerbose & getVerboseImpl() const
        {
            return mVerbose;
        }
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // SpaceTypeFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    SpaceTypeFactory::SpaceTypeFactory() :
        mVerboseValid(false)
    {
    }
    //-----------------------------------------------------------------------
    SpaceTypeFactory::~SpaceTypeFactory()
    {

    }
    //-----------------------------------------------------------------------
    const SpaceTypeVerbose & SpaceTypeFactory::getVerbose() const
    {
        if(mVerboseValid != true)
        {
            const_cast<SpaceTypeFactory *>(this)->mVerboseValid = true;
            return getVerboseImpl();
        }
        return mVerbose;
    }
    //-----------------------------------------------------------------------
    template<> SpaceTypeManager * Singleton<SpaceTypeManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    /*SpaceTypeManager * SpaceTypeManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    SpaceTypeManager & SpaceTypeManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //-----------------------------------------------------------------------
    SpaceTypeManager::SpaceTypeManager() :
        mCount(0),
        mRenderSys(0)
    {
        add(N_new DefaultSceneManagerFactory());
    }
    //-----------------------------------------------------------------------
    SpaceTypeManager::~SpaceTypeManager()
    {
        SpaceList instancesCopy = mSpaceList;
        for(SpaceList::iterator i = instancesCopy.begin(); i != instancesCopy.end(); ++i)
        {
            for(FactoryList::iterator f = mFactoryList.begin(); f != mFactoryList.end(); ++f)
            {
                if((*f)->getVerbose().mTypeID == i->second->getTypeID())
                {
                    (*f)->destroy(i->second);
                    mSpaceList.erase(i->first);
                    break;
                }
            }
        }
        mSpaceList.clear();
        for(FactoryList::iterator f = mFactoryList.begin(); f != mFactoryList.end(); ++f)
        {
            if((*f)->getID() == N_FACTORY_NormalSpaceType)
            {
                N_delete *f;
                mFactoryList.erase(f);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void SpaceTypeManager::add(SpaceTypeFactory * obj)
    {
        mFactoryList.push_back(obj);
        mSpaceTypeList.push_back(&obj->getVerbose());

        N_Only(Log).log(_I18n("SpaceTypeFactory for type '") +
            N_conv(obj->getVerbose().mTypeID) + _I18n("' registered."));
    }
    //-----------------------------------------------------------------------
    void SpaceTypeManager::remove(FactoryID tid)
    {
        // remove from metadata
        SpaceTypeList::iterator m, mend = mSpaceTypeList.end();
        for(m = mSpaceTypeList.begin(); m != mend; ++m)
        {
            if((*m)->mTypeID == tid)
            {
                mSpaceTypeList.erase(m);
                break;
            }
        }
        FactoryList::iterator f, fend = mFactoryList.end();
        for (f = mFactoryList.end(); f != fend; ++fend)
        {
            if ((*f)->getID() == tid)
            {
                // destroy all instances for this factory
                SpaceList::iterator i, iend = mSpaceList.end();
                for (i = mSpaceList.begin(); i != iend;)
                {
                    SpaceManager * instance = i->second;
                    if (instance->getTypeID() == tid)
                    {
                        (*f)->destroy(instance);
                        SpaceList::iterator deli = i++;
                        mSpaceList.erase(deli);
                    }
                    else
                    {
                        ++i;
                    }
                }
                mFactoryList.erase(f);
                break;
            }
        }

    }
    //-----------------------------------------------------------------------
    const SpaceTypeVerbose * SpaceTypeManager::getVerbose(FactoryID tid) const
    {
        SpaceTypeList::const_iterator i, iend = mSpaceTypeList.end();
        for(i = mSpaceTypeList.begin(); i != iend; ++i)
        {
            if(tid == (*i)->mTypeID)
            {
                return *i;
            }
        }

        N_EXCEPT(NotExist, _I18n("No metadata found for scene manager of type '") + N_conv(tid) + _T("'"));
    }
    //-----------------------------------------------------------------------
    SpaceManager * SpaceTypeManager::createSpace(FactoryID tid, const String & name)
    {
        if (mSpaceList.find(name) != mSpaceList.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("SpaceManager instance called '") + name + _I18n("' already exists"));
        }

        SpaceManager* inst = 0;
        for(FactoryList::iterator i = mFactoryList.begin(); i != mFactoryList.end(); ++i)
        {
            if ((*i)->getVerbose().mTypeID == tid)
            {
                if (name.empty())
                {
                    // generate a name
                    StringStream s;
                    s << _I18n("SceneManagerInstance") << ++mCount;
                    inst = (*i)->create(s.str());
                }
                else
                {
                    inst = (*i)->create(name);
                }
                break;
            }
        }

        if(!inst)
        {
            // Error!
            N_EXCEPT(NotExist, _I18n("No factory found for scene manager of type '") +  N_conv(tid) + _T("'"));
        }

        // assign rs if already configured
        if(mRenderSys)
            inst->setRender(mRenderSys);

        mSpaceList[inst->getName()] = inst;

        return inst;
    }
    //-----------------------------------------------------------------------
    SpaceManager * SpaceTypeManager::createSpace(Nmark mark, const String & name)
    {
        if (mSpaceList.find(name) != mSpaceList.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("SpaceManager instance called '") + name + _I18n("' already exists"));
        }

        SpaceManager * re = 0;
        String name2 = name;
        if (name.empty())
        {
            StringStream s;
            s << _I18n("SceneManagerInstance") << ++mCount;
            name2 = s.str();
        }

        // Iterate backwards to find the matching factory registered last
        for(FactoryList::reverse_iterator i = mFactoryList.rbegin(); i != mFactoryList.rend(); ++i)
        {
            if ((*i)->getVerbose().mMark & mark)
            {
                re = (*i)->create(name);
                break;
            }
        }

        // use default factory if none
        if(!re)
            re = mFactoryList[0]->create(name);

        // assign rs if already configured
        if(mRenderSys)
            re->setRender(mRenderSys);

        mSpaceList[re->getName()] = re;

        return re;
    }
    //-----------------------------------------------------------------------
    void SpaceTypeManager::destroy(SpaceManager * sm)
    {
        mSpaceList.erase(sm->getName());
        for(FactoryList::iterator i = mFactoryList.begin(); i != mFactoryList.end(); ++i)
        {
            if ((*i)->getVerbose().mTypeID == sm->getTypeID())
            {
                (*i)->destroy(sm);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    SpaceManager * SpaceTypeManager::getSpace(const String & name) const
    {
        SpaceList::const_iterator i = mSpaceList.find(name);
        if(i != mSpaceList.end())
        {
            return i->second;
        }
        else
        {
            N_EXCEPT(NotExist, _I18n("SpaceManager instance with name '") + name + _I18n("' not found."));
        }
    }
    //---------------------------------------------------------------------
    bool SpaceTypeManager::isSpaceExist(const String & name) const
    {
        return mSpaceList.find(name) != mSpaceList.end();
    }
    //-----------------------------------------------------------------------
    void SpaceTypeManager::setRender(RenderSys * rs)
    {
        mRenderSys = rs;
        SpaceList::iterator i, iend = mSpaceList.end();
        for(i = mSpaceList.begin(); i != iend; ++i)
        {
            i->second->setRender(mRenderSys);
        }
    }
    //-----------------------------------------------------------------------
    void SpaceTypeManager::destroyAll()
    {
        SpaceList::iterator i, iend = mSpaceList.end();
        for(i = mSpaceList.begin(); i != iend; ++i)
        {
            i->second->destroyAllCreateObj();
        }
    }
    //-----------------------------------------------------------------------
}
