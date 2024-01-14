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
#include "NiiIOSKeyboardControlPattern.h"

namespace NII
{
    //------------------------------------------------------------------------
    IOSKeyboardControlPattern::IOSKeyboardControlPattern(Nid id) :
            KeyboardControlPattern(id),
    {

    }
    //------------------------------------------------------------------------
    IOSKeyboardControlPattern::~IOSKeyboardControlPattern()
    {

    }
    //------------------------------------------------------------------------
    void Win32KeyboardControlPattern::_init()
    {

    }
    //------------------------------------------------------------------------
    void IOSKeyboardControlPattern::_update()
    {

    }
    //------------------------------------------------------------------------
    void IOSKeyboardControlPattern::_reset()
    {
        // donothing
    }
    //------------------------------------------------------------------------
    void IOSKeyboardControlPattern::_start()
    {
        mActive = true;
    }
    //------------------------------------------------------------------------
    void IOSKeyboardControlPattern::_pause()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    void IOSKeyboardControlPattern::_stop()
    {
        mActive = false;
    }
    //------------------------------------------------------------------------
    Nui8 IOSKeyboardControlPattern::getQuality() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    Nui8 IOSKeyboardControlPattern::getPerformance() const
    {
        return 255;
    }
    //------------------------------------------------------------------------
    PatternID IOSKeyboardControlPattern::getPID() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void IOSKeyboardControlPattern::syn() const
    {

    }
    //------------------------------------------------------------------------
}