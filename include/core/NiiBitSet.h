/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#ifndef _NII_IO_VALUE_MAP_H_
#define _NII_IO_VALUE_MAP_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"

#if N_ARCH == N_ARCH_32
    #define IOTemp      Nui16
    #define IOMark      5
    #define IOValid     32
    #define IOByteCount 4
#elif N_ARCH == N_ARCH_64
    #define IOTemp      Nui16
    #define IOMark      6
    #define IOValid     64
    #define IOByteCount 8
#elif N_ARCH == N_ARCH_128
    #define IOTemp      Nui32
    #define IOMark      7
    #define IOValid     128
    #define IOByteCount 16
#elif N_ARCH == N_ARCH_256
    #define IOTemp      Nui32
    #define IOMark      8
    #define IOValid     256
    #define IOByteCount 32
#elif N_ARCH == N_ARCH_512
    #define IOTemp      Nui32
    #define IOMark      9
    #define IOValid     512
    #define IOByteCount 64
#elif N_ARCH == N_ARCH_1024
    #define IOTemp      Nui32
    #define IOMark      10
    #define IOValid     1024
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
        typedef struct _IOCell
        {
            IOType data;
            IOTemp mark;
        }IOCell;
    public:
        BitSet();
        BitSet(NCount count);
        ~BitSet();

        /** �����С
        @remark
            ÿ�δ�С�ı��ʱ�򶼱�������������,ÿ�ε�������������������������,
            ������󲻰�������������,���Բ�����ʱ�����ȷ��������Χ��������Χ��
        @param[in] count IO����
        */
        void resize(NCount count);

        /** ����ָ��ioλ�õ�ֵΪ��
        @param[in] ioλ��(0��ʼ)
        */
        void setTrue(Nidx io);
        
        /** ����ָ��ioλ�õ�ֵΪ��
        @param[in] r λ����
        @param[in] sr ��Χ
        */
        void setTrue(Nidx r, Nidx sr);

        /** ����ָ��ioλ�õ�ֵΪ��
        @param[in] ioλ��(0��ʼ)
        */
        void setFalse(Nidx io);
        
        /** ����ָ��ioλ�õ�ֵΪ��
        @param[in] r λ����
        @param[in] sr ��Χ
        */
        void setFalse(Nidx r, Nidx sr);
        
        /** ���ָ��ioλ�õ�ֵ�Ƿ�Ϊ��
        @param[in] ioλ��(0��ʼ)
        @return ��ֵ״̬
        */
        inline bool isTrue(Nidx io)
        {
            return (mData[io >> IOMark].data & NiiOrMark[io % IOValid]) != 0;
        }

        /** ���ָ��ioλ�õ�ֵ�Ƿ�Ϊ��
        @param[in] r λ����
        @param[in] sr ��Χ
        @return ��ֵ״̬
        */
        inline bool isTrue(Nidx r, Nidx sr)
        {
            return (mData[r].data & NiiOrMark[sr]) != 0;
        }

        /** ��ȡָ��[0-io]λ�������ķ�Χ�������Чλ(0��ʼ)
        @param[in] ioλ��(0��ʼ)
        @return ��������ڣ��򷵻� -1 ֵ
        */
        NCount getMostSignificant(Nidx io);
        
        /** ��ȡָ����Χ�ڵ�IO�����Чλ(0��ʼ)
        @param[in] r λ����
        @param[in] sr ��Χ
        @return ��������ڣ��򷵻� -1 ֵ
        */
        NCount getMostSignificant(Nidx r, Nidx sr);
        
        /** ��ȡָ��[0-io]λ�������ķ�Χ�ڵ�IO��ֵ����
        @param[in] ioλ��(����)
        @return ��λ��ǰ��������ֵ����
        */
        NCount getCount(Nidx io);

        /** ��ȡָ����Χ�ڵ�IO��ֵ����
        @param[in] r λ����
        @param[in] sr ��Χ
        @return ��λ��ǰ��������ֵ����
        */
        NCount getCount(Nidx r, Nidx sr);
    private:
        IOCell * mData;                    ///< ���ڴ�
        NCount mCount;
    };
}
#endif