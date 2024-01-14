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

#ifndef _NII_FileSerializerFactory_H_
#define _NII_FileSerializerFactory_H_

#include "NiiPreInclude.h"
#include "NiiFileSerializer.h"

#define N_FileSerializerFactory(c, lid) PreDefineFileSerializerFactory<c>(lid)

namespace NII
{
    /** xml分析工厂类
    @versioin NIIEngine 3.0.0
    */
    class _EngineAPI FileSerializerFactory : public SerializerAlloc
    {
    public:
        FileSerializerFactory(){}
        virtual ~FileSerializerFactory(){}

        /** 获取工厂ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** 创建对象
        @version NIIEngine 3.0.0
        */
        virtual FileSerializer * create(const DataStream * src) = 0;

        /** 创建对象
        @version NIIEngine 3.0.0
        */
        virtual FileSerializer * create(const FileSerializer & src) = 0;

        /** 删除对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(FileSerializer * obj) = 0;
    };

    /** 预定制UI单元工厂类
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

        /** 注入到系统中
        @version NIIEngine 3.0.0
        */
        static void plugin();

        /** 从系统中移出
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