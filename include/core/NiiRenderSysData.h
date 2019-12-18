/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-31

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_RENDERSYS_DATA_H_
#define _NII_RENDERSYS_DATA_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 平台特定数据
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI RenderSysData
    {
    public:
        enum Type
        {
            T_Unknow        = 0,
            T_DX9_0         = 0x01,
            T_DX10_0        = 0x02,
            T_DX11_0        = 0x04,
            T_DX12_0        = 0x08,
            T_OPENGL1_1     = 0x10,
            T_OPENGL2_1     = 0x20,
            T_OPENGL3_0     = 0x30,
            T_OPENGL4_0     = 0x40,
            T_OPENGL4_1     = 0x80,
            T_OPENGL_ES1_1  = 0x100,
            T_OPENGL_ES2_1  = 0x200,
            T_OPENGL_ES3_1  = 0x400,
            T_DIRECT_FB     = 0x800,

            T_WIN32         = 0x100000,
            T_WINRT         = 0x200000,     ///< win8编程框架
            T_WIN8_PHONE    = 0x400000,
            T_LINUX         = 0x800000,     ///< linux内核
            T_LINUX_X       = 0x1000000,    ///< x窗体级别的linux系统
            T_OSX           = 0x2000000,    ///< 苹果mac os x(darwin)
            T_IOS           = 0x4000000,    ///< 苹果iphone(darwin)
            T_ANDROID       = 0x8000000,    ///< 安桌系统(linux内核级)
            T_EMSCRIPTEN    = 0x10000000,   ///< web编程框架
            T_FREEBSD       = 0x20000000,   ///< freeBSD
        };
    public:
        /** 获取平台额外信息
        @param[in] name 需要获取的属性名字
        @param[in] data 指向存储获取内容的内存指针
        @return 是否获取成功
        @note 一般在最底端接口重写这个函数
        @version NIIEngien 3.0.0 高级api
        */
        virtual bool getPlatformData(const String & name, void * data) const;

        /** 获取平台额外信息
        @param[in] name 需要获取的属性名字
        @param[in] data 指向存储获取内容的内存指针
        @return 组合类型
        @note 一般在最底端接口重写这个函数
        @version NIIEngien 3.0.0 高级api
        */
        virtual Nmark getPlatformDataType() const;
    };
}
#endif