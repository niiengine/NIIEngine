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

#ifndef _NII_REF_FUNCTOR_H_
#define _NII_REF_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiFunctor.h"

namespace NII
{
namespace NII_COMMAND
{
    /** �����ͺ���
    @remark
    @version NIIEngine 3.0.0
    */
    class RefFunctor : public Functor
    {
    public:
        RefFunctor(Functor & func);
        
        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);
        
        /// @copydetails Functor::clone
        Functor * clone() const;
    private:
        Functor & mReal;
    };
}
}
#endif