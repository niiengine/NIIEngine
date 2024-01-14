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
    /** 节点专用关键帧
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NodeKeyFrame : public KeyFrame
    {
    public:
        NodeKeyFrame(const KeyFrameTrack * own, TimeDurMS pos);
        ~NodeKeyFrame();

        /** 设置关联这个关键针的平移
        @remark 平移因子,影响关键帧它的动画对象在它的时间索引中的平移(移动)数量
        @param[in] trans 通过该向量平移
        @version NIIEngine 3.0.0
        */
        void setTranslate(const Vector3f & trans);

        /** 获取这个关键针应用的平移
        @version NIIEngine 3.0.0
        */
        const Vector3f & getTranslate() const;

        /** 设置这个关键针应用到动画对象在它的时间索引中的缩放因子
        @param[in] scale
            通过该向量缩放(任何成分为0的值,它将缩放对象到0尺寸)
        @version NIIEngine 3.0.0
        */
        void setScale(const Vector3f & scale);

        /** 获取这个关键针应用的缩放因子
        @version NIIEngine 3.0.0
        */
        const Vector3f & getScale() const;

        /** 设置这个关键针应用的旋转
        @param[in] rot
            应用的旋转;如果你不喜欢直接使用四元数的话,使用元数的方法从angle/axis
            或Matrix3中转换.
        @version NIIEngine 3.0.0
        */
        void setRotation(const Quaternion & rot);

        /** 获取这个关键针应用的旋转
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