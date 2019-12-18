/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
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