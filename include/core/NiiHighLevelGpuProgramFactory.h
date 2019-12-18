/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-31

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

#ifndef _NII_HighLevelGpuProgramFactory_H
#define _NII_HighLevelGpuProgramFactory_H

#include "NiiPreInclude.h"
#include "NiiFactoryObj.h"

namespace NII
{
    /** �߲���ɫ���򹤳���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI HighLevelGpuProgramFactory : public FactoryObj
    {
        friend class HighLevelGpuProgramManager;
    public:
        HighLevelGpuProgramFactory(const HighLevelGpuProgramManager * mag);
        virtual ~HighLevelGpuProgramFactory();

        /// @copydetails FactoryObj::getVFSType
        virtual Nmark getVFSType() const;

        /** ��������
        @version NIIEngine 3.0.0
        */
        virtual ShaderLanguage getLanguage() const = 0;

        /** ������ɫ����
        @version NIIEngine 3.0.0
        */
        virtual HighLevelGpuProgram * create(ResourceID rid, GroupID gid) = 0;
    protected:
        HighLevelGpuProgramManager * mManager;
    };
}
#endif