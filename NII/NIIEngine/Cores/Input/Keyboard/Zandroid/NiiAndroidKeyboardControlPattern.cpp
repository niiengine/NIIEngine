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
#include "NiiAndroidKeyboardControlPattern.h"

namespace NII
{
    //------------------------------------------------------------------------
    AndroidKeyboardControlPattern::AndroidKeyboardControlPattern(Nid id) :
            KeyboardControlPattern(id),
    {
    }
    //------------------------------------------------------------------------
    AndroidKeyboardControlPattern::~AndroidKeyboardControlPattern()
    {
    }
    //------------------------------------------------------------------------
    void AndroidKeyboardControlPattern::_init()
    {

    }
    //------------------------------------------------------------------------
    void AndroidKeyboardControlPattern::_update()
    {

    }
    //------------------------------------------------------------------------
    void AndroidKeyboardControlPattern::_reset()
    {
        // donothing
    }
    //------------------------------------------------------------------------
    void AndroidKeyboardControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void AndroidKeyboardControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void AndroidKeyboardControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    Nui8 AndroidKeyboardControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    Nui8 AndroidKeyboardControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    PatternID AndroidKeyboardControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void AndroidKeyboardControlPattern::syn() const
    {

    }
    //------------------------------------------------------------------------
}