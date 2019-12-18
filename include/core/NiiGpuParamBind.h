/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-25

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

#ifndef _NII_GPU_PARAM_BIND_H_
#define _NII_GPU_PARAM_BIND_H_

#include "NiiPreInclude.h"
#include "NiiGpuParamDef.h"

namespace NII
{
    /** ��ɫ�����ڴ��
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuParamMemBind
    {
    public:
    public:
        GpuParamMemBind();
        GpuParamMemBind(Nidx memidx, NCount size, Nmark mark);

        Nidx mMemIndex;
        NCount mSize;
        Nmark mTypeMark;
    };
    typedef map<Nui32, GpuParamMemBind>::type GpuParamBindList;

    /** ͬ�������ڴ��
    @version NIIEngine 3.0.0
    */
    struct _EngineAPI GpuSyncParamBind
    {
    public:
        GpuSyncParamBind(GpuSyncParam type, Nidx memidx, NIIi input, Nmark mark, NCount count = 4);

        GpuSyncParamBind(GpuSyncParam type, Nidx memidx, NIIf input, Nmark mark, NCount count = 4);

        Nidx mMemIndex;
        GpuSyncParam mSyncParam;
        Nmark mTypeMark;
        NCount mUnitCount;
        union
        {
            NIIi mInputInt;
            NIIf mInputFloat;
        };
    };
    typedef vector<GpuSyncParamBind>::type SyncParamList;

    /** ��ɫ�����ڴ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamBindMap : public GpuParamsAlloc
    {
    public:
        GpuParamBindMap();

        GpuParamBindList mIntList;
        GpuParamBindList mFloatList;
        GpuParamBindList mDoubleList;
        NCount mIntSize;
        NCount mFloatSize;
        NCount mDoubleSize;
    };
}
#endif