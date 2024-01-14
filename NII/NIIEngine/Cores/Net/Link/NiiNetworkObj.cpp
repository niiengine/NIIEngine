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

#include "NativeFeatureIncludes.h"
#include "NiiPreProcess.h"
#include "NiiNetworkObj.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"
#include "NiiSocketManager.h"
#include "StringTable.h"

namespace NII
{
namespace NII_NET
{
    /** NetObjServerSocketIO
    @version NIIEngine 4.0.0
    */
    class _EngineAPI NetObjServerSocketIO : public ServerSocketIO
    {
        friend class SocketManager;
    public:
        NetObjServerSocketIO(NetworkObj * obj) : mObj(obj){}

        /// @copydetails ServerSocketIO::create
        virtual Message * create(Nui8 * buf, Nui32 size) const
        {
            return N_new SocketMessage(buf, size, true, false);
        }
        
        /// @copydetail ServerSocketIO::createInstance
        virtual SocketIO * createInstance() const
        {
            return N_new NetObjServerSocketIO(mObj);
        }
        
        /// @copydetail ServerSocketIO::onMessage
        void onMessage(Message * msg) 
        {
            mObj->mInMutex.lock();
            mObj->mInList.push_back(msg);
            mObj->mInMutex.lock();
        }
    protected:
        NetworkObj * mObj;
    };
    
    /** NetObjClientSocketIO
    @version NIIEngine 4.0.0
    */
    class _EngineAPI NetObjClientSocketIO : public ClientSocketIO
    {
        friend class SocketManager;
    public:
        NetObjClientSocketIO(NetworkObj * obj) : mObj(obj){}

        /// @copydetails ClientSocketIO::create
        virtual Message * create(Nui8 * buf, Nui32 size) const
        {
            return N_new SocketMessage(buf, size, true, false);
        }
        
        /// @copydetail ClientSocketIO::createInstance
        virtual SocketIO * createInstance() const
        {
            return N_new NetObjClientSocketIO(mObj);
        }
        
        /// @copydetail ClientSocketIO::onMessage
        void onMessage(Message * msg) 
        {
            mObj->mInMutex.lock();
            mObj->mInList.push_back(msg);
            mObj->mInMutex.lock();
        }
    protected:
        NetworkObj * mObj;
    };
    /** socket´¦Àí
    @version NIIEngine 4.0.0
    */
    class _EngineInner NetObjSocketPrc : public SocketPrc
    {
    public:
        NetObjSocketPrc(NetworkObj * obj, SocketIO * io) : SocketPr(io), mObj(obj){}
    public:
        /// @copydetails SocketPrc::onConnect
        virtual void onConnect()
        {
            mObj->mRemoteListMutex.lock();
            mObj->mRemoteList.push_back(mIO);
            mAddress->setIndex(mObj->mRemoteList.size() - 1);            
            mObj->mRemoteListMutex.unlock();

            mObj->mServerConnectMutex.lock();
            mObj->mServerList.push_back(mAddress);
            mObj->mServerConnectMutex.unlock();
        }

        /// @copydetails SocketPrc::onConfirm
        virtual void onConfirm()
        {
            mObj->mRemoteListMutex.lock();
            mObj->mRemoteList.push_back(mIO);
            mAddress->setIndex(mObj->mRemoteList.size() - 1);
            mObj->mRemoteListMutex.unlock();

            mObj->mClientConnectMutex.lock();
            mObj->mClientList.push_back(mAddress);
            mObj->mClientConnectMutex.unlock();
        }

        /// @copydetails SocketPrc::onClose
        virtual void onClose()
        {
            mObj->mConnectCloseMutex.lock();
            mObj->mConnectClosetList.push_back(mAddress);
            mObj->mConnectCloseMutex.unlock();

            mObj->mRemoteListMutex.lock();
            SocketIOList::iterator i, iend = mObj->mRemoteList.end();
            std::advance(i, mAddress->getIndex());
            mObj->mRemoteList.erase(i);
            mObj->mRemoteListMutex.unlock();

            if(mIO->getType() >= SocketIOClientType)
            {
                mObj->mClientConnectMutex.lock();
                AddressList::iterator i, iend = mObj->mClientList.end();
                for(i = mObj->mClientList.begin(); i != iend; ++i)
                {
                    if(*i == *mAddress)
                    {
                        mObj->mClientList.erase(i);
                    }
                }
                mObj->mClientConnectMutex.unlock();
            }
            else
            {
                mObj->mServerConnectMutex.lock();
                AddressList::iterator i, iend = mObj->mServerList.end();
                for(i = mObj->mServerList.begin(); i != iend; ++i)
                {
                    if(*i == *mAddress)
                    {
                        mObj->mServerList.erase(i);
                    }
                }
                mObj->mServerConnectMutex.unlock();
            }
            SocketPrc::onClose();
        }
    protected:
        NetworkObj * mObj;
    };

    class _EngineInner NetObjSSLSocketPrc : public SSLSocketPrc
    {
    public:
        NetObjSSLSocketPrc(NetworkObj * obj, SocketIO * io) : SocketPr(io), mObj(obj){}
    public:
        /// @copydetails SSLSocketPrc::onConnect
        virtual void onConnect()
        {
            mObj->mRemoteListMutex.lock();
            mObj->mRemoteList.push_back(mIO);
            mAddress->setIndex(mObj->mRemoteList.size() - 1);
            mObj->mRemoteListMutex.unlock();
            
            mObj->mServerConnectMutex.lock();
            mObj->mServerList.push_back(mAddress);
            mObj->mServerConnectMutex.unlock();

            mSSLConnectMutex.lockW();
            mObj->mSSLConnectList.push_back(mAddress);
            mSSLConnectMutex.unlockW();
        }

        /// @copydetails SSLSocketPrc::onConfirm
        virtual void onConfirm()
        {
            mObj->mRemoteListMutex.lock();
            mObj->mRemoteList.push_back(mIO);
            mAddress->setIndex(mObj->mRemoteList.size() - 1);
            mObj->mRemoteListMutex.unlock();
            
            mObj->mClientConnectMutex.lock();
            mObj->mClientList.push_back(mAddress);
            mObj->mClientConnectMutex.unlock();

            mSSLConnectMutex.lockW();
            mObj->mSSLConnectList.push_back(mAddress);
            mSSLConnectMutex.unlockW();
        }

        /// @copydetails SSLSocketPrc::onClose
        virtual void onClose()
        {
            mSSLConnectMutex.lockW();
            AddressList::iterator c, cend = mObj->mSSLConnectList.end();
            for(c = mObj->mSSLConnectList.begin(); c != cend; ++c)
            {
                if(*c == *mAddress)
                    mObj->mSSLConnectList.erase(c);
            }
            mSSLConnectMutex.unlockW();
            
            mObj->mConnectCloseMutex.lock();
            mObj->mConnectClosetList.push_back(mAddress);
            mObj->mConnectCloseMutex.unlock();
            
            mObj->mRemoteListMutex.lock();
            SocketIOList::iterator i, iend = mObj->mRemoteList.end();
            std::advance(i, mAddress->getIndex());
            mObj->mRemoteList.erase(i);
            mObj->mRemoteListMutex.unlock();

            if(mIO->getType() >= SocketIOClientType)
            {
                mObj->mClientConnectMutex.lock();                
                AddressList::iterator i, iend = mObj->mClientList.end();
                for(i = mObj->mClientList.begin(); i != iend; ++i)
                {
                    if(*i == *mAddress)
                    {
                        mObj->mClientList.erase(i);
                    }
                }
                mObj->mClientConnectMutex.unlock();
            }
            else
            {
                mObj->mServerConnectMutex.lock();
                AddressList::iterator i, iend = mObj->mServerList.end();
                for(i = mObj->mServerList.begin(); i != iend; ++i)
                {
                    if(*i == *mAddress)
                    {
                        mObj->mServerList.erase(i);
                    }
                }
                mObj->mServerConnectMutex.unlock();            
            }
            SocketPrc::onClose();
        }
    protected:
        NetworkObj * mObj;
    };
    //------------------------------------------------------------------------
    NetworkObj::NetworkObj() :
        mUpdateThread(0)
    {
        N_Only(Socket).init();
    }
    //------------------------------------------------------------------------
    NetworkObj::~NetworkObj()
    {
        stop();
        N_Only(Socket).destroy();
        if(mUpdateThread)
        {
            N_delete mUpdateThread;
            mUpdateThread = 0;
        }
    }
    //------------------------------------------------------------------------
    bool NetworkObj::start(Nui16 maxConnect, NIIi level)
    {
        if (mRun)
            return false;
        mRun = true;
        
        if(maxConnect == 0)
            maxConnect = 1;

        mThreadPrcLevel = level;

        mUpdateThread = N_new Thread("", this, 0, false);
        mUpdateThread->setPriority(mThreadPrcLevel);
        mUpdateThread->create();

        while(mRunThreadCount == 0)
            Nsleep(0);

        PluginList::iterator i, iend = mPluginList.end();
        for(i = mPluginList.begin(); i != iend; ++i) 
            (*i)->init();

        return true;
    }
    //------------------------------------------------------------------------
    void NetworkObj::stop()
    {
        PluginList::iterator i, iend = mPluginList.end();
        for(i = mPluginList.begin(); i != iend; ++i) 
            (*i)->shutdown();

        if (mRun == false)
            return;

        mRun = false;

        while(mRunThreadCount > 0)
            Nsleep(15);

        mInList.clear();
        mServerList.clear();
        mConnectClosetList.clear();
        mConnectFailList.clear();
        mClientList.clear();

        SSLAddressList::iterator j, jend = mSSLAddressList.end();
        for(j = mSSLAddressList.begin(); j != jend; ++j)
        {
            N_delete (*j)->mAddres;
        }
        mSSLAddressList.clear();
        mSSLConnectList.clear();
    }
    //------------------------------------------------------------------------
    Nid NetworkObj::listen(const String & ip, Nui16 port, Nui16 sfamily, Nui16 maxConnect)
    {
        if(mRunThreadCount == 0)
            return 0;
        
        String buffout;
        AutoAddress taddr;
        taddr.read(host);
        taddr.setHSBPort(port);
        taddr.read(buffout, false);
        SSLSocketPrc * prc = 0;
        Nid sockfd = -1;
        bool isok = false;

        mSSLAddressMutex.lockR();
        AddressList::iterator i, iend = mSSLAddressList.end();
        for(i = mSSLAddressList.begin(); i != iend; ++i)
        {
            if(taddr == *ssladdress)
            {
                prc = N_new NetObjSSLSocketPrc();
                prc->initSSLCTX(certfile, keyfile, keypw);
                sockfd = listen(ip, port, sfamily, maxConnect);
                isok = true;
            }
        }
        mSSLAddressMutex.unlockR();
        if(!isok)
        {
            prc = N_new NetObjSocketPrc();
            sockfd = listen(ip, port, sfamily, maxConnect);
        }

        if(sockfd > 0)
            mListenList.push_back(sockfd);
        return sockfd;
    }
    //------------------------------------------------------------------------
    Address * NetworkObj::connect(const String & host, Nui16 port, Nui16 sfamily)
    {
        if(mRunThreadCount == 0)
            return 0;

        String buffout;
        AutoAddress taddr;
        taddr.read(host);
        taddr.setHSBPort(port);
        taddr.read(buffout, false);
        
        SSLSocketPrc * prc = 0;
        Nid sockfd = -1;
        bool isok = false;

        mSSLAddressMutex.lockR();
        AddressList::iterator i, iend = mSSLAddressList.end();
        for(i = mSSLAddressList.begin(); i != iend; ++i)
        {
            if(taddr == *ssladdress)
            {
                prc = N_new NetObjSSLSocketPrc();
                prc->setupSSL(prc->initSSLCTX(certfile, keyfile, keypw));
                sockfd = connect(buffout, port, sfamily);
                isok = true;
            }
        }
        mSSLAddressMutex.unlockR();

        if(!isok)
        {
            prc = N_new NetObjSocketPrc();
            sockfd = connect(buffout, port, sfamily);
        }

        if (sockfd <= 0)
        {
            mConnectFailMutex.lock();
            mConnectFailList.push_back(taddr);
            mConnectFailMutex.unlock();
        }

        return sockfd;
    }
    //------------------------------------------------------------------------
    void NetworkObj::setSSL(const Address & address, const String & certfile, const String & keyfile, const String & keypw)
    {
        SSLData temp;
        temp.mAddres = address.clone();
        temp.mCertFile = certfile;
        temp.mKeyFile = keyfile;
        temp.mKeyPW = keypw;
        bool add = true;

        mSSLAddressMutex.lockR();
        SSLAddressList::iterator j, jend = mSSLAddressList.end();
        for(j = mSSLAddressList.begin(); j != jend; ++j)
        {
            if((*j)->mAddres == address)
            {
                add = false;
                break;
            }
        }
        mSSLAddressMutex.unlockR();
        
        if(add)
        {
            mSSLAddressMutex.lockW();
            mSSLAddressList.push_back(temp);
            mSSLAddressMutex.unlockW();
        }
    }
    //------------------------------------------------------------------------
    bool NetworkObj::isSSLActive(const Address & address)
    {
        ScopeRWLock tlock(mSSLConnectMutex);
        AddressList::iterator c, cend = mSSLConnectList.end();
        for(c = mSSLConnectList.begin(); c != cend; ++c)
        {
            if(*c == address)
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool NetworkObj::send(const Nui8 * data, NCount size, const Address & address, bool broadcast)
    {
        if (mRun == false)
            return false;
        if (address.isInvalid() && broadcast == false)
            return false;

        if(broadcast)
        {
            for(i = 0; i < mRemoteList.size(); ++i)
            {
                if(mRemoteList[i].mAddress != address)
                {
                    mRemoteList[i].send(data, size);
                }
            }
        }
        else
        {
            if(address.mIndex < mRemoteList.size() && mRemoteList[address.mIndex].mAddress == address)
            {
                mRemoteList[address.mIndex].send(data, size);
            }
            else
            {
                for(i = 0; i < mRemoteList.size(); ++i)
                {
                    if(mRemoteList[i].mAddress == address)
                    {
                        mRemoteList[i].send(data, size);
                    }
                }
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    void NetworkObj::update()
    {
        PluginList::iterator i, iend = mPluginList.end();
        for(i = mPluginList.begin(); i != iend; ++i) 
            (*i)->update();

        mServerConnectMutex.lock();
        if(!mServerList.empty())
        {
            Address * out = mServerList.front();            
            mServerList.pop_front();
            mServerConnectMutex.unlock();
            
            if(!out->isInvalid())
            {
                PluginList::iterator i, iend = mPluginList.end();
                for(i = mPluginList.begin(); i != iend; ++i)
                {
                    (*i)->onConnect(out, UniqueID::INVALID);
                }
                onServerConnect(out);
            }
        }
        else
        {
            mServerConnectMutex.unlock();
        }
        
        mClientConnectMutex.lock();
        if(!mClientList.empty())
        {
            out = mClientList.front();            
            mClientList.pop_front();
            mClientConnectMutex.unlock();
            
            if(!out->isInvalid())
            {
                PluginList::iterator i, iend = mPluginList.end();
                for(i = mPluginList.begin(); i != iend; ++i)
                {
                    (*i)->onConfirm(out, UniqueID::INVALID);
                }
                onClientConnect(out);
            }
        }
        else
        {
            mClientConnectMutex.unlock();
        }

        mConnectCloseMutex.lock();
        if(!mConnectClosetList.empty())
        {
            out = mConnectClosetList.front();
            mConnectClosetList.pop_front();
            mConnectCloseMutex.unlock();
            
            if(!out->isInvalid())
            {
                PluginList::iterator i, iend = mPluginList.end();
                for(i = mPluginList.begin(); i != iend; ++i)
                {
                    (*i)->onConnectClose(out, UniqueID::INVALID, CCT_Shutdown);
                }
                onConnectClose(out);
            }
        }
        else
        {
            mConnectCloseMutex.unlock();
        }
        
        mConnectFailMutex.lock();
        if(!mConnectFailList.empty())
        {
            out = mConnectFailList.front();
            mConnectFailList.pop_front();
            mConnectFailMutex.unlock();
            
            if(!out->isInvalid())
            {
                PluginList::iterator i, iend = mPluginList.end();
                for(i = mPluginList.begin(); i != iend; ++i)
                {
                    SocketMessage p(0);
                    p.mAddress = out;
                    (*i)->onConnectFail(&p, CFT_Fail);
                }
                onConnectFail(out);
            }
        }
        else
        {
            mConnectFailMutex.unlock();
        }
    }
    //------------------------------------------------------------------------
    void NetworkObj::run(void * arg)
    {
        mInMutex.lock();
        if(mInList.empty())
        {
            mInMutex.unlock();
        }
        else
        {
            SocketMessage * re = mInList.front();            
            mInList.pop_front();
            mInMutex.unlock();
            PluginList::iterator i, iend = mPluginList.end();
            for(i = mPluginList.begin(); i != iend; ++i) 
            {
                ConnectReceiveType rtype = (*i)->onMessage(re);
                if(rtype == CRT_Destroy)
                {
                    N_delete re;
                    re = 0;
                    break;
                }
                else if(rtype == CRT_Inner)
                {
                    re = 0;
                    break;
                }
            }
            if(re)
            {
                onMessage(re);
            }
        }
    }
    //------------------------------------------------------------------------
    void NetworkObj::attach(Plugin * plugin)
    {
        PluginList::iterator i, iend = mPluginList.end();
        for(i = mPluginList.begin(); i != iend; ++i)
        {
            if((*i) == plugin)
                return;
        }
        mPluginList.push_back(plugin);
        plugin->setPrc(this);
        plugin->onAttach();        
    }
    //------------------------------------------------------------------------
    void NetworkObj::detach(Plugin * plugin)
    {
        PluginList::iterator i, iend = mPluginList.end();
        for(i = mPluginList.begin(); i != iend; ++i)
        {
            if(*i == plugin)
            {
                mPluginList.erase(i);
                plugin->setPrc(0);
                plugin->onDetach();
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void NetworkObj::close(const Address & address)
    {
        if(mRun == false)
            return;
        if(address->isInvalid())
            return;

        PluginList::iterator i, iend = mPluginList.end();
        for(i = mPluginList.begin(); i != iend; ++i)
            (*i)->onConnectClose(address, UniqueID::INVALID, CCT_Close);

        if(address.mIndex < mRemoteList.size() && mRemoteList[address.mIndex].mAddress == address)
        {
            mRemoteList[address.mIndex].abort();
        }
        else
        {
            for(NCount i = 0; i < mRemoteList.size(); ++i)
            {
                if(mRemoteList[i].mAddress == address)
                {
                    mRemoteList[address.mIndex].abort();
                    break;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    SocketMessage * NetworkObj::createMessage(NCount size)
    {
        SocketMessage * p = N_new SocketMessage(size);
        p->mAutoDestroy = false;
        return p;
    }
    //------------------------------------------------------------------------
    void NetworkObj::onMessage(SocketMessage * msg)
    {
        
    }
    //------------------------------------------------------------------------
    void NetworkObj::addMessage(SocketMessage * msg, bool head)
    {
        ScopeLock temp(mInMutex);
        if(head)
            mInList.push_front(msg);
        else
            mInList.push_back(msg);
    }
    //------------------------------------------------------------------------
    bool NetworkObj::isRun() const
    {
        return mRunThreadCount > 0;
    }
    //------------------------------------------------------------------------
    void NetworkObj::onClientConnect(Address * address)
    {
        mPrcClientList.push_back(address);
    }
    //------------------------------------------------------------------------
    void NetworkObj::onConnectFail(Address * address)
    {
        mPrcClientFailList.push_back(address);
    }
    //------------------------------------------------------------------------
    void NetworkObj::onServerConnect(Address * address)
    {
        mPrcServerList.push_back(address);
    }
    //------------------------------------------------------------------------
    void NetworkObj::onConnectClose(Address * address)
    {
        mPrcClientCloseList.push_back(address);
    }
    //------------------------------------------------------------------------
    void NetworkObj::getConnect(AddressList & remotelist) const
    {
        remotelist = mRemoteList;
    }
    //------------------------------------------------------------------------
    Nui32 NetworkObj::getOutputSize(const Address & address) const
    {
        Nui32 re = 0;
        if (address.mIndex < mRemoteList.size() && mRemoteList[address.mIndex].mAddress == address)
        {
            re = mRemoteList[address.mIndex]->getSendSize();
            return re;
        }

        for(int i = 0; i < mRemoteList.size(); ++i)
        {
            if(mRemoteList[i].mAddress == address)
            {
                re += mRemoteList[i]->getSendSize();
            }
        }
        return re;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ClusterNetworkObj
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    /** NetObjServerSocketIO
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ClusterServerSocketIO : public ServerSocketIO
    {
        friend class SocketManager;
    public:
        ClusterServerSocketIO(NetworkObj * obj) : ServerSocketIO(obj), mLastSize(0){}

        /// @copydetails ServerSocketIO::create
        virtual Message * create(Nui8 * buf, Nui32 size) const
        {
            Message * re = 0;
            Nui32 headsize = *(Nui32 *)buf;
            if(NetSerializer::isFlipData())
                Serializer::mirror((Nui8 *)&headsize, sizeof(headsize));
            // Header indicates packet size. If enough data is available, read out and return one packet
            if(size >= headsize + sizeof(Nui32))
            {
                re = N_new SocketMessage(buf + sizeof(Nui32), headsize, true, false);
                re->mAddress = inMsg->mAddress;
            }
            else
            {
                Nui32 oldWritten = mLastSize - size;
                Nui32 newWritten = mLastSize;

                // Return ID_DOWNLOAD_PROGRESS
                if (newWritten / 65536 != oldWritten / 65536)
                {
                    re = N_new SocketMessage(sizeof(Token) + sizeof(Nui32) * 2 + sizeof(Nui32) + 65536, false);
                    re->mBitCount = N_b_TO_B(size);
                    re->mAddress = inMsg->mAddress;
                    re->mData[0] = (Token)ID_DOWNLOAD_PROGRESS;
                    
                    Nui32 totalParts = headsize / 65536;
                    Nui32 partIndex = newWritten / 65536;
                    Nui32 oneChunkSize = 65536;
                    memcpy(re->mData + sizeof(Token), &partIndex, sizeof(Nui32));
                    memcpy(re->mData + sizeof(Token) + sizeof(Nui32) * 1, &totalParts, sizeof(Nui32));
                    memcpy(re->mData + sizeof(Token) + sizeof(Nui32) * 2, &oneChunkSize, sizeof(Nui32));
                    re->writeSkip(sizeof(Token) + sizeof(Nui32) * 3);
                    re->write(buf + sizeof(Nui32), oneChunkSize);
                }
            }
            mLastSize = size;
            return re;
        }
    protected:
        Nui32 mLastSize;
    };
    
    /** NetObjClientSocketIO
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ClusterClientSocketIO : public ClientSocketIO
    {
        friend class SocketManager;
    public:
        ClusterClientSocketIO(NetworkObj * obj) : ClientSocketIO(obj), mLastSize(0){}

        /// @copydetails ClientSocketIO::create
        virtual Message * create(Nui8 * buf, Nui32 size) const
        {
            Message * re = 0;
            Nui32 headsize = *(Nui32 *)buf;
            if(NetSerializer::isFlipData())
                Serializer::mirror((Nui8 *)&headsize, sizeof(headsize));
            // Header indicates packet size. If enough data is available, read out and return one packet
            if(size >= headsize + sizeof(Nui32))
            {
                re = N_new SocketMessage(buf + sizeof(Nui32), headsize, true, false);
                re->mAddress = inMsg->mAddress;
            }
            else
            {
                Nui32 oldWritten = mLastSize - size;
                Nui32 newWritten = mLastSize;

                // Return ID_DOWNLOAD_PROGRESS
                if (newWritten / 65536 != oldWritten / 65536)
                {
                    re = N_new SocketMessage(sizeof(Token) + sizeof(Nui32) * 2 + sizeof(Nui32) + 65536, false);
                    re->mBitCount = N_b_TO_B(size);
                    re->mAddress = inMsg->mAddress;
                    re->mData[0] = (Token)ID_DOWNLOAD_PROGRESS;
                    
                    Nui32 totalParts = headsize / 65536;
                    Nui32 partIndex = newWritten / 65536;
                    Nui32 oneChunkSize = 65536;
                    memcpy(re->mData + sizeof(Token), &partIndex, sizeof(Nui32));
                    memcpy(re->mData + sizeof(Token) + sizeof(Nui32) * 1, &totalParts, sizeof(Nui32));
                    memcpy(re->mData + sizeof(Token) + sizeof(Nui32) * 2, &oneChunkSize, sizeof(Nui32));
                    re->writeSkip(sizeof(Token) + sizeof(Nui32) * 3);
                    re->write(buf + sizeof(Nui32), oneChunkSize);
                }
            }
            mLastSize = size;
            return re;
        }
    protected:
        Nui32 mLastSize;
    };
    //------------------------------------------------------------------------
}
}