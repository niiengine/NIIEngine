/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-8

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