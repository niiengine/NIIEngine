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

#ifndef _NII_METHOD_FUNCTOR_H_
#define _NII_METHOD_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiFunctor.h"

namespace NII
{
namespace NII_COMMAND
{
	typedef void(Event::*EventMethod)(const EventArgs *);

	/** ��ͨ�ĺ���
	@remark
		ӵ�� bool ���غ� const EventArgs * ��Ϊ��������ͨ����
	@version NIIEngine 3.0.0
	*/
	class _EngineAPI MethodFunctor : public Functor
	{
	public:
		MethodFunctor(Event * obj, EventMethod m);

		/// @copydetails Functor::execute
		bool execute(const EventArgs * args);

		/// @copydetails Funcotr::clone
		Functor * clone() const;
	protected:
		Event * mObj;
		EventMethod mMethod;
	};
}
}
#endif