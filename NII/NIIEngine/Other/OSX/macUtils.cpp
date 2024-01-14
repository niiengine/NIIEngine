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

#include "macUtils.h"

#include "OgreString.h"
#include <dlfcn.h>

namespace NII {

    CFBundleRef mac_loadExeBundle(const NIIb *name) {
        CFBundleRef baseBundle = CFBundleGetBundleWithIdentifier(CFSTR("org.ogre3d.Ogre"));
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFStringRef nameRef = CFStringCreateWithCString(NULL, name, kCFStringEncodingASCII);
        CFURLRef bundleURL = 0; //URL of bundle to load
        CFBundleRef bundle = 0; //bundle to load
        
        //cut off .bundle if present
        if(CFStringHasSuffix(nameRef, CFSTR(".bundle"))) {
            CFStringRef nameTempRef = nameRef;
            NIIi end = CFStringGetLength(nameTempRef) - CFStringGetLength(CFSTR(".bundle"));
            nameRef = CFStringCreateWithSubstring(NULL, nameTempRef, CFRangeMake(0, end));
            CFRelease(nameTempRef);
        }
                
        //assume relative to Resources/ directory of Main bundle
        bundleURL = CFBundleCopyResourceURL(mainBundle, nameRef, CFSTR("bundle"), NULL);
        if(bundleURL) {
            bundle = CFBundleCreate(NULL, bundleURL);
            CFRelease(bundleURL);
        }
        
        //otherwise, try Resources/ directory of Ogre Framework bundle
        if(!bundle) {
            bundleURL = CFBundleCopyResourceURL(baseBundle, nameRef, CFSTR("bundle"), NULL);
            if(bundleURL) {
               bundle = CFBundleCreate(NULL, bundleURL);
               CFRelease(bundleURL);
            }
        }
        CFRelease(nameRef);
       
        if(bundle) {
            if(CFBundleLoadExecutable(bundle)) {
                return bundle;
            }
            else {
                CFRelease(bundle);
            }
        }
        
        return 0;
    }
    
    void *mac_getBundleSym(CFBundleRef bundle, const NIIb *name) {
        CFStringRef nameRef = CFStringCreateWithCString(NULL, name, kCFStringEncodingASCII);
        void *sym = CFBundleGetFunctionPointerForName(bundle, nameRef);
        CFRelease(nameRef);
        return sym;
    }
    
    //returns 1 on error, 0 otherwise
    bool mac_unloadExeBundle(CFBundleRef bundle) {
        if(bundle) {
            //no-op, can't unload Obj-C bundles without crashing
            return 0;
        }
        return 1;
    }

	void* mac_loadDylib(const NIIb* name)
	{
		std::string fullPath=name;
		if(name[0]!='/')
			fullPath = macPluginPath()+"/"+fullPath;
		
		return dlopen(fullPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	}
	
    std::string macBundlePath()
    {
        NIIb path[1024];
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        N_assert(mainBundle);
        
        CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
        N_assert(mainBundleURL);
        
        CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
        N_assert(cfStringRef);
        
        CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
        
        CFRelease(mainBundleURL);
        CFRelease(cfStringRef);
        
        return std::string(path);
    }
    
    std::string macPluginPath()
	{
		return macBundlePath() + "/Contents/Plugins/";
	}
}
