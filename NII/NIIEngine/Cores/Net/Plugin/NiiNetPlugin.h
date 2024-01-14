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
#ifndef _NII_NET_PLUGIN_H_
#define _NII_NET_PLUGIN_H_

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiPlugin.h"

namespace NII
{
namespace NII_NET
{
    class NetworkPrc;

    /** 连接失败类型
    @version NIIEngine 4.0.0
    */
    enum ConnectFailType
    {
        CFT_Fail,
        CFT_Ban,
        CFT_Recent,
        CFT_Duplicate,
        CFT_Max,
        CFT_Password,
        CFT_Encrypt,
        CFT_Proto
    };
    
    /** 连接接收类型
    @version NIIEngine 4.0.0
    */
    enum ConnectReceiveType
    {
        CRT_Normal,
        CRT_Inner,
        CRT_Destroy
    };
    
    /** 连接关闭类型
    @version NIIEngine 4.0.0
    */
    enum ConnectCloseType
    {
        CCT_Close,
        CCT_Shutdown,
        CCT_Exception
    };

    /** 处理型插件
    @remark
        其机制很像NII::Pattern,但NII::Pattern是相对与图形帧改变而更新的,而它依赖的是具体连
        接网络的消息触发更新的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Plugin : public NII::Plugin
    {
    public:
        Plugin();
        virtual ~Plugin();

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onConnect(const UniqueAddress & id){}

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onConfirm(const UniqueAddress & id);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onConnectClose(const UniqueAddress &, ConnectCloseType type){}

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onConnectFail(SocketMessage *, ConnectFailType){}

        /**
        @version NIIEngine 3.0.0
        */
        virtual ConnectReceiveType onMessage(SocketMessage *);

        /**
        @version NIIEngine 3.0.0
        */
        inline void setPrc(NetworkPrc * prc) { mPrc = prc; }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline LinkBase * getPrc() const {return mPrc; }

        /**
        @version NIIEngine 3.0.0
        */
        const UniqueID & getID() const;
    protected:
        /**
        @version NIIEngine 3.0.0
        */
        virtual void onAttach();

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onDetach();
    
        /**
        @version NIIEngine 3.0.0
        */
        void addMessage(SocketMessage * msg, bool head);

        /**
        @version NIIEngine 3.0.0
        */
        void send(const Nui8 * data, NCount size, PacketLevel plevel, PacketType ptype, PacketCH pch, const UniqueAddress & addr, bool broadcast);

        /**
        @version NIIEngine 3.0.0
        */
        inline void send(const NetSerializer * out, PacketLevel plevel, PacketType ptype, PacketCH pch, const UniqueAddress & addr, bool broadcast)
        {
            send(out->getData, out->getSize(), plevel, ptype, pch, addr, broadcast);
        }

        /**
        @version NIIEngine 3.0.0
        */
        SocketMessage * createMessage(NCount dataSize);
    protected:
        NetworkPrc * mPrc;
    };
    vector<Plugin *>::type PluginList;
}
}
#endif