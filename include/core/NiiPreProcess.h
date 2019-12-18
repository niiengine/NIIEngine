/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_PRE_PROCESS_H_
#define _NII_PRE_PROCESS_H_

#include "NiiPlat.h"

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
    #include "NiiLog.h"
    #include "NiiMath.h"
    #include "NiiMatrix3.h"
    #include "NiiMatrix4.h"
    #include "NiiPosNode.h"
    #include "NiiPlane.h"
    #include "NiiQuaternion.h"
    #include "NiiResource.h"
    #include "NiiSerializer.h"
    #include "NiiSharedPtrST.h"
    #include "NiiRenderObj.h"
    #include "NiiLinearSpline.h"
    #include "NiiSingleton.h"
    #include "NiiSpaceObj.h"
    #include "NiiSphere.h"
    #include "NiiStdInclude.h"
    #include "NiiString.h"
    #include "NiiPropertyCmdObj.h"
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
