/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-11-4

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

#ifndef _NII_DATA_TYPE_H_
#define _NII_DATA_TYPE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ����Ԫ������,����ȷ���������ݵĻ�������
    @version NIIEngine 3.0.0
    */
    enum DataType
    {
        DT_Float1x = 0,         ///< ��1������Ϊ��λ��
        DT_Float2x = 1,         ///< ��2������Ϊ��λ��
        DT_Float3x = 2,         ///< ��3������Ϊ��λ��
        DT_Float4x = 3,         ///< ��4������Ϊ��λ��
        DT_Colour = 4,          ///< ��ɫ��ʽ(һ����32λ)
        DT_Colour_ARGB = 5,     ///< ��ɫ��ʽ(D3D����)
        DT_Colour_ABGR = 6,     ///< ��ɫ��ʽ(GL����)
        DT_Short1x = 7,         ///< ��1��������Ϊ��λ��
        DT_Short2x = 8,         ///< ��2��������Ϊ��λ��
        DT_Short3x = 9,         ///< ��3��������Ϊ��λ��
        DT_Short4x = 10,        ///< ��4��������Ϊ��λ��
        DT_Vector2f = 11,       ///< ��2������Ϊ��λ��
        DT_Vector3f = 12,       ///< ��3������Ϊ��λ��
        DT_Vector4f = 13,       ///< ��4������Ϊ��λ��
        DT_Matrix3f = 14,       ///< ��9������Ϊ��λ��
        DT_Matrix4f = 15,       ///< ��16������Ϊ��λ��
        DT_U_Byte = 16,         ///< �޷���(32λ)
        DT_Unkonw = 17
    };
    
    /** �������͸���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DataTypeUtil
    {
    public:
        /** ��ȡ�ɷִ�С
        @param[in] type ���ݳɷ�
        @version NIIEngine 3.0.0
        */
        static NCount getSize(DataType type);

        /** ��ȡ�ɷֻ�������
        @param[in] type ���ݳɷ�
        @version NIIEngine 3.0.0
        */
        static NCount getCount(DataType type);

        /** �򵥵�ת��������,��
        @param[in] type ���ϻ����ɷ�
        @param[out] count �����ɷ�����
        @version NIIEngine 3.0.0
        */
        static DataType getBaseType(DataType type, NCount & count);

        /** ��ȡ�ϳ�����
        @param[in] type �����ɷ�
        @param[in] count �����ɷ�����
        @version NIIEngine 3.0.0
        */
        static DataType getMultiType(DataType type, NCount count);
    };
}

#endif