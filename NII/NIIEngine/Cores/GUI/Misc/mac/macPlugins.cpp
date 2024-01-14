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
#include "macPlugins.h"

namespace CEGUI 
{
    //------------------------------------------------------------------------
    CFStringRef createBundleNameString(const char* name)
    {
        CFStringRef s = CFStringCreateWithCString(0, name, kCFStringEncodingASCII);

        if(!CFStringHasSuffix(s, CFSTR(".bundle"))) 
        {
            CFMutableStringRef ms = CFStringCreateMutableCopy(0, 0, s);
            CFRelease(s);
            CFStringAppend(ms, CFSTR(".bundle"));
            s = ms;
        }
        
        return s;
    }
    //------------------------------------------------------------------------
    CFURLRef createFullPluginsURL(CFBundleRef bundle)
    {
        CFURLRef pluginsURL = 0;

        CFURLRef baseURL = CFBundleCopyBundleURL(bundle);

        if (baseURL)
        {
            CFURLRef pluginsLocation = CFBundleCopyBuiltInPlugInsURL(bundle);
            
            if (pluginsLocation)
            {
                pluginsURL = CFURLCreateCopyAppendingPathComponent(0, baseURL,
                                                  CFURLGetString(pluginsLocation),
                                                  false);
                CFRelease(pluginsLocation);
            }
            
            CFRelease(baseURL);
        }
        
        return pluginsURL;
    }
    //------------------------------------------------------------------------
    CFBundleRef createBundle(CFBundleRef sourceBundle, CFStringRef bundleName)
    {
        CFBundleRef bundle = 0;

        CFURLRef pluginsURL = createFullPluginsURL(sourceBundle);

        if (pluginsURL)
        {
            CFURLRef bundleURL =
                CFURLCreateCopyAppendingPathComponent(0, pluginsURL,
                                                      bundleName, false);
            if (bundleURL)
            {
                bundle = CFBundleCreate(0, bundleURL);
                CFRelease(bundleURL);
            }

            CFRelease(pluginsURL);
        }

        return bundle;
    }
    //------------------------------------------------------------------------
    CFBundleRef mac_loadExeBundle(const char* name) 
    {
        N_Only(Log).log(_I18n("---- Beginning exe bundle loading ----"));
        N_Only(Log).log(_I18n("-- Input bundle name: ") + String(name));

        CFBundleRef bundle = 0;

        CFStringRef pluginName = createBundleNameString(name);
        
        if (pluginName)
        {
            bundle = createBundle(CFBundleGetMainBundle(), pluginName);

            if (!bundle)
            {
                N_Only(Log).log(
                    _I18n("-- Couldn't get bundle from main app bundle reference; trying CEGUIbase instead."));

                bundle = createBundle(CFBundleGetBundleWithIdentifier(
                    CFSTR("net.sourceforge.crayzedsgui.CEGUIBase")),  pluginName);
            }

            if (bundle) 
            {
                if (!CFBundleLoadExecutable(bundle))
                {
                    CFRelease(bundle);
                    bundle = 0;
                }
            }
            
            CFRelease(pluginName);
        }
        if (bundle)
            N_Only(Log).log(_I18n("-- Bundle loaded successfully."));
        else
            N_Only(Log).log(_I18n("-- Bundle loading failed!"));

        N_Only(Log).log(_I18n("---- Finished exe bundle loading ----"));

        return bundle;
    }
    //------------------------------------------------------------------------
    void* mac_getBundleSym(CFBundleRef bundle, const char* name) 
    {
        CFStringRef nameRef =
            CFStringCreateWithCString(0, name, kCFStringEncodingASCII);
        void* sym = CFBundleGetFunctionPointerForName(bundle, nameRef);
        CFRelease(nameRef);

        return sym;
    }
    //------------------------------------------------------------------------
    // Returns 1 on error, 0 otherwise.
    bool mac_unloadExeBundle(CFBundleRef bundle) 
    {
        if (bundle) 
        {
            // No-op, can't unload Obj-C bundles without crashing.
            return 0;
        }
        
        return 1;
    }
    //------------------------------------------------------------------------
    const char* mac_errorBundle() 
    {
        return "Unknown Error";
    }
    //------------------------------------------------------------------------
}