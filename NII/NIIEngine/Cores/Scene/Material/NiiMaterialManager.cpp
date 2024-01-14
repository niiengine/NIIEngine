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
#include "NiiMaterialManager.h"
#include "NiiMaterialSerializer.h"
#include "NiiLogManager.h"
#include "NiiLodSchemeManager.h"
#include "NiiMaterialLodScheme.h"
#include "NiiVFS.h"
#include "NiiStrConv.h"
#include "NiiException.h"
#include "NiiScriptCompilerManager.h"
#include "NiiEquationManager.h"
#include "NiiAnimationManager.h"
#include "NiiPredefinedControllers.h"

#if 1 == 1
    #include "NiiScriptCompiler.h"
#endif

namespace NII
{
    /** 预定义控制器值, 获取/设置 纹理层帧数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureFrameControllerValue : public DataValue<NIIf>
    {
    public:
        TextureFrameControllerValue(bool autoDsy, ShaderChTextureUnit * t) :
            DataValue<NIIf>(autoDsy)
        {
            mTextureLayer = t;
        }
        /** @copydetails DataValue::getValue
        @par 获取帧数作为参数值范围在[0,1]
        */
        const NIIf getValue() const
        {
            NIIi numFrames = mTextureLayer->getFrameCount();
            return ((NIIf)mTextureLayer->getCurrentFrame() / (NIIf)numFrames);
        }

        /** @copydetails DataValue::setValue
        @par 设置帧数作为一个参数值范围在[0,1],实际帧数值为value * (numFrames-1).
        */
        void setValue(const NIIf & value)
        {
            NIIi numFrames = mTextureLayer->getFrameCount();
            mTextureLayer->setCurrentFrame((NIIi)(value * numFrames) % numFrames);
        }
    protected:
        ShaderChTextureUnit * mTextureLayer;
    };
    //-----------------------------------------------------------------------
    const ShaderChColour *          MaterialManager::InvalidColour = 0;
    const ShaderChPoint *           MaterialManager::InvalidPoint = 0;
    const ShaderChFog *             MaterialManager::InvalidFog = 0;
    const ShaderChBlend *           MaterialManager::InvalidBlend = 0;
    const ShaderChStencil *         MaterialManager::InvalidStencil = 0;
    const ShaderChTexture *         MaterialManager::InvalidTexture = 0;
    const ShaderChProgram *         MaterialManager::InvalidProgram = 0;
    const Material * MaterialManager::WhiteLight = 0;
    const Material * MaterialManager::WhiteColour = 0;
    SchemeID MaterialManager::Default = 0;
    //-----------------------------------------------------------------------
    template<> MaterialManager * Singleton<MaterialManager>::mOnly = 0;
    /*MaterialManager * MaterialManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    MaterialManager & MaterialManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //-----------------------------------------------------------------------
    MaterialManager::MaterialManager() :
        mNull(0)
    {
        mType = RT_Material;
        mMin = TFOP_LINEAR;
        mMag = TFOP_LINEAR;
        mMip = TFOP_POINT;
        mMaxAniso = 1;

        // Create primary thread copies of script compiler / serializer
        // other copies for other threads may also be instantiated
        mSerializer =  N_new MaterialSerializer();

        // Loading order
        mScriptLevel = 100;
        // Scripting is supported by this manager
#if 1 == 0
        mScriptPattern.push_back("*.program");
        mScriptPattern.push_back("*.material");
        N_Only(ResourceScheme).add(this);
#endif
        // 注册资源类型
        N_Only(ResourceScheme).add(mType, this);

        mCurrent = 0;
    }
    //-----------------------------------------------------------------------
    MaterialManager::~MaterialManager()
    {
        if (InvalidColour)
        {
            N_delete InvalidColour;
            InvalidColour = 0;
        }
        if (InvalidPoint)
        {
            N_delete InvalidPoint;
            InvalidPoint = 0;
        }
        if (InvalidFog)
        {
            N_delete InvalidFog;
            InvalidFog = 0;
        }
        if (InvalidBlend)
        {
            N_delete InvalidBlend;
            InvalidBlend = 0;
        }
        if (InvalidStencil)
        {
            N_delete InvalidStencil;
            InvalidStencil = 0;
        }
        if (InvalidTexture)
        {
            N_delete InvalidTexture;
            InvalidTexture = 0;
        }
        if (InvalidProgram)
        {
            N_delete InvalidProgram;
            InvalidProgram = 0;
        }
        mNull = 0;
        WhiteLight = 0;
        WhiteColour = 0;
        // Resources cleared by superclass
        // Unregister with resource group manager
        N_Only(ResourceScheme).remove(mType);
        N_Only(ResourceScheme).remove(this);

        // delete primary thread instances directly, other threads will delete
        // theirs automatically when the threads end.
        N_delete mSerializer;
    }
    //-----------------------------------------------------------------------
    void MaterialManager::init()
    {
        // Set up default material - don't use name contructor as we want to avoid applying defaults
        mNull = static_cast<Material *>(create(0, GroupInner));
        mNull->ref(false);
        mNull->create()->create();

        mNull->setLodScheme(static_cast<MaterialLodScheme *>(N_Only(LodScheme).createDefault(N_LOD_Material)));

        InvalidColour = N_new ShaderChColour();
        InvalidPoint = N_new ShaderChPoint();
        InvalidFog = N_new ShaderChFog();
        InvalidBlend = N_new ShaderChBlend();
        InvalidStencil = N_new ShaderChStencil();
        InvalidTexture = N_new ShaderChTexture(0);
        InvalidProgram = N_new ShaderChProgram(0);
        // 建立一个有灯照基本白色材质
        WhiteLight = static_cast<Material *>(create(0, GroupInner));
        const_cast<Material *>(WhiteLight)->ref(false);
        // 建立一个无灯照基本白色材质
        WhiteColour = static_cast<Material *>(create(0, GroupInner));
        const_cast<Material *>(WhiteColour)->ref(false);
        const_cast<Material *>(WhiteColour)->setLight(false);
    }
    //-----------------------------------------------------------------------
    void MaterialManager::add(SchemeID sid, MaterialMatchListener * l)
    {
        mListenerMap[sid].push_back(l);
    }
    //-----------------------------------------------------------------------
    void MaterialManager::remove(SchemeID sid, MaterialMatchListener * l)
    {
        mListenerMap[sid].remove(l);
    }
    //-----------------------------------------------------------------------
    void MaterialManager::notify(ShaderCh * obj)
    {

    }
    //-----------------------------------------------------------------------
    void MaterialManager::notify(ShaderFusion * obj)
    {

    }
    //-----------------------------------------------------------------------
    void MaterialManager::setBlend(Material * obj, FrameBlendMode fbm)
    {
        ShaderFusionList::iterator i, iend = obj->mFusions.end();
        for(i = obj->mFusions.begin(); i != iend; ++i)
        {
            ShaderChList::iterator j, jend = (*i)->mChList.end();
            for(j = (*i)->mChList.begin(); j != jend; ++j)
            {
                (*j)->setBlend(ShaderCh::SOT_Set)->setBlend(fbm);
            }
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setBlend(ShaderFusion * obj, FrameBlendMode fbm)
    {
        ShaderChList::iterator i, iend = obj->mChList.end();
        for(i = obj->mChList.begin(); i != iend; ++i)
        {
            (*i)->setBlend(ShaderCh::SOT_Set)->setBlend(fbm);
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setBlend(Material * obj, FrameBlendMode fbm, FrameBlendMode afbm)
    {
        ShaderFusionList::iterator i, iend = obj->mFusions.end();
        for(i = obj->mFusions.begin(); i != iend; ++i)
        {
            ShaderChList::iterator j, jend = (*i)->mChList.end();
            for(j = (*i)->mChList.begin(); j != jend; ++j)
            {
                (*j)->setBlend(ShaderCh::SOT_Set)->setBlend(fbm, afbm);
            }
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setBlend(ShaderFusion * obj, FrameBlendMode fbm, FrameBlendMode afbm)
    {
        ShaderChList::iterator i, iend = obj->mChList.end();
        for(i = obj->mChList.begin(); i != iend; ++i)
        {
            (*i)->setBlend(ShaderCh::SOT_Set)->setBlend(fbm, afbm);
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setBlend(Material * obj, ColourFactor src, ColourFactor dest)
    {
        ShaderFusionList::iterator i, iend = obj->mFusions.end();
        for(i = obj->mFusions.begin(); i != iend; ++i)
        {
            ShaderChList::iterator j, jend = (*i)->mChList.end();
            for(j = (*i)->mChList.begin(); j != jend; ++j)
            {
                (*j)->setBlend(ShaderCh::SOT_Set)->setBlend(src, dest);
            }
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setBlend(ShaderFusion * obj, ColourFactor src, ColourFactor dest)
    {
        ShaderChList::iterator i, iend = obj->mChList.end();
        for(i = obj->mChList.begin(); i != iend; ++i)
        {
            (*i)->setBlend(ShaderCh::SOT_Set)->setBlend(src, dest);
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setAmbient(Material * obj, const Colour & c)
    {
        ShaderFusionList::iterator i, iend = obj->mFusions.end();
        for(i = obj->mFusions.begin(); i != iend; ++i)
        {
            ShaderChList::iterator j, jend = (*i)->mChList.end();
            for(j = (*i)->mChList.begin(); j != jend; ++j)
            {
                (*j)->setColour(ShaderCh::SOT_Set)->setAmbient(c);
            }
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setAmbient(ShaderFusion * obj, const Colour & c)
    {
        ShaderChList::iterator i, iend = obj->mChList.end();
        for(i = obj->mChList.begin(); i != iend; ++i)
        {
            (*i)->setColour(ShaderCh::SOT_Set)->setAmbient(c);
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setDepthCheck(Material * obj, bool set)
    {
        ShaderFusionList::iterator i, iend = obj->mFusions.end();
        for(i = obj->mFusions.begin(); i != iend; ++i)
        {
            ShaderChList::iterator j, jend = (*i)->mChList.end();
            for(j = (*i)->mChList.begin(); j != jend; ++j)
            {
                (*j)->setStencil(ShaderCh::SOT_Set)->setDepthEnable(set);
            }
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setDepthCheck(ShaderFusion * obj, bool set)
    {
        ShaderChList::iterator i, iend = obj->mChList.end();
        for(i = obj->mChList.begin(); i != iend; ++i)
        {
            (*i)->setStencil(ShaderCh::SOT_Set)->setDepthEnable(set);
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setDepthWrite(Material * obj, bool set)
    {
        ShaderFusionList::iterator i, iend = obj->mFusions.end();
        for(i = obj->mFusions.begin(); i != iend; ++i)
        {
            ShaderChList::iterator j, jend = (*i)->mChList.end();
            for(j = (*i)->mChList.begin(); j != jend; ++j)
            {
                (*j)->setStencil(ShaderCh::SOT_Set)->setDepthWrite(set);
            }
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setDepthWrite(ShaderFusion * obj, bool set)
    {
        ShaderChList::iterator i, iend = obj->mChList.end();
        for(i = obj->mChList.begin(); i != iend; ++i)
        {
            (*i)->setStencil(ShaderCh::SOT_Set)->setDepthWrite(set);
        }
    }
    //-----------------------------------------------------------------------
    Resource * MaterialManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
        return N_new Material(rid, gid, ls, rs);
    }
    //-----------------------------------------------------------------------
    DataEquation<TimeDurMS, NIIf> * MaterialManager::createAnimation(
        ShaderChTextureUnit * layer, TimeDurMS dur)
    {
        DataValue<NIIf> * texVal = N_new TextureFrameControllerValue(true, layer);
        DataFunc<TimeDurMS, NIIf> * func = N_new AnimationControllerFunction(true, dur);

        return N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), texVal, func);
    }
    //-----------------------------------------------------------------------
    DataEquation<TimeDurMS, NIIf> * MaterialManager::createUVScroll(
        ShaderChTextureUnit * layer, NIIf speed)
    {
        DataEquation<TimeDurMS, NIIf> * ret = 0;

        if (speed != 0)
        {
            DataValue<NIIf> * val;
            DataFunc<TimeDurMS, NIIf> * func;

            // We do both scrolls with a single controller
            val = N_new TexCoordModifierControllerValue(true, layer, true, true);
            // Create function: use -speed since we're altering texture coords so they have reverse effect
            func = N_new ScaleControllerFunction(true, -speed);
            ret = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), val, func);
        }

        return ret;
    }
    //-----------------------------------------------------------------------
    DataEquation<TimeDurMS, NIIf> * MaterialManager::createUScroll(
        ShaderChTextureUnit * layer, NIIf speed)
    {
        DataEquation<TimeDurMS, NIIf> * ret = 0;

        if (speed != 0)
        {
            DataValue<NIIf> * val;
            DataFunc<TimeDurMS, NIIf> * func;

            val = N_new TexCoordModifierControllerValue(true, layer, true);
            // Create function: use -speed since we're altering texture coords so they have reverse effect
            func = N_new ScaleControllerFunction(true, -speed);
            ret = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), val, func);
        }

        return ret;
    }
    //-----------------------------------------------------------------------
    DataEquation<TimeDurMS, NIIf> * MaterialManager::createVScroll(
        ShaderChTextureUnit * layer, NIIf vSpeed)
    {
        DataEquation<TimeDurMS, NIIf> * ret = 0;

        if (vSpeed != 0)
        {
            DataValue<NIIf> * val;
            DataFunc<TimeDurMS, NIIf> * func;

            // Set up a second controller for v scroll
            val = N_new TexCoordModifierControllerValue(true, layer, false, true);
            // Create function: use -speed since we're altering texture coords so they have reverse effect
            func = N_new ScaleControllerFunction(true, -vSpeed);
            ret = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), val, func);
        }

        return ret;
    }
    //-----------------------------------------------------------------------
    DataEquation<TimeDurMS, NIIf> * MaterialManager::createUVScale(
        ShaderChTextureUnit * layer, NIIf speed)
    {
        DataEquation<TimeDurMS, NIIf> * ret = 0;

        if(speed != 0)
        {
            DataValue<NIIf> * val;
            DataFunc<TimeDurMS, NIIf> * func;

            // We do both scrolls with a single controller
            val = N_new TexCoordModifierControllerValue(true, layer, false, false, true, true);
            // Create function: use -speed since we're altering texture coords so they have reverse effect
            func = N_new ScaleControllerFunction(true, -speed);
            ret = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), val, func);
        }

        return ret;
    }
    //-----------------------------------------------------------------------
    DataEquation<TimeDurMS, NIIf> * MaterialManager::createUScale(
        ShaderChTextureUnit * layer, NIIf speed)
    {
        DataEquation<TimeDurMS, NIIf> * ret = 0;

        if (speed != 0)
        {
            DataValue<NIIf> * val;
            DataFunc<TimeDurMS, NIIf> * func;

            // We do both scrolls with a single controller
            val = N_new TexCoordModifierControllerValue(true, layer, false, false, true, false);
            // Create function: use -speed since we're altering texture coords so they have reverse effect
            func = N_new ScaleControllerFunction(true, -speed);
            ret = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), val, func);
        }

        return ret;
    }
    //-----------------------------------------------------------------------
    DataEquation<TimeDurMS, NIIf> * MaterialManager::createVScale(
        ShaderChTextureUnit * layer, NIIf speed)
    {
        DataEquation<TimeDurMS, NIIf> * ret = 0;

        if (speed != 0)
        {
            DataValue<NIIf> * val;
            DataFunc<TimeDurMS, NIIf> * func;

            // We do both scrolls with a single controller
            val = N_new TexCoordModifierControllerValue(true, layer, false, false, false, true);
            // Create function: use -speed since we're altering texture coords so they have reverse effect
            func = N_new ScaleControllerFunction(true, -speed);
            ret = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), val, func);
        }

        return ret;
    }
    //-----------------------------------------------------------------------
    DataEquation<TimeDurMS, NIIf> * MaterialManager::createRotate(
        ShaderChTextureUnit * layer, NIIf speed)
    {
        DataValue<NIIf> * val;
        DataFunc<TimeDurMS, NIIf> * func;

        // Target value is texture coord rotation
        val = N_new TexCoordModifierControllerValue(true, layer, false, false, false, false, true);
        // Function is simple scale (seconds * speed)
        // Use -speed since altering texture coords has the reverse visible effect
        func = N_new ScaleControllerFunction(true, -speed);

        return N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), val, func);
    }
    //-----------------------------------------------------------------------
    void MaterialManager::destroy(DataEquation<TimeDurMS, NIIf> * obj)
    {
        return N_Only(Equation).destroy(obj);
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setTextureFilter(TextureFilterMode fo)
    {
        switch(fo)
        {
        case TFM_NONE:
            mMin = TFOP_POINT;
            mMag = TFOP_POINT;
            mMip = TFOP_NONE;
            break;
        case TFM_BILINEAR:
            mMin = TFOP_LINEAR;
            mMag = TFOP_LINEAR;
            mMip = TFOP_POINT;
            break;
        case TFM_TRILINEAR:
            mMin = TFOP_LINEAR;
            mMag = TFOP_LINEAR;
            mMip = TFOP_LINEAR;
            break;
        case TFM_ANISOTROPIC:
            mMin = TFOP_ANISOTROPIC;
            mMag = TFOP_ANISOTROPIC;
            mMip = TFOP_LINEAR;
            break;
        }
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setTextureAniso(Nui32 aniso)
    {
        mMaxAniso = aniso;
    }
    //-----------------------------------------------------------------------
    Nui32 MaterialManager::getTextureAniso() const
    {
        return mMaxAniso;
    }
    //-----------------------------------------------------------------------
    Material * MaterialManager::getNull() const
    {
        return mNull;
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setTextureFilter(TextureFilterType type, TextureFilterOP opts)
    {
        switch(type)
        {
        case TFT_MIN:
            mMin = opts;
            break;
        case TFT_MAG:
            mMag = opts;
            break;
        case TFT_MIP:
            mMip = opts;
            break;
        }
    }
    //-----------------------------------------------------------------------
    TextureFilterOP MaterialManager::getTextureFilter(TextureFilterType type) const
    {
        switch(type)
        {
        case TFT_MIN:
            return mMin;
        case TFT_MAG:
            return mMag;
        case TFT_MIP:
            return mMip;
        }
        // to keep compiler happy
        return mMin;
    }
    //-----------------------------------------------------------------------
    void MaterialManager::setCurrent(SchemeID sid)
    {
        mCurrent = sid;
    }
    //-----------------------------------------------------------------------
    SchemeID MaterialManager::getCurrent() const
    {
        return mCurrent;
    }
    //-----------------------------------------------------------------------
    ShaderFusion * MaterialManager::match(const GeometryObj * rend, const Material * mat, Nidx lod)
    {
        //First, check the scheme specific listeners
        ListenerMap::iterator it = mListenerMap.find(mCurrent);
        if(it != mListenerMap.end())
        {
            Listeners & lis = it->second;
            Listeners::iterator i, iend = lis.end();
            for(i = lis.begin(); i != iend; ++i)
            {
                ShaderFusion * t = (*i)->miss(mCurrent, mat,  rend, lod);
                if(t)
                    return t;
            }
        }

        it = mListenerMap.find(0);
        if(it != mListenerMap.end())
        {
            Listeners & lis = it->second;
            Listeners::iterator i, iend = lis.end();
            for(i = lis.begin(); i != iend; ++i)
            {
                ShaderFusion * t = (*i)->miss(mCurrent, mat, rend, lod);
                if(t)
                    return t;
            }
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    void MaterialManager::parse(DataStream * stream, GroupID gid, PtrList & out)
    {
#if 1 == 1
        N_Only(ScriptCompiler).parse(stream, gid, out);
#else
        if(mSerializer != 0)
            mSerializer->parseScript(stream, out, gid);
#endif
    }
    //-----------------------------------------------------------------------
}