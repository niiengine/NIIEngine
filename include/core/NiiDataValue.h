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

#ifndef _NII_DATA_VALUE_H_
#define _NII_DATA_VALUE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ����.
    @version NIIEngine 3.0.0
    */
    template <typename T> class DataValue : public DataAlloc
    {
    public:
        DataValue(bool autoDsy) :
            mAutoDestroy(autoDsy)
        {
        }

        virtual ~DataValue() {}

        /** ��ȡֵ
        @remark version NIIEngine 3.0.0
        */
        virtual const T getValue() const = 0;

        /** ����ֵ
        @remark version NIIEngine 3.0.0
        */
        virtual void setValue(const T & v) = 0;

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