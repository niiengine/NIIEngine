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
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_SERIAL_ITEM_H_
#define _NII_SERIAL_ITEM_H_

#include "NiiPreInclude.h"
#include "NiiFunctor.h"

namespace NII
{
namespace NII_MEDIA
{
    /** �����豸�����¼�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SerialItemArgs : public EventArgs
    {
    public:
        virtual ~SerialItemArgs() {}

        /** ��ɱȽ�
        @remark ���������д�������
        */
        virtual bool operator == (const SerialItemArgs * o) const = 0;

        /** ����ɱȽ�
        @remark ���������д�������
        */
        virtual bool operator != (const SerialItemArgs * o) const = 0;
    };
    
    /** ���е�Ԫ
    @version NIIEngine 3.0.0
    */
    class SerialItem : public ControlAlloc
    {
    public:
        typedef vector<const SerialItemArgs *>::type ItemSerial;
    public:
        SerialItem(NII_COMMAND::Functor * func);
        ~SerialItem() {}

        /** ���һ�����е�Ԫ
        @remark ������󲻸������һ���ڴ��ͷ�
        @param[in] item Ҫ������е����е�Ԫ
        */
        void add(const SerialItemArgs * item);

        /**  ���һ�����е�Ԫ
        @remark 
            ������󲻸�����������ڴ��ͷ�,�������һ��λ�����Ѿ���Ԫ��,���Ḳ��,����
            Ȼ�ǲ���ԭ����ʹ�õ��ڴ��ͷ�
        @param[in] pos Ҫ������е�λ��,������0��ʼ
        @param[in] item Ҫ������е����е�Ԫ
        */
        void add(NCount pos, const SerialItemArgs * item);

        /** ��ȥ���һ�����е�Ԫ
        @remark ������󲻸�����ȥ������ڴ��ͷ�
        @param[in] pos ��Ҫ��ȥ�����е�Ԫ��λ��
        */
        void remove(NCount pos);

        /** ������ṹ
        @remark ʹ����add��remove����������������,��������ȷ������
        */
        void compile();

        /** ���
        @remark
        @param[in] arg ����
        */
        bool check(const EventArgs * arg);
    private:
        ItemSerial mItemSerial;
        ItemSerial::iterator mBegin;
        ItemSerial::iterator mCurrent;
        ItemSerial::iterator mEnd;
        NII_COMMAND::Functor * mFunc;
    };
}
}
#endif