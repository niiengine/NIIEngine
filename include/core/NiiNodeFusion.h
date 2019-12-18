/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-9-17

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

#ifndef _NII_NODE_FUSION_H_
#define _NII_NODE_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** �ڵ���ʵ��
    @remark ���ò�ֵ�������LOD����ʹ������Ч�Ŀ�������Ч��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NodeFusion : public AnimationFusion
    {
    public:
        /** ƽ���ֵ����.
        @note ���LOD����һ��ʹ��
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_LINEAR,       ///< ���Բ�ֵ.��,��׼ȷ
            M_SPLINE        ///< ���۲�ֵ,׼ȷ,������.
        };

        /** �ռ��ֵ������.
        @note ���LOD����һ��ʹ��
        @version NIIEngine 3.0.0
        */
        enum RotationMode
        {
            RM_LINEAR,      ///< ���Բ�ֵ.��,��׼ȷ.
            RM_SPHERICAL    ///< ���β�ֵ,׼ȷ,������.
        };
    public:
        NodeFusion(AnimationObj * obj, Animation * ani,  TimeDurMS pos, TimeDurMS dur, 
            NIIf scale = 1.0, NIIf weight = 1.0, bool enable = false);

        NodeFusion(AnimationFusionObj * own, const NodeFusion & o);

        ~NodeFusion();

        /** ִ��Ӧ��
        @param[in] factor ���Ӱ��Ȩ��(0.0 - 1.0]
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void setScale(NIIf scale);

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        NIIf getScale() const;
        
        /** ���ò�ֵģʽ.
        @param[in] m
        @version NIIEngine 3.0.0
        */
        void setMode(Mode m);

        /** ��ȡ��ֵģʽ.
        @version NIIEngine 3.0.0
        */
        Mode getMode() const;

        /** ������ת��ֵģʽ.
        @param[in] m
        @version NIIEngine 3.0.0
        */
        void setRotationMode(RotationMode m);

        /** ��ȡ��ת��ֵģʽ.
        @version NIIEngine 3.0.0
        */
        RotationMode getRotationMode() const;

        /** ���ýڵ�Ȩ��
        @param[in] boneidx ��ͷ����
        @param[in] weight Ȩ��
        @version NIIEngine 3.0.0
        */
        void setFactor(Nidx boneidx, NIIf weight);

        /** ��ȡ�ڵ�Ȩ��
        @param[in] boneidx ��Ҫ����
        @version NIIEngine 3.0.0
        */
        NIIf getFactor(Nidx boneidx) const;

        /** ���û��Ȩ��
        @remark �ж��ٽڵ�,���ж�������,
        @param[in] in Ȩ������
        @note ������С����,inָ�����ݽ���ֱ������(�ڴ潫�����NodeFusion�������)
        @version NIIEngine 3.0.0
        */
        void setFactor(NIIf * in);

        /** ��ȡ���Ȩ��
        @remark �ж��ٽڵ�,���ж�������
        @param[in] out Ȩ������
        @note ��ֱ����������,�κεĸı䶼ֱ��Ӱ����
        @version NIIEngine 3.0.0
        */
        void getFactor(NIIf *& out) const;
    public:
        /// @copydetails NodeFusion::reset
        void reset();

        /// @copydetails NodeFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        SkeletonFusion * mDst;  ///< �������ʵ��
        NIIf mScale;            ///< ��������
        NIIf * mBlendMask;      ///< �ڵ��Ȩ��
        Mode mMode;
        RotationMode mRotationMode;

    };
}
#endif