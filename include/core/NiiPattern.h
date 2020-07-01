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