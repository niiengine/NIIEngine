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
#include "NiiAndroidMouseControlPattern.h"

namespace NII
{
    //------------------------------------------------------------------------
    AndroidMouseControlPattern::AndroidMouseControlPattern(){}
    //------------------------------------------------------------------------
    AndroidMouseControlPattern::AndroidMouseControlPattern(Nid id) :
        MouseControlPattern(id){}
    //------------------------------------------------------------------------
    AndroidMouseControlPattern::~AndroidMouseControlPattern()
    {

    }
    //------------------------------------------------------------------------
    NIIb AndroidMouseControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    NIIb AndroidMouseControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    void AndroidMouseControlPattern::_init()
    {

    }
    //------------------------------------------------------------------------
    void AndroidMouseControlPattern::_update()
    {

    }
    //------------------------------------------------------------------------
    void AndroidMouseControlPattern::_reset()
    {
        // donothing
    }
    //------------------------------------------------------------------------
    void AndroidMouseControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void AndroidMouseControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void AndroidMouseControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    PatternID AndroidMouseControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}