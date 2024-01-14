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

#ifndef _NII_MEMBER_FUNCTOR_H_
#define _NII_MEMBER_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiFunctor.h"
#include "NiiCondition.h"

namespace NII
{
namespace NII_COMMAND
{
    /** ��Ա��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MemberFunctor : public Functor
    {
    public:
        typedef vector<Condition::ConditionParam *>::type DestList;
    public:
        MemberFunctor(EventObj * own);
        virtual ~MemberFunctor();

        /** ���һ������
        @remark ������Ӻ����������ͬ����ƥ���
        @param[in] factor
        @version NIIEngine 3.0.0
        */
        void add(Condition::ConditionParam * factor);

        /** ��ȥһ������
        @remark ������Ӻ����������ͬ����ƥ���
        @param[in] factor
        @version NIIEngine 3.0.0
        */
        void remove(Condition::ConditionParam * factor);

        /** �����Ƿ����������������
        @remark ����������Ļ�,����������ӵ�״̬
        @version NIIEngine 3.0.0
        */
        void setEnable(bool s);
        
        /** �����Ƿ����������������
        @remark ����������Ļ�,����������ӵ�״̬
        @version NIIEngine 3.0.0
        */
        bool getEnable() const;
        
        /** ������������Ƿ���Ч
        @remark �����������û���κ��������ӵ�ʱ��,�ͻ���Ч
        @return ����һ������,��ʶ�Ƿ���Ч
        @version NIIEngine 3.0.0
        */
        bool isInvalid() const;
        
        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);
    protected:
        /** �����������Ƚ�,�����������ı�����Ա����,����ı��������Ӵ��״̬
        @remark ����mid����Ӧ�ĺ�������Ͳ���ex�����
        @param[in] mid �����������ID
        @param[in] cp �Ƚ�����
        @param[in] ex ��Ҫȥ�Ƚϵ���ֵ
        @return һ������,�Ƚϳ����򷵻�true,���򷵻�false
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual bool process(CmpMode cp, const EventArgs * args) = 0;

        /** Ч��������Ԫ�Ƿ����,�����������ı�Ŀ��ĳ�Ա����
        @remark ����mid����Ӧ�ĺ�������Ͳ���arg�����
        @param[in] mid �������Ĵ�����
        @param[in] arg ���䵽�������Ĳ���
        @return һ������,��ʾ�ڲ�����������״̬�Ƿ��иı�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual bool check(const EventArgs * arg) = 0;
    protected:
        EventObj * mOwn;        ///< �����Ŀ��������
        DestList mDestList;        ///< 
        bool mEnable;            ///< 
    };
}
}
#endif