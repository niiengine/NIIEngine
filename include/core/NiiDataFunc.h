/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

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

#ifndef _NII_DADA_FUNC_H_
#define _NII_DADA_FUNC_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���ݺ���
    @remark ����ӳ������
    @version NIIEngine 3.0.0
    */
    template <typename in, typename out> class DataFunc : public DataAlloc
    {
    public:
        DataFunc(bool autoDsy) : 
            mAutoDestroy(autoDsy)
        {
        }

        virtual ~DataFunc() {}

        /** ����
        @param[in] src ����ֵ
        @return ���ֵ
        @version NIIEngine 3.0.0
        */
        virtual out func(const in & src) = 0;
        
        /** �����Ƿ���Ч
        @version NIIEngine 3.0.0
        */
        virtual bool isValid() const { return true; }
        
        /** �����Ƿ��Զ�ɾ������
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b){ mAutoDestroy = b; }
        
        /** ��ȡ�Ƿ��Զ�ɾ������
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const{ return mAutoDestroy; }
    protected:
        bool mAutoDestroy;
    };
}
#endif