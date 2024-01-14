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

#ifndef _NII_NET_SECURE_H_
#define _NII_NET_SECURE_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_NET
{
	/** ��������
	@version NIIEngine 3.0.0
	*/
    class _EngineAPI Ban : public NetAlloc
    {
        class Data;
    public:
        Ban();
        ~Ban();
        
        /** ���
        @version NIIEngine 3.0.0
        */
        void add(const String & ip, TimeDurMS time = 0);
        
        /** ��ȥ
        @version NIIEngine 3.0.0
        */
        void remove(const String & ip);
        
        /** �Ƿ����
        @version NIIEngine 3.0.0
        */
        bool isExist(const String & ip);
        
        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void removeAll();
    private:
        vector<Data *>::type Bans;
	private:
        ThreadMutex mMutex;
        Bans mBans;
    };
    
    /** ���Ӱ�ȫ
	@note ��ʾ�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Secure : public NetAlloc
    {
        class Data;
    public:
        Secure();
        ~Secure();

        /** �����Ƿ�������ȫ
        @version NIIEngine 3.0.0
        */
        void setSecure(bool b);

        /** ��ȡ�Ƿ�������ȫ
        @version NIIEngine 3.0.0
        */
        bool isSecure() const;

        /** ��ʼ��
        @param[in] publicKey
        @param[in] privateKey
        @param[in] clientKey
        @version NIIEngine 3.0.0
        */
        bool init(const Nui8 * publicKey, const Nui8 * privateKey, bool clientKey = false);
        
        /** ���
        @version NIIEngine 3.0.0
        */
        void add(const String & ip);
        
        /** ɾ��
        @version NIIEngine 3.0.0
        */
        void remove(const String & ip);
        
        /** �Ƿ����
        @version NIIEngine 3.0.0
        */
        bool isExist(const String & ip);
        
        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void removeAll();
    private:
        typedef list<String>::type IPList;
    private:
        Data * mData;
        ThreadRWMutex mMutex;
        IPList mIPList;
    };
}
}
#endif