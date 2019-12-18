/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-24

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

#ifndef _NII_VERTEX_ELEMENT_H_
#define _NII_VERTEX_ELEMENT_H_

#include "NiiPreInclude.h"
#include "NiiColour.h"
#include "NiiDataType.h"

namespace NII
{
    /** ����Ԫ�ص�����,����ʶ�𶥵㻺�����������
    @version NIIEngine 3.0.0
    */
    enum VertexType
    {
        VT_Pos = 1,             ///< λ��,3��NIIf
        VT_Matrix_Weight = 2,   ///< ���Ȩ��
        VT_Matrix_Index = 3,    ///< �������
        VT_Normals = 4,         ///< ����,3��NIIf
        VT_Diffuse = 5,         ///< ��������ɫ
        VT_Specular = 6,        ///< ���淴����ɫ
        VT_Tex_Coord = 7,       ///< ��������
        VT_Space = 8,           ///< ���㸨���ռ�
        VT_Binormal = 9,        ///< ������(Y��,��������Zʱ)
        VT_Tangent = 10,        ///< ���� (X��,��������Zʱ)
        VT_Count = 11
    };

    /**
    @remark
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexCom : public VertexDataAlloc
    {
        friend class VertexData;
    public:
        VertexCom();

        VertexCom(DataType dt, VertexType vt, Nui16 bidx, NCount oft);

        VertexCom(DataType dt, VertexType vt, Nui16 bidx, NCount oft, Nui16 eidx);

        bool operator== (const VertexCom & o) const;

        /** ��ȡ����
        @param[in] ��������
        @param[out] ʵ��λ������
        @version NIIEngine 3.0.0
        */
        inline void getData(const void * in, Nui8 *& out) const
        {
            out = (Nui8 *)in + mOffset;
        }

        /** ��ȡ����
        @param[in] ��������
        @param[out] ʵ��λ������
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, void *& out) const
        {
            out = (void *)(in + mOffset);
        }

        /** ��ȡ����
        @param[in] ��������
        @param[out] ʵ��λ������
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui8 *& out) const
        {
            out = (Nui8 *)(in + mOffset);
        }

        /** ��ȡ����
        @param[in] in ��������
        @param[out] ʵ��λ������
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui16 *& out) const
        {
            out = (Nui16 *)(in + mOffset);
        }

        /** ��ȡ����
        @param[in] in ��������
        @param[out] out ʵ��λ������
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui32 *& out) const
        {
            out = (Nui32 *)(in + mOffset);
        }

        /** ��ȡ����
        @param[in] in ��������
        @param[out] out ʵ��λ������
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, NIIf *& out) const
        {
            out = (NIIf *)(in + mOffset);
        }

        /** ��ȡ���Ԫ�ص��ֽڴ�С
        @version NIIEngine 3.0.0
        */
        NCount getSize() const;

        /** ת������������ɫ����
        @param[in] src DT_Colour DT_Colour_ABGR �� DT_Colour_ARGB.
        @param[in] dst DT_Colour_ABGR �� DT_Colour_ARGB.
        */
        static void conv(VertexData * obj, DataType src, DataType dest);
    public:
        DataType mType;         ///< Ԫ������
        VertexType mVType;      ///< Ԫ�صĺ���
        NCount mOffset;         ///< ����ƫ��
        Nui16 mSource;          ///< ���ڵĻ���
        Nui16 mIndex;           ///< �������,�����ڸ���Ԫ��,����������Ԫ��
    };
}
#endif