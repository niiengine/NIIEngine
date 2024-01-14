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
#include "NiiFileStreamSerializer.h"

#include "NiiCommon.h"
#include "NiiException.h"
#include "NiiStrConv.h"
#include "NiiLogManager.h"
#include "NiiVector2.h"
#include "NiiVector3.h"
#include "NiiVector4.h"
#include "NiiMatrix3.h"
#include "NiiMatrix4.h"
#include "NiiQuaternion.h"
#include "NiiAABox.h"
#include "NiiPosNode.h"
#include "NiiRay.h"
#include "NiiSphere.h"
#include "NiiHash.h"

#define checkStream(a, b, c) \
    if (!mStream) N_EXCEPT(InvalidState, _I18n("Invalid operation, stream is null"));\
    if (a && mStream->end()) N_EXCEPT(InvalidState, _I18n("Invalid operation, end of file on stream"));\
    if (b && !mStream->isReadable()) N_EXCEPT(InvalidState, _I18n("Invalid operation, file is not readable"));\
    if (c && !mStream->isWriteable()) N_EXCEPT(InvalidState, _I18n("Invalid operation, file is not writeable"));

namespace NII
{
    //---------------------------------------------------------------------
    Nui32 FileStreamSerializer::HEADER_ID = 0x0001;
    Nui32 FileStreamSerializer::REVERSE_HEADER_ID = 0x10000000;
    Nui32 FileStreamSerializer::CHUNK_HEADER_SIZE =
        sizeof(Nui32) + sizeof(Nui16) + sizeof(Nui32) + sizeof(Nui32); 
        //id + version + length + checksum
    //---------------------------------------------------------------------
    FileStreamSerializer::FileStreamSerializer(const DataStream * stream, Endian emode, bool checkHeader):
        FileSerializer(stream),
        mCheckHeader(checkHeader)
    {
        mEndian = emode;
        if (mEndian != E_Native)
        {
#if N_ENDIAN == N_ENDIAN_B
            if (mEndian == E_L)
                mFlipEndian = true;
#else
            if (mEndian == E_B)
                mFlipEndian = true;
#endif
        }

        checkStream(false, false, false);
    }
    //---------------------------------------------------------------------
    FileStreamSerializer::~FileStreamSerializer()
    {
        // really this should be empty if read/write was complete, but be tidy
        if (!mChunkStack.empty())
        {
            N_Only(Log).stream() << _I18n("Warning: stream ") << mStream->getName() <<
                _I18n(" was not fully read / written; ") << mChunkStack.size() <<
                _I18n(" chunks remain unterminated.");
        }
        for(ChunkStack::iterator i = mChunkStack.begin(); i != mChunkStack.end(); ++i)
            delete *i;
        mChunkStack.clear();
    }
    //---------------------------------------------------------------------
    Endian FileStreamSerializer::getEndian() const 
    { 
        return mEndian; 
    }
    //---------------------------------------------------------------------
    Nui32 FileStreamSerializer::getCurrentChunkID() const
    {
        if (mChunkStack.empty())
            return 0;
        else
            return mChunkStack.back()->id;
    }
    //---------------------------------------------------------------------
    const FileStreamSerializer::DataChunk * FileStreamSerializer::readChunkBegin()
    {
        if (mCheckHeader)
            readFileHeader();
        if (mEndian == E_Native)
            N_EXCEPT(InvalidState, _I18n("Endian mode has not been determined, did you disable header without setting?"));

        DataChunk * chunk = readChunkImpl();
        mChunkStack.push_back(chunk);

        return chunk;
    }
    //---------------------------------------------------------------------
    const FileStreamSerializer::DataChunk * FileStreamSerializer::readChunkBegin(
        Nui32 id, Nui16 version, const String & msg)
    {
        const DataChunk * c = readChunkBegin();
        if (c->id != id)
        {
            seekLastChunk(c->id);
            return 0;
        }
        else if (c->version > version)
        {
            N_Only(Log).stream() << _I18n("Error: ") << msg << _I18n(" : Data version is ") << 
                c->version << _I18n(" but this software can only read ") << 
                    _I18n("up to version ") << version;

            readChunkEnd(c->id);
            return 0;
        }

        return c;

    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::seekLastChunk(Nui32 id)
    {
        DataChunk * c = popChunk(id);

        checkStream(false, false, false);

        mStream->seek(c->offset);

        N_delete c;
    }
    //---------------------------------------------------------------------
    Nui32 FileStreamSerializer::peekNextChunkID()
    {
        checkStream(false, false, false);

        if (eof())
            return 0;

        if (mCheckHeader)
            readFileHeader();
        if (mEndian == E_Native)
            N_EXCEPT(InvalidState, _I18n("Endian mode has not been determined, did you disable header without setting?"));

        NCount last = mStream->tell();
        Nui32 ret;
        read(&ret);
        mStream->seek(last);

        return ret;
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::readChunkEnd(Nui32 id)
    {
        DataChunk * c = popChunk(id);

        checkStream(false, false, false);

        // skip to the end of the chunk if we were not there already
        // this lets us quite reading a chunk anywhere and have the read marker
        // automatically skip to the next one
        NCount dst = c->offset + CHUNK_HEADER_SIZE + c->length;
        if (mStream->tell() < dst)
            mStream->seek(dst);

        N_delete c;
    }
    //---------------------------------------------------------------------
    bool FileStreamSerializer::isEndOfChunk(Nui32 id)
    {
        const DataChunk * c = getCurrentChunk();
        N_assert(c->id == id, "error");
        return mStream->tell() == (c->offset + CHUNK_HEADER_SIZE + c->length);
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::readFileHeader()
    {
        Nui32 headerid;
        NCount acread = mStream->read(&headerid, sizeof(Nui32));
        mStream->skip(0 - (NIIl)acread);
        if (headerid == REVERSE_HEADER_ID)
        {
            mFlipEndian = true;
        }
        else if (headerid == HEADER_ID)
        {
            mFlipEndian = false;
        }
        else
        {
            N_EXCEPT(InvalidState, _I18n("Cannot determine endian mode because header is missing"));
        }
#if N_ENDIAN == N_ENDIAN_B
        if (mFlipEndian)
            mEndian = E_L;
        else
            mEndian = E_B;
#else
        if (mFlipEndian)
            mEndian = E_B;
        else
            mEndian = E_L;
#endif

        mCheckHeader = false;

        const DataChunk * c = readChunkBegin();
        N_assert(c->id == HEADER_ID, "error");
        (void)c;

        readChunkEnd(HEADER_ID);
    }
    //---------------------------------------------------------------------
    const FileStreamSerializer::DataChunk * FileStreamSerializer::getCurrentChunk() const
    {
        if (mChunkStack.empty())
            return 0;
        else
            return mChunkStack.back();
    }
    //---------------------------------------------------------------------
    bool FileStreamSerializer::eof() const
    {
        checkStream(false, false, false);
        return mStream->end();
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::writeFileHeader()
    {
        if (mEndian == E_Native)
        {
#if N_ENDIAN == N_ENDIAN_B
            if (mFlipEndian)
                mEndian = E_L;
            else
                mEndian = E_B;
#else
            if (mFlipEndian)
                mEndian = E_B;
            else
                mEndian = E_L;
#endif
        }

        writeChunkImpl(HEADER_ID, 1);
        writeChunkEnd(HEADER_ID);
        mCheckHeader = false;
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::writeChunkBegin(Nui32 id, Nui16 version)
    {
        checkStream(false, false, true);

        if(mCheckHeader)
            writeFileHeader();

        if(mEndian == E_Native)
            N_EXCEPT(InvalidState, _I18n("无法确定字节序 !"));

        writeChunkImpl(id, version);
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::writeChunkEnd(Nui32 id)
    {
        checkStream(false, false, true);

        DataChunk * c = popChunk(id);

        NCount currPos = mStream->tell();
        c->length = static_cast<Nui32>(currPos - c->offset - CHUNK_HEADER_SIZE);

        // seek to 'length' position in stream for this chunk
        // skip id (32) and version (16)
        mStream->seek(c->offset + sizeof(Nui32) + sizeof(Nui16));
        write(&c->length);
        // write updated checksum
        Nui32 checksum = calcChecksum(c);
        write(&checksum);
        // seek back to previous position
        mStream->seek(currPos);

        N_delete c;
    }
    //---------------------------------------------------------------------
    NCount FileStreamSerializer::getChunkDataOffset() const
    {
        checkStream(false, false, false);

        if (mChunkStack.empty())
        {
            return 0;
        }
        else
        {
            NCount pos = mStream->tell();
            NCount diff = pos - mChunkStack.back()->offset;
            if(diff >= CHUNK_HEADER_SIZE)
                return diff - CHUNK_HEADER_SIZE;
            else
                return 0;
        }
    }
    //---------------------------------------------------------------------
    FileStreamSerializer::DataChunk * FileStreamSerializer::readChunkImpl()
    {
        DataChunk * chunk = N_new DataChunk();
        chunk->offset = static_cast<Nui32>(mStream->tell());
        read(&chunk->id);
        read(&chunk->version);
        read(&chunk->length);

        Nui32 checksum;
        read(&checksum);

        if (checksum != calcChecksum(chunk))
        {
            Nui32 off = chunk->offset;
            N_delete chunk;
            N_EXCEPT(InvalidState, _I18n("Corrupt chunk detected in stream ") + 
                mStream->getName() + _I18n(" at byte ") + N_conv(off));
        }
        else
        {
            return chunk;
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::writeChunkImpl(Nui32 id, Nui16 version)
    {
        DataChunk * c = N_new DataChunk();
        c->id = id;
        c->version = version;
        c->offset = mStream->tell();
        c->length = 0;

        mChunkStack.push_back(c);

        write(&c->id);
        write(&c->version);
        write(&c->length);
        write(&c->length);
    }
    //---------------------------------------------------------------------
    NCount FileStreamSerializer::write(const void * buf, NCount size, NCount cnt, bool _flip)
    {
        checkStream(false, false, true);
        return Serializer::write(buf, size, cnt, true);
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::write(const AABox * aabb, NCount cnt)
    {
        for(NCount i = 0; i < cnt; ++i, ++aabb)
        {
            write(&aabb->getMin());
            write(&aabb->getMax());
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::write(const Sphere * sphere, NCount cnt)
    {
        for (NCount i = 0; i < cnt; ++i, ++sphere)
        {
            write(&sphere->getCenter());
            NIIf radius = sphere->getRadius();
            write(&radius);
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::write(const Plane * plane, NCount cnt)
    {
        for (NCount i = 0; i < cnt; ++i, ++plane)
        {
            write(&plane->mNormal);
            write(&plane->mD);
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::write(const Ray * ray, NCount cnt)
    {
        for(NCount i = 0; i < cnt; ++i, ++ray)
        {
            write(&ray->getOrigin());
            write(&ray->getDirection());
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::write(const Radian * rad, NCount cnt)
    {
        for(NCount i = 0; i < cnt; ++i, ++rad)
        {
            NIIf r = rad->getValue();
            write(&r);
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::write(const PosNode * node, NCount cnt)
    {
        for(NCount i = 0; i < cnt; ++i, ++node)
        {
            write(&node->getPos());
            write(&node->getOri());
            write(&node->getScale());
        }
    }
    //---------------------------------------------------------------------
    NCount FileStreamSerializer::read(void * out, NCount size, NCount cnt, bool _flip) const
    {
        checkStream(true, true, false);
        return Serializer::read(out, size, cnt, _flip);
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::read(AABox * aabb, NCount cnt) const
    {
        for(NCount i = 0; i < cnt; ++i, ++aabb)
        {
            Vector3f min, max;
            read(&min);
            read(&max);

            aabb->setMin(min);
            aabb->setMax(max);
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::read(Sphere * sphere, NCount cnt) const
    {
        for (NCount i = 0; i < cnt; ++i, ++sphere)
        {
            Vector3f center;
            NIIf radius;
            read(&center);
            read(&radius);
            sphere->setCenter(center);
            sphere->setRadius(radius);
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::read(Plane * plane, NCount cnt) const
    {
        for (NCount i = 0; i < cnt; ++i, ++plane)
        {
            read(&plane->mNormal);
            read(&plane->mD);
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::read(Ray * ray, NCount cnt) const
    {
        for (NCount i = 0; i < cnt; ++i, ++ray)
        {
            Vector3f origin, dir;
            read(&origin);
            read(&dir);
            ray->setOrigin(origin);
            ray->setDirection(dir);
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::read(Radian * angle, NCount cnt)
    {
        for (NCount i = 0; i < cnt; ++i, ++angle)
        {
            NIIf rads;
            read(&rads);
            *angle = Radian(rads);
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::read(PosNode * node, NCount cnt)
    {
        for(NCount i = 0; i < cnt; ++i, ++node)
        {
            Vector3f pos, scale;
            Quaternion orient;
            read(&pos);
            read(&orient);
            read(&scale);
            node->setPos(pos);
            node->setOri(orient);
            node->setScale(scale);
        }
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::writeNIIfAsNIId(const NIIf * val, NCount cnt)
    {
        writeConverted<NIIf, NIId>(val, cnt);
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::writeNIIdAsNIIf(const NIId * val, NCount cnt)
    {
        writeConverted<NIId, NIIf>(val, cnt);
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::readNIIfAsNIId(NIId * val, NCount cnt)
    {
        readConverted<NIId, NIIf>(val, cnt);
    }
    //---------------------------------------------------------------------
    void FileStreamSerializer::readNIIdAsNIIf(NIIf * val, NCount cnt)
    {
        readConverted<NIIf, NIId>(val, cnt);
    }
    //---------------------------------------------------------------------
    Nui32 FileStreamSerializer::calcChecksum(DataChunk * c)
    {
        // Always calculate checksums in little endian to make sure they match
        // Otherwise checksums for the same data on different endians will not match
        Nui32 id = c->id;
        Nui16 version = c->version;
        Nui32 length = c->length;
#if N_ENDIAN == N_ENDIAN_B
        flipEndian(&id, sizeof(Nui32));
        flipEndian(&version, sizeof(Nui16));
        flipEndian(&length, sizeof(Nui32));
#endif
        Nui32 hashVal = FastHash((const Nui8*)&id, sizeof(Nui32));
        hashVal = FastHash((const Nui8*)&version, sizeof(Nui16), hashVal);
        hashVal = FastHash((const Nui8*)&length, sizeof(Nui32), hashVal);

        return hashVal;
    }
    //---------------------------------------------------------------------
    FileStreamSerializer::DataChunk * FileStreamSerializer::popChunk(Nui32 id)
    {
        if(mChunkStack.empty())
            N_EXCEPT(InvalidState, _I18n("No active chunk!"));

        const DataChunk * chunk = mChunkStack.back();
        if(chunk->id != id)
            N_EXCEPT(InvalidState, _I18n("Incorrect chunk id!"));

        DataChunk * re = mChunkStack.back();
        mChunkStack.pop_back();
        return re;
    }
    //---------------------------------------------------------------------------
}