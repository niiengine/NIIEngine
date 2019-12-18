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
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_GETTEXT_I18N_LIB_H_
#define _NII_GETTEXT_I18N_LIB_H_

#include <libintl.h>

#define _T(text) dgettext("NIIEngine", text)

//从源文件中展开消息行, 用来创建起始翻译模板.
//xgettext -d NIIword -o NIIword.po -kNIII18N:1,2 -kNIII18N -s ...

//生成一个新的 .po 文件,并使用用户环境中的消息来对这个文件进行初始化.
//msginit -l zh_CN -i NIIword.po

//从翻译列表生成一个二进制的消息列表.
//msgfmt NIIword.po -o Lang/zh_CN/LC_MESSAGES/NIIword.mo
//-----------------------------------------------------------

namespace NII
{
namespace NII_I18N
{
    /** i18n和 L10N的实现
    @param[in] dest 目标文件名字
    @param[in] path 相对路径
    */
    static void init(const String & dest = "NIIEngine",const String & path = "Lang");

    /// 获取协议
    static const String & getLisence();

    /// 获取描述
    static const String & getVerbose();
}
}
#endif