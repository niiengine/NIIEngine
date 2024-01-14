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

#include "NiiPreProcess.h"
#include "NiiNetAddress.h"
#include "NiiNetIPV4Address.h"
#include "NiiNetIPV6Address.h"
#include <stdio.h>
#include <stdlib.h>

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AddressID
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    const AddressID AddressID::INVALID;
    //------------------------------------------------------------------------
    AddressID::AddressID():
        mIdex(0)
    {
        *this = INVALID;
    }
    //------------------------------------------------------------------------
    AddressID::AddressID(Nui64 id) :
        mID(id),
        mIdex(0)
    {
    }
	//------------------------------------------------------------------------
	void AddressID::setIndex(Nui16 idx)
	{
		mIdex = idx;
	}
	//------------------------------------------------------------------------
	Nui16 AddressID::getIndex() const
	{
		return mIdex;
	}
    //------------------------------------------------------------------------
    void AddressID::read(String & dest) const
    {
        static NIIb buffer[128] = "";

        if(*this == INVALID)
            dest = "INVALID";
        else
        {
            NIIi n = sprintf(buffer, "%" N_64BIT_MOD "u", (Nui64)g);
            dest = String(buffer, n);
        }
    }
    //------------------------------------------------------------------------
    bool AddressID::write(const String & src)
    {
        if(src == N_BLACK)
            return false;

#if N_PLAT == N_PLAT_WIN32
        mID = _strtoui64(src.c_str(), NULL, 10);
#elif (N_PLAT == N_PLAT_LINUX) || (N_PLAT == N_PLAT_LINUX_X)
        mID = strtoull(src.c_str(),0,10);
#else
        mID = strtoull(src.c_str(), (char**)NULL, 10);
#endif
        return true;
    }
    //------------------------------------------------------------------------
    AddressID & AddressID::operator = (const AddressID & org)
    {
        mID = org.mID;
        mIdex = org.mIdex;
        return *this;
    }
    //------------------------------------------------------------------------
    bool AddressID::operator == (const AddressID & org)
    {
        return mID == org.mID;
    }
    //------------------------------------------------------------------------
    bool AddressID::operator != (const AddressID & org)
    {
        return mID != org.mID;
    }
    //------------------------------------------------------------------------
    bool AddressID::operator > (const AddressID & org)
    {
        return mID > org.mID;
    }
    //------------------------------------------------------------------------
    bool AddressID::operator < (const AddressID & org)
    {
        return mID < org.mID;
    }
    //------------------------------------------------------------------------
    operator AddressID::Nul() const
    {
        return ((Nul)(mID >> 32)) ^ ((Nul)(mID & 0xFFFFFFFF));
    ]
    //------------------------------------------------------------------------
    NCount AddressID::getSize() const
    {
        return sizeof(Nui64);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Address
    //------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool Address::isNum(const String & src)
	{
		Nui32 i = 0;
		const char * in = src.c_str();
		while (in[i])
		{
			// IPV4: natpunch.jenkinssoftware.com
			// IPV6: fe80::7c:31f7:fec4:27de%14
			if ((in[i] >= 'g' && in[i] <= 'z') ||
				(in[i] >= 'G' && in[i] <= 'Z'))
				return false;
			++i;
		}
		return true;
	}
    //------------------------------------------------------------------------
    bool Address::isEqual(const String & o1, const String & o2)
    {
        if (o2.empty() || o2.size() > 15)
            return false;

        Nui32 indx = 0;
        while(1)
        {
            if(o1[indx] == o2[indx])
            {
                // Equal characters
                if(indx >= o2.size())
                {
                    return true;
                }
                indx++;
            }
            else
            {
                if(indx >= o1.size() || indx >= o2.size())
                {
                    // End of one of the strings
                    break;
                }
                // Characters do not match
                if(o1[indx] == '*')
                {
                    // Domain is banned.
                    return true;
                }
                // Characters do not match and it is not a *
                break;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AutoAddress
    //------------------------------------------------------------------------
	//------------------------------------------------------------------------
    AutoAddress::AutoAddress(Address * addr) :
        mData(addr)
    {
    }
    //------------------------------------------------------------------------
    AutoAddress::AutoAddress() :
        mData(0)
    {
    }
    //------------------------------------------------------------------------
    AutoAddress::~AutoAddress()
    {
        if(mData)
        {
            N_delete mData;
            mData = 0;
        }
    }
    //------------------------------------------------------------------------
    bool operator == AutoAddress::(const Address & o)
    {
        return mData == o;
    }
    //------------------------------------------------------------------------
    bool operator != AutoAddress::(const Address & o)
    {
        return mData != o;
    }
    //------------------------------------------------------------------------
    AutoAddress::operator Nul() const
    {
        if(mData)
            mData->Nul();
    }
    //------------------------------------------------------------------------
    Nui16 AutoAddress::getHSBPort() const
    {
        if(mData)
            return mData->getHSBPort();
    }
    //------------------------------------------------------------------------
    Nui16 AutoAddress::getNSBPort() const
    {
        if(mData)
            return mData->getNSBPort();
    }
    //------------------------------------------------------------------------
    void AutoAddress::setHSBPort(Nui16 port)
    {
        if(mData)
            mData->setHSBPort(port);
    }
    //------------------------------------------------------------------------
    void AutoAddress::setNSBPort(Nui16 port)
    {
        if(mData)
            mData->setNSBPort(port);
    }
    //------------------------------------------------------------------------
    Nui32 AutoAddress::getVersion() const
    {
        if(mData)
            return mData->getVersion();
    }
    //------------------------------------------------------------------------
    Nui32 AutoAddress::getProto() const
    {
        if(mData)
            return mData->getProto();
    }
    //------------------------------------------------------------------------
    NCount AutoAddress::getSize() const
    {
        if(mData)
            return mData->getSize();
    }
    //------------------------------------------------------------------------
    void AutoAddress::setLocalHost()
    {
        if(mData)
            mData->setLocalHost();
    }
    //------------------------------------------------------------------------
    void AutoAddress::setInvalid()
    {
        if(mData)
            mData->setInvalid();
    }
    //------------------------------------------------------------------------
    bool AutoAddress::isLocalHost() const
    {
        if(mData)
            return mData->isLocalHost();
    }
    //------------------------------------------------------------------------
    bool AutoAddress::isLan() const
    {
        if(mData)
            return mData->isLan();
    }
	//------------------------------------------------------------------------
	bool AutoAddress::isInvalid() const
	{
        if(mData)
            return mData->isInvalid();
        return true;
	}
    //------------------------------------------------------------------------
    void AutoAddress::read(String & out, bool writePort) const
    {
        if(mData)
            mData->read(out, writePort);
    }
    //------------------------------------------------------------------------
    bool AutoAddress::write(const String & in)
    {
        if(IPV4Address::isAddress(in))
        {
            if(mData)
            {
                N_delte mData;
            }
            mData = N_new IPV4Address();
            return mData->write(in);
        }
        else if(IPV6Address::isAddress(in))
        {
            if(mData)
            {
                N_delte mData;
            }
            mData = N_new IPV6Address();
            return mData->write(in);
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool AutoAddress::write(const String & in, Nui16 port)
    {
        if(!write(in))
        {
            *this = INVALID;
            return false;
        }
        mSrc.sin_port = htons(port);
        return true;
    }
    //------------------------------------------------------------------------
    void AutoAddress::read(NetSerializer * out) const
    {
        if(mData)
            mData->read(out);
    }
    //------------------------------------------------------------------------
    void AutoAddress::write(const NetSerializer * in)
    {
		mSrc.addr4.sin_family = AF_INET;
		// in->read(var.binaryAddress);
		// Don't endian swap the address or port
		Nui32 binaryAddress;
		in->readBit((Nui8 *)&binaryAddress, sizeof(binaryAddress) * 8, true);
		// Unhide the IP address, done to prevent routers from changing it
		mSrc.addr4.sin_addr.s_addr = ~binaryAddress;
		bool b = in->readBit((Nui8 *)&mSrc.addr4.sin_port, sizeof(mSrc.addr4.sin_port) * 8, true);
		//mSrc.debugPort = ntohs(mSrc.addr4.sin_port);
		return b;
    }
    //------------------------------------------------------------------------
    Address * AutoAddress::clone() const
    {
        return mData->clone();
    }
    //------------------------------------------------------------------------
    const Address & AutoAddress::getInvalid() const
    {
        return mData->getInvalid();
    }
    //------------------------------------------------------------------------
    const Address & AutoAddress::getLocalHost() const
    {
        return mData->getLocalHost();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // UniqueID
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    const UniqueID UniqueID::INVALID(0);
    //------------------------------------------------------------------------
    UniqueID::UniqueID()
    {
        mGuid = 0;
        mIndex = (Nindex)-1;
    }
    //------------------------------------------------------------------------
    UniqueID::UniqueID(Nui64 _g)
    {
        mGuid = _g;
        mIndex = (Nindex)-1;
    }
    //------------------------------------------------------------------------
    UniqueID & UniqueID::operator = (const UniqueID & o)
    {
        mGuid = o.mGuid;
        mIndex = o.mIndex;
        return *this;
    }
    //------------------------------------------------------------------------
    bool UniqueID::operator== (const UniqueID & o) const
    {
        return mGuid == o.mGuid;
    }
    //------------------------------------------------------------------------
    bool UniqueID::operator != (const UniqueID & o) const
    {
        return mGuid != o.mGuid;
    }
    //------------------------------------------------------------------------
    bool UniqueID::operator > (const UniqueID & o) const
    {
        return mGuid > o.mGuid;
    }
    //------------------------------------------------------------------------
    bool UniqueID::operator < (const UniqueID & o) const
    {
        return mGuid < o.mGuid;
    }
    //------------------------------------------------------------------------
    void UniqueID::read(String & out) const
    {
        if(*this == UniqueID::INVALID)
            out = "INVALID";
        else
            out = StrConv::from(mGuid);
    }
    //------------------------------------------------------------------------
    bool UniqueID::write(const String & in)
    {
        if(in == "INVALID")
            return false;
#if defined(WIN32)
        mGuid = _strtoui64(in.c_str(), NULL, 10);
#else
        // Changed from mGuid = strtoull(source, 0, 10); for android
        mGuid = strtoull(in.c_str(), (char **)NULL, 10);
#endif
        return true;
    }
    //------------------------------------------------------------------------
    void UniqueID::read(NetSerializer * out) const
    {
        out->write(out.mGuid);
    }
    //------------------------------------------------------------------------
    void UniqueID::write(const NetSerializer * in)
    {
        in->read(in.mGuid);
    }
    //------------------------------------------------------------------------
    Nui64 UniqueID::genLocalGuid()
    {
        // Mac address is a poor solution because you can't have multiple connections from the same system
    #if N_PLAT == N_PLAT_WIN32
        Nui64 g = N_Engine().getTimer().getUS();
        TimeDurUS lastTime, thisTime;
        // Sleep a small random time, then use the last 4 bits as a source of randomness
        for (int j = 0; j < 8; ++j)
        {
            lastTime = N_Engine().getTimer().getUS();
            Nsleep(1);
            thisTime = N_Engine().getTimer().getUS();
            TimeDurUS diff = thisTime-lastTime;
            Nui32 diff4Bits = (Nui32) (diff & 15);
            diff4Bits <<= 32-4;
            diff4Bits >>= j*4;
            ((char*)&g)[j] ^= diff4Bits;
        }
        return g;
    #else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_usec + tv.tv_sec * 1000000;
    #endif
    }
    //------------------------------------------------------------------------
    Nul UniqueID::toNum(const UniqueID & guid)
    {
        return ((Nul) (guid.mGuid >> 32)) ^ ((Nul) (guid.mGuid & 0xFFFFFFFF));
    }
    //------------------------------------------------------------------------
    NCount UniqueID::size() 
    {
        return sizeof(Nui64);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // UniqueAddress
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    UniqueAddress::UniqueAddress():
        mID(UniqueID::INVALID),
        mAddress(0)
    {
    }
    //------------------------------------------------------------------------
    UniqueAddress::UniqueAddress(SocketMessage * msg)
    {
        mID = msg->mID;
        mAddress = msg->mAddress.clone();
    }
    //------------------------------------------------------------------------
    UniqueAddress::UniqueAddress(const UniqueID & o) :
        mAddress(0)
    {
        mID = o;
    }
    //------------------------------------------------------------------------
    UniqueAddress::UniqueAddress(const Address * o)
    {
        mID = UniqueID::INVALID;
        if(o)
            mAddress = o->clone();
    }
    //------------------------------------------------------------------------
    UniqueAddress::UniqueAddress(const Address * addr, const UniqueID & id)
    {
        mID = id;
        if(o)
            mAddress = addr->clone();
    }
    //------------------------------------------------------------------------
    UniqueAddress::UniqueAddress(const UniqueAddress & o)
    {
        mID = o.mID;
        mAddress = o.mAddress.clone();
    }
    //------------------------------------------------------------------------
    UniqueAddress::~UniqueAddress()
    {
		if (mAddress != 0)
		{
			N_delete mAddress;
			mAddress = 0;
		}
    }
    //------------------------------------------------------------------------
    UniqueAddress & UniqueAddress::operator = (const UniqueID & o)
    {
		if (mAddress != 0)
		{
			N_delete mAddress;
			mAddress = 0;
		}

        mID = o;
        return *this;
    }
    //------------------------------------------------------------------------
    UniqueAddress & UniqueAddress::operator = (const Address & o)
    {
        if(mAddress != 0)
            N_delete mAddress;

        mID = UniqueID::INVALID;
        mAddress = o.clone();
        return *this;
    }
    //------------------------------------------------------------------------
    UniqueAddress & UniqueAddress::operator = (const UniqueAddress & o)
    {
        if(mAddress != 0)
            N_delete mAddress;

        mID = o.mID;
        mAddress = o.mAddress.clone();
        return *this;
    }
    //------------------------------------------------------------------------
    bool UniqueAddress::operator == (const UniqueAddress & o) const
    {
        return (mID != UniqueID::INVALID && mID == o.mID) ||
            (mAddress != 0 && o.mAddress != 0 && mAddress == *(o.mAddress));
    }
    //------------------------------------------------------------------------
    bool UniqueAddress::isValid() const
    {
        return mID == UniqueID::INVALID && mAddress == 0;
    }
    //------------------------------------------------------------------------
    Nul UniqueAddress::toNum(const UniqueAddress & obj)
    {
        if(obj.mID != UniqueID::INVALID)
            return UniqueID::toNum(obj.mID);
        else if(obj.mAddress != 0)
            return obj.mAddress;
        return 0;
    }
    //------------------------------------------------------------------------
    void UniqueAddress::read(String & out, bool port) const
    {
        if(mID != UniqueID::INVALID)
			mID.read(out);
        else if(mAddress != 0)
            mAddress.read(out, port);
    }
	//------------------------------------------------------------------------
	void UniqueAddress::write(const String & in, bool port)
	{
		if (mID.write(out) == false)
			mAddress.write(in, port);
	}
    //------------------------------------------------------------------------
}