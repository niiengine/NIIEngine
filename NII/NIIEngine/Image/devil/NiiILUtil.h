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

#ifndef _NII_ILUtil_H_
#define _NII_ILUtil_H_

#include <NiiPrerequisites.h>
#include <NiiCommon.h>
#include <NiiPixelUtil.h>

namespace NII
{
    /** DevIL 特定的实用类
    */    
    class _EngineInner ILUtil 
    {
    public:
        /// 封装devIL图象定义结构
        struct ILFormat 
        {
            /// 构造一个无效ILFormat结构
            ILFormat():
                numberOfChannels(0), format(-1), type(-1) {};

            /// 从参数中构建一个ILFormat
            ILFormat(NIIi channels, NIIi format, NIIi type=-1):
                numberOfChannels(channels), format(format), type(type) {}

            /// 返回这个结构是否描述一个有效的DevIL格式
            bool isValid() { return format!=-1; }

            /// 通道数,通常是3或4
            NIIi numberOfChannels;
            /// IL_RGBA,IL_BGRA,IL_DXTx, ...
            NIIi format;
            /// IL_UNSIGNED_BYTE, IL_UNSIGNED_SHORT, ... 压缩格式可能是-1
            NIIi type;
        };

        /// 传递一个NII类型获得IL格式的最佳转换
        static PixelFormat ilFormat2NiiFormat(NIIi ImageFormat, NIIi ImageType);
        
        /** 从内存中找到NII类型所属的IL格式
        @remark 返回一个无效ILFormat(isValid()==false)当IL中没有与此批配的格式
        */
        static ILFormat NiiFormat2ilFormat(PixelFormat format);  
        
        /** 转换当前IL图象到一个NII格式,目标大小为 PixelUtil::getNumElemBytes(fmt) * 
            ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 
            ilGetInteger(IL_IMAGE_DEPTH).IIL图象类型必须为IL(_UNSIGNED_)BYTE
            或者IL_FLOAT.IL图象格式必须为(IL_RGBA, IL_BGRA, IL_RGB, IL_BGR, 
            IL_LUMINANCE 或者 IL_LUMINANCE_ALPHA)
        */
        static void to(const PixelBlock & dst);

        /** 转换一个NII格式图象到当前的IL图象.
        @param[in] src 封装资源指针,宽度,高度,深度和格式
        */
        static void from(const PixelBlock & src);
    };
}

#endif
