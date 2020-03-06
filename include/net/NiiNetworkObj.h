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

#ifndef _NII_NETWORKOBJ_H_
#define _NII_NETWORKOBJ_H_

#include "NiiNetPreInclude.h"
#include "NiiThreadManager.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"

namespace NII
{
namespace NII_NET
{
    /** �������
    @version NIIEngine 4.0.0
    */
    class _EngineAPI NetworkObj
    {
        friend class NetObjSocketPrc;
    public:
        NetworkObj();
        virtual ~NetworkObj();
        
        /** �Ƿ�����
        @version NIIEngine 4.0.0
        */
        bool isRun() const;
        
        /** ����
        @version NIIEngien 4.0.0
        */
        bool run(Nui16 maxLink, NIIi prclevel = N_ThreadPriorityHigh);

        /** ֹͣ����
        @version NIIEngine 4.0.0
        */
        void stop();
        
        /** ���������(����)
        @version NIIEngien 4.0.0
        */
        Nid listen(const String & ip, Nui16 port, Nui16 maxLink = 64, Nui16 sfamily = AF_INET, 
            const String & certfile = "", const String & keyfile = "", const String & keypw = "");

        /** �����ͷ���
        @version NIIEngine 4.0.0
        */
        Address * connect(const String & host, Nui16 port, Nui16 sfamily = AF_INET, 
            const String & certfile = "", const String & keyfile = "", const String & keypw = "");

        /** �ر�����
        @version NIIEngine 4.0.0
        */
        void close(const Address & address);
        
        /** ����SSL����
        @version NIIEngine 4.0.0
        */
        void setSSL(const Address & address, bool set = true);

        /** �Ƿ�����SSL����
        @version NIIEngine 4.0.0
        */
        bool isSSLActive(const Address & address);

        /** ����
        @version NIIEngine 4.0.0
        */
        virtual void send(const Nui8 * data, NCount size, const Address & address, bool broadcast);

        /** ��ȡ����ʣ���С
        @version NIIEngine 4.0.0
        */
        Nui32 getOutputSize(const Address & address) const;

        /** ����
        @version NIIEngine 4.0.0
        */
        virtual SocketMessage * receive();

        /** ��ȡ��������
        @version NIIEngine 4.0.0
        */
        void getConnect(AddressList & remotelist) const;

        /** ����(�������)
        @version NIIEngine 4.0.0
        */
        void addMessage(SocketMessage * msg, bool head);

        /** ����������
        @version NIIEngine 4.0.0
        */
        void attach(Plugin * plugin);

        /** ����������
        @version NIIEngine 4.0.0
        */
        void detach(Plugin * plugin);
    protected:
        /** ����/�������ݣ������̵߳���
        @version NIIEngine 4.0.0
        */
        void update();
        
        /** ����Ϣ
        @version NIIEngine 4.0.0
        */
        void onMessage(SocketMessage * msg);

        /**
        @version NIIEngine 4.0.0
        */
        SocketMessage * createMessage(NCount size);

        /**
        @version NIIEngine 4.0.0
        */
        void onClientConnect(Address * address);

        /**
        @version NIIEngine 4.0.0
        */
        void onServerConnect(Address * address);

        /**
        @version NIIEngine 4.0.0
        */
        void onConnectFail(Address * address);

        /**
        @version NIIEngine 4.0.0
        */
        void onConnectClose(Address * address);
    protected:
        typedef list<SocketMessage *>::type MessageList;
        typedef list<Address *>::type AddressList;
    protected:
        SocketIOList mRemoteList;
        Plugins mPluginList;
        ThreadMutex mClientSocketMutex;
        ThreadMutex mClientConnectMutex;
        ThreadMutex mServerConnectMutex;
        ThreadMutex mConnectFailMutex;        
        ThreadMutex mConnectCloseMutex;
        ThreadMutex mInMutex;
        Thread * mUpdateThread;
        Thread * mConnectThread;
        AddressList mClientList;
        AddressList mServerList;
        AddressList mConnectFailList;
        AddressList mConnectClosetList;
        AddressList mPrcClientList;
        AddressList mPrcServerList;
        AddressList mPrcClientCloseList;
        AddressList mPrcClientFailList;

        NIIi mThreadPrcLevel;
        STNui32 mRunThreadCount;
        STbool mRun;

        ThreadMutex mSSLConnectMutex;
        ThreadMutex mSSLAddressMutex;
        AddressList mSSLAddressList;
        AddressList mSSLConnectList;
    };

    /** ��Ⱥ
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ClusterNetworkObj : public NetworkObj
    {
    public:
        ClusterNetworkObj();
        virtual ~ClusterNetworkObj();

        /// @copydetails ClusterNetworkObj::stop
        void stop();

        /// @copydetails ClusterNetworkObj::send
        void send(const Nui8 * data, NCount size, const Address & address, bool broadcast);

        /// @copydetails ClusterNetworkObj::receive
        SocketMessage * receive();

        /// @copydetails ClusterNetworkObj::close
        void close(Address * address);

        /// @copydetails ClusterNetworkObj::onClientConnect
        void onClientConnect(Address * address);

        /// @copydetails ClusterNetworkObj::onServerConnect
        void onServerConnect(Address * address);
        
        /// @copydetails ClusterNetworkObj::onConnectFail()
        void onConnectFail(Address * address);

        /// @copydetails ClusterNetworkObj::onConnectClose
        void onConnectClose(Address * address);
    protected:
        typedef vector<SocketIO *>::type ConnectList; 
        ConnectList mConnectList;
    };
}
}
#endif