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
#include "NiiIPV6Address.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    const IPV6Address IPV6Address::LOCALHOST = IPV6Address(String("::1"));
    const IPV6Address IPV6Address::INVALID = IPV6Address();
    //------------------------------------------------------------------------
    IPV6Address::IPV6Address() :
        mIndex(0)
    {
        memset(&mSrc, 0, sizeof(struct sockaddr_in6));
        mSrc.sin6_family = AF_INET6;
    }
    //------------------------------------------------------------------------
	IPV6Address::IPV6Address(const IPV6Address & o) :
        mIndex(o.mIndex)
    {
        memcpy(&mSrc, &o.mSrc, sizeof(struct sockaddr_in6));
    }
    //------------------------------------------------------------------------
    IPV6Address::IPV6Address(const String & src) :
        mIndex(0)
    {
        mSrc.sin6_family = AF_INET6;
        setHSBPort(0);
        write(src);
    }
    //------------------------------------------------------------------------
    IPV6Address::IPV6Address(const String & src, Nui16 port) :
        mIndex(0)
    {
        mSrc.sin6_family = AF_INET6;
        write(src, port);
    }
    //------------------------------------------------------------------------
    IPV6Address & IPV6Address::operator = (const IPV6Address & o)
    {
        memcpy(&mSrc, &o.mSrc, sizeof(struct sockaddr_in6));
        mIndex = o.mIndex;
        return *this;
    }
    //------------------------------------------------------------------------
    bool IPV6Address::operator == (const IPV6Address & o)
    {
        const IPV6Address * temp = &dynamic_cast<const IPV6Address &>(o);
        if(temp)
            return mSrc.sin6_port == o.mSrc.sin6_port && equal(o);
        return false;
    }
    //------------------------------------------------------------------------
    bool IPV6Address::operator != (const IPV6Address & o)
    {
        const IPV6Address * temp = &dynamic_cast<const IPV6Address &>(o);
        if(temp)
            return mSrc.sin6_port != o.mSrc.sin6_port || !equal(o);
        return true;
    }
    //------------------------------------------------------------------------
    bool IPV6Address::operator > (const IPV6Address & o)
    {
        if (mSrc.sin6_port == o.mSrc.sin6_port)
        {
            return memcmp(mSrc.sin6_addr.s6_addr, o.mSrc.sin6_addr.s6_addr,
                sizeof(mSrc.sin6_addr.s6_addr)) > 0;
        }
        return mSrc.sin6_port > o.mSrc.sin6_port;
    }
    //------------------------------------------------------------------------
    bool IPV6Address::operator < (const IPV6Address & o)
    {
        if (mSrc.sin6_port == o.mSrc.sin6_port)
        {
            return memcmp(mSrc.sin6_addr.s6_addr, o.mSrc.sin6_addr.s6_addr,
                sizeof(mSrc.sin6_addr.s6_addr)) > 0;
        }
        return mSrc.sin6_port < o.mSrc.sin6_port;
    }
    //------------------------------------------------------------------------
    IPV6Address::operator Nul() const
    {
        Nui32 last = FastHash((const char *) & mSrc.sin6_port,
            sizeof(mSrc.sin6_port), sizeof(mSrc.sin6_port));
        return FastHash((const char *) & mSrc.sin6_addr.s6_addr,
            sizeof(mSrc.sin6_addr.s6_addr), last);
    }
    //------------------------------------------------------------------------
    Nui16 IPV6Address::getHSBPort() const
    {
        return ntohs(mSrc.sin6_port);
    }
    //------------------------------------------------------------------------
    Nui16 IPV6Address::getNSBPort() const
    {
        return mSrc.sin6_port;
    }
    //------------------------------------------------------------------------
    void IPV6Address::setHSBPort(Nui16 port)
    {
        mSrc.sin6_port = htons(port);
    }
    //------------------------------------------------------------------------
    void IPV6Address::setNSBPort(Nui16 port)
    {
        mSrc.sin6_port = port;
    }
    //------------------------------------------------------------------------
    Nui32 IPV6Address::getVersion() const
    {
        return AF_INET6;
    }
    //------------------------------------------------------------------------
    Nui32 IPV6Address::getProto() const
    {
        return IPPROTO_IPV6;
    }
    //------------------------------------------------------------------------
    NCount IPV6Address::getSize() const
    {
        return sizeof(sockaddr_in6) + sizeof(char);
    }
    //------------------------------------------------------------------------
    void IPV6Address::setLocalHost()
    {
        String temp("::1");
        write(temp, 0);
    }
    //------------------------------------------------------------------------
    void IPV4Address::setInvalid()
    {
        this = INVALID;
    }
    //------------------------------------------------------------------------
    bool IPV6Address::isLocalHost() const
    {
        const static char LOCALHOST[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
        if (memcmp(&mSrc.sin6_addr, LOCALHOST, 16) == 0)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    bool IPV6Address::isLan() const
    {
    #if defined(__WIN32__)
        return mSrc.sin6_addr.S_un.S_un_b.s_b1 == 10 || mSrc.sin6_addr.S_un.S_un_b.s_b1 == 192;
    #else
        return (mSrc.sin6_addr.s6_addr >> 24) == 10 || (mSrc.sin6_addr.s6_addr >> 24) == 192;
    #endif
    }
	//------------------------------------------------------------------------
	bool IPV6Address::isInvalid() const
	{
		return mSrc.sin6_port == INVALID.mSrc.sin6_port && equal(INVALID);
	}
    //------------------------------------------------------------------------
    void IPV6Address::read(String & out, bool port_) const
    {
        char ip[70];
        if(*this == INVALID)
        {
            out = "INVALID";
            return;
        }

        int ret = getnameinfo((struct sockaddr *) &mSrc, sizeof(struct sockaddr_in6),
            ip, INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);

        if(ret != 0)
        {
            out = "INVALID";
            return;
        }
        out = ip;
        if (port_)
        {
            char port[32];
            i_a(getHSBPort(), port, 32);
            out += ':';
            out += port;
        }
    }
    //------------------------------------------------------------------------
    bool IPV6Address::write(const String & in)
    {
        if(in == "")
        {
            memset(&mSrc, 0, sizeof(mSrc));
            mSrc.sin_family = AF_INET6;
            return true;
        }

        const char * out = in.c_str();
        char ip[INET6_ADDRSTRLEN];
        char port[32];
        int i = 0, j;

        // TODO - what about 255.255.255.255?
        if(isNum(out))
        {
            for (; i < sizeof(ip) && out[i] != 0 && out[i] != ']'; ++i)
            {
                if ((out[i] < '0' || out[i] > '9') && (out[i] < 'a' || out[i] > 'f') && (out[i] < 'A' || out[i] > 'F'))
                    break;

                ip[i] = out[i];
            }
            ip[i] = 0;
        }
        else
        {
            strncpy(ip, out, sizeof(ip));
            ip[sizeof(ip) - 1] = 0;
        }

        j = 0;
        if (out[i] == ']' && out[i + 1] == ':')
        {
            i += 2;
            for (; j < sizeof(port) && out[i] != 0; ++i, ++j)
            {
                port[j] = out[i];
            }
        }
        port[j] = 0;

        N_Only(Socket).init();
        // This could be a domain, or a printable mSrc such as "192.0.2.1" or "2001:db8:63b3:1::3490"
        // I want to convert it to its binary representation
        addrinfo hints, * servinfo = 0;
        memset(&hints, 0, sizeof hints);

        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_family = AF_INET6;

        getaddrinfo(ip, "", &hints, &servinfo);
        if(servinfo == 0)
            return false;

        Nui16 oldPort = mSrc.sin6_port;

        mSrc.sin_family = AF_INET6;
        memcpy(&mSrc, (struct sockaddr_in6 *)servinfo->ai_addr,
            sizeof(struct sockaddr_in6));

        freeaddrinfo(servinfo);

        N_Only(Socket).destroy();

        if(port[0])
        {
            mSrc.sin6_port = htons((Nui16)atoi(port));
        }
        else
        {
            mSrc.sin6_port = oldPort;
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool IPV6Address::write(const String & in, Nui16 port)
    {
        bool b = write(in);
        if(b == false)
        {
            *this = INVALID;
            return false;
        }
        mSrc.sin6_port = htons(port);
        return true;
    }
    //------------------------------------------------------------------------
    void IPV6Address::read(NetSerializer * out) const
	{
		// Don't endian swap
		out->writeBit((const Nui8 *)&mSrc.addr6, sizeof(mSrc.addr6) * 8, true);
    }
    //------------------------------------------------------------------------
    void IPV6Address::write(const NetSerializer * in)
    {
		bool b = in->readBit((Nui8 *)&mSrc.addr6, sizeof(mSrc.addr6) * 8, true);
		//mSrc.debugPort = ntohs(mSrc.addr6.sin6_port);
		return b;
    }
    //------------------------------------------------------------------------
    void IPV6Address::setPort(const IPV6Address & src)
    {
        mSrc.sin6_port = src.mSrc.sin6_port;
    }
    //------------------------------------------------------------------------
    bool IPV6Address::equal(const IPV6Address & o)
    {
        return (mSrc.sin6_family == AF_INET6 &&
            memcmp(mSrc.sin6_addr.s6_addr, o.mSrc.sin6_addr.s6_addr,
                sizeof(mSrc.sin6_addr.s6_addr)) == 0);
    }
    //------------------------------------------------------------------------
    const Address & IPV6Address::getInvalid() const
    {
        return INVALID;
    }
    //------------------------------------------------------------------------
    const Address & IPV6Address::getLocalHost() const
    {
        return LOCALHOST;
    }
    //------------------------------------------------------------------------
    Address * IPV6Address::clone() const
    {
        return N_new IPV6Address(*this);
    }
    //------------------------------------------------------------------------
    bool IPV6Address::isAddress(const String & addr)
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
            return true;
		return false;
    }
    //------------------------------------------------------------------------
}
}