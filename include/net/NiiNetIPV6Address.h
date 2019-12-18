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

#ifndef _NII_NET_IPV6_ADDRESS_H_
#define _NII_NET_IPV6_ADDRESS_H_

#include "NiiNetPreInclude.h"
#include "NiiNetAddress.h"

namespace NII
{
namespace NII_NET
{
    /** 128位网络地址,8个16位
    @note 端口必须用 [IPV6地址格式]: 分割
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IPV6Address : public Address
    {
    public:
        IPV6Address();
        IPV6Address(const String & src);
        IPV6Address(const String & src, Nui16 port);

        /// 等于操作符
        IPV6Address & operator = (const IPV6Address & o);

        /// 相等操作符
        bool operator == (const IPV6Address & o);

        /// 不等操作符
        bool operator != (const IPV6Address & o);

        /// 大于操作符
        bool operator > (const IPV6Address & o);

        /// 小于操作符
        bool operator < (const IPV6Address & o);

		/** 复制地址端口
		@param[in] src 另一个地址
		@version NIIEngine 3.0.0
		*/
		void setPort(const IPV6Address & src);

		/** 是否相等,不含端口
		@version NIIEngine 3.0.0
		*/
		bool equal(const IPV6Address & o);

        /// @copydetails Address::setHSBPort
        void setHSBPort(Nui16 port);

        /// @copydetails Address::getHSBPort
        Nui16 getHSBPort() const;

        /// @copydetails Address::setNSBPort
        void setNSBPort(Nui16 port);

        /// @copydetails Address::getNSBPort
        Nui16 getNSBPort() const;

        /// @copydetails Address::getVersion
        Nui32 getVersion() const;

        /// @copydetails Address::getProto
        Nui32 getProto() const;

        /// @copydetails Address::getSize
        NCount getSize() const;

        /// @copydetails Address::setLocalHost
        void setLocalHost();

        /// @copydetails Address::setInvalid
        void setInvalid();

        /// @copydetails Address::isLocalHost
        bool isLocalHost() const;

        /// @copydetails Address::isLan
        bool isLan() const;

		/// @copydetails Address::isInvalid
		bool isInvalid() const;

        /// @copydetails Address::Nul
        operator Nul() const;

        /**
        @copydetails Address::write
        @note 标准格式 [bbb1:aaad:eee3:ccc:3333:1111:aaaa:2222]:9394
        */
        void write(String & out, bool port = true) const;

        /**
        @copydetails Address::read
        @note 标准格式 [ddd1:aaad:eee3:ccc:3333:1111:dddd:2222]:9394
        */
        bool read(const String & in);

        /**
        @copydetails Address::read
        @note 标准格式 [ccc1:aaad:eee3:ccc:3333:1111:dddd:2222]:9394
        */
        bool read(const String & in, Nui16 port);

        /// @copydetails Address::write
        void write(NetSerializer * out) const;

        /// @copydetails Address::read
        void read(const NetSerializer * in);

		/// @copydetails Address::getInvalid
        const Address & getInvalid() const;
        
		/// @copydetails Address::getLocalHost
        const Address & getLocalHost() const;
	public:
        static const IPV6Address LOCALHOST;
        static const IPV6Address INVALID;

        struct sockaddr_in6 mSrc;   ///< Socket实例
    };
}
}
#endif