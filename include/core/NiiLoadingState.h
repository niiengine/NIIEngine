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

#ifndef _NII_LOADING_STATE_H_
#define _NII_LOADING_STATE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 确定资源加载状态的枚举
    @version NIIEngine 3.0.0
    */
    enum LoadingState
    {
        LS_UNSETUP = 101,   ///< 未配置
        LS_SETUPING = 102,  ///< 配置中
        LS_SETUPED = 103,   ///< 已策略
        LS_PLANNED = 104,   ///< 完全准备好
        LS_PLANNING = 105,  ///< 准备中
        LS_LOADING = 106,   ///< 加载中
        LS_LOADED = 107,    ///< 完全加载完
        LS_UNLOADING = 108, ///< 当前卸载中
        LS_UNLOADED = 109,  ///< 还没加载
    };
}
#endif