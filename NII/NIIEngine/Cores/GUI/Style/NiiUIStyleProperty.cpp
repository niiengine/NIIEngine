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
#include "NiiUIStyleProperty.h"
#include "NiiScriptProperty.h"

namespace NII
{
namespace UI
{
    N_PROPERTY(ScriptProperty, StyleProperty, StyleUnitProperty,             0); // Style tag
    N_PROPERTY(ScriptProperty, StyleProperty, WidgetStyleUnitProperty,       1); // WidgetStyle tag
    N_PROPERTY(ScriptProperty, StyleProperty, StyleWidgetUnitProperty,       2); // WidgetCom tag
    N_PROPERTY(ScriptProperty, StyleProperty, StyleSectionUnitProperty,      3); // StyleSection tag
    N_PROPERTY(ScriptProperty, StyleProperty, StyleLayoutUnitProperty,       4); // StyleLayout tag
    N_PROPERTY(ScriptProperty, StyleProperty, LayerUnitProperty,             5); // Layer tag
    N_PROPERTY(ScriptProperty, StyleProperty, SectionUnitProperty,           6); // Section tag
    N_PROPERTY(ScriptProperty, StyleProperty, SectionImageUnitProperty,      7); // StyleSectionImageUnit tag
    N_PROPERTY(ScriptProperty, StyleProperty, SectionTextUnitProperty,       8); // StyleSectionTextUnit tag
    N_PROPERTY(ScriptProperty, StyleProperty, SectionFrameUnitProperty,      9); // StyleSectionFrameUnit tag
    N_PROPERTY(ScriptProperty, StyleProperty, ComAreaUnitProperty,           10);// StyleArea tag
    N_PROPERTY(ScriptProperty, StyleProperty, ComPixelUnitProperty,          11);// ComPixel tag
    N_PROPERTY(ScriptProperty, StyleProperty, ColourUnitProperty,            12);// Colour tag
    N_PROPERTY(ScriptProperty, StyleProperty, ColoursUnitProperty,           13);// Colours tag
    N_PROPERTY(ScriptProperty, StyleProperty, ColoursIDUnitProperty,         14);// ColoursProperty tag
    N_PROPERTY(ScriptProperty, StyleProperty, VLayoutUnitProperty,           15);// VLayout tag
    N_PROPERTY(ScriptProperty, StyleProperty, HLayoutUnitProperty,           16);// HLayout tag
    N_PROPERTY(ScriptProperty, StyleProperty, VLayoutIDUnitProperty,         17);// VLayoutProperty tag
    N_PROPERTY(ScriptProperty, StyleProperty, HLayoutIDUnitProperty,         18);// HLayoutProperty tag
    N_PROPERTY(ScriptProperty, StyleProperty, VAlignUnitProperty,            19);// VAlign tag
    N_PROPERTY(ScriptProperty, StyleProperty, HAlignUnitProperty,            20);// HAlign tag
    N_PROPERTY(ScriptProperty, StyleProperty, PropertyUnitProperty,          21);// Property tag
    N_PROPERTY(ScriptProperty, StyleProperty, SizeDefineProperty,            22);// SizeDefine tag
    N_PROPERTY(ScriptProperty, StyleProperty, SizeIDUnitProperty,            23);// SizeProperty tag
    N_PROPERTY(ScriptProperty, StyleProperty, RelSizeUnitProperty,           24);// RelSize tag
    N_PROPERTY(ScriptProperty, StyleProperty, AbsSizeUnitProperty,           25);// AsbSize tag
    N_PROPERTY(ScriptProperty, StyleProperty, PixelSizeUnitProperty,         26);// PixelSize tag
    N_PROPERTY(ScriptProperty, StyleProperty, PixelSizeIDUnitProperty,       27);// PixelSizeProperty tag
    N_PROPERTY(ScriptProperty, StyleProperty, WidgetSizeUnitProperty,        28);// WidgetSize tag
    N_PROPERTY(ScriptProperty, StyleProperty, FontSizeUnitProperty,          29);// FontSize tag
    N_PROPERTY(ScriptProperty, StyleProperty, TextUnitProperty,              30);// Text tag
    N_PROPERTY(ScriptProperty, StyleProperty, TextIDUnitProperty,            31);// TextProperty tag
    N_PROPERTY(ScriptProperty, StyleProperty, StyleSpecialUnitProperty,      32);// StyleSpecial tag
    N_PROPERTY(ScriptProperty, StyleProperty, StylePropertyUnitProperty,     33);// StyleProperty tag
    N_PROPERTY(ScriptProperty, StyleProperty, ListenerPropertyUnitProperty,  34);// ListenerProperty tag
    N_PROPERTY(ScriptProperty, StyleProperty, ListenerTargetUnitProperty,    35);// ListenerTarget tag
    N_PROPERTY(ScriptProperty, StyleProperty, ListenerEventUnitProperty,     36);// ListenerEvent tag
    N_PROPERTY(ScriptProperty, StyleProperty, EventTargetUnitProperty,       37);// EventTarget tag
    N_PROPERTY(ScriptProperty, StyleProperty, SizeOpUnitProperty,            38);// SizeOp tag
    N_PROPERTY(ScriptProperty, StyleProperty, ComAreaIDUnitProperty,         39);// ComAreaPropery tag
    N_PROPERTY(ScriptProperty, StyleProperty, PixelIDUnitProperty,           40);// N_PropertyPixelBuffer_Pixel tag
    N_PROPERTY(ScriptProperty, StyleProperty, FontIDUnitProperty,            41);// FontProperty tag
    N_PROPERTY(ScriptProperty, StyleProperty, StyleSpecialIDUnitProperty,    42);// StyleSpecialProperty tag
    //------------------------------------------------------------------------
    N_PROPERTY(ScriptProperty, StyleProperty, TopLeftAttribute,              43);// top_left attrib
    N_PROPERTY(ScriptProperty, StyleProperty, TopRightAttribute,             44);// top_right attrib
    N_PROPERTY(ScriptProperty, StyleProperty, BottomLeftAttribute,           45);// bottom_left attrib
    N_PROPERTY(ScriptProperty, StyleProperty, BottomRightAttribute,          46);// bottom_right attrib
    N_PROPERTY(ScriptProperty, StyleProperty, PriorityAttribute,             47);// priority attrib
    N_PROPERTY(ScriptProperty, StyleProperty, WidgetStyleAttribute,          48);// widget_style attrib
    N_PROPERTY(ScriptProperty, StyleProperty, ScaleAttribute,                49);// scale attrib
    N_PROPERTY(ScriptProperty, StyleProperty, OffsetAttribute,               50);// offset attrib
    N_PROPERTY(ScriptProperty, StyleProperty, WidgetAttribute,               51);// widget attrib
    N_PROPERTY(ScriptProperty, StyleProperty, StringAttribute,               52);// string attrib
    N_PROPERTY(ScriptProperty, StyleProperty, FontAttribute,                 53);// font attrib
    N_PROPERTY(ScriptProperty, StyleProperty, ClippedAttribute,              54);// clipped attrib
    N_PROPERTY(ScriptProperty, StyleProperty, PaddingAttribute,              55);// padding attrib
    N_PROPERTY(ScriptProperty, StyleProperty, LayoutOnWriteAttribute,        56);// signal_layout attrib
    N_PROPERTY(ScriptProperty, StyleProperty, RedrawOnWriteAttribute,        57);// signal_refresh attrib
    N_PROPERTY(ScriptProperty, StyleProperty, TargetPropertyAttribute,       58);// target attrib
    N_PROPERTY(ScriptProperty, StyleProperty, ControlPropertyAttribute,      59);// ctl_property attrib
    N_PROPERTY(ScriptProperty, StyleProperty, PropertyAttribute,             60);// property attrib
    N_PROPERTY(ScriptProperty, StyleProperty, ControlValueAttribute,         61);// ctl_value attrib
    N_PROPERTY(ScriptProperty, StyleProperty, ControlWidgetAttribute,        62);// ctl_widget attrib
    N_PROPERTY(ScriptProperty, StyleProperty, HelpStringAttribute,           63);// help attrib
    N_PROPERTY(ScriptProperty, StyleProperty, EventAttribute,                64);// event attrib
    N_PROPERTY(ScriptProperty, StyleProperty, InheritsAttribute,             65);// inherits attrib
    N_PROPERTY(ScriptProperty, StyleProperty, AutoWindowAttribute,           66);// auto attrib
    N_PROPERTY(ScriptProperty, StyleProperty, FireEventAttribute,            67);// fire_event
    N_PROPERTY(ScriptProperty, StyleProperty, PixelIDProperty,               68);// pixel_id attrib
    N_PROPERTY(ScriptProperty, StyleProperty, PropertyCount,                 69);
}
}