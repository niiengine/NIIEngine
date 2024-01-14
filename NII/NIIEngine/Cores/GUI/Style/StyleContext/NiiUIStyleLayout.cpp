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
#include "NiiUIStyleLayout.h"
#include "NiiUIManager.h"
#include "NiiGeometryPixel.h"
#include "NiiXmlSerializer.h"
#include "NiiUIStyleSection.h"
#include "NiiUIException.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIContainer.h"
#include <iostream>

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Layer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Layer::Layer(NCount level) :
        mLevel(level)
    {
    }
    //------------------------------------------------------------------------
    void Layer::render(const Widget * dest, const ColourArea * ca, const Rectf * clip) const
    {
        Sections::const_iterator i, iend = mSections.end();
        for(i = mSections.begin(); i != iend; ++i)
        {
            (*i).render(dest, ca, clip);
        }
    }
    //------------------------------------------------------------------------
    void Layer::render(const Widget * dest, const Rectf & area, const ColourArea * ca,
        const Rectf * clip) const
    {
        Sections::const_iterator i, iend = mSections.end();
        for(i = mSections.begin(); i != iend; ++i)
        {
            (*i).render(dest, area, ca, clip);
        }
    }
    //------------------------------------------------------------------------
    void Layer::write(XmlSerializer * out) const
    {
        out->begin(_T("Layer"));

        if (mLevel != 0)
        {
            out->attribute(_T("priority"), mLevel);
        }
        Sections::const_iterator i, iend = mSections.end();
        for(i = mSections.begin(); i != iend; ++i)
        {
            (*i).write(out);
        }
        out->end();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Section
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Section::Section()
    {
    }
    //------------------------------------------------------------------------
    Section::Section(StyleID style, Nid section, PropertyID ctlproperty, WidgetID ctlwidget) :
        mStyle(style),
        mStyleSection(section),
        mColourID(0),
        mCtlProperty(ctlproperty),
        mCtlWidget(ctlwidget)
    {
        Colour val(1, 1, 1, 1);
        mColour.mTL = val;
        mColour.mTR = val;
        mColour.mBL = val;
        mColour.mBR = val;
    }
    //------------------------------------------------------------------------
    Section::Section(StyleID style, Nid section, PropertyID ctlproperty,
        WidgetID ctlwidget, const ColourArea & cols) :
            mStyle(style),
            mColour(cols),
            mStyleSection(section),
            mColourID(0),
            mCtlProperty(ctlproperty),
            mCtlWidget(ctlwidget)
    {
    }
    //------------------------------------------------------------------------
    void Section::render(const Widget * dest, const ColourArea * mc, const Rectf * clipper) const
    {
        assert(dest);
        if(mCtlProperty != 0)
        {
            bool temp;
            if(mCtlWidget == 0)
            {
                dest->get(mCtlProperty, &temp);
            }
            else if(mCtlWidget == N_Parent_Obj)
            {
                Widget * tdest = dest->getParent();
                if(tdest == 0)
                    return;
                tdest->get(mCtlProperty, &temp);
            }
            else
            {
                Widget * tdest = static_cast<const Container *>(dest)->get(mCtlWidget);
                if(tdest == 0)
                    return;
                tdest->get(mCtlProperty, &temp);
            }
            if(temp == false)
                return;
        }

        try
        {
            const StyleSection * sect = &N_Only(Widget).getStyle(mStyle)->getDesign(mStyleSection);

            ColourArea c;
            if(mColourID != 0)
            {
                dest->get(mColourID, &c);
            }
            else
            {
                c = mColour;
            }
            c.setAlphaFactor(dest->getAlphaEffect());

            if(mc)
                c *= *mc;

            sect->render(dest, &c, clipper);
        }
        catch(RuntimeException &)
        {
        }
    }
    //------------------------------------------------------------------------
    void Section::render(const Widget * dest, const Rectf & rect, const ColourArea * mc,
        const Rectf * clipper) const
    {
        if(mCtlProperty != 0)
        {
            bool temp;
            if(mCtlWidget == 0)
            {
                if(dest == 0)
                    return;
                dest->get(mCtlProperty, &temp);
            }
            else if(mCtlWidget == N_Parent_Obj)
            {
                Widget * tdest = dest->getParent();
                if(tdest == 0)
                    return;
                tdest->get(mCtlProperty, &temp);
            }
            else
            {
                Widget * tdest = static_cast<const Container *>(dest)->get(mCtlWidget);
                if(tdest == 0)
                    return;
                tdest->get(mCtlProperty, &temp);
            }
            if(temp == false)
                return;
        }

        try
        {
            const StyleSection * sect = &N_Only(Widget).getStyle(mStyle)->getDesign(mStyleSection);

            ColourArea c;
            if(mColourID != 0)
            {
                dest->get(mColourID, &c);
            }
            else
            {
                c = mColour;
            }
            c.setAlphaFactor(dest->getAlphaEffect());

            if(mc)
                c *= *mc;

            sect->render(dest, rect, &c, clipper);
        }
        catch (RuntimeException &)
        {
        }
    }
    //------------------------------------------------------------------------
    void Section::write(XmlSerializer * out) const
    {
        out->begin(_T("Section"));

        if(mStyle != 0)
            out->attribute(_T("widget_style"), mStyle);

        out->attribute(_T("style_section"), mStyleSection);

        if(mCtlProperty != 0)
            out->attribute(_T("ctl_property"), mCtlProperty);

        if(mCtlWidget != 0)
            out->attribute(_T("ctl_widget"), mCtlWidget);

        if(mColourID != 0)
        {
            out->begin(_T("ColourAttribute"));
            out->attribute(_T("id"), mColourID).end();
        }
        else if(!mColour.isOneColour() || mColour.mTL != Colour(1,1,1,1))
        {
            out->begin(_T("Colours"))
                .attribute(_T("top_left"), mColour.mTL.toStr())
                .attribute(_T("top_right"), mColour.mTR.toStr())
                .attribute(_T("bottom_left"), mColour.mBL.toStr())
                .attribute(_T("bottom_right"), mColour.mBR.toStr())
                .end();
        }
        out->end();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleLayout
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleLayout::StyleLayout() :
        mID(0),
        mMark(0),
        mClip(true)
    {
    }
    //------------------------------------------------------------------------
    StyleLayout::StyleLayout(Nid id) :
        mID(id),
        mMark(0),
        mClip(true)
    {
    }
    //------------------------------------------------------------------------
    StyleLayout::StyleLayout(const StyleLayout & o)
    {
        mID = o.mID;
        mLayers = o.mLayers;
        mMark = o.mMark;
        mClip = o.mClip;
    }
    //------------------------------------------------------------------------
    void StyleLayout::render(const Widget * dest, const ColourArea * ca, const Rectf * clip) const
    {
        dest->getPixelData()->setClip(mClip);
        if(mClip)
            clip = 0;
        Layers::const_iterator i, iend = mLayers.end();
        for(i = mLayers.begin(); i != iend; ++i)
            (*i).render(dest, ca, clip);
    }
    //------------------------------------------------------------------------
    void StyleLayout::render(const Widget * dest, const Rectf & baseRect, const ColourArea * ca,
        const Rectf * clip) const
    {
        dest->getPixelData()->setClip(mClip);
        if(mClip)
            clip = 0;
        Layers::const_iterator i, iend = mLayers.end();
        for(i = mLayers.begin(); i != iend; ++i)
            (*i).render(dest, baseRect, ca, clip);
    }
    //------------------------------------------------------------------------
    void StyleLayout::write(XmlSerializer * out) const
    {
        out->begin(_T("StyleLayout")).attribute(_T("name"), mID);

        if(mClip)
            out->attribute(_T("clipped"), _T("true"));
        else
            out->attribute(_T("clipped"), _T("false"));

        // output all layers defined for this state
        Layers::const_iterator i, iend = mLayers.end();
        for(i = mLayers.begin(); i != iend; ++i)
            (*i).write(out);

        out->end();
    }
    //------------------------------------------------------------------------
}
}