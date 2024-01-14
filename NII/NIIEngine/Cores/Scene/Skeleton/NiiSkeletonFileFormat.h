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

#ifndef _NII_SkeletonFileFormat_H_
#define _NII_SkeletonFileFormat_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 定义 NII skeleton文件格式 
        skeleton 文件是二进制文件(在运行时读取效率高),还有布置成块数据,非常像3D Studio格式
        一个块总包含:
                Nui16   CHUNK_ID   : 下面的块IDList,识别块
                Nul   LENGTH        : 块以字节为单位的长度,包含这个头
                void*   DATA       : 数据,可能包含其他 子-块 (各种数据类型)
        
        一个.skeleton文件包含骨骼对象定义和他包含的动画.他只包含一个单一骨骼,不过可以包含多个动画
    */
    enum SkeletonChunkID 
    {
        SKELETON_HEADER            = 0x1000,
            // NIIb* version           : Version number check
            SKELETON_BLENDMODE           = 0x1010, // optional
                // Nui16 blendmode        : Skeleton::Mode            
        SKELETON_BONE              = 0x2000,
        // Repeating section defining each bone in the system. 
        // Bones are assigned indexes automatically based on their order of declaration
        // starting with 0.

            // NIIb* name                       : name of the bone
            // Nui16 handle            : handle of the bone, should be contiguous & start at 0
            // Vector3 position                 : position of this bone relative to parent 
            // Quaternion orientation           : orientation of this bone relative to parent 
            // Vector3 scale                    : scale of this bone relative to parent 

        SKELETON_BONE_PARENT       = 0x3000,
        // Record of the parent of a single bone, used to build the node tree
        // Repeating section, listed in Bone Index order, one per Bone

            // Nui16 handle             : child bone
            // Nui16 parentHandle   : parent bone

        SKELETON_ANIMATION         = 0x4000,
        // A single animation for this skeleton

            // NIIb* name                       : Name of the animation
            // NIIf length                      : Length of the animation in seconds
            
            SKELETON_ANIMATION_BASEINFO = 0x4010,
            // [Optional] base keyframe information
            // NIIb* baseAnimationName (blank for self)
            // NIIf baseKeyFrameTime
            
            SKELETON_ANIMATION_TRACK = 0x4100,
            // A single animation track (relates to a single bone)
            // Repeating section (within SKELETON_ANIMATION)
                
                // Nui16 boneIndex     : Index of bone to apply to

                SKELETON_ANIMATION_TRACK_KEYFRAME = 0x4110,
                // A single keyframe within the track
                // Repeating section

                    // NIIf time                    : The time position (seconds)
                    // Quaternion rotate            : Rotation to apply at this keyframe
                    // Vector3 translate            : Translation to apply at this keyframe
                    // Vector3 scale                : Scale to apply at this keyframe
        SKELETON_ANIMATION_LINK         = 0x5000
        // Link to another skeleton, to re-use its animations

            // NIIb* skeletonName                    : name of skeleton to get animations from
            // NIIf scale                            : scale to apply to trans/scale keys
    };
    /** @} */
    /** @} */
} // namespace
#endif