/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-3-7

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

#ifndef _NII_ANIMATION_MANAGER_H__
#define _NII_ANIMATION_MANAGER_H__

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** ����������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationManager : public Singleton<AnimationManager>, public GeometryAlloc
    {
    public:
        AnimationManager();
        ~AnimationManager();

        /** ����ʱ�����
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

		/** ��ȡ���һ֡ʱ�����ֵ
		@version NIIEngine 3.0.0
		*/
		DataValue<TimeDurMS> * getLastTimeValue() const;

		/** ��ȡ�򵥵�ʱ����ƺ���.
		@version NIIEngine 3.0.0
		*/
		DataFunc<TimeDurMS, TimeDurMS> * getNormalTimeFunc() const;

        /** ��������(ȫ��)
        @note AnimationObj �кܿ����Ѿ����ڶ���, ��������ǿ��ƶ�̬���Ӷ���
        @version NIIEngine 3.0.0 �߼�api
        */
        Animation * createAnimation(AnimationObj * obj, AnimationID id, TimeDurMS time, AnimationMode mode);

        /** ��ȡ����(ȫ��)
        @note AnimationObj �кܿ����Ѿ����ڶ���, ��������ǿ��ƶ�̬���Ӷ���
        @version NIIEngine 3.0.0 �߼�api
        */
        Animation * getAnimation(AnimationObj * obj, AnimationID id) const;

        /** ��ȡ����(ȫ��)
        @note AnimationObj �кܿ����Ѿ����ڶ���, ��������ǿ��ƶ�̬���Ӷ���
        @version NIIEngine 3.0.0 �߼�api
        */
        Animation * getAnimation(AnimationObj * obj, Nidx index) const;

        /** �Ƿ���ڶ���(ȫ��)
        @note AnimationObj �кܿ����Ѿ����ڶ���, ��������ǿ��ƶ�̬���Ӷ���
        @version NIIEngine 3.0.0 �߼�api
        */
        bool isAnimationExist(AnimationObj * obj, AnimationID id) const;

        /** ɾ������(ȫ��)
        @note AnimationObj �кܿ����Ѿ����ڶ���, ��������ǿ��ƶ�̬���Ӷ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void destroyAnimation(AnimationObj * obj, AnimationID id);

        /** ɾ�����ж���(ȫ��)
        @note AnimationObj �кܿ����Ѿ����ڶ���, ��������ǿ��ƶ�̬���Ӷ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void destroyAllAnimations();

        /** ��ȡ���ж���
        @note AnimationObj �кܿ����Ѿ����ڶ���, ��������ǿ��ƶ�̬���Ӷ���
        @version NIIEngine 3.0.0 �߼�api
        */
        NCount getCount(AnimationObj * obj) const;

        /** ������
        @note ֻ�аѻ��ʵ�����뵽����Ż�����Զ������ڲ�ʱ��
        @version NIIEngine 3.0.0
        */
        void addFusion(AnimationFusion * fusion);

        /** ɾ�����
        @note ֻ�аѻ��ʵ�����뵽����Ż�����Զ������ڲ�ʱ��
        @version NIIEngine 3.0.0
        */
        void removeFusion(AnimationFusion * fusion);

		/** ɾ�����
		@note ֻ�аѻ��ʵ�����뵽����Ż�����Զ������ڲ�ʱ��
		@version NIIEngine 3.0.0
		*/
		void removeAllFusion(AnimationFusionObj * obj);

        /** ɾ�����л��
        @note ֻ�аѻ��ʵ�����뵽����Ż�����Զ������ڲ�ʱ��
        @version NIIEngine 3.0.0
        */
        void removeAllFusion();

        /** ��ȡ�������
        @version NIIEngine 3.0.0
        */
        NCount getFusionCount(AnimationFusionObj * obj) const;

		/** ����ʱ�������������ٶ�ʱ��.
		@version NIIEngine 3.0.0
		*/
		void setTimeFactor(NIIf f);

		/** ����ʱ�������������ٶ�ʱ��.
		@version NIIEngine 3.0.0
		*/
		NIIf getTimeFactor() const;

		/** ����һ��������֡����.
		@version NIIEngine 3.0.0
		*/
		void setFixFrame(TimeDurMS f);

		/** ��ȡÿ֡�����ŵ�ʱ�䳣��.
		@version NIIEngine 3.0.0
		*/
		TimeDurMS getFixFrame() const;

		/** �������õ�ʱ��.
		@version NIIEngine 3.0.0
		*/
		void setCostTime(TimeDurMS cost);

		/** �������õ�ʱ��.
		@version NIIEngine 3.0.0
		*/
		TimeDurMS getCostTime() const;

        /** ������ԴȺ��
        @version NIIEngine 3.0.0
        */
        void setGroup(GroupID gid);

        /** ��ȡ��ԴȺ��
        @version NIIEngine 3.0.0
        */
        GroupID getGroup();

        /** ������ЧID
        @version NIIEngine 3.0.0
        */
        Nid genValidID();
    private:
        typedef vector<AnimationID>::type AnimationList;
        typedef map<AnimationObj *, AnimationList>::type AniObjList;
        typedef multimap<AnimationObjID, AnimationFusion *>::type AniFusionList;
		typedef map<AnimationFusionObj *, AniFusionList>::type ObjFusionList;
	private:
        GroupID mGroupID;
        Nid mValidID;
        N_mutex(mValidIDMutex)
        N_mutable_mutex(mAnimationsMutex)
        AniObjList mAniObjList;
		ObjFusionList mObjFusionList;
		DataValue<TimeDurMS> * mFrameTimeValue;				
		DataFunc<TimeDurMS, TimeDurMS> * mNormalTimeFunc;
		Nul mLastFrameNumber;
    };
}
#endif