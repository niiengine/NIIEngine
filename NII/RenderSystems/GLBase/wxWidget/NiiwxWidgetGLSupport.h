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

#ifndef _NII_WXWIDGETGLSUPPORT_H_
#define _NII_WXWIDGETGLSUPPORT_H_

#include "NiiGLPreInclude.h"
#include "NiiGLSupport.h"
#include "NiiSingleton.h"

namespace NII 
{
    class wxWidgetGLSupport : public GLSupport, public Singleton<wxWidgetGLSupport>
    {
    public:
        wxWidgetGLSupport();

        /// @copydetails GLSupport::setupConfig
        void setupConfig(SysOptionList & out, PropertyData & out2);

        /// @copydetails GLSupport::newWindow
        ViewWindow * newWindow(const String & name, NCount width, NCount height, bool full, const PropertyData * custom = 0);

        /// @copydetails GLSupport::start
        void start();

        /// @copydetails GLSupport::stop
        void stop();

        /// @copydetails GLSupport::getProcAddress
        void * getProcAddress(const VString & procname);

        static wxWidgetGLSupport & getOnly(void);

        static wxWidgetGLSupport * getOnlyPtr(void);
    };
};
#endif