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

#include <CoreFoundation/CoreFoundation.h>
#include "NiiPreInclude.h"

namespace NII 
{
    CFBundleRef mac_loadExeBundle(const NIIb * path);
    void * mac_getBundleSym(CFBundleRef bundle, const NIIb * name);
    bool mac_unloadExeBundle(CFBundleRef bundle);
    void * mac_loadDylib(const NIIb* name);
    _EngineAPI std::string macPluginPath();
    _EngineAPI std::string macBundlePath();
}