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
#include "NiiMaterial.h"
#include "NiiSpaceTypeManager.h"
#include "NiiMaterialManager.h"
#include "NiiLogManager.h"
#include "NiiException.h"
#include "NiiStrConv.h"
#include "NiiMaterialLodScheme.h"
#include "NiiLodSchemeManager.h"
#include "NiiDefine.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiGpuProgramManager.h"

#define __CPL_Disable     0x01
#define __CPL_Compile     0x02
#define __CPL_Mark        0x03        // 完成检测/编译 不支持
#define __CPL_Not_Mark    0xFFFFFFF8
#define __CPL_Valid       0x08

#define __N_M_ReceiveShadows 0x01
#define __N_M_AlphaShadowCast 0x02
#define __N_M_IntelDirty 0x04

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ShaderFusion
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ShaderFusion::ShaderChLink ShaderFusion::mChLink;
    ShaderChList ShaderFusion::mDetachList;
    //------------------------------------------------------------------------
    ShaderFusion::Fusion::Fusion()
    {
    }
    //------------------------------------------------------------------------
    ShaderFusion::Fusion::~Fusion()
    {
    }
    //------------------------------------------------------------------------
    ShaderFusion::ShaderFusion(Material * parent) :
        mParent(parent),
        mLodIndex(0),
        mSchemeID(0),
        mMark(0)
    {
    }
    //------------------------------------------------------------------------
    ShaderFusion::ShaderFusion(Material * parent, const ShaderFusion & o) :
        mParent(parent),
        mLodIndex(0),
        mSchemeID(0)
    {
        *this = o;
    }
    //------------------------------------------------------------------------
    ShaderFusion::~ShaderFusion()
    {
        removeAll();

        FusionShaderList::iterator i, iend = mFusionChList.end();
        for(i = mFusionChList.begin(); i != iend; ++i)
        {
            if((*i)->mDest->isFusion())
            {
                N_delete (*i)->mDest;
            }
            N_delete *i;
        }
        mFusionChList.clear();
    }
    //---------------------------------------------------------------------
    bool ShaderFusion::isValid() const
    {
        return (mMark & __CPL_Valid) && !(mMark & __CPL_Disable);
    }
    //---------------------------------------------------------------------
    void ShaderFusion::add(const GpuVendorDetail & rule)
    {
        GpuVendorDetailList::iterator i, iend = mVendorList.end();
        for(i = mVendorList.begin(); i != iend; ++i)
        {
            if(i->mVendor == rule.mVendor && i->mSer == rule.mSer)
            {
                mVendorList.erase(i);
                break;
            }
        }
        mVendorList.push_back(rule);
    }
    //---------------------------------------------------------------------
    void ShaderFusion::remove(const GpuVendorDetail & rule)
    {
        GpuVendorDetailList::iterator i, iend = mVendorList.end();
        for(i = mVendorList.begin(); i != iend;)
        {
            if(i->mVendor == rule.mVendor && i->mSer == rule.mSer)
                i = mVendorList.erase(i);
            else
                ++i;
        }
    }
    //------------------------------------------------------------------------
    ShaderCh * ShaderFusion::create()
    {
        ShaderCh * re = N_new ShaderCh(this, mChList.size());
        mChList.push_back(re);
        return re;
    }
    //------------------------------------------------------------------------
    ShaderCh * ShaderFusion::get(const String & name)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            if((*i)->getName() == name)
            {
                return *i;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ShaderFusion::remove(Nidx index)
    {
        N_assert(index < mChList.size(), "Index out of bounds");
        ShaderChList::iterator i = mChList.begin();
        std::advance(i, index);
        detach(*i);
        i = mChList.erase(i);

        for(; i != mChList.end(); ++i, ++index)
        {
            (*i)->mIndex = index;
        }
    }
    //------------------------------------------------------------------------
    void ShaderFusion::removeAll()
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            detach(*i);
        }
        mChList.clear();
    }
    //---------------------------------------------------------------------
    String ShaderFusion::check(bool fix)
    {
        StringStream errors;

        const RenderFeature * caps = N_Engine().getRender()->getFeature();
        GpuVendorDetailList::const_iterator z, zend = mVendorList.end();
        for(z = mVendorList.begin(); z != zend; ++z)
        {
            errors << GpuVendorDetail::toStr(z->mVendor) << z->mSer << N_conv(z->mNum) << " ";

            if(z->mVendor == caps->getGpuInfo()->getVendor() && z->mSer == caps->getGpuInfo()->getVendorSer() &&
                z->mNum <= caps->getGpuInfo()->getVendorSerNum())
            {
                mMark = mMark | __CPL_Valid;
                break;
            }
        }

        if(mMark & __CPL_Valid)
        {
            Nmark shadermark;
            Nui16 pindex = 0;
            NCount chtex;
            NCount rsftex = caps->getMaxTexUnit();
            ShaderChList::iterator z, zend = mChList.end();
            for(z = mChList.begin(); z != zend; ++z, ++pindex)
            {
                ShaderCh * pass = *z;
                pass->mIndex = pindex;
                shadermark = 0;

                if((pass->getBlend().getBlendMode() != CBM_ADD ||
                    pass->getBlend().getAlphaBlendMode() != CBM_ADD) && !caps->isSupport(GF_Colour_Blend))
                {
                    mMark &= __CPL_Mark;
                    break;
                }
                
                chtex = pass->getTexture().getCount(ShaderChTexture::T_NORMAL);
                if(!pass->getProgram().isExist(ShaderChProgram::ST_FS))
                {
    #if defined(NII_Max_TextureUnit) && NII_Max_TextureUnit > 0
                    if(rsftex > NII_Max_TextureUnit)
                        rsftex = NII_Max_TextureUnit;
    #endif
                    if(chtex > rsftex)
                    {
                        if(!fix)
                        {
                            errors << "着色通路 " << pindex << ": 太多纹理单元" << std::endl;
                            mMark &= __CPL_Mark;
                            break;
                        }
                        else if(pass->getProgram().isExist(ShaderChProgram::ST_VS))
                        {
                            errors << "着色通路 " << pindex << ": 太多纹理单元, 而且无法分割顶点着色程序的通道 " << std::endl;
                            mMark &= __CPL_Mark;
                            break;
                        }
                    }
                }
                GpuProgram * prog;
                pass->getProgram().get(ShaderChProgram::ST_VS, prog);
                if(prog)
                {
                    if(!prog->isValid())
                    {
                        errors << "着色通路: " << pindex << "不能使用" << prog->getOriginID() << "顶点着色程序,因为";
                        if(prog->getErrorCode() != 0)
                            errors << "编译错误.";
                        else
                            errors << "不支持.";
                        errors << std::endl;
                        mMark &= __CPL_Mark;
                        break;
                    }
                    shadermark |= ShaderChProgram::ST_VS;
                }
                pass->getProgram().get(ShaderChProgram::ST_GS, prog);
                if(prog)
                {
                    if(!prog->isValid())
                    {
                        errors << "着色通路: " << pindex << "不能使用" << prog->getOriginID() << "几何着色程序";
                        if(prog->getErrorCode() != 0)
                            errors << "编译错误.";
                        else
                            errors << "不支持.";
                        errors << std::endl;
                        mMark &= __CPL_Mark;
                        break;
                    }
                    shadermark |= ShaderChProgram::ST_GS;
                }
                pass->getProgram().get(ShaderChProgram::ST_FS, prog);
                if(prog)
                {
                    if(!prog->isValid())
                    {
                        errors << "着色通路: " << pindex << "不能使用" << prog->getOriginID() << "片段着色程序";
                        if(prog->getErrorCode() != 0)
                            errors << "编译错误.";
                        else
                            errors << "不支持.";
                        errors << std::endl;
                        mMark &= __CPL_Mark;
                        break;
                    }
                    shadermark |= ShaderChProgram::ST_FS;
                }
                else
                {
                    // Check a few fixed-function options in texture layers
                    NCount texUnit = 0;
                    ShaderChTexture::Textures::const_iterator i, iend = pass->setTexture(ShaderCh::SOT_Set)->getTextures().end();
                    for(i = pass->setTexture(ShaderCh::SOT_Set)->getTextures().begin(); i != iend; ++i, ++texUnit)
                    {
                        ShaderChTextureUnit * tex = *i;
                        // Any Cube textures? NB we make the assumption that any
                        // card capable of running fragment programs can support
                        // cubic textures, which has to be true, surely?
                        /*if(tex->getType() == Texture::T_CUBE && !caps->isSupport(GF_Tex_Cube))
                        {
                            errors << "着色通路: " << pindex << " 纹理单元 " << texUnit << ",当前不支持Cube纹理" << std::endl;
                            mMark &= __CPL_Mark;
                            break;
                        }*/
                        // Any 3D textures? NB we make the assumption that any
                        // card capable of running fragment programs can support
                        // 3D textures, which has to be true, surely?
                        if((tex->getType() == Texture::T_3D || tex->getType() == Texture::T_2DArray) && !caps->isSupport(GF_Tex_3D))
                        {
                            errors << "着色通路: " << pindex << " 纹理单元 " << texUnit << ",当前不支持容积纹理" << std::endl;
                            mMark &= __CPL_Mark;
                            break;
                        }

                        if(tex->getColourBlend()->mOP == TBO_DOT_PRODUCT && !caps->isSupport(GF_Tex_Dot3))
                        {
                            errors << "着色通路: " << pindex << " 纹理单元 " << texUnit << ",当前不支持DOT3混合" << std::endl;
                            mMark &= __CPL_Mark;
                            break;
                        }
                    }

                    // We're ok on operations, now we need to check # texture units
                    //if(!pass->getProgram().isExist(ShaderChProgram::ST_FS))
                    {
                        ShaderChTexture * sct = pass->setTexture(ShaderCh::SOT_Set);
                        if((shadermark & (ShaderChProgram::ST_VS | ShaderChProgram::ST_FS | ShaderChProgram::ST_GS)) != 0)
                        {
                            N_EXCEPT(InvalidParam, _I18n("有着色程序的渲染通路不能分割! "));
                        }
                        // Keep splitting this pass so NIIl as units requested > gpu units
                        while(chtex > rsftex)
                        {
                            // chop this pass into many passes
                            if(sct->getCount(ShaderChTexture::T_NORMAL) > rsftex)
                            {
                                ShaderCh * ch = pass->mParent->create();
                                ShaderChTexture * dst = ch->setTexture(ShaderCh::SOT_Set);
                                ShaderChTextureUnit * f = sct->get(ShaderChTexture::T_NORMAL, rsftex);
                                // Set the new pass to fallback using scene blend
                                ch->setBlend(ShaderCh::SOT_Set)->setBlend(CF_SRC_ALPHA, CF_DST_ALPHA);
                                // Fixup the texture unit 0   of new pass   blending method   to replace
                                // all colour and alpha   with texture without adjustment, because we
                                // assume it's detail texture.
                                f->getColourBlend()->setBlend(TBO_OPERAND1, TBS_TEXTURE, TBS_PRE_TEXTURE);
                                f->getAlphaBlend()->setBlend(TBO_OPERAND1, TBS_TEXTURE, TBS_PRE_TEXTURE);

                                sct->move(ShaderChTexture::T_NORMAL, rsftex,
                                    sct->getCount(ShaderChTexture::T_NORMAL) - 1, dst);

                                pass = ch;
                                sct = dst;
                            }

                            chtex = sct->getCount(ShaderChTexture::T_NORMAL);
                            // Advance pass number
                            ++pindex;
                            // Reset iterator
                            z = mChList.begin() + pindex;
                            // Move the new pass to the right place (will have been created
                            // at the end, may be other passes in between)
                            N_assert(mChList.back() == pass, "error");
                            std::copy_backward(z, (mChList.end()-1), mChList.end());
                            *z = pass;
                            // Adjust pass index
                            pass->mIndex = pindex;
                        }
                    }
                }
            }
        }

        FusionShaderList::iterator f, fend = mFusionChList.end();
        for(f = mFusionChList.begin(); f != fend; ++f)
        {
            if((*f)->mDest->isFusion())
            {
                N_delete (*f)->mDest;
            }
            N_delete *f;
        }
        mFusionChList.clear();
        return errors.str();
    }
    //------------------------------------------------------------------------
    ShaderFusion & ShaderFusion::operator=(const ShaderFusion & o)
    {
        mName = o.mName;
        mMark = o.mMark;
        mSchemeID = o.mSchemeID;
        mLodIndex = o.mLodIndex;
        mShadowCaster = o.mShadowCaster;
        mShadowReceiver = o.mShadowReceiver;
        mVendorList = o.mVendorList;

        removeAll();
        ShaderChList::const_iterator i, iend = o.mChList.end();
        for(i = o.mChList.begin(); i != iend; ++i)
        {
            ShaderCh * p = N_new ShaderCh(this, (*i)->getIndex(), *(*i));
            mChList.push_back(p);
        }

        FusionShaderList::iterator j, jend = mFusionChList.end();
        for(j = mFusionChList.begin(); j != jend; ++j)
        {
            if((*j)->mDest->isFusion())
            {
                N_delete (*j)->mDest;
            }
            N_delete *j;
        }
        mFusionChList.clear();
        mMark = mMark & __CPL_Not_Mark;
        return *this;
    }
    //------------------------------------------------------------------------
    bool ShaderFusion::isTransparent() const
    {
        if(mChList.empty())
        {
            return false;
        }
        else
        {
            return mChList[0]->getBlend().isTransparent();
        }
    }
    //------------------------------------------------------------------------
    bool ShaderFusion::isDepthSort() const
    {
        if(mChList.empty())
        {
            return true;
        }
        else
        {
            return mChList[0]->mDepthSort;
        }
    }
    //------------------------------------------------------------------------
    bool ShaderFusion::isForceDepthSort() const
    {
        if(mChList.empty())
        {
            return false;
        }
        else
        {
            return mChList[0]->mForceDepthSort;
        }
    }
    //------------------------------------------------------------------------
    bool ShaderFusion::isDepthWrite() const
    {
        if(mChList.empty())
        {
            return false;
        }
        else
        {
            return mChList[0]->getStencil().isDepthWrite();
        }
    }
    //------------------------------------------------------------------------
    bool ShaderFusion::isDepthEnable() const
    {
        if(mChList.empty())
        {
            return false;
        }
        else
        {
            return mChList[0]->getStencil().isDepthEnable();
        }
    }
    //------------------------------------------------------------------------
    Nmark ShaderFusion::getColourWrite() const
    {
        if(mChList.empty())
        {
            return true;
        }
        else
        {
            return mChList[0]->getColourWrite();
        }
    }
    //------------------------------------------------------------------------
    void ShaderFusion::plan()
    {
        N_assert1(mMark & __CPL_Valid);

        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            (*i)->plan();
        }

        FusionShaderList::iterator j, jend = mFusionChList.end();
        for(j = mFusionChList.begin(); j != jend; ++j)
        {
            if((*j)->mDest != (*j)->mSrc)
                (*j)->mDest->plan();
        }
    }
    //------------------------------------------------------------------------
    void ShaderFusion::unplan()
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            (*i)->unplan();
        }
    }
    //------------------------------------------------------------------------
    void ShaderFusion::load()
    {
        N_assert1(mMark & __CPL_Valid);

        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            (*i)->load();
        }

        FusionShaderList::iterator j, jend = mFusionChList.end();
        for(j = mFusionChList.begin(); j != jend; ++j)
        {
            if((*j)->mDest != (*j)->mSrc)
                (*j)->mDest->load();
        }

        if(mShadowCaster != 0)
        {
            mShadowCaster->load();
        }

        if(mShadowReceiver != 0)
        {
            mShadowReceiver->load();
        }
    }
    //------------------------------------------------------------------------
    void ShaderFusion::unload()
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            (*i)->unload();
        }
    }
    //------------------------------------------------------------------------
    bool ShaderFusion::isLoad() const
    {
        return (mParent->getState() == LS_LOADED) && (mMark & __CPL_Valid);
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setPoint(SetOperatorType type, const ShaderChPoint * p)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            i->setPoint(type, p);
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setColour(SetOperatorType type, const ShaderChColour * c)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            i->setColour(type, c);
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setFog(SetOperatorType type, const ShaderChFog * f)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            i->setFog(type, f);
        }
    }
    //--------------------------------------------------------------------
    void ShaderFusion::setBlend(SetOperatorType type, const ShaderChBlend * b)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            i->setBlend(type, b);
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setColourWrite(Nmark mark)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            (*i)->setColourWrite(mark);
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setCullingMode(CullingMode mode)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            (*i)->setCullingMode(mode);
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setSysCullingMode(SysCullingMode mode)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            (*i)->setSysCullingMode(mode);
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setLight(bool b)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            (*i)->setLight(b);
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setShadeMode(ShadeMode mode)
    {
        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            (*i)->setShadeMode(mode);
        }
    }
    //---------------------------------------------------------------------
    void ShaderFusion::notify()
    {
        if((mMark & __CPL_Disable) == 0)
        {
            mParent->notify();
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setLodIndex(Nidx16 index)
    {
        mLodIndex = index;
        notify();
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::setSchemeID(SchemeID sid)
    {
        mSchemeID = sid;
        notify();
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::fusion()
    {
        bool gtemp = false;
        FusionShaderList::iterator z, zend = mFusionChList.end();
        for(z = mFusionChList.begin(); z != zend; ++z)
        {
            if((*z)->mDest->isFusion())
            {
                N_delete (*z)->mDest;
            }
            N_delete *z;
        }
        mFusionChList.clear();

        ShaderChList::iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            if((*i)->getShaderStep() == SS_Unknow)
            {
                gtemp = true;
                break;
            }
        }

        if(gtemp)
        {
            i = mChList.begin();

            ShaderStep iStage = SS_Global;

            bool haveAmbient = false;
            while(i != iend)
            {
                Fusion * fs;
                ShaderCh * p = *i;
                switch(iStage)
                {
                case SS_Global:
                {
                    // Keep looking for ambient only
                    bool temp = !p->isLight() || ((p->getMark() & SB_Colour_RGBA) == 0) ||
                        !p->getColour().isReflexColour();

                    if (temp)
                    {
                        // Add this pass wholesale
                        fs = N_new Fusion();
                        fs->mSrc = fs->mDest = p;
                        fs->mStep = iStage;
                        mFusionChList.push_back(fs);
                        haveAmbient = true;
                        // prniiss to next pass
                        ++i;
                    }
                    else
                    {
                        // Split off any ambient part
                        if (p->getColour().getAmbient() != Colour::Black ||
                            p->getColour().getEmissive() != Colour::Black ||
                            p->getColour().getAlphaReject() != CPM_ALWAYS_PASS)
                        {
                            // Copy existing pass
                            ShaderCh * fch = N_new ShaderCh(this, p->getIndex(), *p, true);
                            if (fch->getColour().getAlphaReject() != CPM_ALWAYS_PASS)
                            {
                                // Alpha rejection passes must retain their transparency, so
                                // we allow the texture units, but override the colour functions
                                ShaderChTexture::Textures::const_iterator i, iend = fch->setTexture(ShaderCh::SOT_Set)->getTextures().end();
                                for (i = fch->setTexture(ShaderCh::SOT_Set)->getTextures().begin(); i != iend; ++i)
                                {
                                    (*i)->getColourBlend()->setBlend(TBO_OPERAND1, TBS_PRE_TEXTURE, TBS_PRE_TEXTURE);
                                }
                            }
                            else
                            {
                                // Remove any texture units
                                fch->setTexture(ShaderCh::SOT_Set)->removeAll();
                            }
                            // Remove any fragment program
                            if (fch->getProgram().isExist(ShaderChProgram::ST_FS))
                                fch->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, 0, 0);
                            // We have to leave vertex program alone (if any) and
                            // just trust that the author is using light bindings, which
                            // we will ensure there are none in the ambient pass
                            fch->setColour(ShaderCh::SOT_Set)->setDiffuse(Colour(0, 0, 0, fch->getColour().getDiffuse().a));  // Preserving alpha
                            fch->setColour(ShaderCh::SOT_Set)->setSpecular(Colour::Black);

                            fs = N_new Fusion();
                            fs->mSrc = p;
                            fs->mDest = fch;
                            fs->mStep = iStage;

                            mFusionChList.push_back(fs);
                            haveAmbient = true;
                        }

                        if (!haveAmbient)
                        {
                            // Make up a new basic pass
                            ShaderCh * fch = N_new ShaderCh(this, p->getIndex(), true);
                            fch->setColour(ShaderCh::SOT_Set)->setAmbient(Colour::Black);
                            fch->setColour(ShaderCh::SOT_Set)->setDiffuse(Colour::Black);

                            fs = N_new Fusion();
                            fs->mSrc = p;
                            fs->mDest = fch;
                            fs->mStep = iStage;
                            mFusionChList.push_back(fs);
                            haveAmbient = true;
                        }
                        // This means we're done with ambients, prniiss to per-light
                        iStage = SS_Detail_Light;
                    }
                }
                break;
                case SS_Detail_Light:
                {
                    if (p->isEachLight())
                    {
                        // If this is per-light already, use it directly
                        fs = N_new Fusion();
                        fs->mSrc = fs->mDest = p;
                        fs->mStep = iStage;
                        mFusionChList.push_back(fs);
                        // prniiss to next pass
                        ++i;
                    }
                    else
                    {
                        // Split off per-light details (can only be done for one)
                        if (p->isLight() && p->getColour().isReflexColour())
                        {
                            // Copy existing pass
                            ShaderCh * fch = N_new ShaderCh(this, p->getIndex(), *p, true);
                            if (fch->getColour().getAlphaReject() != CPM_ALWAYS_PASS)
                            {
                                // Alpha rejection passes must retain their transparency, so
                                // we allow the texture units, but override the colour functions
                                ShaderChTexture::Textures::const_iterator i, iend = fch->setTexture(ShaderCh::SOT_Set)->getTextures().end();
                                for (i = fch->setTexture(ShaderCh::SOT_Set)->getTextures().begin(); i != iend; ++i)
                                {
                                    (*i)->getColourBlend()->setBlend(TBO_OPERAND1, TBS_PRE_TEXTURE, TBS_PRE_TEXTURE);
                                }
                            }
                            else
                            {
                                // remove texture units
                                fch->setTexture(ShaderCh::SOT_Set)->removeAll();
                            }
                            // remove fragment programs
                            if (fch->getProgram().isExist(ShaderChProgram::ST_FS))
                                fch->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, 0, 0);
                            // Cannot remove vertex program, have to assume that
                            // it will process diffuse lights, ambient will be turned off
                            fch->setColour(ShaderCh::SOT_Set)->setAmbient(Colour::Black);
                            fch->setColour(ShaderCh::SOT_Set)->setEmissive(Colour::Black);
                            // must be additive
                            fch->setBlend(ShaderCh::SOT_Set)->setBlend(CF_ONE, CF_ONE);

                            fs = N_new Fusion();
                            fs->mSrc = p;
                            fs->mDest = fch;
                            fs->mStep = iStage;

                            mFusionChList.push_back(fs);
                        }
                        // This means the end of per-light passes
                        iStage = SS_Decal;
                    }
                }
                break;
                case SS_Decal:
                {
                    // We just want a 'lighting off' pass to finish off
                    // and only if there are texture units
                    if (p->getTexture().getCount(ShaderChTexture::T_NORMAL) > 0)
                    {
                        if (!p->isLight())
                        {
                            // we assume this pass already combines as required with the scene
                            fs = N_new Fusion();
                            fs->mSrc = fs->mDest = p;
                            fs->mStep = iStage;
                            mFusionChList.push_back(fs);
                        }
                        else
                        {
                            // Copy the pass and tweak away the lighting parts
                            ShaderCh * fch = N_new ShaderCh(this, p->getIndex(), *p, true);
                            fch->setColour(ShaderCh::SOT_Set)->setAmbient(Colour::Black);
                            fch->setColour(ShaderCh::SOT_Set)->setDiffuse(Colour(0, 0, 0, fch->getColour().getDiffuse().a));  // Preserving alpha
                            fch->setColour(ShaderCh::SOT_Set)->setSpecular(Colour::Black);
                            fch->setColour(ShaderCh::SOT_Set)->setEmissive(Colour::Black);
                            fch->setLight(false);
                            fch->setEachLight(false);
                            fch->setLightTypeLimit(LT_NONE);
                            // modulate
                            fch->setBlend(ShaderCh::SOT_Set)->setBlend(CF_DST, CF_ZERO);

                            // NB there is nothing we can do about vertex & fragment
                            // programs here, so people will just have to make their
                            // programs friendly-like if they want to use this technique
                            fs = N_new Fusion();
                            fs->mSrc = p;
                            fs->mDest = fch;
                            fs->mStep = iStage;
                            mFusionChList.push_back(fs);

                        }
                    }
                    ++i; // always increment on decal, since nothing more to do with this pass
                }
                break;
                case SS_Unknow:
                    break;
                }
            }
        }
        else
        {
            for(i = mChList.begin(); i != iend; ++i)
            {
                Fusion * fs = N_new Fusion();
                fs->mSrc = fs->mDest = *i;
                fs->mStep = (*i)->getShaderStep();
                mFusionChList.push_back(fs);
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::attach(ShaderCh * obj)
    {
        ShaderChLink::iterator i = mChLink.find(obj);
        if(i != mChLink.end())
        {
            ShaderFusionList::iterator j, jend = i->second.end();
            for(j = i->second.begin(); j != jend; ++j)
            {
                if (this == *j)
                    return;
            }
            i->second.push_back(this);
        }
        else
        {
            std::pair<ShaderChLink::iterator, bool> in =
                mChLink.insert(std::pair<ShaderCh *, ShaderFusionList>(obj, ShaderFusionList()));
            in.first->second.push_back(this);
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::detach(ShaderCh * obj)
    {
        ShaderChLink::iterator i = mChLink.find(obj);
        if(i != mChLink.end())
        {
            ShaderFusionList::iterator j, jend = i->second.end();
            for(j = i->second.begin(); j != jend; ++j)
            {
                if(this == *j)
                {
                    i->second.erase(j);
                    break;
                }
            }

            if (i->second.empty())
            {
                obj->setTexture(ShaderCh::SOT_Set)->removeAll();
                obj->setProgram(ShaderCh::SOT_Set)->removeAll();
                mDetachList.push_back(obj);
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderFusion::recover()
    {
        ShaderChList::iterator i, iend = mDetachList.end();
        for (i = mDetachList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mDetachList.clear();
    }
    //-----------------------------------------------------------------------
    ShaderFusion::FusionShaderList & ShaderFusion::getFusionShaderList()
    {
        if((mMark & __CPL_Compile) == 0)
        {
            mMark = (mMark & __CPL_Not_Mark) | __CPL_Disable;

            fusion();

            mMark = (mMark & __CPL_Not_Mark) | __CPL_Compile;
        }

        return mFusionChList;
    }
    //-----------------------------------------------------------------------
    GroupID ShaderFusion::getResourceGroup() const
    {
        return mParent->getGroup();
    }
    //-----------------------------------------------------------------------
    ShaderFusion * ShaderFusion::clone(Material * o)
    {
        ShaderFusion * t = o->create();
        t->mName = mName;
        t->mMark = mMark;
        t->mSchemeID = mSchemeID;
        t->mLodIndex = mLodIndex;
        t->mShadowCaster = mShadowCaster;
        t->mShadowReceiver = mShadowReceiver;
        t->mVendorList = mVendorList;

        ShaderChList::const_iterator i, iend = mChList.end();
        for(i = mChList.begin(); i != iend; ++i)
        {
            ShaderCh * p = N_new ShaderCh(t, (*i)->getIndex(), *(*i));
            t->mChList.push_back(p);
        }
        t->mMark = t->mMark & __CPL_Not_Mark;
        return t;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Material
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Material::ShaderFusionLink Material::mFusionLink;
    //-----------------------------------------------------------------------
    bool Material::LessSort::operator()(const Material * x, const Material * y) const
    {
        if(x->isTransparent() && !y->isTransparent())
        {
            return false;
        }
        else if(!x->isTransparent() && y->isTransparent())
        {
            return true;
        }

        return x < y;
    }
    //-----------------------------------------------------------------------
    Material::Material(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, LangID lid) :
            Resource(N_CmdObj_Material, rid, gid, ls, rs, lid)
    {
        mCreator = N_OnlyPtr(Material);
        mLodScheme = static_cast<MaterialLodScheme *>(N_Only(LodScheme).createDefault(N_LOD_Material));
        Material * defaults = N_OnlyPtr(Material)->getNull();
        if(defaults != 0)
        {
            *this = *defaults;
        }
        mMark = __N_M_ReceiveShadows | __N_M_IntelDirty;
        setupCmd(N_CmdObj_Material);
    }
    //-----------------------------------------------------------------------
    Material::~Material()
    {
        if(mLodScheme != 0)
            N_delete mLodScheme;
        destroyAll();

        unload();
    }
    //-----------------------------------------------------------------------
    Material & Material::operator=(const Material & o)
    {
        destroyAll();
        ShaderFusionList::const_iterator i, iend = o.mFusions.end();
        for(i = o.mFusions.begin(); i != iend; ++i)
        {
            ShaderFusion * t = (*i)->clone(this);
            if((*i)->isValid())
            {
                mValidFusions.push_back(t);
                // get scheme
                SchemeID schemeIndex = t->getSchemeID();
                SchemeFusions::iterator i = mSchemeFusions.find(schemeIndex);
                LodList * lodtechs = 0;
                if(i == mSchemeFusions.end())
                {
                    lodtechs = N_new_t(LodList);
                    mSchemeFusions[schemeIndex] = lodtechs;
                }
                else
                {
                    lodtechs = i->second;
                }

                // Insert won't replace if supported technique for this scheme/lod is
                // already there, which is what we want
                lodtechs->insert(LodList::value_type(t->getLodIndex(), t));
            }
        }
        mLodScheme = static_cast<MaterialLodScheme *>(o.mLodScheme->clone());
        mSize = o.mSize;
        mState = o.mState;
        mMark = o.mMark;
        N_assert(mState == o.mState, "error");

        return *this;
    }
    //-----------------------------------------------------------------------
    void Material::ref(bool doload)
    {
        if(mMark & __N_M_IntelDirty)
            check();

        Resource::ref(doload);
    }
    //-----------------------------------------------------------------------
    void Material::planImpl()
    {
        if(mMark & __N_M_IntelDirty)
            check();

        ShaderFusionList::iterator i, iend = mValidFusions.end();
        for(i = mValidFusions.begin(); i != iend; ++i)
        {
            (*i)->plan();
        }
    }
    //-----------------------------------------------------------------------
    void Material::unplanImpl()
    {
        ShaderFusionList::iterator i, iend = mValidFusions.end();
        for(i = mValidFusions.begin(); i != iend; ++i)
        {
            (*i)->unplan();
        }
    }
    //-----------------------------------------------------------------------
    void Material::loadImpl()
    {
        ShaderFusionList::iterator i, iend = mValidFusions.end();
        for(i = mValidFusions.begin(); i != iend; ++i)
        {
            (*i)->load();
        }
    }
    //-----------------------------------------------------------------------
    void Material::unloadImpl()
    {
        ShaderFusionList::iterator i, iend = mValidFusions.end();
        for(i = mValidFusions.begin(); i != iend; ++i)
        {
            (*i)->unload();
        }
    }
    //-----------------------------------------------------------------------
    NCount Material::calcSize() const
    {
        return 0;
    }
    //-----------------------------------------------------------------------
    Material * Material::clone(ResourceID rid, GroupID gid) const
    {
        Material * re;
        if(gid != GroupUnknow)
        {
            re = static_cast<Material *>(N_Only(Material).create(rid, gid));
        }
        else
        {
            re = static_cast<Material *>(N_Only(Material).create(rid, mGroup));
        }
        *re = *this;
        return re;
    }
    //-----------------------------------------------------------------------
    ShaderFusion * Material::create()
    {
        ShaderFusion * re = N_new ShaderFusion(this);
        mFusions.push_back(re);
        mMark |= __N_M_IntelDirty;
        return re;
    }
    //-----------------------------------------------------------------------
    ShaderFusion * Material::get(const String & name)
    {
        ShaderFusionList::iterator i = mFusions.begin(), iend = mFusions.end();
        ShaderFusion * re = 0;

        while(i != iend)
        {
            if((*i)->getName() == name)
            {
                re = (*i);
                break;
            }
            ++i;
        }
        return re;
    }
    //-----------------------------------------------------------------------
    void Material::destroy(Nidx index)
    {
        N_assert(index < mFusions.size(), "Index out of bounds.");
        ShaderFusionList::iterator i = mFusions.begin();
        std::advance(i, index);
        N_delete(*i);
        mFusions.erase(i);
        mValidFusions.clear();
        SchemeFusions::iterator z, zend = mSchemeFusions.end();
        for(z = mSchemeFusions.begin(); z != zend; ++z)
        {
            N_delete_t(z->second, LodList);
        }
        mSchemeFusions.clear();
        mMark |= __N_M_IntelDirty;
    }
    //-----------------------------------------------------------------------
    void Material::destroyAll()
    {
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            N_delete (*i);
        }
        mFusions.clear();
        mValidFusions.clear();
        SchemeFusions::iterator j, jend = mSchemeFusions.end();
        for(j = mSchemeFusions.begin(); j != jend; ++j)
        {
            N_delete_t(j->second, LodList);
        }
        mSchemeFusions.clear();
        mMark |= __N_M_IntelDirty;
    }
    //-----------------------------------------------------------------------
    NCount Material::getLodCount(SchemeID sid) const
    {
        if(mSchemeFusions.empty())
            return 0;

        SchemeFusions::const_iterator i = mSchemeFusions.find(sid);
        if(i == mSchemeFusions.end())
        {
            // get the first item, will be 0 (the default) if default
            // scheme techniques exist, otherwise the earliest defined
            i = mSchemeFusions.begin();
        }
        return i->second->size();
    }
    //-----------------------------------------------------------------------
    ShaderFusion * Material::getLOD(const GeometryObj * obj, Nidx lod) const
    {
        if(mValidFusions.empty())
        {
            return NULL;
        }
        else
        {
            ShaderFusion * ret = 0;
            SchemeFusions::const_iterator si = mSchemeFusions.find(N_Only(Material).getCurrent());
            if(si == mSchemeFusions.end())
            {
                ret = N_Only(Material).match(obj, this, lod);
                if(ret)
                    return ret;
                si = mSchemeFusions.begin();
            }

            LodList::iterator li = si->second->find(lod);
            if(li == si->second->end())
            {
                LodList::reverse_iterator rli, rliend = si->second->rend();
                for(rli = si->second->rbegin(); rli != rliend; ++rli)
                {
                    if(rli->second->getLodIndex() < lod)
                    {
                        ret = rli->second;
                        break;
                    }
                }
                if(!ret)
                {
                    // shouldn't ever hit this really, unless user defines no LOD 0
                    // pick the first LOD we have (must be at least one to have a scheme entry)
                    ret = si->second->begin()->second;
                }
            }
            else
            {
                ret = li->second;
            }
            return ret;
        }
    }
    //-----------------------------------------------------------------------
    bool Material::isTransparent() const
    {
        ShaderFusionList::const_iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            if((*i)->isTransparent())
                return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void Material::setReceiveShadow(bool b)
    {
        if(b)
            mMark |= __N_M_ReceiveShadows;
        else
            mMark &= ~__N_M_ReceiveShadows;
    }
    //-----------------------------------------------------------------------
    bool Material::isReceiveShadow() const
    {
        return mMark & __N_M_ReceiveShadows;
    }
    //-----------------------------------------------------------------------
    void Material::setAlphaShadowCast(bool b)
    {
        if(b)
            mMark |= __N_M_AlphaShadowCast;
        else
            mMark &= ~__N_M_AlphaShadowCast;
    }
    //-----------------------------------------------------------------------
    bool Material::isAlphaShadowCast() const
    {
        return mMark & __N_M_AlphaShadowCast;
    }
    //-----------------------------------------------------------------------
    bool Material::check(bool fix)
    {
        // Compile each technique, then add it to the list of supported techniques
        mValidFusions.clear();
        SchemeFusions::iterator j, jend = mSchemeFusions.end();
        for(j = mSchemeFusions.begin(); j != jend; ++j)
        {
            N_delete_t(j->second, LodList);
        }
        mSchemeFusions.clear();
        mLastError.clear();

        NCount techNo = 0;
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i, ++techNo)
        {
            ShaderFusion * t = *i;
            String compileMessages = t->check(fix);
            if(t->isValid())
            {
                mValidFusions.push_back(t);
                // get scheme
                SchemeID schemeIndex = t->getSchemeID();
                SchemeFusions::iterator i = mSchemeFusions.find(schemeIndex);
                LodList * lodtechs = 0;
                if(i == mSchemeFusions.end())
                {
                    lodtechs = N_new_t(LodList);
                    mSchemeFusions[schemeIndex] = lodtechs;
                }
                else
                {
                    lodtechs = i->second;
                }

                // Insert won't replace if supported technique for this scheme/lod is
                // already there, which is what we want
                lodtechs->insert(LodList::value_type(t->getLodIndex(), t));
            }
            else
            {
                // Log informational
                StringStream str;
                str << _I18n("Material ") << mOriginID << _I18n(" ShaderFusion ") << techNo;
                if (!t->getName().empty())
                    str << "(" << t->getName() << ")";
                str << _I18n(" is not supported. ") << compileMessages;
                N_Only(Log).log(str.str(), LL_Trace);
                mLastError += compileMessages;
            }
        }

        mMark &= ~__N_M_IntelDirty;

        if(mValidFusions.empty())
        {
            N_Only(Log).stream()
                << _I18n("WARNING: material ") << mOriginID << _I18n(" has no supportable ")
                << _I18n("ShaderFusionList and will be blank. Explanation: \n") << mLastError;
            return false;
        }
        return true;
    }
    //-----------------------------------------------------------------------
    void Material::setFog(const ShaderChFog & f)
    {
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            (*i)->setFog(f);
        }
    }
    //-----------------------------------------------------------------------
    void Material::setPoint(const ShaderChPoint & p)
    {
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            (*i)->setPoint(p);
        }
    }
    //-----------------------------------------------------------------------
    void Material::setColour(const ShaderChColour & c)
    {
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            (*i)->setColour(c);
        }
    }
    //-----------------------------------------------------------------------
    void Material::setColourWrite(Nmark mark)
    {
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            (*i)->setColourWrite(mark);
        }
    }
    //-----------------------------------------------------------------------
    void Material::setCullingMode(CullingMode mode)
    {
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            (*i)->setCullingMode(mode);
        }
    }
    //-----------------------------------------------------------------------
    void Material::setSysCullingMode(SysCullingMode mode)
    {
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            (*i)->setSysCullingMode(mode);
        }
    }
    //-----------------------------------------------------------------------
    void Material::setLight(bool b)
    {
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            (*i)->setLight(b);
        }
    }
    //-----------------------------------------------------------------------
    void Material::setShadeMode(ShadeMode mode)
    {
        ShaderFusionList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            (*i)->setShadeMode(mode);
        }
    }
    //-----------------------------------------------------------------------
    void Material::notify()
    {
        Resource::notify();
        mMark |= __N_M_IntelDirty;

        if(mState == LS_LOADED)
            unload();
    }
    //-----------------------------------------------------------------------
    void Material::setLodScheme(MaterialLodScheme * obj)
    {
        if(mLodScheme != obj)
        {
            if(mLodScheme != 0)
            {
                N_delete mLodScheme;
            }

            mLodScheme = obj;

            if(mLodScheme)
            {
                mLodScheme->map();
            }
        }
    }
    //-----------------------------------------------------------------------
    void Material::attach(ShaderFusion * obj)
    {
        ShaderFusionLink::iterator i = mFusionLink.find(obj);
        if (i != mFusionLink.end())
        {
            MaterialList::iterator j, jend = i->second.end();
            for (j = i->second.begin(); j != jend; ++j)
            {
                if (this == *j)
                    return;
            }
            i->second.push_back(this);
        }
        else
        {
            std::pair<ShaderFusionLink::iterator, bool> in =
                mFusionLink.insert(std::pair<ShaderFusion *, MaterialList>(obj, MaterialList()));
            in.first->second.push_back(this);
        }
    }
    //-----------------------------------------------------------------------
    void Material::detach(ShaderFusion * obj)
    {
        ShaderFusionLink::iterator i = mFusionLink.find(obj);
        if (i != mFusionLink.end())
        {
            MaterialList::iterator j, jend = i->second.end();
            for (j = i->second.begin(); j != jend; ++j)
            {
                if (this == *j)
                {
                    i->second.erase(j);
                    break;
                }
            }
            if (i->second.empty())
            {
                N_delete obj;
                mFusionLink.erase(i);
            }
        }
    }
    //---------------------------------------------------------------------
}