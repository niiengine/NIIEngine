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
#include "NiiIDManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // IDManager
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    IDManager::IDManager(const String & autoname) : mCurrent(0){}
    //------------------------------------------------------------------------
    IDManager::~IDManager()
    {
        clear();
    }
    //------------------------------------------------------------------------
    void * IDManager::generate(const String & name)
    {
        N_mutex1_lock
        Nid temp;
        if (mRList.empty())
        {
            temp = mCurrent++;
        }
        else
        {
            temp = mRList.back();
            mRList.pop_back();
        }
        void * target = create(name);
        mList.insert(PoolList::value_type(temp, target));

        return target;
    }
    //------------------------------------------------------------------------
    void * IDManager::generate()
    {
        N_mutex1_lock
        Nid temp;
        if (mRList.empty())
        {
            temp = mCurrent++;
        }
        else
        {
            temp = mRList.back();
            mRList.pop_back();
        }
        void * target = create();
        mList.insert(PoolList::value_type(temp, target));

        return target;
    }
    //------------------------------------------------------------------------
    void IDManager::add(const void * target)
    {
        N_mutex1_lock
        Nid temp;
        if (mRList.empty())
        {
            temp = mCurrent++;
        }
        else
        {
            temp = mRList.back();
            mRList.pop_back();
        }

        reset(target, temp);

        mList.insert(Npair(temp, const_cast<void *>(target)));
    }
    //------------------------------------------------------------------------
    void IDManager::remove(ObjID id)
    {
        N_mutex1_lock
        PoolList::iterator i = mList.find(id);
        if(i != mList.end())
        {
            destroy(i->second);
            mList.erase(i);
            mRList.push_back(id);
        }
    }
    //------------------------------------------------------------------------
    void IDManager::remove(void * target)
    {
        N_mutex1_lock
        PoolList::iterator i, iend = mList.end();
        for(i = mList.begin(); i != iend; i++)
        {
            if(i->second == target)
            {
                destroy(i->second);
                mList.erase(i);
                mRList.push_back(i->first);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void IDManager::clear()
    {
        N_mutex1_lock
        PoolList::iterator i, iend = mList.end();
        for(i = mList.begin(); i != iend; i++)
        {
            destroy(i->second);
        }
        mRList.clear();
        mList.clear();
        mCurrent = 0;
    }
    //------------------------------------------------------------------------
}