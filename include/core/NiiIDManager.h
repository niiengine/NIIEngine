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

#ifndef _NII_ID_MANAGER_H_
#define _NII_ID_MANAGER_H_

#include "NiiPreInclude.h"

#include "NiiString.h"
namespace NII
{
    /** ��¼ĳһ����ĳ�Ա����������������
    @remarks
        ����֪��һ����ĳ�Ա�ľ���ǿ��Դ�1��ʼ��,��������Ҫʹ����������һ������ʱ,
        �����ַ�����ʾ�������ĺ���,���ڳ������ַ����������Ƕ����,�����������Ҫ
        ֪������ַ������庬����ֻ��Ҫ֪���������Ҫ������һ������,�ڵ�����ʱ�����
        ����Ҫ֪������������Ϣ,����������ʱ����ע�ص���Ч��,�����һ�������Manager
        �����FactoryObj�����ٻ��ߺ��и��������һ��ʹ��,ǰ����һ���е���ʵ������
        (��ʹ������),�����й�������.������ֻ�оֲ���������,���ID����������������
        �Ǻ��ж������������,Ҳ����˵������ͬ���ʵĸ�����
    @par
        �����Ƕ����ڶ�������,������Ӧ�ó���,���Բ���Ω��ʵ��
    */
    class _EngineAPI IDManager : public ManagerAlloc
    {
    public:
        IDManager(const String & autoname);
        virtual ~IDManager();

        /** ����һ��ʵ������
        @param[in] nane �������
        @return һ��ʵ������,ӵ��һ��Ω��ID
        */
        void * generate(const String & name);

        /** ����һ��ʵ������
        @return һ��ʵ������,ӵ��һ��Ω��ID
        */
        void * generate();

        /** ���һ���Ѿ����ڵĶ���
        @remark
            ������ƻ��Զ����������������һ��ID,Ҳ����˵���ԭ����������Ѿ��и�
            ID,����������������ID���ʵ�ӳ�佫�ᵼ�²���Ԥ�ϵĽ��,��������ӵ���
            ��������ǰ��Ҫ���������������IDӳ��
        @return һ��ʵ������,ӵ��һ��Ω��ID
        */
        void add(const void * target);

        /// ��ȥָ��ID����
        void remove(ObjID id);

        /// ��ȥָ���Ķ���ʵ��
        void remove(void * target);

        /** ������ж���
        @remark ���������������ʱ��ŵ����������,��Ȼ�ᵼ�²�����Ԥ�ϵĽ��
        */
        void clear();
    protected:
        /** ����һ�������ֵĶ���
        @remark ͨ��������ȥ����
        @param[in] name
        */
        virtual void * create(const String & name) = 0;

        /** ����һ������
        @remark ͨ��������ȥ����
        */
        virtual void * create() = 0;

        /** ɾ��һ������
        @remark ͨ��������ȥ����
        */
        virtual void destroy(void * target) = 0;

        /** ��������һ������
        @remark ͨ������������ȥ����
        @param[in] target ��Ҫ��ID�Ķ���
        @param[in] id ��ID
        */
        virtual void reset(const void * target, ObjID id) = 0;
    public:
        typedef map<Nid, Nvoid>::type PoolList;
        typedef map<String, Nid>::type AliasPoolList;
        typedef vector<Nid>::type RemainList;
    protected:
        N_mutex1
        ObjID mCurrent;
        PoolList mList;
        AliasPoolList mAList;
        RemainList mRList;
    };
}
#endif