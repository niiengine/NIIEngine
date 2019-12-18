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
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_PATTERN_H_
#define _NII_PATTERN_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���������������Э��ʽ�������
    @remark
        ���������Լ�������ʱ��Ĵ�С,ͬʱ����Ӱ������չ,�������ṩ����������,Ҫ��
        ������ľ���Ƕ��ʽ������ʵ��,�����������ʵ��������������д��鷽��
    @par
        ����������Ǻ���Ⱦ֡��Ե�,Ҳ����˵ÿ����һ֡����������Ÿ���һ��,����������
        Ч��Э����Ⱦ���ݺ��û�����,�������Ҫ��������һ�������Ĺ�����,�鿴NiiJob.h
    @note ����������г�Ա������Ӧ��������һ����ʱ״̬
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Pattern : public PatternAlloc
    {
        friend class Engine;
    public:
        typedef vector<PatternID>::type PatternType;
    public:
        virtual ~Pattern();

        /** ��ȡ����������Ĵ���Ч��(����)
        @remark ����Խ����Ч��Խ��
        @return [0-255]
        @version NIIEngine 3.0.0
        */
        virtual Nui8 getQuality() const;

        /** ��ȡ����������Ĵ����ٶ�(����)
        @remark ����Խ�����ٶ�Խ��
        @return [0-255]
        @version NIIEngine 3.0.0
        */
        virtual Nui8 getPerformance() const;
    protected:
        /** ��ʼ��������
        @remark ����������ܻ�ִ�в��ɹ�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual bool _init() = 0;

        /** ����������
        @remark ����������ܻ�ִ�в��ɹ�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual bool _end() = 0;
        
        /** �����������������ڲ���Ϣ
        @remark �������������Ⱦÿ֡ǰ���õ�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _update() = 0;

        /** �����Ӵ�����
        @remark ��������N in 1Ч��(Ĭ��ȫ����)
        @par �ڲ��й涨
        @return �Ƿ������ɹ�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual bool on(PatternID proc);

        /** �ر��Ӵ�����
        @remark �رղ���N in 1Ч��
        @par �ڲ��й涨
        @return �Ƿ�رճɹ�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual bool off(PatternID proc);

        /** ��ȡ�����������ID
        @remark Ψһ��,�Ͱ汾�޹�ϵ
        @par �ڲ��й涨
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual PatternID getPID() const;
    protected:
        PatternID mMark;                        ///< ��������
    };
}
#endif