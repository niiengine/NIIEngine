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

#ifndef _NII_PRE_PROCESS_H_
#define _NII_PRE_PROCESS_H_

#if (N_PLAT == N_PLAT_WIN32 && !defined(__MINGW32__)) || \
    N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
    #include "NiiPreInclude.h"
    #include "NiiVFS.h"
    #include "NiiAABox.h"
    #include "NiiCamera.h"
    #include "NiiColour.h"
    #include "NiiCommon.h"
    #include "NiiDataStream.h"
    #include "NiiException.h"
    #include "NiiFileSystem.h"
    #include "NiiMath.h"
    #include "NiiMatrix3.h"
    #include "NiiMatrix4.h"
    #include "NiiPosNode.h"
    #include "NiiPlane.h"
    #include "NiiQuaternion.h"
    #include "NiiLogManager.h"
    #include "NiiResource.h"
    #include "NiiSerializer.h"
    #include "NiiSharedPtrST.h"
    #include "NiiRenderObj.h"
    #include "NiiLinearSpline.h"
    #include "NiiSingleton.h"
    #include "NiiSpaceObj.h"
    #include "NiiSphere.h"
    #include "NiiString.h"
    #include "NiiPropertyObj.h"
    #include "NiiString.h"
    #include "NiiExtData.h"
    #include "NiiVector2.h"
    #include "NiiVector3.h"
    #include "NiiVector4.h"
    #include "NiiRect.h"
    #include "NiiStrConv.h"
    #include "NiiWireAABGeo.h"
#endif

#endif
