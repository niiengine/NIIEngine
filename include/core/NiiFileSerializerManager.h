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

#ifndef _NII_FILE_SERIALIZER_MANAGER_H_
#define _NII_FILE_SERIALIZER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiFileSerializer.h"

namespace NII
{
    /** �ļ����й�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FileSerializerManager : public Singleton<FileSerializerManager>, public ManagerAlloc
    {
    public:
        FileSerializerManager();
        ~FileSerializerManager();

        /** ��ȡָ����Ԫ
        @version NIIEngine 3.0.0
        */
        void read(DataStream * src, FactoryID fid, Nui32 objid, void * out);

        /** ��ȡ��һ����Ԫ
        @version NIIEngine 3.0.0
        */
        void readUnit(DataStream * src, FactoryID & fid, Nui32 & objid, void * out);

        /** д����һ����Ԫ
        @version NIIEngine 3.0.0
        */
        void write(DataStream * dst, FactoryID fid, Nui32 objid, const void * in,
            SerialVer ver = 30000, SerialFormat sf = FileSerializer::F_EC_None, 
                Endian emode = E_Native);

        /** ��ӹ�����
        @param[in] fat
        @version NIIEngine 3.0.0
        */
        bool addFactory(FileSerializerFactory * fat);

        /** ��ȥ������
        @param[in] fid
        @version NIIEngine 3.0.0
        */
        void removeFactory(FactoryID fid);

        /** ��ȡ������
        @param[in] fid
        @version NIIEngine 3.0.0
        */
        FileSerializerFactory * getFactory(FactoryID fid) const;

        /** �����ļ�������
        @version NIIEngine 3.0.0
        */
        FileSerializer * create(FactoryID fid, DataStream * src);

        /** ɾ���ļ�������
        @version NIIEngine 3.0.0
        */
        void destroy(FileSerializer * obj);

        /// @copydetails Singleton::getOnly
        static FileSerializerManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static FileSerializerManager * getOnlyPtr();
    protected:
        typedef map<FactoryID, FileSerializerFactory *>::type FileSerializerFactoryList;
        FileSerializerFactoryList mFileSerializers;
    };
}

#endif