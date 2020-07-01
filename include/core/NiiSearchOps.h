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

#ifndef _NII_SearchOps_H_
#define _NII_SearchOps_H_
// 仿真_findfirst,_findnext在非Windows平台

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "NiiPlat.h"

#if N_PLAT != N_PLAT_WIN32

#include <dirent.h>
#include <unistd.h>
#if N_PLAT != N_PLAT_SYMBIAN
#include <fnmatch.h>
#endif

struct _finddata_t
{
    NIIb * name;
    NIIi attrib;
    Nul size;
};

#if N_PLAT != N_PLAT_SYMBIAN
    #define _A_NORMAL 0x00  /* 普通文件-没有读写限制 */
    #define _A_RDONLY 0x01  /* 只读文件 */
    #define _A_HIDDEN 0x02  /* 隐藏文件 */
    #define _A_SYSTEM 0x04  /* 系统文件 */
    #define _A_VOLID  0x08  /* 卷号 */
    #define _A_ARCH   0x20  /* 存档文件 */
    
#endif
    #define _A_SUBDIR 0x10  /* 子目录 */

    intptr_t _findfirst(const char * pattern, struct _finddata_t * data);

    NIIi _findnext(intptr_t id, struct _finddata_t * data);

    NIIi _findclose(intptr_t id);

#endif
#endif