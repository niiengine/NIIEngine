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
        //Ĭ�ϰ�������Ϊ��������
        setlocale(LC_ALL,"");
        //�趨���ʻ���������ƣ�ʡ����.mo
        textdomain (dest.c_str());
        //�趨���ʻ����������λ��
        bindtextdomain (dest.c_str(), path.c_str());
        //����gettext()������ַ���Ϊutf-8����
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