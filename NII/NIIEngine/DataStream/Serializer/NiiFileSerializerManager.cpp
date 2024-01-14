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

#include "NiiPreProcess.h"
#include "NiiFileSerializerManager.h"
#include "NiiFileSerializerList.h"
#include "NiiFileSerializerFactory.h"
#include "NiiMeshSerializer.h"
#include "NiiSkeletonSerializer.h"

namespace NII
{
    class _EngineInner InnerFileSerializer : public FileSerializer
    {
    public:
        InnerFileSerializer(const DataStream * src, bool flip = false) :
            FileSerializer(src, flip)
        {
        }
        InnerFileSerializer(const FileSerializer & obj) :
            FileSerializer(obj)
        {

        }
    protected:
        /// @copydetails FileSerializer::getUnit
        SerializerID getUnit() const
        {
            return N_Serializer_File;
        }

        /// @copydetails FileSerializer::getGroup
        GroupID getGroup() const
        {
            return GroupUnknow;
        }

        /// @copydetails FileSerializer::read
        void readUnit(void * out)
        {
            (void)out;
        }

        /// @copydetails FileSerializer::write
        void writeUnit(const void * in, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native)
        {
            (void)in;
            (void)ver;
            (void)sf;
            (void)emode;
        }
    };
    //------------------------------------------------------------------------
    template<> FileSerializerManager * Singleton<FileSerializerManager>::mOnly = 0;
    //------------------------------------------------------------------------
    FileSerializerManager::FileSerializerManager()
    {
        addFactory(N_new PreDefineFileSerializerFactory<InnerFileSerializer>(N_Serializer_File));
        addFactory(N_new PreDefineFileSerializerFactory<MeshSerializer>(N_Serializer_Mesh_30000));
        addFactory(N_new PreDefineFileSerializerFactory<SkeletonSerializer>(N_Serializer_Skeleton_30000));
    }
    //------------------------------------------------------------------------
    FileSerializerManager::~FileSerializerManager()
    {
        FileSerializerFactoryList::iterator i, iend = mFileSerializers.end();
        for(i = mFileSerializers.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mFileSerializers.clear();
    }
    //------------------------------------------------------------------------
    void FileSerializerManager::read(DataStream * src, FactoryID fid, Nui32 objid, void * out)
    {
        FileSerializerFactoryList::iterator i = mFileSerializers.find(fid);
        if(i != mFileSerializers.end())
        {
            FileSerializer * serl = i->second->create(src);
            while(serl->skipHeader())
            {
                if(serl->mObjectID == objid && serl->isVersion(serl->mVersion))
                {
                    serl->readUnit(out);
                }
            }
            i->second->destroy(serl);
        }
    }
    //------------------------------------------------------------------------
    void FileSerializerManager::readUnit(DataStream * src, FactoryID & uid, Nui32 & objid,
        void * out)
    {
        FileSerializerFactoryList::iterator s, i = mFileSerializers.find(N_Serializer_File);
        if(i != mFileSerializers.end())
        {
            FileSerializer * serl = i->second->create(src);
            if(serl->skipHeader())
            {
                uid = serl->mUnitID;
                objid = serl->mObjectID;

                s = mFileSerializers.find(uid);
                if(s != mFileSerializers.end())
                {
                    FileSerializer * serl2 = s->second->create(*serl);
                    if(serl2->isVersion(serl2->mVersion))
                    {
                        serl2->readUnit(out);
                    }
                    s->second->destroy(serl2);
                }
            }
            i->second->destroy(serl);
        }
    }
    //------------------------------------------------------------------------
    void FileSerializerManager::write(DataStream * dst, FactoryID fid, Nui32 uid,
        const void * in, SerialVer ver, SerialFormat sf, Endian emode)
    {
        FileSerializerFactoryList::iterator i = mFileSerializers.find(fid);
        if(i != mFileSerializers.end())
        {
            FileSerializer * serl = i->second->create(dst);
            if(serl->isVersion(ver))
            {
                serl->mUnitID = fid;
                serl->mObjectID = uid;
                serl->writeUnit(in, ver, sf, emode);
            }
            i->second->destroy(serl);
        }
    }
    //------------------------------------------------------------------------
    bool FileSerializerManager::addFactory(FileSerializerFactory * f)
    {
        FileSerializerFactoryList::iterator i = mFileSerializers.find(f->getID());
        if(i == mFileSerializers.end())
        {
            mFileSerializers.insert(Npair(f->getID(), f));
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void FileSerializerManager::removeFactory(FactoryID fid)
    {
        FileSerializerFactoryList::iterator i = mFileSerializers.find(fid);
        if(i != mFileSerializers.end())
        {
            mFileSerializers.erase(i);
        }
    }
    //------------------------------------------------------------------------
    FileSerializerFactory * FileSerializerManager::getFactory(FactoryID fid) const
    {
        FileSerializerFactoryList::const_iterator i = mFileSerializers.find(fid);
        if(i != mFileSerializers.end())
        {
            return i->second;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    FileSerializer * FileSerializerManager::create(FactoryID fid, DataStream * src)
    {
        FileSerializerFactoryList::iterator i = mFileSerializers.find(fid);
        if(i != mFileSerializers.end())
        {
            return i->second->create(src);
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void FileSerializerManager::destroy(FileSerializer * obj)
    {
        FileSerializerFactoryList::iterator i = mFileSerializers.find(obj->getUnit());
        if(i != mFileSerializers.end())
        {
            i->second->destroy(obj);
        }
        N_assert(0, "file serializer no find!");
    }
    //------------------------------------------------------------------------
    /*FileSerializerManager * FileSerializerManager::getOnlyPtr()
    {
        return mOnly;
    }
    //------------------------------------------------------------------------
    FileSerializerManager & FileSerializerManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //------------------------------------------------------------------------
}