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
#include "NiiLodSchemeManager.h"
#include "NiiException.h"
#include "NiiMeshLodScheme.h"
#include "NiiMaterialLodScheme.h"

/**
Converters

NVIDIA Melody
This tool allows you creating a normal map
for a low-poly model using as reference a high-poly model.

The usual working flow is as follows: you create a high-poly
model for cinematics. Then you create a new low-poly model
using the high-poly model as reference by reducing the number
of polygons of the high-model using a decimation/polygon reduction
tool. Finally this tool will generate the normal map from both
models which you can use to make the low-poly model look like a high-poly one.

Simplygon
Commercial Mesh optimization and Mesh reduction software

VizUp
Commercial Mesh optimization and Mesh reduction software
*/

namespace NII
{
    //-----------------------------------------------------------------------
    template<> LodSchemeManager * Singleton<LodSchemeManager>::mOnly = 0;
    /*LodSchemeManager * LodSchemeManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    LodSchemeManager & LodSchemeManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //-----------------------------------------------------------------------
    LodSchemeManager::LodSchemeManager()
    {
        add(N_LOD_Mesh, N_new MeshViewLodScheme(2));
        add(N_LOD_Mesh, N_new MeshCameraLodScheme(1));
        setDefault(N_LOD_Mesh, 1);

        add(N_LOD_Material, N_new MaterialViewLodScheme(2));
        add(N_LOD_Material, N_new MaterialCameraLodScheme(1));
        setDefault(N_LOD_Material, 1);
    }
    //-----------------------------------------------------------------------
    LodSchemeManager::~LodSchemeManager()
    {
        removeAll();
    }
    //-----------------------------------------------------------------------
    void LodSchemeManager::add(ClassID cid, LodScheme * obj)
    {
        ClassSchemeList::iterator i = mSchemeList.find(cid);
        if(i != mSchemeList.end())
        {
            SchemeList::iterator j = (i->second).find(obj->getTypeID());
            if(j == (i->second).end())
                (i->second).insert(std::pair<SchemeID, LodScheme *>(obj->getTypeID(), obj));
        }
        else
        {
            ClassSchemeList::iterator temp = mSchemeList.insert(std::pair<ClassID, SchemeList>(cid, SchemeList())).first;
            (temp->second).insert(std::pair<SchemeID, LodScheme *>(obj->getTypeID(), obj));
        }
    }
    //-----------------------------------------------------------------------
    void LodSchemeManager::remove(ClassID cid, SchemeID sid)
    {
        ClassSchemeList::iterator i = mSchemeList.find(cid);
        if(i != mSchemeList.end())
        {
            SchemeList::iterator j = (i->second).find(sid);
            if(j != (i->second).end())
            {
                N_delete j->second;
                (i->second).erase(j);
                if((i->second).empty())
                {
                    mSchemeList.erase(i);
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void LodSchemeManager::removeAll(ClassID cid)
    {
        ClassSchemeList::iterator i = mSchemeList.find(cid);
        if(i != mSchemeList.end())
        {
            SchemeList::iterator j, jend = (i->second).end();
            for(j = (i->second).begin(); j != jend; ++j)
            {
                N_delete j->second;
            }
            (i->second).clear();
            mSchemeList.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void LodSchemeManager::removeAll()
    {
        ClassSchemeList::iterator i, iend = mSchemeList.end();
        for(i = mSchemeList.begin(); i != iend; ++i)
        {
            SchemeList::iterator j, jend = (i->second).end();
            for(j = (i->second).begin(); j != jend; ++j)
            {
                N_delete j->second;
            }
            (i->second).clear();
        }
        mSchemeList.clear();
    }
    //-----------------------------------------------------------------------
    LodScheme * LodSchemeManager::get(ClassID cid, SchemeID sid)
    {
        ClassSchemeList::iterator i = mSchemeList.find(cid);
        if(i != mSchemeList.end())
        {
            SchemeList::iterator j = (i->second).find(sid);
            if(j != (i->second).end())
            {
                return j->second;
            }
            else
            {
                DefaultList::iterator z = mDefaultList.find(cid);
                if(z != mDefaultList.end())
                    return z->second;
            }
        }
        return NULL;
    }
    //-----------------------------------------------------------------------
    LodScheme * LodSchemeManager::create(ClassID cid, SchemeID sid)
    {
        ClassSchemeList::iterator i = mSchemeList.find(cid);
        if(i != mSchemeList.end())
        {
            SchemeList::iterator j = (i->second).find(sid);
            if(j != (i->second).end())
            {
                return j->second->clone();
            }
            else
            {
                DefaultList::iterator z = mDefaultList.find(cid);
                if(z != mDefaultList.end())
                    return z->second->clone();
            }
        }
        return NULL;
    }
    //-----------------------------------------------------------------------
    void LodSchemeManager::setDefault(ClassID cid, SchemeID sid)
    {
        LodScheme * temp = get(cid, sid);
        if(temp != 0)
        {
            DefaultList::iterator z = mDefaultList.find(cid);
            if(z != mDefaultList.end())
            {
                z->second = temp;
            }
            else
            {
                mDefaultList.insert(Npair(cid, temp));
            }
        }
    }
    //-----------------------------------------------------------------------
    LodScheme * LodSchemeManager::createDefault(ClassID cid)
    {
        DefaultList::iterator z = mDefaultList.find(cid);
        if(z != mDefaultList.end())
            return z->second->clone();
        return 0;
    }
    //-----------------------------------------------------------------------
    const LodSchemeManager::SchemeList * LodSchemeManager::getList(ClassID cid)
    {
        ClassSchemeList::iterator i = mSchemeList.find(cid);
        if(i != mSchemeList.end())
        {
            return &(i->second);
        }
        return 0;
    }
    //-----------------------------------------------------------------------
}