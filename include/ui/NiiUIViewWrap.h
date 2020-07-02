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

#ifndef _NII_UI_VIEWWRAP_H_
#define _NII_UI_VIEWWRAP_H_

#include "NiiPreInclude.h"
#include "NiiUIPixelLayout.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    template <typename T> class ViewWrap : public PixelLayout
    {
    public:
        ViewWrap(const PixelUnitGrid * str);
        ~ViewWrap();

        ///@copydetails PixelLayout::layout
        void layout(const Widget * dest, const PlaneSizef & area_size);
        
        ///@copydetails PixelLayout::draw
        void draw(const Widget * dest, GeometryPixel & buffer, const Vector2f & pos,
            const ColourArea * mc, const Rectf * clip_rect) const;
            
        ///@copydetails PixelLayout::getRowCount
        NCount getRowCount() const;
        
        ///@copydetails PixelLayout::getHExtent
        NIIf getHExtent(const Widget * dest) const;
        
        ///@copydetails PixelLayout::getVExtent
        NIIf getVExtent(const Widget * dest) const;
    protected:
        typedef vector<PixelLayout *>::type LineList;
        LineList mLineList;
    };

    template<> _EngineAPI void ViewWrap<JustifyLayout>::layout(const Widget * dest, const PlaneSizef & area_size);
    //------------------------------------------------------------------------
    template <typename T> ViewWrap<T>::ViewWrap(const PixelUnitGrid * str) :
        PixelLayout(str)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> ViewWrap<T>::~ViewWrap()
    {
        NCount i, iend = mLineList.size();
        for(i = 0; i < iend; ++i)
        {
            const PixelUnitGrid * rs = mLineList[i]->getGrid();
            N_delete rs;
            N_delete mLineList[i];
        }
        mLineList.clear();
    }
    //------------------------------------------------------------------------
    template <typename T>
    void ViewWrap<T>::layout(const Widget * dest, const PlaneSizef & area_size)
    {
        NCount i, iend = mLineList.size();
        for(i = 0; i < iend; ++i)
        {
            const PixelUnitGrid * rs = mLineList[i]->getGrid();
            N_delete rs;
            N_delete mLineList[i];
        }
        mLineList.clear();

        PixelUnitGrid rstring, lstring;
        rstring = *mPixelGrid;
        NIIf rs_width;

        T * frs;
        NCount l, lend = rstring.getLineCount();
        for(l = 0; l < lend; ++l)
        {
            while((rs_width = rstring.getPixelSize(dest, l).mWidth) > 0)
            {
                // skip line if no wrapping occurs
                if (rs_width <= area_size.mWidth)
                    break;

                // split rstring at width into lstring and remaining rstring
                rstring.split(dest, l, area_size.mWidth, lstring);
                frs = N_new T(N_new PixelUnitGrid(lstring));
                frs->layout(dest, area_size);
                mLineList.push_back(frs);
                l = 0;
            }
        }

        frs = N_new T(N_new PixelUnitGrid(rstring));
        frs->layout(dest, area_size);
        mLineList.push_back(frs);
    }
    //------------------------------------------------------------------------
    template <typename T> void ViewWrap<T>::draw(const Widget * dest, GeometryPixel & buffer,
        const Vector2f & pos, const ColourArea * colour, const Rectf * clip) const
    {
        Vector2f line_pos(pos);
        typename LineList::const_iterator i, iend = mLineList.begin();
        for(i = mLineList.begin(); i != iend; ++i)
        {
            (*i)->draw(dest, buffer, line_pos, colour, clip);
            line_pos.y += (*i)->getVExtent(dest);
        }
    }
    //------------------------------------------------------------------------
    template <typename T> NCount ViewWrap<T>::getRowCount() const
    {
        return mLineList.size();
    }
    //------------------------------------------------------------------------
    template <typename T> NIIf ViewWrap<T>::getHExtent(const Widget * dest) const
    {
        NIIf finalw = 0;
        NIIf w;
        typename LineList::const_iterator i, iend = mLineList.end();
        for(i = mLineList.begin(); i != iend; ++i)
        {
            w = (*i)->getHExtent(dest);
            if (w > finalw)
                finalw = w;
        }

        return finalw;
    }
    //------------------------------------------------------------------------
    template <typename T> NIIf ViewWrap<T>::getVExtent(const Widget * dest) const
    {
        NIIf reh = 0;
        typename LineList::const_iterator i, iend = mLineList.end();
        for(i = mLineList.begin(); i != iend; ++i)
        {
            reh += (*i)->getVExtent(dest);
        }
        return reh;
    }
    //------------------------------------------------------------------------
}
}
#endif