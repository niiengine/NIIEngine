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

#ifndef _NII_COMMON_NII_ERRORDIALOG_H_
#define _NII_COMMON_NII_ERRORDIALOG_H_

#include "NiiPreInclude.h"

#if (N_PLAT == N_PLAT_WIN32 || N_PLAT_WINRT == N_PLAT_WINRT || N_PLAT_WINRT == N_PLAT_WIN8_PHONE)
#include "NiiWIN32ErrorDialogImp.h"
#elif(N_PLAT == N_PLAT_ANDROID)
#include "NiiAndroidErrorDialogImp.h"
#elif(N_PLAT == N_PLAT_LINUX_X)
#include "NiiGLXErrorDialogImp.h"
#elif(N_PLAT == N_PLAT_OSX)
#include "NiiOSXErrorDialogImp.h"
#elif(N_PLAT == N_PLAT_IOS)
#include "NiiIOSErrorDialogImp.h"
#endif

#endif