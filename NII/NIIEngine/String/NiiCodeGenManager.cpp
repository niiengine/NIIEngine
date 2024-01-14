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
#include "NiiCodeGenManager.h"
#include "NiiException.h"
#include "NiiLogManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    template<> CodeGenManager * Singleton<CodeGenManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    CodeGenManager::CodeGenManager()
    {
    }
    //-----------------------------------------------------------------------
    CodeGen * CodeGenManager::load(const String * rawcode, Nid codeid)
    {
        CodeGenInsList::iterator i = mCodeGenInsList.find(rawcode);
        CodeGen * re = 0;

        if(i == mCodeGenInsList.end())
        {
            FactoryList::iterator it = mFactoryList.find(codeid);
            if(it == mFactoryList.end())
                return 0;

            re = it->second->create(rawcode);
            mCodeGenInsList.insert(Npair(rawcode, re));
        }
        else
        {
            re = i->second;
        }
        return re;
    }
    //-----------------------------------------------------------------------
    void CodeGenManager::unload(const String * rawcode)
    {
        CodeGenInsList::iterator i = mCodeGenInsList.find(rawcode);

        if(i != mCodeGenInsList.end())
        {
            FactoryList::iterator fit = mFactoryList.find(i->second->getCodeID());
            if(fit != mFactoryList.end())
            {
                fit->second->destroy(i->second);
                mCodeGenInsList.erase(i);
            }
        }
    }
    //-----------------------------------------------------------------------
    CodeGenManager::~CodeGenManager()
    {
        CodeGenInsList::iterator i, iend = mCodeGenInsList.end();
        for(CodeGenInsList::iterator i = mCodeGenInsList.begin(); i != iend; ++i)
        {
            CodeGen * gen = i->second;

            FactoryList::iterator fit = mFactoryList.find(gen->getCodeID());
            if(fit != mFactoryList.end())
            {
                fit->second->destroy(gen);
            }
        }

        mCodeGenInsList.clear();
        
        FactoryList::iterator j, jend = mFactoryList.end();
        for(j = mFactoryList.begin(); j != jend; ++j)
        {
            N_delete j->second;
        }
        mFactoryList.clear();
    }
    //-----------------------------------------------------------------------
    void CodeGenManager::add(CodeGenFactory * factory)
    {
        mFactoryList.insert(FactoryList::value_type(factory->getCodeID(), factory));
        N_Only(Log).log(_I18n("CodeGen工厂类型: ") + N_conv(factory->getCodeID()) + _I18n(" 注册成功!"));
    }
    //-----------------------------------------------------------------------
    void CodeGenManager::remove(CodeGenFactory * factory)
    {
        FactoryList::iterator i, iend = mFactoryList.end();
        for(i = mFactoryList.begin(); i != iend; ++i)
        {
            if(i->second == factory)
            {
                N_delete i->second;
                mFactoryList.erase(i);
                N_Only(Log).log(_I18n("CodeGen工厂类型: ") + N_conv(factory->getCodeID()) + _I18n(" 卸除成功!"));
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
}