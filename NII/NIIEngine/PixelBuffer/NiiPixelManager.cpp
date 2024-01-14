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
#include "NiiPixelManager.h"
#include "NiiException.h"
#include "NiiResourceSchemeManager.h"
#include "NiiEngine.h"
#include "NiiTextureManager.h"
#include "NiiXmlPattern.h"
#include "NiiPixelBuffer.h"
#include "NiiXmlAnalyzeManager.h"
#include "NiiLogManager.h"
#include "NiiXmlAnalyzeList.h"

namespace NII
{
    //------------------------------------------------------------------------
    static Texture * _N_PixelBufferMagTex = 0;
    static ScaleMode _N_PixelBufferMagAutoRes = SM_Unknow;
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PixelBufferXmlAnalyze
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    /** 像素元素集xml分析
    @version NIIEngine 3.0.0
    */
    class _EngineInner PixelBufferXmlAnalyze : public XmlAnalyze
    {
    public:
        PixelBufferXmlAnalyze(LangID lid = N_PrimaryLang) :
            XmlAnalyze(2, N_Script_PixelBuffer, _T("PixelBuffer.xsd"), lid),
            mListPBG(0)
        {
            setBeginPrc(N_PropertyPixelBuffer_Pixel, (PrcCB)(&PixelBufferXmlAnalyze::PrcPixelBufferCB));
        }
        ~PixelBufferXmlAnalyze(){}

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const
        {
            return N_XML_TAG_ViewPixel;
        }

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const
        {
            return N_Only(Pixel).getGroup();
        }

        inline PixelBufferGroupList getGroupList() const
        {
            return mList;
        }
    protected:
        /// @copydetails XmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc)
        {
            const String & name(pdc->get(NameProperty));
            ResourceID rid;
            pdc->get(N_PropertyPixelBuffer_TextureSrc, rid);

            Nui32 rgrp;
            pdc->get(GroupProperty, rgrp);

            N_Only(Log).log(_I18n("**** 创建像素集:"));
            N_Only(Log).log(_I18n("**** 像素集名字: ") + pdc->get(NameProperty));
            N_Only(Log).log(_I18n("****像素集纹理: ") + rid);
            N_Only(Log).log(_I18n("****像素集纹理资源组: ") + (rgrp == 0 ? GroupGlobal : rgrp));

            Nui32 version;
            ScaleMode mode;
            pdc->get(VersionProperty, version, 0);
            if(version == 2)
                return;

            N_EXCEPT(InvalidRequest, _I18n("版本请求错误! "));

            TextureManager * mag = N_OnlyPtr(Texture);

            if(mag->isExist(rid))
            {
                N_Only(Log).log(_I18n("WARNING: Using existing texture: ") + rid);
                _N_PixelBufferMagTex = static_cast<Texture *>(mag->get(rid));
            }
            else
            {
                // create texture from image
                _N_PixelBufferMagTex = static_cast<Texture *>(mag->createTexture(rid, rgrp == 0 ?
                    N_Only(Pixel).getGroup() : rgrp));
            }

            Nui32 tid;
            NIIf tf1, tf2, t3, t4;
            pdc->get(N_PropertyPixelBuffer_NativeResX, tf1, 640.0f);
            pdc->get(N_PropertyPixelBuffer_NativeResY, tf2, 480.0f);

            // set auto-scaling as needed
            StrConv::conv(pdc->getDefault(N_PropertyPixelBuffer_AutoRes, _T("false")), _N_PixelBufferMagAutoRes);

            pdc->get(GroupProperty, tid);
            pdc->get(N_PropertyPixelBuffer_TextureSrc, rid, _N_PixelBufferMagTex->getOriginID());
            pdc->get(N_PropertyPixelBuffer_PosXOffset, t3);
            pdc->get(N_PropertyPixelBuffer_PosYOffset, t4);
            StrConv::conv(pdc->getDefault(N_PropertyPixelBuffer_AutoRes, N_conv(_N_PixelBufferMagAutoRes)), mode);
            Texture * texobj = static_cast<Texture *>(N_OnlyPtr(Texture)->get(tid));
            PixelBufferGroup * pbg = N_Only(Pixel).create(texobj, mode, Vector2f::ZERO,
                Vector2f(t3, t4), PlaneSizef(tf1, tf2));

            mListPBG = pbg;
            mList.push_back(pbg);
        }

        /** 处理像素函数
        @version NIIEngine 3.0.0
        */
        void PrcPixelBufferCB(const PropertyData * pdc)
        {
            Nui32 id, rid;
            NIIi t1, t2, t5, t6;

            pdc->get(IDProperty, id);
            pdc->get(N_PropertyPixelBuffer_Width, t1, 0);
            pdc->get(N_PropertyPixelBuffer_Height, t2, 0);
            pdc->get(N_PropertyPixelBuffer_PosX, t5, 0);
            pdc->get(N_PropertyPixelBuffer_PosY, t6, 0);

            N_Only(Pixel).create((PixelBufferID)id, mListPBG, Rectf(Vector2f((NIIf)t5, (NIIf)t6), PlaneSizef((NIIf)t1, (NIIf)t2)));
        }
    protected:
        PixelBufferGroup * mListPBG;
        PixelBufferGroupList mList;
    };
    //------------------------------------------------------------------------
    template<> PixelManager * Singleton<PixelManager>::mOnly = 0;
    GroupID PixelManager::ResourceGroup = GroupUnknow;
    //------------------------------------------------------------------------
    PixelManager::PixelManager():
        mValidRID(N_Max_Valid_ID)
    {
    }
    //------------------------------------------------------------------------
    PixelManager::~PixelManager()
    {
        destroyAll();
    }
    //------------------------------------------------------------------------
    PixelBufferGroupList PixelManager::create(const String & file, GroupID gid)
    {
        if (gid == GroupUnknow || gid == 0)
            gid = ResourceGroup;
        PixelBufferXmlAnalyze prc;
        DataStream * data = N_Only(ResourceScheme).open(file, gid);
        if(data)
        {
            N_Engine().getXmlPattern()->parse(&prc, data);
            N_delete data;
        }
        return prc.getGroupList();
    }
    //------------------------------------------------------------------------
    PixelBufferGroup * PixelManager::create(PixelBufferID pid, const String & file, GroupID rgid)
    {
        if (rgid == GroupUnknow || rgid == 0)
            rgid = ResourceGroup;
        Texture * tex = N_Only(Texture).createTexture(0, rgid, file);
        Rectf rect(0.0f, 0.0f, PlaneSizef(tex->getOriginWidth(), tex->getOriginHeight()));

        PixelBufferGroup * pbg = create(tex);
        PixelBuffer * pb = create(pid, pbg, rect);
        pbg->add(pb);
        return pbg;
    }
    //------------------------------------------------------------------------
    PixelBufferGroup * PixelManager::create(Texture * obj, ScaleMode mode, const Vector2f & oft,
        const Vector2f & pixeloft, const PlaneSizef & srcRes)
    {
        ResourceGroupList::iterator i, end = mPBRList.upper_bound(obj->getOriginID());
        for (i = mPBRList.lower_bound(obj->getOriginID()); i != end; ++i)
        {
            PixelBufferGroup * pbg = i->second;
            if (pbg->getMode() == mode && pbg->getOffset() == oft && pbg->getPixelOffset() == pixeloft && 
                pbg->getNativeRes() == srcRes)
            {
                return i->second;
            }
        }
        PixelBufferGroup * re = N_new PixelBufferGroup(genValidID(), obj, mode, oft, pixeloft, srcRes);

        mPBGroupList.insert(Npair(re->getGroupID(), re));
        mPBRList.insert(Npair(obj->getOriginID(), re));
        return re;
    }
    //------------------------------------------------------------------------
    PixelBufferGroup * PixelManager::getGroup(GroupID gid)
    {
        PixelGroupList::iterator i = mPBGroupList.find(gid);
        if (i != mPBGroupList.end())
        {
            return i->second;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool PixelManager::isGroupExist(GroupID gid)
    {
        PixelGroupList::iterator i = mPBGroupList.find(gid);
        if (i != mPBGroupList.end())
        {
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void PixelManager::destroyGroup(GroupID gid)
    {
        PixelGroupList::iterator i = mPBGroupList.find(gid);
        if (i != mPBGroupList.end())
        {
            mPBGroupList.erase(i);

            ResourceGroupList::iterator j, end = mPBRList.upper_bound(gid);
            for (j = mPBRList.lower_bound(gid); j != end; ++j)
            {
                mPBRList.erase(j);
            }
            N_delete i->second;
        }
    }
    //------------------------------------------------------------------------
    PixelBuffer * PixelManager::create(PixelBufferID pid, PixelBufferGroup * pbg, const Rectf & area)
    {
        PixelBuffer * pb = N_new PixelBuffer(pid, pbg, area);
        pbg->add(pb);
        return pb;
    }
    //------------------------------------------------------------------------
    PixelBuffer * PixelManager::get(PixelBufferComID id) const
    {
        PixelGroupList::const_iterator g = mPBGroupList.find(GroupID(id >> N_Com_Escape));
        if (g != mPBGroupList.end())
        {
            return g->second->get(PixelBufferID(id & N_Com_EscapeMark));
        }
        return 0;
    }
    //------------------------------------------------------------------------
    PixelBuffer * PixelManager::get(GroupID rid, PixelBufferID pid) const
    {
        PixelGroupList::const_iterator g = mPBGroupList.find(rid);
        if (g != mPBGroupList.end())
        {
            return g->second->get(pid);
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool PixelManager::isExist(PixelBufferComID id) const
    {
        PixelGroupList::const_iterator g = mPBGroupList.find(GroupID(id >> N_Com_Escape));
        if (g != mPBGroupList.end())
        {
            return g->second->isExist(PixelBufferID(id & N_Com_EscapeMark));
        }

        return false;
    }
    //------------------------------------------------------------------------
    bool PixelManager::isExist(GroupID rid, PixelBufferID pid) const
    {
        PixelGroupList::const_iterator g = mPBGroupList.find(rid);
        if (g != mPBGroupList.end())
        {
            return g->second->isExist(pid);
        }

        return false;
    }
    //------------------------------------------------------------------------
    void PixelManager::destroy(GroupID rid, PixelBufferID pid, bool deltex)
    {
        PixelGroupList::iterator j = mPBGroupList.find(rid);
        if (j != mPBGroupList.end())
        {
            PixelBufferGroup * pbg = j->second;
            PixelBuffer * de = pbg->remove(pid);
            if(de)
            {
                N_delete de;
                if (pbg->getCount() == 0)
                {
                    ResourceGroupList::iterator i, end = mPBRList.upper_bound(rid);
                    for (i = mPBRList.lower_bound(rid); i != end; ++i)
                    {
                        if (i->second == pbg)
                        {
                            mPBRList.erase(i);
                            break;
                        }
                    }

                    if (deltex)
                    {
                        N_Only(Texture).destroy(rid);
                    }
                    N_delete pbg;
                    mPBGroupList.erase(j);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void PixelManager::destroy(GroupID gid, bool deltex)
    {
        PixelGroupList::iterator j = mPBGroupList.find(gid);
        if(j != mPBGroupList.end())
        {
            ResourceID rid = j->second->getTexture()->getOriginID();
            ResourceGroupList::iterator i, end = mPBRList.upper_bound(rid);
            for (i = mPBRList.lower_bound(rid); i != end; ++i)
            {
                if (i->second == j->second)
                {
                    mPBRList.erase(i);
                }
            }
            if (deltex)
            {
                N_Only(Texture).destroy(rid);
            }
            N_delete j->second;
            mPBGroupList.erase(j);
        }
    }
    //------------------------------------------------------------------------
    void PixelManager::destroyAll(bool deltex)
    {
        PixelGroupList::iterator g, gend = mPBGroupList.end();
        for(g = mPBGroupList.begin(); g != gend; ++g)
        {
            ResourceID rid = g->second->getTexture()->getOriginID();

            N_delete g->second;
            if(deltex)
            {
                N_Only(Texture).destroy(rid);
            }
        }
        mPBGroupList.clear();
    }
    //------------------------------------------------------------------------
    NCount PixelManager::getCount(GroupID rid) const
    {
        PixelGroupList::const_iterator g = mPBGroupList.find(rid);
        if (g != mPBGroupList.end())
        {
            return g->second->getCount();
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void PixelManager::notifyView(const PlaneSizei & size)
    {
        PixelGroupList::iterator g, gend = mPBGroupList.end();
        for(g = mPBGroupList.begin(); g != gend; ++g)
        {
            g->second->notifyView(size);
        }
    }
    //-----------------------------------------------------------------------
    GroupID PixelManager::genValidID()const
    {
        N_lock_mutex(mValidGIDMutex)
        ResourceID temp = ++mValidRID;
        return temp;
    }
    //------------------------------------------------------------------------
}