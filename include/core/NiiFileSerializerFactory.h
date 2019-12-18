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

#ifndef _NII_FileSerializerFactory_H_
#define _NII_FileSerializerFactory_H_

#include "NiiPreInclude.h"
#include "NiiFileSerializer.h"

#define N_FileSerializerFactory(c, lid) PreDefineFileSerializerFactory<c>(lid)

namespace NII
{
    /** xml����������
    @versioin NIIEngine 3.0.0
    */
    class _EngineAPI FileSerializerFactory : public SerializerAlloc
    {
    public:
        FileSerializerFactory(){}
        virtual ~FileSerializerFactory(){}

        /** ��ȡ����ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** ��������
        @version NIIEngine 3.0.0
        */
        virtual FileSerializer * create(const DataStream * src) = 0;

        /** ��������
        @version NIIEngine 3.0.0
        */
        virtual FileSerializer * create(const FileSerializer & src) = 0;

        /** ɾ������
        @version NIIEngine 3.0.0
        */
        virtual void destroy(FileSerializer * obj) = 0;
    };

    /** Ԥ����UI��Ԫ������
    @version NIIEngine 3.0.0
    */
    template <typename T> class PreDefineFileSerializerFactory: public FileSerializerFactory
    {
    public:
        PreDefineFileSerializerFactory(FactoryID fid);

        /// @copydetails FileSerializerFactory::getID
        FactoryID getID() const;

        /// @copydetails FileSerializerFactory::create
        FileSerializer * create(const DataStream * src);

        /// @copydetails FileSerializerFactory::create
        FileSerializer * create(const FileSerializer & src);

        /// @copydetails FileSerializerFactory::destroy
        void destroy(FileSerializer * obj);

        /** ע�뵽ϵͳ��
        @version NIIEngine 3.0.0
        */
        static void plugin();

        /** ��ϵͳ���Ƴ�
        @version NIIEngine 3.0.0
        */
        static void unplugin();
    protected:
        FactoryID mID;
    };
    //------------------------------------------------------------------------
    template <typename T> PreDefineFileSerializerFactory<T>::PreDefineFileSerializerFactory(FactoryID fid) :
        mID(fid)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> FactoryID PreDefineFileSerializerFactory<T>::getID() const
    {
        return mID;
    }
    //------------------------------------------------------------------------
    template <typename T> FileSerializer * PreDefineFileSerializerFactory<T>::create(const DataStream * src)
    {
        return N_new T(src);
    }
    //------------------------------------------------------------------------
    template <typename T> FileSerializer * PreDefineFileSerializerFactory<T>::create(const FileSerializer & src)
    {
        return N_new T(src);
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineFileSerializerFactory<T>::destroy(FileSerializer * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineFileSerializerFactory<T>::plugin()
    {
        if(N_OnlyPtr(FileSerializer))
        {
            FileSerializerFactory * factory = N_new PreDefineFileSerializerFactory<T>(mID);
            if(N_Only(FileSerializer).addFactory(factory) != true)
                N_delete factory;
        }
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineFileSerializerFactory<T>::unplugin()
    {
        if(N_OnlyPtr(FileSerializer))
        {
            FileSerializerFactory * factory = N_Only(FileSerializer).getFactory(mID);
            if(factory != 0)
            {
                N_Only(FileSerializer).removeFactory(mID);
                N_delete factory;
            }
        }
    }
    //------------------------------------------------------------------------
}
#endif