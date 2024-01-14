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

#ifndef _NII_Config_H_
#define _NII_Config_H_

#include "NiiCustomConfig.h"

#ifndef NII_PROFILING
    #define NII_PROFILING           0
#endif

#define NII_Max_TextureUnit         0

#define NII_MAX_TextureCoord        8

#define NII_MAX_TextureLayer        16

#define NII_MAX_RenderTarget        8
#define NII_MAX_RenderTargetDS      10
#define NII_DepthTargetIndex        8
#define NII_StencilTargetIndex      9

#define NII_MAX_LIGHT               8

#define NII_MAX_BlendFactor         4

#define NII_StdAllocType            1
#define NII_NedAllocType            2
#define NII_CustomAllocType         3
#define NII_NedPoolAllocType        4

#ifndef NII_MemAllocType
    #define NII_MemAllocType        NII_NedAllocType
#endif

#ifndef NII_STLCustomAlloc
    #define NII_STLCustomAlloc      1
#endif

#ifndef NII_StringCustomAlloc
    #define NII_StringCustomAlloc   0
#endif

#ifndef N_DEBUG_MEMTRACK
    #define N_DEBUG_MEMTRACK        0
#endif

#ifndef N_RELEASE_MEMTRACK
    #define N_RELEASE_MEMTRACK      0
#endif

#ifndef NII_THREAD_SUPPORT
    #define NII_THREAD_SUPPORT      0
#endif

#define N_THREAD_NONE               0
#define N_THREAD_WIN32              1
#define N_THREAD_PTHREAD            2

#ifndef N_THREAD
    #define N_THREAD                0
#endif

#define NII_MEDIUM                  1
#define NII_BIG                     2
#define NII_SMALL                   3

#ifndef NII_VOLUME
    #define NII_VOLUME              NII_MEDIUM
#endif

#define NII_TESTMODE                1
#define NII_APPMODE                 2
#define NII_DEBUGMODE               3

#ifndef NII_USERMODE
    #define NII_USERMODE            NII_TESTMODE
#endif

#define NII_AsciiType               0
#define NII_UnicodeType             1

#ifndef NII_STRING
    #define NII_STRING              NII_UnicodeType
#endif

#define NII_NonSSE                  0
#define NII_SSE                     1

#ifndef NII_PLAMODE
    #if NII_SSE
        #define NII_PLAMODE         NII_SSE
    #else
        #define NII_PLAMODE         NII_NonSSE
    #endif
#endif

#endif
