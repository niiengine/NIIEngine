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

#ifndef __AUTOPATCHER_PATCH_CONTEXT_H
#define __AUTOPATCHER_PATCH_CONTEXT_H

enum PatchContext
{
	PC_HASH_1_WITH_PATCH,
	PC_HASH_2_WITH_PATCH,
	PC_WRITE_FILE,
	PC_ERROR_FILE_WRITE_FAILURE,
	PC_ERROR_PATCH_TARGET_MISSING,
	PC_ERROR_PATCH_APPLICATION_FAILURE,
	PC_ERROR_PATCH_RESULT_CHECKSUM_FAILURE,
	PC_NOTICE_WILL_COPY_ON_RESTART,
	PC_NOTICE_FILE_DOWNLOADED,
	PC_NOTICE_FILE_DOWNLOADED_PATCH,
};

#endif
