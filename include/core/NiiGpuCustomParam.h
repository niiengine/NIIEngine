/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-7

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

#ifndef _NII_GPU_CUSTOM_PARAM_H_
#define _NII_GPU_CUSTOM_PARAM_H_

#include "NiiPreInclude.h"
#include "NiiGpuParamBind.h"

namespace NII
{
    /** ��ɫ�����Զ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuCustomParam
    {
    public:
        GpuCustomParam();
        ~GpuCustomParam();

        /** Ϊ�������Ⱦ������һ���Զ������,�����Ƶ��������ָ���Ŀ���Ⱦ��,
            ��GPU��ɫ�����������
        @param[in] index
        @param[in] value
        @version NIIEngine 3.0.0
        */
        void setCustonParam(Nidx index, const Vector4f & value);

        /** ����������ȡ�����������Ⱦ����Զ���ֵ
        @param[in] index ����ʹ�õ�����
        @version NIIEngine 3.0.0
        */
        const Vector4f & getCustomParam(Nidx index) const;

        /** ��ȥ�Զ������
        @param[in] index ����ʹ�õ�����
        @version NIIEngine 3.0.0
        */
        void removeCustomParam(Nidx index);

        /** �Ƿ�����Զ������
        @param[in] index ����ʹ�õ�����
        @version NIIEngine 3.0.0
        */
        bool isCustomParamExist(Nidx index) const;

        /** �����Զ��������ʵ�ʵ���ɫ���������
        @note ����ʹ�õ����� bind ��� mInputInt
        @param[in] dst ��Ҫ���µ���ɫ���������
        @param[in] bind ��Ҫ������ɫ�������
        @version NIIEngine 3.0.0
        */
        virtual void updateCustom(GpuProgramParam * dst, const GpuSyncParamBind & bind) const;
        
        /** �����Զ��������ʵ�ʵ���ɫ���������
        @param[in] dst ��Ҫ���µ���ɫ���������
        @param[in] index ����ʹ�õ�����
        @param[in] bind ��Ҫ������ɫ�������
        @version NIIEngine 3.0.0
        */
        virtual void updateCustom(GpuProgramParam * dst, Nidx index, const GpuSyncParamBind & entry) const;
    protected:
        typedef map<Nidx, Vector4f>::type CustomParameterMap;
    protected:
        CustomParameterMap mCustomParameters;
    };
}
#endif