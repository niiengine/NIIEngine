/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-6-17

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

#ifndef _NII_PropertyFusion_H_
#define _NII_PropertyFusion_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"
#include "NiiFusionData.h"

namespace NII
{
    /** �������Զ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyFusion : public AnimationFusion
    {
    public:
        PropertyFusion(PropertyCmdObj * dst, AnimationObj * obj, Animation * ani, 
            NIIf weight = 1.0, bool enable = false);

        PropertyFusion(PropertyCmdObj * dst, AnimationObj * obj,
            Animation * ani, TimeDurMS pos, TimeDurMS dur, NIIf weight = 1.0, bool enable = false);

        PropertyFusion(AnimationFusionObj * own, const PropertyFusion & o);
        
        ~PropertyFusion();

        /** Ӧ��
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0);

        /** �������Ե�ǰֵ
        @remark ���Ե�ֵ�����ڶ�����
        @version NIIEngine 3.0.0
        */
        void setBaseValue(PropertyID pid);

        /** ��ȡ���Ա���ֵ
        @remark ���Ե�ֵ�����ڶ�����
        @version NIIEngine 3.0.0
        */
        const FusionData & getBaseValue(PropertyID pid);

        /** ����Ŀ�����
        @version NIIEngine 3.0.0
        */
        void setTarget(PropertyCmdObj * dst);

        /** ��ȡĿ�����
        @version NIIEngine 3.0.0
        */
        PropertyCmdObj * getTarget() const;
        
        /// @copydetails AnimationFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        /// @copydetails AnimationFusion::onStart
        void onStart();
    private:
        typedef map<PropertyID, FusionData>::type PropertyValueMap;

        PropertyCmdObj * mDst;
        PropertyValueMap mBaseValue;
    };
}
#endif