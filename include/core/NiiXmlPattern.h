/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

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

#ifndef _NII_XML_SYS_H_
#define _NII_XML_SYS_H_

#include "NiiPreInclude.h"
#include "NiiPattern.h"

namespace NII
{
    /** XML������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI XmlPattern : public Pattern
    {
    public:
        XmlPattern();
        virtual ~XmlPattern();

        /** ��ʼ��
        @version NIIEngine 3.0.0
        */
        bool startup();

        /** �ر�
        @version NIIEngine 3.0.0
        */
        void shutdown();

        /** ����
        @version NIIEngine 3.0.0
        */
        virtual void parse(XmlAnalyze * prc, const DataStream * data) = 0;
    protected:
        /// @copydetails Pattern::_update
        void _update();
        
        /// @copydetails Pattern::getPID
        PatternID getPID() const;
    private:
        PatternID mPatternID;
        bool mInit;
    };
}
#endif