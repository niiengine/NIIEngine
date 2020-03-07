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
#ifndef _NII_NET_IPV4_ADDRESS_H_
#define _NII_NET_IPV4_ADDRESS_H_

#include "NiiNetPreInclude.h"
#include "NiiNetAddress.h"

namespace NII
{
namespace NII_NET
{
    /** 32位网络地址,4个8位
    @note 端口必须用 : 分割
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IPV4Address : public Address
    {
    public:
        IPV4Address();
        IPV4Address(const IPV4Address & o);
        IPV4Address(const String & src);
        IPV4Address(const String & src, Nui16 port);

        /// 等于操作符
        IPV4Address & operator = (const IPV4Address & o);

        /// @copydetail Address::operator ==
        bool operator == (const Address & o);

        /// @copydetail Address::operator !=
        bool operator != (const Address & o);

        /// 大于操作符
        bool operator > (const IPV4Address & o);

        /// 小于操作符
        bool operator < (const IPV4Address & o);

		/** 复制地址端口
		@param[in] src 另一个地址
		@version NIIEngine 3.0.0
		*/
		void setPort(const IPV4Address & src);

		/** 是否相等,不含端口
		@version NIIEngine 3.0.0
		*/
		bool equal(const IPV4Address & o);

        /// @copydetail Address::setHSBPort
        void setHSBPort(Nui16 port);

        /// @copydetail Address::getHSBPort
        Nui16 getHSBPort() const;

        /// @copydetail Address::setNSBPort
        void setNSBPort(Nui16 port);

        /// @copydetail Address::getNSBPort
        Nui16 getNSBPort() const;

        /// @copydetail Address::getVersion
        Nui32 getVersion() const;

        /// @copydetail Address::getProto
        Nui32 getProto() const;

        /// @copydetail Address::getSize
        NCount getSize() const;

        /// @copydetail Address::setLocalHost
        void setLocalHost();

        /// @copydetails Address::setInvalid
        void setInvalid();

        /// @copydetail Address::isLocalHost
        bool isLocalHost() const;

        /// @copydetail Address::isLan
        bool isLan() const;

		/// @copydetail Address::isInvalid
		bool isInvalid() const;

        /// @copydetail Address::Nul
        operator Nul() const;

        /// @copydetail Address::read
        void read(String & out, bool port = true) const;

        /**
        @copydetail Address::write
        @note 标准格式 192.168.0.1:9393
        */
        bool write(const String & in);

        /**
        @copydetail Address::write
        @note 标准格式 192.168.0.1:9393 or localhost:9393
        */
        bool write(const String & in, Nui16 port);

        /// @copydetail Address::read
        void read(NetSerializer * out) const;

        /// @copydetail Address::write
        void write(const NetSerializer * in);

        /// @copydetails Address::getInvalid
        const Address & getInvalid() const;

        /// @copydetails Address::getLocalHost
        const Address & getLocalHost() const;

        /// @copydetails Address::clone
        Address * clone() const;
        
        /** 是否属于IPV4地址成分
        @version NIIEngine 4.0.0
        */
        static bool isAddress(const String & str);
	public:
        static const IPV4Address LOCALHOST;
        static const IPV4Address INVALID;

        struct sockaddr_in mSrc;    ///< Socket实例
    };
}
}
#endif