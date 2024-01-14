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
#include "NiiIOSMouseControlPattern.h"

namespace NII
{
    //------------------------------------------------------------------------
    IOSMouseControlPattern::IOSMouseControlPattern(){}
    //------------------------------------------------------------------------
    IOSMouseControlPattern::IOSMouseControlPattern(Nid id) :
        MouseControlPattern(id){}
    //------------------------------------------------------------------------
    IOSMouseControlPattern::~IOSMouseControlPattern()
    {

    }
    //------------------------------------------------------------------------
    NIIb IOSMouseControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    NIIb IOSMouseControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    void IOSMouseControlPattern::_init()
    {

    }
    //------------------------------------------------------------------------
    void IOSMouseControlPattern::_update()
    {

    }
    //------------------------------------------------------------------------
    void IOSMouseControlPattern::_reset()
    {
        // donothing
    }
    //------------------------------------------------------------------------
    void IOSMouseControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void IOSMouseControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void IOSMouseControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    PatternID IOSMouseControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}