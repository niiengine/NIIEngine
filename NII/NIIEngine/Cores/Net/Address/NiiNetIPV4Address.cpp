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
#include "NiiIPV4Address.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    const IPV4Address IPV4Address::LOCALHOST = IPV4Address(String("127.0.0.1"));
    const IPV4Address IPV4Address::INVALID = IPV4Address();
    //------------------------------------------------------------------------
    IPV4Address::IPV4Address() :
        mIndex(0)
    {
        memset(&mSrc, 0, sizeof(struct sockaddr_in));
        mSrc.sin_family = AF_INET;
    }
    //------------------------------------------------------------------------
	IPV4Address::IPV4Address(const IPV4Address & o) :
        mIndex(o.mIndex)
    {
        memcpy(&mSrc, &o.mSrc, sizeof(struct sockaddr_in));
    }
    //------------------------------------------------------------------------
    IPV4Address::IPV4Address(const String & src) :
        mIndex(0)
    {
        mSrc.sin_family = AF_INET;
        setHSBPort(0);
        write(src);
    }
    //------------------------------------------------------------------------
    IPV4Address::IPV4Address(const String & src, Nui16 port) :
        mIndex(0)
    {
        mSrc.sin_family = AF_INET;
        write(src, port);
    }
    //------------------------------------------------------------------------
    IPV4Address & IPV4Address::operator = (const IPV4Address & o)
    {
        memcpy(&mSrc, &o.mSrc, sizeof(struct sockaddr_in));
        mIndex = o.mIndex;
        return *this;
    }
    //------------------------------------------------------------------------
    bool IPV4Address::operator == (const Address & o)
    {
        const IPV4Address * temp = &dynamic_cast<const IPV4Address &>(o);
        if(temp)
            return mSrc.sin_port == o.mSrc.sin_port && equal(o);
        return false;
    }
    //------------------------------------------------------------------------
    bool IPV4Address::operator != (const Address & o)
    {
        const IPV4Address * temp = &dynamic_cast<const IPV4Address &>(o);
        if(temp)
            return mSrc.sin_port != o.mSrc.sin_port || !equal(o);
        return true;
    }
    //------------------------------------------------------------------------
    bool IPV4Address::operator > (const IPV4Address & o)
    {
        if(mSrc.sin_port == o.mSrc.sin_port)
        {
            return mSrc.sin_addr.s_addr > o.mSrc.sin_addr.s_addr;
        }
        return mSrc.sin_port > o.mSrc.sin_port;
    }
    //------------------------------------------------------------------------
    bool IPV4Address::operator < (const IPV4Address & o)
    {
        if(mSrc.sin_port == o.mSrc.sin_port)
        {
            return mSrc.sin_addr.s_addr < o.mSrc.sin_addr.s_addr;
        }
        return mSrc.sin_port < o.mSrc.sin_port;
    }
    //------------------------------------------------------------------------
    IPV4Address::operator Nul() const
    {
        Nui32 last = FastHash((const char *) &mSrc.sin_port,
            sizeof(mSrc.sin_port), sizeof(mSrc.sin_port));

        return FastHash((const char *) &mSrc.sin_addr.s_addr,
            sizeof(mSrc.sin_addr.s_addr), last);
    }
    //------------------------------------------------------------------------
    Nui16 IPV4Address::getHSBPort() const
    {
        return ntohs(mSrc.sin_port);
    }
    //------------------------------------------------------------------------
    Nui16 IPV4Address::getNSBPort() const
    {
        return mSrc.sin_port;
    }
    //------------------------------------------------------------------------
    void IPV4Address::setHSBPort(Nui16 port)
    {
        mSrc.sin_port = htons(port);
    }
    //------------------------------------------------------------------------
    void IPV4Address::setNSBPort(Nui16 port)
    {
        mSrc.sin_port = port;
    }
    //------------------------------------------------------------------------
    Nui32 IPV4Address::getVersion() const
    {
        return AF_INET;
    }
    //------------------------------------------------------------------------
    Nui32 IPV4Address::getProto() const
    {
        return IPPROTO_IP;
    }
    //------------------------------------------------------------------------
    NCount IPV4Address::getSize() const
    {
        return sizeof(Nul) + sizeof(Nui16) + sizeof(char);
    }
    //------------------------------------------------------------------------
    void IPV4Address::setLocalHost()
    {
        String temp("127.0.0.1");
        write(temp, 0);
    }
    //------------------------------------------------------------------------
    void IPV4Address::setInvalid()
    {
        this = INVALID;
    }
    //------------------------------------------------------------------------
    bool IPV4Address::isLocalHost() const
    {
        if(htonl(mSrc.sin_addr.s_addr) == 2130706433)
            return true;

        if(mSrc.sin_addr.s_addr == 0)
            return true;

        return false;
    }
    //------------------------------------------------------------------------
    bool IPV4Address::isLan() const
    {
    #if defined(__WIN32__)
        return mSrc.sin_addr.S_un.S_un_b.s_b1 == 10 || mSrc.sin_addr.S_un.S_un_b.s_b1 == 192;
    #else
        return (mSrc.sin_addr.s_addr >> 24) == 10 || (mSrc.sin_addr.s_addr >> 24) == 192;
    #endif
    }
	//------------------------------------------------------------------------
	bool IPV4Address::isInvalid() const
	{
		return mSrc.sin_port == INVALID.mSrc.sin_port && equal(INVALID);
	}
    //------------------------------------------------------------------------
    void IPV4Address::read(String & out, bool writePort) const
    {
        if (*this == INVALID)
        {
            out = "INVALID";
            return;
        }

        in_addr in;
        in.s_addr = mSrc.sin_addr.s_addr;
        out = inet_ntoa(in);

        if (writePort)
        {
            char port[32];
            i_a(getHSBPort(), port, 32);
            out += ':';
            out += port;
        }
    }
    //------------------------------------------------------------------------
    bool IPV4Address::write(const String & in)
    {
        if(isNum(in))
        {
            const char * src = in.c_str();
            NIIi i, portIndex;
            char ip[22];
            char port[10];
            for (i = 0; src[i] && src[i] != ':' && i < 22; ++i)
            {
                if (src[i] != '.' && (src[i] < '0' || src[i] > '9'))
                    break;
                ip[i] = src[i];
            }
            ip[i] = 0;
            port[0] = 0;
            if (src[i] && src[i + 1])
            {
                i++;
                for (portIndex = 0; portIndex < 10 && src[i] && i < 22 + 10; ++i, ++portIndex)
                {
                    if (src[i] < '0' || src[i] > '9')
                        break;

                    port[portIndex] = src[i];
                }
                port[portIndex] = 0;
            }

            if (ip[0])
            {
                mSrc.sin_addr.s_addr = nii_inet_addr(ip);
            }

            if (port[0])
            {
                mSrc.sin_port = htons((Nui16)atoi(port));
            }
        }
        else
        {
        #if N_PLAT == N_PLAT_WIN32
            if (_strnicmp(src, "LOCALHOST", 9) == 0)
        #else
            if (strncasecmp(src, "LOCALHOST", 9) == 0)
        #endif
            {
                mSrc.sin_addr.s_addr = nii_inet_addr("127.0.0.1");
                if(src[9])
                {
                    setHSBPort((Nui16)atoi(src + 9));
                }
                return true;
            }

            VString ip = N_Only(Net).resolveDomain(src);

            if(ip.size())
            {
                mSrc.sin_addr.s_addr = nii_inet_addr(ip);

            }
            else
            {
                *this = INVALID;
                return false;
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool IPV4Address::write(const String & in, Nui16 port)
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
    void IPV4Address::read(NetSerializer * out) const
    {
		// Hide the address so routers don't modify it
		Address * var2 = clone();
		Nui32 binaryAddress = ~mSrc.addr4.sin_addr.s_addr;
		// Don't endian swap the address or port
		out->writeBit((Nui8 *)&binaryAddress, sizeof(binaryAddress) * 8, true);
		Nui16 p = var2->getNSBPort();
		out->writeBit((Nui8 *)&p, sizeof(Nui16) * 8, true);
		N_delete var2;
    }
    //------------------------------------------------------------------------
    void IPV4Address::write(const NetSerializer * in)
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
    void IPV4Address::setPort(const IPV4Address & src)
    {
        mSrc.sin_port = src.mSrc.sin_port;
    }
    //------------------------------------------------------------------------
    bool IPV4Address::equal(const IPV4Address & o)
    {
        return (mSrc.sin_family == AF_INET && mSrc.sin_addr.s_addr == o.sin_addr.s_addr);
    }
    //------------------------------------------------------------------------
    Address * clone() const
    {
        return N_new IPV4Address(*this);
    }
    //------------------------------------------------------------------------
    const Address & IPV4Address::getInvalid() const
    {
        return INVALID;
    }
    //------------------------------------------------------------------------
    const Address & IPV4Address::getLocalHost() const
    {
        return LOCALHOST;
    }
    //------------------------------------------------------------------------
    bool IPV4Address::isAddress(const String & addr)
    {
        bool num = true;
        NCount chart = 0;
        NCount dot = 0;
        NCount colon = 0;
        String::iterator i, iend = addr.end();
        for(i = addr.begin(); i != iend; ++i)
		{
			// IPV4: natpunch.jenkinssoftware.com
			// IPV6: fe80::7c:31f7:fec4:27de%14
			if ((*i >= 'g' && *i <= 'z') || (*i >= 'G' && *i <= 'Z'))
				num = false;
            else if(*i == '.')
                ++dot;
            else if(*i == ':')
                ++colon;
            else
                ++chart;
		}
        if(colon > 1)
            return false;

    #if N_PLAT == N_PLAT_WIN32
        if (_strnicmp(addr.c_str(), "LOCALHOST", 9) == 0)
    #else
        if (strncasecmp(addr.c_str(), "LOCALHOST", 9) == 0)
    #endif
        {
            return true;
        }
                
		return false;
    }
    //------------------------------------------------------------------------
}
}