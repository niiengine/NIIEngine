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
#include "OgreXSIHelper.h"
#include <fstream>

namespace Ogre {

	//-----------------------------------------------------------------------
	void copyFile(const String & src, const String & dst)
	{
		std::ifstream in(src.c_str(), std::ios::in | std::ios::binary);
		std::ofstream out(dst.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

		if(!in || !out)
		{
			LogOgreAndXSI("Unable to copy texture '" + src + "' to '" + dst + "'");
			return;
		}

		char tmpBuf[2048];

		while(!in.eof())
		{
			in.read(tmpBuf, 2048);
			std::streamsize c = in.gcount();
			out.write(tmpBuf, c);
		}

		in.close();
		out.close();
	}
	//-----------------------------------------------------------------------
	template<> ProgressManager * Singleton<ProgressManager>::msSingleton = 0;
	ProgressManager * ProgressManager::getSingletonPtr(void)
	{
		return msSingleton;
	}
	ProgressManager& ProgressManager::getSingleton(void)
	{  
		assert( msSingleton );  return ( *msSingleton );  
	}
	//-----------------------------------------------------------------------------
	ProgressManager::ProgressManager(size_t numberOfStages)
		:mNumberOfStages(numberOfStages), mProgress(0)
	{
		XSI::Application app;

		mProgressBar = app.GetUIToolkit().GetProgressBar();
		mProgressBar.PutMaximum(numberOfStages);
		mProgressBar.PutStep(1);
		mProgressBar.PutVisible(true);
		mProgressBar.PutCaption(L"Exporting");
	}
	//-----------------------------------------------------------------------------
	ProgressManager::~ProgressManager()
	{
	}
	//-----------------------------------------------------------------------------
	void ProgressManager::progress(void)
	{
		++mProgress;
		mProgressBar.Increment();
		
	}

}