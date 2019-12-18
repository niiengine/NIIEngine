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

/* 我们的简化数据输入结构 */
struct _finddata_t
{
    NIIb * name;
    NIIi attrib;
    Nul size;
};

#if N_PLAT != N_PLAT_SYMBIAN
    #define _A_NORMAL 0x00  /* 普通文件-没有 读/写 限制 */
    #define _A_RDONLY 0x01  /* 只读文件 */
    #define _A_HIDDEN 0x02  /* 隐藏文件 */
    #define _A_SYSTEM 0x04  /* 系统文件 */
    #define _A_ARCH   0x20  /* 存档文件 */
#endif
    #define _A_SUBDIR 0x10  /* 子目录 */

    intptr_t _findfirst(const NIIb * pattern, struct _finddata_t * data);

    NIIi _findnext(intptr_t id, struct _finddata_t * data);

    NIIi _findclose(intptr_t id);

#endif
#endif