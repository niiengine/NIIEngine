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

#include "NiiPreInclude.h"

namespace NII
{
    /* ��������,yaw * pitch * roll. yaw����������ת.pitch��������ת,roll�ķ�������ת.
	@version NIIEngine 3.0.0
    */
	extern _EngineAPI bool eulerXYZ(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerXZY(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerYXZ(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerYZX(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerZXY(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerZYX(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI void eulerXYZ(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerXZY(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerYXZ(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerYZX(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerZXY(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerZYX(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);
}