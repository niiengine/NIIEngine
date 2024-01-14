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

#ifndef _UILAYOUT_DIALOG_MAIN_H_
#define _UILAYOUT_DIALOG_MAIN_H_

#include "gsgsPreInclude.h"
#include "NiiPreInclude.h"
#include "NiiUIWindow.h"
#include "UILayoutEditorCanvas.h"
#include <wx/treectrl.h>
#include <wx/grid.h>
#include <wx/cmdproc.h>

using namespace NII;
using namespace NII::UI;

namespace gsgs
{
    /** Classes can register to the OverlayEditorDocument through this interface, when they are interested 
    * to be notified in one ore more of the available methods. Note that the methods are not 'pure' since
    * not each class might be interested in every notification. */

    class IDocumentObserver 
    {
    public:
        /** Called when an overlay has been saved.
         * @param aRoot the new root window.*/
        virtual void LayoutOpened (Window* aRoot)   {}

        /** Called when the current overlay has been closed.*/
        virtual void LayoutClosed ()                {}

        /** Called when a new overlay has been started.
         * @param aRoot the new root window.*/
        virtual void LayoutStarted (Window* aRoot)  {}

        /** Called when a window is selected or unselected.
         * @param aWindow selected window.*/
        virtual void WindowSelected (Widget* aWindow) {}

        /** Called when a change takes place. Can be a drag of the selection or the
         * setting of a parameter.*/
        virtual void LayoutModified (Window* aRoot) {}

        /** Called when an new window has been added or when windows have been pasted.
         * @param pWindow the new window.*/
        virtual void WindowAdded (Widget * aWindow)  {}

        /** Called when one or more windows have been deleted.*/
        virtual void WindowsRemoved ()              {}

        /** Called when the current selection is being dragged or resized.*/
        virtual void SelectionDraggedOrResized ()   {}
    } ;

    //////////////////////////////////////////////////////////////////////////
    // WINDOWCONTEXT
    //////////////////////////////////////////////////////////////////////////

    /** This class is given a Window, and then you can do requests on it to determine in which
     * context you should work. At the moment, the class is used to determine whether Windows
     * can be added the set Window.
     */
    class WindowContext 
    {
    public:
        WindowContext (Widget * pWindow);

        /** Sets the (new) window. From now on, "queries" are done on this window.
         *
         * @param pWindow the (new) window to do queries on.*/
        void SetWindow(Widget * pWindow);

        /** "Course grained" way of checking whether the currently set window can be given a child
         * window by just creating a new window and setting the parent / child relation. For example,
         * when the current window is a Listbox, it will fail, because this widget requires you to call 
         * "addListItem" from your code, so that's a special case.
         */
        bool AcceptsWindowAsChild() const;
        /** Used to do a context sensitive delete.*/
        //bool isListItem();
        //bool acceptsListItemAsChild();
        //bool acceptsColumn();
        //bool acceptsRow();
        //bool requiresTabs();
        //void addWindow(Window* pChildWindow);
        //void addColumn(String columnLabel);
        //void addRow(String rowLabel);
    private:
        Widget* m_pWindow;
    };

    /** This dialog lets the user select a Window type to add. The window will be added as a 
    * child of the selected Layout root or Window in the browse tree. (@see DialogMain).
    */
    class DialogAddWindow : public wxDialog
    {
    public:
        /* Constructor.*/
        DialogAddWindow(wxWindow* aParentDialog, const Widget * aParentWindow);

        /** Destructor.
        * @access public 
        * @qualifier none
        * @return  void
        * @remarks No cleanup is needed because all allocated member variables 
        * will be handled by wxWidgets.
        */
        ~DialogAddWindow() {};
        
        /** Returns the chosen new window name.
         * @access public 
         * @qualifier none
         * @return wxString& Chosen window name
         */
        wxString& GetWindowName()           { return m_windowName; }

        /** Returns the chosen new window type.
         * @access public 
         * @qualifier none
         * @return wxString& Chosen window type
         */
        StyleViewID GetWindowType()           { return m_windowType; }
    private:
        /** Set in the OK handler, so the main dialog can see which new name was typed.*/
        wxString m_windowName;

        /** Set in the OK handler, so the main dialog can see which new type was chosen.*/
        StyleViewID m_windowType;

        /** Allows for accepting the new window's name.*/
        wxTextCtrl* m_editName;

        /** Allows for selecting a widget type.*/
        wxTreeCtrl* m_widgetTree;

        /** Indicates whether the tree will display all items (false) or only those that
        * are not dependent on parents (true).*/
        bool m_treeFilter;

        /** Shared member to allow us to ppen the previously used looknfeel. */
        static wxString	m_previousLooknfeel;

        /** Holds the parent window of this dialog.*/
        const Widget* m_parentWindow;

        /** Allows the user to specify the type of desired filter in the widget tree.*/
        wxRadioBox* m_filterBox;

        /** Keep reference so we can disable it on no valid widget selection.*/
        wxButton*   m_okButton;
        
        // Control ID(s) for the window widgets
        enum {
            ID_SET_AS_DEFAULT = wxID_HIGHEST,
            ID_WIDGET_TREE,
            ID_FILTER_BOX,  
        };

        /************************************************************************
         * PRIVATE FUNCTIONS
         ************************************************************************/
        
        /** Returns whether the given window type requires a parent, and is therefor irrelevant to add on its own.*/
        bool RequiresParent(FactoryID windowType) const;

        /** Builds up a tree with all available looks and their widgets.*/
        void InitWidgetTree(const Widget * aParentWindow);

        /** Returns whether the given widget can/may be added to the given parent.*/
        bool Filter(FactoryID aWidgetType, const Widget * aParentWindow) const;

        /** General initialization method. Should be called after the constructor.
         * @access private 
         * @qualifier none
         * @return void
         */
        void Setup();

        void OnOK(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        void OnSelchangedWidgetTree(wxTreeEvent& event);

        /** Processes the event raised when the user changes the filter radio box selection.
        * @access private 
        * @qualifier none
        * @param event Object that holds the event data.
        * @return void
        */
        void OnFilterBoxSel(wxCommandEvent& event);

        DECLARE_EVENT_TABLE();
    };

    /** This command adds a window in the "do" and "redo" mode, and destroys it
    * in the "undo" mode.
    */
    class CommandAddWindow : public wxCommand
    {
    public:
        /** Constructor.
         * @param aDocument the document on which to apply the command.
         * @param aParent the parent window to which the new window must be added.
         * @param aType the type of the new window, e.g. "Button".
         * @param aName the name of the new window, e.g. "HealthBar".
         */
        CommandAddWindow (EditorDocument* aDocument, Window* aParent, StyleViewID aType, const String& aName);
        
        /** "Does" or "Re-does" the command.*/
        bool Do();

        /** "Un-does" the command.*/
        bool Undo();
    protected:
        /** Block default construction not possible.*/
        CommandAddWindow();
    private:
        /** Stored constructor parameter.*/
        EditorDocument * mDocument;

        /** Stored constructor parameter.*/
        Window * mParent;

        /** Stored constructor parameter.*/
        StyleViewID mType;

        /** Stored constructor parameter.*/
        String mName;

        /** Stores the parent's name, so we can check whether it still exists,
        * without running into a null-pointer error when the parent is deleted.*/
        String mParentName;

        /** Stores the created window for Undoing.*/
        Widget * mCreatedWindow; 
    };

    /** The AddGuiElement dialog. This dialog lets the user browse through the availabe
    * Overlays and their elements. Besides that, it provides a way of changing parameters
    * of the selected element(s) in the Edit area.
    */
    class DialogMain : public wxDialog, IDocumentObserver
    {
    public:
        /** Constructor. Clears out all control-mapped member variables.
         * @access public 
         * @qualifier none
         * @param parent the dialog's parent window.
         * @return nothing
         */
        DialogMain(wxWindow* parent);

        /** Destructor.
         * @access public 
         * @qualifier none
         * @return nothing
         */
        ~DialogMain();
        
        /** Sets the Document to communicate with. This dialog needs the Document to 
         * performs task like getting / setting Window parameters and such. GUI 
         * could have been invoked directly from here, but using the Document makes 
         * the system more centralized.
         * @access public 
         * @qualifier none
         * @param document pointer to the wxDocument, which communicates with the GUI system.
         * @return void
         */
        void SetDocument (EditorDocument* document);

        /** Implements the IDocumentObserver.*/
        void LayoutOpened (Window* aRoot);

        /** Implements the IDocumentObserver.*/
        void LayoutClosed () ;

        /** Implements the IDocumentObserver.*/
        void LayoutStarted (Window* aRoot);

        /** Implements the IDocumentObserver.*/
        void WindowSelected (Widget * aWindow) ;

        /** Implements the IDocumentObserver.*/
        void LayoutModified (Window* aRoot) ;

        /** Implements the IDocumentObserver.*/
        void WindowAdded (Widget * aWindow = NULL) ;

        /** Implements the IDocumentObserver.*/
        void WindowsRemoved ();

        /** Implements the IDocumentObserver.*/
        virtual void SelectionDraggedOrResized ();

        /** Its need for the color editor in the EditorDocument.cpp */
        Widget *GetCurrentWindow()              {return mCurrent;};

        /** Holds the widget name and look, as well as the widget itself. */
        typedef struct LookAndWidgetStruct
        {
            Widget * mWindow;
            StyleViewID mStyleView;
            StyleID mStyle;
            FactoryID mModel;
            WidgetViewlID mView;
        }LookAndWidget;

        /** Returns the widget looks registered in the system.
         * @access public 
         * @qualifier none
         * @return wxArrayString Looks registered in the system
         */
        wxArrayString GetAvailableLooks() { return m_availableLooks; }

        /** Modifies the Look'N'Feel of ALL the windows in the layout.
         * @access public 
         * @qualifier none
         * @param newLook New Look'N'Feel to apply
         * @return void
         * @remarks Widgets that do not support the new Look'N'Feel are ignored.
         */
        void SetWidgetsLook(StyleID newLook);

        /** Modifies the Look'N'Feel of the windows in the current selection.
         * @access public 
         * @qualifier none
         * @param currentSelection Currently selected widgets
         * @param newLook New Look'N'Feel to apply
         * @return void
         * @remarks Widgets that do not support the new Look'N'Feel are ignored.
         */
        void SetWidgetsLook(Selection* currentSelection, StyleID newLook);

        /** Modifies the Look'N'Feel of the supplied widget.
         * @access public 
         * @qualifier none
         * @param currentWindow Widget whose Look'N'Feel we're going to change
         * @param newLook New Look'N'Feel to apply
         * @param refresh Indicates if we need to refresh the application's main frame or not
         * @return void
         */
        void SetWidgetLook(Widget *currentWindow, StyleID newLook, bool refresh);

        /** Scans all available Falagard mappings and for each widget saves the looks it can mapped with.
         * @access public 
         * @qualifier none
         * @param clearBeforeHand Indicates if the current widget/look hash mapping is to be cleared before the operation or not
         * @return void
         * @remarks Duplicate pairs (widget/look) are not allowed and will be ignored
         */
        void StoreWidgetLooks(bool clearBeforeHand = false);
    private:
        /** Control ID's who generate interesting events.*/
        enum 
        {
            ID_LAYOUT_TREE = wxID_HIGHEST,
            ID_TEXT,
            ID_GRID,
            ID_ADD_WINDOW,
            ID_LOAD_LAYOUT,
            ID_SAVE_SELECTION,
            ID_SKINS,
            ID_SKINS_DISPLAY
        };

        /** Grid column identifiers.*/
        enum Columns
        {
            COLUMN_NAME   = 0,
            COLUMN_VALUE  = 1
        };

        /** Property types.*/
        enum PropertyType
        {
            FLOAT_TYPE,
            BOOL_TYPE,
            FONT_TYPE,
            TEXT_TYPE,
            VERT_ALIGN_TYPE,
            HORZ_ALIGN_TYPE,
            VERT_TEXT_FORMAT_TYPE,
            HORZ_TEXT_FORMAT_TYPE,
            TEXT_COLOUR,
            UNSUPPORTED
        };

        /** Reference to the application's Document.*/
        EditorDocument * m_document ;

        /** Some handy references.*/
        Window * m_currentRoot;
        Widget * mCurrent;

        /** Represents the current layout, and allows for re-arranging and re-naming.*/
        wxTreeCtrl * m_tree;

        /** The currently BOLD item.*/
        //wxTreeItemId		m_boldItem;
        Widget * m_boldItemWindow;

        /** Button which shows the AddWindow dialog.*/
        wxButton * m_addWindow;
        wxButton * m_saveSelection;

        /** Provides quick access to the common "Text" property.*/
        wxTextCtrl * m_editText;

        /** This maps each existing registered widget (hash key) to its available looks (e.g. Vanilla, TaharezLook, etc.) */
        typedef std::multimap<WidgetViewlID, StyleID> WidgetLookHash;
        WidgetLookHash	m_widgetLooks;

        /** Holds all the looks registered in the system */
        wxArrayString m_availableLooks;

        /** Displays the Type of the currently selected control */
        wxStaticText * m_typeText;

        /** Displays the current skin of the selected widget and the available ones for the user to switch */
        wxComboBox * m_skinList;

        /** Radio box that allows the user to switch between the available display modes (common or all)*/
        wxRadioBox * m_displayModeRadio;

        /** Indicates when in multi-selection mode if we are to only show skins and properties that 
         * ALL elements in the selection share (true) OR if we are to show skins and properties 
         * that at least one element in the selection supports (false). In the second case, if the 
         * user chooses a skin or modifies a property that is not applicable for all elements, 
         * only apply to those that can and ignore others. */
        bool m_displayOnlyCommon;

        /** Represents the current layout, and allows for re-arranging and re-naming.*/
        wxGrid * m_grid;

        /** We need the sizer to calculate the new column sizes when the user resizes the window.
         * Attaching the sizer with m_grid->setSizer() just didn't draw anything anymore...*/
        wxStaticBoxSizer* m_propertySizer;

        wxString * m_fonts;
        size_t m_fontCount;

        /** Block 'changed' event on certain occasions.*/
        bool m_updating;

        typedef std::map<PropertyID, PropertyType>	SupportedPropertiesType;

        /** Maps supported properties and their type.*/
        SupportedPropertiesType m_supportedProperties;

        typedef std::map<Widget*, wxTreeItemId> TreeItemMappingType;

        /** Stores windows and their tree item id for easy lookups.*/
        TreeItemMappingType m_treeItemMapping;

        /** List that stores widget looks */
        typedef std::list<StyleID> WidgetLooks;

        /** We save the last-selected row, so we can save the property when the user selects a different widget.*/
        Widget * m_lastSelectedWindow;

        int m_lastSelectedRow;

        /** Builds up a map with supported GUI properties and their type. The type is used,
         * to determine which wxGridCellEditor subclass we must use for each property's edit cell.
         * @access private 
         * @qualifier none
         * @return void
         */
        void CreateSupportedPropertyMap();

        /** Empties the grid, except for the headers columns.
         * @access private 
         * @qualifier none
         * @return void
         */
        void ClearGrid();

        /** Create our grid object internals.
         * @access private 
         * @qualifier none
         * @return void
         */
        void CreateGrid();

        /** Repopulates the grid, thus updating all property values (both "fake" and "real").
         * @access private 
         * @qualifier none
         * @param mapSkins Maps the possible skins for the current user selection (true) or not (false)
         * @return void
         */
        void UpdateProperties(bool mapSkins = true);

        /** Adds the "fake" properties, like the name and UDIM values for the Left, Top, Right
         * and Bottom window area.
         * @access private 
         * @qualifier none
         * @return void
         */
        void AddFakeProperties();

        /** Adds the remaining "real" and supported GUI properties for the current selection.
         * @access private 
         * @qualifier none
         * @param mapSkins Maps the possible skins for the current user selection (true) or not (false)
         * @return void
         */
        void AddRealProperties(const bool mapSkins = true);

        /** Returns the value of the given "fake" property in the supplied window.
         * @access private 
         * @qualifier none
         * @param propertyName Name of the "fake" property to check
         * @param currentWindow Window whose "fake" property value we're interested in
         * @return wxString Value of the supplied "fake" property
         */
        wxString GetFakePropertyValue(PropertyID propertyName, const Widget * current) const;

        /** Given the supplied "fake" property, verifies in ALL currently selected widgets if its
         * value is the same (common). If it is, the shared value is returned. Otherwise, the function
         * returns an empty value - wxT("") - to inform the user the property has different
         * values in the selection.
         * @access private 
         * @qualifier none
         * @param propertyName Property to process (e.g. "Alpha")
         * @return wxString Fake property value or empty string
         * @remarks "Fake" properties, unlike real ones, are mandatory elements and every widget
         * must have them!
         */
        wxString GetFakePropertyCommonValue(PropertyID propertyName) const;

        /** Verifies if the supplied property contains its default value in ALL elements
         * in the current selection that contain the property. Elements that do not possess
         * the property are ignored.
         * @access private 
         * @qualifier none
         * @param propertyName Property to check (e.g. "Alpha")
         * @return bool True if property is at default value in all elements that contain it, false otherwise
         */
        bool IsPropertyDefaultInSelection(PropertyID propertyName);

        /** The property value font is set in bold if the property is not at its default value
         * in ALL elements in the current selection (@see IsPropertyDefaultInSelection) or as normal
         * text otherwise.
         * @access private 
         * @qualifier none
         * @param propertyRow Grid row to affect
         * @param propertyName Property to check (e.g. "Alpha")
         * @param forceRefresh Forces the visual update of the grid (true) or not (false)
         * @return void
         */
        void SetPropertyValueFont(int propertyRow, PropertyID propertyName, bool forceRefresh=false);

        /** Updates a "fake" property, when a corresponding grid cell has changed.
         * It uses the first 8 cells to build up a new window area rectangle.
         * @access private 
         * @qualifier none
         * @return void
         */
        void ApplyFakePropertyChange();

        /** Add a row to the grid for the given property, when it is supported. It will search
         * for the corresponding so-called "cellEditor". Color management is supported.
         * @access private 
         * @qualifier none
         * @param propertyName Property name (e.g. "Alpha")
         * @param propertyValue Property value (e.g. "0.7")
         * @param textColor Text color (only for value column)
         * @param backgroundColor Background color (for both name and value columns)
         * @param lockedBackgroundColor Background color for readonly properties (only for value column)
         * @return void
         */
        void AddPropertyRow(PropertyID propertyName, const String& propertyValue, const wxColour& textColor=*wxBLACK, const wxColour& backgroundColor=*wxWHITE, const wxColour& lockedBackgroundColor=*wxLIGHT_GREY);

        /** Adds a row with the given editor, name and value. Color management is supported.
         * @access private 
         * @qualifier none
         * @param editor One of the specific wxWidgets cell editors, such as "float", "choice" and such.
         * @param propertyName Property name (e.g. "Alpha")
         * @param propertyValue Property value (e.g. "0.7")
         * @param valueReadOnly Whether the value cell is editable (false) or not (true).
         * @param textColor Text color (only for value column)
         * @param backgroundColor Background color (for both name and value columns)
         * @param lockedBackgroundColor Background color for readonly properties (only for value column)
         * @return void
         */
        void AddPropertyRow(wxGridCellEditor* editor, PropertyID propertyName, const wxString& propertyValue, bool valueReadOnly = true, const wxColour& textColor=*wxBLACK, const wxColour& backgroundColor=*wxWHITE, const wxColour& lockedBackgroundColor=*wxLIGHT_GREY);

        /** Returns the property type for the given GUI property, or UNSUPPORTED if it can't find the
         * property.
         * @access private 
         * @qualifier none
         * @param propertyName Property name (e.g. "Alpha")
         * @return DialogMain::PropertyType Property type or UNSUPPORTED (if not found)
         */
        PropertyType GetPropertyType(PropertyID pid);

        /** Returns an appropriate wxWidgets cell editor for the given property. Or NULL when the 
         * property is unsupported.
         * @access private 
         * @qualifier none
         * @param propertyName Property name (e.g. "Alpha")
         * @return wxGridCellEditor* Specific wxWidgets cell editor for the property, such as "float", 
           "choice" and such. 
         */
        wxGridCellEditor* GetPropertyEditor(PropertyID propertyName);

        /** Update the widths of our grid columns
         * @access private 
         * @qualifier none
         * @return void
         */
        void UpdateColumnWidths();

        /** Cleans up, ready for a new Document.
         * @access private 
         * @qualifier none
         * @return void
         */
        void Reset() ;

        /** Empties the tree and the mapping to GUI windows.
         * @access private 
         * @qualifier none
         * @return void
         */
        void ClearTree();

        /** Initializes the Tree Control for the given overlay.*/
        void InitLayoutTree (Widget* window, const wxTreeItemId& parent, const bool addThis = true) ;

        /** Caches window/tree item pairs for easy cross-lookup.*/
        void AddTreeItemMapping (Widget* window, const wxTreeItemId& treeItem);

        /** Rebuilds the browse-tree. Way easier than keep valid after additions/changes/deletions.
         * @access private 
         * @qualifier none
         * @return void
         */
        void SyncTree();

        /** When an item is programmaticaly selected, it doesn't get blue. This little trick
         * highlights it bold.
         * @access private 
         * @qualifier none
         * @return void
         */
        void HighlightCurrentWindow();

        /** Collects available Fonts.
         * @access private 
         * @qualifier none
         * @return void
         */
        void FindAvailableFonts();

        /** Finds the current GUI Window for the given browse-tree item. Returns the found one.*/
        Window* FindWindow (const wxTreeItemId& treeItem);

        /** Verifies if the given look can be applied to the supplied widget.
         * @access private 
         * @qualifier none
         * @param widget Widget kind (e.g. Button) 
         * @param look Look name (e.g. TaharezLook)
         * @return bool TRUE if the look can be applied to the widget, FALSE otherwise
         */
        bool WidgetLookExists(WidgetViewlID widget, StyleID look);

        /** Returns all the available looks for a given widget.
         * @access private 
         * @qualifier none
         * @param widget Widget kind (e.g. Button)
         * @return DialogMain::WidgetLooks Possible looks for the given widget
         */    
        WidgetLooks GetWidgetLooks(WidgetViewlID widget);

        /** Retrieves the look (e.g. TaharezLook) and the widget kind (e.g. Button).
         * @access private 
         * @qualifier none
         * @param window Widget to scan
         * @return DialogMain::LookAndWidget Info containing the widget's look and kind
         */
        LookAndWidget GetLookAndWidgetFromType(Widget *window) const;

        /** Given the current user widget selection, populates the skin dropdown with 
         * the possible skins that can be mapped on the elements of the selection.
         * @access private 
         * @qualifier none
         * @return void
         */
        void MapWidgetSelectionPossibleSkins();

        /** Given the current user widget selection, update the text, type and skin fields
         * to reflect the selection data.
         * @access private 
         * @qualifier none
         * @return void
         */
        void UpdateSelectionInfo();

        /** General initialization method. Should be called after the constructor.
         * @access private 
         * @qualifier none
         * @return void
         */
        void Setup();
        
        /** Class type function object (functor) that verifies if the supplied widget look is already
        * stored in the widget looks list.
        * @remarks This class could be made more generic through the use
        * of templates, but since it serves a very specific goal it wasn't
        * deemed necessary.
        */
        class NotIn : public std::unary_function<StyleID, bool>
        {
        public: 
            NotIn(const WidgetLooks & looks) : m_looks(looks) {} 
            // Function call operator overload which verifies if the supplied widget look
            // is present in the stored widget list
            bool operator() (StyleID sid)
            {
                std::list<StyleID>::iterator it = m_looks.begin();
                while (it != m_looks.end() && *it != sid) ++it;
                return (it != m_looks.end() ? false : true);
            }
        private:
            WidgetLooks m_looks;
        };

        /************************************************************************
         * EVENTS
         ************************************************************************/
        
        /** Lots of interesting event here :-) 
         * @remarks This set the class access level as protected
         */
        DECLARE_EVENT_TABLE();

        /** The dialog gets resized. We manually update the grid size then.*/
        void OnResize(wxSizeEvent& event);
        //void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL);

        void OnAddWindow(wxCommandEvent& event);
        void OnSaveSelection(wxCommandEvent& event);
        void OnChangeText(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        void OnOK(wxCommandEvent& event);
        void OnSelchangedLayoutTree(wxTreeEvent& event);
        
        /** Tells us that the contents of a cell have changed.*/
        void OnGridCellChange(wxGridEvent& event);

        /** Saves the property at the row which is edited. */
        void SaveProperty(int row);

        /** Called when the user changes the size of the columns by dragging the middle line.
         * @access protected 
         * @qualifier none
         * @param event Object that holds the event data.
         * @return void
         */
        void OnGridColSize(wxGridSizeEvent& event);

        /** Processes the event raised when the user chooses a new skin.
         * @access protected 
         * @qualifier none
         * @param event Object that holds the event data.
         * @return void
         */
        void OnChangeSkin(wxCommandEvent& event);	

        /** Processes the event raised when the user chooses a multi-selection skin display type.
         * @access protected 
         * @qualifier none
         * @param event Object that holds the event data.
         * @return void
         */
        void OnDisplayModeSelection(wxCommandEvent& event);    
    };
}
#endif // _DIALOG_MAIN_H_
