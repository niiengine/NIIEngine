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

//��Դ�ļ���չ����Ϣ��, ����������ʼ����ģ��.
//xgettext -d NIIword -o NIIword.po -kNIII18N:1,2 -kNIII18N -s ...

//����һ���µ� .po �ļ�,��ʹ���û������е���Ϣ��������ļ����г�ʼ��.
//msginit -l zh_CN -i NIIword.po

//�ӷ����б�����һ�������Ƶ���Ϣ�б�.
//msgfmt NIIword.po -o Lang/zh_CN/LC_MESSAGES/NIIword.mo
//-----------------------------------------------------------

namespace NII
{
namespace NII_I18N
{
    /** i18n�� L10N��ʵ��
    @param[in] dest Ŀ���ļ�����
    @param[in] path ���·��
    */
    static void init(const String & dest = "NIIEngineApp",const String & path = "Lang");

    /// ��ȡЭ��
    static const String & getLisence();

    /// ��ȡ����
    static const String & getVerbose();
}
}
#endif