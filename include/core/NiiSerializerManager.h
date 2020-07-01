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

#ifndef _NII_SERIALIZER_MANAGER_H_
#define _NII_SERIALIZER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSerializerFactoryObj.h"

namespace NII
{
    /** 管理序列解析器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SerializerManager : public Singleton<SerializerManager>,
        public SerializerAlloc
    {
    public:
        SerializerManager();
        ~SerializerManager();

        /** 添加文件序列解码器
        @remark 这个类释放的时候不会删除实例对象本身,仅仅是把删除记录
        @param[in] serial
        @version NIIEngine 3.0.0
        */
        void add(SerializerFactoryObj * serial);

        /** 移去文件序列解码器
        @remark 这个类释放的时候不会删除实例对象本身,仅仅是把删除记录
        @param[in] serial
        @version NIIEngine 3.0.0
        */
        void remove(SerializerFactoryObj * serial);

        /** 获取文件序列解析齐
        @param[in] id 内部类的标识ID,不是所有的类都有标识ID的
        @version NIIEngine 3.0.0
        */
        SerializerFactoryObj * getFactory(Nui32 id);

        /** 移去所有的文件序列解码器
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /// @copydetails Singleton::getOnly
        static SerializerManager & getOnly();
        
        /// @copydetails Singleton::getOnlyPtr
        static SerializerManager * getOnlyPtr();
    private:
        typedef map<FactoryID, SerializerFactoryObj *>::type SerializerList;
        SerializerList mSerializers;
    };
}

#endif