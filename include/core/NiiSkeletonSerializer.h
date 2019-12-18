/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_SkeletonSerializer_H_
#define _NII_SkeletonSerializer_H_

#include "NiiPreInclude.h"
#include "NiiSkeleton.h"
#include "NiiFileSerializer.h"

namespace NII
{
    /** 读取骨骼成分文件
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SkeletonSerializer : public FileSerializer
    {
    public:
        SkeletonSerializer(const DataStream * src);
        SkeletonSerializer(const FileSerializer & obj);
        virtual ~SkeletonSerializer();

        /** 导出骨骼到文件
        @version NIIEngine 3.0.0
        */
        void wirte(const Skeleton * in, const String & file, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);

        /** 导出骨骼到流
        @version NIIEngine 3.0.0
        */
        void wirte(const Skeleton * in, DataStream * dest, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);

        /** 导入流到骨骼
        @version NIIEngine 3.0.0
        */
        void read(const String & file, Skeleton * out);
        
        /** 导入流到骨骼
        @version NIIEngine 3.0.0
        */
        void read(DataStream * src, Skeleton * out);
        
        using Serializer::read;

        /// @copydetails FileSerializer::isVersion
        bool isVersion(SerialVer ver) const;
        
        /// @copydetails FileSerializer::getUnit
        SerializerID getUnit() const;
        
        /// @copydetails FileSerializer::getGroup
        GroupID getGroup() const;
    protected:
        /// @copydetails FileSerializer::read
        void readUnit(void * out);

        /// @copydetails FileSerializer::write
        void writeUnit(const void * in, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);

        void writeSkeleton(const Skeleton * in);
        void writeBone(const Bone * bone);
        void writeAnimation(const Animation * anim);
        void writeKeyFrameTrack(const NodeKeyFrameTrack * track);
        void writeKeyFrame(const NodeKeyFrame * key);
        void writeSkeletonExt(const Skeleton * in, const Skeleton::Ext & link);

        void readSkeleton(Skeleton * out);
        void readBone(Skeleton * out);
        void readAnimation(Skeleton * out);
        void readKeyFrameTrack(Skeleton * out, Animation * anim);
        void readKeyFrame(Skeleton * out, NodeKeyFrameTrack * track);
        void readSkeletonExt(Skeleton * out);

        NCount getBoneSize(const Bone * obj);
        NCount getAnimationSize(const Animation * obj);
        NCount getKeyFrameTrackSize(const NodeKeyFrameTrack * obj);
        NCount getKeyFrameSize(const NodeKeyFrame * obj);
        NCount getSkeletonExtSize(const Skeleton::Ext & obj);
    protected:
        typedef map<Nui32, Bone *>::type BoneParentList;
        BoneParentList mBoneParentList;
    };
}
#endif