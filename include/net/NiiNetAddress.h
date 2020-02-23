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

#ifndef _NII_NET_ADDRESS_H_
#define _NII_NET_ADDRESS_H_

#include "NiiNetPreInclude.h"

namespace NII
{
namespace NII_NET
{
    /** ��Ϸ�����ڲ��������ַ
    @remark ���IDֻ���ڳɹ�������Ϸ�����Ż������,������Ψһ����ı�ʶ
	@version NIIEngine 3.0.0
    */
    class _EngineAPI AddressID : public NetWorkAlloc
    {
    public:
        AddressID();
        AddressID(Nui64 id);

		/** ��������
		@version NIIEngine 3.0.0
		*/
		void setIndex(Nui16 idx);

		/** ��ȡ����
		@version NIIEngine 3.0.0
		*/
		Nui16 getIndex() const;

        /// ��ָ���ַ�����ת��������ID
        bool read(const String & in);

        /// ת������ID��һ��ָ���ַ���
        void write(String & out) const;

        /// ���ڲ�����
        AddressID & operator =(const AddressID & org);

        /// ��Ȳ�����
        bool operator == (const AddressID & org);

        /// ���Ȳ�����
        bool operator != (const AddressID & org);

        /// ���ڲ�����
        bool operator > (const AddressID & org);

        /// С�ڲ�����
        bool operator < (const AddressID & org);

        /// ��λ������
        operator Nui32() const;

        /// ����ID�ṹ��С
        static NCount getSize() const;

        /// ��Ч������ID
        static const AddressID INVALID;
    protected:
        Nui64 mID;                      ///< Ӧ����ϵ�е�ΨһID
        Nui16 mIdex;                    ///< ����ΨһID
    };
    
    /** �����ַ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Address : public NetAlloc
    {
    public:
        virtual ~Address(){}

        /** ���ñ����ֽ���˿�
        @remark �ֽ����ޱ�
        @param[in] port �˿ں�
        */
        virtual void setHSBPort(Nui16 port) = 0;

        /** ��ȡ�����ֽ���˿�
        @remark �ֽ����ޱ�
        @version NIIEngine 3.0.0
        */
        virtual Nui16 getHSBPort() const = 0;

        /** ���������ֽ���˿�
        @remark �ֽ����б�
        @param[in] port �˿ں�
        */
        virtual void setNSBPort(Nui16 port) = 0;

        /** ��ȡ�����ֽ���˿�
        @remark �ֽ����б�
        @version NIIEngine 3.0.0
        */
        virtual Nui16 getNSBPort() const = 0;

        /** ��ȡIP�汾
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getVersion() const = 0;

        /** ��ȡЭ�������
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getProto() const = 0;

        /** ��ȡ�ṹ��С
        @version NIIEngine 3.0.0
        */
        virtual NCount getSize() const = 0;

        /** ���óɱ���
        @version NIIEngine 3.0.0
        */
        virtual void setLocalHost() = 0;

        /** ����Ϊ��Ч��ַ
        @version NIIEngine 3.0.0
        */
        virtual void setInvalid() = 0;

        /** ����
        @version NIIEngine 3.0.0
        */
        virtual bool isLocalHost() const = 0;

        /** ������
        @version NIIEngine 3.0.0
        */
        virtual bool isLan() const = 0;

		/** �Ƿ���Ч��ַ
		@version NIIEngine 3.0.0
		*/
		bool isInvalid() const = 0;

        /** ת��λ��ֵ
        @version NIIEngine 3.0.0
        */
        virtual operator Nul() const = 0;

        /** ������д�뵽�ַ�����
        @param[in] ����ַ���
        @param[in] �Ƿ�����˿ڶ˿�
        @version NIIEngine 3.0.0
        */
        virtual void write(String & out, bool port = true) const = 0;

        /** ���ַ����ж�ȡ����
        @param[in] ��ַ�ַ���
        */
        virtual bool read(const String & in) = 0;

		/** ���ַ����ж�ȡ����
		@param[in] in ��ַ�ַ���
		@param[in] port �˿ں�
		*/
		virtual bool read(const String & in, Nui16 port) = 0;

        /** �����ݶ�ȡ������������
        @version NIIEngine 3.0.0
        */
        virtual void write(NetSerializer * out) const = 0;

		/** ����������д�뵽������
		@version NIIEngine 3.0.0
		*/
		virtual void read(const NetSerializer * in) = 0;

        /** ��������
        @note ʹ��������� N_delete �ͷ�,�����ڴ�й¶
        @version NIIEngine 3.0.0
        */
        Address * clone() const = 0;
        
        /** ��ȡһ����Ч��ַ
        @version NIIEngine 3.0.0
        */
        const Address & getInvalid() const = 0;

        /** ��ȡ���ص�ַ
        @version NIIEngine 3.0.0
        */
        const Address & getLocalHost() const = 0;

		/** �Ƿ������͵�ַ
		@version NIIEngine 3.0.0
		*/
		static bool isNum(const String & src);
        
        /** �Ƿ���ͬ
        @version NIIEngine 3.0.0
        */
        static bool isEqual(const String o1, const String & o2);
    protected:
        Nindex mIndex;              ///< �ֲ������е�����
    };
    typedef vector<Address *>::type AddressList;
}
}
#endif