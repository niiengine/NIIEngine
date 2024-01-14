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

#include "NiiPreProcess.h"
#include "NiiSkeletonSerializer.h"
#include "NiiSkeletonFileFormat.h"
#include "NiiFileSerializerList.h"
#include "NiiDefaultStream.h"
#include "NiiNodeKeyFrameTrack.h"
#include "NiiNodeKeyFrame.h"
#include "NiiLogManager.h"
#include "NiiSkeleton.h"
#include "NiiAnimation.h"
#include "NiiString.h"

namespace NII
{
    //------------------------------------------------------------------------
    SkeletonSerializer::SkeletonSerializer(const DataStream * src) :
        FileSerializer(src)
    {
        mVersion = 30000;
    }
    //------------------------------------------------------------------------
    SkeletonSerializer::SkeletonSerializer(const FileSerializer & obj) :
        FileSerializer(obj)
    {
        mVersion = 30000;
    }
    //------------------------------------------------------------------------
    SkeletonSerializer::~SkeletonSerializer()
    {
    }
    //------------------------------------------------------------------------
    bool SkeletonSerializer::isVersion(SerialVer ver) const
    {
        if(ver == 30000)
            return true;

        return false;
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::wirte(const Skeleton * src, const String & dest,
        SerialVer ver, SerialFormat sf, Endian emode)
    {
        std::fstream * f = N_new_t(std::fstream)();
        f->open(dest.c_str(), std::ios::binary | std::ios::out);

        DataStream * stream = N_new FileDataStream(f);
        wirte(src, stream, ver, sf, emode);
        stream->close();
        N_delete stream;
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::wirte(const Skeleton * src, DataStream * dest,
        SerialVer ver, SerialFormat sf, Endian emode)
    {
        DataStream * old = mStream;
        mStream = dest;
        if(!dest->isWriteable())
        {
            N_EXCEPT(IO, _I18n("Unable to write to stream ") + dest->getName());
        }

        writeFileHeader();
        writeHeader();
        writeUnit(src, ver, sf, emode);
        reviseWrite();
        mStream = old;
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::read(const String & file, Skeleton * out)
    {
        std::fstream * f = N_new_t(std::fstream)();
        f->open(file.c_str(), std::ios::binary | std::ios::in);

        DataStream * stream = N_new FileDataStream(f);
        read(stream, out);
        stream->close();
        N_delete stream;
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::read(DataStream * src, Skeleton * dest)
    {
        DataStream * old = mStream;
        mStream = src;
        readFileHeader();
        readHeader();
        readUnit(dest);
        mStream = old;
    }
    //------------------------------------------------------------------------
    SerializerID SkeletonSerializer::getUnit() const
    {
        return N_Serializer_Skeleton_30000;
    }
    //------------------------------------------------------------------------
    GroupID SkeletonSerializer::getGroup() const
    {
        return GroupUnknow;
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::writeUnit(const void * in, SerialVer, SerialFormat sf, Endian emode)
    {
        check(emode);
        setFormat(sf);

        const Skeleton * src = static_cast<const Skeleton *>(in);

        // Write main skeleton data
        N_Only(Log).log(_I18n("Exporting bones.."));
        writeSkeleton(src);
        N_Only(Log).log(_I18n("Bones exported."));

        // Write all animations
        NCount a, aend = src->getAnimationCount();
        N_Only(Log).stream() << _I18n("Exporting animations, count=") << aend;
        for(a = 0; a < aend; ++a)
        {
            Animation * anim = src->getAnimation(a);
            N_Only(Log).stream() << _I18n("Exporting animation: ") << anim->getID();
            writeAnimation(anim);
            N_Only(Log).log(_I18n("Animation exported."));
        }

        Skeleton::ExtList::const_iterator i, iend = src->getExtList().end();
        for(i = src->getExtList().begin(); i != iend; ++i)
        {
            writeSkeletonExt(src, *i);
        }
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::writeSkeleton(const Skeleton * in)
    {
        // Write blend mode
        Nui16 t1 = SKELETON_BLENDMODE;
        Nui32 t2 = ChunkHeaderSize + sizeof(Nui16);
        Nui16 blendMode = static_cast<Nui16>(in->getBlendMode());
        write(&t1, 1);
        write(&t2, 1);
        write(&blendMode, 1);

        // Write each bone
        Nidx i, iend = in->getCount();
        for(i = 0; i < iend; ++i)
        {
            writeBone(in->get(i));
        }
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::writeBone(const Bone * bone)
    {
        Nui16 t1 = SKELETON_BONE;
        Nui32 t2 = getBoneSize(bone);
        Nui32 tid = bone->getID();
        write(&t1, 1);
        write(&t2, 1);
        // Nui32 handle             : handle of the bone, should be contiguous & start at 0
        write((Nui32 *)tid, 1);
        // Vector3 position         : position of this bone relative to parent
        write(&bone->getPos());
        // Quaternion orientation   : orientation of this bone relative to parent
        write(bone->getOri());
        // Vector3 scale            : scale of this bone relative to parent
        write(&bone->getScale());

        // Nui32 parentHandle : parent bone
        tid = bone->getParent()->getID();
        if(bone->getParent())
        {
            write((Nui32 *)&tid, 1);
        }
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::writeAnimation(const Animation * anim)
    {
        Nui16 t1 = SKELETON_ANIMATION;
        Nui32 t2 = getAnimationSize(anim);
        Nui32 id = anim->getID();
        Nui32 len = anim->getDur();

        write(&t1, 1);
        write(&t2, 1);
        // Nui32 id         : ID of the animation
        write(&id, 1);
        // Nui32 length     : Length of the animation in seconds
        write(&len, 1);

        // Write all tracks
        Animation::TrackList::const_iterator i, iend = anim->getTrack().end();
        for(i = anim->getTrack().begin(); i != iend; ++i)
        {
            writeKeyFrameTrack(static_cast<const NodeKeyFrameTrack *>(i->second));
        }
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::writeKeyFrameTrack(const NodeKeyFrameTrack * track)
    {
        Nui16 t1 = SKELETON_ANIMATION_TRACK;
        Nui32 t2 = getKeyFrameTrackSize(track);
        write(&t1, 1);
        write(&t2, 1);
        // Nui16 boneIndex     : Index of bone to apply to
        Bone * bone = (Bone *)track->getTarget();
        Nui32 boneid = bone->getID();
        write(&boneid, 1);

        for(NCount i = 0; i < track->getCount(); ++i)
        {
            writeKeyFrame(static_cast<NodeKeyFrame *>(track->get(i)));
        }
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::writeKeyFrame(const NodeKeyFrame * key)
    {
        Nui16 t1 = SKELETON_ANIMATION_TRACK_KEYFRAME;
        Nui32 t2 = getKeyFrameSize(key);
        Nui32 time = key->getTime();

        write(&t1, 1);
        write(&t2, 1);
        // Nui32 time                    : The time position (seconds)
        write(&time, 1);
        // Quaternion rotate            : Rotation to apply at this keyframe
        write(key->getRotation());
        // Vector3 translate            : Translation to apply at this keyframe
        write(&key->getTranslate());
        // Vector3 scale                : Scale to apply at this keyframe
        write(&key->getScale());
    }
    //------------------------------------------------------------------------
    NCount SkeletonSerializer::getBoneSize(const Bone * obj)
    {
        NCount size = ChunkHeaderSize;

        // handle
        size += sizeof(Nui32);
        // position
        size += sizeof(NIIf) * 3;
        // orientation
        size += sizeof(NIIf) * 4;
        // scale
        size += sizeof(NIIf) * 3;

        // parent handle
        if(obj->getParent())
        {
            size += sizeof(Nui32);
        }

        return size;
    }
    //------------------------------------------------------------------------
    NCount SkeletonSerializer::getAnimationSize(const Animation * obj)
    {
        NCount size = ChunkHeaderSize;
        // id
        size += sizeof(Nui32);
        // length
        size += sizeof(Nui32);

        // base animation
        if(obj->getBaseID() != 0)
        {
            size += ChunkHeaderSize;
            // Nui32 baseAnimationID
            size += sizeof(Nui32);
            // Nui32 baseKeyFrameTime
            size += sizeof(Nui32);
        }

        // Write all tracks
        Animation::TrackList::const_iterator i, iend = obj->getTrack().end();
        for(i = obj->getTrack().begin(); i != iend; ++i)
        {
            size += getKeyFrameTrackSize(static_cast<const NodeKeyFrameTrack *>(i->second));
        }

        return size;
    }
    //------------------------------------------------------------------------
    NCount SkeletonSerializer::getKeyFrameTrackSize(const NodeKeyFrameTrack * obj)
    {
        NCount size = ChunkHeaderSize;
        // Nui32 boneIndex     : Index of bone to apply to
        size += sizeof(Nui32);

        // Nested keyframes
        for(NCount i = 0; i < obj->getCount(); ++i)
        {
            size += getKeyFrameSize(static_cast<NodeKeyFrame *>(obj->get(i)));
        }

        return size;
    }
    //------------------------------------------------------------------------
    NCount SkeletonSerializer::getKeyFrameSize(const NodeKeyFrame * obj)
    {
        NCount size = ChunkHeaderSize;

        // Nui32 time                   : The time position (seconds)
        size += sizeof(Nui32);
        // Quaternion rotate            : Rotation to apply at this keyframe
        size += sizeof(NIIf) * 4;
        // Vector3 translate            : Translation to apply at this keyframe
        size += sizeof(NIIf) * 3;
        // Vector3 scale                : Scale to apply at this keyframe
        size += sizeof(NIIf) * 3;

        return size;
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::readUnit(void * out)
    {
        Skeleton * dest = static_cast<Skeleton *>(out);
        readSkeleton(dest);
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::readSkeleton(Skeleton * out)
    {
        mBoneParentList.clear();

        Nui16 streamID;
        while(!mStream->end())
        {
            read(&streamID, 1);
            read(&mChunkSize, 1);
            switch (streamID)
            {
            case SKELETON_BLENDMODE:
            {
                // Optional blend mode
                Nui16 blendMode;
                read(&blendMode, 1);
                out->setBlendMode(static_cast<Skeleton::Mode>(blendMode));
            }
            break;
            case SKELETON_BONE:
                readBone(out);
                break;
            case SKELETON_ANIMATION:
                readAnimation(out);
                break;
            case SKELETON_ANIMATION_LINK:
                readSkeletonExt(out);
                break;
            }
        }
        BoneParentList::iterator i, iend = mBoneParentList.end();
        for(i = mBoneParentList.begin(); i != iend; ++i)
        {
            out->get(i->first)->add(i->second);
        }
        // 假设骨头是存储在绑定姿态中
        out->build();
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::readBone(Skeleton * out)
    {
        // Nui32 handle     : handle of the bone, should be contiguous & start at 0
        Nui32 handle;
        read(&handle, 1);
        // Create new bone
        Bone * bone = out->create(handle);

        // Vector3 position : position of this bone relative to parent
        Vector3f pos;
        read<NIIf>(&pos);
        bone->setPos(pos);
        // Quaternion orientation : orientation of this bone relative to parent
        Quaternion q;
        read(&q);
        bone->setOri(q);
        // scale
        Vector3f scale;
        read<NIIf>(&scale);
        bone->setScale(scale);

        if(mChunkSize > getBoneSize(bone))
        {
            Nui32 pbone;
            // Nui32 pbone   : parent bone
            read(&pbone, 1);
            mBoneParentList.insert(Npair(pbone, bone));
        }
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::readAnimation(Skeleton * out)
    {
        // Nui32 id     : ID of the animation
        Nui32 id;
        // Nui32 length  : Length of the animation in seconds
        Nui32 len;

        read(&id, 1);
        read(&len, 1);

        Animation * ani = out->createAnimation(id, len);

        // Read all tracks
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(streamID == SKELETON_ANIMATION_TRACK && !mStream->end())
            {
                readKeyFrameTrack(out, ani);

                if(!mStream->end())
                {
                    // Get next stream
                    read(&streamID, 1);
                    read(&mChunkSize, 1);
                }
            }
            if(!mStream->end())
            {
                // Backpedal back to start of this stream if we've found a non-track
                mStream->skip(-ChunkHeaderSize);
            }
        }
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::readKeyFrameTrack(Skeleton * out, Animation * anim)
    {
        // Nui32 boneIndex     : Index of bone to apply to
        Nui32 boneHandle;
        read(&boneHandle, 1);

        // Find bone
        Bone * targetBone = out->get(boneHandle);
        // Create track
        NodeKeyFrameTrack * track = N_new NodeKeyFrameTrack(anim, boneHandle);
        track->setTarget(targetBone);
        // Keep looking for nested keyframes
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(streamID == SKELETON_ANIMATION_TRACK_KEYFRAME && !mStream->end())
            {
                readKeyFrame(out, track);

                if(!mStream->end())
                {
                    // Get next stream
                    read(&streamID, 1);
                    read(&mChunkSize, 1);
                }
            }
            if(!mStream->end())
            {
                // Backpedal back to start of this stream if we've found a non-keyframe
                mStream->skip(-ChunkHeaderSize);
            }
        }
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::readKeyFrame(Skeleton * out, NodeKeyFrameTrack * track)
    {
        // Nui32 time           : The time position (seconds)
        Nui32 time;
        read(&time, 1);

        NodeKeyFrame * kf = static_cast<NodeKeyFrame *>(track->create(time));

        // rotate    : Rotation to apply at this keyframe
        Quaternion rot;
        // translate    : Translation to apply at this keyframe
        Vector3f trans;
        // scale
        Vector3f scale;

        read(&rot);
        read<NIIf>(&trans);
        read<NIIf>(&scale);

        kf->setRotation(rot);
        kf->setTranslate(trans);
        kf->setScale(scale);

        track->buildSpline();
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::writeSkeletonExt(const Skeleton * obj, const Skeleton::Ext & link)
    {
        Nui16 t1 = SKELETON_ANIMATION_LINK;
        Nui32 t2 = getSkeletonExtSize(link);
        write(&t1, 1);
        write(&t2, 1);
        // Nui32 skeletonid
        write(&link.mSkeletonID, 1);
        // NIIf scale
        write(&link.mScale, 1);
    }
    //------------------------------------------------------------------------
    NCount SkeletonSerializer::getSkeletonExtSize(const Skeleton::Ext & obj)
    {
        NCount size = ChunkHeaderSize;
        // Nui32 skeletonid
        size += sizeof(Nui32);
        // NIIf scale
        size += sizeof(NIIf);

        return size;
    }
    //------------------------------------------------------------------------
    void SkeletonSerializer::readSkeletonExt(Skeleton * out)
    {
        Nui32 skeletonrid;
        NIIf scale;

        read(&skeletonrid, 1);
        read(&scale, 1);

        out->addExt(skeletonrid, scale);
    }
    //------------------------------------------------------------------------
}