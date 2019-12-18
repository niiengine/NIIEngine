/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-8

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

#ifndef _NII_JOYHANDLE_CONTROL_VECTOR_ARGS_H_
#define _NII_JOYHANDLE_CONTROL_VECTOR_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiJoyDevControlArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyHandleControlVectorArgs : public JoyDevControlArgs
    {
    public:
        JoyHandleControlVectorArgs(NIIi v, NIIf x, NIIf y, NIIf z) :
			JoyDevControlArgs(JDCT_Vector),
			mVector(v),
			mX(x),
			mY(y),
			mZ(z) {}

        NIIi mVector;   ///< vector����
        NIIf mX;        ///< x����
        NIIf mY;        ///< y����
        NIIf mZ;        ///< z����
    };
}
}
#endif