/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-11-3

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

#ifndef _NII_ANIMATION_OBJ_H_
#define _NII_ANIMATION_OBJ_H_

#include "NiiPreInclude.h"
#include "NiiAnimation.h"
#include "NiiAnimationFusionObj.h"

namespace NII
{
    /** ��������
    @remarks ���Ǹ���������,������һ�������嶯��,Ҳ����˵�������Ǹ�ʵ��ģ��,������ʵ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationObj
    {
    public:
        typedef map<AnimationID, Animation *>::type AnimationList;
    public:
        AnimationObj();
        AnimationObj(AnimationObjID aoid);
        virtual ~AnimationObj();

        /** ��ȡ��������ID    
        @version NIIEngine 3.0.0
        */
        AnimationObjID getAniObjID() const;

        /** ��������
        @param[in] id ����ID
        @param[in] dur ��������
        @version NIIEngine 3.0.0
        */
        virtual Animation * createAnimation(AnimationID id, TimeDurMS dur);

        /** ��ȡ����
        @param[in] index ����
        @version NIIEngine 3.0.0
        */
        virtual Animation * getAnimation(Nidx index) const;

        /** ��ȡ����
        @param[in] id ����ID
        @version NIIEngine 3.0.0
        */
        virtual Animation * getAnimation(AnimationID id) const;

        /** ��ȡ�������Ƴ�����
        @param[in] id ����ID
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual Animation * getAnimationPop(AnimationID id);

        /** �Ƿ���ڶ���
        @param[in] id ����ID
        @version NIIEngine 3.0.0
        */
        virtual bool isAnimationExist(AnimationID id) const;

        /** ��ȥ����
        @param[in] id ����ID
        @version NIIEngine 3.0.0
        */
        virtual void removeAnimation(AnimationID id);

        /** ��ȥ���ж���
        @version NIIEngine 3.0.0
        */
        virtual void removeAllAnimations();

        /** ��ȡ��������еĶ�������
        @version NIIEngine 3.0.0
        */
        virtual NCount getAnimationCount() const;

        /** �Ƿ���ڶ�����
        @remark ���԰���ϵͳ�ж��Ƿ�Ҫ����������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual bool isVertexFusion() const;

        /** �Ƿ���ڶ��㷨�߻��
        @remark ���԰���ϵͳ�ж��Ƿ�Ҫ����������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual bool isNormlFusion() const;

        /** �Ƿ���ڹ������
        @remark ���԰���ϵͳ�ж��Ƿ�Ҫ����������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual bool isSkeletonFusion() const;

        /** �Ż����ж���
        @version NIIEngine 3.0.0
        */
        virtual void optimise();

        /** ע�붯�����ݹ�ϵ
        @remark ������ʽ
        @param[in] obj ʵ������
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _init(AnimationFusionObj * obj);

        /** ���Ӷ������ݹ�ϵ
        @remark ���ӷ�ʽ
        @param[in] obj ʵ������
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _append(AnimationFusionObj * obj);

        /** ˢ�¶������ݹ�ϵ
        @remark ˢ�·�ʽ
        @param[in] obj ʵ������
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _refresh(AnimationFusionObj * obj);

        /** ˢ��һ��������ϵ
        @remark ���·�ʽ
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _update(AnimationFusionObj * obj);
    protected:
        static Nid mValidID;
        AnimationObjID mAniObjId;
        AnimationList mAnimations;  ///< �����б�
    };
}

#endif