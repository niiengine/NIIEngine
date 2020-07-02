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
        bool write(const String & in);

        /// ת������ID��һ��ָ���ַ���
        void read(String & out) const;

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
    
    /** ����Ψһ��ʶ
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

		/** ��ȡ
        @version NIIEngine 3.0.0
		*/
        void read(String & out) const;

		/** д��
        @version NIIEngine 3.0.0
		*/
        bool write(const String & in);

		/** ��ȡ
        @version NIIEngine 3.0.0
		*/
        void read(NetSerializer * out) const;

		/** д��
        @version NIIEngine 3.0.0
		*/
        void write(const NetSerializer * in);

        /**
        @version NIIEngine 3.0.0
        */
        static Nui64 genLocalGuid();
        
		/** ��ȡ��С
        @version NIIEngine 3.0.0
		*/
        static NCount size();

		/** ���ֻ�
        @version NIIEngine 3.0.0
		*/
        static Nul toNum(const UniqueID & guid);

        static const UniqueID INVALID;
	public:
        Nindex mIndex;
        Nui64 mGuid;
    };
    typedef vector<UniqueID *>::type UniqueIDList;
    
    /** �����ַ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Address : public NetAlloc
    {
    public:
        virtual ~Address(){}

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline void setIndex(Nindex idx){ mIndex = idx; }
        
        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        inline Nindex getIndex() const { return mIndex; }
        
        /** ��Ȳ�����
        @version NIIEngine 3.0.0
        */
        virtual bool operator == (const Address & o) = 0;

        /** ���Ȳ�����
        @version NIIEngine 3.0.0
        */
        virtual bool operator != (const Address & o) = 0;
        
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
        virtual void read(String & out, bool port = true) const = 0;

        /** ���ַ����ж�ȡ����
        @param[in] ��ַ�ַ���
        */
        virtual bool write(const String & in) = 0;

		/** ���ַ����ж�ȡ����
		@param[in] in ��ַ�ַ���
		@param[in] port �˿ں�
		*/
		virtual bool write(const String & in, Nui16 port) = 0;

        /** �����ݶ�ȡ������������
        @version NIIEngine 3.0.0
        */
        virtual void read(NetSerializer * out) const = 0;

		/** ����������д�뵽������
		@version NIIEngine 3.0.0
		*/
		virtual void write(const NetSerializer * in) = 0;

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
        static bool isEqual(const String & o1, const String & o2);
    protected:
        Nindex mIndex;              ///< �ֲ������е�����
    };
    typedef vector<Address *>::type AddressList;
    
    /** ����Ψһ��ַ
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

		/** ��ȡ
        @version NIIEngine 3.0.0
		*/
		void read(String & out, bool port = true) const;

		/** д��
        @version NIIEngine 3.0.0
		*/
		void write(const String & in, bool port = true);

		/** �Ƿ���Ч
        @version NIIEngine 3.0.0
		*/
		bool isValid() const;

		/** ���ֻ�
        @version NIIEngine 3.0.0
		*/
        static Nul toNum(const UniqueAddress & obj);
	public:
        UniqueID mID;
        Address * mAddress;
    };
    typedef vector<UniqueAddress>::type UniqueAddressList;
    
    /** �����ַ
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