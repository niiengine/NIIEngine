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

#ifndef _NII_LINUX_CONTROLPATTERN_MANAGER_PLUGIN_H_
#define _NII_LINUX_CONTROLPATTERN_MANAGER_PLUGIN_H_

#include "NiiPreInclude.h"
#include "NiiPlugin.h"

namespace NII
{
namespace NII_MEDIA
{
    class InputManager;
    /**
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class LinuxControlPatternManagerPlugin : public Plugin
    {
    public:
        LinuxControlPatternManagerPlugin();

        /// @copydetails Plugin::install
        void install();

        /// @copydetails Plugin::init
        void init();

        /// @copydetails Plugin::shutdown
        void shutdown();

        /// @copydetails Plugin::uninstall
        void uninstall();

        /// @copydetails Plugin::getName
        const String & getName() const;

        /// @copydetails Plugin::getLisence
        const String & getLisence() const;

        /// @copydetails Plugin::getOwnVersion
        const String & getOwnVersion() const;

        /// @copydetails Plugin::getVerbose
        const String & getVerbose()const;

        /// @copydetails Plugin::getComplex
        virtual NIIb getComplex()const;

        /// @copydetails Plugin::getExtention
        virtual NIIb getExtention()const;
    private:
        static InputManager * mInstance;
    };
}
}
#endif