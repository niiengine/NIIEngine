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

#ifndef _NII_DATA_TYPE_H_
#define _NII_DATA_TYPE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 顶点元素类型,用来确定顶点内容的基本类型
    @version NIIEngine 3.0.0
    */
    enum DataType
    {
        DT_Float1x      = 0,         ///< 以1个浮点为单位的
        DT_Float2x      = 1,         ///< 以2个浮点为单位的
        DT_Float3x      = 2,         ///< 以3个浮点为单位的
        DT_Float4x      = 3,         ///< 以4个浮点为单位的
        DT_Double1x     = 4,
        DT_Double2x     = 5,
        DT_Double3x     = 6,
        DT_Double4x     = 7,
        DT_Int1x        = 8,
        DT_Int2x        = 9,
        DT_Int3x        = 10,
        DT_Int4x        = 11,
        DT_UInt1x       = 12,
        DT_UInt2x       = 13,
        DT_UInt3x       = 14,
        DT_UInt4x       = 15,
        DT_Colour       = 16,       ///< 颜色格式(一般是32位)
        DT_Colour_ARGB  = 17,       ///< 颜色格式(D3D期望)
        DT_Colour_ABGR  = 18,       ///< 颜色格式(GL期望)
        DT_Short2x      = 20,       ///< 以2个短整形为单位的
        DT_Short4x      = 22,       ///< 以4个短整形为单位的
        DT_UShort2x     = 24,       ///< 以2个无符号短整形为单位的
        DT_UShort4x     = 26,       ///< 以4个无符号短整形为单位的
        DT_ShortUnit2x  = 27,
        DT_ShortUnit4x  = 28,
        DT_UShortUnit2x = 29,   
        DT_UShortUnit4x = 30,
        DT_Vector2f     = 31,       ///< 以2个浮点为单位的
        DT_Quaternion   = 32,       ///< 以4个浮点为单位的
        DT_Vector4f     = 33,       ///< 以4个浮点为单位的
        DT_Matrix4f     = 35,       ///< 以16个浮点为单位的
        DT_UByte4x      = 36,       ///< 无符号(32位)
        DT_Byte4x       = 37,
        DT_UByteUnit4x  = 38,  
        DT_ByteUnit4x   = 39,
        DT_HalfFloat2x  = 40,
        DT_HalfFloat4x  = 41,
        DT_Sampler      = 42,
        DT_Subroutine   = 43,
        DT_Unkonw       = 44
    };
    
    /** 数据类型辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DataTypeUtil
    {
    public:
        /**
        @version NIIEngine 5.0.0
        */
        bool isUnit(DataType type);
    
        /** 获取成分大小
        @param[in] type 数据成份
        @version NIIEngine 3.0.0
        */
        static NCount getSize(DataType type);

        /** 获取成分基础数量
        @param[in] type 数据成份
        @version NIIEngine 3.0.0
        */
        static NCount getCount(DataType type);

        /** 简单的转换器功能,把
        @param[in] type 复合基础成份
        @param[out] count 基础成份数量
        @version NIIEngine 3.0.0
        */
        static DataType getBaseType(DataType type, NCount & count);

        /** 获取合成类型
        @param[in] type 基础成份
        @param[in] count 基础成份数量
        @version NIIEngine 3.0.0
        */
        static DataType getMultiType(DataType type, NCount count);
    };
}

#endif