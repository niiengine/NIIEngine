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

#ifndef _NII_GameAPI_EXPORT_H_
#define _NII_GameAPI_EXPORT_H_

#	if defined( NII_GAMEAPI_STATIC_LIB )
#   	define _NIIGameExport
#   else
#   	if defined( NII_GAMEAPI_NONCLIENT_BUILD )
#       	define _NIIGameExport __declspec( dllexport )
#   	else //vc和 mingw 都能用
#           define _NIIGameExport __declspec( dllimport ) 
#   	endif
#	endif
#   define _NIIAPIPrivate

#endif