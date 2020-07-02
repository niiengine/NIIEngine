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