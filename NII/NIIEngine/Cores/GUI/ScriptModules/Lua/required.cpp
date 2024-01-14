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

#include "NiiUI.h"
#include "required.h"

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    /*************************************************************************
        Functions for getting PosButton range pairs as two return values
    *************************************************************************/

    // returns horizontal range as two values for lua
    void ceguiLua_Thumb_getHorzRange(PosButton * wnd, float * min, float * max)
    {
        NIIf _min, _max;
        wnd->getXRange(_min, _max);
        *min = _min;
        *max = _max;
    }

    // returns vertical range as two values for lua
    void ceguiLua_Thumb_getVertRange(PosButton * wnd, float * min, float * max)
    {
        NIIf _min, _max;
        wnd->getYRange(_min, _max);
        *min = _min;
        *max = _max;
    }

    /*************************************************************************
        Functions for creating list box items
    *************************************************************************/
    // allocates and returns a new ListboxTextItem
    ListboxTextItem * ceguiLua_createListboxTextItem(const String & text, 
        Nui32 item_id, void* item_data, bool disabled, bool auto_delete)
    {
        return new ListboxTextItem(text,item_id,item_data,disabled,auto_delete);
    }
    /*************************************************************************
    Functions for creating tree items
    *************************************************************************/

    // allocates and returns a new TreeItem
    TreeItem * ceguiLua_createTreeItem(const String & text, Nui32 item_id, 
        void * item_data, bool disabled, bool auto_delete)
    {
        return new TreeItem(text,item_id,item_data,disabled,auto_delete);
    }
    /************************************************************************
        std::ostream
    *************************************************************************/
    void ceguiLua_FileStream_open(FileStream * os, const char * filename)
    {
        os->open(filename, std::ios::binary);
    }
    /************************************************************************
        EventConnection helper class implementation
    *************************************************************************/
    EventConnection::EventConnection(SharedPtr<EventFunctor> slot) :
        d_slot(slot)
    {
    }
    //----------------------------------------------------------------------------//
    bool EventConnection::connected() const
    {
        return d_slot->connected();
    }
    //----------------------------------------------------------------------------//
    void EventConnection::disconnect()
    {
        d_slot->disconnect();
    }
    //----------------------------------------------------------------------------//
    /************************************************************************
        ceguiLua_PropertyHelper class implementation
    *************************************************************************/
    float ceguiLua_PropertyHelper::stringToFloat(const String & str)
    {
        float temp;
        StrConv::conv(str, temp);
        return temp;
    }

    //----------------------------------------------------------------------------//
    unsigned int ceguiLua_PropertyHelper::stringToUint(const String & str)
    {
        Nui32 temp;
        StrConv::conv(str, temp);
        return temp;
    }

    //----------------------------------------------------------------------------//
    bool ceguiLua_PropertyHelper::stringToBool(const String & str)
    {
        bool temp;
        StrConv::conv(str, temp);
        return temp;
    }

    //----------------------------------------------------------------------------//
    PlaneSizef ceguiLua_PropertyHelper::stringToSize(const String & str)
    {
        PlaneSizef temp
        UIStrConv::conv(str, temp);
        return temp;
    }

    //----------------------------------------------------------------------------//
    Vector2f ceguiLua_PropertyHelper::stringToVector2(const String & str)
    {
        Vector2f re;
        return StrConv::conv(str, re);
        return re;
    }
    //----------------------------------------------------------------------------//
    Rectf ceguiLua_PropertyHelper::stringToRect(const String & str)
    {
        Rectf temp;
        StrConv::conv(str, temp);
        return temp;
    }

    //----------------------------------------------------------------------------//
    const PixelBuffer* ceguiLua_PropertyHelper::stringToImage(const String & str)
    {
        StringList temp;
        StrUtil::split(str, temp, "|");
        Nui32 rid, pid;
        StrConv::conv(temp[0], rid);
        StrConv::conv(temp[1], pid);
        return N_Only(Pixel).get(rid, pid);
    }

    //----------------------------------------------------------------------------//
    Colour ceguiLua_PropertyHelper::stringToColour(const String & str)
    {
        Colour temp;
        StrConv::conv(str, temp);
        return temp;
    }

    //----------------------------------------------------------------------------//
    ColourArea ceguiLua_PropertyHelper::stringToColourArea(const String & str)
    {
        ColourArea temp;
        StrConv::conv(str, temp);
        return temp;
    }

    //----------------------------------------------------------------------------//
    RelPos ceguiLua_PropertyHelper::stringToUDim(const String & str)
    {
        RelPos temp;
        StrConv::conv(str, temp);
        return temp;
    }

    //----------------------------------------------------------------------------//
    RelVector2 ceguiLua_PropertyHelper::stringToRelVector2(const String & str)
    {
        RelVector2 temp;
        StrConv::conv(str, temp);
        return temp;
    }
    //----------------------------------------------------------------------------//
    RelPlaneSize ceguiLua_PropertyHelper::stringToRelPlaneSize(const String & str)
    {
        RelPlaneSize temp;
        StrConv::conv(str, temp);
        return temp;
    }
    //----------------------------------------------------------------------------//
    RelRect ceguiLua_PropertyHelper::stringToRelRect(const String& str)
    {
        RelRect temp;
        StrConv::conv(str, temp);
        return temp;
    }
    //----------------------------------------------------------------------------//
    AspectMode ceguiLua_PropertyHelper::stringToAspectMode(const String& str)
    {
        AspectMode temp;
        StrConv::conv(str, temp);
        return temp;
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::floatToString(float val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::uintToString(unsigned int val)
    {
        return StrConv::conv((Nui32)temp);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::boolToString(bool val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::sizeToString(const PlaneSizef & val)
    {
        return StrConv::conv(temp);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::vector2ToString(const Vector2f & val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::rectToString(const Rectf & val)
    {
        return StrConv::conv(temp);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::imageToString(const PixelBuffer * val)
    {
        return StrConv::conv(val->getGroupID()) + "|" + StrConv::conv(val->getID());
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::colourToString(const Colour & val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::colourRectToString(const ColourArea & val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::udimToString(const RelPos & val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::uvector2ToString(const RelVector2 & val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::usizeToString(const RelPlaneSize & val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::urectToString(const RelRect & val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
    String ceguiLua_PropertyHelper::aspectModeToString(AspectMode val)
    {
        return StrConv::conv(val);
    }
    //----------------------------------------------------------------------------//
}
}
}