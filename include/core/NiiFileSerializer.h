/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-7-22

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

#ifndef _NII_FILE_SERIALIZER_H_
#define _NII_FILE_SERIALIZER_H_

#include "NiiPreInclude.h"
#include "NiiSerializer.h"

namespace NII
{
    // 12�ֽ�       + 2�ֽ�  + 4�ֽ�  + 4�ֽ�    + 2�ֽ�       + 4�ֽ�		    + 2�ֽ�    + 4�ֽ�
    // NIIEngine300 + �ֽ��� + UnitID + �����ID + ����ѹ���ɷ� + �ļ��еĳɷִ�С + ���а汾 + ��һ����Դ�Ŀ�ʼλ��(����)
    #define NII_SERIAL_FILE_COM_SIZE 31

    /** ��һ�򸴺��ļ��ɷ�������
    @remarks ����/�������ݶ������ļ�(.niiengine�ļ�ר��).
    @verion NIIEngine 3.0.0
    */
    class _EngineAPI FileSerializer : public Serializer
    {
        friend class FileSerializerManager;
    public:
        /** ������
        @version NIIEngine 3.0.0
        */
        class _EngineAPI Listener : public SerializerAlloc
        {
        public:
            Listener(){}
            virtual ~Listener(){}
        };

        /** ������ͨ�ø�ʽ(ǰ��λ�Ǹ�ʽ����,���λ��ѹ����ʽ��
        @version NIIEngine 3.0.0
        */
        enum Format
        {
            F_EC_None    = 0x0,         ///< ��ʹ�ø�ʽ�Ͱ�װ

            F_E_AES128   = 0x01,        ///< 128λ��AES(��ѹ����)
            F_E_AES256   = 0x02,        ///< 256λ��AES(7z��zip��)
            F_E_ZIP      = 0x03,        ///< zip��ʽ��װר��
            F_E_LAMZ     = 0x04,        ///< lamz����
            F_E_INNER    = 0xFF,        ///< �ڲ�����

            F_C_ZIP      = 0x100,       ///< zip��ʽ��װ
            F_C_7Z       = 0x200,       ///< 7z��ʽ��װ
            F_C_XZ       = 0x300,       ///< Xz��ʽ��װ
            F_C_GZIP     = 0x400,       ///< Gzip��ʽ��װ
            F_C_BZ2      = 0x500,       ///< Bz2��ʽ��װ
            F_C_INNER    = 0xFF00,      ///< �ڲ������ʽ��װ
        };
    public:
        FileSerializer(const DataStream * src, bool flip = false);
        FileSerializer(const FileSerializer & obj);
        virtual ~FileSerializer();

        using Serializer::read;
        using Serializer::write;

        /** ���ü�����
        @version NIIEngine 3.0.0
        */
        void setListener(Listener * obj);

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        Listener * getListener() const;

        /** д���ļ�ͷ��
        @remark .niiengine �ļ�ͷ
        @version NIIEngine 3.0.0
        */
        virtual void writeFileHeader();

        /** ��ȡ�ļ�ͷ��
        @remark .niiengine �ļ�ͷ
        @version NIIEngine 3.0.0
        */
		virtual void readFileHeader();

        /** д�뵥Ԫͷ
        @version NIIEngine 3.0.0 �߼�api
        */
        void writeHeader();

        /** ��ȡ��Ԫͷ
        @version NIIEngine 3.0.0 �߼�api
        */
        NCount readHeader();

        /** ɨ���¸���Ԫͷ
        @version NIIEngine 3.0.0
        */
        bool searchHeader();
        
        /** ��ת����һ����Ԫͷ
        @note ��ȡ��һ����Ԫͷ
        @version NIIEngine 3.0.0 �߼�api
        */
        bool skipHeader();
        
        /** ��ת���¸���Ԫͷ
        @version NIIEngine 3.0.0
        */
        bool skipToNextUnit();

        /** �����ļ�����
        @note Format ���,��������д��ʱʹ��
        @version NIIEngine 3.0.0
        */
        void setFormat(SerialFormat m);

        /** ��ȡ�ļ�����
        @note Format ���,�������ж�ȡʱʹ��
        @version NIIEngine 3.0.0
        */
        SerialFormat getFormat() const;

        /** ����Ƿ�֧�ֵ�ǰ�汾
        @version NIIEngine 3.0.0
        */
        virtual bool isVersion(SerialVer ver) const;

        /** ��ȡXml�ɷ�ID
        @note �������ʵ��
        @version NIIEngine 3.0.0
        */
        virtual SerializerID getUnit() const = 0;

        /** ��ȡȺ��ID
        @note �������ʵ��
        @version NIIEngine 3.0.0
        */
        virtual GroupID getGroup() const = 0;
    protected:
        FileSerializer();

        /** д�벿��
        @param[in] out �������ָ��
        @param[in] src �����
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void readUnit(void * out) = 0;

        /** ��ȡ����
        @param[in] in �������ָ��
        @param[in] dst �����
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void writeUnit(const void * in, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native) = 0;

        /** ��ɵ�Ԫд��ʱ����
        @note д���겿�������
        @version NIIEngine 3.0.0 �߼�api
        */
        void reviseWrite();
    protected:
        Nui32 mUnitID;          ///< ��Ӧ�������ĵ�Ԫ����(��д)
        Nui32 mObjectID;        ///< ��ǰ���������ID(��д)
        Nui32 mComSize;         ///< ��ǰ�������ܳ���(���������ļ��еĴ�С,������ѹ��δѹ������)(��д)
        Nui32 mFileSize;        ///< ��ǰ�������ܴ�С(��д)
        Nui32 mNextPos;
        Nui16 mVersion;         ///< ��ǰ�������еİ汾(��д)
        Nui16 mFormat;          ///< ��ǰ�����ļ���ѹ��(2���ɷ�)(��д)
		Endian mEndian;
        Nui32 mComHeaderPos;    ///< ��Ԫͷλ��
        Nui32 mChunkSize;       ///< ��ǰ�����ɷֵĳ���(��д)
        Listener * mListener;
    };
}
#endif