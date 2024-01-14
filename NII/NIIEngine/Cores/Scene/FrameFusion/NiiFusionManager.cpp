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
#include "NiiFusionManager.h"
#include "NiiFrameFusion.h"
#include "NiiScriptCompilerManager.h"
#include "NiiEngine.h"
#include "NiiTextureManager.h"

namespace NII
{
    //---------------------------------------------------------------------
    class TextureDef
    {
    public:
        TextureDef(NCount w, NCount h, PixelFormat pf, Nui32 fsaa, const String & fsaahint, bool sRGB) :
            mWidth(w), 
            mHeight(h), 
            mFormat(pf), 
            mFSAA(fsaa),
            mFSAAHint(fsaahint), 
            msRGBwrite(sRGB) {}
    public:
        NCount mWidth;
        NCount mHeight;
        PixelFormat mFormat;
        Nui32 mFSAA;
        String mFSAAHint;
        bool msRGBwrite;
    };
    /*
    struct TextureDefLess
    {
        bool _EngineAPI operator()(const TextureDef & x, const TextureDef & y) const
        {
            if (x.format < y.format)
            {
                return true;
            }
            else if (x.format == y.format)
            {
                if (x.width < y.width)
                    return true;
                else if (x.width == y.width)
                {
                    if (x.height < y.height)
                        return true;
                    else if (x.height == y.height)
                    {
                        if (x.fsaa < y.fsaa)
                            return true;
                        else if (x.fsaa == y.fsaa)
                        {
                            if (x.fsaaHint < y.fsaaHint)
                                return true;
                            else if (x.fsaaHint == y.fsaaHint)
                            {
                                if (!x.sRGBwrite && y.sRGBwrite)
                                    return true;
                            }

                        }
                    }
                }
            }
            return false;
        }
        virtual ~TextureDefLess() {}
    };
    */
    //---------------------------------------------------------------------
    bool isPreTarget(FrameFusion * fusion, const String & localName)
    {
        const FrameShader::TargetList & it = fusion->getShader()->getTargetList();
        FrameShader::TargetList::const_iterator i, iend = it.end();
        for (i = it.begin(); i != iend; ++i)
        {
            FusionShader * tp = *i;
            if (tp->isUsePreResult() && tp->getOutputName() == localName)
            {
                return true;
            }

        }
        return false;
    }
    //---------------------------------------------------------------------
    bool isPreTarget(FrameFusion * fusion, Texture * tex)
    {
        const FrameShader::TargetList & it = fusion->getShader()->getTargetList();
        FrameShader::TargetList::const_iterator i, iend = it.end();
        for (i = it.begin(); i != iend; ++i)
        {
            FusionShader * tp = *i;
            if (tp->isUsePreResult())
            {
                Texture * t = fusion->getTexture(tp->getOutputName(), 0);
                if ((t != 0) && t == tex)
                    return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------
    bool isInputTarget(FrameFusion * fusion, const String & localName)
    {
        const FusionShader::OpList & pit = fusion->getShader()->getOutputCh()->getList();
        FusionShader::OpList::const_iterator i, iend = pit.end();
        for (i = pit.begin(); i != iend; ++i)
        {
            FusionShaderOp * p = *i;
            for (NCount c = 0; c < p->getInputCount(); ++c)
            {
                if (p->getInput(c).first == localName)
                    return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------
    bool isInputTarget(FrameFusion * fusion, Texture * tex)
    {
        const FusionShader::OpList & pit = fusion->getShader()->getOutputCh()->getList();
        FusionShader::OpList::const_iterator i, iend = pit.end();
        for (i = pit.begin(); i != iend; ++i)
        {
            FusionShaderOp * p = *i;
            for (NCount c = 0; c < p->getInputCount(); ++c)
            {
                Texture * t = fusion->getTexture(p->getInput(c).first, 0);
                if ((t != 0) && t == tex)
                    return true;
            }
        }

        return false;
    }
    //------------------------------------------------------------------------
    Nui64 calcDefID(const TextureDef & def)
    {
        N_assert(def.mFormat < 124, "error");
        N_assert(def.mWidth < 65536, "error");
        N_assert(def.mHeight < 65536, "error");
        N_assert(def.mFSAA < 255, "error");
        N_assert(def.mFSAAHint.size() < 255, "error");

        Nui64 re = (Nui64)def.mFormat << 60;
        re |= (Nui64)def.mWidth << 40;
        re |= def.mHeight << 20;
        re |= def.mFSAA << 12;
        re |= def.mFSAAHint.size() << 4;
        re |= def.msRGBwrite ? 1 : 0;
        return re;
    }
    //------------------------------------------------------------------------
    template<> FusionManager * Singleton<FusionManager>::mOnly = 0;
    //------------------------------------------------------------------------
    /*FusionManager * FusionManager::getOnlyPtr()
    {
        return mOnly;
    }
    //------------------------------------------------------------------------
    FusionManager & FusionManager::getOnly()
    {
        N_assert(mOnly, "error");
        return(*mOnly);
    }*/
    //------------------------------------------------------------------------
    FusionManager::FusionManager() :
        ResourceManager(RT_FrameFilter),
        mRect(0)
    {
        // Loading order (just after materials)
        mScriptLevel = 110;
        // Register with resource group manager
        N_Only(ResourceScheme).add(mType, this);
    }
    //-----------------------------------------------------------------------
    FusionManager::~FusionManager()
    {
        FusionResultList::iterator i, iend = mFusionResultList.end();
        for (i = mFusionResultList.begin(); i != iend; ++i)
        {
            N_delete  i->second;
        }
        mFusionResultList.clear();
        freeFusionTexture(true);
        if (mRect)
        {
            N_delete mRect;
            mRect = 0;
        }
        // Resources cleared by superclass
        // Unregister with resource group manager
        N_Only(ResourceScheme).remove(mType);
        N_Only(ResourceScheme).remove(this);
    }
    //-----------------------------------------------------------------------
    Resource * FusionManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs,
            const PropertyData * params)
    {
        return N_new Fusion(rid, gid, ls, rs);
    }
    //-----------------------------------------------------------------------
    void FusionManager::init()
    {

    }
    //-----------------------------------------------------------------------
    void FusionManager::parse(DataStream * stream, GroupID gid, PtrList & out)
    {
        N_Only(ScriptCompiler).parse(stream, gid, out);
    }
    //-----------------------------------------------------------------------
    FusionInstance * FusionManager::getInstance(Viewport * vp)
    {
        FusionResultList::iterator i = mFusionResultList.find(vp);
        if(i != mFusionResultList.end())
        {
            return i->second;
        }
        else
        {
            FusionInstance * res = N_new FusionInstance(vp);
            mFusionResultList[vp] = res;
            return res;
        }
    }
    //-----------------------------------------------------------------------
    bool FusionManager::isInstanceExist(Viewport * vp) const
    {
        return mFusionResultList.find(vp) != mFusionResultList.end();
    }
    //-----------------------------------------------------------------------
    void FusionManager::removeInstance(Viewport * vp)
    {
        FusionResultList::iterator i = mFusionResultList.find(vp);
        if (i != mFusionResultList.end())
        {
            N_delete i->second;
            mFusionResultList.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void FusionManager::removeAll()
    {
        FusionResultList::iterator i, iend = mFusionResultList.end();
        for (i = mFusionResultList.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mFusionResultList.clear();
        ResourceManager::destroyUnused();
    }
    //-----------------------------------------------------------------------
    GeometryObj * FusionManager::getArea() const
    {
        if(!mRect)
        {
            // 2D rectangle, to use for render_quad passes
            mRect = N_new RectGeo(true, Buffer::M_WRITE | Buffer::M_WHOLE);
        }
        RenderSys * rs = N_Engine().getRender();
        Viewport * vp = rs->getViewports();
        NIIf hOffset = rs->getTexelXOffset() / (0.5f * vp->getPixelWidth());
        NIIf vOffset = rs->getTexelYOffset() / (0.5f * vp->getPixelHeight());
        mRect->setRect(-1 + hOffset, 1 - vOffset, 1 + hOffset, -1 - vOffset);
        return mRect;
    }
    //-----------------------------------------------------------------------
    FrameFusion * FusionManager::attach(Viewport * vp, ResourceID rid, NCount pos)
    {
        Fusion * fusion = static_cast<Fusion *>(get(rid));
        if(fusion)
            return 0;
        FusionInstance * res = getInstance(vp);
        return res->create(fusion, pos);
    }
    //-----------------------------------------------------------------------
    void FusionManager::detach(Viewport * vp, ResourceID rid)
    {
        FusionInstance * res = getInstance(vp);
        for(NCount pos = 0; pos < res->getCount(); ++pos)
        {
            FrameFusion * fusion = res->get(pos);
            if(fusion->getFusion()->getOriginID() == rid)
            {
                res->destroy(pos);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void FusionManager::setEnable(Viewport * vp, ResourceID rid, bool value)
    {
        FusionInstance * res = getInstance(vp);
        for(NCount pos = 0; pos < res->getCount(); ++pos)
        {
            FrameFusion * fusion = res->get(pos);
            if(fusion->getFusion()->getOriginID() == rid)
            {
                res->setEnable(pos, value);
                break;
            }
        }
    }
    //---------------------------------------------------------------------
    void FusionManager::rebuildInstance()
    {
        // In order to deal with shared resources, we have to disable *all* compositors
        // first, that way shared resources will get freed
        typedef vector<FrameFusion *>::type FusionList;
        FusionList relist;
        FusionResultList::iterator i, iend = mFusionResultList.end();
        for(FusionResultList::iterator i = mFusionResultList.begin(); i != iend; ++i)
        {
            FusionInstance * result = i->second;
            const FusionInstance::FusionList & instIt = result->getList();
            FusionInstance::FusionList::const_iterator cc, ccend = instIt.end();
            for(cc = instIt.begin(); cc != ccend; ++cc)
            {
                FrameFusion * fusion = *cc;
                if(fusion->isEnable())
                {
                    fusion->setEnable(false);
                    relist.push_back(fusion);
                }
            }
        }
        //UVs are lost, and will never be reconstructed unless we do them again, now
        if (mRect)
        {
            mRect->setTexCoord(Vector2f::ZERO, Vector2f::Y, Vector2f::X, Vector2f::UNIT);
        }
        FusionList::iterator j, jend = relist.end();
        for(j = relist.begin(); j != jend; ++j)
        {
            FrameFusion * fusion = *j;
            fusion->setEnable(true);
        }
    }
    //---------------------------------------------------------------------
    Texture * FusionManager::touchFusionTexture(FrameFusion * fusion, ResourceID rid, 
        const String & localName, NCount w, NCount h, PixelFormat pf, Nui32 fsaa, 
            const String & fsaahint, bool sRGB, TextureSet & applyList, 
                FrameShader::SourceType scope)
    {
        if(scope == FrameShader::ST_Pool)
        {
            N_EXCEPT(InvalidParam, _I18n("全局纹理不在池中."));
        }

        TextureDef def(w, h, pf, fsaa, fsaahint, sRGB);
        Nui64 defid = calcDefID(def);
        if(scope == FrameShader::ST_Fusion)
        {
            DefineList & defList = mFusionTextureList[Npair(fusion->getFusion()->getOriginID(), localName)];
            DefineList::iterator it = defList.find(defid);
            if (it != defList.end())
            {
                it->second->ref(false);
                return it->second;
            }
            Texture * re = N_Only(Texture).createTexture(rid, GroupInner, Texture::T_2D, 
                pf, w, h, 1, 0, Texture::MM_FRAME_RGB, 0, 0, sRGB, fsaa, fsaahint);

            re->ref(false);
            defList.insert(DefineList::value_type(defid, re));
            return re;
        }

        TempTextureList::iterator i = mTempTextureList.find(defid);
        if(i == mTempTextureList.end())
        {
            i = mTempTextureList.insert(TempTextureList::value_type(defid, N_new_t(TextureList))).first;
        }
        FrameFusion * previous = fusion->getInstance()->getPre(fusion);
        FrameFusion * next = fusion->getInstance()->getNext(fusion);

        Texture * re = 0;
        TextureList * tempList = i->second;
        for(TextureList::iterator t = tempList->begin(); t != tempList->end(); ++t)
        {
            Texture * temp = *t;
            // check not already used
            if(applyList.find(temp) == applyList.end())
            {
                bool reUse = true;
                // ok, we didn't use this one already
                // however, there is an edge case where if we re-use a texture
                // which has an 'input previous' pass, and it is chained from another
                // compositor, we can end up trying to use the same texture for both
                // so, never allow a texture with an input previous pass to be
                // shared with its immediate predecessor in the chain
                if(isPreTarget(fusion, localName))
                {
                    // Check whether this is also an input to the output target of previous
                    // can't use FrameFusion::mPreFusion, only set up
                    // during compile
                    if (previous && isInputTarget(previous, temp))
                        reUse = false;
                }
                // now check the other way around since we don't know what order they're bound in
                if(isInputTarget(fusion, localName))
                {
                    if(next && isPreTarget(next, temp))
                        reUse = false;
                }

                if(reUse)
                {
                    re = temp;
                    break;
                }
            }
        }

        if(re == 0)
        {
            re = N_Only(Texture).createTexture(rid, GroupInner, Texture::T_2D, pf,
                w, h, 1, 0, Texture::MM_FRAME_RGB, 0, 0, sRGB, fsaa, fsaahint);
            re->ref(false);
            tempList->push_back(re);
        }
        applyList.insert(re);
        return re;
    }
    //---------------------------------------------------------------------
    void FusionManager::freeFusionTexture(bool all)
    {
        if (all)
        {
            for (TempTextureList::iterator i = mTempTextureList.begin(); i != mTempTextureList.end(); ++i)
            {
                TextureList::iterator j, jend = i->second->end();
                for (j = i->second->begin(); j != jend; ++j)
                {
                    //if ((*j)->getRefCount() == 0)
                    N_Only(Texture).destroy((*j)->getPrc());
                }
                N_delete_t(i->second, TextureList);
            }
            for (FusionTextureList::iterator i = mFusionTextureList.begin(); i != mFusionTextureList.end(); ++i)
            {
                DefineList::iterator j, jend = i->second.end();
                for (j = i->second.begin(); j != jend; ++j)
                {
                    //if (j->second->getRefCount() == 0)
                    N_Only(Texture).destroy(j->second->getPrc());
                }
            }
            mTempTextureList.clear();
            mFusionTextureList.clear();
        }
        else
        {
            for(TempTextureList::iterator i = mTempTextureList.begin(); i != mTempTextureList.end(); ++i)
            {
                TextureList * tempList = i->second;
                for(TextureList::iterator j = tempList->begin(); j != tempList->end();)
                {
                    // if the resource system, plus this class, are the only ones to have a reference..
                    // NOTE: any material references will stop this texture getting freed (e.g. compositor demo)
                    // until this routine is called again after the material no longer references the texture
                    if((*j)->getRefCount() == 0)
                    {
                        N_Only(Texture).destroy((*j)->getPrc());
                        j = tempList->erase(j);
                    }
                    else
                    {
                        ++j;
                    }
                }
            }

            for(FusionTextureList::iterator i = mFusionTextureList.begin(); i != mFusionTextureList.end(); ++i)
            {
                DefineList & defList = i->second;
                for(DefineList::iterator j = defList.begin(); j != defList.end();)
                {
                    const Texture * temp = j->second;
                    if(temp->getRefCount() == 0)
                    {
                        N_Only(Texture).destroy(temp->getPrc());
                        defList.erase(j++);
                    }
                    else
                    {
                        ++j;
                    }
                }
            }
        }
    }
    //---------------------------------------------------------------------
    bool FusionManager::addListener(const String & name, FrameFusionListener * obj)
    {
        if(name.empty() || mListenerList.find(name) != mListenerList.end())
        {
            return false;
        }
        mListenerList[name] = obj;
        return true;
    }
    //---------------------------------------------------------------------
    void FusionManager::removeListener(const String & name)
    {
        ListenerList::iterator itor = mListenerList.find(name);
        if(itor != mListenerList.end())
        {
            mListenerList.erase(itor);
        }
    }
    //---------------------------------------------------------------------
    FrameFusionListener * FusionManager::getListener(const String & name)
    {
        ListenerList::iterator it = mListenerList.find(name);
        if (it == mListenerList.end())
        {
            N_EXCEPT(NotExist, _I18n("Fusion logic '") + name + _I18n("' not registered."));
        }
        return it->second;
    }
    //---------------------------------------------------------------------
    bool FusionManager::addRender(const String & name, FusionRender * obj)
    {
        if(name.empty() || mRenderList.find(name) != mRenderList.end())
        {
            return false;
        }
        mRenderList[name] = obj;
        return true;
    }
    //---------------------------------------------------------------------
    void FusionManager::removeRender(const String & name)
    {
        RenderList::iterator i = mRenderList.find(name);
        if (i != mRenderList.end())
        {
            mRenderList.erase(i);
        }
    }
    //---------------------------------------------------------------------
    FusionRender * FusionManager::getRender(const String & name)
    {
        RenderList::iterator i = mRenderList.find(name);
        if (i == mRenderList.end())
        {
            N_EXCEPT(NotExist, _I18n("Custom composition pass '") + name + _I18n("' not registered."));
        }
        return i->second;
    }
    //---------------------------------------------------------------------
}