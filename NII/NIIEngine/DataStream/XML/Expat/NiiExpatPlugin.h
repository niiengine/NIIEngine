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

#ifndef _NII_PLUGIN_H_
#define _NII_PLUGIN_H_

#include "NiiPreInclude.h"
#include "NiiPlugin.h"

namespace NII
{
    /** Expat专用插件
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ExpatPlugin : public Plugin
    {
    public:
        ExpatPlugin(PluginID pid);
        ~ExpatPlugin();

        /// @copydetails Plugin::getName
        String getName();

        /// @copydetails Plugin::Version
        String Version() const;

        /// @copydetails Plugin::getLisence
        String getLisence() const;

        /// @copydetails Plugin::getVerbose
        String getVerbose() const;

        /// @copydetails Plugin::install
        void install();

        /// @copydetails Plugin::init
        void init();

        /// @copydetails Plugin::shutdown
        void shutdown();

        /// @copydetails Plugin::uninstall
        void uninstall();

        /// @copydetails Plugin::getOwnLisence
        String getOwnLisence() const;

        /// @copydetails Plugin::
        String getOwnVersion() const;

        /// @copydetails Plugin::getPerformance
        NIIb getPerformance() const;

        /// @copydetails Plugin::getEffectiveness
        NIIb getEffectiveness() const;

        /// @copydetails Plugin::getComplex
        NIIb getComplex() const;

        /// @copydetails Plugin::getExtention
        NIIb getExtention() const;
    private:
        ExpatParser * mInstance;
    };
}
#endif