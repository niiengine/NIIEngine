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

#ifndef _NII_ADDRESS_ID_H_
#define _NII_ADDRESS_ID_H_

#include "NiiNetPreInclude.h"

namespace NII
{
namespace NII_NET
{
    /** 游戏引擎内部的网络地址
    @remark 这个ID只有在成功连接游戏引擎后才会产生的,而且是唯一对象的标识
	@version NIIEngine 3.0.0
    */
    class _EngineAPI AddressID : public NetWorkAlloc
    {
    public:
        AddressID();
        AddressID(Nui64 id);

		/** 设置索引
		@version NIIEngine 3.0.0
		*/
		void setIndex(Nui16 idx);

		/** 获取索引
		@version NIIEngine 3.0.0
		*/
		Nui16 getIndex() const;

        /// 从指定字符串中转换出网络ID
        bool read(const String & in);

        /// 转换网络ID到一个指定字符串
        void write(String & out) const;

        /// 等于操作符
        AddressID & operator =(const AddressID & org);

        /// 相等操作符
        bool operator == (const AddressID & org);

        /// 不等操作符
        bool operator != (const AddressID & org);

        /// 大于操作符
        bool operator > (const AddressID & org);

        /// 小于操作符
        bool operator < (const AddressID & org);

        /// 缩位操作符
        operator Nui32() const;

        /// 网络ID结构大小
        static NCount getSize() const;

        /// 无效的网络ID
        static const AddressID INVALID;
    protected:
        Nui64 mID;                      ///< 应用体系中的唯一ID
        Nui16 mIdex;                    ///< 本地唯一ID
    };
}
}