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

#ifndef _NII_DUALQuaternion_H_
#define _NII_DUALQuaternion_H_

#include "NiiPreInclude.h"
#include "NiiMath.h"

namespace NII
{
	/** ˫��Ԫ��
    @note ���ڹ�����Ƥ
    @version NIIEngine 3.0.0
	*/
	class _EngineAPI DualQuaternion
	{
	public:
        DualQuaternion();
        DualQuaternion(NIIf * data);
        DualQuaternion(NIIf w, NIIf x, NIIf y, NIIf z, NIIf dw, NIIf dx, NIIf dy, NIIf dz);
        DualQuaternion(const Quaternion & rot, const Vector3f & oft);
        DualQuaternion(const Matrix4f & rotspace);

        /** ��ȡֵ
        @version NIIEngine 3.0.0
        */
		inline NIIf operator [](NCount i) const
		{
			return *(&w+i);
		}

        /** ��ȡֵ
        @version NIIEngine 3.0.0
        */
		inline NIIf & operator [](NCount i)
		{
			return *(&w+i);
		}

        /** ��ֵ
        @version NIIEngine 3.0.0
        */
        DualQuaternion & operator= (const DualQuaternion & o);

        /** �Ƿ����
        @version NIIEngine 3.0.0
        */
        bool operator== (const DualQuaternion & o) const;

        /** �Ƿ񲻵�
        @version NIIEngine 3.0.0
        */
        bool operator!= (const DualQuaternion & o) const;

        /** ����ֵ
        @version NIIEngine 3.0.0
        */
        void swap(DualQuaternion & o);

		/** ����Ԫ���ƽ���й���
        @version NIIEngine 3.0.0
        */
		void from(const Quaternion & q, const Vector3f & oft);

		/** ��������Ԫ���ƽ��
        @version NIIEngine 3.0.0
        */
		void to(Quaternion & q, Vector3f & oft) const;

		/** ͨ���ռ���󹹽�
        @version NIIEngine 3.0.0
        */
		void from(const Matrix4f & rotspace);

		/** �������ռ����
        @version NIIEngine 3.0.0
        */
		void to(Matrix4f & rotspace) const;

		/** �Ƿ���Ч
        @version NIIEngine 3.0.0
        */
        bool isNaN() const;
	public:
		NIIf w, x, y, z, dw, dx, dy, dz;
	};

    inline _EngineAPI std::ostream & operator << (std::ostream & o, const DualQuaternion & q)
    {
        o << "DualQuaternion(" << q.w << "," << q.x << "," << q.y << "," << q.z << ","
            << q.dw << "," << q.dx << "," << q.dy << "," << q.dz << ")";
        return o;
    }
}
#endif
