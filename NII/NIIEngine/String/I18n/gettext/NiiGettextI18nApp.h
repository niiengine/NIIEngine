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

#ifndef _NII_GETTEXT_I18N_LIB_H_
#define _NII_GETTEXT_I18N_LIB_H_

#include <libintl.h>

#define _T(text) gettext(text)

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
    static void init(const String & dest = "NIIEngineApp",const String & path = "Lang");

    /// 获取协议
    static const String & getLisence();

    /// 获取描述
    static const String & getVerbose();
}
}
#endif