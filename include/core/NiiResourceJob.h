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

#ifndef _NII_ResourceJob_H_
#define _NII_ResourceJob_H_

#include "NiiPreInclude.h"
#include "NiiResource.h"
#include "NiiJob.h"

namespace NII
{
    /// ö�ٳ����������
    enum RequestType
    {
        RT_INITIALISE_GROUP = 0,        ///<
        RT_INITIALISE_ALL_GROUPS = 1,   ///<
        RT_PREPARE_GROUP = 2,           ///<
        RT_PREPARE_RESOURCE = 3,        ///<
        RT_LOAD_GROUP = 4,              ///<
        RT_LOAD_RESOURCE = 5,           ///<
        RT_UNLOAD_GROUP = 6,            ///<
        RT_UNLOAD_RESOURCE = 7          ///<
    };
    /** ��Դ����
    @version NIIEngine 3.0.0
    */
    class _EngineInner ResourceJob : public Job
    {
        friend class ResourceLoadSchemeManager;
    public:
        ResourceJob(RequestType type, void * data, Nui8 retry = 0, RequestID id = 0) :
            Job(id, data, retry),
            mType(type){}

        ~ResourceJob()
        {
            if(mData != 0)
            {
                N_free(mData);
                mData = 0;
            }
        }

        /** ��ȡ���������
        @version NIIEngine 3.0.0
        */
        inline RequestType getType() const { return mType; }
    protected:
        RequestType mType;
    };
    
    class _EngineInner ResourceJobResult : public JobResult
    {
        friend class ResourceLoadSchemeManager;
    public:
        ResourceJobResult(const Job * src, void * data, bool complete,
            const String & msg = StrUtil::WBLANK) :
                JobResult(src, data, complete, msg){}
        ~ResourceJobResult()
        {
            if(mData)
            {
                ResourceResponse * temp = static_cast<ResourceResponse *>(mData);
                N_delete temp;
                temp = 0;
            }
        }
    };
}
#endif