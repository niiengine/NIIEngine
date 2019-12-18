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

#ifndef _NII_STRCONV_H_
#define _NII_STRCONV_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiMatrix4.h"
#include "NiiQuaternion.h"
#include "NiiVector2.h"
#include "NiiRelPos.h"
#include "NiiShaderCh.h"
#include "NiiGpuProgram.h"

namespace NII
{
    /** 把单位数据 转为/换成 字符串类型的类
    @remark
        这个类支持单位类型的数据到字符串之间的变换操作,NIIEngine使用UTF-8和使用
        char作为核心字符串结构,所以脚本代码要保存为UTF-8的编码,如果是.niiengine
        后缀文件则可以忽略这个问题.在操作字符串时注意是使用std::string的结构体系.
    @note
        Unicode 有两套编码集,UCS-2和UCS-4,Windows的内部其实是用的UCS-2标准,并用
        UTF-16来实现.而非Windows系统大多采用了UTF-8实现.在windows上wchar_t是2
        个字节表示,而在Linux上wchar_t是4个字节表示的.这样写跨平台的程序时,就会不
        统一.有一点是需要注意的是,如果你的路径数据是出现在代码(c,c++)里的,这个时候
        就要注意你的代码文本是什么编码格式存储的.还有就是setlocale也会影响系统实际效
        果(如std,print等系统函数受到setlocale的影响)
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StrConv
    {
    public:
        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, Ni16 & out, Ni16 o = 0);

        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, Ni16 & out, Ni16 o = 0);

        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, Nui16 & out, Nui16 o = 0);

        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, Nui16 & out, Nui16 o = 0);

        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, Ni32 & out, Ni32 o = 0);

        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, Ni32 & out, Ni32 o = 0);

        /** 转换符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, Nui32 & out, Nui32 o = 0);

        /** 转换符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, Nui32 & out, Nui32 o = 0);

        /** 转换符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, Ni64 & out, Ni64 o = 0);

        /** 转换符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, Ni64 & out, Ni64 o = 0);

        /** 转换符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, Nui64 & out, Nui64 o = 0);

        /** 转换符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, Nui64 & out, Nui64 o = 0);

        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, NIIl & out, NIIl o = 0);

        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, NIIl & out, NIIl o = 0);

        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, Nul & out, Nul o = 0);

        /** 转换字符串到数字
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, Nul & out, Nul o = 0);

        /** 转换字符串到NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 解析失败则返回0.0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, NIIf & out, NIIf o = 0);

        /** 转换字符串到NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 解析失败则返回0.0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, NIIf & out, NIIf o = 0);

        /** 转换字符串到NIId
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 解析失败则返回0.0
        @version NIIEngine 3.0.0
        */
        static void conv(const WString & in, NIId & out, NIId o = 0);

        /** 转换字符串到NIId
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 解析失败则返回0.0
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, NIId & out, NIId o = 0);

        /** 转换字符串到布尔
        @remark {true, yes, 1} = true,{false, no, 0} = false
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 返回字符串中的,如果不存在则默认返回0
        */
        static void conv(const String & in, bool & out, bool o = false);

        /** 转换字符串到Angle
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @return 解析失败则返回0.0
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Radian & out, const Radian & o = Radian(0.0f));

        /** 从字符串中解析出Vector2f
        @remark 格式"x y",2个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Vector2f & out, const Vector2f & o = Vector2f::ZERO);

        /** 从字符串中解析出Vector3f
        @remark 格式"x y z", 3个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Vector3f & out, const Vector3f & o = Vector3f::ZERO);

        /** 从字符串中解析出Vector4f
        @remark 格式"x y z w", 4个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Vector4f & out, const Vector4f & o = Vector4f::ZERO);

        /** 从字符串中解析出Vector4i
        @remark 格式"x y z w", 4个NIIi
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Vector4i & out, const Vector4i & o = Vector4i::ZERO);

        /** 从字符串中解析出Matrix3f
        @remark 格式"00 01 02 10 11 12 20 21 22",'01'表示第0行第1列.
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Matrix3f & out, const Matrix3f & o = Matrix3f::IDENTITY);

        /** 从字符串中解析出Matrix4f
        @remark 格式"00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33",'01'表示第0行第1列.
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Matrix4f & out, const Matrix4f & o = Matrix4f::IDENTITY);

        /** 从字符串中解析出Quaternion
        @remark 格式"w x y z",4个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Quaternion & out, const Quaternion & o = Quaternion::IDENTITY);

        /** 从字符串中解析出Colour
        @remark 格式"r g b a" ,4个NIIf 或 "r g b" a值为1.0
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Colour & out, const Colour & o = Colour::Black);

        /** 从字符串中解析出Rectf
        @remark 格式"l t r b" ,4个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, Rectf & out, const Rectf & o = Rectf(0.0f));

        /** 从字符串中解析出RelPos
        @remark 格式"s f" ,2个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, RelPos & out, const RelPos & o = RelPos(1.0f, 0.0f));

        /** 从字符串中解析出RelRect
        @remark 格式"(ls,loft),(ts,toft),(rs,roft),(bs,boft)", 8个NIIf
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, RelRect & out, const RelRect & o = RelRect(RelPos(1.0f, 0.0f)));

        /** 从字符串中解析出 StringList
        @remark 分割空格
        @param[in] in 来源数据
        @param[out] out 输出数据
        @param[in] o 默认数据
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, StringList & out);

        /// 转换Ni16到字符串
        static const WString & conv(Ni16 in, WString & out);

        /// 转换Ni16到字符串
        static const VString & conv(Ni16 in, VString & out);

        /// 转换 Nui16 到字符串
        static const WString & conv(Nui16 in, WString & out);

        /// 转换 Nui16 到字符串
        static const VString & conv(Nui16 in, VString & out);

        /// 转换Ni32到字符串
        static const WString & conv(Ni32 in, WString & out);

        /// 转换Ni32到字符串
        static const VString & conv(Ni32 in, VString & out);

        /// 转换 Nui32 到字符串
        static const WString & conv(Nui32 in, WString & out);

        /// 转换 Nui32 到字符串
        static const VString & conv(Nui32 in, VString & out);

        /// 转换Ni64到字符串
        static const WString & conv(Ni64 in, WString & out);

        /// 转换Ni64到字符串
        static const VString & conv(Ni64 in, VString & out);

        /// 转换Nui64到字符串
        static const WString & conv(Nui64 in, WString & out);

        /// 转换Nui64到字符串
        static const VString & conv(Nui64 in, VString & out);

        /// 转换NIIf到字符串
        static const WString & conv(NIIf in, WString & out);

        /// 转换NIIf到字符串
        static const VString & conv(NIIf in, VString & out);

        /// 转换 NIId 到字符串
        static const WString & conv(NIId in, WString & out);

        /// 转换 NIId 到字符串
        static const VString & conv(NIId in, VString & out);

        /// 转换 Nul 到字符串
        static const String & conv(Nul in, String & out, Nui16 width = 0, NIIb fill = ' ',
            std::ios::fmtflags flags = std::ios::fmtflags(0));

        /// 转换 NIIl 到字符串
        static const String & conv(NIIl in, String & out, Nui16 width = 0, NIIb fill = ' ',
            std::ios::fmtflags flags = std::ios::fmtflags(0));

        /** 转换boolean到字符串
        @param[in] true/false
        @version NIIEngine 3.0.0
        */
        static const String & conv(bool in, String & out);

        /** 转换Vector2 到字符串
        @remark 格式"x y",2个NIIf
        @version NIIEngine 3.0.0
        */
        static const String & conv(const Vector2f & in, String & out);

        /** 转换Vector3 到字符串
        @remark 格式"x y z",3个NIIf
        @version NIIEngine 3.0.0
        */
        static const String & conv(const Vector3f & in, String & out);

        /** 转换Vector4 到字符串
        @remark 格式"x y z w",4个NIIf
        @version NIIEngine 3.0.0
        */
        static const String & conv(const Vector4f & in, String & out);

        /** 转换Vector4 到字符串
        @remark 格式"x y z w",4个NIIi
        @version NIIEngine 3.0.0
        */
        static const String & conv(const Vector4i & in, String & out);

        /** 转换Matrix3 到字符串
        @remark 格式"00 01 02 10 11 12 20 21 22",'01'表示行0列1
        @version NIIEngine 3.0.0
        */
        static const String & conv(const Matrix3f & in, String & out);

        /** 转换 Matrix4 到字符串
        @remark 格式"00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33",'01'表示行0列1
        @version NIIEngine 3.0.0
        */
        static const String & conv(const Matrix4f & in, String & out);

        /** 转换四元组到字符串
        @remark 格式"w x y z",4个NIIf
        @version NIIEngine 3.0.0
        */
        static const String & conv(const Quaternion & in, String & out);

        /** 转换Colour为字符串.
        @remark 格式"r g b a",4个NIIf
        @version NIIEngine 3.0.0
        */
        static const String & conv(const Colour & in, String & out);

        /** 转换Rectf为字符串.
        @remark 格式"l t r b",4个NIIf
        @version NIIEngine 3.0.0
        */
        static const String & conv(const Rectf & in, String & out);

        /** 转换RelPos为字符串.
        @remark 格式"s f",2个NIIf
        @version NIIEngine 3.0.0
        */
        static const String & conv(const RelPos & in, String & out);

        /** 转换RelRect为字符串.
        @remark 格式"(ls,loft),(ts,toft),(rs,roft),(bs,boft)", 8个NIIf
        @version NIIEngine 3.0.0
        */
        static const String & conv(const RelRect & in, String & out);

        /** 转换StringList为字符串
        @note 结果将被空格分开
        @version NIIEngine 3.0.0
        */
        static const String & conv(const StringList & in, String & out);

        /** 转换字符串到枚举 ScaleMode
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, ScaleMode & out);

        /** 转换枚举 ScaleMode 为字符串
        @version NIIEngine 3.0.0
        */
        static const String & conv(ScaleMode in, String & out);

        /** 转换字符串到枚举 ColourFactor
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, ColourFactor & out);

        /** 转换枚举 ColourFactor 为字符串
        @version NIIEngine 3.0.0
        */
        static const String & conv(ColourFactor in, String & out);

        /** 转换字符串到枚举 CmpMode
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, CmpMode & out);

        /** 转换枚举 CmpMode 为字符串
        @version NIIEngine 3.0.0
        */
        static const String & conv(CmpMode in, String & out);

        /** 转换字符串到枚举 ShaderLanguage
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, ShaderLanguage & out);

        /** 转换枚举 ShaderLanguage 为字符串
        @version NIIEngine 3.0.0
        */
        static const String & conv(ShaderLanguage in, String & out);

        /** UTF-8单字符串转使用的字符串
        @note
            一般在win32路径中使用.linux内核是使用ucs-4的,而且wchar_t是4个字节,不像win32中wchar_t是2个字节.
        @version NIIEngine 3.0.0
        */
        static void conv(const VString & in, String & out);

        /** UTF-8使用的字符串转单字符串
        @note
            一般在win32路径中使用.linux内核是使用ucs-4的,而且wchar_t是4个字节,不像win32中wchar_t是2个字节.
        @version NIIEngine 3.0.0
        */
        static void conv(const String & in, VString & out);

        /** 转换UTF-8编码到UCS-2编码
        @note
            一般在win32路径中使用.linux内核是使用ucs-4的,而且wchar_t是4个字节,不像win32中wchar_t是2个字节.
        @param[out] out 使用完后必须使用 N_free 释放掉
        @version NIIEngine 3.0.0
        */
        static void convUTF(const VString & in, Nui16 *& out);

        /** 转换UCS-2字符为字符串
        @note
            一般在win32路径中使用.linux内核是使用ucs-4的,而且wchar_t是4个字节,不像win32中wchar_t是2个字节.
        @version NIIEngine 3.0.0
        */
        static void convUTF(const Nui16 * in, VString & out);

        /** 检查字符串是有效的数值
        @version NIIEngine 3.0.0
        */
        static bool isNumber(const String & val);
    };

    template<typename T> N_FORCEINLINE String N_conv(T in)
    {
        String temp;
        return StrConv::conv(in, temp);
    }

    template<typename T> N_FORCEINLINE VString N_convV(T in)
    {
        VString temp;
        return StrConv::conv(in, temp);
    }

    template<typename T> N_FORCEINLINE WString N_convW(T in)
    {
        WString temp;
        return StrConv::conv(in, temp);
    }

    template<typename T> N_FORCEINLINE String N_conv(T in, T value)
    {
        String temp;
        return StrConv::conv(in, temp, value);
    }

    inline _EngineAPI String operator + (const String & src, Nui32 value)
    {
        String temp;
        return src + StrConv::conv(value, temp);
    }

    inline _EngineAPI String operator + (const String & src, Ni32 value)
    {
        String temp;
        return src + StrConv::conv(value, temp);
    }

    inline _EngineAPI String operator + (const String & src, NIIf value)
    {
        String temp;
        return src + StrConv::conv(value, temp);
    }

    inline _EngineAPI String operator + (const String & src, NIId value)
    {
        String temp;
        return src + StrConv::conv(value, temp);
    }

    inline _EngineAPI String operator + (Nui32 value, const String & src)
    {
        String temp;
        return StrConv::conv(value, temp) + src;
    }

    inline _EngineAPI String operator + (Ni32 value, const String & src)
    {
        String temp;
        return StrConv::conv(value, temp) + src;
    }

    inline _EngineAPI String operator + (NIIf value, const String & src)
    {
        String temp;
        return StrConv::conv(value, temp) + src;
    }

    inline _EngineAPI String operator + (NIId value, const String & src)
    {
        String temp;
        return StrConv::conv(value, temp) + src;
    }
}
#endif