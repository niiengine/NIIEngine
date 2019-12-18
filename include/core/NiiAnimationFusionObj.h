/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-15

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

#ifndef _NII_ANIMATION_FUSION_OBJ_H_
#define _NII_ANIMATION_FUSION_OBJ_H_

#include "NiiPreInclude.h"

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
        Nmark getFrameMark() const;

        /** ��ȡ���еĶ���ʵ�ʻ��(ʵ��)
        @version NIIEngine 3.0.0
        */
        AnimationFusions & getFusions(Nid id = 0) const;

        /** ��ȡ��Ч�Ķ���ʵ�ʻ��(ʵ��)
        @version NIIEngine 3.0.0
        */
        const FusionList & getValidFusions() const;

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
}
#endif