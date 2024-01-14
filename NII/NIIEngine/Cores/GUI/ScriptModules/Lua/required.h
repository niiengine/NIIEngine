/***********************************************************************
	filename: required.h
	created:  16/3/2005
	author:   Tomas Lindquist Olsen

	purpose:  Header that includes the necessary stuff needed for the Lua bindings
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
 
#include "NiiUI.h"
#include "CEGUI/ScriptModules/Lua/ScriptModule.h"
#include "CEGUI/ScriptModules/Lua/Functor.h"
#include "NiiUISheetContext.h"
#include "NiiUIInstance.h"
#include "NiiGeometryPixel.h"
#include "NiiVertex.h"
#include "CEGUI/RenderEffect.h"
#include <fstream>

#define __operator_increment    operator++
#define __operator_decrement    operator--
#define __operator_dereference  operator*

#define LuaFunctorSubscribeEvent NII::LuaFunctor::SubscribeEvent

//This is used to keep compilers happy
#define CEGUIDeadException(e) &

#if defined(_MSC_VER) && !defined(snprintf)
#   define snprintf   _snprintf
#endif

// map the utf8string funcs to c-string funcs
#define tolua_pushutf8string(x,y) tolua_pushstring(x,y)
#define tolua_isutf8string tolua_isstring
#define tolua_isutf8stringarray tolua_isstringarray
#define tolua_pushfieldutf8string(L,lo,idx,s) tolua_pushfieldstring(L, lo, idx, s)
#define tolua_toutf8string tolua_tostring
#define tolua_tofieldutf8string tolua_tofieldstring

typedef NII::String string;
typedef NII::Nutf8 * utf8string;

namespace NII
{
    /*************************************************************************
        Helper class to enable us to represent an SignalPtr object
        (which is actually a SharedPtr<EventFunctor> object)
    *************************************************************************/
    class EventConnection
    {
        SharedPtr<EventFunctor> d_slot;
    public:
        EventConnection(SharedPtr<EventFunctor> slot);
        bool connected() const;
        void disconnect();
    };

    /*************************************************************************
        Functions for getting PosButton range pairs as two return values
    *************************************************************************/
    void ceguiLua_Thumb_getHorzRange(PosButton * wnd, float * min, float * max);
    void ceguiLua_Thumb_getVertRange(PosButton * wnd, float * min, float * max);

    /*************************************************************************
        Functions for creating list box items
    *************************************************************************/
    ListboxTextItem * ceguiLua_createListboxTextItem(const String& text, Nui32 item_id, void* item_data, bool disabled, bool auto_delete);

    /*************************************************************************
    Functions for creating tree items
    *************************************************************************/
    TreeItem * ceguiLua_createTreeItem(const String& text, Nui32 item_id, void* item_data, bool disabled, bool auto_delete);

    /************************************************************************
        Stuff needed to make the iterators work
    *************************************************************************/
    typedef WidgetManager::WidgetModelDefine WidgetModelDefine;

    typedef std::ofstream FileStream;
    void ceguiLua_FileStream_open(FileStream*, const char* filename);

    /************************************************************************
        wrapper class for String convert de-templatise it.
    *************************************************************************/
    class ceguiLua_PropertyHelper
    {
    public:
        static float stringToFloat(const String & str);
        static unsigned int stringToUint(const String & str);
        static bool stringToBool(const String & str);
        static PlaneSizef stringToSize(const String & str);
        static Vector2f stringToVector2(const String & str);
        static Rectf stringToRect(const String & str);
        static const PixelBuffer* stringToImage(const String & str);
        static Colour stringToColour(const String & str);
        static ColourArea stringToColourArea(const String & str);
        static RelPos stringToUDim(const String & str);
        static RelVector2 stringToRelVector2(const String & str);
        static RelPlaneSize stringToRelPlaneSize(const String & str);
        static RelRect stringToRelRect(const String & str);
        static AspectMode stringToAspectMode(const String & str);

        static String floatToString(float val);
        static String uintToString(unsigned int val);
        static String boolToString(bool val);
        static String sizeToString(const PlaneSizef & val);
        static String vector2ToString(const Vector2f & val);
        static String rectToString(const Rectf & val);
        static String imageToString(const PixelBuffer * val);
        static String colourToString(const Colour & val);
        static String colourRectToString(const ColourArea & val);
        static String udimToString(const RelPos & val);
        static String uvector2ToString(const RelVector2 & val);
        static String usizeToString(const RelPlaneSize & val);
        static String urectToString(const RelRect & val);
        static String aspectModeToString(AspectMode val);
    };
}
// the binding file generates alot of
// warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#if defined(_MSC_VER)
#   pragma warning(disable : 4800)
#endif