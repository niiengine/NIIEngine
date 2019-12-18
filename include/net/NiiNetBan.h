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

#ifndef _NII_NET_BAN_H_
#define _NII_NET_BAN_H_

#include "NiiNetPreInclude.h"

namespace NII
{
namespace NII_NET
{
    class BanStruct;
    
	/** ��������
	@version NIIEngine 3.0.0
	*/
    class _EngineAPI Ban : public NetAlloc
    {
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
        vector<BanStruct *>::type Bans;
	private:
        SimpleMutex banListMutex;
        Bans mBans;
    };
}
}
#endif