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
#include "NiiThreadGroupLoadScheme.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ThreadGroupLoadScheme
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ThreadGroupLoadScheme::ThreadGroupLoadScheme(SchemeID sid, GroupID gid, NCount wl):
        GroupLoadScheme(sid, gid, wl)
    {
        
    }
    //------------------------------------------------------------------------
    ThreadGroupLoadScheme::~ThreadGroupLoadScheme()
    {
        
    }
    //------------------------------------------------------------------------
    bool ThreadGroupLoadScheme::request(LoadingState ls)
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool ThreadGroupLoadScheme::run()
    {
        return false;
    }
    //------------------------------------------------------------------------
    void ThreadGroupLoadScheme::pause()
    {
        
    }
    //------------------------------------------------------------------------
    void ThreadGroupLoadScheme::stop()
    {
        
    }
    //------------------------------------------------------------------------
    void ThreadGroupLoadScheme::main()
    {
        
    }
    //------------------------------------------------------------------------
    void ThreadGroupLoadScheme::add(Resource * r)
    {
        
    }
    //------------------------------------------------------------------------
    void ThreadGroupLoadScheme::remove(Resource * r)
    {
        
    }
    //------------------------------------------------------------------------
}