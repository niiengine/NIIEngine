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
    class _EngineAPI NetworkObj : public ThreadMain
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
        bool start(Nui16 maxLink, NIIi prclevel = N_ThreadPriorityHigh);

        /** ֹͣ����
        @version NIIEngine 4.0.0
        */
        void stop();

        /** ���������(����)
        @version NIIEngien 4.0.0
        */
        Nid listen(const String & ip, Nui16 port, Nui16 sfamily = AF_INET, Nui16 maxConnect = 128);

        /** �����ͷ���
        @version NIIEngine 4.0.0
        */
        Address * connect(const String & host, Nui16 port, Nui16 sfamily = AF_INET);

        /** �ر�����
        @version NIIEngine 4.0.0
        */
        void close(const Address & address);
        
        /** ����SSL����
        @version NIIEngine 4.0.0
        */
        void setSSL(const Address & address, const String & certfile = "", const String & keyfile = "", const String & keypw = "");

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
        /// @copydetails ThreadMain::run
        virtual void run(void * arg);

        /** ������Ϣ��
        @version NIIEngine 4.0.0
        */
        virtual SocketMessage * createMessage(NCount size);

        /** �����յ���Ϣʱ����
        @version NIIEngine 4.0.0
        */
        virtual void onMessage(SocketMessage * msg) = 0;
        
        /** ���Կͷ�������ʱ
        @version NIIEngine 4.0.0
        */
        virtual void onClientConnect(Address * address);

        /** ���Է��������ʱ
        @version NIIEngine 4.0.0
        */
        virtual void onServerConnect(Address * address);

        /** ����ʧ��ʱ
        @version NIIEngine 4.0.0
        */
        virtual void onConnectFail(Address * address);

        /** ���ӹر�ʱʱ
        @version NIIEngine 4.0.0
        */
        virtual void onConnectClose(Address * address);
    protected:
        typedef list<SocketMessage *>::type MessageList;
        typedef list<Address *>::type AddressList;
        
        typedef struct _SSLData
        {
            Address * mAddres;
            String mCertFile;
            String mKeyFile;
            String mKeyPW;
        }SSLData;

        typedef list<SSLData>::type SSLAddressList;
    protected:
        SocketPrcList mListenList;
        SocketIOList mRemoteList;
        MessageList mInList;
        Plugins mPluginList;
        ThreadMutex mRemoteListMutex;
        ThreadMutex mClientConnectMutex;
        ThreadMutex mServerConnectMutex;
        ThreadMutex mConnectFailMutex;        
        ThreadMutex mConnectCloseMutex;
        ThreadMutex mInMutex;
        Thread * mUpdateThread;
        AddressList mClientList;
        AddressList mServerList;
        AddressList mConnectFailList;
        AddressList mConnectClosetList;

        NIIi mThreadPrcLevel;
        STNui32 mRunThreadCount;
        STbool mRun;

        ThreadRWMutex mSSLConnectMutex;
        ThreadRWMutex mSSLAddressMutex;
        SSLAddressList mSSLAddressList;
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

        /// @copydetails ClusterNetworkObj::close
        void close(Address * address);
    protected:
        /// @copydetails ThreadMain::run
        void run(void * arg);
        
        /// @copydetails NetworkObj::onClientConnect
        void onClientConnect(Address * address);

        /// @copydetails NetworkObj::onServerConnect
        void onServerConnect(Address * address);
        
        /// @copydetails NetworkObj::onConnectFail()
        void onConnectFail(Address * address);

        /// @copydetails NetworkObj::onConnectClose
        void onConnectClose(Address * address);
    protected:
        AddressList mPrcClientList;
        AddressList mPrcServerList;
        AddressList mPrcClientCloseList;
        AddressList mPrcClientFailList;
    };
}
}
#endif