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

#ifndef _NII_UI_PREINCLUDE_H_
#define _NII_UI_PREINCLUDE_H_

#include "NiiPreInclude.h"

namespace NII
{
    typedef Nid WidgetID;
    typedef Nid WidgetViewlID;
    typedef Nid StyleID;
namespace UI
{
    class Cursor;    
    class BidiText;
    class CenterLayout;
    class DefaultLogger;
    class Widget;
    class Container;
    class WidgetEventArgs;
    class Event;
    class Exception;
    class EffectTextView;
    class NormalTextView;
    class PixelLayout;
    class UISheet;
    class StyleSection;
    class JustifyLayout;
    class LeftLayout;
    class PixelUnitGrid;
    class TextView;
    class PixelUnit;
    class ImagePixelUnit;
    class TextPixelUnit;
    class WidgetPixelUnit;
    struct SheetContext;
    class UIInstance;
    class RightLayout;
    class Scheme;

    class StyleEventArgs;
    class CursorEventArgs;
    struct CursorMultiTouch;
    class StringTranscoder;
    class TextUtils;
    class Style;
    class Contaienr;
    class Window;
    class WidgetFactory;
    class WidgetView;
    class WindowRendererModule;
    class DragDropWindow;
    class TooltipWidgetAide;
    class CursorWidgetAide;
    class StyleWidget;
    class StyleLayout;
    class Layer;
    class Section;
    class StyleSectionImageUnit;
    class StyleArea;
    class SizeDefine;
    class StyleSectionTextUnit;
    class StyleSpecial;
    class StyleSectionFrameUnit;
    class ListenerProperty;
    class Button;
    class Combobox;
    class ComboDropList;
    class DefaultWindow;
    class DragContainer;
    class Editbox;
    class FrameWindow;
    class IndexLayoutContainer;
    class GridLayoutContainer;
    class HorizontalLayoutContainer;
    class ListItem;
    class ListContainer;
    class ItemListbox;
    class LayoutContainer;
    class Listbox;
    class ListboxItem;
    class ListboxTextItem;
    class ListHeader;
    class ListHeaderItem;
    class Menubar;
    class MenuItem;
    class MultiColumnList;
    class MultiEditbox;
    class PopupMenu;
    class ProgressBar;
    class PushButton;
    class RadioButton;
    class ScrollContainer;
    class Scrollbar;
    class SizeContainer;
    class ScrolledListContainer;
    class Slider;
    class NumSpinner;
    class TabButton;
    class TabControl;
    class PosButton;
    class Titlebar;
    class ToggleButton;
    class Tooltip;
    class Tree;
    class TreeItem;
    class VerticalLayoutContainer;
    class ActivationEventArgs;
    class DisplayEventArgs;
    class DragDropEventArgs;
    class ColumnEventArgs;
    class KeyEventArgs;
    class CursorViewArgs;
    class RegexMatchStateArgs;
    class ResourceEventArgs;
    class TreeEventArgs;
    class UpdateEventArgs;

    class KeyboardControlItem;
    class MouseControlItem;
    class JoyPadControlItem;
    class TouchControlItem;

    class ListenerEvent;

    class ButtonView;

    typedef Nid WidgetID;
    
    typedef vector<Widget *>::type WidgetList;
}
}
#endif