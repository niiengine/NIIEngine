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
    protected:
        typedef map<FactoryID, FileSerializerFactory *>::type FileSerializerFactoryList;
        FileSerializerFactoryList mFileSerializers;
    };
}

#endif