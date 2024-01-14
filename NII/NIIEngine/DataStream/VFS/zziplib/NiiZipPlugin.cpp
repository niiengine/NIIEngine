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

#include "NiiZipPlugin.h"
#include "NiiVFSManager.h"
#include "NiiZip.h"

namespace NII
{
	//---------------------------------------------------------------------
	ZipPlugin::ZipPlugin() : Plugin(2, T_Compress_Data), m1(0), m2(0){}
	//---------------------------------------------------------------------
	String ZipPlugin::getName() const
	{
		return _T("Zip Archive");
	}
	//---------------------------------------------------------------------
	void ZipPlugin::install()
	{
        m1 = N_new ZipArchiveFactory();
        N_Only(VFS).add(m1);
        m2 = N_new EmbeddedZipArchiveFactory();
        N_Only(VFS).add(m2);
	}
	//---------------------------------------------------------------------
	void ZipPlugin::init()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void ZipPlugin::shutdown()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void ZipPlugin::uninstall()
	{
        N_Only(VFS).add(m1);
        N_Only(VFS).add(m2);
        N_delete m1;
        N_delete m2;
	}
    //-----------------------------------------------------------------------
}