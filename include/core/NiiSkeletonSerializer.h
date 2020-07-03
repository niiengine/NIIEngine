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