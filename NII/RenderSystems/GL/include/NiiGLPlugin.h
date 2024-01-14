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

#ifndef _NII_GLPlugin_H_
#define _NII_GLPlugin_H_

#include "NiiPlugin.h"
#include "NiiGLRenderSystem.h"

namespace NII
{
	class GLPlugin : public Plugin
	{
	public:
		GLPlugin();

		/// @copydetails Plugin::getName
		String getName() const;

		/// @copydetails Plugin::install
		void install();

		/// @copydetails Plugin::init
		void init();

		/// @copydetails Plugin::shutdown
		void shutdown();

		/// @copydetails Plugin::uninstall
		void uninstall();

		/// @copydetails Plugin::Version
		virtual String Version() const { return _T("5.6.1"); }

		/// @copydetails Plugin::getLisence
		virtual String getLisence() const { return _T(""); }

		/// @copydetails Plugin::getVerbose
		virtual String getVerbose() const { return _T(""); }

		/// @copydetails Plugin::getOwnLisence
		virtual String getOwnLisence() const { return _T(""); }

		/// @copydetails Plugin::getOwnVersion
		virtual String getOwnVersion() const { return _T(""); }

		/// @copydetails Plugin::getPerformance
		virtual NIIb getPerformance() const { return 0; }

		/// @copydetails Plugin::getEffectiveness
		virtual NIIb getEffectiveness() const { return 0; }

		/// @copydetails Plugin::getComplex
		virtual NIIb getComplex() const { return 0; }

		/// @copydetails Plugin::getExtention
		virtual NIIb getExtention() const { return 0; }
	protected:
		GLRenderSystem * mRSys;
	};
}

#endif
