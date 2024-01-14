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

#ifndef __I_IRRKLANG_FILE_FACTORY_H_INCLUDED__
#define __I_IRRKLANG_FILE_FACTORY_H_INCLUDED__

#include "ik_IRefCounted.h"

namespace irrklang
{
	class IFileReader;

	//! Interface to overwrite file access in irrKlang.
	/** Derive your own class from IFileFactory, overwrite the createFileReader()
		method and return your own implemented IFileReader to overwrite file access of irrKlang.
		Use ISoundEngine::addFileFactory() to let irrKlang know about your class.
		Example code can be found in the tutorial 04.OverrideFileAccess.
	 */
	class IFileFactory : public virtual IRefCounted
	{
	public:

		virtual ~IFileFactory() {};

		//! Opens a file for read access.
		/** Derive your own class from IFileFactory, overwrite this
		method and return your own implemented IFileReader to overwrite file access of irrKlang.
		Use ISoundEngine::addFileFactory() to let irrKlang know about your class.
		Example code can be found in the tutorial 04.OverrideFileAccess.
		\param filename Name of file to open.
		\return Returns a pointer to the created file interface.
		The returned pointer should be dropped when no longer needed.
		See IRefCounted::drop() for more information. Returns 0 if file cannot be opened. */
		virtual IFileReader* createFileReader(const ik_c8* filename) = 0;		
	};

} // end namespace irrklang

#endif

