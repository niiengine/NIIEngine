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
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_ID_HELPER_H_
#define _NII_ID_HELPER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �����ű���������
    @remark
    */
    class _EngineAPI IDHelper : public IDAlloc
    {
    public:
        typedef map<String, ChildID>::type IDMaps;
    public:
        IDHelper();
        ~IDHelper();

        void add(const String & name, ChildID id, const String & help);
    protected:
        ClassID mPID;
        ClassID mID;
        IDMaps mIDMaps;
    };
};

#ifndef N_IDHelp
#define N_IDHelp IDHelper mIDHelper
#endif

#ifndef N_IDHelp_Def
#define N_IDHelp_Def(name, id, help) mIDHelper.add(name, id, help)
#endif

#endif