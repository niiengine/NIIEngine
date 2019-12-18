/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-1

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

#ifndef _NII_GPU_PREDEFINE_VALUE_H_
#define _NII_GPU_PREDEFINE_VALUE_H_

#include "NiiPreInclude.h"
#include "NiiGpuParamDef.h"

namespace NII
{
    /** Ԥ������ɫ����ֵ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuPreDefineValue : public GpuParamsAlloc
    {
    public:
        GpuPreDefineValue(const String & name);
        virtual ~GpuPreDefineValue();

        /** ��������
        @version NIIEngine 3.0.0
        */
        const String & getName();

        /** ��ӱ���
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, GpuDataType type, NCount count = 1);

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        const GpuParamDef & get(const VString & name) const;

        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void remove(const VString & name);

        /** ��ȥ���б���
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NIIi in);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, NIIf in);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Colour & in);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Vector4f & in);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Vector3f & in);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIIi * in, NCount count);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIIf * in, NCount count);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIId * in, NCount count);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Matrix4f & in);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Matrix4f * in, NCount count);

        /** ��ȡ�����б�
        @version NIIEngine 3.0.0 �߼�api
        */
        const GpuParamDefines & getDefList() const;

        /** ��ȡ״̬����
        @version NIIEngine 3.0.0
        */
        Nul getStateMark() const;

        /** ��ȡ���㲿������
        @version NIIEngine 3.0.0
        */
        NIIf * getFloatData(NCount pos);

        /** ��ȡ���㲿������
        @version NIIEngine 3.0.0
        */
        const NIIf * getFloatData(NCount pos) const;

        /** ��ȡ���β�������
        @version NIIEngine 3.0.0
        */
        NIId * getDoubleData(NCount pos);

        /** ��ȡ���β�������
        @version NIIEngine 3.0.0
        */
        const NIId * getDoubleData(NCount pos) const;

        /** ��ȡ���β�������
        @version NIIEngine 3.0.0
        */
        NIIi * getIntData(NCount pos);

        /** ��ȡ���β�������
        @version NIIEngine 3.0.0
        */
        const NIIi * getIntData(NCount pos) const;
    protected:
        String mName;
        GpuParamDefines mDefines;
        IntArray mIntData;
        FloatArray mFloatData; 
        DoubleArray mDoubleData;
        NCount mIntSize;
        NCount mFloatSize;
        NCount mDoubleSize;
        Nmark mDirtyMark;
    };
}

#endif