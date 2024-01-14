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
#include "NiiRenderSysManager.h"
#include "NiiString.h"
#include "NiiLogManager.h"
#include "NiiVFSManager.h"
#include "NiiStrConv.h"
#include "NiiException.h"
#include "NiiRenderFeatureSerializer.h"

namespace NII
{
    //-----------------------------------------------------------------------
    template<> RenderSysManager * Singleton<RenderSysManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    RenderSysManager::RenderSysManager()
    {
        mSerializer = N_new RenderFeatureSerializer();
    }
    //-----------------------------------------------------------------------
    RenderSysManager::~RenderSysManager()
    {
        FeatureList::iterator i, iend = mFeatureList.end();
        for (i = mFeatureList.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        N_delete mSerializer;
    }
    //----------------------------------------------------------------------
    void RenderSysManager::add(const String & name, RenderFeature * spec)
    {
        FeatureList::iterator i = mFeatureList.find(name);
        if(i != mFeatureList.end())
            mFeatureList.insert(FeatureList::value_type(name, spec));
        else
        {
            delete i->second;
            i->second = spec;
        }
    }
    //-----------------------------------------------------------------------
    void RenderSysManager::remove(const String & name)
    {
        FeatureList::iterator i = mFeatureList.find(name);
        if (i != mFeatureList.end())
        {
            mFeatureList.erase(name);
        }
    }
    //-----------------------------------------------------------------------
    void RenderSysManager::load(const String & filename, VFSType type, bool r)
    {
        // get the list of .gpufeature files
        VFS * vfs = N_Only(VFS).load(filename, type);
        StringList files;
        vfs->find(files, _T("*.gpufeature"), r);

        // loop through .gpufeature files and load each one
        for(StringList::iterator it = files.begin(), end = files.end(); it != end; it++)
        {
            DataStream * stream = vfs->open(*it);
            mSerializer->read(stream);
            stream->close();
            N_delete stream;
        }
    }
    //----------------------------------------------------------------------
    RenderFeature * RenderSysManager::getFeature(const String & name) const
    {
        FeatureList::iterator i = mFeatureList.find(name);
        if(i != mFeatureList.end())
        {
            return i->second;
        }
        return 0;
    }
    //----------------------------------------------------------------------
}