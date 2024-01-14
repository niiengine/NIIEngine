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
#include "NiiUIStyleWidget.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIContainer.h"
#include "NiiUIException.h"
#include "NiiUIStrConv.h"
#include "NiiXmlSerializer.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AssignProperty
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    AssignProperty::AssignProperty()
    {
    }
    //------------------------------------------------------------------------
    AssignProperty::AssignProperty(PropertyID pid, const String & value) :
        mProperty(pid),
        mValue(value)
    {
    }
    //------------------------------------------------------------------------
    void AssignProperty::apply(PropertyCmdObj * obj) const
    {
        try
        {
            obj->set(mProperty, mValue);
        }
        catch (NotExistException &)
        {
        }
    }
    //------------------------------------------------------------------------
    void AssignProperty::write(XmlSerializer * out) const
    {
        out->begin(_T("Property")).attribute(_T("name"), mProperty)
            .attribute(_T("value"), mValue).end();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleWidget
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleWidget::StyleWidget()
    {
    }
    //------------------------------------------------------------------------
    StyleWidget::StyleWidget(StyleViewID sid, WidgetID wid,  bool autowin) :
        mViewID(sid),
        mID(wid),
        mAutoGen(autowin),
        mVAlign(VA_TOP),
        mHAlign(HA_LEFT)
    {
    }
    //------------------------------------------------------------------------
    void StyleWidget::attach(Widget * own) const
    {
        N_assert(dynamic_cast<Container *>(own), "error logic");

        Widget * widget = N_Only(Widget).create(mViewID, mID, static_cast<Container *>(own));
        widget->setAutoWindow(mAutoGen);
        widget->setYAlign(mVAlign);
        widget->setXAlign(mHAlign);

        // TODO: We still need code to specify position and size.  Due to the advanced
        // TODO: possibilities, this is better handled via a 'layout' method instead of
        // TODO: setting this once and forgetting about it.

        // initialise properties.  This is done last so that these properties can
        // override properties specified in the look assigned to the created widget.
        PropertyList::const_iterator curr, currend = mPropertyList.end();
        for(curr = mPropertyList.begin(); curr != currend; ++curr)
        {
            (*curr).apply(own);
        }
    }
    //------------------------------------------------------------------------
    void StyleWidget::detach(Widget * own) const
    {
        if (own->isContainer())
            return;
        Container * temp = static_cast<Container *>(own);
        if(!temp->isExist(mID))
            return;

        Widget * wid = temp->get(mID);

        temp->destroy(wid);
    }
    //------------------------------------------------------------------------
    void StyleWidget::add(const AssignProperty & assign)
    {
        mPropertyList.push_back(assign);
    }
    //------------------------------------------------------------------------
    void StyleWidget::removeAssignProperty(PropertyID pid)
    {
        PropertyList::iterator i, iend = mPropertyList.end();
        for(i = mPropertyList.begin(); i != iend; ++i)
        {
            if (i->getProperty() == pid)
            {
                mPropertyList.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void StyleWidget::removeAllAssignProperty()
    {
        mPropertyList.clear();
    }
    //------------------------------------------------------------------------
    void StyleWidget::layout(Widget * dst) const
    {
        try
        {
            Rectf pixelArea(mArea.getPixelRect(*dst));
            RelRectf window_area(RelPosf(0, pixelArea.left()), RelPosf(0, pixelArea.top()),
                RelPosf(0, pixelArea.right()), RelPosf(0, pixelArea.bottom()));

            Widget * wnd = static_cast<Container *>(dst)->get(mID);
            wnd->setRelArea(RelRectf(window_area.mLeft, window_area.mTop, window_area.getSize()));
            wnd->notifyPosChange();
        }
        catch(UnknownObjectException &)
        {
        }
    }
    //------------------------------------------------------------------------
    const AssignProperty * StyleWidget::getAssignProperty(PropertyID name) const
    {
        PropertyList::const_reverse_iterator i = mPropertyList.rbegin();
        while(i != mPropertyList.rend())
        {
            if((*i).getProperty() == name)
                return &(*i);
            ++i;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void StyleWidget::write(XmlSerializer * out) const
    {
        // output opening tag
        out->begin(_T("Widget")).attribute(_T("type"), mViewID).attribute(_T("name"), mID);

        if(!mAutoGen)
            out->attribute(_T("autoWindow"), _T("false"));

        // output target area
        mArea.write(out);

        // output vertical alignment
        out->begin(_T("VAlign")).attribute(_T("type"), NUI_conv(mVAlign)).end();

        // output horizontal alignment
        out->begin(_T("HAlign")).attribute(_T("type"), NUI_conv(mHAlign)).end();

        //output property initialisers
        PropertyList::const_iterator prop, propend = mPropertyList.end();
        for(prop = mPropertyList.begin(); prop != propend; ++prop)
        {
            (*prop).write(out);
        }

        // output closing tag
        out->end();
    }
    //------------------------------------------------------------------------
}
}