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

#ifndef _NII_ANIMATION_OBJ_H_
#define _NII_ANIMATION_OBJ_H_

#include "NiiPreInclude.h"
#include "NiiAnimation.h"

namespace NII
{
    /** �ɶ�������
    @remark �����������ʵ�ʻ��(ʵ��)
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationFusionObj : public AnimableAlloc
    {
        friend class AnimationManager;
    public:
        typedef map<AnimationID, AnimationFusion *>::type AnimationFusions;
        typedef map<Nid, AnimationFusions>::type ObjFusions;
        typedef vector<AnimationFusion *>::type FusionList;
    public:
        AnimationFusionObj();
        AnimationFusionObj(const AnimationFusionObj & o);
        ~AnimationFusionObj();

        /** �Ƿ���ڶ������
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** ��Ӷ������
        @note �ڴ潻�ɱ������
        @param[in] subid
        @param[in] enable �Ƿ����ö���
        @version NIIEngine 3.0.0
        */
        void addFusion(AnimationFusion * fusion, Nid subid = 0, bool enable = false);

        /** ��ȡ�������
        @param[in] aid
        @param[in] subid
        @version NIIEngine 3.0.0
        */
        AnimationFusion * getFusion(AnimationID aid, Nid subid = 0) const;

        /** �Ƿ���ڶ������
        @param[in] aid
        @param[in] id
        @version NIIEngine 3.0.0
        */
        bool isFusionExist(AnimationID aid, Nid id = 0) const;

        /** ��ȥ����״̬
        @param[in] aid
        @param[in] id
        @version NIIEngine 3.0.0
        */
        void removeFusion(AnimationID aid, Nid id = 0);

        /** ��ȥ���ж���״̬
        @version NIIEngine 3.0.0
        */
        void removeAllFusion(Nid id);

        /** ��ȥ���ж���״̬
        @version NIIEngine 3.0.0
        */
        void removeAllFusion();

        /** ֪ͨ�������ñ仯
        @version NIIEngine 3.0.0 �߼�api
        */
        void _notifyDisable(AnimationObjID aid);

        /** ֪ͨ�������ñ仯
        @version NIIEngine 3.0.0 �߼�api
        */
        void _notifyEnable(AnimationFusion * obj);

        /** ֪ͨ���ݻ�ǰ֡�仯
        @version NIIEngine 3.0.0 �߼�api
        */
        void _notifyFrame();

        /** ��õ�ǰ֡��
        @version NIIEngine 3.0.0 �߼�api
        */
        Nmark getFrameMark() const { return mFrameMark; }

        /** ��ȡ���еĶ���ʵ�ʻ��(ʵ��)
        @version NIIEngine 3.0.0
        */
        AnimationFusions & getFusions(Nid id = 0) const;

        /** ��ȡ��Ч�Ķ���ʵ�ʻ��(ʵ��)
        @version NIIEngine 3.0.0
        */
        const FusionList & getValidFusions() const { return mValidFusions; }

        /** ����һ�������и���״̬
        @note ����ʹ��ͬһ������
        @param[in] src ������Դ
        @version NIIEngine 3.0.0
        */
        void copyState(const AnimationFusionObj * src);
    protected:
        /** ֪ͨ�������ж���
        @version NIIEngine 3.0.0 �߼�api
        */
        void _notifyDisable();
    private:
        static AnimationFusions Empty;
    protected:
        N_mutex1
        mutable ObjFusions mFusions;
        FusionList mValidFusions;
        Nmark mFrameMark;
    };

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