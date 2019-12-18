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
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
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
        /// ����ID������ʵ��ָ���ӳ��
        typedef map<Nid, NIIp>::type PoolList;
        /// ��������������ID��ӳ��
        typedef map<String, Nid>::type AliasPoolList;
        /// ����ID��
        typedef vector<Nid>::type RemainList;
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
    protected:
        N_mutex1                ///< ��
        ObjID mCurrent;         ///< ��ǰ����
        PoolList mList;         ///< �����б�,������ڸ������ʹ��
        AliasPoolList mAList;   ///< �����б�,
        RemainList mRemain;     ///< �Ѿ����յ�ID��
    };
}
#endif