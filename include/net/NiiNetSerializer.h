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

#ifndef _NII_NET_SERIALIZER_H_
#define _NII_NET_SERIALIZER_H_

#include "NiiNetPreInclude.h"
#include "NiiSerializer.h"

namespace NII
{
    /** ����������
    @remark
        ����ͨ�ļ���������ͬ����,�����֧��λд����������,����Ϊ������Դ����,����
        ��ʹ����λ��Ϊ��λȥ���������Ͳ������8��λ������.����һ�����������ռ�8��λ
        ����Ĳ����ŵ�ͬһ���ֽ���,������֤Ч�ʵ�ͬʱ,Ҳ��ʡ�˺ܶ�������Դ.���һ���
        һ����Ƿ�ת�ֽ��򲢲���һ��Ч�ʸߵĶ���
    @par
        �е���Ҫע�����,����������д��˳���,����д���˳������ô��,��ȡ��˳�����
        ��ô��
    @par
        ����memcpy��c������������byteΪ����(���ܻ��ܲ��м���),������������������
        ��������ͨ�������������ĸ��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NetSerializer : public Serializer
    {
    public:
        NetSerializer();
        NetSerializer(NCount size);
        NetSerializer(const Nui8 * data, NCount size, bool copy);
        ~NetSerializer();

        /**
        @param[in] in
        @vesion NIIEngine 3.0.0
        */
        void write(const NetSerializer * in);

        /**
        @param[in] in
        @param[in] count
        @vesion NIIEngine 3.0.0
        */
        void write(const NetSerializer * in, NBitCount count);

        /** д��һ��λ
        @remark ��ռһ��λ,Ҫ��read(bool &)
        @version NIIEngine 3.0.0
        */
        void write0();

        /** д��һ��λ
        @remark ��ռһ��λ,Ҫ��read(bool &)
        @version NIIEngine 3.0.0
        */
        void write1();

        /** д��һ���ֽ�
        @remark �ٶȱȻ����д���һ���
        @version NIIEngine 3.0.0
        */
        void write(Nui8 in);

        /** д������ֽ�
        @remark �ٶȱȻ����д���һ���
        @param[in] in
        */
        void write(Nui16 in);

        /** д���ĸ��ֽ�
        @remark �ٶȱȻ����д���һ���
        @param[in] in
        @version NIIEngine 3.0.0
        */
        void write(Nui32 in);

        /** д��һ������
        @remark ������һ��
        @version NIIEngine 3.0.0
        */
        void writeCompress(NIIf in);

        /**
        @remark ������һ��
        @version NIIEngine 3.0.0
        */
        void writeCompress(NIId in);

        /**
        @version NIIEngine 3.0.0
        */
        void writeAlign();

        /**
        @version NIIEngine 3.0.0
        */
        void writeAlign(NCount count);

        /**
        @param[in] in
        @param[in] count
        @param[in] Ralign
        @version NIIEngine 3.0.0
        */
        void writeBit(const Nui8 * in, NBitCount count, bool Ralign = true);

        /**
        @param[in] in ����λ����
        @param[in] count
        @param[in] flip
        @version NIIEngine 3.0.0
        */
        void writeCompress(const Nui8 * in, NBitCount count, bool flip = true);

        /// @copydetails Serializer::write
        NCount write(const void * in, NCount size, NCount count, bool flip = true);

        /**
        @vesion NIIEngine 3.0.0
        */
        NBitCount read(NetSerializer * out) const;

        /**
        @vesion NIIEngine 3.0.0
        */
        NBitCount read(NetSerializer * out, NBitCount count) const;

        /**
        @remark �ٶȱȻ���Ķ�ȡ��һ���
        @version NIIEngine 3.0.0
        */
        NBitCount read(bool & out) const;

        /**
        @remark �ٶȱȻ���Ķ�ȡ��һ���
        @version NIIEngine 3.0.0
        */
        NCount read(Nui8 & out) const;

        /**
        @remark �ٶȱȻ���Ķ�ȡ��һ���
        @version NIIEngine 3.0.0
        */
        NCount read(Nui16 & out) const;

        /**
        @remark �ٶȱȻ���Ķ�ȡ��һ���
        @param[in] in
        @version NIIEngine 3.0.0
        */
        NCount read(Nui32 & out) const;

        /**
        @remark ������һ��
        @version NIIEngine 3.0.0
        */
        NCount readCompress(NIIf & out) const;

        /**
        @remark ������һ��
        @version NIIEngine 3.0.0
        */
        NCount readCompress(NIId & out) const;

        /**
        @version NIIEngine 3.0.0
        */
        NBitCount readAlign() const;

        /**
        @version NIIEngine 3.0.0
        */
        NBitCount readAlign(NCount count) const;

        /**
        @param[out] out
        @param[in] count
        @param[in] Ralign
        @version NIIEngine 3.0.0
        */
        NBitCount readBit(Nui8 * out, NBitCount count, bool Ralign = true) const;

        /** д��һ��ѹ��
        @param[in] out
        @param[in] count ����λ����
        @param[in] flip
        @version NIIEngine 3.0.0
        */
        NBitCount readCompress(Nui8 * out, NBitCount count, bool flip = true) const;

        /// @copydetails Serializer::read
        NCount read(void * out, NCount size, NCount count, bool flip = true) const;

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void seekWrite(NBitCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void skipWrite(NBitCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void seekRead(NBitCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void skipRead(NBitCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */

        void enlargeByte(NCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void enlargeBit(NBitCount count);

        /** ��ȡ��λ�ĵ�һ�ֽ�
        @remark ���ұ߿�ʼ
        @param[in] data
        @param[in] byteskip
        @version NIIEngine 3.0.0
        */
        bool get1Hbit(const Nui8 * data, NCount byteskip = 0);

        /** ��ȡ��λ�ĵڶ��ֽ�
        @remark ���ұ߿�ʼ
        @param[in] data
        @param[in] byteskip
        @version NIIEngine 3.0.0
        */
        bool get2Hbit(const Nui8 * data, NCount byteskip = 0);

        /** ��ȡ��λ�ĵ����ֽ�
        @remark ���ұ߿�ʼ
        @param[in] data
        @param[in] byteskip
        @version NIIEngine 3.0.0
        */
        bool get3Hbit(const Nui8 * data, NCount byteskip = 0);

        /** ��ȡ��λ�ĵ����ֽ�
        @remark ���ұ߿�ʼ
        @param[in] data
        @param[in] byteskip
        @version NIIEngine 3.0.0
        */
        bool get4Hbit(const Nui8 * data, NCount byteskip = 0);

        /** ��ȡ��λ�ĵ�1�ֽ�
        @remark ����߿�ʼ
        @param[in] data
        @param[in] byteskip
        */
        bool get1Lbit(const Nui8 * data, NCount byteskip = 0);

        /** ��ȡ��λ�ĵ�2�ֽ�
        @remark ����߿�ʼ
        @param[in] data
        @param[in] byteskip
        */
        bool get2Lbit(const Nui8 * data, NCount byteskip = 0);

        /** ��ȡ��λ�ĵ�3�ֽ�
        @remark ����߿�ʼ
        @param[in] data
        @param[in] byteskip
        */
        bool get3Lbit(const Nui8 * data, NCount byteskip = 0);

        /** ��ȡ��λ�ĵ�4�ֽ�
        @remark ����߿�ʼ
        @param[in] data
        @param[in] byteskip
        */
        bool get4Lbit(const Nui8 * data, NCount byteskip = 0);

        /** ������Ч������λ
        @version NIIEngine 3.0.0
        */
        NBitCount getWriteOffset();

        /** ��ǰ��ȡ������λ
        @version NIIEngine 3.0.0
        */
        NBitCount getReadOffset();

        /** ʵ��ռ�õ��ֽ���
        @version NIIEngine 3.0.0
        */
        NCount getSize() const;

        /**
        @version NIIEngine 3.0.0
        */
        NBitCount getRemainCount() const;

        /**
        @version NIIEngine 3.0.0
        */
        const Nui8 * getData() const;

        /** �Ƿ�ת����(���ݵ��ֽ���)
        @remark �����豸����һ������,������ͬ�������Ͽ��ܴ���ͬ���Ľ��
        @version NIIEngine 3.0.0
        */
        static bool isFlipData();

        /** �Ƿ��������ֽ���(���ݵ��ֽ���)
        @remark �����豸����һ������,������ͬ�������Ͽ��ܴ���ͬ���Ľ��
        @version NIIEngine 3.0.0
        */
        static bool isNetEndian();
    protected:
        ///
        static const Nui8 ZeroOMark[8];

        ///
        static const Nui8 ZeroIMark[8];

        ///
        static const Nui8 HMark[8];

        ///
        static const Nui8 LMark[8];
    private:
        NBitCount mBitWrite;
        NBitCount mBitAlloc;
        mutable NBitCount mBitRead;
        Nui8 * mData;
        bool mAutoData;
    };
}
#endif