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
#include "NiiNetSecure.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    class _EngineInner Ban::Data
    {
    public:
        Data(const String & ip) : 
            mIP(ip){}

        String mIP;
        TimeDurMS mTimeOut; // 0 for none
    };
    //------------------------------------------------------------------------
    // Description:
    // Bans an IP from connecting. Banned IPs persist between connections.
    //
    // Parameters
    // IP - Dotted IP address.  Can use * as a wildcard, such as 128.0.0.* will ban
    // All IP addresses starting with 128.0.0
    // ms - how many ms for a temporary ban.  Use 0 for a permanent ban
    //------------------------------------------------------------------------
    void Ban::add(const String & ip, TimeDurMS ms)
    {
        TimeDurMS time = N_Engine().getTimer().getMS();

        if(ip == String("") || ip.length() > 15)
            return ;

        {
        ScopeMute tmutex(mMutex);
        Bans::iterator i, iend = mBans.end();
        for(i = mBans.begin(); i != iend; ++i)
        {
            if(ip == (*i)->mIP)
            {
                // Already in the ban list.  Just update the time
                if(ms == 0)
                    (*i)->mTimeOut = 0; // Infinite
                else
                    (*i)->mTimeOut = time + ms;
                return;
            }
        }
        }

        Data * ban = N_new Data(ip);
        if(ms == 0)
            ban->mTimeOut = 0; // Infinite
        else
            ban->mTimeOut = time + ms;

        {
        ScopeMute tmutex(mMutex);
        mBans.push_back(ban);
        }
    }
    //------------------------------------------------------------------------
    // Description:
    // Allows a previously banned IP to connect.
    //
    // Parameters
    // IP - Dotted IP address.  Can use * as a wildcard, such as 128.0.0.* will ban
    // All IP addresses starting with 128.0.0
    //------------------------------------------------------------------------
    void Ban::remove(const String & ip)
    {
        if(ip == String("") || ip.length() > 15)
            return ;

        ScopeMute tmutex(mMutex);
        Bans::iterator i, iend = mBans.end();
        for(i = mBans.begin(); i != iend; ++i)
        {
            if(ip == (*i)->mIP)
            {
                mBans.erase(i);
                N_delete *i;
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    bool Ban::isExist(const String & ip)
    {
        if(ip == String("") || ip.length() > 15)
            return false;

        TimeDurMS time= N_Engine().getTimer().getMS();

        mMutex.lock();
        Bans::iterator i, iend = mBans.end();
        for(i = mBans.begin(); i != iend; ++i)
        {
            if((*i)->mTimeOut > 0 && (*i)->mTimeOut < time)
            {
                mBans.erase(i);
                N_delete *i;
            }
            else
            {
                if((*i)->mIP == ip)
                {
                    mMutex.unlock();
                    return true;
                }
                else
                {
                    if((*i)->mIP == "*")
                    {
                        mMutex.unlock();
                        return true;
                    }
                }
                ++i;
            }
        }
        mMutex.unlock();
        return false;
    }
    //------------------------------------------------------------------------
    void Ban::removeAll()
    {
        ScopeMute tmutex(mMutex);
        Bans::iterator i, iend = mBans.end();
        for(i = mBans.begin(); i != mBans.end(); ++i)
        {
            N_delete *i;
        }
        mBans.clear();
    }
    //------------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Secure::Data
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    class _EngineInner Secure::Data
    {
    public:
        Data()
        {
            CAT_AUDIT_PRINTF("AUDIT: Initializing NetworkSys security flags: using_security = false, server_handshake = null, cookie_jar = null\n");
            _using_security = false;
            _server_handshake = 0;
            _cookie_jar = 0;
        }
        
        ~Data()
        {
            CAT_AUDIT_PRINTF("AUDIT: Deleting NetworkSys security objects, handshake = %x, cookie jar = %x\n", _server_handshake, _cookie_jar);
            if(_server_handshake) 
                N_delete _server_handshake;
            if(_cookie_jar) 
                N_delete _cookie_jar;
        }
        
        void setSecure(bool b)
        {
            CAT_AUDIT_PRINTF("AUDIT: setSecure() called, so deleting _server_handshake %x and cookie_jar %x\n", _server_handshake, _cookie_jar);
            N_delete _server_handshake;
            _server_handshake = 0;
            N_delete _cookie_jar;
            _cookie_jar = 0;

            _using_security = b;
        }
        
        bool isSecure() const
        {
            return _using_security;
        }
        
        bool init(const Nui8 * publicKey, const Nui8 * privateKey, bool clientKey)
        {
            if(mStop == false)
                return false;

            // Copy client public key requirement flag
            _require_client_public_key = clientKey;

            if(_server_handshake)
            {
                CAT_AUDIT_PRINTF("AUDIT: Deleting old server_handshake %x\n", _server_handshake);
                N_delete _server_handshake;
            }
            if(_cookie_jar)
            {
                CAT_AUDIT_PRINTF("AUDIT: Deleting old cookie jar %x\n", _cookie_jar);
                N_delete _cookie_jar;
            }

            _server_handshake = N_new cat::ServerEasyHandshake();
            _cookie_jar = N_new cat::CookieJar();

            CAT_AUDIT_PRINTF("AUDIT: Created new server_handshake %x\n", _server_handshake);
            CAT_AUDIT_PRINTF("AUDIT: Created new cookie jar %x\n", _cookie_jar);
            CAT_AUDIT_PRINTF("AUDIT: Running _server_handshake->Initialize()\n");

            if (_server_handshake->Initialize(publicKey, privateKey))
            {
                CAT_AUDIT_PRINTF("AUDIT: Successfully initialized, filling cookie jar with goodies, storing public key and setting using security flag to true\n");

                _server_handshake->FillCookieJar(_cookie_jar);

                memcpy(my_public_key, publicKey, sizeof(my_public_key));

                _using_security = true;
                return true;
            }

            CAT_AUDIT_PRINTF("AUDIT: Failure to initialize so deleting server handshake and cookie jar; also setting using_security flag = false\n");

            N_delete _server_handshake;
            _server_handshake = 0;
            N_delete _cookie_jar;
            _cookie_jar = 0;
            _using_security = false;
            return false;
        }
    private:
        bool _using_security, _require_client_public_key;
        char my_public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
        cat::ServerEasyHandshake * _server_handshake;
        cat::CookieJar * _cookie_jar;
    };
    //------------------------------------------------------------------------
    Secure::Secure()
    {
    #if LIBCAT_SECURITY == 1
        mData = new Data();
    #endif
    }
    //------------------------------------------------------------------------
    Secure::~Secure()
    {
    #if LIBCAT_SECURITY == 1
        delete mData;
        mData = 0;
    #endif
    }
    //------------------------------------------------------------------------
    void Secure::setSecure(bool b)
    {
    #if LIBCAT_SECURITY == 1
        mData->setSecure(b);
    #endif
    }
    //------------------------------------------------------------------------
    bool Secure::isSecure() const
    {
        return mData->isSecure();
    }
    //------------------------------------------------------------------------
    // Description:
    // Must be called while offline
    //
    // If you accept connections, you must call this or else security will not be enabled for incoming connections.
    //
    // This feature requires more round trips, bandwidth, and CPU time for the connection handshake
    // x64 builds require under 25% of the CPU time of other builds
    //
    // See the Encryption sample for example usage
    //
    // Parameters:
    // publicKey = A pointer to the public key for accepting new connections
    // privateKey = A pointer to the private key for accepting new connections
    // If the private keys are 0, then a new key will be generated when this function is called
    // clientKey: Should be set to false for most servers.  Allows the server to accept a public key from connecting clients as a proof of identity but eats twice as much CPU time as a normal connection
    //------------------------------------------------------------------------
    bool Secure::init(const Nui8 * publicKey, const Nui8 * privateKey, bool clientKey)
    {
    #if LIBCAT_SECURITY == 1
        mData->init(publicKey, privateKey, clientKey);
    #else
        (void) publicKey;
        (void) privateKey;
        (void) clientKey;

        return false;
    #endif
    }
    //------------------------------------------------------------------------
    void Secure::add(const String & ip)
    {
        ScopeRWMute tmutex(mMutex, true);
        mIPList.push_back(ip);
    }
    //------------------------------------------------------------------------
    void Secure::remove(const String & ip)
    {
        ScopeRWMute tmutex(mMutex, true);
        IPList::iterator i, iend = mIPList.end();
        for(i = mIPList.begin(); i != iend;)
        {
            if(Address::isEqual(*i, ip))
            {
                IPList::iterator temp = i++;
                mIPList.erase(temp);
            }
            else
            {
                ++i;
            }
        }
    }
    //------------------------------------------------------------------------
    bool Secure::isExist(const String & ip)
    {
        ScopeRWMute tmutex(mMutex);
        IPList::iterator i, iend = mIPList.end();
        for(i = mIPList.begin(); i != iend;)
        {
            if(Address::isEqual(*i, ip))
            {
                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void Secure::removeAll()
    {
        ScopeRWMute tmutex(mMutex, true);
        mIPList.clear();
    }
    //------------------------------------------------------------------------
}
}