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
#include "NiiSerializerManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    template<> SerializerManager * Singleton<SerializerManager>::mOnly = 0;
    /*SerializerManager * SerializerManager::getOnlyPtr()
    {
        return mOnly;
    }
    SerializerManager & SerializerManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //------------------------------------------------------------------------
    SerializerManager::SerializerManager(){}
    //------------------------------------------------------------------------
    SerializerManager::~SerializerManager()
    {
        removeAll();
    }
    //------------------------------------------------------------------------
    void SerializerManager::add(SerializerFactoryObj * obj)
    {
        SerializerList::iterator it = mSerializers.find(obj->getID());
        if(it == mSerializers.end())
            mSerializers.insert(Npair(obj->getID(), obj));
    }
    //------------------------------------------------------------------------
    void SerializerManager::remove(SerializerFactoryObj * obj)
    {
        SerializerList::iterator it = mSerializers.find(obj->getID());
        if (it != mSerializers.end())
        {
            mSerializers.erase(it);
        }
    }
    //------------------------------------------------------------------------
    SerializerFactoryObj * SerializerManager::getFactory(Nui32 id)
    {
        SerializerList::iterator it = mSerializers.find(id);
        if(it != mSerializers.end())
            return it->second;
        return 0;
    }
    //------------------------------------------------------------------------
    void SerializerManager::removeAll()
    {
        mSerializers.clear();
    }
    //------------------------------------------------------------------------
}