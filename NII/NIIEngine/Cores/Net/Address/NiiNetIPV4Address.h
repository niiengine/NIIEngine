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