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
#include "NiiVFSManager.h"
#include "NiiException.h"
#include "NiiLogManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    template<> VFSManager * Singleton<VFSManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    /*VFSManager * VFSManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    VFSManager & VFSManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //-----------------------------------------------------------------------
    VFSManager::VFSManager()
    {
        mVFSList.clear();
    }
    //-----------------------------------------------------------------------
    VFS * VFSManager::load(const String & protocol, VFSType type)
    {
        VFSInsList::iterator i = mVFSInsList.find(protocol);
        VFS * re = 0;

        if(i == mVFSInsList.end())
        {
            VFSList::iterator it = mVFSList.find(type);
            if(it == mVFSList.end())
                N_EXCEPT(NotExist, _I18n("不能找到指定类型档案工厂,类型ID: ") + type);

            re = it->second->create(protocol);
            re->peek();
            mVFSInsList[protocol] = re;
        }
        else
        {
            re = i->second;
        }
        return re;
    }
    //-----------------------------------------------------------------------
    void VFSManager::unload(const String & protocol)
    {
        VFSInsList::iterator i = mVFSInsList.find(protocol);

        if(i != mVFSInsList.end())
        {
            i->second->close();

            VFSList::iterator fit = mVFSList.find(i->second->getType());
            if(fit != mVFSList.end())
            {
                fit->second->destroy(i->second);
                mVFSInsList.erase(i);
            }
        }
    }
    //-----------------------------------------------------------------------
    VFSManager::~VFSManager()
    {
        VFSInsList::iterator i, iend = mVFSInsList.end();
        for(VFSInsList::iterator i = mVFSInsList.begin(); i != iend; ++i)
        {
            VFS * vfs = i->second;
            vfs->close();

            VFSList::iterator fit = mVFSList.find(vfs->getType());
            if(fit != mVFSList.end())
            {
                fit->second->destroy(vfs);
            }
        }

        mVFSInsList.clear();
    }
    //-----------------------------------------------------------------------
    void VFSManager::add(VFSFactory * factory)
    {
        mVFSList.insert(VFSList::value_type(factory->getType(), factory));
        N_Only(Log).log(_I18n("VFS工厂类型: ") + N_conv(factory->getType()) + _I18n(" 注册成功!"));
    }
    //-----------------------------------------------------------------------
    void VFSManager::remove(VFSFactory * factory)
    {
        VFSList::iterator i, iend = mVFSList.end();
        for(i = mVFSList.begin(); i != iend; ++i)
        {
            if(i->second == factory)
            {
                N_delete i->second;
                mVFSList.erase(i);
                N_Only(Log).log(_I18n("VFS工厂类型: ") + N_conv(factory->getType()) + _I18n(" 卸除成功!"));
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
}