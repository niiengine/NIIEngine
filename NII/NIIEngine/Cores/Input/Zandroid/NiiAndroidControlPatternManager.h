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

#ifndef _NII_ANDROID_CONTROLPATTERN_MANAGER_H_
#define _NII_ANDROID_CONTROLPATTERN_MANAGER_H_

#include "NiiAndroidControlPatternPrereq.h"
#include "NiiInputManager.h"

namespace NII
{
    /** 这是一个专门为NII引擎包装的Android
    @version NIIEngine 3.0.0
    */
    class AndroidControlPatternManager : public InputManager
    {
    public:
        AndroidControlPatternManager();
        ~AndroidControlPatternManager();

        /// @copydetails InputManager::set
        void set(const PropertyData * misc);

        /// @copydetails InputManager::getCount
        NCount getCount(const ViewWindow * main, ControlDevType type) const;
    protected:
        /// @copydetails InputManager::create
        ControlPattern * create(const ViewWindow * main, ControlDevType type);
    private:
    };
}
#endif