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

#include "NiiGettextI18nApp.h"
#include <locale.h>

namespace NII
{
namespace NII_I18N
{
    //-------------------------------------------------------------------
    void init(const String & dest, const String & path)
    {
        //默认把它设置为简体中文
        setlocale(LC_ALL,"");
        //设定国际化翻译包名称，省略了.mo
        textdomain (dest.c_str());
        //设定国际化翻译包所在位置
        bindtextdomain (dest.c_str(), path.c_str());
        //设置gettext()输出的字符串为utf-8编码
        bind_textdomain_codeset (dest.c_str(), "UTF-8");    
    }
    //-------------------------------------------------------------------
    const String & getLisence()
    {
    
    }
    //-------------------------------------------------------------------
    const String & getVerbose()
    {
    
    }
    //-------------------------------------------------------------------
}
}