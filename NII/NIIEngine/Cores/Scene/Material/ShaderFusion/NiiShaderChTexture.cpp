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
#include "NiiShaderChTexture.h"
#include "NiiMaterialManager.h"
#include "NiiEquationManager.h"
#include "NiiTextureManager.h"
#include "NiiAnimationManager.h"
#include "NiiLogManager.h"
#include "NiiException.h"

#define __ShaderChTexture_Env           0x01
#define __ShaderChTexture_Proj          0x02
#define __ShaderChTexture_Cubic         0x04
#define __ShaderChTexture_Alpha         0x08
#define __ShaderChTexture_HWSRGB        0x10
#define __ShaderChTexture_RecalcMatrix  0x20
#define __ShaderChTexture_LoadFailed    0x40
//#define __ShaderChTexture_UV_Offset     0x80
//#define __ShaderChTexture_UV_Scale      0x100
//#define __ShaderChTexture_Rotate       0x200
//#define __ShaderChTexture_BColour       0x400
#define __ShaderChTexture_AutoLoad      0x800

namespace NII
{
    N_mutable_mutex(mTexUnitChangeMutex)
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // TextureSampleUnit
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    TextureSampleUnit::TextureSampleUnit() : 
        mMinOP(TFOP_LINEAR),
        mMagOP(TFOP_LINEAR),
        mMipOP(TFOP_POINT),
        mCompareFunc(CPM_GREATER_EQUAL),
        mBorder(Colour::Black),
        mMaxAniso(1), 
        mMipmapBias(0)
    {
        setMode(TAM_REPEAT, TAM_REPEAT, TAM_REPEAT);
    }
    //-----------------------------------------------------------------------
    TextureSampleUnit::TextureSampleUnit(TextureFilterOP min, TextureFilterOP mag, TextureFilterOP mip,
        TextureAddressingMode u, TextureAddressingMode v, TextureAddressingMode w) :
        mMinOP(min),
        mMagOP(mag),
        mMipOP(mip),
        mCompareFunc(CPM_GREATER_EQUAL),
        mBorder(Colour::Black),
        mMaxAniso(1),
        mMipmapBias(0)
    {
        setMode(u, v, w);
    }
    //-----------------------------------------------------------------------
    TextureSampleUnit::~TextureSampleUnit() 
    {
    }
    //-----------------------------------------------------------------------
    TextureSampleUnit & TextureSampleUnit::operator =(const TextureSampleUnit & o)
    {
        mMinOP = o.mMinOP;
        mMagOP = o.mMagOP;
        mMipOP = o.mMipOP;
        mAddressMode.mS_U = o.mAddressMode.mS_U;
        mAddressMode.mT_V = o.mAddressMode.mT_V;
        mAddressMode.mR_W = o.mAddressMode.mR_W;
        mMaxAniso = o.mMaxAniso;
        mMipmapBias = o.mMipmapBias;
        mBorder = o.mBorder;
        mCompareFunc = o.mCompareFunc;
        mMinLod = o.mMinLod;
        mMaxLod = o.mMaxLod;
        return *this;
    }
    //-----------------------------------------------------------------------
    bool TextureSampleUnit::operator ==(const TextureSampleUnit & o) const
    {
        if(mMinOP == o.mMinOP && mMagOP == o.mMagOP && mMipOP == o.mMipOP &&
            mAddressMode.mS_U == o.mAddressMode.mS_U &&
            mAddressMode.mT_V == o.mAddressMode.mT_V &&
            mAddressMode.mR_W == o.mAddressMode.mR_W &&
            mMaxAniso == o.mMaxAniso && mMipmapBias == o.mMipmapBias &&
            mBorder == o.mBorder && mCompareFunc == o.mCompareFunc &&
            mMinLod == o.mMinLod && mMaxLod == o.mMaxLod)
            return true;
        return false;
    }
    //-----------------------------------------------------------------------
    bool TextureSampleUnit::operator !=(const TextureSampleUnit & o) const
    {
        if(mMinOP != o.mMinOP || mMagOP != o.mMagOP || mMipOP != o.mMipOP ||
            mAddressMode.mS_U != o.mAddressMode.mS_U ||
            mAddressMode.mT_V != o.mAddressMode.mT_V ||
            mAddressMode.mR_W != o.mAddressMode.mR_W ||
            mMaxAniso != o.mMaxAniso || mMipmapBias != o.mMipmapBias ||
            mBorder != o.mBorder && mCompareFunc != o.mCompareFunc ||
                mMinLod != o.mMinLod || mMaxLod != o.mMaxLod)
            return true;
        return false;
    }
    //-----------------------------------------------------------------------
    bool TextureSampleUnit::operator <(const TextureSampleUnit & o) const
    {
        if(mMinOP < o.mMinOP || mMagOP < o.mMagOP || mMipOP < o.mMipOP ||
            mAddressMode.mS_U < o.mAddressMode.mS_U ||
            mAddressMode.mT_V < o.mAddressMode.mT_V ||
            mAddressMode.mR_W < o.mAddressMode.mR_W ||
            mMaxAniso < o.mMaxAniso || mMipmapBias < o.mMipmapBias ||
            mBorder < o.mBorder || mCompareFunc < o.mCompareFunc ||
                mMinLod < o.mMinLod || mMaxLod < o.mMaxLod)
            return true;
        return false;
    }
    //-----------------------------------------------------------------------
    void TextureSampleUnit::setFiltering(TextureFilterMode type)
    {
        switch (type)
        {
        case TFM_NONE:
            setFiltering(TFOP_POINT, TFOP_POINT, TFOP_NONE);
            break;
        case TFM_BILINEAR:
            setFiltering(TFOP_LINEAR, TFOP_LINEAR, TFOP_POINT);
            break;
        case TFM_TRILINEAR:
            setFiltering(TFOP_LINEAR, TFOP_LINEAR, TFOP_LINEAR);
            break;
        case TFM_ANISOTROPIC:
            setFiltering(TFOP_ANISOTROPIC, TFOP_ANISOTROPIC, TFOP_LINEAR);
            break;
        }
    }
    //-----------------------------------------------------------------------
    void TextureSampleUnit::setFiltering(TextureFilterType type, TextureFilterOP fo)
    {
        switch (type)
        {
        case TFT_MIN:
            mMinOP = fo;
            break;
        case TFT_MAG:
            mMagOP = fo;
            break;
        case TFT_MIP:
            mMipOP = fo;
            break;
        }
    }
    //-----------------------------------------------------------------------
    TextureFilterOP TextureSampleUnit::getFiltering(TextureFilterType type) const
    {
        switch (type)
        {
        case TFT_MIN:
            return mMinOP;
        case TFT_MAG:
            return mMagOP;
        case TFT_MIP:
            return mMipOP;
        }
        return mMinOP;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TextureSample
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TextureSample::TextureSample()
    {
        memset(mUsed, 0, sizeof(mUsed));
    }
    //------------------------------------------------------------------------
    TextureSample::~TextureSample()
    {
        mSetList.clear();
    }
    //------------------------------------------------------------------------
    TextureSample & TextureSample::operator = (const TextureSample & o) const
    {
        mUnitList = o.mUnitList;
        mSetList = o.mSetList;
        memcpy(mUsed, o.mUsed, sizeof(mUsed));
    }
    //------------------------------------------------------------------------
    bool TextureSample::operator == (const TextureSample & o) const
    {
        if(memcmp(mUsed, o.mUsed, sizeof(mUsed)) == 0)
        {
            TextureSampleUnitList::iterator i, iend = mSetList.end();
            TextureSampleUnitList::iterator j, jend = o.mSetList.end();
            for(i = mSetList.begin(), j = mSetList.begin(); i != iend; ++i, ++j)
            {
                if(*i != *j)
                    return false;
            }
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool TextureSample::operator != (const TextureSample & o) const
    {
        if(memcmp(mUsed, o.mUsed, sizeof(mUsed)) == 0)
        {
            TextureSampleUnitList::iterator i, iend = mSetList.end();
            TextureSampleUnitList::iterator j, jend = o.mSetList.end();
            for(i = mSetList.begin(), j = mSetList.begin(); i != iend; ++i, ++j)
            {
                if(*i != *j)
                    return true;
            }
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool TextureSample::operator < (const TextureSample & o) const
    {
        int re = memcmp(mUsed, o.mUsed, sizeof(mUsed));
        if(re != 0)
        {
            return re < 0;
        }
        TextureSampleUnitList::iterator i, iend = mSetList.end();
        TextureSampleUnitList::iterator j, jend = o.mSetList.end();
        for(i = mSetList.begin(), j = o.mSetList.begin(); i != iend; ++i, ++j)
        {
            if(*i != *j)
                return *i < *j;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void TextureSample::add(const VString & name, const TextureSampleUnit & unit)
    {
        UnitListL::iterator i = mUnitList.find(name);
        if(i == mUnitList.end())
        {
            mUnitList.emplace(name, unit);
        }
        else
        {
            i->second = unit;
        }
    }
    //------------------------------------------------------------------------
    void TextureSample::remove(const VString & name)
    {
        UnitListL::iterator i = mUnitList.find(name);
        if(i != mUnitList.end())
        {
            mUnitList.erase(i);
        }
    }   
    //------------------------------------------------------------------------
    void TextureSample::set(Nidx slot, const VString & name)
    {
        UnitListL::iterator i = mUnitList.find(name);
        if(i != mUnitList.end())
        {
            if(mSetList.size() > slot)
                mSetList.resize(slot + 1);
            mSetList[slot] = *i->second;
        }
    }   
    //------------------------------------------------------------------------
    TextureSampleUnit * TextureSample::get(const VString & name) const
    {
        UnitListL::iterator i = mUnitList.find(name);
        if(i != mUnitList.end())
        {
            return *i->second;
        }
    }
    //------------------------------------------------------------------------
    TextureSampleUnit * TextureSample::get(Nidx slot) const
    {
        N_assert(mSetList.size() > slot)
        return mSetList[slot];
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TextureBlend
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TextureBlend::TextureBlend()
    {
    }
    //------------------------------------------------------------------------
    TextureBlend::TextureBlend(TextureColourType type) :
        mColourType(type)
    {
    }
    //------------------------------------------------------------------------
    TextureBlend::TextureBlend(TextureColourType type, TextureBlendOp op,
        TextureBlendSource op1, TextureBlendSource op2) :
            mColourType(type),
            mOP(op),
            mColourSrc1(op1),
            mColourSrc2(op2)
    {
    }
    //------------------------------------------------------------------------
    TextureBlend::~TextureBlend()
    {
    }
    //------------------------------------------------------------------------
    void TextureBlend::operator =(const TextureBlend & o)
    {
        mOP = o.mOP;
        mColourType = o.mColourType;
        mColourSrc1 = o.mColourSrc1;
        mColourSrc2 = o.mColourSrc2;
        mConstantFactor = o.mConstantFactor;
        mConstant1 = o.mConstant1;
        mConstant2 = o.mConstant2;
    }
    //------------------------------------------------------------------------
    bool TextureBlend::operator==(const TextureBlend & o) const
    {
        if(mColourType == o.mColourType)
        {
            if (mColourType == TCT_RGB)
            {
                if(mOP == o.mOP &&
                    mColourSrc1 == o.mColourSrc1 && mColourSrc2 == o.mColourSrc2 &&
                        mConstant1 == o.mConstant1 && mConstant2 == o.mConstant2 &&
                            mConstantFactor == o.mConstantFactor)
                {
                    return true;
                }
            }
            else
            {
                if(mOP == o.mOP &&
                    mColourSrc1 == o.mColourSrc1 && mColourSrc2 == o.mColourSrc2 &&
                        mConstant1.a == o.mConstant1.a && mConstant2.a == o.mConstant2.a &&
                            mConstantFactor == o.mConstantFactor)
                {
                    return true;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool TextureBlend::operator!=(const TextureBlend & o) const
    {
        if(mColourType == o.mColourType)
        {
            if (mColourType == TCT_RGB)
            {
                if(mOP != o.mOP ||
                    mColourSrc1 != o.mColourSrc1 || mColourSrc2 != o.mColourSrc2 ||
                        mConstant1 != o.mConstant1 || mConstant2 != o.mConstant2 ||
                            mConstantFactor != o.mConstantFactor)
                {
                    return true;
                }
            }
            else
            {
                if(mOP != o.mOP ||
                    mColourSrc1 != o.mColourSrc1 || mColourSrc2 != o.mColourSrc2 ||
                        mConstant1.a != o.mConstant1.a || mConstant2.a != o.mConstant2.a ||
                            mConstantFactor != o.mConstantFactor)
                {
                    return true;
                }
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    void TextureBlend::setColourBlend(TextureBlendMode mode)
    {
        // Set up the multitexture and multipass blending operations
        switch(mode)
        {
        case TBM_Src_Replace_Dst:
            setBlend(TBO_OPERAND1, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        case TBM_Src_Add_Dst:
            setBlend(TBO_ADD, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        case TBM_Src_Sub_Dst:
            setBlend(TBO_SUB, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        case TBM_Src_Modulate_Dst:
            setBlend(TBO_MODULATE, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        case TBM_Src_Alpha_Dst:
            setBlend(TBO_ADD_TEXTURE, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        }
        mColourType = TCT_RGB;
    }
    //------------------------------------------------------------------------
    void TextureBlend::setAlphaBlend(TextureBlendMode mode)
    {
        // Set up the multitexture and multipass blending operations
        switch(mode)
        {
        case TBM_Src_Replace_Dst:
            setBlend(TBO_OPERAND1, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        case TBM_Src_Add_Dst:
            setBlend(TBO_ADD, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        case TBM_Src_Sub_Dst:
            setBlend(TBO_SUB, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        case TBM_Src_Modulate_Dst:
            setBlend(TBO_MODULATE, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        case TBM_Src_Alpha_Dst:
            setBlend(TBO_ADD_TEXTURE, TBS_TEXTURE, TBS_PRE_TEXTURE);
            break;
        }
        mColourType = TCT_AAA;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ShaderChTexture
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ShaderChTexture::Slot ShaderChTexture::NullSlot;
    //-----------------------------------------------------------------------
    ShaderChTexture::ShaderChTexture()
    {
        memset(mUsed, 0, sizeof(mUsed));
    }
    //-----------------------------------------------------------------------
    ShaderChTexture::ShaderChTexture(const ShaderChTexture & o):
    {
        mSlotList = o.mSlotList;
        memcpy(mUsed, o.mUsed, sizeof(mUsed));
    }
    //-----------------------------------------------------------------------
    ShaderChTexture::~ShaderChTexture()
    {
        mSlotList.clear();
        mApplyList.clear();
    }
    //-----------------------------------------------------------------------
    ShaderChTexture & ShaderChTexture::operator =(const ShaderChTexture & o)
    {
        mSlotList = o.mSlotList;
        memcpy(mUsed, o.mUsed, sizeof(mUsed));
        if(mApplyList.size())
            notify();
        return *this;
    }
    //-----------------------------------------------------------------------
    bool ShaderChTexture::operator == (const ShaderChTexture & o) const
    {
        if(memcmp(mUsed, o.mUsed, sizeof(mUsed)) == 0)
        {
            SlotList::iterator i, iend = mSlotList.end();
            SlotList::iterator j, jend = o.mSlotList.end();
            for(i = mSlotList.begin(), j = mSlotList.begin(); i != iend; ++i, ++j)
            {
                if(*i != *j)
                    return false;
            }
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool ShaderChTexture::operator != ( const ShaderChTexture & o ) const
    {
        if(memcmp(mUsed, o.mUsed, sizeof(mUsed)) == 0)
        {
            SlotList::iterator i, iend = mSlotList.end();
            SlotList::iterator j, jend = o.mSlotList.end();
            for(i = mSlotList.begin(), j = mSlotList.begin(); i != iend; ++i, ++j)
            {
                if(*i != *j)
                    return true;
            }
            return false;
        }
        return true;
    }
    //-----------------------------------------------------------------------
    bool ShaderChTexture::operator < ( const ShaderChTexture & o ) const
    {
        int re = memcmp(mUsed, o.mUsed, sizeof(mUsed));
        if(re != 0)
        {
            return re < 0;
        }
        SlotList::iterator i, iend = mSlotList.end();
        SlotList::iterator j, jend = o.mSlotList.end();
        for(i = mSlotList.begin(), j = o.mSlotList.begin(); i != iend; ++i, ++j)
        {
            if(*i != *j)
                return *i < *j;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::notify()
    {
        ShaderChList::iterator i, iend = mApplyList.end();
        for(i = mApplyList.begin(); i != iend; ++i)
        {
            (*i)->notify();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::addRef(const ShaderCh * sth)
    {
        ShaderChList::iterator i, iend = mApplyList.end();
        for(i = mApplyList.begin(); i != iend; ++i)
        {
            if(*i == sth)
                return;
        }
        mApplyList.push_back(sth);
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::removeRef(const ShaderCh * sth)
    {
        ShaderChList::iterator i, iend = mApplyList.end();
        for(i = mApplyList.begin(); i != iend; ++i)
        {
            if(*i == sth)
            {
                mApplyList.erase(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::setFilterMode(TextureFilterMode mode)
    {
        N_lock_mutex(mTexUnitChangeMutex)
        SlotList::iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            if(i->isTexture())
                i->getTexture().getBuffer()->getSample()->setFiltering(mode);
        }
    }
    // --------------------------------------------------------------------
    void ShaderChTexture::setAnisotropy(Nui32 maxAniso)
    {
        N_lock_mutex(mTexUnitChangeMutex)
        SlotList::iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            if(i->isTexture())
                i->getTexture().getBuffer()->getSample()->setAnisotropy(maxAniso);
        }
    }
    //-----------------------------------------------------------------------
    ShaderChTextureUnit * ShaderChTexture::create(GpuProgram::ShaderType stype, Type type)
    {
        ShaderChTextureUnit * t = N_new ShaderChTextureUnit();
        
        t->setContentType(type);
        add(stype, t);
        return t;
    }
    //-----------------------------------------------------------------------
    ShaderChTextureUnit * ShaderChTexture::create(ResourceID rid, GpuProgram::ShaderType stype, Nidx slot, Type type)
    {
        ShaderChTextureUnit * t = N_new ShaderChTextureUnit();
        
        ResourceIDList ridlist;
        ridlist.push_back(rid);
        t->setSrc(ridlist, Texture::T_2D);
        t->setCoordSet(slot);
        t->setContentType(type);
        add(stype, t);
        return t;
    }
    //-----------------------------------------------------------------------
    ShaderChTextureUnit * ShaderChTexture::create(const Texture * tex, GpuProgram::ShaderType stype, Nidx slot, Type type)
    {
        ShaderChTextureUnit * t = N_new ShaderChTextureUnit();

        t->setSrc(tex, Texture::T_2D);
        t->setCoordSet(slot);
        t->setContentType(type);
        add(stype, t);
        return t;
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::add(GpuProgram::ShaderType type, ShaderChTextureUnit * unit, Nidx idx)
    {
        N_lock_mutex(mTexUnitChangeMutex)

        if(unit)
        {
            NCount count = (idx == -1 ? mUsed[type] : idx);
            for(NCount c = 0; c < type; ++c)
            {
                count += mUsed[c];
            }
            SlotList::iterator i = mSlotList.begin() + count;
            if(i == mSlotList.end())
            {
                unit->setLocalIndex(this, mSlotList.size());
                mSlotList.push_back(unit);
            }
            else
            {
                NCount idx = (*i)->getLocalIndex(this);
                mSlotList.insert(i, unit);
                i = mSlotList.begin() + count;
                SlotList::iterator iend = mSlotList.end()
                for(; i != iend; ++i, ++idx)
                {
                    (*i)->setLocalIndex(this, idx);
                }
            }
            ++mUsed[type];

            unit->addRef(this);

            // if texture unit unit name is empty then give it a default name based on its index
            if(unit->getName().empty())
            {
                unit->setName(N_conv(idx));
            }

            notify();
        }
    }
    //-----------------------------------------------------------------------
    Slot & ShaderChTexture::get(Nidx index) const
    {
        N_lock_mutex(mTexUnitChangeMutex)
        N_assert (index < mSlotList.size(), "Index out of bounds");
        return mSlotList[index];
    }
    //-----------------------------------------------------------------------------
    Slot & ShaderChTexture::get(const String & name) const
    {
        N_lock_mutex(mTexUnitChangeMutex)
        SlotList::const_iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            if(i->isTexture() && i->getTexture().getBuffer()->getName() == name)
            {
                return *i;
            }
        }
        return NullSlot;
    }
    //-----------------------------------------------------------------------
    Slot & ShaderChTexture::get(GpuProgram::ShaderType type, Nidx index) const
    {
        if(mUsed[type] > index)
        {
            for(NCount c = 0; c < type; ++c)
            {
                index += mUsed[c];
            }

            return mSlotList[index];
        }
        
        return NullSlot;
    }
    //-----------------------------------------------------------------------
    Slot & ShaderChTexture::get(GpuProgram::ShaderType type, SlotList::const_iterator & begin, SlotList::const_iterator & end) const
    {
        if(mUsed[type] > 0)
        {
            NCount count = 0;
            for(NCount c = 0; c < type; ++c)
            {
                count += mUsed[c];
            }

            begin = mSlotList.begin() + count;
            end = begin + mUsed[type];
            return *begin;
        }
        
        begin = mSlotList.end();
        end = mSlotList.end();
        return NullSlot;
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::remove(GpuProgram::ShaderType type, Nidx index)
    {
        N_lock_mutex(mTexUnitChangeMutex)
        if(mUsed[type] > index)
        {
            for(NCount c = 0; c < type; ++c)
            {
                index += mUsed[c];
            }

            SlotList::iterator j = mSlotList.begin() + index;
            SlotList::iterator i, iend = mSlotList.end();
            for(i = j; i != iend; ++i)
            {
                (*i)->setLocalIndex(this, (*i)->getLocalIndex() - 1);
            }
            mSlotList.erase(j);

            --mUsed[type];

            notify();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::move(GpuProgram::ShaderType type, Nidx begin, Nidx end, ShaderChTexture * o)
    {
        N_lock_mutex(mTexUnitChangeMutex)
        if(mUsed[type] > begin)
        {
            if(mUsed[type] < end)
                end = mUsed[type];

            NCount z = 0;
            NCount count = 0;
            for(NCount c = 0; c < type; ++c)
            {
                count += mUsed[c];
            }

            SlotList::iterator j, j2 = mSlotList.begin() + count + begin;
            SlotList::iterator jend = mSlotList.begin() + count + end;
            for(j = j2; j != jend; ++j, ++z)
            {
                (*j)->removeRef(this);
                o->add(type, *j);
            }

            SlotList::iterator i, iend = mSlotList.end();
            for(i = jend; i != iend; ++i)
            {
                (*i)->setLocalIndex(this, (*i)->getLocalIndex() - z);
            }

            mSlotList.erase(j2, jend);

            mUsed[type] -= z;

            notify();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::removeAll()
    {
        N_lock_mutex(mTexUnitChangeMutex)
        SlotList::iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mSlotList.clear();
        memset(mUsed, 0, sizeof(mUsed));
        
        notify();
    }
    //-----------------------------------------------------------------------
    bool ShaderChTexture::isValid() const
    {
        SlotList::iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            const Slot & slot = *itor;
            if(slot.isTexture())
            {
                const SamplerUnit & su = slot.getTexture();
                if(su.mFormat = su.mTexture->getFormat() && !su.mTexture->isAlterView())
                {
                    //This warning here is for
                    N_EXCEPT(Internal,
                        "To reinterpret the texture as a different format, you must set the reinterpretable flag "
                        "(Texture::MM_AlterView) for texture: '" + su.mTexture->getName() + "' " +
                        su.mTexture->getSettingsDesc() );
                }
            }
            else if(slot.isBuffer())
            {
                const StorageUnit & su = slot.getBuffer();
                if(N_MarkTrue(su.mBuffer->getModeMark(), M_Normal))
                {
                    N_EXCEPT(Internal, "Dynamic buffers cannot be baked into a static DescriptorSet" );
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::plan()
    {
        SlotList::iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            (*i)->plan();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::unplan()
    {
        SlotList::iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            (*i)->unplan();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::load()
    {
        SlotList::iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            (*i)->load();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::unload()
    {
        SlotList::iterator i, iend = mSlotList.end();
        for(i = mSlotList.begin(); i != iend; ++i)
        {
            (*i)->unload();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTexture::notify()
    {
        ShaderChList::iterator i, iend = mApplyList.end();
        for(i = mApplyList.begin(); i != iend; ++i)
        {
            (*i)->notify();
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ShaderChTexture
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ShaderChTextureUnit::ShaderChTextureUnit() :
        mResGroupID(GroupUnknow),
        mCurrentFrame(0),
        mAnimDuration(0),
        mTextureType(Texture::T_2D),
        mFormat(PF_UNKNOWN),
        mMipmapCount(-1),
        mCoordSet(0),
        mUOft(0),
        mVOft(0),
        mUScale(1),
        mVScale(1),
        mRotate(0.0f),
        mMatrix(Matrix4f::IDENTITY),
        mFetchType(TDF_FS),
        mContentType(ShaderChTexture::T_NORMAL),
        mTimeFunc(0),
        mMultiIndex(0),
        mRefCount(0),
        mMark(0)
    {
        mSample.mMinOP = N_Only(Material).getTextureFilter(TFT_MIN);
        mSample.mMagOP = N_Only(Material).getTextureFilter(TFT_MAG);
        mSample.mMipOP = N_Only(Material).getTextureFilter(TFT_MIP);
        mSample.mMaxAniso = N_Only(Material).getTextureAniso();
        mSample.mMipmapBias = 0;
        mSample.mBorder = Colour::Blue;
        mColourBlend.setColourBlend(TBM_Src_Modulate_Dst);
        mAlphaBlend.setAlphaBlend(TBM_Src_Modulate_Dst);
        //mAddressMode.mS_U = TAM_REPEAT;
        //mAddressMode.mT_V = TAM_REPEAT;
        //mAddressMode.mR_W = TAM_REPEAT;
    }
    //-----------------------------------------------------------------------
    ShaderChTextureUnit::ShaderChTextureUnit(ResourceID rid) :
        mResGroupID(GroupUnknow),
        mCurrentFrame(0),
        mAnimDuration(0),
        mCoordSet(0),
        mFormat(PF_UNKNOWN),
        mMipmapCount(-1),
        mUOft(0),
        mVOft(0),
        mUScale(1),
        mVScale(1),
        mRotate(0.0f),
        mMatrix(Matrix4f::IDENTITY),
        mFetchType(TDF_FS),
        mTimeFunc(0),
        mMultiIndex(0),
        mRefCount(0),
        mMark(0)
    {
        mSample.mMinOP = N_Only(Material).getTextureFilter(TFT_MIN);
        mSample.mMagOP = N_Only(Material).getTextureFilter(TFT_MAG);
        mSample.mMipOP = N_Only(Material).getTextureFilter(TFT_MIP);
        mSample.mMaxAniso = N_Only(Material).getTextureAniso();
        mSample.mBorder = Colour::Black;
        mSample.mMipmapBias = 0;
        mColourBlend.setColourBlend(TBM_Src_Modulate_Dst);
        mAlphaBlend.setAlphaBlend(TBM_Src_Modulate_Dst);
        //mAddressMode.mS_U = TAM_REPEAT;
        //mAddressMode.mT_V = TAM_REPEAT;
        //mAddressMode.mR_W = TAM_REPEAT;
        ResourceIDList temp;
        temp.push_back(rid);
        setSrc(temp, Texture::T_2D);
    }
    //-----------------------------------------------------------------------
    ShaderChTextureUnit::ShaderChTextureUnit(const ShaderChTextureUnit & o)
    {
        mTimeFunc = 0;
        *this = o;
    }
    //-----------------------------------------------------------------------
    ShaderChTextureUnit::~ShaderChTextureUnit()
    {
        EffectMap::iterator i, iend = mEffects.end();
        for (i = mEffects.begin(); i != iend; ++i)
        {
            if (i->second.mTimer)
            {
                N_Only(Equation).destroy(i->second.mTimer);
            }
        }
        mEffects.clear();
        recover();
        unload();
    }
    //-----------------------------------------------------------------------
    ShaderChTextureUnit & ShaderChTextureUnit::operator = (const ShaderChTextureUnit & o)
    {
        N_assert(mTimeFunc == 0, "error");

        mCurrentFrame = o.mCurrentFrame;
        mAnimDuration = o.mAnimDuration;
        mResGroupID = o.mResGroupID;
        mMark = o.mMark;
        mTextureType = o.mTextureType;
        mFormat = o.mFormat;
        mMipmapCount = o.mMipmapCount;
        mCoordSet = o.mCoordSet;
        mColourBlend = o.mColourBlend;
        mAlphaBlend = o.mAlphaBlend;
        mUOft = o.mUOft;
        mVOft = o.mVOft;
        mUScale = o.mUScale;
        mVScale = o.mVScale;
        mRotate = o.mRotate;
        mMatrix = o.mMatrix;
        mSample = o.mSample;
        mFetchType = o.mFetchType;
        mContentType = o.mContentType;
        mMultiIndex = o.mMultiIndex;

        mTexIDList = o.mTexIDList;

        TextureList::iterator c1, cend1 = mTextures.end();
        for (c1 = mTextures.begin(); c1 != cend1; ++c1)
        {
            if ((*c1) != 0)
                (*c1)->unref();
        }
        TextureList::const_iterator c2, cend2 = o.mTextures.end();
        for (c2 = o.mTextures.begin(); c2 != cend2; ++c2)
        {
            if ((*c2) != 0)
                (*c2)->ref();
        }
        mTextures = o.mTextures;
        mName = o.mName;
        mEffects = o.mEffects;

        mFrameFusion = o.mFrameFusion;
        mFusionTex = o.mFusionTex;
        // Can't sharing controllers with other TUS, reset to null to avoid potential bug.
        for (EffectMap::iterator j = mEffects.begin(); j != mEffects.end(); ++j)
        {
            j->second.mTimer = 0;
        }

        if (mMark & __ShaderChTexture_AutoLoad)
        {
            load();
        }

        return *this;
    }
    //-----------------------------------------------------------------------
    ResourceID ShaderChTextureUnit::getResourceID() const
    {
        if (mCurrentFrame < mTexIDList.size())
            return mTexIDList[mCurrentFrame];
        else
            return 0;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setScrollAnimation(NIIf uf, NIIf vf)
    {
        // Remove existing effects
        remove(TTT_UV_OFT);
        remove(TTT_U_OFT);
        remove(TTT_V_OFT);

        // don't create an effect if the speeds are both 0
        if (uf == 0.0f && vf == 0.0f)
        {
            return;
        }

        // Create new effect
        TextureEffect eff;
        if (uf == vf)
        {
            eff.mType = TTT_UV_OFT;
            eff.mFactor = uf;

            // Replace - must be unique
            // Search for existing eff of this mType
            EffectMap::iterator i = mEffects.find(eff.mType);
            if (i != mEffects.end())
            {
                if (i->second.mTimer)
                {
                    N_Only(Equation).destroy(i->second.mTimer);
                }

                mEffects.erase(i);
            }

            if (mMark & __ShaderChTexture_AutoLoad)
            {
                eff.mTimer = N_Only(Material).createUVScroll(this, eff.mFactor);
            }
            else
            {
                eff.mTimer = 0;
            }
            mEffects.insert(EffectMap::value_type(eff.mType, eff));
        }
        else
        {
            if (uf)
            {
                eff.mType = TTT_U_OFT;
                eff.mFactor = uf;

                // Replace - must be unique
                // Search for existing eff of this mType
                EffectMap::iterator i = mEffects.find(eff.mType);
                if (i != mEffects.end())
                {
                    if (i->second.mTimer)
                    {
                        N_Only(Equation).destroy(i->second.mTimer);
                    }
                    mEffects.erase(i);
                }

                if (mMark & __ShaderChTexture_AutoLoad)
                {
                    eff.mTimer = N_Only(Material).createUScroll(this, eff.mFactor);
                }
                else
                {
                    eff.mTimer = 0;
                }
                mEffects.insert(EffectMap::value_type(eff.mType, eff));
            }
            if (vf)
            {
                eff.mType = TTT_V_OFT;
                eff.mFactor = vf;

                // Replace - must be unique
                // Search for existing eff of this mType
                EffectMap::iterator i = mEffects.find(eff.mType);
                if (i != mEffects.end())
                {
                    if (i->second.mTimer)
                    {
                        N_Only(Equation).destroy(i->second.mTimer);
                    }
                    mEffects.erase(i);
                }

                if (mMark & __ShaderChTexture_AutoLoad)
                {
                    eff.mTimer = N_Only(Material).createVScroll(this, eff.mFactor);
                }
                else
                {
                    eff.mTimer = 0;
                }
                mEffects.insert(EffectMap::value_type(eff.mType, eff));
            }
        }
    }
    //------------------------------------------------------------------------
    void ShaderChTextureUnit::setScaleAnimation(NIIf uf, NIIf vf)
    {
        // Remove existing effects
        remove(TTT_UV_SCALE);
        remove(TTT_U_SCALE);
        remove(TTT_V_SCALE);

        // don't create an effect if the speeds are both 0
        if (uf == 0.0f && vf == 0.0f)
        {
            return;
        }

        TextureEffect eff;
        if (uf == vf)
        {
            eff.mType = TTT_UV_SCALE;
            eff.mFactor = uf;

            EffectMap::iterator i = mEffects.find(eff.mType);
            if (i != mEffects.end())
            {
                if (i->second.mTimer)
                {
                    N_Only(Equation).destroy(i->second.mTimer);
                }

                mEffects.erase(i);
            }

            if (mMark & __ShaderChTexture_AutoLoad)
            {
                eff.mTimer = N_Only(Material).createUVScale(this, eff.mFactor);
            }
            else
            {
                eff.mTimer = 0;
            }
            mEffects.insert(EffectMap::value_type(eff.mType, eff));
        }
        else
        {
            if (uf)
            {
                eff.mType = TTT_U_SCALE;
                eff.mFactor = uf;

                EffectMap::iterator i = mEffects.find(eff.mType);
                if (i != mEffects.end())
                {
                    if (i->second.mTimer)
                    {
                        N_Only(Equation).destroy(i->second.mTimer);
                    }
                    mEffects.erase(i);
                }

                if (mMark & __ShaderChTexture_AutoLoad)
                {
                    eff.mTimer = N_Only(Material).createUScale(this, eff.mFactor);
                }
                else
                {
                    eff.mTimer = 0;
                }
                mEffects.insert(EffectMap::value_type(eff.mType, eff));
            }
            if (vf)
            {
                eff.mType = TTT_V_SCALE;
                eff.mFactor = vf;

                EffectMap::iterator i = mEffects.find(eff.mType);
                if (i != mEffects.end())
                {
                    if (i->second.mTimer)
                    {
                        N_Only(Equation).destroy(i->second.mTimer);
                    }
                    mEffects.erase(i);
                }

                if (mMark & __ShaderChTexture_AutoLoad)
                {
                    eff.mTimer = N_Only(Material).createVScale(this, eff.mFactor);
                }
                else
                {
                    eff.mTimer = 0;
                }
                mEffects.insert(EffectMap::value_type(eff.mType, eff));
            }
        }
    }
    //------------------------------------------------------------------------
    void ShaderChTextureUnit::setRotateAnimation(NIIf sf)
    {
        // Remove existing effect
        remove(TTT_ROTATE);

        if (sf == 0.0f)
        {
            return;
        }

        TextureEffect eff;
        eff.mType = TTT_ROTATE;
        eff.mFactor = sf;

        EffectMap::iterator i = mEffects.find(eff.mType);
        if (i != mEffects.end())
        {
            if (i->second.mTimer)
            {
                N_Only(Equation).destroy(i->second.mTimer);
            }

            mEffects.erase(i);
        }

        if (mMark & __ShaderChTexture_AutoLoad)
        {
            eff.mTimer = N_Only(Material).createRotate(this, eff.mFactor);
        }
        else
        {
            eff.mTimer = 0;
        }
        mEffects.insert(EffectMap::value_type(eff.mType, eff));
    }
    //------------------------------------------------------------------------
    void ShaderChTextureUnit::addAnimation(TextureTransformType ttype,
        DataValue<NIIf> * in, DataFunc<TimeDurMS, NIIf> * out)
    {
        // Remove existing effect
        // note, only remove for mEffect, not entire TTT_FUNCTION
        // otherwise we won't be able to combine subtypes
        // Get range of items matching this effect
        for (EffectMap::iterator i = mEffects.begin(); i != mEffects.end(); ++i)
        {
            if (i->second.mType == TTT_FUNCTION && i->second.mEffect == ttype)
            {
                if (i->second.mTimer)
                {
                    N_Only(Equation).destroy(i->second.mTimer);
                }
                mEffects.erase(i);
                // should only be one, so jump out
                break;
            }
        }
        if (out->isValid())
        {

            TextureEffect eff;
            eff.mType = TTT_FUNCTION;
            eff.mEffect = ttype;
            eff.mIn = in;
            eff.mOutFunc = out;

            if (mMark & __ShaderChTexture_AutoLoad)
            {
                eff.mTimer = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), eff.mIn, eff.mOutFunc);
            }
            else
            {
                eff.mTimer = 0;
            }
            mEffects.insert(EffectMap::value_type(eff.mType, eff));
        }
    }
    //------------------------------------------------------------------------
    void ShaderChTextureUnit::remove(TextureTransformType type)
    {
        std::pair<EffectMap::iterator, EffectMap::iterator> remPair = mEffects.equal_range(type);

        EffectMap::iterator i, iend = remPair.second;
        for (i = remPair.first; i != iend; ++i)
        {
            if (i->second.mTimer)
            {
                N_Only(Equation).destroy(i->second.mTimer);
            }
        }
        mEffects.erase(remPair.first, remPair.second);
    }
    //------------------------------------------------------------------------
    void ShaderChTextureUnit::removeAllEffects()
    {
        EffectMap::iterator i, iend = mEffects.end();
        for (i = mEffects.begin(); i != iend; ++i)
        {
            if (i->second.mTimer)
            {
                N_Only(Equation).destroy(i->second.mTimer);
            }
        }
        mEffects.clear();
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setSrc(const ResourceIDList & rid, Texture::Type type, Type ctype)
    {
        recover();
        mContentType = ctype;

        mMark &= ~__ShaderChTexture_LoadFailed;
        switch (type)
        {
        case Texture::T_CUBE:
            mTextureType = Texture::T_CUBE;
            mMark |= __ShaderChTexture_Cubic;

            mTexIDList.resize(1);
            mTextures.resize(1, 0);
            mTexIDList[0] = rid[0];
            mTextures[0] = 0;
            mAnimDuration = 0;
            mCurrentFrame = 0;

            notify();
            break;
        case Texture::T_CUBE_2D:
            mTextureType = Texture::T_2D;
            mMark |= __ShaderChTexture_Cubic;

            mTexIDList.resize(6);
            mTextures.resize(6, 0);
            mTexIDList[0] = rid[0];
            mTexIDList[1] = rid[1];
            mTexIDList[2] = rid[2];
            mTexIDList[3] = rid[3];
            mTexIDList[4] = rid[4];
            mTexIDList[5] = rid[5];
            mAnimDuration = 0;
            mCurrentFrame = 0;

            notify();
            break;
        default:
            mTextureType = type;
            mMark &= ~__ShaderChTexture_Cubic;

            mTexIDList.resize(rid.size());
            mTextures.resize(rid.size(), 0);
            mTexIDList = rid;

            if (rid.size() && (mMark & __ShaderChTexture_AutoLoad))
            {
                load();
            }
            break;
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setSrc(const Texture * tex, Texture::Type type, Type ctype)
    {
        N_assert(tex != 0, "error");

        recover();
        mContentType = ctype;

        mMark &= ~__ShaderChTexture_LoadFailed;
        if (type == Texture::T_CUBE)
        {
            mTextureType = Texture::T_CUBE;
            mMark |= __ShaderChTexture_Cubic;

            mTexIDList.resize(1);
            mTextures.resize(1);
            mAnimDuration = 0;
            mCurrentFrame = 0;

            mTexIDList[0] = tex->getOriginID();
            mTextures[0] = const_cast<Texture *>(tex);
            mTextures[0]->ref(false);

            notify();
        }
        else if (type == Texture::T_CUBE_2D)
        {
            mTextureType = Texture::T_2D;
            mMark |= __ShaderChTexture_Cubic;

            mTexIDList.resize(6);
            mTextures.resize(6);
            mAnimDuration = 0;
            mCurrentFrame = 0;

            for (NCount i = 0; i < 6; ++i)
            {
                mTexIDList[i] = tex[i].getOriginID();
                mTextures[i] = const_cast<Texture *>(&tex[i]);
                mTextures[i]->ref(false);
            }
            notify();
        }
        else
        {
            mTexIDList.resize(1);
            mTextures.resize(1);
            mCurrentFrame = 0;
            mTextureType = tex->getType();
            mMark &= ~__ShaderChTexture_Cubic;

            mTexIDList[0] = tex->getOriginID();
            mTextures[0] = const_cast<Texture *>(tex);
            mTextures[0]->ref(false);

            if (mMark & __ShaderChTexture_AutoLoad)
            {
                load(); // reload
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setSrc(ResourceID rid, const StringList & src, Texture::Type type, Type ctype)
    {
        recover();
        mContentType = ctype;

        if (type == Texture::T_CUBE || type == Texture::T_CUBE_2D)
        {
            mMark &= ~__ShaderChTexture_LoadFailed;
            mMark |= __ShaderChTexture_Cubic;
            mTexIDList.resize(1);
            mTextures.resize(1);

            mTexIDList[0] = rid;
            mTextures[0] = static_cast<Texture *>(N_Only(Texture).ref(rid, mResGroupID).first);
            mTextures[0]->setUnitResource(src);
            mTextureType = Texture::T_CUBE;
            mAnimDuration = 0;
            mCurrentFrame = 0;

            notify();
        }
        else
        {
            mMark &= ~(__ShaderChTexture_LoadFailed | __ShaderChTexture_Cubic);
            mTexIDList.resize(1);
            mTextures.resize(1);
            mTextureType = type;
            mCurrentFrame = 0;

            mTexIDList[0] = rid;
            if (rid != 0)
            {
                mTextures[0] = static_cast<Texture *>(N_Only(Texture).ref(rid, GroupUnknow).first);
                mTextures[0]->setUnitResource(src);
                if (mMark & __ShaderChTexture_AutoLoad)
                {
                    load();
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setAni(const ResourceIDList & rid, TimeDurMS duration)
    {
        mContentType = ShaderChTexture::T_NORMAL;
        recover();

        mMark &= ~(__ShaderChTexture_LoadFailed | __ShaderChTexture_Cubic);
        mAnimDuration = duration;
        mCurrentFrame = rid.size();

        mTexIDList.resize(mCurrentFrame);
        mTextures.resize(mCurrentFrame);

        for (NCount i = 0; i < mCurrentFrame; ++i)
        {
            mTexIDList[i] = rid[i];
            mTextures[i] = 0;
        }

        mCurrentFrame = 0;

        if (mMark & __ShaderChTexture_AutoLoad)
        {
            load();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setFusion(ResourceID fusion, const String & tex)
    {
        mContentType = ShaderChTexture::T_FUSION;
        mFrameFusion = fusion;
        mFusionTex = tex;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setContentType(Type type)
    {
        mContentType = type;
        if (type == ShaderChTexture::T_SHADOW || type == ShaderChTexture::T_FUSION)
        {
            recover();
            mTextures.resize(1);
            mTextures[0] = 0;
        }
    }
    //-----------------------------------------------------------------------
    bool ShaderChTextureUnit::isCubic() const
    {
        return mMark & __ShaderChTexture_Cubic;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setFrame(ResourceID rid, NCount frame)
    {
        mMark &= ~__ShaderChTexture_LoadFailed;
        if (frame < mTexIDList.size())
        {
            mTexIDList[frame] = rid;
            // reset pointer (don't populate until requested)
            if (mTextures[frame] != 0)
            {
                mTextures[frame]->unref();
                mTextures[frame] = 0;
            }
            if (mMark & __ShaderChTexture_AutoLoad)
            {
                load(); // reload
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::addFrame(ResourceID rid)
    {
        setContentType(ShaderChTexture::T_NORMAL);
        mMark &= ~__ShaderChTexture_LoadFailed;
        mTexIDList.push_back(rid);
        mTextures.push_back(0);

        if (mMark & __ShaderChTexture_AutoLoad)
        {
            load();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::removeFrame(NCount frameNumber)
    {
        mMark &= ~__ShaderChTexture_LoadFailed;
        if (frameNumber < mTexIDList.size())
        {
            mTexIDList.erase(mTexIDList.begin() + frameNumber);

            if (mTextures[frameNumber] != 0)
                mTextures[frameNumber]->unref();

            mTextures.erase(mTextures.begin() + frameNumber);

            if (mMark & __ShaderChTexture_AutoLoad)
            {
                load();
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setIsAlpha(bool isAlpha)
    {
        if (isAlpha)
            mMark |= __ShaderChTexture_Alpha;
        else
            mMark &= ~__ShaderChTexture_Alpha;
    }
    //-----------------------------------------------------------------------
    bool ShaderChTextureUnit::getIsAlpha() const
    {
        return mMark & __ShaderChTexture_Alpha;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setHardwareSRGB(bool b)
    {
        if (b)
            mMark |= __ShaderChTexture_HWSRGB;
        else
            mMark &= ~__ShaderChTexture_HWSRGB;
    }
    //-----------------------------------------------------------------------
    bool ShaderChTextureUnit::isHardwareSRGB() const
    {
        return mMark & __ShaderChTexture_HWSRGB;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setLocalIndex(ShaderChTexture * sct, Nidx16 index)            
    { 
        LocalIndex::iterator i = mLocalIndexList.find(sct);
        if(i == mLocalIndexList.end())
            mLocalIndexList.insert(Npair(sct, index));
        else
            i->second = index;
    }
    //-----------------------------------------------------------------------
    Nidx16 ShaderChTextureUnit::getLocalIndex(ShaderChTexture * sct) const                
    { 
        LocalIndex::iterator i = mLocalIndexList.find(sct);
        if(i == mLocalIndexList.end())
            return 0;
        return i->second;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setAutoLoad(bool b)
    {
        if (b)
        {
            mMark |= __ShaderChTexture_AutoLoad;
        }
        else
        {
            mMark &= ~__ShaderChTexture_AutoLoad;
        }
    }
    //-----------------------------------------------------------------------
    bool ShaderChTextureUnit::isAutoLoad() const
    {
        return mMark & __ShaderChTexture_AutoLoad;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setEnvMap(bool b)
    {
        if (b)
        {
            mMark |= __ShaderChTexture_Env;
        }
        else
        {
            mMark &= ~__ShaderChTexture_Env;
        }
    }
    //-----------------------------------------------------------------------
    bool ShaderChTextureUnit::isEnvMap() const
    {
        return mMark & __ShaderChTexture_Env;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setMatrix(const Matrix4f & m)
    {
        mMatrix = m;
        mMark &= ~__ShaderChTexture_RecalcMatrix;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setUOft(NIIf value)
    {
        mUOft = value;
        mMark |= __ShaderChTexture_RecalcMatrix;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setVOft(NIIf value)
    {
        mVOft = value;
        mMark |= __ShaderChTexture_RecalcMatrix;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setUScale(NIIf value)
    {
        mUScale = value;
        mMark |= __ShaderChTexture_RecalcMatrix;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setVScale(NIIf value)
    {
        mVScale = value;
        mMark |= __ShaderChTexture_RecalcMatrix;
    }
    //-----------------------------------------------------------------------
    const Matrix4f & ShaderChTextureUnit::getMatrix() const
    {
        if (mMark & __ShaderChTexture_RecalcMatrix)
        {
            // Assumption: 2D texture coords
            mMatrix = Matrix4f::IDENTITY;
            if (mUScale != 1 || mVScale != 1)
            {
                // Offset to center of texture
                mMatrix[0][0] = 1 / mUScale;
                mMatrix[1][1] = 1 / mVScale;
                // Skip matrix concat since first matrix update
                mMatrix[0][3] = (-0.5f * mMatrix[0][0]) + 0.5f;
                mMatrix[1][3] = (-0.5f * mMatrix[1][1]) + 0.5f;
            }

            if (mUOft || mVOft)
            {
                Matrix4f xlate = Matrix4f::IDENTITY;

                xlate[0][3] = mUOft;
                xlate[1][3] = mVOft;

                mMatrix = xlate * mMatrix;
            }

            if (mRotate != Radian(0))
            {
                Matrix4f rot = Matrix4f::IDENTITY;
                Radian theta(mRotate);
                NIIf cosTheta = Math::Cos(theta);
                NIIf sinTheta = Math::Sin(theta);

                rot[0][0] = cosTheta;
                rot[0][1] = -sinTheta;
                rot[1][0] = sinTheta;
                rot[1][1] = cosTheta;
                // Offset center of rotation to center of texture
                rot[0][3] = 0.5f + ((-0.5f * cosTheta) - (-0.5f * sinTheta));
                rot[1][3] = 0.5f + ((-0.5f * sinTheta) + (-0.5f * cosTheta));

                mMatrix = rot * mMatrix;
            }
            mMark &= ~__ShaderChTexture_RecalcMatrix;
        }
        return mMatrix;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::plan()
    {
        for (NCount i = 0; i < mTexIDList.size(); ++i)
        {
            if (mTexIDList[i] != 0 && !(mMark & __ShaderChTexture_LoadFailed))
            {
                // Ensure texture is loaded, specified number of mipmaps and
                // priority
                if (mTextures[i] == 0)
                {
                    try
                    {
                        mTextures[i] = N_Only(Texture).plan(mTexIDList[i], mResGroupID,
                            mTextureType, mFormat, mMipmapCount, 1.0f, mMark);
                    }
                    catch (Exception & e)
                    {
                        mTextures[i] = N_Only(Texture).getWarning();
                        mTextures[i]->ref(false);
                        String msg;
                        msg = msg + _I18n("Error loading texture ") + N_conv(mTexIDList[i]) +
                            _I18n(". Texture layer will be blank. Loading the texture failed with the following exception: ")
                            + e.getErrorVerbose();
                        N_Only(Log).log(msg);
                        mMark |= __ShaderChTexture_LoadFailed;
                    }
                }
                else
                {
                    // Just ensure existing pointer is prepared
                    mTextures[i]->plan();
                    mTextures[i]->ref(false);
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::load()
    {
        for (NCount i = 0; i < mTexIDList.size(); ++i)
        {
            if (mTexIDList[i] != 0 && !(mMark & __ShaderChTexture_LoadFailed))
            {
                // Ensure texture is loaded, specified number of mipmaps and
                // priority
                if (mTextures[i] == 0)
                {
                    try
                    {
                        mTextures[i] = N_Only(Texture).load(mTexIDList[i], mResGroupID,
                            mTextureType, mFormat, mMipmapCount, 1.0f, mMark);
                    }
                    catch (Exception & e)
                    {
                        mTextures[i] = N_Only(Texture).getWarning();
                        mTextures[i]->ref(false);
                        String msg;
                        msg = msg + _I18n("不能加载 ") + N_conv(mTexIDList[i]) + _I18n(". 异常: ") + e.getErrorVerbose();
                        N_Only(Log).log(msg);
                        mMark |= __ShaderChTexture_LoadFailed;
                    }
                }
                else
                {
                    // Just ensure existing pointer is loaded
                    mTextures[i]->load();
                }
            }
        }

        if (mAnimDuration != 0)
        {
            if (mTimeFunc)
            {
                N_Only(Equation).destroy(mTimeFunc);
                mTimeFunc = 0;
            }
            mTimeFunc = N_Only(Material).createAnimation(this, mAnimDuration);
        }

        // Effect controllers
        for (EffectMap::iterator it = mEffects.begin(); it != mEffects.end(); ++it)
        {
            TextureEffect & effect = it->second;
            if (effect.mTimer)
            {
                N_Only(Equation).destroy(effect.mTimer);
                effect.mTimer = 0;
            }
            switch (effect.mType)
            {
            case TTT_UV_OFT:
                effect.mTimer = N_Only(Material).createUVScroll(this, effect.mFactor);
                break;
            case TTT_U_OFT:
                effect.mTimer = N_Only(Material).createUScroll(this, effect.mFactor);
                break;
            case TTT_V_OFT:
                effect.mTimer = N_Only(Material).createVScroll(this, effect.mFactor);
                break;
            case TTT_ROTATE:
                effect.mTimer = N_Only(Material).createRotate(this, effect.mFactor);
                break;
            case TTT_FUNCTION:
                effect.mTimer = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), effect.mIn, effect.mOutFunc);
                break;
            default:
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    const Texture * ShaderChTextureUnit::getTexture(NCount frame) const
    {
        if (mContentType == ShaderChTexture::T_NORMAL)
        {
            if (frame < mTexIDList.size() && !(mMark & __ShaderChTexture_LoadFailed))
            {
                if (mTexIDList[frame] != 0)
                {
                    // Ensure texture is loaded, specified number of mipmaps and
                    // priority
                    if (mTextures[frame] == 0)
                    {
                        try
                        {
                            const_cast<ShaderChTextureUnit *>(this)->mTextures[frame] =
                                N_Only(Texture).load(mTexIDList[frame], mResGroupID,
                                    mTextureType, mFormat, mMipmapCount, 1.0f, mMark);
                        }
                        catch (Exception & e)
                        {
                            mTextures[frame] = N_Only(Texture).getWarning();
                            mTextures[frame]->ref(false);
                            String msg;
                            msg = msg + _I18n("不能加载 ") + N_conv(mTexIDList[frame]) + _I18n(". 异常: ") + e.getErrorVerbose();
                            N_Only(Log).log(msg);
                            mMark |= __ShaderChTexture_LoadFailed;
                        }
                    }
                    else
                    {
                        // Just ensure existing pointer is loaded
                        mTextures[frame]->load();
                    }
                }
                return mTextures[frame];
            }
            else
            {
                return 0;
            }
        }
        else
        {
            // Manually bound texture, no name or loading
            N_assert(frame < mTextures.size(), "error");
            return mTextures[frame];
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setTexture(const Texture * tex, NCount frame)
    {
        N_assert(frame < mTextures.size(), "error");
        if (mTextures[frame] != 0)
        {
            mTextures[frame]->unref();
        }
        if (tex != 0)
        {
            mTextures[frame] = const_cast<Texture *>(tex);
            mTextures[frame]->ref(false);
        }
        else
        {
            mTextures[frame] = 0;
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setRotate(const Radian & r)
    {
        mRotate = r;
        mMark |= __ShaderChTexture_RecalcMatrix;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::unplan()
    {
        TextureList::iterator i, iend = mTextures.end();
        for (i = mTextures.begin(); i != iend; ++i)
        {
            (*i)->unref();
            (*i) = 0;
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::unload()
    {
        if (mTimeFunc)
        {
            N_Only(Equation).destroy(mTimeFunc);
            mTimeFunc = 0;
        }

        TextureList::iterator i, iend = mTextures.end();
        for (i = mTextures.begin(); i != iend; ++i)
        {
            (*i)->unref();
            (*i) = 0;
        }

        EffectMap::iterator j, jend = mEffects.end();
        for (j = mEffects.begin(); j != jend; ++j)
        {
            if (j->second.mTimer)
            {
                N_Only(Equation).destroy(j->second.mTimer);
                j->second.mTimer = 0;
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::unload()
    {
    }
    //-----------------------------------------------------------------------
    NCount ShaderChTextureUnit::ref()
    {
        ++mRefCount;
        return mRefCount;
    }
    //---------------------------------------------------------------------------
    NCount ShaderChTextureUnit::unref()
    {
        --mRefCount;
        if(mRefCount == 0 && mAutoDestroy)
            mMag->recover(this);
        return mRefCount;
    }
    //-----------------------------------------------------------------------
    bool ShaderChTextureUnit::isLoad() const
    {
        TextureList::iterator i, iend = mTextures.end();
        for (i = mTextures.begin(); i != iend; ++i)
        {
            if(!(*i)->isLoaded())
               return false;
        }
        return true;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::notify()
    {
        ShaderChTextureList::iterator i, iend = mApplyList.end();
        for(i = mApplyList.begin(); i != iend; ++i)
        {
            (*i)->notify();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::addRef(const ShaderChTexture * sct)
    {
        ShaderChTextureList::iterator i, iend = mApplyList.end();
        for(i = mApplyList.begin(); i != iend; ++i)
        {
            if(*i == sct)
                return;
        }
        mApplyList.push_back(sct);
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::removeRef(const ShaderChTexture * sct)
    {
        ShaderChTextureList::iterator i, iend = mApplyList.end();
        for(i = mApplyList.begin(); i != iend; ++i)
        {
            if(*i == sct)
            {
                mApplyList.erase(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    bool ShaderChTextureUnit::isMatrixRelation() const
    {
        return mEffects.size();
    }
    //-----------------------------------------------------------------------
    bool ShaderChTextureUnit::isCameraRelation() const
    {
        // Right now this only returns true for reflection maps
        if ((mMark & __ShaderChTexture_Env) && (mEnvMapType == ENM_Reflection))
            return true;

        if (mMark & __ShaderChTexture_Proj)
            return true;

        return false;
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::recover()
    {
        TextureList::iterator i, iend = mTextures.end();
        for (i = mTextures.begin(); i != iend; ++i)
        {
            if ((*i) != 0)
            {
                (*i)->unref();
            }
        }
        mTextures.clear();
        mTexIDList.clear();
    }
    //-----------------------------------------------------------------------
    void ShaderChTextureUnit::setProjTexture(bool b)
    {
        if (b)
        {
            mMark |= __ShaderChTexture_Proj;
        }
        else
        {
            mMark &= ~__ShaderChTexture_Proj;
        }
    }
    //-----------------------------------------------------------------------
    bool ShaderChTextureUnit::isProjTexture() const
    {
        return mMark & __ShaderChTexture_Proj;
    }
    //-----------------------------------------------------------------------
}