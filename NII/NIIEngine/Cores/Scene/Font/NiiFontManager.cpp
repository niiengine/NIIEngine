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
#include "NiiFontManager.h"
#include "NiiResourceSchemeManager.h"
#include "NiiFontXmlAnalyze.h"
#include "NiiScriptCompilerManager.h"
#include "NiiException.h"
#include "NiiXmlPattern.h"
#include "NiiPixmapFont.h"
#include "NiiFreeTypeFont.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiString.h"

namespace NII
{
    //------------------------------------------------------------------------
    template<> FontManager * Singleton<FontManager>::mOnly = 0;
    GroupID FontManager::ResourceGroup = GroupUnknow;
    //------------------------------------------------------------------------
    FontManager::FontManager() :
        ResourceManager(RT_Font)
    {
        mScriptLevel = 200;
        mScriptPattern.push_back(_T("*.font"));

        // Register scripting with resource group manager
        N_Only(ResourceScheme).add(this);
        // Register with resource group manager
        N_Only(ResourceScheme).add(mType, this);
    }
    //------------------------------------------------------------------------
    FontManager::~FontManager()
    {
        destroyAll();

        N_Only(ResourceScheme).remove(mType);
        N_Only(ResourceScheme).remove(this);
    }
    //------------------------------------------------------------------------
    void FontManager::createDefine(const String & pattern, GroupID gid)
    {
        StringList names;
        N_Only(ResourceScheme).find(gid, names, pattern);
        StringList::iterator i, iend = names.end();
        for(i = names.begin(); i != iend; ++i)
        {
            create(*i, gid);
        }
    }
    //------------------------------------------------------------------------
    Font * FontManager::create(const String & file, GroupID gid)
    {
        if (gid == 0 || gid == GroupUnknow)
            gid = ResourceGroup;
        FontXmlAnalyze analyze;
        analyze.setSkipOther(true);
        DataStream * data = N_Only(ResourceScheme).open(file, gid);
        if(data)
        {
            try
            {
                N_Engine().getXmlPattern()->parse(&analyze, data);
            }
            catch(...)
            {
                N_delete data;
                throw;
            }
            N_delete data;
        }
        FontList::const_iterator i, iend = analyze.getObjectList().end();
        for(i = analyze.getObjectList().begin(); i != iend; ++i)
        {
            addImpl(*i);
        }
        if (analyze.getObjectList().size())
            return *analyze.getObjectList().begin();
        return 0;
    }
    //------------------------------------------------------------------------
    Font * FontManager::getFont(FontID fid) const
    {
        FontMap::const_iterator i = mFontList.find(fid);
        if(i != mFontList.end())
            return i->second;

        return 0;
    }
    //------------------------------------------------------------------------
    bool FontManager::isFontExist(FontID fid) const
    {
        FontMap::const_iterator i = mFontList.find(fid);
        if(i != mFontList.end())
            return true;

        return false;
    }
    //------------------------------------------------------------------------
    void FontManager::destroyFont(FontID fid)
    {
        FontMap::iterator i = mFontList.find(fid);
        if(i != mFontList.end())
        {
            Font * temp = i->second;
            removeImpl(temp);
            N_delete temp;
        }
    }
    //------------------------------------------------------------------------
    Font * FontManager::createFreeType(FontID fid, NIIf size, NCount pdi,
        bool antialias, const String & fontfile, GroupID gid, ScaleMode sm,
            const PlaneSizef & nativeRes)
    {
        Font * re = getFont(fid);
         if(re)
            return re;

        re = N_new FreeTypeFont(fid, size, pdi, antialias, fontfile,
            genValidID(), gid, 0, 0, sm, nativeRes);

        addImpl(re);

        N_Only(ResourceScheme).onCreate(re);

        return re;
    }
    //------------------------------------------------------------------------
    Font * FontManager::createPixmap(FontID fid, const String & imagesrc,
        GroupID gid, ScaleMode sm, const PlaneSizef & nativeRes)
    {
        Font * re = getFont(fid);
         if(re)
            return re;

        re = N_new PixmapFont(fid, imagesrc, genValidID(), gid, 0, 0, sm, nativeRes);

        addImpl(re);

        N_Only(ResourceScheme).onCreate(re);

        return re;
    }
    //------------------------------------------------------------------------
    void FontManager::notifyView(const PlaneSizei & size)
    {
        FontMap::iterator i, iend = mFontList.end();
        for(i = mFontList.begin(); i != iend; ++i)
        {
            i->second->setViewRes(size);
        }
    }
    //------------------------------------------------------------------------
    void FontManager::notifySize(const Font * obj)
    {

    }
    //------------------------------------------------------------------------
    void FontManager::write(FontID rid, XmlSerializer * out, LangID lid) const
    {
        FontMap::const_iterator i = mFontList.find(rid);
        if(i != mFontList.end())
        {
            FontXmlAnalyze xml(lid, i->second->getFontType());
            xml.write(out, i->second);
        }
    }
    //---------------------------------------------------------------------
    void FontManager::init()
    {
    }
    //---------------------------------------------------------------------
    Resource * FontManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
        N_assert(params->isExist(N_PropertyFont_Type), "error");
        Nui32 fid;
        params->get(N_PropertyFont_ID, fid);
        const String & src = params->get(N_PropertyFont_Src);
        const String & type = params->get(N_PropertyFont_Type);

        if(type == _T("freetype"))
        {
            bool tbool;
            NIIf tf1;
            params->get(N_PropertyFont_Size, tf1, 12.0f);
            params->get(N_PropertyFont_AntiAlias, tbool, true);
            return N_new FreeTypeFont(fid, tf1, 96, tbool, src, rid, gid, ls, rs);
        }
        else if(type == _T("pixmap"))
        {
            return N_new PixmapFont(fid, src, rid, gid, ls, rs);
        }

        return 0;
    }
    //---------------------------------------------------------------------
    void FontManager::parse(DataStream * stream, GroupID gid, PtrList & out)
    {
        N_Only(ScriptCompiler).parse(stream, gid, out);
    }
    //---------------------------------------------------------------------
    void FontManager::addImpl(Resource * obj)
    {
        Font * temp = static_cast<Font *>(obj);
        ResourceManager::addImpl(obj);
        mFontList.insert(Npair(temp->getID(), temp));
    }
    //---------------------------------------------------------------------
    void FontManager::removeImpl(Resource * obj)
    {
        ResourceManager::removeImpl(obj);
        Font * temp = static_cast<Font *>(obj);
        FontMap::iterator i = mFontList.find(temp->getID());
        if(i != mFontList.end())
        {
            mFontList.erase(i);
        }
    }
    //------------------------------------------------------------------------
}