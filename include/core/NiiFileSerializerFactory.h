/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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