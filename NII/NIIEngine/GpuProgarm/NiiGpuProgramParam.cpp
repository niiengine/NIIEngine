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
#include "NiiGpuProgramParam.h"
#include "NiiGpuProgramManager.h"
#include "NiiDualQuaternion.h"
#include "NiiLight.h"
#include "NiiEngine.h"
#include "NiiSysSyncParam.h"
#include "NiiStrConv.h"
#include "NiiFrameObj.h"
#include "NiiLogManager.h"
#include "NiiFileSerializer.h"
#include "NiiDefaultStream.h"
#include "NiiResourceSchemeManager.h"
#include "NiiFileSerializerList.h"

namespace NII
{
    //------------------------------------------------------------------------
    class _EngineInner GpuParamDefsSerializer : public FileSerializer
    {
    public:
        GpuParamDefsSerializer(const DataStream * src);
        ~GpuParamDefsSerializer();

        using FileSerializer::write;
        using FileSerializer::read;

        void write(const GpuParamDefine * in, const String & file, SerialVer ver = 30000, 
            SerialFormat sf = F_EC_None, Endian emode = E_Native);

        void write(const GpuParamDefine * in, DataStream * stream, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);

        void read(DataStream * in, GpuParamDefine * out);

        /// @copydetails FileSerializer::isVersion
        bool isVersion(SerialVer ver) const;

        /// @copydetails FileSerializer::getUnit
        SerializerID getUnit() const    { return N_Serializer_GpuParamDefs_30000; }

        /// @copydetails FileSerializer::getGroup
        GroupID getGroup() const        { return GroupUnknow; }
    protected:
        /// @copydetails FileSerializer::read
        void readUnit(void * out);

        /// @copydetails FileSerializer::write
        void writeUnit(const void * in, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);
    };
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // GpuEnvParamUnit
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    /** 环境参数
    @version NIIEngine 3.0.0 Deprecated
    */
    /*struct _EngineAPI GpuEnvParamUnit
    {
        GpuEnvParamUnit(const String & name, Nui32 syncParam, NCount cnt, Nui16 exttype, Nui16 datatype);
            mName(name),
            mSyncParam(syncParam),
            mCount(cnt),
            mInputType(exttype),
            mDataType(datatype){}
        inline bool isFloat() const         { return mDataType > GDT_Unknow && mDataType < GDT_Int; }

        inline bool isDouble() const        { return mDataType > GDT_SamplerArray2D && mDataType < GDT_Block; }

        inline bool isSampler() const       { return mDataType > GDT_Bool4X && mDataType < GDT_64Int; }

        inline bool isInt() const           { return mDataType > GDT_Matrix4X4 && mDataType < GDT_UInt; }

        inline bool isUInt() const          { return mDataType > GDT_Int4X && mDataType < GDT_Bool; }

        inline bool isBool() const          { return mDataType > GDT_UInt4X && mDataType < GDT_Sampler1D; }

        inline bool isBlock() const         { return mDataType == GDT_Block; }

        String mName;
        Nui16 mDataType;
        Nui16 mInputType;
        Nui32 mSyncParam;
        Nui32 mCount;
    };*/
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // GpuParamUnit
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    const GpuParamUnit GpuParamUnit::UnitNone(GSP_Null, 0, SPI_None, GDT_Unknow);
    //---------------------------------------------------------------------
    GpuParamUnit::GpuParamUnit(const GpuParamUnit & o)
    {
        memcpy(this, &o, sizeof(GpuParamUnit));
    }
    //---------------------------------------------------------------------
    GpuParamUnit::GpuParamUnit() :
        mIndex(0),
        mDataType(GDT_Unknow),
        mSyncParam(GSP_Null),
        mUnit32bSize(0),
        mUnitCount(0),
        mMemIndex(N_TYPE_MAX(Nidx)),
        mArrayIndex(-1),
        mGBTMark(GPT_Render),
        mArrayMode(false){}
    //---------------------------------------------------------------------
    GpuParamUnit(Nui16 syncParam, Nui32 unitCnt, Nui16 inputType, Nui16 dataType) :
        mIndex(0),
        mDataType(dataType),
        mSyncParam(syncParam),
        mUnitCount(unitCnt),
        mInputType(inputType),
        mMemIndex(N_TYPE_MAX(Nidx)),
        mArrayIndex(-1),
        mGBTMark(GPT_Render),
        mArrayMode(false)
    {
        mUnit32bSize = isDouble() ? 2 : 1,
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // GpuParamDefine
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    GpuParamDefine::GpuParamDefine() :
        m32bSize(0)
    {
    }
    //---------------------------------------------------------------------
    GpuParamDefine::GpuParamDefine(const GpuParamUnitList & defs)
    {
        mDefineList = defs;
        GpuParamUnitList::const_iterator i, iend = mDefineList.end();
        for(i = mDefineList.begin(); i != iend; ++i)
        {
            m32bSize += i->second.mUnit32bSize * i->second.mUnitCount;
        }
    }
    //---------------------------------------------------------------------
    GpuParamDefine & GpuParamDefine::operator = (const GpuParamDefine & o)
    {
        mDefineList = o.mDefineList;
        m32bSize = o.m32bSize;
        return *this;
    }
    //---------------------------------------------------------------------
    void GpuParamDefine::add(const VString & name, const GpuParamUnit & def, bool arrayMode)
    {
        if (arrayMode)
        {
            GpuParamUnit arrayDef = def;
            arrayDef.mArrayMode = true;
            mDefineList.emplace(name, def);

            arrayDef.mUnitCount = 1;

            NCount arrayindex = 1;
            if (def.mUnitCount <= 16)
                arrayindex = def.mUnitCount;

            for (NCount i = 0, arrayDef.mArrayIndex = 0; i < arrayindex; ++i, ++arrayDef.mArrayIndex)
            {
                mDefineList.emplace(name + "[" + N_convV(i) + "]", arrayDef);
                arrayDef.mMemIndex += arrayDef.mUnit32bSize;
            }
        }
        else
        {
            mDefineList.emplace(name, def);
        }

        NCount dstsize = def.mMemIndex + def.mUnit32bSize * def.mUnitCount;
        if (m32bSize < dstsize)
            m32bSize = dstsize;
    }
    //---------------------------------------------------------------------
    GpuParamUnit * GpuParamDefine::get(const VString & name) const
    {
        GpuParamUnitList::const_iterator i = mDefineList.find(name);
        if(i != mDefineList.end())
        {
            return const_cast<GpuParamUnit *>(&(i->second));
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void GpuParamDefine::remove(const VString & name)
    {
        GpuParamUnitList::iterator i = mDefineList.find(name);
        if(i != mDefineList.end())
        {
            GpuParamUnit & unit = i->second;
            if(unit.mArrayMode)
            {
                NCount arrayindex = 1;
                if(unit.mUnitCount <= 16)
                    arrayindex = unit.mUnitCount;
                mDefineList.erase(i);

                for(NCount j = 0; j < arrayindex; ++j)
                {
                    i = mDefineList.find(name + "[" + N_convV(j) + "]");
                    if(i != mDefineList.end())
                    {
                        mDefineList.erase(i);
                    }
                }
            }
            else
            {
                mDefineList.erase(i);
            }
        }
    }
    //---------------------------------------------------------------------
    void GpuParamDefine::removeAll()
    {
        mDefineList.clear();
        m32bSize = 0;
    }
    //---------------------------------------------------------------------
    void GpuParamDefine::save(const String & file) const
    {
        GpuParamDefsSerializer ser(0);
        ser.write(this, file);
    }
    //---------------------------------------------------------------------
    void GpuParamDefine::load(const String & file)
    {
        DataStream * stream = N_Only(ResourceScheme).open(file, GroupUnknow, true);
        load(stream);
        N_delete stream;
    }
    //---------------------------------------------------------------------
    void GpuParamDefine::load(DataStream * stream)
    {
        GpuParamDefsSerializer ser(0);
        ser.read(stream, this);
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // GpuParamDefsSerializer
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    GpuParamDefsSerializer::GpuParamDefsSerializer(const DataStream * src) :
        FileSerializer(src)
    {
        mVersion = 30000;
    }
    //---------------------------------------------------------------------
    GpuParamDefsSerializer::~GpuParamDefsSerializer()
    {

    }
    //---------------------------------------------------------------------
    void GpuParamDefsSerializer::write(const GpuParamDefine * in, const String & file,
        SerialVer ver, SerialFormat sf, Endian emode)
    {
        std::fstream * f = N_new_t(std::fstream)();
        f->open(file.c_str(), std::ios::binary | std::ios::out);

        DataStream * stream = N_new FileDataStream(f);
        write(in, stream, ver, sf, emode);
        stream->close();

        N_delete stream;
    }
    //---------------------------------------------------------------------
    void GpuParamDefsSerializer::write(const GpuParamDefine * in, DataStream * stream,
        SerialVer ver, SerialFormat sf, Endian emode)
    {
        DataStream * old = mStream;
        mStream = stream;
        if(!stream->isWriteable())
        {
            N_EXCEPT(IO, _I18n("不能写入数据流: ") + stream->getName());
        }

        writeFileHeader();
        writeHeader();
        write(in, ver, sf, emode);

        mStream = old;
    }
    //---------------------------------------------------------------------
    void GpuParamDefsSerializer::read(DataStream * stream, GpuParamDefine * out)
    {
        DataStream * old = mStream;
        mStream = stream;

        readFileHeader();
        readHeader();
        readUnit(out);

        mStream = old;
    }
    //-------------------------------------------------------------------------
    bool GpuParamDefsSerializer::isVersion(SerialVer ver) const
    {
        if(ver == 30000)
        {
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------------------
    void GpuParamDefsSerializer::readUnit(void * out)
    {
        GpuParamDefine * defs = static_cast<GpuParamDefine *>(out);
        defs->removeAll();
        NCount temp = defs->getSize();
        read((Nui32 *)&temp, 1);

        while(!mStream->end())
        {
            VString name;
            read(name);
            // 处理尾部信息
            if(name.empty())
                continue;

            GpuParamUnit def;
            read(((Nui32 *)&def.mMemIndex), 1);
            read(((Nui32 *)&def.mGBTMark), 1);
            read(((Nui32 *)&def.mSyncParam), 1);
            read(((Nui32 *)&def.mUnit32bSize), 1);
            read(((Nui32 *)&def.mUnitCount), 1);
            read(((Nui16 *)&def.mIndex), 1);
            read(((Nui16 *)&def.mDataType, 1);
            read(((Nui16 *)&def.mArrayIndex), 1);
            read(((Nui16 *)&def.mInputType), 1);
            read(((bool *)&def.mArrayMode), 1);

            defs->add(name, def);
        }
    }
    //-------------------------------------------------------------------------
    void GpuParamDefsSerializer::writeUnit(const void * in, SerialVer ver, SerialFormat sf, Endian emode)
    {
        const GpuParamDefine * defs = static_cast<const GpuParamDefine *>(in);

        check(emode);
        setFormat(sf);
        NCount temp = defs->getSize();
        write((Nui32 *)&temp, 1);

        GpuParamUnitList::const_iterator i, iend = defs->getList().end();
        for(i = defs->getList().begin(); i != iend; ++i)
        {
            const GpuParamUnit & def = i->second;

            write(i->first);
            write(((Nui32 *)&def.mMemIndex), 1);
            write(((Nui32 *)&def.mGBTMark), 1);
            write(((Nui32 *)&def.mSyncParam), 1);
            write(((Nui32 *)&def.mUnit32bSize), 1);
            write(((Nui32 *)&def.mUnitCount), 1);
            write(((Nui16 *)&def.mIndex), 1);
            write(((Nui16 *)&def.mDataType), 1);
            write(((Nui16 *)&def.mArrayIndex), 1);
            write(((Nui16 *)&def.mInputType), 1);
            write(((bool *)&def.mArrayMode), 1);
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GpuParamBufferUnit 方法
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    GpuParamBufferUnit::GpuParamBufferUnit() : mRefCount(0), mAutoDestroy(false){}
    //------------------------------------------------------------------------
    GpuParamBufferUnit::~GpuParamBufferUnit()
    {
        
    }
    //------------------------------------------------------------------------
    void GpuParamBufferUnit::set(NCount oft, NIIf in, NIIf in2, NIIf in3, NIIf in4)
    {
        if (mParamType == PT_Storage)
        {
            NIIf temp[4] = {in, in2, in3, in4};
            mStorage.mBuffer->write(temp, oft, 4 * sizeof(NIIf));
        }
        else
        {
            GpuBuffer * buffer = mSampler.mTexture->getGpuBuffer();
            NIIf temp[4] = {in, in2, in3, in4};
            buffer->lock(Buffer::MM_WRITE, oft, 4 * sizeof(NIIf));
            buffer->write(temp, 0, 4 * sizeof(NIIf));
            buffer->unlock();
        }
    }
    //------------------------------------------------------------------------
    void GpuParamBufferUnit::set(NCount oft, NIId in, NIId in2, NIId in3, NIId in4)
    {
        if (mParamType == PT_Storage)
        {
            NIId temp[4] = {in, in2, in3, in4};
            mStorage.mBuffer->write(temp, oft, 4 * sizeof(NIId));
        }
        else
        {
            GpuBuffer * buffer = mSampler.mTexture->getGpuBuffer();
            NIId temp[4] = {in, in2, in3, in4};
            buffer->lock(Buffer::MM_WRITE, oft, 4 * sizeof(NIId));
            buffer->write(temp, 0, 4 * sizeof(NIId));
            buffer->unlock();
        }
    }
    //------------------------------------------------------------------------
    void GpuParamBufferUnit::set(NCount oft, Ni32 in, Ni32 in2, Ni32 in3, Ni32 in4)
    {
        if (mParamType == PT_Storage)
        {
            Ni32 temp[4] = {in, in2, in3, in4};
            mStorage.mBuffer->write(temp, oft, 4 * sizeof(Ni32));
        }
        else
        {
            GpuBuffer * buffer = mSampler.mTexture->getGpuBuffer();
            Ni32 temp[4] = {in, in2, in3, in4};
            buffer->lock(Buffer::MM_WRITE, oft, 4 * sizeof(Ni32));
            buffer->write(temp, 0, 4 * sizeof(Ni32));
            buffer->unlock();
        }
    }
    //------------------------------------------------------------------------
    void GpuParamBufferUnit::set(NCount oft, const bool * in, NCount cnt)
    {
        if (mParamType == PT_Storage)
        {
            mStorage.mBuffer->write(in, oft, cnt * sizeof(bool));
        }
        else
        {
            GpuBuffer * buffer = mSampler.mTexture->getGpuBuffer();
            buffer->lock(Buffer::MM_WRITE, oft, cnt * sizeof(bool));
            buffer->write(in, 0, cnt * sizeof(bool));
            buffer->unlock();
        }
    }
    //------------------------------------------------------------------------
    void GpuParamBufferUnit::set(NCount oft, const Ni32 * in, NCount cnt)
    {
        if (mParamType == PT_Storage)
        {
            mStorage.mBuffer->write(in, oft, cnt * sizeof(Ni32));
        }
        else
        {
            GpuBuffer * buffer = mSampler.mTexture->getGpuBuffer();
            buffer->lock(Buffer::MM_WRITE, oft, cnt * sizeof(Ni32));
            buffer->write(in, 0, cnt * sizeof(Ni32));
            buffer->unlock();
        }
    }
    //------------------------------------------------------------------------
    void GpuParamBufferUnit::set(NCount oft, const NIIf * in, NCount cnt)
    {
        if (mParamType == PT_Storage)
        {
            mStorage.mBuffer->write(in, oft, cnt * sizeof(NIIf));
        }
        else
        {
            GpuBuffer * buffer = mSampler.mTexture->getGpuBuffer();
            buffer->lock(Buffer::MM_WRITE, oft, cnt * sizeof(NIIf));
            buffer->write(in, 0, cnt * sizeof(NIIf));
            buffer->unlock();
        }
    }
    //------------------------------------------------------------------------
    void GpuParamBufferUnit::set(NCount oft, const NIId * in, NCount cnt)
    {
        if (mParamType == PT_Storage)
        {
            mStorage.mBuffer->write(in, oft, cnt * sizeof(NIId));
        }
        else
        {
            GpuBuffer * buffer = mSampler.mTexture->getGpuBuffer();
            buffer->lock(Buffer::MM_WRITE, oft, cnt * sizeof(NIId));
            buffer->write(in, 0, cnt * sizeof(NIId));
            buffer->unlock();
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GpuParamBuffer 方法
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool GpuParamBuffer::operator == (const GpuParamBuffer & o) const
    {
        if (mUnitList.size() == o.mUnitList.size())
        {
            NCount i, iend = mUnitList.size();
            for (i = 0; i < iend; ++i)
            {
                if (mUnitList[i] != o.mUnitList[i])
                    return false;
            }
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool GpuParamBuffer::operator < (const GpuParamBuffer & o) const
    {
        if (mUnitList.size() != o.mUnitList.size())
        {
            return mUnitList.size() < o.mUnitList.size();
        }
        NCount i, iend = mUnitList.size();
        for (i = 0; i < iend; ++i)
        {
            if (mUnitList[i] < o.mUnitList[i])
                return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void GpuParamBuffer::addBinding(Nidx slot, const GpuParamBufferUnit & unit)
    {
        Nidx target = -1;
        Nidx temptarget;
        GpuParamBufferUnitList::iterator i, iend = mUnitList.end();
        for(temptarget = 0, i = mUnitList.begin(); i != iend; ++i, ++temptarget)
        {
            if(*i == unit)
            {
                target = temptarget;
                break;
            }
        }
        if(target == -1)
        {
            mUnitList.emplace_back(unit);
            target = mUnitList.size() - 1;
        }
        
        SlotList::iterator j = mSlotList.find(slot);
        if(j != mSlotList.end())
        {
            N_assert(mUnitList.size() > j->second, "error");
            GpuParamBufferUnitList::iterator i = mUnitList.begin();
            std::advance(i, j->second);
            --(*i).mRefCount;
            if((*i).mRefCount == 0)
            {
                //if((*i).mAutoDestroy)
                //    N_delete *i;
                mUnitList.erase(i);
            }
            j->second = target;
        }
        else
        {
            mSlotList.insert(Npair(slot, target));
        }
        ++mUnitList[target].mRefCount;
    }
    //------------------------------------------------------------------------
    void GpuParamBuffer::removeBinding(Nidx slot)
    {
        SlotList::iterator j = mSlotList.find(slot);
        if(j != mSlotList.end())
        {
            N_assert(mUnitList.size() > j->second, "error");
            GpuParamBufferUnitList::iterator i = mUnitList.begin();
            std::advance(i, j->second);
            --(*i).mRefCount;
            if((*i).mRefCount == 0)
            {
                //if((*i).mAutoDestroy)
                //    N_delete *i;
                mUnitList.erase(i);
            }
            mSlotList.erase(j);

            NamedBindList::iterator z,  zend = mNamedSlotList.end();
            for(z = mNamedSlotList.begin(); z != zend;)
            {
                if(z->second == slot)
                    z = mNamedSlotList.erase(z);
                else
                    ++z;
            }
        }
    }
    //------------------------------------------------------------------------
    const GpuParamBufferUnit * GpuParamBuffer::getBinding(Nidx slot) const
    {
        SlotList::const_iterator j = mSlotList.find(slot);
        if(j != mSlotList.end())
        {
            N_assert(mUnitList.size() > j->second, "");
            return &mUnitList[j->second];
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void GpuParamBuffer::removeBindingPoint(const VString & name)
    {
        NamedBindList::iterator i = mNamedSlotList.find(name);
        if(i != mNamedSlotList.end())
            mNamedSlotList.erase(i);
    }
    //------------------------------------------------------------------------
    Nidx GpuParamBuffer::getBindingPoint(const VString & name) const
    {
        NamedBindList::const_iterator i = mNamedSlotList.find(name);
        if(i != mNamedSlotList.end())
            return i->second; 
        return -1;
    }
    //------------------------------------------------------------------------
    bool GpuParamBuffer::isValid() const
    {
        SlotList::const_iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            const GpuParamBufferUnit & unit = mUnitList[i->first];
            if(unit.isTexture())
            {
                const SamplerUnit & su = unit.getTexture();
                if(su.mFormat != su.mTexture->getFormat() && !su.mTexture->isAlterView())
                {
                    //This warning here is for
                    N_EXCEPT(Internal, "UAV sRGB textures must be bound as non-sRGB. You must set the reinterpretable flag "
                        "(Texture::MM_AlterView) for texture: '" + su.mTexture->getName() + "' " + 
                            su.mTexture->getSettingsDesc());
                }
            }
            else if(unit.isStorage())
            {
                const StorageUnit & su = unit.getBuffer();
                if(N_MarkTrue(su.mBuffer->getModeMark(), M_Normal))
                {
                    N_EXCEPT(Internal, "Dynamic buffers cannot be baked into a static DescriptorSet");
                }
            }
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GpuParamValue 方法
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    GpuParamValue::GpuParamValue(const String & name) :
        mName(name),
        mDataSize(0),
        mDirtyMark(0)
    {
        //N_Engine().getCurrentFrame();
    }
    //------------------------------------------------------------------------
    GpuParamValue::~GpuParamValue()
    {
    }
    //------------------------------------------------------------------------
    void GpuParamValue::update(GpuProgramParam * params, bool reset)
    {
        GpuParamUnitList::iterator i, iend = mDefines.end();
        for(i = mDefines.begin(); i != iend; ++i)
        {
            if(reset)
            {
                const GpuParamUnit & unit = i->second;
                if(mSyncParam == GSP_Null)
                {
                    if(unit.mInputType != SPI_Float)
                    {
                        params->set(i->first, unit.mSyncParam,  );
                    }
                    else
                    {
                        params->set(i->first, unit.mSyncParam,  );
                    }
                }
                else if(unit.isFloat())
                {
                    params->set(i->first, 0, (NIIf *)mBufferData[unit.mMemIndex], unit.mUnitCount, unit.mUnit32bSize);
                }
                else if(unit.isDouble())
                {
                    params->set(i->first, 0, (NIId *)mBufferData[unit.mMemIndex], unit.mUnitCount, unit.mUnit32bSize);
                }
                else
                {
                    params->set(i->first, 0, (Ni32 *)mBufferData[unit.mMemIndex], unit.mUnitCount, unit.mUnit32bSize);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void GpuParamValue::add(const VString & name, GpuDataType type, NCount count)
    {
        if(mDefines.find(name) == mDefines.end())
        {
            GpuParamUnit def;
            def.mSyncParam = GSP_Null;
            def.mUnitCount = count;
            def.mDataType = type;
            def.mUnit32bSize = GpuProgramParam::get32bCount(type, false);
            def.mIndex = 0;
            def.mGBTMark = GPT_Render;
            def.mMemIndex = mBufferData.size();
            mBufferData.resize(mBufferData.size() + def.mUnitCount * def.mUnit32bSize);

            mDefines.emplace(name, def);

            ++mDirtyMark;
        }
    }
    //------------------------------------------------------------------------
    void GpuParamValue::add(const VString & name, GpuSyncParam gsp, SyncParamInputData extData)
    {
        if(mDefines.find(name) == mDefines.end())
        {
            const GpuParamUnit * def = GpuProgramManager::getSyncParam(gsp);
            if(def)
            {
                NCount cnt = def->mUnitCount;
                if (cnt % 4 > 0)
                {
                    cnt += 4 - (cnt % 4);
                }
                cnt *= def.mUnit32bSize;
                def.mMemIndex = mBufferData.size();
                mBufferData.resize(mBufferData.size() + cnt);
                mInputDataList.insert(Npair(name, extData));
                mDefines.insert(Npair(name, *def));

                ++mDirtyMark;
            }
        }
    }
    //------------------------------------------------------------------------
    const GpuParamUnit & GpuParamValue::get(const VString & name) const
    {
        GpuParamUnitList::const_iterator i = mDefines.find(name);
        if(i == mDefines.end())
        {
            return GpuParamUnit::UnitNone;
        }
        return i->second;
    }
    //------------------------------------------------------------------------
    void GpuParamValue::remove(const VString & name)
    {
        GpuParamUnitList::iterator i = mDefines.find(name);
        if (i != mDefines.end())
        {
            const GpuParamUnit & def = i->second;
            NCount cnt = def.mUnit32bSize * def.mUnitCount;
            for(GpuParamUnitList::iterator j = mDefines.begin(); j != mDefines.end(); ++j)
            {
                GpuParamUnit & o = j->second;
                if(o.mMemIndex > def.mMemIndex)
                {
                    o.mMemIndex -= cnt;
                }
            }

            mDataSize -= cnt;

            GBufferArray::iterator beg = mBufferData.begin();
            std::advance(beg, def.mMemIndex);
            GBufferArray::iterator en = beg;
            std::advance(en, cnt);

            mBufferData.erase(beg, en);
            ++mDirtyMark;
        }
    }
    //------------------------------------------------------------------------
    void GpuParamValue::removeAll()
    {
        mDefines.clear();
        mBufferData.clear();
        mDataSize = 0;
    }
    //------------------------------------------------------------------------
    void GpuParamValue::set(const VString & name, NCount oft32b, const NIIf * in, NCount cnt)
    {
        GpuParamUnitList::const_iterator i = mDefines.find(name);
        if(i != mDefines.end())
        {
            const GpuParamUnit & def = i->second;
            memcpy(&mBufferData[def.mMemIndex + oft32b], in, sizeof(NIIf) * std::min(cnt, def.mUnit32bSize * def.mUnitCount));
        }
        //N_Engine().getCurrentFrame();
    }
    //------------------------------------------------------------------------
    void GpuParamValue::set(const VString & name, NCount oft32b, const NIId * in, NCount cnt)
    {
        GpuParamUnitList::const_iterator i = mDefines.find(name);
        if(i != mDefines.end())
        {
            const GpuParamUnit & def = i->second;
            memcpy(&mBufferData[def.mMemIndex + oft32b], in, sizeof(NIIf) * std::min(cnt * 2, def.mUnit32bSize * def.mUnitCount));
        }
        //N_Engine().getCurrentFrame();
    }
    //------------------------------------------------------------------------
    void GpuParamValue::set(const VString & name, NCount oft32b, const Ni32 * in, NCount cnt)
    {
        GpuParamUnitList::const_iterator i = mDefines.find(name);
        if(i != mDefines.end())
        {
            const GpuParamUnit & def = i->second;
            memcpy(&mBufferData[def.mMemIndex + oft32b], in, sizeof(Ni32) * std::min(cnt, def.mUnit32bSize * def.mUnitCount));
        }
        //N_Engine().getCurrentFrame();
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // ParamValueSync 方法
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    class _EngineInner ParamValueSync : public GpuParamsAlloc
    {
    public:
        ParamValueSync(GpuParamValue * value, GpuProgramParam * params):
            mValue(value),
            mParams(params)
        {
            mapData();
        }
        /** 执行同步
        @version NIIEngine 3.0.0
        */
        void sync();

        /** 获取共享参数集的名字
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const               { return mValue->getName(); }

        /** 获取公共参数
        @version NIIEngine 3.0.0
        */
        inline GpuParamValue * getValue() const             { return mValue; }

        /** 获取副本参数
        @version NIIEngine 3.0.0
        */
        inline GpuProgramParam * getParam() const           { return mParams; }
    protected:
        struct MapData
        {
            const GpuParamUnit * mSrc;
            const GpuParamUnit * mDst;
        };
        typedef vector<MapData>::type CopyDataList;
        
        void mapData()
        {
            mMapList.clear();
            
            const GpuParamDefine * def = mParams->getDefine();
            if(def)
            {
                GpuParamUnitList::const_iterator i, iend = mValue->getDefList().end();
                for(i = mValue->getDefList().begin(); i != iend; ++i)
                {
                    const GpuParamUnit & shareddef = i->second;
                    const GpuParamUnit * unit = def->get(i->first);
                    if(unit)
                    {
                        if(unit->mDataType == shareddef.mDataType && unit->mUnitCount == shareddef.mUnitCount)
                        {
                            MapData md;
                            md.mSrc = &shareddef;
                            md.mDst = unit;
                            mMapList.push_back(md);
                        }
                    }
                }
            }
            mStateMark = mValue->getStateMark();
        }
    protected:
        GpuProgramParam * mParams;
        GpuParamValue * mValue;
        CopyDataList mMapList;
        Nul mStateMark;
    };
    //----------------------------------------------------------------------------
    void ParamValueSync::sync()
    {
        if(mStateMark != mValue->getStateMark())
            mapData();

        for(CopyDataList::iterator i = mMapList.begin(); i != mMapList.end(); ++i)
        {
            MapData & e = *i;

            if(e.mDst->isFloat())
            {
                const NIIf * pSrc = (const NIIf *)mValue->getBufferData(e.mSrc->mMemIndex);
                NIIf * pDst = (NIIf *)mParams->getBufferData(e.mDst->mMemIndex);

                if(mParams->isTransposeMatrix() && e.mDst->mDataType == GDT_Matrix4X4)
                {
                    pDst[0 * 4 + 0] = pSrc[0 * 4 + 0];
                    pDst[0 * 4 + 1] = pSrc[1 * 4 + 0];
                    pDst[0 * 4 + 2] = pSrc[2 * 4 + 0];
                    pDst[0 * 4 + 3] = pSrc[3 * 4 + 0];

                    pDst[1 * 4 + 0] = pSrc[0 * 4 + 1];
                    pDst[1 * 4 + 1] = pSrc[1 * 4 + 1];
                    pDst[1 * 4 + 2] = pSrc[2 * 4 + 1];
                    pDst[1 * 4 + 3] = pSrc[3 * 4 + 1];

                    pDst[2 * 4 + 0] = pSrc[0 * 4 + 2];
                    pDst[2 * 4 + 1] = pSrc[1 * 4 + 2];
                    pDst[2 * 4 + 2] = pSrc[2 * 4 + 2];
                    pDst[2 * 4 + 3] = pSrc[3 * 4 + 2];

                    pDst[3 * 4 + 0] = pSrc[0 * 4 + 3];
                    pDst[3 * 4 + 1] = pSrc[1 * 4 + 3];
                    pDst[3 * 4 + 2] = pSrc[2 * 4 + 3];
                    pDst[3 * 4 + 3] = pSrc[3 * 4 + 3];
                    pSrc += 16;
                    pDst += 16;
                }
                else
                {
                    NCount u32size = e.mSrc->mUnit32bSize;
                    if (e.mDst->mUnit32bSize == u32size)
                    {
                        memcpy(pDst, pSrc, sizeof(NIIf) * e.mDst->mUnit32bSize * e.mDst->mUnitCount);
                    }
                    else
                    {
                        N_assert(e.mDst->mUnit32bSize % 4 == 0, "error");
                        NCount iterations = (e.mDst->mUnit32bSize / 4) * e.mDst->mUnitCount;
                        for(NCount l = 0; l < iterations; ++l)
                        {
                            memcpy(pDst, pSrc, sizeof(NIIf) * u32size);
                            pSrc += u32size;
                            pDst += 4;
                        }
                    }
                }
            }
            else if (e.mDst->isDouble())
            {
                const NIId * pSrc = (const NIId *)mValue->getBufferData(e.mSrc->mMemIndex);
                NIId * pDst = (NIId *)mParams->getBufferData(e.mDst->mMemIndex);

                if (mParams->isTransposeMatrix() && e.mDst->mDataType == GDT_Matrixd4X4)
                {
                    pDst[0 * 4 + 0] = pSrc[0 * 4 + 0];
                    pDst[0 * 4 + 1] = pSrc[1 * 4 + 0];
                    pDst[0 * 4 + 2] = pSrc[2 * 4 + 0];
                    pDst[0 * 4 + 3] = pSrc[3 * 4 + 0];

                    pDst[1 * 4 + 0] = pSrc[0 * 4 + 1];
                    pDst[1 * 4 + 1] = pSrc[1 * 4 + 1];
                    pDst[1 * 4 + 2] = pSrc[2 * 4 + 1];
                    pDst[1 * 4 + 3] = pSrc[3 * 4 + 1];

                    pDst[2 * 4 + 0] = pSrc[0 * 4 + 2];
                    pDst[2 * 4 + 1] = pSrc[1 * 4 + 2];
                    pDst[2 * 4 + 2] = pSrc[2 * 4 + 2];
                    pDst[2 * 4 + 3] = pSrc[3 * 4 + 2];

                    pDst[3 * 4 + 0] = pSrc[0 * 4 + 3];
                    pDst[3 * 4 + 1] = pSrc[1 * 4 + 3];
                    pDst[3 * 4 + 2] = pSrc[2 * 4 + 3];
                    pDst[3 * 4 + 3] = pSrc[3 * 4 + 3];
                    pSrc += 16;
                    pDst += 16;
                }
                else
                {
                    if (e.mDst->mUnit32bSize == e.mSrc->mUnit32bSize)
                    {
                        memcpy(pDst, pSrc, sizeof(NIIf) * e.mDst->mUnit32bSize * e.mDst->mUnitCount);
                    }
                    else
                    {
                        N_assert(e.mDst->mUnit32bSize % 8 == 0, "error");
                        NCount iterations = (e.mDst->mUnit32bSize / 8) * e.mDst->mUnitCount;
                        NCount u32size = e.mSrc->mUnit32bSize / 2;
                        for (NCount l = 0; l < iterations; ++l)
                        {
                            memcpy(pDst, pSrc, sizeof(NIId) * u32size);
                            pSrc += u32size;
                            pDst += 4;
                        }
                    }
                }
            }
            else if(e.mDst->isInt() || e.mDst->isSampler() || e.mDst->isBlock() || e.mDst->isUInt() || e.mDst->isBool())
            {
                const Nui32 * pSrc = (const Nui32 *)mValue->getBufferData(e.mSrc->mMemIndex);
                Nui32 * pDst = (Nui32 *)mParams->getBufferData(e.mDst->mMemIndex);

                if (e.mDst->mUnit32bSize == e.mSrc->mUnit32bSize)
                {
                    memcpy(pDst, pSrc, sizeof(Nui32) * e.mDst->mUnit32bSize * e.mDst->mUnitCount);
                }
                else
                {
                    N_assert(e.mDst->mUnit32bSize % 4 == 0, "error");
                    NCount iterations = (e.mDst->mUnit32bSize / 4) * e.mDst->mUnitCount;
                    NCount u32size = e.mSrc->mUnit32bSize;
                    for (NCount l = 0; l < iterations; ++l)
                    {
                        memcpy(pDst, pSrc, sizeof(Nui32) * u32size);
                        pSrc += u32size;
                        pDst += 4;
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GpuProgramParam 方法
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GpuProgramParam::GpuProgramParam() :
        mParamGBTMark(GPT_Render),
        mTransposeMatrix(false) ,
        mAllowParamLost(false),
        mRenderCountPtr(0)
    {
        mBitSet.resize(128);
    }
    //-----------------------------------------------------------------------------
    GpuProgramParam::~GpuProgramParam()
    {
        removeAllValue();
    }
    //-----------------------------------------------------------------------------
    GpuProgramParam::GpuProgramParam(const GpuProgramParam & o)
    {
        *this = o;
    }
    //-----------------------------------------------------------------------------
    GpuProgramParam & GpuProgramParam::operator=(const GpuProgramParam & o)
    {
        mBufferData = o.mBufferData;
        mSyncBlockList = o.mSyncBlockList;
        mBlockList = o.mBlockList;
        mParamDefine = o.mParamDefine;

        ShareSyncList::const_iterator i, iend = mShareSyncList.end();
        for(i = mShareSyncList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mShareSyncList.clear();
        ShareSyncList::const_iterator j, jend = o.mShareSyncList.end();
        for(j = o.mShareSyncList.begin(); j != iend; ++j)
        {
            mShareSyncList.push_back(N_new ParamValueSync((*j)->getValue(), this));
        }

        mParamGBTMark = o.mParamGBTMark;
        mTransposeMatrix = o.mTransposeMatrix;
        mAllowParamLost  = o.mAllowParamLost;
        *mRenderCountPtr = *(o.mRenderCountPtr);
        return *this;
    }
    //-------------------------------------------------------------------------
    void GpuProgramParam::setDefine(GpuParamDefine * def)
    {
        mParamDefine = def;

        GpuParamUnitList::const_iterator i, iend = mParamDefine->getList().end();
        for (i = mParamDefine->getList().begin(); i != iend; ++i)
        {
            const VString & name = i->first;
            const GpuParamUnit & def = i->second;
            // 只考虑非阵列条目
            if (name.find("[") == VString::npos && name.find("]") == VString::npos)
            {
                mBlockList.insert(Npair(def.mIndex, GpuParamBlock(def.mIndex, def.mMemIndex, def.mDataType, def.mUnitCount * def.mUnit32bSize, def.mGBTMark)));
            }
        }
        if (mParamDefine->getSize() > mBufferData.size())
        {
            mBufferData.resize(mParamDefine->getSize() - mBufferData.size());
        }
    }
    //----------------------------------------------------------------------------
    void GpuProgramParam::define(Nui32 index, Nui32 gbtMark, GpuDataType dataType, NCount cnt)
    {
        getBlock(index, cnt * GpuProgramParam::get32bCount(dataType, false), gbtMark, dataType);
    }
    //----------------------------------------------------------------------------
    void GpuProgramParam::undefine(Nui32 index)
    {
        GpuParamBlockMap::iterator logi = mBlockList.find(index);
        if(logi != mBlockList.end())
        {
            const GpuParamBlock * block = &(logi->second);
            Nui32 eraseCnt = block->m32bSize;
            Nidx memidx = block->mMemIndex;
            mBlockList.erase(logi);
            if(eraseCnt)
            {
                GBufferArray::iterator erasePos = mBufferData.begin();
                std::advance(erasePos, memidx);
                mBufferData.erase(erasePos, erasePos + eraseCnt);
            
                GpuParamBlockMap::iterator i, iend = mBlockList.end();
                for(i = mBlockList.begin(); i != iend; ++i)
                {
                    if (i->second.mMemIndex > memidx)
                        i->second.mMemIndex -= eraseCnt;
                }

                if(mParamDefine)
                {
                    GpuParamUnitList::iterator z, zend = mParamDefine->mDefineList.end();
                    for(z = mParamDefine->mDefineList.begin(); z != zend; ++z)
                    {
                        if(z->second.mMemIndex > memidx)
                            z->second.mMemIndex -= eraseCnt;
                    }
                    mParamDefine->m32bSize -= eraseCnt;
                }
            }
        }
    }
    //----------------------------------------------------------------------------
    void GpuProgramParam::set(Nui32 index, NCount oft32b, const Matrix4f & in)
    {
        if(mTransposeMatrix)
        {
            Matrix4f t = in.T();
            set(index, oft32b, t[0], 4);
        }
        else
        {
            set(index, oft32b, in[0], 4);
        }
    }
    //-----------------------------------------------------------------------------
    void GpuProgramParam::set(Nui32 index, NCount oft32b, const Matrix4f * in, NCount cnt)
    {
        if(mTransposeMatrix)
        {
            for(NCount i = 0; i < cnt; ++i)
            {
                Matrix4f t = in[i].T();
                set(index, oft32b, t[0], 4);
                index += 4;
            }
        }
        else
        {
            set(index, oft32b, in[0][0], 4 * cnt);
        }
    }
    //-----------------------------------------------------------------------------
    void GpuProgramParam::set(Nui32 index, NCount oft32b, const Ni32 * in, NCount cnt4x)
    {
        NCount rawCount = cnt4x * 4;
        GpuParamBlock * bind = getBlock(index, rawCount, GPT_Render, GDT_Int);
        _write(bind ? bind->mMemIndex + oft32b : oft32b, in, rawCount);
    }
    //-------------------------------------------------------------------------
    void GpuProgramParam::set(Nui32 index, NCount oft32b, const NIIf * in, NCount cnt4x)
    {
        NCount rawCount = cnt4x * 4;
        GpuParamBlock * bind = getBlock(index, rawCount, GPT_Render, GDT_Float);
        _write(bind ? bind->mMemIndex + oft32b : oft32b, in, rawCount);
    }
    //-------------------------------------------------------------------------
    void GpuProgramParam::set(Nui32 index, NCount oft32b, const NIId * in, NCount cnt4x)
    {
        NCount rawCount = cnt4x * 8;
        GpuParamBlock * bind = getBlock(index, rawCount, GPT_Render, GDT_Double);
        _write(bind ? bind->mMemIndex + oft32b : oft32b, in, rawCount);
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::setExtData(Nui32 index, Ni32 ext)
    {
        ExtDataList::iterator i = mExtDataList.find(index);
        if (i != mExtDataList.end())
        {
            i->second = ext;
        }
    }
    //---------------------------------------------------------------------------
    bool GpuProgramParam::getExtData(Nui32 index, Ni32 & ext) const
    {
        ExtDataList::const_iterator i = mExtDataList.find(index);
        if (i != mExtDataList.end())
        {
            ext = i->second;
            return true;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, NIIf val)
    {
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex, oft32b, val);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, Ni32 val)
    {
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex, oft32b, val);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, const Vector4f & vec)
    {
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex, oft32b, vec, def->mUnit32bSize);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, const Vector3f & vec)
    {
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex, oft32b, vec);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, const Matrix4f & m)
    {
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex, oft32b, m, def->mUnit32bSize);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, const Matrix4f * m, NCount cnt)
    {
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex, oft32b, m, cnt);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, const NIIf * val, NCount count, NCount size32b)
    {
        NCount rawCount = count * size32b;
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex + oft32b, val, rawCount);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, const NIId * val, NCount count, NCount size64b)
    {
        NCount rawCount = count * size64b * 2;
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex + oft32b, val, rawCount);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, const Ni32 * val, NCount count, NCount size32b)
    {
        NCount rawCount = count * size32b;
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex + oft32b, val, rawCount);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, NCount oft32b, const Colour & colour)
    {
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        const GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            _write(def->mMemIndex, oft32b, colour, def->mUnit32bSize);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void GpuProgramParam::_write(Nidx memidx, NCount oft32b, const Matrix4f & in, NCount size32b)
    {
        if(mTransposeMatrix)
        {
            Matrix4f t = in.T();
            _write(memidx + oft32b, t[0], size32b > 16 ? 16 : size32b);
        }
        else
        {
            _write(memidx + oft32b, in[0], size32b > 16 ? 16 : size32b);
        }
    }
    //-----------------------------------------------------------------------------
    void GpuProgramParam::_write(Nidx memidx, NCount oft32b, const Matrix4f * in, NCount cnt)
    {
        if(mTransposeMatrix)
        {
            for(NCount i = 0; i < cnt; ++i)
            {
                Matrix4f t = in[i].T();
                _write(memidx + oft32b, t[0], 16);
                memidx += 16;
            }
        }
        else
        {
            _write(memidx + oft32b, in[0][0], 16 * cnt);
        }
    }
    //-----------------------------------------------------------------------------
    Nui32 GpuProgramParam::getGBTMark(GpuSyncParam gsp)
    {
        switch(gsp)
        {
        case GSP_Matrix_View:
        case GSP_Matrix_View_Inverse:
        case GSP_Matrix_View_Transpose:
        case GSP_Matrix_View_InverseTranspose:
        case GSP_Matrix_Proj:
        case GSP_Matrix_Proj_Inverse:
        case GSP_Matrix_Proj_Transpose:
        case GSP_Matrix_Proj_InverseTranspose:
        case GSP_Matrix_Proj_X_View:
        case GSP_Matrix_Proj_X_View_Inverse:
        case GSP_Matrix_Proj_X_View_Transpose:
        case GSP_Matrix_Proj_X_View_InverseTranspose:
        case GSP_View_PixelSize:
        case GSP_View_PixelWidth:
        case GSP_View_PixelHeight:
        case GSP_Camera_Dir:
        case GSP_Camera_Right:
        case GSP_Camera_Up:
        case GSP_Camera_FOV:
        case GSP_Camera_Pos:
        case GSP_Camera_LOD_Pos:
            return GPT_View_Param;
        case GSP_Ch_Emissive_Evn:
        case GSP_Ch_Ambient:
        case GSP_Ch_Diffuse:
        case GSP_Ch_Specular:
        case GSP_Ch_Emissive:
        case GSP_Ch_Shininess:
        case GSP_Ambient:
        case GSP_Ch_Ambient_X_Env:
            return GPT_Colour_Param;
        case GSP_Texture_Dim_ParamIndex:
        case GSP_Texel_Oft:
        case GSP_Texture_InverseDim_ParamIndex:
        case GSP_Texture_DimUnit_ParamIndex:
        case GSP_Texture_Matrix_Transform_ParamIndex:
            return GPT_Texture_Param;
        //case GSP_Fog_Colour:
        case GSP_Fog_Params:
            return GPT_Fog_Param;
        case GSP_FPS:
            return GPT_Fps_Param;
        case GSP_Sys_CostTime:
        case GSP_Sys_CostTime_Fmod_ParamData:
        case GSP_Frame_CostTime:
        case GSP_Vertex_Winding:
        case GSP_Texture_Flipping:
        case GSP_Ch_Index:
        case GSP_Ch_Depth_Range:
        case GSP_Camera_NearClip:
        case GSP_Camera_FarClip:
            return GPT_Other_Param;
        case GSP_Matrix_Space:
        case GSP_Matrix_Space_Inverse:
        case GSP_Matrix_Space_Transpose:
        case GSP_Matrix_Space_InverseTranspose:
        case GSP_Matrix3x4_Space_Array:
        case GSP_Matrix4x4_Space_Array:
        case GSP_DualQuaternion_Space:
        case GSP_Matrix3x4_Space_Scale_Shear:
        case GSP_Matrix_View_X_Space:
        case GSP_Matrix_View_X_Space_Inverse:
        case GSP_Matrix_View_X_Space_Transpose:
        case GSP_Matrix_View_X_Space_InverseTranspose:
        case GSP_Matrix_Proj_X_View_X_Space:
        case GSP_Matrix_Proj_X_View_X_Space_Inverse:
        case GSP_Matrix_Proj_X_View_X_Space_Transpose:
        case GSP_Matrix_Proj_X_View_X_Space_InverseTranspose:
        case GSP_Camera_Pos_ChSpace:
        case GSP_Camera_LOD_Pos_ChSpace:
        case GSP_Ch_Custom:
        case GSP_Ch_AniParam:
            return GPT_Space_Param;
        case GSP_Light_Pos_ChSpace_ParamIndex:
        case GSP_Light_Dir_ChSpace_ParamIndex:
        case GSP_Light_Dis_ChSpace_ParamIndex:
        case GSP_Light_Pos_ChSpace_ParamCount:
        case GSP_Light_Dir_ChSpace_ParamCount:
        case GSP_Light_Dis_ChSpace_ParamCount:
        case GSP_Texture_Matrix_Proj_X_View_X_Space_ParamIndex:
        case GSP_Texture_Matrix_Proj_X_View_X_Space_ParamCount:
        case GSP_Light_Spot_Matrix_Proj_X_View_X_Space:
            return (GPT_Space_Param | GPT_Light_Param);
        case GSP_Light_ActiveCount:
        case GSP_Light_Diffuse_ParamIndex:
        case GSP_Light_Specular_ParamIndex:
        case GSP_Light_Pos_ParamIndex:
        case GSP_Light_Dir_ParamIndex:
        case GSP_Light_Pos_ChView_ParamIndex:
        case GSP_Light_Dir_ChView_ParamIndex:
        case GSP_Shadow_VolExtent:
        case GSP_Shadow_ChDepthRange_ParamIndex:
        case GSP_Shadow_Colour:
        case GSP_Light_BrightFactor_ParamIndex:
        case GSP_Light_Index_ParamIndex:
        case GSP_Light_CastShadow_ParamIndex:
        case GSP_Light_Attenuation_ParamIndex:
        case GSP_Light_Spot_Params_ParamIndex:
        case GSP_Light_Diffuse_ParamCount:
        case GSP_Light_Specular_ParamCount:
        case GSP_Light_Pos_ParamCount:
        case GSP_Light_Dir_ParamCount:
        case GSP_Light_Pos_ChView_ParamCount:
        case GSP_Light_Dir_ChView_ParamCount:
        case GSP_Light_BrightFactor_ParamCount:
        case GSP_Light_Attenuation_ParamCount:
        case GSP_Light_Spot_Params_ParamCount:
        case GSP_Texture_Matrix_Proj_X_View_ParamIndex:
        case GSP_Texture_Matrix_Proj_X_View_ParamCount:
        case GSP_Light_Spot_Matrix_Proj_X_View_ParamIndex:
        case GSP_Light_Spot_Matrix_Proj_X_View_ParamCount:
        case GSP_Light_Custom:
            return GPT_Light_Param;
        case GSP_Light_Diffuse_Ch_ParamIndex:
        case GSP_Light_Specular_Ch_ParamIndex:
        case GSP_Light_Diffuse_Ch_ParamCount:
        case GSP_Light_Specular_Ch_ParamCount:
            return (GPT_Render | GPT_Light_Param);
        case GSP_Ch_RenderCount:
            return GPT_ShaderCh_Param;
        default:
            return GPT_Render;
        };
    }
    //---------------------------------------------------------------------
    GpuParamBlock * GpuProgramParam::getBlock(Nui32 index, NCount size32b, Nui32 typemark, GpuDataType dtype, bool reduce)
    {
        GpuParamBlock * block = 0;
        Nidx memidx;
        GpuParamBlockMap::iterator logi = mBlockList.find(index);
        if(logi == mBlockList.end())
        {
            if(size32b)
            {
                NCount teccn = mBitSet.getMostSignificant(index);
                if(teccn == -1)
                {
                    memidx = 0;
                }
                else
                {
                    GpuParamBlockMap::iterator logi2 = mBlockList.find(teccn);
                    N_assert(logi2 != mBlockList.end(), "error");
                    block = &(logi2->second);
                    
                    memidx = block->mMemIndex + block->m32bSize;
                }

                GBufferArray::iterator insertPos = mBufferData.begin();
                std::advance(insertPos, memidx);
                mBufferData.insert(insertPos, size32b, 0.0f);
                
                GpuParamBlockMap::iterator i, iend = mBlockList.end();
                for(i = mBlockList.begin(); i != iend; ++i)
                {
                    if (i->second.mMemIndex >= memidx)
                        i->second.mMemIndex += size32b;
                }
                
                if(mParamDefine)
                {
                    GpuParamUnitList::iterator z, zend = mParamDefine->mDefineList.end();
                    for(z = mParamDefine->mDefineList.begin(); z != zend; ++z)
                    {
                        if(z->second.mMemIndex >= memidx)
                            z->second.mMemIndex += size32b;
                    }
                    mParamDefine->m32bSize += size32b;
                }
   
                i = mBlockList.insert(Npair(index, GpuParamBlock(index, memidx, dtype, size32b, typemark))).first;

                mBitSet.setTrue(index);

                block = &(i->second);
            }
            else
            {
                return 0;
            }
        }
        else
        {
            block = &(logi->second);

            if (block->m32bSize < size32b)
            {
                size32b -= block->m32bSize;
                GBufferArray::iterator insertPos = mBufferData.begin();
                memidx = block->mMemIndex;
                std::advance(insertPos, block->mMemIndex + block->m32bSize);
                mBufferData.insert(insertPos, size32b, 0.0f);
                block->m32bSize += size32b;

                GpuParamBlockMap::iterator i, iend = mBlockList.end();
                for(i = mBlockList.begin(); i != iend; ++i)
                {
                    if (i->second.mMemIndex > memidx)
                        i->second.mMemIndex += size32b;
                }
                
                if(mParamDefine)
                {
                    GpuParamUnitList::iterator z, zend = mParamDefine->mDefineList.end();
                    for(z = mParamDefine->mDefineList.begin(); z != zend; ++z)
                    {
                        if(z->second.mMemIndex > memidx)
                            z->second.mMemIndex += size32b;
                    }
                    mParamDefine->m32bSize += size32b;
                }
            }
            else
            {
                Ni32 tsize32b = block->m32bSize - size32b;
                if(reduce & tsize32b)
                {
                    GBufferArray::iterator erasePos = mBufferData.begin();
                    memidx = block->mMemIndex + block->m32bSize - tsize32b;
                    std::advance(erasePos, memidx);
                    mBufferData.erase(erasePos, tsize32b);
                    block->m32bSize -= tsize32b;

                    GpuParamBlockMap::iterator i, iend = mBlockList.end();
                    for(i = mBlockList.begin(); i != iend; ++i)
                    {
                        if (i->second.mMemIndex > memidx)
                            i->second.mMemIndex -= tsize32b;
                    }
                    
                    if(mParamDefine)
                    {
                        GpuParamUnitList::iterator z, zend = mParamDefine->mDefineList.end();
                        for(z = mParamDefine->mDefineList.begin(); z != zend; ++z)
                        {
                            if(z->second.mMemIndex > memidx)
                                z->second.mMemIndex -= tsize32b;
                        }
                        mParamDefine->m32bSize -= tsize32b;
                    }
                }
                size32b = 0;
            }
        }

        if(block)
        {
            block->mDataType = dtype;
            block->mGBTMark = typemark;
        }
        return block;
    }
    //--------------------------------------------------------------------------
    void GpuProgramParam::set(GpuParamBlock * block, GpuSyncParam gsp, Ni32 ext)
    {
        mParamGBTMark |= block->mGBTMark;
        block->mSyncParam = gsp;
        block->mInputInt = ext;
        GpuParamBlockList::iterator i, iend = mSyncBlockList.end();
        for(i = mSyncBlockList.begin(); i != iend; ++i)
        {
            if(*i == block)
            {
                //*i = block;
                return;
            }
        }

        mSyncBlockList.push_back(block);
    }
    //--------------------------------------------------------------------------
    void GpuProgramParam::set(GpuParamBlock * block, GpuSyncParam gsp, NIIf ext)
    {
        mParamGBTMark |= block->mGBTMark;
        block->mSyncParam = gsp;
        block->mInputFloat = ext;
        GpuParamBlockList::iterator i, iend = mSyncBlockList.end();
        for(i = mSyncBlockList.begin(); i != iend; ++i)
        {
            if(*i == block)
            {
                //*i = block;
                return;
            }
        }

        mSyncBlockList.push_back(block);
    }
    //--------------------------------------------------------------------------
    Nui32 GpuProgramParam::getLocation(Nidx memidx) const
    {
        GpuParamBlockMap::const_iterator i, iend = mBlockList.end();
        for(i = mBlockList.begin(); i != iend; ++i)
        {
            if(i->second.mMemIndex == memidx)
                return i->first;
        }
        return N_TYPE_MAX(Nidx);
    }
    //--------------------------------------------------------------------------
    const GpuParamUnit & GpuProgramParam::getUnit(const VString & name) const
    {
        N_assert(mParamDefine != 0, _I18n("参数定义体没有定义"));

        const GpuParamUnit * def = mParamDefine->get(name);
        if (def == 0)
        {
            return GpuParamUnit::UnitNone;
        }
        return *def;
    }
    //--------------------------------------------------------------------------
    void GpuProgramParam::set(Nui32 index, GpuSyncParam gsp, NIIf extraInfo)
    {
        const GpuParamUnit * def = GpuProgramManager::getSyncParam(gsp);
        if(def)
        {
            NCount cnt = def->mUnitCount;
            if (cnt % 4 > 0)
            {
                cnt += 4 - (cnt % 4);
            }
			cnt = def->isDouble() ? cnt * 2 : cnt;
            GpuParamBlock * block = getBlock(index, cnt, getGBTMark(gsp), (GpuDataType)def->mDataType);
            if(block)
                set(block, gsp, extraInfo);
        }
    }
    //-----------------------------------------------------------------------------
    void GpuProgramParam::set(Nui32 index, GpuSyncParam gsp, Ni32 extraInfo)
    {
        const GpuParamUnit * def = GpuProgramManager::getSyncParam(gsp);
        if(def)
        {
            NCount cnt = def->mUnitCount;
            if(cnt % 4 > 0)
            {
                cnt += 4 - (cnt % 4);
            }
			cnt = def->isDouble() ? cnt * 2 : cnt;
            GpuParamBlock * block = getBlock(index, cnt, getGBTMark(gsp), (GpuDataType)def->mDataType);
            if(block)
                set(block, gsp, extraInfo);
        }
    }
    //-----------------------------------------------------------------------------
    void GpuProgramParam::_set(Nidx memidx, GpuSyncParam gsp, Ni32 ext, Nui32 gbtMark, NCount blocksize)
    {
        mParamGBTMark |= gbtMark;
        GpuParamBlockList::iterator i, iend = mSyncBlockList.end();
        for(i = mSyncBlockList.begin(); i != iend; ++i)
        {
            GpuParamBlock * gpb = *i;
            if(gpb->mMemIndex == memidx)
            {
                GpuParamBlock * tgpb = getBlock(gpb->mIndex, blocksize, gbtMark, GDT_Float, true);
                N_assert1(gpb == tgpb);
                tgpb->mSyncParam = gsp;
                tgpb->mInputInt = ext;
                return;
            }
        }

        //mSyncBlockList.push_back(GpuParamBlock(memidx, gsp, ext, blocksize, gbtMark));
    }
    //----------------------------------------------------------------------------
    void GpuProgramParam::_set(Nidx memidx, GpuSyncParam gsp, NIIf ext, Nui32 gbtMark, NCount blocksize)
    {
        mParamGBTMark |= gbtMark;
        GpuParamBlockList::iterator i, iend = mSyncBlockList.end();
        for(i = mSyncBlockList.begin(); i != iend; ++i)
        {
            GpuParamBlock * gpb = *i;
            if(gpb->mMemIndex == memidx)
            {
                GpuParamBlock * tgpb = getBlock(gpb->mIndex, blocksize, gbtMark, GDT_Float, true);
                N_assert1(gpb == tgpb);
                tgpb->mSyncParam = gsp;
                tgpb->mInputFloat = ext;
                return;
            }
        }

        //mSyncBlockList.push_back(GpuParamBlock(memidx, gsp, ext, chunksize, typemark));
    }
    //-------------------------------------------------------------------------
    void GpuProgramParam::removeSyncParam(Nidx index)
    {
        GpuParamBlock * gpb = getBlock(index, 0, GPT_Render, GDT_Float);

        if(gpb)
        {
            gpb->mGBTMark = GPT_Render;
            NCount memidx = gpb->mMemIndex;

            GpuParamBlockList::iterator i, iend = mSyncBlockList.end();
            for(i = mSyncBlockList.begin(); i != iend; ++i)
            {
                if((*i)->mMemIndex == memidx)
                {
                    mSyncBlockList.erase(i);
                    undefine(index);
                    
                    break;
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void GpuProgramParam::removeSyncParam(const VString & name)
    {
        if (mParamDefine)
        {
            GpuParamUnit * def = mParamDefine->get(name);
            if (def)
            {
                def->mGBTMark = GPT_Render;
                NCount memidx = def->mMemIndex;

                GpuParamBlockList::iterator i, iend = mSyncBlockList.end();
                for (i = mSyncBlockList.begin(); i != iend; ++i)
                {
                    if ((*i)->mMemIndex == memidx)
                    {
                        mSyncBlockList.erase(i);
                        undefine(def->mIndex);
                        break;
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void GpuProgramParam::removeAllSyncParam()
    {
        GpuParamBlockList::const_iterator i, iend = mSyncBlockList.end();
        for(i = mSyncBlockList.begin(); i != iend; ++i)
        {
            undefine((*i)->mMemIndex);
            //const GpuParamBlock * gpb = *i;
            //Nidx eraseCnt = gpb->m32bSize;
            /*if(eraseCnt)
            {
                Nidx memidx = gpb->mMemIndex;
                GBufferArray::iterator erasePos = mBufferData.begin();
                std::advance(erasePos, memidx);
                mBufferData.erase(erasePos, erasePos + eraseCnt);
                GpuParamBlockMap::iterator i, iend = mBlockList.end();
                for(i = mBlockList.begin(); i != iend; ++i)
                {
                    if (i->second.mMemIndex > memidx)
                        i->second.mMemIndex -= eraseCnt;
                }

                if(mParamDefine)
                {
                    GpuParamUnitList::iterator z, zend = mParamDefine->mDefineList.end();
                    for(z = mParamDefine->mDefineList.begin(); z != zend; ++z)
                    {
                        if(z->second.mMemIndex > memidx)
                            z->second.mMemIndex -= eraseCnt;
                    }
                    mParamDefine->m32bSize -= eraseCnt;
                }
            }*/
        }
        mSyncBlockList.clear();
        mParamGBTMark = GPT_Render;
    }
    //-----------------------------------------------------------------------------
    void GpuProgramParam::sync(const SysSyncParam * source, Nmark mask)
    {
        if(mSyncBlockList.empty() == true || !(mask & mParamGBTMark))
            return;

        NCount index;
        NCount numMatrices;
        Matrix3f m3;
        Matrix4f scaleM;
        DualQuaternion dQuat;

        mRenderCountPtr = 0;
        GpuParamBlockList::const_iterator i, iend = mSyncBlockList.end();
        for(i = mSyncBlockList.begin(); i != iend; ++i)
        {
            const GpuParamBlock * gpb = *i;
            if(gpb->mGBTMark & mask)
            {
                switch(gpb->mSyncParam)
                {
                case GSP_Matrix_View:
                    _write(gpb->mMemIndex, 0, source->getViewMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_View_Inverse:
                    _write(gpb->mMemIndex, 0, source->getInverseViewMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_View_Transpose:
                    _write(gpb->mMemIndex, 0, source->getTransposeViewMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_View_InverseTranspose:
                    _write(gpb->mMemIndex, 0, source->getInverseTransposeViewMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj:
                    _write(gpb->mMemIndex, 0, source->getProjMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj_Inverse:
                    _write(gpb->mMemIndex, 0, source->getInverseProjectionMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj_Transpose:
                    _write(gpb->mMemIndex, 0, source->getTransposeProjectionMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj_InverseTranspose:
                    _write(gpb->mMemIndex, 0, source->getInverseTransposeProjectionMatrix(), gpb->m32bSize);
                    break;

                case GSP_Matrix_Proj_X_View:
                    _write(gpb->mMemIndex, 0, source->getViewProjectionMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj_X_View_Inverse:
                    _write(gpb->mMemIndex, 0, source->getInverseViewProjMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj_X_View_Transpose:
                    _write(gpb->mMemIndex, 0, source->getTransposeViewProjMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj_X_View_InverseTranspose:
                    _write(gpb->mMemIndex, 0, source->getInverseTransposeViewProjMatrix(), gpb->m32bSize);
                    break;
                case GSP_Texture_Flipping:
                    _write(gpb->mMemIndex, 0, source->getCurrentFBO()->isFlipping() ? -1.f : 1.f);
                    break;
                case GSP_Vertex_Winding:
                    _write(gpb->mMemIndex, 0, N_Engine().getRender()->isInvertVertexWinding() ? -1.f : +1.f);
                    break;
                    // 注意,环境光还在这里,因为它与特定的光无相关
                case GSP_Ambient:
                    _write(gpb->mMemIndex, 0, source->getAmbient(), gpb->m32bSize);
                    break;
                case GSP_Ch_Ambient_X_Env:
                    _write(gpb->mMemIndex, 0, source->getEnvAmbient(), gpb->m32bSize);
                    break;
                case GSP_Ch_Emissive_Evn:
                    _write(gpb->mMemIndex, 0, source->getSceneColour(), gpb->m32bSize);
                    break;
                //case GSP_Fog_Colour:
                //    _write(gpb->mMemIndex, source->getFogColour());
                //    break;
                case GSP_Fog_Params:
                {
                    _write(gpb->mMemIndex, 0, source->getFogParams(), gpb->m32bSize - 4);
                    _write(gpb->mMemIndex + 4, 0, source->getFogColour());
                }
                    break;
                case GSP_Ponit_Params:
                    _write(gpb->mMemIndex, 0, source->getPointParams(), gpb->m32bSize);
                    break;
                case GSP_Ch_Ambient:
                    _write(gpb->mMemIndex, 0, source->getChAmbient(), gpb->m32bSize);
                    break;
                case GSP_Ch_Diffuse:
                    _write(gpb->mMemIndex, 0, source->getChDiffuse(), gpb->m32bSize);
                    break;
                case GSP_Ch_Specular:
                    _write(gpb->mMemIndex, 0, source->getChSpecular(), gpb->m32bSize);
                    break;
                case GSP_Ch_Emissive:
                    _write(gpb->mMemIndex, 0, source->getChEmissive(), gpb->m32bSize);
                    break;
                case GSP_Ch_Shininess:
                    _write(gpb->mMemIndex, 0, source->getChShininess());
                    break;
                case GSP_Camera_Pos:
                    _write(gpb->mMemIndex, 0, source->getCameraPosition(), gpb->m32bSize);
                    break;
                case GSP_Sys_CostTime:
                    _write(gpb->mMemIndex, 0, source->getTime() * gpb->mInputFloat);
                    break;
                case GSP_Sys_CostTime_Fmod_ParamData:
                    _write(gpb->mMemIndex, 0, (Ni32)source->getTime_0_X(gpb->mInputInt));
                    break;
                case GSP_Frame_CostTime:
                    _write(gpb->mMemIndex, 0, source->getFrameTime() * gpb->mInputFloat);
                    break;
                case GSP_FPS:
                    _write(gpb->mMemIndex, 0, source->getFPS());
                    break;
                case GSP_View_PixelWidth:
                    _write(gpb->mMemIndex, 0, source->getViewportWidth());
                    break;
                case GSP_View_PixelHeight:
                    _write(gpb->mMemIndex, 0, source->getViewportHeight());
                    break;
                case GSP_View_PixelSize:
                    _write(gpb->mMemIndex, 0, Vector4f(
                        source->getViewportWidth(),
                        source->getViewportHeight(),
                        source->getInverseViewportWidth(),
                        source->getInverseViewportHeight()), gpb->m32bSize);
                    break;
                case GSP_Texel_Oft:
                    {
                        RenderSys * rsys = N_Engine().getRender();
                        _write(gpb->mMemIndex, 0, Vector4f(
                            rsys->getTexelXOffset(),
                            rsys->getTexelYOffset(),
                            rsys->getTexelXOffset() * source->getInverseViewportWidth(),
                            rsys->getTexelYOffset() * source->getInverseViewportHeight()),
                            gpb->m32bSize);
                    }
                    break;
                case GSP_Texture_Dim_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getTextureSize(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Texture_InverseDim_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getInverseTextureSize(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Texture_DimUnit_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getPackedTextureSize(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Ch_Depth_Range:
                    _write(gpb->mMemIndex, 0, source->getSceneDepthRange(), gpb->m32bSize);
                    break;
                case GSP_Camera_Dir:
                    _write(gpb->mMemIndex, 0, source->getViewDirection());
                    break;
                case GSP_Camera_Right:
                    _write(gpb->mMemIndex, 0, source->getViewSideVector());
                    break;
                case GSP_Camera_Up:
                    _write(gpb->mMemIndex, 0, source->getViewUpVector());
                    break;
                case GSP_Camera_FOV:
                    _write(gpb->mMemIndex, 0, source->getFOV());
                    break;
                case GSP_Camera_NearClip:
                    _write(gpb->mMemIndex, 0, source->getNearClipDistance());
                    break;
                case GSP_Camera_FarClip:
                    _write(gpb->mMemIndex, 0, source->getFarClipDistance());
                    break;
                case GSP_Ch_Index:
                    _write(gpb->mMemIndex, 0, (NIIf)source->getPassNumber());
                    break;
                case GSP_Ch_RenderCount:
                    _write(gpb->mMemIndex, 0, 0.0f);
                    mRenderCountPtr = &mBufferData[gpb->mMemIndex];
                    break;
                case GSP_Texture_Matrix_Transform_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getTextureTransformMatrix(gpb->mInputInt),gpb->m32bSize);
                    break;
                case GSP_Camera_LOD_Pos:
                    _write(gpb->mMemIndex, 0, source->getLodCameraPosition(), gpb->m32bSize);
                    break;
                case GSP_Texture_Matrix_Proj_X_View_X_Space_ParamIndex:
                    // 也可以在灯光中更新
                    _write(gpb->mMemIndex, 0, source->getTextureWorldViewProjMatrix(gpb->mInputInt),gpb->m32bSize);
                    break;
                case GSP_Texture_Matrix_Proj_X_View_X_Space_ParamCount:
                    for(Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        // 也可以在灯光中更新
                        _write(gpb->mMemIndex + l * gpb->m32bSize, 0, source->getTextureWorldViewProjMatrix(l), gpb->m32bSize);
                    }
                    break;
                case GSP_Light_Spot_Matrix_Proj_X_View_X_Space:
                    _write(gpb->mMemIndex, 0, source->getSpotlightWorldViewProjMatrix(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Light_Pos_ChSpace_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getInverseWorldMatrix().affine(
                        source->getLightRelSpace4(gpb->mInputInt)), gpb->m32bSize);
                    break;
                case GSP_Light_Dir_ChSpace_ParamIndex:
                {
                    // 我们需要相反的逆转置
                    m3 = source->getInverseTransposeWorldMatrix().inverse();
                    Vector3f vec3 = m3 * source->getLightDirection(gpb->mInputInt);
                    vec3.normalise();
                    // 为兼容性,设为四维向量
                    _write(gpb->mMemIndex, 0, Vector4f(vec3.x, vec3.y, vec3.z, 0.0f), gpb->m32bSize);
                }
                    break;
                case GSP_Light_Dis_ChSpace_ParamIndex:
                {
                    Vector3f vec3 = source->getInverseWorldMatrix().affine(source->getLightPosition(gpb->mInputInt));
                    _write(gpb->mMemIndex, 0, vec3.length());
                }
                    break;
                case GSP_Light_Pos_ChSpace_ParamCount:
                    for(Ni32 l = 0; l < gpb->mInputInt; ++l)
                        _write(gpb->mMemIndex + l * gpb->m32bSize, 0, source->getInverseWorldMatrix().affine(
                            source->getLightRelSpace4(l)), gpb->m32bSize);
                    break;
                case GSP_Light_Dir_ChSpace_ParamCount:
                {
                    // 我们需要相反的逆转置
                    Vector3f vec3;
                    m3 = source->getInverseTransposeWorldMatrix().inverse();
                    for (Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        vec3 = m3 * source->getLightDirection(l);
                        vec3.normalise();
                        _write(gpb->mMemIndex + l * gpb->m32bSize, 0, Vector4f(vec3.x, vec3.y, vec3.z, 0.0f), gpb->m32bSize);
                    }
                }
                    break;

                case GSP_Light_Dis_ChSpace_ParamCount:
                {
                    Vector3f vec3;
                    for (Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        vec3 = source->getInverseWorldMatrix().affine(source->getLightPosition(l));
                        _write(gpb->mMemIndex + l * gpb->m32bSize, 0, vec3.length());
                    }
                }
                    break;

                case GSP_Matrix_Space:
                    _write(gpb->mMemIndex, 0, source->getWorldMatrix(), gpb->m32bSize);
                    break;
                case GSP_Matrix_Space_Inverse:
                    _write(gpb->mMemIndex, 0, source->getInverseWorldMatrix(),gpb->m32bSize);
                    break;
                case GSP_Matrix_Space_Transpose:
                    _write(gpb->mMemIndex, 0, source->getTransposeWorldMatrix(),gpb->m32bSize);
                    break;
                case GSP_Matrix_Space_InverseTranspose:
                    _write(gpb->mMemIndex, 0, source->getInverseTransposeWorldMatrix(),gpb->m32bSize);
                    break;
                case GSP_Matrix3x4_Space_Array:
                {
                    // 遍历矩阵
                    const Matrix4f * pMatrix = source->getWorldMatrixArray();
                    numMatrices = source->getWorldMatrixCount();
                    index = gpb->mMemIndex;
                    for (NCount m = 0; m < numMatrices; ++m)
                    {
                        _write(index, (*pMatrix)[0], 12);
                        index += 12;
                        ++pMatrix;
                    }
                }
                    break;
                case GSP_Matrix4x4_Space_Array:
                    _write(gpb->mMemIndex, 0, source->getWorldMatrixArray(), source->getWorldMatrixCount());
                    break;
                case GSP_DualQuaternion_Space:
                {
                    // Loop over matrices
                    const Matrix4f * pMatrix = source->getWorldMatrixArray();
                    numMatrices = source->getWorldMatrixCount();
                    index = gpb->mMemIndex;
                    for (NCount m = 0; m < numMatrices; ++m)
                    {
                        dQuat.from(*pMatrix);
                        _write(index, &dQuat.w, 8);
                        index += 8;
                        ++pMatrix;
                    }
                }
                    break;
                case GSP_Matrix3x4_Space_Scale_Shear:
                {
                    // Loop over matrices
                    const Matrix4f * pMatrix = source->getWorldMatrixArray();
                    numMatrices = source->getWorldMatrixCount();
                    index = gpb->mMemIndex;

                    scaleM = Matrix4f::IDENTITY;

                    for (NCount m = 0; m < numMatrices; ++m)
                    {
                        //Based on Matrix4::decompostion, but we don't need the rotation or position components
                        //but do need the scaling and shearing. Shearing isn't available from Matrix4::get
                        N_assert((*pMatrix).isAffine(), "error");

                        m3 = *pMatrix;

                        Matrix3f matQ;
                        Vector3f scale;

                        //vecU is the scaling component with vecU[0] = u01, vecU[1] = u02, vecU[2] = u12
                        //vecU[0] is shearing (x,y), vecU[1] is shearing (x,z), and vecU[2] is shearing (y,z)
                        //The first component represents the coordinate that is being sheared,
                        //while the second component represents the coordinate which performs the shearing.
                        Vector3f vecU;
                        m3.getQDU(matQ, scale, vecU);

                        scaleM[0][0] = scale.x;
                        scaleM[1][1] = scale.y;
                        scaleM[2][2] = scale.z;

                        scaleM[0][1] = vecU[0];
                        scaleM[0][2] = vecU[1];
                        scaleM[1][2] = vecU[2];

                        _write(index, scaleM[0], 12);
                        index += 12;
                        ++pMatrix;
                    }
                }
                    break;
                case GSP_Matrix_View_X_Space:
                    _write(gpb->mMemIndex, 0, source->getWorldViewMatrix(),gpb->m32bSize);
                    break;
                case GSP_Matrix_View_X_Space_Inverse:
                    _write(gpb->mMemIndex, 0, source->getInverseWorldViewMatrix(),gpb->m32bSize);
                    break;
                case GSP_Matrix_View_X_Space_Transpose:
                    _write(gpb->mMemIndex, 0, source->getTransposeWorldViewMatrix(),gpb->m32bSize);
                    break;
                case GSP_Matrix_View_X_Space_InverseTranspose:
                    _write(gpb->mMemIndex, 0, source->getInverseTransposeWorldViewMatrix(),gpb->m32bSize);
                    break;

                case GSP_Matrix_Proj_X_View_X_Space:
                    _write(gpb->mMemIndex, 0, source->getWorldViewProjMatrix(),gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj_X_View_X_Space_Inverse:
                    _write(gpb->mMemIndex, 0, source->getInverseWorldViewProjMatrix(),gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj_X_View_X_Space_Transpose:
                    _write(gpb->mMemIndex, 0, source->getTransposeWorldViewProjMatrix(),gpb->m32bSize);
                    break;
                case GSP_Matrix_Proj_X_View_X_Space_InverseTranspose:
                    _write(gpb->mMemIndex, 0, source->getInverseTransposeWorldViewProjMatrix(),gpb->m32bSize);
                    break;
                case GSP_Camera_Pos_ChSpace:
                    _write(gpb->mMemIndex, 0, source->getCameraPositionObjectSpace(), gpb->m32bSize);
                    break;
                case GSP_Camera_LOD_Pos_ChSpace:
                    _write(gpb->mMemIndex, 0, source->getLodCameraPositionObjectSpace(), gpb->m32bSize);
                    break;

                case GSP_Ch_Custom:
                case GSP_Ch_AniParam:
                    source->getCurrentGeometryObj()->updateCustom(this, *gpb);
                    break;
                case GSP_Light_Custom:
                {
                    Nui16 lightIndex = (Nui16)(gpb->mInputInt & 0xFFFF);
                    Nui16 paramIndex = (Nui16)((gpb->mInputInt >> 16) & 0xFFFF);
                    Light *light = source->getLight(lightIndex);
                    light->updateCustom(this, paramIndex, *gpb);
                }
                    break;
                case GSP_Light_ActiveCount:
                    _write(gpb->mMemIndex, 0, source->getLightCount());
                    break;
                case GSP_Light_Diffuse_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getLightDiffuseColour(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Light_Specular_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getLightSpecularColour(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Light_Pos_ParamIndex:
                    // 作为四维向量获取,方向灯光也可以工作
                    // 使用统一槽的情况下,使用记数较小
                    _write(gpb->mMemIndex, 0, source->getLightRelSpace4(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Light_Dir_ParamIndex:
                {
                    const Vector3f & vec3 = source->getLightDirection(gpb->mInputInt);
                    // 为兼容性,设置为4D 向量
                    // 使用统一槽的情况下,使用记数较小
                    _write(gpb->mMemIndex, 0, Vector4f(vec3.x, vec3.y, vec3.z, 1.0f), gpb->m32bSize);
                }
                    break;
                case GSP_Light_Pos_ChView_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getViewMatrix().affine(source->getLightRelSpace4(gpb->mInputInt)), gpb->m32bSize);
                    break;
                case GSP_Light_Dir_ChView_ParamIndex:
                {
                    m3 = source->getInverseTransposeViewMatrix();
                    // 在缩放情况下,逆转置
                    Vector3f vec3 = m3 * source->getLightDirection(gpb->mInputInt);
                    vec3.normalise();
                    // 为兼容性,设置为4D 向量
                    _write(gpb->mMemIndex, 0, Vector4f(vec3.x, vec3.y, vec3.z, 0.0f), gpb->m32bSize);
                }
                    break;
                case GSP_Shadow_VolExtent:
                    // 挤压在物体空间,所以我们必须重新调整由世界反缩放来处理缩放对象
                    m3 = source->getWorldMatrix();
                    _write(gpb->mMemIndex, 0, source->getShadowExtrusionDistance() /
                        Math::sqrt(std::max(std::max(m3.get(0).lengthSquared(), m3.get(1).lengthSquared()), m3.get(2).lengthSquared())));
                    break;
                case GSP_Shadow_ChDepthRange_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getShadowSceneDepthRange(gpb->mInputInt));
                    break;
                case GSP_Shadow_Colour:
                    _write(gpb->mMemIndex, 0, source->getShadowColour(), gpb->m32bSize);
                    break;
                case GSP_Light_BrightFactor_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getLightPowerScale(gpb->mInputInt));
                    break;
                case GSP_Light_Index_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getLightNumber(gpb->mInputInt));
                    break;
                case GSP_Light_CastShadow_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getLightCastsShadows(gpb->mInputInt));
                    break;
                case GSP_Light_Attenuation_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getLightAttenuation(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Light_Spot_Params_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getSpotlightParams(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Light_Diffuse_ParamCount:
                    for(Ni32 l = 0; l < gpb->mInputInt; ++l)
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0, source->getLightDiffuseColour(l), gpb->m32bSize);
                    break;

                case GSP_Light_Specular_ParamCount:
                    for (Ni32 l = 0; l < gpb->mInputInt; ++l)
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0, source->getLightSpecularColour(l), gpb->m32bSize);
                    break;
                case GSP_Light_Pos_ParamCount:
                    // 作为四维向量获取,方向灯光也可以工作
                    for(Ni32 l = 0; l < gpb->mInputInt; ++l)
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0, source->getLightRelSpace4(l), gpb->m32bSize);
                    break;

                case GSP_Light_Dir_ParamCount:
                    for(Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        const Vector3f & vec3 = source->getLightDirection(l);
                        // 为兼容性,设置为四维向量
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0, Vector4f(vec3.x, vec3.y, vec3.z, 0.0f), gpb->m32bSize);
                    }
                    break;

                case GSP_Light_Pos_ChView_ParamCount:
                    for (Ni32 l = 0; l < gpb->mInputInt; ++l)
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0,
                        source->getViewMatrix().affine(source->getLightRelSpace4(l)), gpb->m32bSize);
                    break;

                case GSP_Light_Dir_ChView_ParamCount:
                {
                    m3 = source->getInverseTransposeViewMatrix();
                    Vector3f vec3;
                    for (Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        vec3 = m3 * source->getLightDirection(l);
                        vec3.normalise();
                        // 为兼容性,设置为四维向量
                        _write(gpb->mMemIndex + l * gpb->m32bSize, 0, Vector4f(vec3.x, vec3.y, vec3.z, 0.0f), gpb->m32bSize);
                    }
                }
                    break;

                case GSP_Light_BrightFactor_ParamCount:
                    for (Ni32 l = 0; l < gpb->mInputInt; ++l)
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0, source->getLightPowerScale(l));
                    break;

                case GSP_Light_Attenuation_ParamCount:
                    for (Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0, source->getLightAttenuation(l), gpb->m32bSize);
                    }
                    break;
                case GSP_Light_Spot_Params_ParamCount:
                    for (Ni32 l = 0 ; l < gpb->mInputInt; ++l)
                    {
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0, source->getSpotlightParams(l), gpb->m32bSize);
                    }
                    break;
                case GSP_Light_Diffuse_Ch_ParamIndex:
                    _write(gpb->mMemIndex, 0,
                        source->getLightDiffuseColourWithPower(gpb->mInputInt) * source->getChDiffuse(),
                        gpb->m32bSize);
                    break;
                case GSP_Light_Specular_Ch_ParamIndex:
                    _write(gpb->mMemIndex, 0,
                        source->getLightSpecularColourWithPower(gpb->mInputInt) * source->getChSpecular(),
                        gpb->m32bSize);
                    break;
                case GSP_Light_Diffuse_Ch_ParamCount:
                    for(Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0,
                            source->getLightDiffuseColourWithPower(l) * source->getChDiffuse(),
                            gpb->m32bSize);
                    }
                    break;
                case GSP_Light_Specular_Ch_ParamCount:
                    for(Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0,
                            source->getLightSpecularColourWithPower(l) * source->getChSpecular(),
                            gpb->m32bSize);
                    }
                    break;
                case GSP_Texture_Matrix_Proj_X_View_ParamIndex:
                    // 也可以在灯光中更新
                    _write(gpb->mMemIndex, 0, source->getTextureViewProjMatrix(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Texture_Matrix_Proj_X_View_ParamCount:
                    for(Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        // 也可以在灯光中更新
                        _write(gpb->mMemIndex + l*gpb->m32bSize, 0, source->getTextureViewProjMatrix(l), gpb->m32bSize);
                    }
                    break;
                case GSP_Light_Spot_Matrix_Proj_X_View_ParamIndex:
                    _write(gpb->mMemIndex, 0, source->getSpotlightViewProjMatrix(gpb->mInputInt), gpb->m32bSize);
                    break;
                case GSP_Light_Spot_Matrix_Proj_X_View_ParamCount:
                    for(Ni32 l = 0; l < gpb->mInputInt; ++l)
                    {
                        // can also be updated in lights
                        _write(gpb->mMemIndex + l * gpb->m32bSize, 0, source->getSpotlightViewProjMatrix(l), gpb->m32bSize);
                    }
                    break;
                default:
                    break;
                };
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, GpuSyncParam gsp, NIIf extdata)
    {
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            def->mGBTMark = getGBTMark(gsp);
            GpuParamBlock * block = getBlock(def->mIndex, def->mUnit32bSize * def->mUnitCount, def->mGBTMark, def->mDataType);

            set(block, gsp, extdata);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::set(const VString & name, GpuSyncParam gsp, Ni32 extdata)
    {
        if(mParamDefine == 0 && mAllowParamLost == false)
            N_EXCEPT(InvalidParam, _I18n("参数定义体没有定义"));
        GpuParamUnit * def = mParamDefine->get(name);
        if(def)
        {
            def->mGBTMark = getGBTMark(gsp);
            GpuParamBlock * block = getBlock(def->mIndex, def->mUnit32bSize * def->mUnitCount, def->mGBTMark, def->mDataType);

            set(block, gsp, extdata);
        }
        else
        {
            if(mAllowParamLost == false)
            {
                String tempstr;
                StrConv::conv(name, tempstr);
                N_EXCEPT(InvalidParam, _I18n("不存在参数定义: ") + tempstr);
            }
        }
    }
    //---------------------------------------------------------------------------
    const GpuParamBlock * GpuProgramParam::getSyncParamBlock(Nidx index) const
    {
        GpuParamBlock * bind = const_cast<GpuProgramParam *>(this)->getBlock(index, 0, GPT_Render, GDT_Float);

        return _getSyncParamBlock(bind ? bind->mMemIndex : 0);
    }
    //---------------------------------------------------------------------------
    const GpuParamBlock * GpuProgramParam::getSyncParamBlock(const VString & name) const
    {
        N_assert(mParamDefine != 0, _I18n("参数定义体没有定义"));

        const GpuParamUnit & def = getUnit(name);
        return _getSyncParamBlock(def.mMemIndex);
    }
    //---------------------------------------------------------------------------
    const GpuParamBlock * GpuProgramParam::_getSyncParamBlock(Nidx memidx) const
    {
        GpuParamBlockList::const_iterator i, iend = mSyncBlockList.end();
        for(i = mSyncBlockList.begin(); i != iend; ++i)
        {
            const GpuParamBlock * gpb = *i;
            if(gpb->mMemIndex == memidx)
                return gpb;
        }
        return 0;
    }
    //---------------------------------------------------------------------------
    void GpuProgramParam::copyParamValue(const GpuProgramParam & src)
    {
        mBufferData = src.getBufferData();
        mSyncBlockList = src.getSyncParamList();
        mParamGBTMark = src.mParamGBTMark;

        ShareSyncList::const_iterator i, iend = mShareSyncList.end();
        for(i = mShareSyncList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mShareSyncList.clear();

        ShareSyncList::const_iterator j, jend = src.mShareSyncList.end();
        for(j = src.mShareSyncList.begin(); j != jend; ++j)
        {
            mShareSyncList.push_back(N_new ParamValueSync((*j)->getValue(), this));
        }
    }
    //---------------------------------------------------------------------
    void GpuProgramParam::copyDefineValue(const GpuProgramParam & o)
    {
        if((mParamDefine != 0) && (o.mParamDefine != 0))
        {
            std::map<NCount, VString> srcToDestNamedMap;

            for(GpuParamUnitList::const_iterator i = o.mParamDefine->getList().begin();
                i != o.mParamDefine->getList().end(); ++i)
            {
                const VString & paramName = i->first;
                const GpuParamUnit & olddef = i->second;
                const GpuParamUnit * newdef = mParamDefine->get(paramName);
                if(newdef)
                {
                    // 复制数据,基于更小的共同定义大小
                    NCount sz = std::min(olddef.mUnit32bSize * olddef.mUnitCount, newdef->mUnit32bSize * newdef->mUnitCount);

                    memcpy(getBufferData(newdef->mMemIndex), o.getBufferData(olddef.mMemIndex), sz * sizeof(NIIf));

                    // 我们使用这个映射去解决自动常量,忽视[0]别名
                    if(!StrUtil::endsWith(paramName, "[0]") && o.getSyncParamBlock(paramName))
                        srcToDestNamedMap[olddef.mMemIndex] = paramName;
                }
            }
            GpuParamBlockList::const_iterator i, iend = o.mSyncBlockList.end();
            for(i = o.mSyncBlockList.begin(); i != iend; ++i)
            {
                const GpuParamBlock * gpb = *i;
                std::map<NCount, VString>::iterator mi = srcToDestNamedMap.find(gpb->mMemIndex);
                if(mi != srcToDestNamedMap.end())
                {
                    if(GpuProgramManager::getSyncParam(gpb->mSyncParam)->mInputType == SPI_Float)
                    {
                        set(mi->second, gpb->mSyncParam, gpb->mInputFloat);
                    }
                    else
                    {
                        set(mi->second, gpb->mSyncParam, gpb->mInputInt);
                    }
                }
            }

            // 复制不存在的共享参数设置
            for(ShareSyncList::const_iterator i = o.mShareSyncList.begin(); i != o.mShareSyncList.end(); ++i)
            {
                addValue((*i)->getValue());
            }
        }
    }
    //---------------------------------------------------------------------
    NCount GpuProgramParam::getTypeCount(GpuDataType type, bool pad128b)
    {
        if(pad128b)
        {
            switch(type)
            {
            case GDT_Float:
            case GDT_Int:
            case GDT_UInt:
            case GDT_Sampler1D:
            case GDT_Sampler2D:
            case GDT_SamplerArray2D:
            case GDT_Sampler3D:
            case GDT_SamplerCube:
            case GDT_SamplerRect:
            case GDT_SamplerShadow1D:
            case GDT_SamplerShadow2D:
            case GDT_Float2X:
            case GDT_Int2X:
            case GDT_UInt2X:
            case GDT_Float3X:
            case GDT_Int3X:
            case GDT_UInt3X:
            case GDT_Float4X:
            case GDT_Int4X:
            case GDT_UInt4X:
            case GDT_Bool:
            case GDT_Bool2X:
            case GDT_Bool3X:
            case GDT_Bool4X:
            case GDT_64Int:
            case GDT_64Int2X:
            case GDT_64Int3X:
            case GDT_64Int4X:
            case GDT_64UInt:
            case GDT_64UInt2X:
            case GDT_64UInt3X:
            case GDT_64UInt4X:
            case GDT_Double:
            case GDT_Double2X:
            case GDT_Double3X:
            case GDT_Double4X:
                return 4;
            case GDT_Matrix2X2:
            case GDT_Matrix2X3:
            case GDT_Matrix2X4:
            case GDT_Matrixd2X2:
            case GDT_Matrixd2X3:
            case GDT_Matrixd2X4:
                return 8; //     2个 float4
            case GDT_Matrix3X2:
            case GDT_Matrix3X3:
            case GDT_Matrix3X4:
            case GDT_Matrixd3X2:
            case GDT_Matrixd3X3:
            case GDT_Matrixd3X4:
                return 12; //    3个 float4
            case GDT_Matrix4X2:
            case GDT_Matrix4X3:
            case GDT_Matrix4X4:
            case GDT_Matrixd4X2:
            case GDT_Matrixd4X3:
            case GDT_Matrixd4X4:
                return 16; //     4个 float4
            default:
                return 4;
            };
        }
        else
        {
            switch(type)
            {
            case GDT_Float:
            case GDT_Int:
            case GDT_UInt:
            case GDT_Bool:
            case GDT_Sampler1D:
            case GDT_Sampler2D:
            case GDT_SamplerArray2D:
            case GDT_Sampler3D:
            case GDT_SamplerCube:
            case GDT_SamplerRect:
            case GDT_SamplerShadow1D:
            case GDT_SamplerShadow2D:
            case GDT_64Int:
            case GDT_64UInt:
            case GDT_Double:
                return 1;
            case GDT_Float2X:
            case GDT_Int2X:
            case GDT_UInt2X:
            case GDT_Bool2X:
            case GDT_64Int2X:
            case GDT_64UInt2X:
            case GDT_Double2X:
                return 2;
            case GDT_Float3X:
            case GDT_Int3X:
            case GDT_UInt3X:
            case GDT_Bool3X:
            case GDT_64Int3X:
            case GDT_64UInt3X:
            case GDT_Double3X:
                return 3;
            case GDT_Float4X:
            case GDT_Int4X:
            case GDT_UInt4X:
            case GDT_Matrix2X2:
            case GDT_Matrixd2X2:
            case GDT_Bool4X:
            case GDT_64Int4X:
            case GDT_64UInt4X:
            case GDT_Double4X:
                return 4;
            case GDT_Matrix2X3:
            case GDT_Matrix3X2:
            case GDT_Matrixd2X3:
            case GDT_Matrixd3X2:
                return 6;
            case GDT_Matrix2X4:
            case GDT_Matrix4X2:
            case GDT_Matrixd2X4:
            case GDT_Matrixd4X2:
                return 8;
            case GDT_Matrix3X3:
            case GDT_Matrixd3X3:
                return 9;
            case GDT_Matrix3X4:
            case GDT_Matrix4X3:
            case GDT_Matrixd3X4:
            case GDT_Matrixd4X3:
                return 12;
            case GDT_Matrix4X4:
            case GDT_Matrixd4X4:
                return 16;
            default:
                return 4;
            };
        }
    }
    //-----------------------------------------------------------------------
    NCount GpuProgramParam::get32bCount(GpuDataType type, bool pad128b)
    {
        if(pad128b)
        {
            switch(type)
            {
            case GDT_Float:
            case GDT_Int:
            case GDT_UInt:
            case GDT_Sampler1D:
            case GDT_Sampler2D:
            case GDT_SamplerArray2D:
            case GDT_Sampler3D:
            case GDT_SamplerCube:
            case GDT_SamplerRect:
            case GDT_SamplerShadow1D:
            case GDT_SamplerShadow2D:
            case GDT_Float2X:
            case GDT_Int2X:
            case GDT_UInt2X:
            case GDT_Float3X:
            case GDT_Int3X:
            case GDT_UInt3X:
            case GDT_Float4X:
            case GDT_Int4X:
            case GDT_UInt4X:
            case GDT_Bool:
            case GDT_Bool2X:
            case GDT_Bool3X:
            case GDT_Bool4X:
                return 4;
            case GDT_64Int:
            case GDT_64Int2X:
            case GDT_64Int3X:
            case GDT_64Int4X:
            case GDT_64UInt:
            case GDT_64UInt2X:
            case GDT_64UInt3X:
            case GDT_64UInt4X:
            case GDT_Double:
            case GDT_Double2X:
            case GDT_Double3X:
            case GDT_Double4X:
                return 8;
            case GDT_Matrix2X2:
            case GDT_Matrix2X3:
            case GDT_Matrix2X4:
                return 8; //     2个 float4
            case GDT_Matrixd2X2:
            case GDT_Matrixd2X3:
            case GDT_Matrixd2X4:
                return 16; //    4个 float4
            case GDT_Matrix3X2:
            case GDT_Matrix3X3:
            case GDT_Matrix3X4:
                return 12; //    3个 float4
            case GDT_Matrixd3X2:
            case GDT_Matrixd3X3:
            case GDT_Matrixd3X4:
                return 24; //    6个 float4
            case GDT_Matrix4X2:
            case GDT_Matrix4X3:
            case GDT_Matrix4X4:
                return 16; //     4个 float4
            case GDT_Matrixd4X2:
            case GDT_Matrixd4X3:
            case GDT_Matrixd4X4:
                return 32; //     8个 float4
            default:
                return 4;
            };
        }
        else
        {
            switch(type)
            {
            case GDT_Float:
            case GDT_Int:
            case GDT_UInt:
            case GDT_Bool:
            case GDT_Sampler1D:
            case GDT_Sampler2D:
            case GDT_SamplerArray2D:
            case GDT_Sampler3D:
            case GDT_SamplerCube:
            case GDT_SamplerRect:
            case GDT_SamplerShadow1D:
            case GDT_SamplerShadow2D:
                return 1;
            case GDT_64Int:
            case GDT_64UInt:
            case GDT_Double:
                return 2;
            case GDT_Float2X:
            case GDT_Int2X:
            case GDT_UInt2X:
            case GDT_Bool2X:
                return 2;
            case GDT_64Int2X:
            case GDT_64UInt2X:
            case GDT_Double2X:
                return 4;
            case GDT_Float3X:
            case GDT_Int3X:
            case GDT_UInt3X:
            case GDT_Bool3X:
                return 3;
            case GDT_64Int3X:
            case GDT_64UInt3X:
            case GDT_Double3X:
                return 6;
            case GDT_Float4X:
            case GDT_Int4X:
            case GDT_UInt4X:
            case GDT_Matrix2X2:
            case GDT_Bool4X:
                return 4;
            case GDT_Matrixd2X2:
            case GDT_64Int4X:
            case GDT_64UInt4X:
            case GDT_Double4X:
                return 8;
            case GDT_Matrix2X3:
            case GDT_Matrix3X2:
                return 6;
            case GDT_Matrixd2X3:
            case GDT_Matrixd3X2:
                return 12;
            case GDT_Matrix2X4:
            case GDT_Matrix4X2:
                return 8;
            case GDT_Matrixd2X4:
            case GDT_Matrixd4X2:
                return 16;
            case GDT_Matrix3X3:
                return 9;
            case GDT_Matrixd3X3:
                return 18;
            case GDT_Matrix3X4:
            case GDT_Matrix4X3:
                return 12;
            case GDT_Matrixd3X4:
            case GDT_Matrixd4X3:
                return 24;
            case GDT_Matrix4X4:
                return 16;
            case GDT_Matrixd4X4:
                return 32;
            default:
                return 4;
            };
        }
    }
    //-----------------------------------------------------------------------
    void GpuProgramParam::addValue(GpuParamValue * param)
    {
        ShareSyncList::const_iterator i, iend = mShareSyncList.end();
        for (i = mShareSyncList.begin(); i != iend; ++i)
        {
            if ((*i)->getName() == param->getName())
                return;
        }
        mShareSyncList.push_back(N_new ParamValueSync(param, this));
    }
    //-----------------------------------------------------------------------
    void GpuProgramParam::addValue(const String & name)
    {
        addValue(N_Only(GpuProgram).getShare(name));
    }
    //-----------------------------------------------------------------------
    bool GpuProgramParam::isValueExist(const String & name) const
    {
        ShareSyncList::const_iterator i, iend = mShareSyncList.end();
        for(i = mShareSyncList.begin(); i != iend; ++i)
        {
            if((*i)->getName() == name)
                return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void GpuProgramParam::removeValue(const String & name)
    {
        ShareSyncList::iterator i, iend = mShareSyncList.end();
        for(i = mShareSyncList.begin(); i != iend; ++i)
        {
            if((*i)->getName() == name)
            {
                mShareSyncList.erase(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    GpuParamValue * GpuProgramParam::getValue(Nidx idx) const
    {
        N_assert(mShareSyncList.size() > idx, "");
        return mShareSyncList[idx]->getValue();
    }
    //-----------------------------------------------------------------------
    void GpuProgramParam::removeAllValue()
    {
        ShareSyncList::iterator i, iend = mShareSyncList.end();
        for(i = mShareSyncList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mShareSyncList.clear();
    }
    //---------------------------------------------------------------------
    void GpuProgramParam::syncValue()
    {
        ShareSyncList::iterator i, iend = mShareSyncList.end();
        for(i = mShareSyncList.begin(); i != iend; ++i)
        {
            (*i)->sync();
        }
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // CustomGpuParam
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CustomGpuParam::CustomGpuParam()
    {

    }
    //------------------------------------------------------------------------
    const Vector4f & CustomGpuParam::getCustomParam(Nidx index) const
    {
        GParamValueList::const_iterator i = mCGPUParamList.find(index);
        if(i != mCGPUParamList.end())
        {
            return i->second;
        }

        return Vector4f::ZERO;
    }
    //------------------------------------------------------------------------
    void CustomGpuParam::removeCustomParam(Nidx index)
    {
        GParamValueList::iterator i = mCGPUParamList.find(index);
        if(i != mCGPUParamList.end())
        {
            mCGPUParamList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void CustomGpuParam::updateCustom(GpuProgramParam * params, const GpuParamBlock & src) const
    {
        GParamValueList::const_iterator i = mCGPUParamList.find(src.mInputInt);
        if(i != mCGPUParamList.end())
        {
            params->_write(src.mMemIndex, 0, i->second, src.m32bSize);
        }
    }
    //------------------------------------------------------------------------
    void CustomGpuParam::updateCustom(GpuProgramParam * params, Nidx index, const GpuParamBlock & src) const
    {
        GParamValueList::const_iterator i = mCGPUParamList.find(index);
        if (i != mCGPUParamList.end())
        {
            params->_write(src.mMemIndex, 0, i->second, src.m32bSize);
        }
    }
    //------------------------------------------------------------------------
}