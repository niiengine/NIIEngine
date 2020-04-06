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

#ifndef _NII_NET_ADDRESS_H_
#define _NII_NET_ADDRESS_H_

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
        bool write(const String & in);

        /// 转换网络ID到一个指定字符串
        void read(String & out) const;

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
    
    /** 网络唯一标识
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UniqueID : public NetAlloc
    {
	public:
        UniqueID();
        UniqueID(Nui64 g);

        UniqueID & operator = (const UniqueID & o);

        bool operator == (const UniqueID & o) const;
        bool operator != (const UniqueID & o) const;
        bool operator > (const UniqueID & o) const;
        bool operator < (const UniqueID & o) const;

		/** 读取
        @version NIIEngine 3.0.0
		*/
        void read(String & out) const;

		/** 写入
        @version NIIEngine 3.0.0
		*/
        bool write(const String & in);

		/** 读取
        @version NIIEngine 3.0.0
		*/
        void read(NetSerializer * out) const;

		/** 写入
        @version NIIEngine 3.0.0
		*/
        void write(const NetSerializer * in);

		/** 获取大小
        @version NIIEngine 3.0.0
		*/
        static NCount size();

		/** 数字化
        @version NIIEngine 3.0.0
		*/
        static Nul toNum(const UniqueID & guid);

        static const UniqueID INVALID;
	public:
        Nindex mIndex;
        Nui64 mGuid;
    };
    typedef vector<UniqueID *>::type UniqueIDList;
    
    /** 网络地址
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Address : public NetAlloc
    {
    public:
        virtual ~Address(){}

        /** 设置索引
        @version NIIEngine 3.0.0
        */
        inline void setIndex(Nindex idx){ mIndex = idx; }
        
        /** 获取索引
        @version NIIEngine 3.0.0
        */
        inline Nindex getIndex() const { return mIndex; }
        
        /** 相等操作符
        @version NIIEngine 3.0.0
        */
        virtual bool operator == (const Address & o) = 0;

        /** 不等操作符
        @version NIIEngine 3.0.0
        */
        virtual bool operator != (const Address & o) = 0;
        
        /** 设置本地字节序端口
        @remark 字节序无别
        @param[in] port 端口号
        */
        virtual void setHSBPort(Nui16 port) = 0;

        /** 获取本地字节序端口
        @remark 字节序无别
        @version NIIEngine 3.0.0
        */
        virtual Nui16 getHSBPort() const = 0;

        /** 设置网络字节序端口
        @remark 字节序有别
        @param[in] port 端口好
        */
        virtual void setNSBPort(Nui16 port) = 0;

        /** 获取网络字节序端口
        @remark 字节序有别
        @version NIIEngine 3.0.0
        */
        virtual Nui16 getNSBPort() const = 0;

        /** 获取IP版本
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getVersion() const = 0;

        /** 获取协议包类型
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getProto() const = 0;

        /** 获取结构大小
        @version NIIEngine 3.0.0
        */
        virtual NCount getSize() const = 0;

        /** 设置成本地
        @version NIIEngine 3.0.0
        */
        virtual void setLocalHost() = 0;

        /** 设置为无效地址
        @version NIIEngine 3.0.0
        */
        virtual void setInvalid() = 0;

        /** 本地
        @version NIIEngine 3.0.0
        */
        virtual bool isLocalHost() const = 0;

        /** 局域网
        @version NIIEngine 3.0.0
        */
        virtual bool isLan() const = 0;

		/** 是否无效地址
		@version NIIEngine 3.0.0
		*/
		bool isInvalid() const = 0;

        /** 转换位数值
        @version NIIEngine 3.0.0
        */
        virtual operator Nul() const = 0;

        /** 把数据写入到字符串中
        @param[in] 输出字符串
        @param[in] 是否包含端口端口
        @version NIIEngine 3.0.0
        */
        virtual void read(String & out, bool port = true) const = 0;

        /** 从字符串中读取数据
        @param[in] 地址字符串
        */
        virtual bool write(const String & in) = 0;

		/** 从字符串中读取数据
		@param[in] in 地址字符串
		@param[in] port 端口号
		*/
		virtual bool write(const String & in, Nui16 port) = 0;

        /** 把数据读取到网络序列中
        @version NIIEngine 3.0.0
        */
        virtual void read(NetSerializer * out) const = 0;

		/** 把网络序列写入到数据中
		@version NIIEngine 3.0.0
		*/
		virtual void write(const NetSerializer * in) = 0;

        /** 副本对象
        @note 使用完必须用 N_delete 释放,否则内存泄露
        @version NIIEngine 3.0.0
        */
        Address * clone() const = 0;
        
        /** 获取一个无效地址
        @version NIIEngine 3.0.0
        */
        const Address & getInvalid() const = 0;

        /** 获取本地地址
        @version NIIEngine 3.0.0
        */
        const Address & getLocalHost() const = 0;

		/** 是否数字型地址
		@version NIIEngine 3.0.0
		*/
		static bool isNum(const String & src);
        
        /** 是否相同
        @version NIIEngine 3.0.0
        */
        static bool isEqual(const String & o1, const String & o2);
    protected:
        Nindex mIndex;              ///< 局部区域中的索引
    };
    typedef vector<Address *>::type AddressList;
    
    /** 网络唯一地址
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UniqueAddress : public NetAlloc
    {
    public:
        UniqueAddress();

        ~UniqueAddress();

        UniqueAddress(SocketMessage * msg);

        UniqueAddress(const UniqueID & o);

        UniqueAddress(const Address * o);
        
        UniqueAddress(const Address * addr, const UniqueID & id);

        UniqueAddress(const UniqueAddress & o);
        
        UniqueAddress & operator = (const UniqueID & o);

        UniqueAddress & operator = (const Address & o);

        UniqueAddress & operator = (const UniqueAddress & o);

        bool operator == (const UniqueAddress & o) const;

		/** 读取
        @version NIIEngine 3.0.0
		*/
		void read(String & out, bool port = true) const;

		/** 写入
        @version NIIEngine 3.0.0
		*/
		void write(const String & in, bool port = true);

		/** 是否有效
        @version NIIEngine 3.0.0
		*/
		bool isValid() const;

		/** 数字化
        @version NIIEngine 3.0.0
		*/
        static Nul toNum(const UniqueAddress & obj);
	public:
        UniqueID mID;
        Address * mAddress;
    };
    typedef vector<UniqueAddress>::type UniqueAddressList;
    
    /** 网络地址
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AutoAddress : public Address
    {
    public:
        Address(Address * data);
        virtual ~AutoAddress();

        /// @copydetails Address::operator ==
        virtual bool operator == (const Address & o) = 0;

        /// @copydetails Address::operator !=
        virtual bool operator != (const Address & o) = 0;
        
        /// @copydetails Address::setHSBPort
        virtual void setHSBPort(Nui16 port);

        /// @copydetails Address::getHSBPort
        virtual Nui16 getHSBPort() const;

        /// @copydetails Address::setNSBPort
        virtual void setNSBPort(Nui16 port);

        /// @copydetails Address::getNSBPort
        virtual Nui16 getNSBPort() const;

        /// @copydetails Address::getVersion
        virtual Nui32 getVersion() const;

        /// @copydetails Address::getProto
        virtual Nui32 getProto() const;

        /// @copydetails Address::getSize
        virtual NCount getSize() const;

        /// @copydetails Address::setLocalHost
        virtual void setLocalHost();

        /// @copydetails Address::setInvalid
        virtual void setInvalid();

        /// @copydetails Address::isLocalHost
        virtual bool isLocalHost() const;

        /// @copydetails Address::isLan
        virtual bool isLan() const;

		/// @copydetails Address::isInvalid
		bool isInvalid() const;

        /// @copydetails Address::Nul
        virtual operator Nul() const;

        /// @copydetails Address::read
        virtual void read(String & out, bool port = true) const;

        /// @copydetails Address::write
        virtual bool write(const String & in);

		/// @copydetails Address::write
		virtual bool write(const String & in, Nui16 port);

        /// @copydetails Address::read
        virtual void read(NetSerializer * out) const;

		/// @copydetails Address::write
		virtual void write(const NetSerializer * in);

        /// @copydetails Address::clone
        Address * clone() const;
        
        /// @copydetails Address::getInvalid
        const Address & getInvalid() const;

        /// @copydetails Address::getLocalHost
        const Address & getLocalHost() const;
    protected:
        Address * mData;
    };
}
}
#endif