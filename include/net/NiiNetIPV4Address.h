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
#ifndef _NII_NET_IPV4_ADDRESS_H_
#define _NII_NET_IPV4_ADDRESS_H_

#include "NiiNetPreInclude.h"
#include "NiiNetAddress.h"

namespace NII
{
namespace NII_NET
{
    /** 32λ�����ַ,4��8λ
    @note �˿ڱ����� : �ָ�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IPV4Address : public Address
    {
    public:
        IPV4Address();
        IPV4Address(const IPV4Address & o);
        IPV4Address(const String & src);
        IPV4Address(const String & src, Nui16 port);

        /// ���ڲ�����
        IPV4Address & operator = (const IPV4Address & o);

        /// @copydetail Address::operator ==
        bool operator == (const Address & o);

        /// @copydetail Address::operator !=
        bool operator != (const Address & o);

        /// ���ڲ�����
        bool operator > (const IPV4Address & o);

        /// С�ڲ�����
        bool operator < (const IPV4Address & o);

		/** ���Ƶ�ַ�˿�
		@param[in] src ��һ����ַ
		@version NIIEngine 3.0.0
		*/
		void setPort(const IPV4Address & src);

		/** �Ƿ����,�����˿�
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
        @note ��׼��ʽ 192.168.0.1:9393
        */
        bool write(const String & in);

        /**
        @copydetail Address::write
        @note ��׼��ʽ 192.168.0.1:9393 or localhost:9393
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
        
        /** �Ƿ�����IPV4��ַ�ɷ�
        @version NIIEngine 4.0.0
        */
        static bool isAddress(const String & str);
	public:
        static const IPV4Address LOCALHOST;
        static const IPV4Address INVALID;

        struct sockaddr_in mSrc;    ///< Socketʵ��
    };
}
}
#endif