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
#include "NiiEquationManager.h"
#include "NiiShaderChTexture.h"
#include "NiiLogManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // EquationManager
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    template<> EquationManager * Singleton<EquationManager>::mOnly = 0;
    /*EquationManager * EquationManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    EquationManager & EquationManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //-----------------------------------------------------------------------
    EquationManager::EquationManager():
        mLastFrame(0)
    {
    }
    //-----------------------------------------------------------------------
    EquationManager::~EquationManager()
    {
        destroyAll();
    }
    //-----------------------------------------------------------------------
    void EquationManager::update(Nmark frameMark)
    {
        if(frameMark != mLastFrame)
        {
            EquationList::const_iterator ci, ciend = mEquationList.end();
            for(ci = mEquationList.begin(); ci != ciend; ++ci)
            {
                (*ci)->exec();
            }
            mLastFrame = frameMark;
        }
    }
    //-----------------------------------------------------------------------
    void EquationManager::destroyAll()
    {
        EquationList::iterator i, iend = mEquationList.end();
        for (i = mEquationList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mEquationList.clear();
    }
    //-----------------------------------------------------------------------
    void EquationManager::destroy(DataEquationBase * base)
    {
        EquationList::iterator i = mEquationList.find(base);
        if(i != mEquationList.end())
        {
            N_delete base;
            mEquationList.erase(i);
        }
    }
    //-----------------------------------------------------------------------
}