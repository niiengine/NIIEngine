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
		地理信息系统软件(GIS)
		电影背景立体重构软件
		立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/
#ifndef _PIXEL_BUFFER_MANAGER_H_
#define _PIXEL_BUFFER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiPixelBuffer.h"
#include "NiiException.h"

namespace NII
{
    /** 像素缓存管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBufferManager : public Singleton<PixelBufferManager>, public BufferAlloc
    {
    public:
        PixelBufferManager();
        ~PixelBufferManager();

        /** 加载创建像素缓存
        @param[in] file 脚本文件名字
        @version NIIEngine 3.0.0
        */
        void load(const String & file, GroupID gid = GroupUnknow);

        /** 加载创建像素缓存
        @param[in] id 像素标识
        @param[in] file 文件名字
        @param[in] rgid 资源组标识
        @version NIIEngine 3.0.0
        */
        void load(PixelBufferID id, const String & file, GroupID rgid = GroupUnknow);

        /** 创建像素缓存
        @param[in] id 像素标识
        @param[in] rid 像素纹理源
        @version NIIEngine 3.0.0
        */
        PixelBuffer * create(ResourceID rid, PixelBufferID id);

		/** 获取指定的像素缓存
		@param[in] pid 像素标识
		@version NIIEngine 3.0.0
		*/
		PixelBuffer * get(PixelBufferID pid) const;

        /** 获取指定的像素缓存
        @param[in] rid 像素纹理源
        @param[in] id 像素标识
        @version NIIEngine 3.0.0
        */
        PixelBuffer * get(ResourceID rid, PixelBufferID id) const;

		/** 是否存在定义的像素缓存
		@param[in] pid 像素标识
		@version NIIEngine 3.0.0
		*/
		bool isExist(PixelBufferID id) const;

        /** 是否存在定义的像素缓存
        @param[in] id 像素标识
        @param[in] rid 像素纹理源
        @version NIIEngine 3.0.0
        */
        bool isExist(ResourceID rid, PixelBufferID id) const;

		/** 删除像素组
		@param[in] rid 像素纹理源
		@param[in] deltex 删除纹理
		*/
		void destroy(ResourceID rid, PixelBufferID id, bool deltex);

        /** 删除像素组
        @param[in] rid 像素纹理源
        @param[in] deltex 删除纹理
        */
        void destroy(ResourceID rid, bool deltex);

        /** 删除像素
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 获取像素组中的像素数量
        @param[in] rid 像素纹理源
        @version NIIEngine 3.0.0
        */
        NCount getCount(ResourceID rid) const;

        /** 视图大小
        @param size 尺寸大小
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizef & size);

        /** 设置资源组
        @version NIIEngine 3.0.0
        */
        static void setGroup(GroupID rgid);

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
		static GroupID getGroup();

        /// @copydetails Singleton::getOnly
        static PixelBufferManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static PixelBufferManager * getOnlyPtr();
    protected:
        /** 添加像素
        @version NIIEngine 3.0.0
        */
        void addImpl(PixelBuffer * pb);
    private:
        typedef map<PixelBufferID, PixelBuffer *>::type PixelBuffers;
        typedef map<ResourceID, PixelBuffers>::type PixelList;
        PixelList mPixelBuffers;
		static GroupID ResourceGroup;
    };
}
#endif