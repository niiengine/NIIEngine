/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-11-4

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
        DT_Float1x = 0,         ///< 以1个浮点为单位的
        DT_Float2x = 1,         ///< 以2个浮点为单位的
        DT_Float3x = 2,         ///< 以3个浮点为单位的
        DT_Float4x = 3,         ///< 以4个浮点为单位的
        DT_Colour = 4,          ///< 颜色格式(一般是32位)
        DT_Colour_ARGB = 5,     ///< 颜色格式(D3D期望)
        DT_Colour_ABGR = 6,     ///< 颜色格式(GL期望)
        DT_Short1x = 7,         ///< 以1个短整形为单位的
        DT_Short2x = 8,         ///< 以2个短整形为单位的
        DT_Short3x = 9,         ///< 以3个短整形为单位的
        DT_Short4x = 10,        ///< 以4个短整形为单位的
        DT_Vector2f = 11,       ///< 以2个浮点为单位的
        DT_Vector3f = 12,       ///< 以3个浮点为单位的
        DT_Vector4f = 13,       ///< 以4个浮点为单位的
        DT_Matrix3f = 14,       ///< 以9个浮点为单位的
        DT_Matrix4f = 15,       ///< 以16个浮点为单位的
        DT_U_Byte = 16,         ///< 无符号(32位)
        DT_Unkonw = 17
    };
    
    /** 数据类型辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DataTypeUtil
    {
    public:
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