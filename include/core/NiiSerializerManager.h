/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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