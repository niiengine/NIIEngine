/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-9-1

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

#ifndef _NII_NODE_KEYFRAME_H_
#define _NII_NODE_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiQuaternion.h"
#include "NiiVector3.h"

namespace NII
{
    /** �ڵ�ר�ùؼ�֡
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NodeKeyFrame : public KeyFrame
    {
    public:
        NodeKeyFrame(const KeyFrameTrack * own, TimeDurMS pos);
        ~NodeKeyFrame();

        /** ���ù�������ؼ����ƽ��
        @remark ƽ������,Ӱ��ؼ�֡���Ķ�������������ʱ�������е�ƽ��(�ƶ�)����
        @param[in] trans ͨ��������ƽ��
        @version NIIEngine 3.0.0
        */
        void setTranslate(const Vector3f & trans);

        /** ��ȡ����ؼ���Ӧ�õ�ƽ��
        @version NIIEngine 3.0.0
        */
        const Vector3f & getTranslate() const;

        /** ��������ؼ���Ӧ�õ���������������ʱ�������е���������
        @param[in] scale
            ͨ������������(�κγɷ�Ϊ0��ֵ,�������Ŷ���0�ߴ�)
        @version NIIEngine 3.0.0
        */
        void setScale(const Vector3f & scale);

        /** ��ȡ����ؼ���Ӧ�õ���������
        @version NIIEngine 3.0.0
        */
        const Vector3f & getScale() const;

        /** ��������ؼ���Ӧ�õ���ת
        @param[in] rot
            Ӧ�õ���ת;����㲻ϲ��ֱ��ʹ����Ԫ���Ļ�,ʹ��Ԫ���ķ�����angle/axis
            ��Matrix3��ת��.
        @version NIIEngine 3.0.0
        */
        void setRotation(const Quaternion & rot);

        /** ��ȡ����ؼ���Ӧ�õ���ת
        @version NIIEngine 3.0.0
        */
        const Quaternion & getRotation() const;
    protected:
        KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        Vector3f mTranslate;
        Vector3f mScale;
        Quaternion mRotate;
    };
}
#endif