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