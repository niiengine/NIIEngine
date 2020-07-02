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

#ifndef _NII_NET_IPV6_ADDRESS_H_
#define _NII_NET_IPV6_ADDRESS_H_

#include "NiiNetPreInclude.h"
#include "NiiNetAddress.h"

namespace NII
{
namespace NII_NET
{
    /** 128λ�����ַ,8��16λ
    @note �˿ڱ����� [IPV6��ַ��ʽ]: �ָ�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IPV6Address : public Address
    {
    public:
        IPV6Address();
        IPV6Address(const IPV6Address & o);
        IPV6Address(const String & src);
        IPV6Address(const String & src, Nui16 port);

        /// ���ڲ�����
        IPV6Address & operator = (const IPV6Address & o);

        /// @copydetails Address::operator ==
        bool operator == (const IPV6Address & o);

        /// @copydetails Address::operator !=
        bool operator != (const IPV6Address & o);

        /// ���ڲ�����
        bool operator > (const IPV6Address & o);

        /// С�ڲ�����
        bool operator < (const IPV6Address & o);

		/** ���Ƶ�ַ�˿�
		@param[in] src ��һ����ַ
		@version NIIEngine 3.0.0
		*/
		void setPort(const IPV6Address & src);

		/** �Ƿ����,�����˿�
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
        @copydetails Address::read
        @note ��׼��ʽ [bbb1:aaad:eee3:ccc:3333:1111:aaaa:2222]:9394
        */
        void read(String & out, bool port = true) const;

        /**
        @copydetails Address::write
        @note ��׼��ʽ [ddd1:aaad:eee3:ccc:3333:1111:dddd:2222]:9394
        */
        bool write(const String & in);

        /**
        @copydetails Address::write
        @note ��׼��ʽ [ccc1:aaad:eee3:ccc:3333:1111:dddd:2222]:9394
        */
        bool write(const String & in, Nui16 port);

        /// @copydetails Address::read
        void read(NetSerializer * out) const;

        /// @copydetails Address::write
        void write(const NetSerializer * in);

		/// @copydetails Address::getInvalid
        const Address & getInvalid() const;
        
		/// @copydetails Address::getLocalHost
        const Address & getLocalHost() const;
        
        /// @copydetails Address::clone
        Address * clone() const;
        
        /** �Ƿ�����IPV6��ַ�ɷ�
        @version NIIEngine 4.0.0
        */
        static bool isAddress(const String & str);
	public:
        static const IPV6Address LOCALHOST;
        static const IPV6Address INVALID;

        struct sockaddr_in6 mSrc;   ///< Socketʵ��
    };
}
}
#endif