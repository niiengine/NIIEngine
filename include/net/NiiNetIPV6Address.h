/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
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
        IPV6Address(const String & src);
        IPV6Address(const String & src, Nui16 port);

        /// ���ڲ�����
        IPV6Address & operator = (const IPV6Address & o);

        /// ��Ȳ�����
        bool operator == (const IPV6Address & o);

        /// ���Ȳ�����
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
        @copydetails Address::write
        @note ��׼��ʽ [bbb1:aaad:eee3:ccc:3333:1111:aaaa:2222]:9394
        */
        void write(String & out, bool port = true) const;

        /**
        @copydetails Address::read
        @note ��׼��ʽ [ddd1:aaad:eee3:ccc:3333:1111:dddd:2222]:9394
        */
        bool read(const String & in);

        /**
        @copydetails Address::read
        @note ��׼��ʽ [ccc1:aaad:eee3:ccc:3333:1111:dddd:2222]:9394
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

        struct sockaddr_in6 mSrc;   ///< Socketʵ��
    };
}
}
#endif