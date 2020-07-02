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