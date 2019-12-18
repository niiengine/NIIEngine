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

#ifndef _NII_IO_VALUE_MAP_H_
#define _NII_IO_VALUE_MAP_H_

#include "NiiPreInclude.h"

#if NII_ARCH_TYPE == N_ARCH_32
    #define IOTemp      Nui16
    #define IOMark      5
    #define IOValid     31
    #define IOByteCount 4
#elif NII_ARCH_TYPE == N_ARCH_64
    #define IOTemp      Nui16
    #define IOMark      6
    #define IOValid     63
    #define IOByteCount 8
#elif NII_ARCH_TYPE == N_ARCH_128
    #define IOTemp      Nui16
    #define IOMark      7
    #define IOValid     127
    #define IOByteCount 16
#elif NII_ARCH_TYPE == N_ARCH_256
    #define IOTemp      Nui16
    #define IOMark      8
    #define IOValid     255
    #define IOByteCount 32
#elif NII_ARCH_TYPE == N_ARCH_512
    #define IOTemp      Nui16
    #define IOMark      9
    #define IOValid     511
    #define IOByteCount 64
#elif NII_ARCH_TYPE == N_ARCH_1024
    #define IOTemp      Nui16
    #define IOMark      10
    #define IOValid     1023
    #define IOByteCount 128
#else
    #error ���ִ�����
#endif

namespace NII
{
    /** ֵӳ��
    @remark ���ڱ�������Ƿ����,�Ƿ���Ч
    @version NIIEngine 3.0.0
    */
    class _EngineInner BitSet : public StreamAlloc
    {
    public:
        typedef struct _Com
        {
            IOType data;
            IOTemp mark;
        }Com;
    public:
        BitSet();
        BitSet(NCount count);
        ~BitSet();

        /** �����С
        @remark
            ÿ�δ�С�ı��ʱ�򶼱�������������,ÿ�ε�������������������������û,
            ������󲻰�������������,���Բ�����ʱ�����ȷ��������Χ��������Χ��
        @param[in] count IO����
        */
        void resize(NCount count);

        /** ����ָ��ioλ�õ�ֵΪ��
        @param[in] r λ����
        @param[in] sr ��Χ
        */
        void add(NCount r, NCount sr);

        /** ����ָ��ioλ�õ�ֵΪ��
        @param[in] r λ����
        @param[in] sr ��Χ
        */
        void remove(NCount r, NCount sr);

        /** ��ȡָ��ioλ�������ķ�Χ�ڵ�IO��ֵ����
        @param[in] ioλ��
        @return ��λ��ǰ��������ֵ����
        */
        NCount get(NCount io);

        /** ��ȡָ����Χ�ڵ�IO��ֵ����
        @param[in] r λ����
        @param[in] sr ��Χ
        @return ��λ��ǰ��������ֵ����
        */
        NCount get(NCount r, NCount sr);

        /** ���ָ��ioλ�õ�ֵ�Ƿ�Ϊ��
        @param[in] ioλ��
        @return ��ֵ״̬
        */
        inline bool check(NCount io)
        {
            return (mData[io >> IOMark].data & NiiOrMark[io % IOValid]) != 0;
        }

        /** ���ָ��ioλ�õ�ֵ�Ƿ�Ϊ��
        @param[in] r λ����
        @param[in] sr ��Χ
        @return ��ֵ״̬
        */
        inline bool check(NCount r, NCount sr)
        {
            return (mData[r].data & NiiOrMark[sr]) != 0;
        }

        /** ����ָ��ioλ�õ�ֵΪ��
        @param[in] ioλ��
        */
        inline void setTrue(NCount io)
        {
            mData[io >> IOMark].data |= NiiOrMark[io % IOValid];
        }

        /** ����ָ��ioλ�õ�ֵΪ��
        @param[in] ioλ��
        */
        inline void setFalse(NCount io)
        {
            mData[io >> IOMark].data &= NiiNotMark[io % IOValid];
        }
    private:
        Com * mData;                    ///< ���ڴ�
        NCount mCount;
    };
}
#endif