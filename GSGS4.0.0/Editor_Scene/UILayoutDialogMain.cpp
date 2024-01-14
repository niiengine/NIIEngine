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


#include "UILayoutConfig.h"
#include "UILayoutDialogMain.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIWindow.h"
#include "NiiUIManager.h"
#include "NiiFontManager.h"
#include "NiiEngine.h"
#include "ExceptionManager.h"
#include <wx/sizer.h>
#include "NiiUIStrConv.h"
#include "StrHelper.h"
#include "app.h"
#include "UILayoutEditorFrame.h"

#define BORDER                  5

//////////////////////////////////////////////////////////////////////////
// TYPEDEFS
//////////////////////////////////////////////////////////////////////////

#if wxCHECK_VERSION(2,8,0)
    typedef wxTreeItemIdValue   cookieType;
#else
    typedef long cookieType;
#endif

namespace gsgs
{
    //////////////////////////////////////////////////////////////////////////
    // WINDOWCONTEXT FUNCTIONS
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    WindowContext::WindowContext (Widget * pWindow) :
        m_pWindow(pWindow)
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
    }
    //------------------------------------------------------------------------
    void WindowContext::SetWindow (Widget* pWindow)
    {
        m_pWindow = pWindow;
    }
    //------------------------------------------------------------------------
    bool WindowContext::AcceptsWindowAsChild() const
    {
        // Validations
        wxASSERT_MSG(m_pWindow != NULL, wxT("Window member is NULL"));

        const FactoryID strWindowType = m_pWindow->getType();

        // These require different parent / child handling.
        // The current type must not be equal to the checks below
        // Because of the "find" instead of exact matches, it works for different
        // looknfeels, e.g. both "TaharezLook/Combobox" and "Windowslook/Combobox".
        return strWindowType != N_WIDGET_Combobox &&
            strWindowType != N_WIDGET_ComboDropList &&
            strWindowType != N_WIDGET_ListHeader &&
            strWindowType != N_WIDGET_Listbox &&
            strWindowType == N_WIDGET_MultiColumnList;
    }
    //------------------------------------------------------------------------
    //////////////////////////////////////////////////////////////////////////
    // DIALOGADDWINDOW FUNCTIONS
    //////////////////////////////////////////////////////////////////////////

    //------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(DialogAddWindow, wxDialog)
        // Button-click handler(s)
        EVT_TREE_SEL_CHANGED  (ID_WIDGET_TREE, DialogAddWindow::OnSelchangedWidgetTree)
        EVT_BUTTON            (wxID_OK, DialogAddWindow::OnOK)
        EVT_BUTTON            (wxID_CANCEL, DialogAddWindow::OnCancel)
        EVT_RADIOBOX		  (ID_FILTER_BOX, DialogAddWindow::OnFilterBoxSel) 
    END_EVENT_TABLE()

    //------------------------------------------------------------------------
    wxString DialogAddWindow::m_previousLooknfeel;
    //////////////////////////////////////////////////////////////////////////
    // DIALOGADDWINDOW FUNCTIONS
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    DialogAddWindow::DialogAddWindow(wxWindow* aParentDialog, const Widget* widget) :
        m_windowName(),
        m_windowType(0),
        // Put the elements to NULL, so we can block events during initialization
        m_editName(wx_static_cast(wxTextCtrl*, NULL)),
        m_widgetTree(wx_static_cast(wxTreeCtrl*, NULL)),

        // Initially we assume a filtered display (only those that are not dependent on parents) 
        // of available widgets
        m_treeFilter(true), 

        // Store parent window for later use (may be NULL!)
        m_parentWindow(widget),

        // Put the elements to NULL, so we can block events during initialization
        m_filterBox(wx_static_cast(wxRadioBox*, NULL)),
        m_okButton(wx_static_cast(wxButton*, NULL)),
        wxDialog(aParentDialog, wxID_ANY, wxT("Add Window"), wxDefaultPosition, wxSize(350, 700))
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

        // Initialize dialog
        Setup();
    }

    //-----------------------------------------------------------------------
    void DialogAddWindow::Setup()
    {
        // All "groups". Put the parent's name + type in the text field, when a parent is set
        wxStaticBox* const parentGroup = new wxStaticBox(this, wxID_ANY, 
            m_parentWindow ? StrHelper::ToWXString(m_parentWindow->getName() + _T("(") + m_parentWindow->getType() +  _T("):"))
            : wxT("No parent window"));

        // All static fields
        wxStaticText* const staticName = gsgs_Style().createStaticText(this, wxID_ANY, wxT("The name of the new window:"));
        wxStaticBox* const staticWidgets = new wxStaticBox(this, wxID_ANY, wxT("Available widgets for current parent:"));

        // All edit fields. Default the name to "parentName/"
        m_editName = new wxTextCtrl(this, wxID_ANY, m_parentWindow ? 
            StrHelper::ToWXString(m_parentWindow->getName() + _T("/")) : wxT(""));

        // All widgets
        m_widgetTree = new wxTreeCtrl(this, ID_WIDGET_TREE);

        // Size the components vertically
        wxBoxSizer* const rootSizer = new wxBoxSizer(wxVERTICAL);

        // Sizes the tree - this one may be expanded
        // NOTE: No need to free the custodial pointer parentGroup since
        // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
        wxStaticBoxSizer* const nameSizer = new wxStaticBoxSizer(parentGroup, wxVERTICAL);

        // NOTE: No need to free the custodial pointer staticName since
        // it will be handled internally by wxWidgets wxSizer::Add()
        nameSizer->Add(staticName, 0, wxEXPAND | wxALL, BORDER);
        nameSizer->Add(m_editName, 0, wxEXPAND | wxALL, BORDER);

        // Add to the root sizer (may not stretch)
        // NOTE: No need to free the custodial pointer nameSizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        rootSizer->Add(nameSizer, 0, wxEXPAND | wxALL, BORDER);

        //------------------------------ FILTER BOX -------------------------------

        // Create filter box that will allow the user to switch between displaying all existing widget
        // types or only those that don't require a parent for effective use
        const wxString options[] = {wxT("Show all widgets"), wxT("Show allowed widgets")};
        m_filterBox = new wxRadioBox(this, ID_FILTER_BOX, wxT("Widget filter:"), wxDefaultPosition, wxDefaultSize, 2, options);

        // Use entire sizer
        wxBoxSizer* const filterSizer = new wxBoxSizer(wxVERTICAL);
        filterSizer->Add(m_filterBox, 0, wxEXPAND | wxALL, BORDER);

        // Add to the root sizer (may stretch)
        // NOTE: No need to free the custodial pointer filterSizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        rootSizer->Add(filterSizer, 0, wxEXPAND | wxCENTER, BORDER);

        // Sizes the tree - this one may be expanded
        // NOTE: No need to free the custodial pointer staticWidgets since
        // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
        wxStaticBoxSizer* const treeSizer = new wxStaticBoxSizer(staticWidgets, wxVERTICAL);

        // Add filter warning to tree sizer
        // NOTE: No need to free the custodial pointer filterWarning since
        // it will be handled internally by wxWidgets wxSizer::Add()
        wxStaticText* const filterWarning = gsgs_Style().createStaticText(this, wxID_ANY, wxT("Widgets that require a parent are blue colored."));
        treeSizer->Add(filterWarning, 0, wxEXPAND | wxALL, BORDER);
        treeSizer->Add(m_widgetTree, 1, wxEXPAND | wxALL, BORDER);

        // Add to the root sizer (may not stretch)
        // NOTE: No need to free the custodial pointer treeSizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        rootSizer->Add(treeSizer, 1, wxEXPAND | wxALL, BORDER);

        wxBoxSizer* const buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        m_okButton = new wxButton(this, wxID_OK, wxT("Ok"));
        // Disable until a widget gets selected.
        m_okButton->Enable(false);
        buttonSizer->Add(m_okButton, 0, wxEXPAND | wxALL, BORDER);
        buttonSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxEXPAND | wxALL, BORDER);

        // Add to the root sizer (may stretch)
        // NOTE: No need to free the custodial pointer buttonSizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        rootSizer->Add(buttonSizer, 0, wxEXPAND | wxCENTER, BORDER);

        // Initially we assume a filtered display (only those that are not dependent on parents) 
        // of available widgets
        m_filterBox->SetSelection(m_treeFilter);

        // Apply root sizer
        // NOTE: No need to free the custodial pointer rootSizer since
        // it will be handled internally by wxWidgets wxWindowBase::SetSizer()
        SetSizer(rootSizer);

        // Build up the widget list to choose from
        InitWidgetTree(m_parentWindow);

        // NB: We should not call 'Show' on a wxDialog we later call ShowModal for.
        //(void)wxDialog::Show(true);

        // Somehow the initial name value gets selected by default, which is annoying cause if you start typing, you overwrite the selection.
        // Therefore we pull this trick, required after Show.
        m_editName->SetFocus();
        m_editName->SetInsertionPointEnd();
    }

    //------------------------------------------------------------------------
    void DialogAddWindow::InitWidgetTree(const Widget * Pwidget)
    {
        wxASSERT_MSG(m_widgetTree != NULL, wxT("Widget tree invalid or NULL"));

        std::map<GroupID, wxTreeItemId> looks;

        // If the tree is already populated (probably by a previous 
        // call to this function), clear it so that we avoid duplicate data and
        // errors because of duplicate root nodes (only one per tree allowed)
        if(!m_widgetTree->IsEmpty())
        {
            m_widgetTree->DeleteAllItems();
        }

        // Build up the widget tree. (Not recursive.)

        // Tree-root; not a cegui thing.
        const wxTreeItemId rootNode = m_widgetTree->AddRoot(wxT("Widgets"));

        // Set root node as bold
        m_widgetTree->SetItemBold(rootNode, true);

        // Independent of looknfeel, therefore only added once.
        if (Filter (N_WIDGET_DefaultWindow, Pwidget))
        {
            (void)m_widgetTree->AppendItem(rootNode, wxT("DefaultWindow"));
        }
        if (Filter (N_WIDGET_DragContainer, Pwidget))
        {
            (void)m_widgetTree->AppendItem(rootNode, wxT("DragContainer"));
        }

        wxTreeItemId looknfeelNode;

        // The looks will be nodes; its widgets become leaves. Test each widget for passing the filter.
        const WidgetManager::StyleViewList & falIt = N_Only(Widget).getStyleViewList();
        WidgetManager::StyleViewList::const_iterator i, iend = falIt.end();
        for(i = falIt.begin(); i != iend ; ++i)
        {
            FactoryID baseType = i->second.mModel;
            GroupID look = i->second.mGroup;
            StyleViewID svid = i->second.mID;

            if (Filter(baseType, Pwidget))
            {
                if (looks.find(look) == looks.end())
                {	// Branch not available yet; add it.
                    wxString label = StrHelper::ToWXString(N_conv(look));
                    const wxTreeItemId node = m_widgetTree->AppendItem(rootNode, label);

                    // Skin nodes are bold, as opposed to widget type (leaf) nodes that are normal
                    m_widgetTree->SetItemBold(node, true);

                    // Expand when equal to previous
                    if (label == m_previousLooknfeel)
                    {
                        looknfeelNode = node;
                    }

                    looks[look] = node;
                }

                // Widgets that require a parent appear as blue items in the widget tree
                if(RequiresParent(baseType))
                {
                    m_widgetTree->SetItemTextColour(m_widgetTree->AppendItem(looks[look], StrHelper::ToWXString(N_conv(svid))), wxColour(0,0,255));
                }
                else
                {
                    m_widgetTree->AppendItem(looks[look], StrHelper::ToWXString(N_conv(svid)));
                }
            }
        }
        std::map<GroupID, wxTreeItemId>::iterator lookIter;
        // Alphabetically sort the children of each look
        for(lookIter = looks.begin(); lookIter != looks.end(); ++lookIter)
        {
            m_widgetTree->SortChildren((*lookIter).second);
        }

        // Always expand the root
        m_widgetTree->Expand(rootNode);

        // Expand the last-used look. If only one look, expand that.
        if (looknfeelNode.IsOk())
        {
            m_widgetTree->Expand(looknfeelNode);
        }
        wxTreeItemId lookItem;
        if (looks.size() == 1)
        {
            lookItem = (*looks.begin()).second;
            m_widgetTree->Expand(lookItem);
        }
        // If only one widget, select it. This is convenient when heavy filtering applies, for example
        // when only a MenuItem may be added.
        cookieType cookie;
        wxTreeItemId firstChild;
        if (lookItem.IsOk() && m_widgetTree->GetChildrenCount(lookItem) == 1)
        {
            firstChild = m_widgetTree->GetFirstChild(lookItem, cookie);
        }
        else if (m_widgetTree->GetChildrenCount(rootNode) == 1)
        {
            firstChild = m_widgetTree->GetFirstChild(rootNode, cookie);
        }
        //
        if (firstChild.IsOk())
        {
            m_widgetTree->SelectItem(firstChild);
        }
    }
    //------------------------------------------------------------------------
    void DialogAddWindow::OnSelchangedWidgetTree(wxTreeEvent& event)
    {
        // Validations
        wxASSERT_MSG(m_widgetTree != NULL, wxT("Widget tree invalid or NULL"));
        wxASSERT_MSG(m_okButton != NULL, wxT("OK Button invalid or NULL"));

        const wxTreeItemId node = event.GetItem();
        // If the element has no children, it is a "real" element, and not a node.
        cookieType cookie;
        const wxTreeItemId child = m_widgetTree->GetFirstChild(node, cookie);
        if (!child.IsOk())
        {   // We have reach an end, traverse back and build up the name.
            NII::StrConv::conv(StrHelper::ToNIIString(m_widgetTree->GetItemText(node)), m_windowType);
            wxTreeItemId parent = m_widgetTree->GetItemParent(node);
            // Update 'previous' in case user hits OK
            if (parent && parent != m_widgetTree->GetRootItem())
            {
                m_previousLooknfeel = m_widgetTree->GetItemText(parent);
            }
            /*while (parent && parent != m_widgetTree->GetRootItem())
            {
                m_windowType.insert(0, wxT("/"));
                m_windowType.insert(0, m_widgetTree->GetItemText(parent));
                // Up
                parent = m_widgetTree->GetItemParent(parent);
            }*/
        }
        else
        {
            m_windowType = 0;
        }
        m_okButton->Enable(m_windowType != 0);
    }

    //------------------------------------------------------------------------
    bool DialogAddWindow::Filter(FactoryID aWidgetType, const Widget* Pwidget) const
    {
        if (!Pwidget)
        {	// Allow alot everything since the new window is the first one
            return !RequiresParent (aWidgetType);
        }
        
        // When the parent is a MenuBar or a PopupMenu, we may only add MenuItems
        if (Pwidget->getType() == N_WIDGET_Menubar)
        {
            return (aWidgetType == N_WIDGET_MenuItem);
        }
        // We never explicitly add PopupMenus
        // When the parent is a MenuItem, we have two options:
        // 1. When the item does not have a PopupMenu yet, we may only add a PopupMenu
        // 2. When the item already has a PopupMenu, we may only add a MenuItem. This works in combination with
        //  DialogMain::InitLayoutTree (the PopupMenu skipping part) and the EditorDocument::AddNewWindow 
        // (the step-into PopupMenu part).
        if (Pwidget->getType() == N_WIDGET_MenuItem)
        {
            return (aWidgetType == N_WIDGET_MenuItem);
        }
        // When the parent is a TabControl, we may only add Default windows
        if (Pwidget->getType() == N_WIDGET_TabControl)
        {
            return (aWidgetType == N_WIDGET_DefaultWindow);
        }

        // Are we to filter the displayed widgets by their parent requirement?
        if(m_treeFilter)
            // Last test: only allow widgets who make sense on their own.
            return !RequiresParent (aWidgetType);
        else
            // No parent filter required here
            return true;
    }

    //------------------------------------------------------------------------
    bool DialogAddWindow::RequiresParent(FactoryID windowType) const
    {
        // Although the filtering is now based on a widget's 'base' type, this method
        // needs some rework too. This method will let widgets pass when they are not logically expected
        // to have a required parent. E.g. a ComboDropList requires a ComboBox parent.
        // It does not filter on automatic windows, such as the sample just given ;)
        if (windowType == N_WIDGET_ComboDropList ||
            windowType == N_WIDGET_Combobox  ||
            windowType == N_WIDGET_MenuItem  ||
            windowType == N_WIDGET_PopupMenu  ||
            windowType == N_WIDGET_Slider  ||
            windowType == N_WIDGET_TabButton  ||
            windowType == N_WIDGET_PosButton  ||
            windowType == N_WIDGET_Tooltip  ||
            windowType == N_WIDGET_PushButton  ||
            windowType == N_WIDGET_Tree )
        {
            return true;
        }
        return false;
    }

    //------------------------------------------------------------------------
    void DialogAddWindow::OnOK( wxCommandEvent& WXUNUSED(event) ) 
    {
        // Validations
        wxASSERT_MSG(m_editName != NULL, wxT("Edit name invalid or NULL"));

        // Copy the values from the controls (for some reason doesn't work when closed)
        m_windowName = m_editName->GetValue();
        // We already got the type in the tree handler
        // Pass to superclass
    #if wxCHECK_VERSION(2,8,0)
        // new way of returning
        AcceptAndClose();
    #else
        wxDialog::OnOK(event);
        // Hide to pass control back to caller
        Show(false);
    #endif
    }

    //------------------------------------------------------------------------
    void DialogAddWindow::OnCancel( wxCommandEvent& WXUNUSED(event) ) 
    {
    #if wxCHECK_VERSION(2,8,0)
        // this is no longer supported
        EndDialog(wxID_CANCEL);
    #else
        wxDialog::OnCancel(event);
        Show(false);
    #endif
    }

    //------------------------------------------------------------------------
    void DialogAddWindow::OnFilterBoxSel(wxCommandEvent& event) 
    {
        // Check type of filter selected
        // NOTE: The comparison is used to avoid the annoying warning C4800 
        //       int' : forcing value to bool 'true' or 'false' (performance warning)
        m_treeFilter = (event.GetInt()!=0); 

        // Rebuild widget tree to take in account the current filter
        InitWidgetTree(m_parentWindow);
    }

    //////////////////////////////////////////////////////////////////////////
    // COMMANDADDWINDOW FUNCTIONS
    //////////////////////////////////////////////////////////////////////////

    //------------------------------------------------------------------------
    CommandAddWindow::CommandAddWindow (EditorDocument* aDocument, Window* aParent, StyleViewID aType, const String& aName) :
        // Just copy all parameters and wait for "Do".
        mDocument(aDocument),
        mParent(aParent),
        mType(aType),
        mName(aName),

        // Store for checking existence
        mParentName(aParent != NULL ? aParent->getName() : _T("")),

        // Nothing created yet
        mCreatedWindow(wx_static_cast(Window *, NULL)),

        // This command can be undone
        wxCommand(true, wxT("Add Window"))
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

        // Validations
        wxASSERT_MSG(aDocument != NULL, wxT("Supplied document is NULL"));
    }

    //------------------------------------------------------------------------
    bool CommandAddWindow::Do() 
    {
        //LogDebug(wxT("== CommandAddWindow::Do/Redo =="));
        //LogDebug(wxT(" Parent: %s"), StrHelper::ToWXString(mParentName));
        //LogDebug(wxT(" Type: %s"), StrHelper::ToWXString(mType));
        //LogDebug(wxT(" Name: %s"), StrHelper::ToWXString(mName));

        // In case of re-do, only possible when the parent is still in the layout(!)
        if (N_Only(Widget).isExist(mParentName))
        {
            // Validations
            wxASSERT_MSG(mDocument != NULL, wxT("Document member is NULL"));

            // Add the window with the given parameters
            mCreatedWindow = mDocument->AddWindow(mParent, mType, mName);
            return (mCreatedWindow != NULL);
        }

        // Could not (re)do
        LogWarning (wxT("Failed to (re)do - original parent does not exist anymore."));
        return false;
    }

    //------------------------------------------------------------------------
    bool CommandAddWindow::Undo() 
    {
        // Delete the window which was created during "Do".
        if (mCreatedWindow)
        {
            // Validations
            wxASSERT_MSG(mDocument != NULL, wxT("Document member is NULL"));

            // Remove + notify
            (void)mDocument->RemoveWindow (mCreatedWindow, true);
            mCreatedWindow = NULL;
            return true;
        }

        return false;
    }

    // One day when maybe wxWidgets-only containers and such are used,
    // it can be removed. For the moment i didn't want to rebuild with the
    // USE_STD_IOSTREAM define.
    #include <algorithm>

    //////////////////////////////////////////////////////////////////////////
    // DEFINES
    //////////////////////////////////////////////////////////////////////////

    #define BORDER            5

    #define ASSERT_DOCUMENT_MSG wxT("The m_document member is NULL!")

    // This dialog is resizable, but not smaller then this.
    #define MINIMUM_WIDTH 320
    #define MINIMUM_HEIGHT 650

    // Grid column widths
    static const int COLUMN_ROW_NUMBER_WIDTH = 40;
    static const int COLUMN_CUT_PADDING = 30;
    // The value column gets the remainder

    // We use a fake property to update a window's name.
    #define NAME            N_PropertyUIWidget_Name//wxT("Name")

    // The UDim setup. These names form our other "fake" properties; the ones who do not actually
    // exist in the GUI system. But the string versions are rather difficult and user-unfriendly
    // to edit, because of their format {{scale,offset}}. So we query the current window
    // for its area and query these values as normal floats, which are easier to edit :-)
    #define LEFT_SCALE      N_Property_Custom + 1 //wxT("Left Scale")
    #define LEFT_OFFSET     N_Property_Custom + 2 //wxT("Left Offset")
    #define TOP_SCALE       N_Property_Custom + 3 //wxT("Top Scale")
    #define TOP_OFFSET      N_Property_Custom + 4 //wxT("Top Offset")
    #define RIGHT_SCALE     N_Property_Custom + 5 //wxT("Right Scale")
    #define RIGHT_OFFSET    N_Property_Custom + 6 //wxT("Right Offset")
    #define BOTTOM_SCALE    N_Property_Custom + 7 //wxT("Bottom Scale")
    #define BOTTOM_OFFSET   N_Property_Custom + 8 //wxT("Bottom Offset")
   
    
    #define FAKE_PROPERTY_COUNT 9

    // Color used to display the "fake" property values
    #define FAKE_PROPERTY_COLOR wxColour(36,113,82)

    // Color used to display the non common "real" property values (i.e. properties 
    // that appear in some but not all elements in the current user selection)
    #define NON_COMMON_PROPERTY_COLOR wxColour(60, 69, 155)

    //////////////////////////////////////////////////////////////////////////
    // DIALOG MAIN EVENT TABLE
    //////////////////////////////////////////////////////////////////////////

    //--------------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(DialogMain, wxDialog)
        // Button-click handler(s)
        EVT_BUTTON            (ID_ADD_WINDOW, DialogMain::OnAddWindow)
        EVT_BUTTON            (ID_SAVE_SELECTION, DialogMain::OnSaveSelection)
        EVT_TREE_SEL_CHANGED  (ID_LAYOUT_TREE, DialogMain::OnSelchangedLayoutTree)
        EVT_TEXT              (ID_TEXT, DialogMain::OnChangeText)
        EVT_COMBOBOX		  (ID_SKINS, DialogMain::OnChangeSkin)
        EVT_RADIOBOX		  (ID_SKINS_DISPLAY, DialogMain::OnDisplayModeSelection) 

        // Grid handlers
        EVT_GRID_CELL_CHANGE(DialogMain::OnGridCellChange)
        EVT_GRID_COL_SIZE(DialogMain::OnGridColSize)
        EVT_SIZE(DialogMain::OnResize)
    END_EVENT_TABLE()

    //////////////////////////////////////////////////////////////////////////
    // DIALOG MAIN CONSTRUCTORS/DESTRUCTORS
    //////////////////////////////////////////////////////////////////////////

    //------------------------------------------------------------------------
    DialogMain::DialogMain(wxWindow* parent) : 
        // Put the elements to NULL, so we can block events during initialization
        m_document(wx_static_cast(EditorDocument *, NULL)),
        m_currentRoot(wx_static_cast(Window*, NULL)),
        mCurrent(wx_static_cast(Window*, NULL)),
        m_tree(wx_static_cast(wxTreeCtrl*, NULL)),
        m_boldItemWindow(NULL),
        m_addWindow(wx_static_cast(wxButton*, NULL)),
        m_saveSelection(wx_static_cast(wxButton*, NULL)),
        m_editText(wx_static_cast(wxTextCtrl*, NULL)),
        m_typeText(wx_static_cast(wxStaticText*, NULL)),
        m_skinList(wx_static_cast(wxComboBox*, NULL)),
        m_displayModeRadio(wx_static_cast(wxRadioBox*, NULL)),

        // By default, display all existing skins and properties
        m_displayOnlyCommon(false),	

        // Put the elements to NULL, so we can block events during initialization
        m_grid(wx_static_cast(wxGrid*, NULL)),
        m_propertySizer(wx_static_cast(wxStaticBoxSizer*, NULL)),
        m_fonts(wx_static_cast(wxString*, NULL)),
        m_fontCount(0),
        m_updating(false),

        m_supportedProperties(),
        m_lastSelectedRow(-1),
        m_lastSelectedWindow(wx_static_cast(Window*, NULL)),
        wxDialog(parent, wxID_ANY, wxT("Main Dialog"), wxDefaultPosition, wxSize(MINIMUM_WIDTH, MINIMUM_HEIGHT), 
                 (wxDEFAULT_DIALOG_STYLE & ~wxCLOSE_BOX) | wxRESIZE_BORDER | wxMINIMIZE_BOX /* Minimize/restore */ ) // Cannot be closed
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

        // Initialize dialog
        Setup();
    }

    //------------------------------------------------------------------------
    DialogMain::~DialogMain()
    {
        // Swallow any exceptions found to prevent the destructor from emitting them.
        // NOTE: An exception should never leave a destructor as it may yield
        // undefined behavior or even lead to program abnormal termination 
        // (see Item 8 in 'Effective C++ 3rd Edition' by Scott Meyers)
        try
        {
            m_fontCount = 0;
            delete[] m_fonts;

            // Zero out the member pointers
            // NOTE: All of these will be freed internally by wxWidgets
            m_skinList = wx_static_cast(wxComboBox*, NULL);
            m_propertySizer = wx_static_cast(wxStaticBoxSizer*, NULL);
            m_document = wx_static_cast(EditorDocument*, NULL);
            m_currentRoot = wx_static_cast(Window*, NULL);
            m_displayModeRadio = wx_static_cast(wxRadioBox*, NULL);
            m_grid = wx_static_cast(wxGrid*, NULL);
            mCurrent = wx_static_cast(Window*, NULL);
            m_tree = wx_static_cast(wxTreeCtrl*, NULL);
            m_addWindow = wx_static_cast(wxButton*, NULL);
            m_saveSelection = wx_static_cast(wxButton*, NULL);
            m_editText = wx_static_cast(wxTextCtrl*, NULL);
            m_typeText = wx_static_cast(wxStaticText*, NULL);
        }
        // Add exception to log
        LOG_EXCEPTION_SAFE("DialogMain::~DialogMain", "Failed to destroy main dialog", "Failed to destroy main dialog", "Failed to destroy main dialog");
    }
    //-----------------------------------------------------------------------
    void DialogMain::Setup()
    {
        // We can resize the dialog, but keep the given dimension as the minimum
        wxWindowBase::SetSizeHints(MINIMUM_WIDTH, MINIMUM_HEIGHT);

        // All "groups"
        wxStaticBox* const treeGroup = new wxStaticBox(this, wxID_ANY, wxT("Browse layout:"));
        wxStaticBox* const propertyGroup = new wxStaticBox(this, wxID_ANY, wxT("Available properties in the current selection:"));

        // The layout browse tree
        // NB: This must, must, MUST! be created after the static box that will be
        // used for the box sizer we add the tree to.
        m_tree = new wxTreeCtrl(this, ID_LAYOUT_TREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_VARIABLE_ROW_HEIGHT | wxTR_HAS_BUTTONS);

        // Button(s)
        m_addWindow = new wxButton(this, ID_ADD_WINDOW, wxT("Add window"));
        //m_LoadLayout = new wxButton(this, ID_LOAD_LAYOUT, wxT("Add layout"));
        m_saveSelection = new wxButton(this, ID_SAVE_SELECTION, wxT("Save current selection"));
        // Wait until a window gets selected
        (void)m_saveSelection->Enable(false);

        // Size the components vertically
        wxBoxSizer* const rootSizer = new wxBoxSizer(wxVERTICAL);

        // Sizes the tree - this one may be expanded
        // NOTE: No need to free the custodial pointer treeGroup since
        // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
        wxStaticBoxSizer* const treeSizer = new wxStaticBoxSizer(treeGroup, wxVERTICAL);
        (void)treeSizer->Add(m_tree, 1, wxEXPAND | wxALL, BORDER);

        wxBoxSizer* const buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        // Add the button  (may not stretch)
        (void)buttonSizer->Add(m_addWindow, 0, wxALL, BORDER);
        //buttonSizer->Add(m_LoadLayout, 0, wxALL, BORDER);
        (void)buttonSizer->Add(m_saveSelection, 0, wxALL, BORDER);

        // NOTE: No need to free the custodial pointer buttonSizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        (void)treeSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, BORDER);

        // Add to the root sizer (may stretch)
        // NOTE: No need to free the custodial pointer treeSizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        (void)rootSizer->Add(treeSizer, 1, wxEXPAND | wxALL, BORDER);

        //------------------------------ CONTENT GROUP -------------------------------

        // Create a framed static text for displaying the currently selected widget text
        wxStaticBox* const contentGroup = new wxStaticBox(this, wxID_ANY, wxT("Text:"));
        m_editText = new wxTextCtrl(this, ID_TEXT, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH | wxTE_RICH2);

        // The content (or caption) property
        // NOTE: No need to free the custodial pointer contentGroup since
        // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
        wxStaticBoxSizer* const contentSizer = new wxStaticBoxSizer(contentGroup, wxHORIZONTAL);
        // Use entire sizer
        (void)contentSizer->Add(m_editText, 1, wxEXPAND | wxALL, BORDER);

        // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
        // NOTE: No need to free the custodial pointer contentSizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        (void)rootSizer->Add(contentSizer, 0, wxEXPAND | wxALL, BORDER);

        //------------------------------ TYPE GROUP -------------------------------

        // Create a framed static text for displaying the type of the currently selected widget
        wxStaticBox* const typeGroup = new wxStaticBox(this, wxID_ANY, wxT("Type:"));
        m_typeText = gsgs_Style().createStaticText(this, wxID_ANY, wxT("") );
        wxStaticBoxSizer* const typeSizer = new wxStaticBoxSizer(typeGroup, wxHORIZONTAL);

        // Use entire sizer
        (void)typeSizer->Add(m_typeText, 1, wxEXPAND | wxALL, BORDER);

        // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
        // NOTE: No need to free the custodial pointer typeSizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        (void)rootSizer->Add(typeSizer, 0, wxEXPAND | wxALL, BORDER);

        //------------------------------ DISPLAY GROUP -------------------------------

        // Create display sizer
        wxBoxSizer* const displaySizer = new wxBoxSizer(wxVERTICAL);

        // Create radio box that will allow the user to switch between multi-selection All and
        // common modes
        const wxString options[] = {wxT("Show All"), wxT("Show Only Common")};
        m_displayModeRadio = new wxRadioBox(this, ID_SKINS_DISPLAY, wxT("Multi-Selection Display Mode For Skins And Properties:"), wxDefaultPosition, wxDefaultSize, 2, options);

        // Use entire sizer
        (void)displaySizer->Add(m_displayModeRadio, 0, wxEXPAND | wxALL, 0);

        // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
        // NOTE: No need to free the custodial pointer displaySizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        (void)rootSizer->Add(displaySizer, 0, wxEXPAND | wxALL, BORDER);

        // No widget selection initially, so disable the multi-selection display mode radio box
        (void)m_displayModeRadio->Enable(false);

        //------------------------------ SKIN GROUP -------------------------------

        // Create framed skin dropdown, that will hold the available skins for the current widget selection
        wxStaticBox* const skinGroup = new wxStaticBox(this, wxID_ANY, wxT("Skin:"));	
        m_skinList = new wxComboBox(this, ID_SKINS);

        // NOTE: No need to free the custodial pointer skinGroup since
        // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
        wxStaticBoxSizer* const skinSizer = new wxStaticBoxSizer(skinGroup, wxVERTICAL);

        // Use entire sizer
        (void)skinSizer->Add(m_skinList, 0, wxEXPAND | wxALL, BORDER);

        // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
        // NOTE: No need to free the custodial pointer skinSizer since
        // it will be handled internally by wxWidgets wxSizer::Add()
        (void)rootSizer->Add(skinSizer, 0, wxEXPAND | wxALL, BORDER);

        // No skin selected initially
        (void)m_skinList->Enable(false);

        //------------------------------ PROPERTY GRID -------------------------------

        // NOTE: No need to free the custodial pointer propertyGroup since
        // it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
        m_propertySizer = new wxStaticBoxSizer(propertyGroup, wxHORIZONTAL);
        CreateGrid();
        (void)m_propertySizer->Add(m_grid, 1, wxEXPAND | wxALL, BORDER);

        // Add to the root sizer. Non-proportional, but horizontal stretching is allowed.
        (void)rootSizer->Add(m_propertySizer, 1, wxEXPAND | wxALL, BORDER);

        // NOTE: No need to free the custodial pointer rootSizer since
        // it will be handled internally by wxWidgets wxWindowBase::SetSizer()
        SetSizer(rootSizer);

        // Initially, reset the entire dialog. (Does a bit more than required here, but doesn't matter)
        Reset();

        CreateSupportedPropertyMap();

        (void)wxDialog::SetEscapeId(wxID_NONE);
        (void)wxDialog::Show(true);
    }
    /************************************************************************
     * PROPERTY GRID
     ************************************************************************/
    //------------------------------------------------------------------------
    void DialogMain::ClearGrid()
    {
        if(m_grid)
        {
            const int oldRowCount = m_grid->GetRows();
            if (oldRowCount > 0)
            {
                (void)m_grid->DeleteRows(0, oldRowCount);
            }
        }
    }
    //------------------------------------------------------------------------
    void DialogMain::CreateSupportedPropertyMap()
    {
        // Read settings from options
        const wxString& properties = UILayoutOptions::GetInstancePtr()->GetSupportedProperties();

        // Parse setting - this parsing should eventually be moved in COption
        wxStringTokenizer semicolonTokenizer(properties, wxT(";"));
        while(semicolonTokenizer.HasMoreTokens())
        {
            // token hold something like:  pid,propertyType
            const wxString token = semicolonTokenizer.GetNextToken();

            wxStringTokenizer commaTokenizer(token, wxT(",")); // Second tokenizer

            const wxString pidstr = commaTokenizer.GetNextToken();
            const wxString propertyType = commaTokenizer.GetNextToken();

            if (pidstr.IsEmpty() || propertyType.IsEmpty())
            {
                continue; // Ignore this entry
            }

            // Associate a mnemonic named present in the COption string to our PropertyType enumeration.
            struct PropertyMnemonic
            {
                wxString     mnemonic;
                PropertyType type;
            } mnemonics[] =
            {
                { wxT("bool")               ,  BOOL_TYPE             },
                { wxT("float")              ,  FLOAT_TYPE            },
                { wxT("font")               ,  FONT_TYPE             },
                { wxT("horz_align")         ,  HORZ_ALIGN_TYPE       },
                { wxT("horz_text_format")   ,  HORZ_TEXT_FORMAT_TYPE },
                { wxT("text")               ,  TEXT_TYPE             },
                { wxT("vert_align")         ,  VERT_ALIGN_TYPE       },
                { wxT("vert_text_format")   ,  VERT_TEXT_FORMAT_TYPE },
            };

            // For every mnemonic type, compare with the current COption entry we are parsing
            // Note: Mnemonics that we don't know about will be silently discarded
            for (size_t i = 0; i < sizeof mnemonics / sizeof mnemonics[0]; ++i)
            {
                PropertyMnemonic& M = mnemonics[i]; // The current PropertyMnemonic we are trying to match

                // If the item we are processing match
                if (propertyType == M.mnemonic)
                {
                    Nid pid;
                    NII::StrConv::conv(StrHelper::ToNIIString(pidstr), pid);
                    (void)m_supportedProperties.insert(std::make_pair(pid, M.type)); // Add to the supported properties list
                    break;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void DialogMain::CreateGrid()
    {
        m_grid = new wxGrid(this, ID_GRID);
        (void)m_grid->CreateGrid( 0, 2 );

        m_grid->SetColLabelValue(COLUMN_NAME, wxT("Property"));
        m_grid->SetColLabelValue(COLUMN_VALUE, wxT("Value"));

        UpdateColumnWidths();
    }

    //------------------------------------------------------------------------
    DialogMain::PropertyType DialogMain::GetPropertyType(PropertyID pid)
    {
        const DialogMain::SupportedPropertiesType::iterator propertyIt = m_supportedProperties.find(pid);
        if (propertyIt != m_supportedProperties.end())
        {
            return propertyIt->second;
        }

        // An upsupported property was given
        return UNSUPPORTED;
    }

    //------------------------------------------------------------------------
    void DialogMain::UpdateColumnWidths()
    {
        if (m_propertySizer)
        {
            // Validations
            wxASSERT_MSG(m_grid != NULL, wxT("Grid object is NULL"));

            int width, unused;
            m_grid->GetSize(&width, &unused);
            // Only refresh when different
            const int leftColSize = m_grid->GetColSize(COLUMN_NAME);
            int targetWidth = (width - leftColSize)-(COLUMN_ROW_NUMBER_WIDTH + COLUMN_CUT_PADDING);
            if (targetWidth > 0 && targetWidth != m_grid->GetColSize(COLUMN_VALUE))
            {
                // Reset the column widths so that the entire view is being used. Horizontal scrollbars should
                // never occur, and it's not possible to make the right column smaller. Is this wanted? Don't think so...
                m_grid->SetRowLabelSize(COLUMN_ROW_NUMBER_WIDTH);
                m_grid->SetColSize(COLUMN_VALUE, targetWidth);
                m_grid->ForceRefresh();
            }
        }
    }

    //------------------------------------------------------------------------
    void DialogMain::UpdateProperties(bool mapSkins)
    {
        ClearGrid();
        if (m_document == NULL || m_document->GetSelection()->Size() == 0)
        {
            // The user unselected the last selection element, so we need to update the
            // selection info (text, skin, etc.) to reflect this before taking our leave
            UpdateSelectionInfo();
            return;
        }

        // Adds the Udim  and name stuff
        AddFakeProperties();

        // Add the GUI propertySet stuff
        AddRealProperties(mapSkins);

        UpdateColumnWidths();

        m_grid->AdjustScrollbars();
    }

    //------------------------------------------------------------------------
    void DialogMain::AddFakeProperties()
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, wxT("Can't add fake properties with empty document"));
        wxASSERT_MSG(mCurrent != NULL, wxT("Can't add fake properties with empty window"));

        // Only set common short-cuts when no more then 1 window is selected. Otherwise each
        // window in the selection would get the same value! Effectively making them invisible (right
        // behind the first one) or giving all the same text!
        const bool multiSelect = m_document->GetSelection()->Size() > 1;
        // Only allow changes to position and size when the window is not locked (even when it's just one window selected).
        const bool locked = m_document->GetSelection()->FindBoxFor(mCurrent)->IsLocked();
        //logDebug (wxT("%s locked? :%d"), StrHelper::ToWXString(mCurrent->getName()), locked);

        // Now pick the elements we need one by one
        // NOTE: If multi-selection exists, the property value is only displayed if 
        // ALL windows in the selection contain the same property value (common value). 
        // Otherwise, the grid entry will appear empty to mark that not 
        // all selected widgets contain the same value for that property.
        // NOTE2: We don't use property lookup for the fake ones. Just add floating 
        // points for the Scales, and integers for the Offsets. Note that you 
        // cannot pre-allocate these grids only once, because that causes Refcount 
        // asserts on cleanup...
        AddPropertyRow(new wxGridCellTextEditor(), NAME, GetFakePropertyCommonValue(NAME), multiSelect, FAKE_PROPERTY_COLOR); 
        AddPropertyRow(new wxGridCellFloatEditor(), LEFT_SCALE, GetFakePropertyCommonValue(LEFT_SCALE), multiSelect || locked, FAKE_PROPERTY_COLOR);
        AddPropertyRow(new wxGridCellNumberEditor(), LEFT_OFFSET, GetFakePropertyCommonValue(LEFT_OFFSET), multiSelect || locked, FAKE_PROPERTY_COLOR);
        AddPropertyRow(new wxGridCellFloatEditor(), TOP_SCALE, GetFakePropertyCommonValue(TOP_SCALE), multiSelect || locked, FAKE_PROPERTY_COLOR);
        AddPropertyRow(new wxGridCellNumberEditor(), TOP_OFFSET, GetFakePropertyCommonValue(TOP_OFFSET), multiSelect || locked, FAKE_PROPERTY_COLOR);
        AddPropertyRow(new wxGridCellFloatEditor(), RIGHT_SCALE, GetFakePropertyCommonValue(RIGHT_SCALE), multiSelect || locked, FAKE_PROPERTY_COLOR);
        AddPropertyRow(new wxGridCellNumberEditor(), RIGHT_OFFSET, GetFakePropertyCommonValue(RIGHT_OFFSET), multiSelect || locked, FAKE_PROPERTY_COLOR);
        AddPropertyRow(new wxGridCellFloatEditor(), BOTTOM_SCALE, GetFakePropertyCommonValue(BOTTOM_SCALE), multiSelect || locked, FAKE_PROPERTY_COLOR);
        AddPropertyRow(new wxGridCellNumberEditor(), BOTTOM_OFFSET, GetFakePropertyCommonValue(BOTTOM_OFFSET), multiSelect || locked, FAKE_PROPERTY_COLOR);
    }
    //------------------------------------------------------------------------
    wxString DialogMain::GetFakePropertyValue(PropertyID pid, const Widget * current) const
    {
        // Validations
        wxASSERT_MSG(current != NULL, wxT("Supplied window is NULL!"));

        // Check type of "fake" property and return appropriate value
        // NOTE: Unlike the "real" properties, which vary according to the widget type,
        // the "fake" properties are always the same, are always present and are 
        // common to all widgets.
        if(pid == N_PropertyUIWidget_Name)
            return StrHelper::ToWXString(current->getName());
        else
        {
            // Get the entire rectangle in one call.
            const RelRectf & windowArea = current->getRelArea();

            // Scales appear as floats, offsets as floats disguised as integers
            if(pid == LEFT_SCALE)
                return wxString::Format(wxT("%f"), windowArea.mLeft.mScale);
            else if(pid == LEFT_OFFSET)
                return wxString::Format(wxT("%.0f"), windowArea.mLeft.mOffset);
            else if(pid == TOP_SCALE)
                return wxString::Format(wxT("%f"), windowArea.mTop.mScale);
            else if(pid == TOP_OFFSET)
                return wxString::Format(wxT("%.0f"), windowArea.mTop.mOffset);
            else if(pid == RIGHT_SCALE)
                return wxString::Format(wxT("%f"), windowArea.mRight.mScale);
            else if(pid == RIGHT_OFFSET)
                return wxString::Format(wxT("%.0f"), windowArea.mRight.mOffset);
            else if(pid == BOTTOM_SCALE)
                return wxString::Format(wxT("%f"), windowArea.mBottom.mScale);
            else if(pid == BOTTOM_OFFSET)
                return wxString::Format(wxT("%.0f"), windowArea.mBottom.mOffset);        
            else
            {
                LogError(wxT("DialogMain::GetFakePropertyValue - Unknown fake property supplied!"));
                return wxT("");
            }
        }
    }
    //------------------------------------------------------------------------
    wxString DialogMain::GetFakePropertyCommonValue(PropertyID pid) const
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, wxT("Document member is NULL!"));

        // The first window in the selection will be our comparison basis
        const Widget *currentWindow = mCurrent;
        wxString firstPropertyValue = wxT("");

        // Make sure we have a valid window
        if(currentWindow)
        {
            // Obtain "fake" property value
            firstPropertyValue = GetFakePropertyValue(pid, currentWindow);

            // Verify that selection is valid (safety-check)
            if(m_document->GetSelection())
            {
                // Scan entire user selection
                Selection::Boxes& boxes = m_document->GetSelection()->GetAllBoxes();
                for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
                {
                    // If the current "fake" property value isn't the same as the stored property
                    // value for the first window in the selection, the value isn't common
                    // to ALL widgets in the selection, so it will appear as empty in the
                    // property grid
                    currentWindow = (*boxIt).GetWindow();
                    if(GetFakePropertyValue(pid, currentWindow) != firstPropertyValue)
                        return wxT("");
                }
            }
        }

        // Property value is common to ALL widgets in the selection, so we can use it
        // for display purposes
        return firstPropertyValue;
    }

    //------------------------------------------------------------------------
    void DialogMain::AddRealProperties(const bool mapSkins)
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, wxT("Can't add real properties with empty document"));

        // If desired, we can map the possible skins for the current user widget selection
        // NOTE: This is not always wanted. For instance, when the Look'N'Feel of a 
        // selection is modified, we want to keep the skins as is since the 
        // selection itself did not change and thus the possible skins remain the same
        if(mapSkins)
            MapWidgetSelectionPossibleSkins();

        // Update text, type and displayed skin info
        UpdateSelectionInfo();

        // Updates the relevant properties by calling the document for the most actual set.
        EditorDocument::RelevantProperties commonProperties, nonCommonProperties;
        m_document->GetRelevantProperties(&commonProperties, &nonCommonProperties, m_displayOnlyCommon);

        // Display common properties to all elements in the selection
        // NOTE: These properties are always shown independently of the current display mode (common or all)
        EditorDocument::RelevantPropertiesIterator current = commonProperties.begin();
        for(; current != commonProperties.end(); ++current)
        {
            LogDebug (wxT("DialogMain::AddRealProperties - Common Property '%s' = '%s'"), 
                StrHelper::ToWXString(mCurrent->getPropertyName((*current).first)).c_str(), 
                StrHelper::ToWXString((*current).second).c_str());
            // Add a new property to the property grid. Pass PropertyName, PropertyValue
            AddPropertyRow((*current).first, (*current).second);
        }
        // Do the same for the non common properties, if the display mode allows it ('Display All' only)
        if(!m_displayOnlyCommon)
        {
            for(current = nonCommonProperties.begin(); current != nonCommonProperties.end(); ++current)
            {
                // Add a new property to the property grid. Pass PropertyName, PropertyValue
                // NOTE: Non common properties' values appear in blueish color
                LogDebug (wxT("DialogMain::AddRealProperties - NonCommon Property '%s' = '%s'"), 
                    StrHelper::ToWXString(mCurrent->getPropertyName((*current).first)).c_str(), 
                    StrHelper::ToWXString((*current).second).c_str());
                AddPropertyRow((*current).first, (*current).second, NON_COMMON_PROPERTY_COLOR);            
            }
        }
    }

    //------------------------------------------------------------------------
    void DialogMain::AddPropertyRow(wxGridCellEditor* editor, PropertyID pid, const wxString& propertyValue,
        bool valueReadOnly, const wxColour& textColor, const wxColour& backgroundColor, const wxColour& lockedBackgroundColor)
    {
        // Validations
        wxASSERT_MSG(m_grid != NULL, wxT("Grid object is NULL"));

        // Add a row, and store the editor + set initial value
        (void)m_grid->AppendRows(1, true);
        const int newRowIndex = m_grid->GetRows() - 1;

        // Set the specified editor
        m_grid->SetCellEditor(newRowIndex, COLUMN_VALUE, editor);

        // Make the index zero-based(!)
        m_grid->SetCellValue(newRowIndex, COLUMN_NAME, StrHelper::ToWXString(mCurrent->getPropertyName(pid)));
        m_grid->SetReadOnly(newRowIndex, COLUMN_NAME, true);
        m_grid->SetCellValue(newRowIndex, COLUMN_VALUE, propertyValue);
        m_grid->SetReadOnly(newRowIndex, COLUMN_VALUE, valueReadOnly);	// Depends

        // Set the background colors accordingly
        m_grid->SetCellBackgroundColour(newRowIndex, COLUMN_NAME, backgroundColor);
        if (valueReadOnly)
            m_grid->SetCellBackgroundColour(newRowIndex, COLUMN_VALUE, lockedBackgroundColor);
        else
            m_grid->SetCellBackgroundColour(newRowIndex, COLUMN_VALUE, backgroundColor);

        // Change text color (only affects value column)
        m_grid->SetCellTextColour(newRowIndex, COLUMN_VALUE, textColor);

        // Set the correct font (normal if property is at default value in all elements in
        // the current selection, bold otherwise)
        SetPropertyValueFont(newRowIndex, pid);
    }

    //------------------------------------------------------------------------
    void DialogMain::AddPropertyRow(PropertyID pid, const String& propertyValue, const wxColour& textColor, 
        const wxColour& backgroundColor, const wxColour& lockedBackgroundColor)
    {
        // Get the editor for the given property (float, bool, or font)
        wxGridCellEditor* const cellEditor = GetPropertyEditor(pid);

        // Supported?
        if (cellEditor)
        {
            AddPropertyRow(cellEditor, pid, StrHelper::ToWXString(propertyValue), false, textColor, backgroundColor, lockedBackgroundColor);
        }
    }

    //------------------------------------------------------------------------
    wxGridCellEditor* DialogMain::GetPropertyEditor(PropertyID pid)
    {
        switch(GetPropertyType(pid))
        {
        case DialogMain::FLOAT_TYPE:
            // Ignores non-floating point input
            return new wxGridCellFloatEditor();
        case DialogMain::BOOL_TYPE:
        {
            static const int BOOL_CHOICE_COUNT = 2;
            const wxString boolChoices[BOOL_CHOICE_COUNT] = { wxT("False"), wxT("True") };

            // Creates a little combobox with two options: True and False.
            return new wxGridCellChoiceEditor(BOOL_CHOICE_COUNT, boolChoices);
        }
        case DialogMain::FONT_TYPE:
            // Creates a combobox with all font names
            FindAvailableFonts();
            return new wxGridCellChoiceEditor(m_fontCount, m_fonts);
        case DialogMain::TEXT_TYPE:
            return new wxGridCellTextEditor();

        case DialogMain::VERT_ALIGN_TYPE:
        {
            static const int VERT_CHOICE_COUNT = 3;
            const wxString vertChoices[VERT_CHOICE_COUNT] = { wxT("Top"), wxT("Centre"), wxT("Bottom") };

            return new wxGridCellChoiceEditor(VERT_CHOICE_COUNT, vertChoices);
        }
        case DialogMain::HORZ_ALIGN_TYPE:
        {
            static const int HORZ_CHOICE_COUNT = 3;
            const wxString horzChoices[HORZ_CHOICE_COUNT] = { wxT("Left"), wxT("Centre"), wxT("Right") };

            return new wxGridCellChoiceEditor(HORZ_CHOICE_COUNT, horzChoices);
        }
        case DialogMain::VERT_TEXT_FORMAT_TYPE:
        {
            static const int VERT_TEXT_FORMAT_CHOICE_COUNT = 3;
            const wxString vert_formatChoices[VERT_TEXT_FORMAT_CHOICE_COUNT] = { wxT("TopAligned"), wxT("VertCentred"), wxT("BottomAligned") };

                /** Because the image's vertical formatting named as the text's, we need 2 vector one for the texts
                and one for the images. If the currently selected window has the "Image" property, we return with
                the image formatting properties otherwise its a text.

                Same for the horz formatting.
                */
            static const int VERT_IMAGE_FORMAT_CHOICE_COUNT = 5;
            const wxString vert_image_formatChoices[VERT_IMAGE_FORMAT_CHOICE_COUNT] = { 
                    StrHelper::ToWXString(NUI_conv(VL_Top)),
                    StrHelper::ToWXString(NUI_conv(VL_Centre)),
                    StrHelper::ToWXString(NUI_conv(VL_Bottom)),
                    StrHelper::ToWXString(NUI_conv(VL_Expand)),
                    StrHelper::ToWXString(NUI_conv(VL_Tile))};

            if (mCurrent->isPropertyExist(N_PropertyUIWidget_Image))
                return new wxGridCellChoiceEditor(VERT_IMAGE_FORMAT_CHOICE_COUNT, vert_image_formatChoices);
            else
                return new wxGridCellChoiceEditor(VERT_TEXT_FORMAT_CHOICE_COUNT, vert_formatChoices);
        }
        case DialogMain::HORZ_TEXT_FORMAT_TYPE:
        {
            static const int HORZ_TEXT_FORMAT_CHOICE_COUNT = 8;
            const wxString horz_formatChoices[HORZ_TEXT_FORMAT_CHOICE_COUNT] = { wxT("LeftAligned"), wxT("RightAligned"), wxT("HorzCentred"),
                wxT("Justified"), wxT("WordWrapLeftAligned"), wxT("WordWrapRightAligned"), wxT("WordWrapCentred"), wxT("WordWrapJustified") };

            static const int HORZ_IMAGE_FORMAT_CHOICE_COUNT = 5;
            const wxString horz_image_formatChoices[HORZ_IMAGE_FORMAT_CHOICE_COUNT] = { 
                    StrHelper::ToWXString(NUI_conv(HL_Left)),
                    StrHelper::ToWXString(NUI_conv(HL_Centre)),
                    StrHelper::ToWXString(NUI_conv(HL_Right)),
                    StrHelper::ToWXString(NUI_conv(HL_Expand)),
                    StrHelper::ToWXString(NUI_conv(HL_Tile))};

            if (mCurrent->isPropertyExist(N_PropertyUIWidget_Image))
                return new wxGridCellChoiceEditor(HORZ_IMAGE_FORMAT_CHOICE_COUNT, horz_image_formatChoices);
            else
                return new wxGridCellChoiceEditor(HORZ_TEXT_FORMAT_CHOICE_COUNT, horz_formatChoices);
        }
        default:
            return NULL;
        }
    }
    //------------------------------------------------------------------------
    void DialogMain::SetPropertyValueFont(int propertyRow, PropertyID pid, bool forceRefresh)
    {
        // If the property is at its default value in ALL the windows in the current
        // user selection (widgets where the property doesn't exist are ignored), the
        // employed font will be normal weight. Otherwise, if at least one widget contains
        // the property with another value than the default one, the font will be bold.
        // NOTE: Please note that if we have multi-selection and the property contains
        // different values in the selected windows, the text will be empty (this is 
        // done elsewhere - GetRelevantProperties) and the type of font will 
        // have no impact since no text will be displayed. This is only relevant for single selection or 
        // for multi-selection where the property has the same value for all 
        // elements in it (be it the default or other value).
        wxFont font(*wxNORMAL_FONT);
        if(IsPropertyDefaultInSelection(pid))
            font.SetWeight(wxFONTWEIGHT_NORMAL);
        else
            font.SetWeight(wxFONTWEIGHT_BOLD);
        m_grid->SetCellFont(propertyRow, COLUMN_VALUE, font);

        // Force grid refresh (if so desired)
        if(forceRefresh)
            m_grid->ForceRefresh();
    }
    //------------------------------------------------------------------------
    bool DialogMain::IsPropertyDefaultInSelection(PropertyID pid)
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, wxT("Document is NULL"));

        // Make sure an user selection exists
        if(m_document->GetSelection())
        {
            // Scan entire user selection
            Selection::Boxes & boxes = m_document->GetSelection()->GetAllBoxes();
            for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
            {
                // Obtain currently selected widget
                const Widget * currentWindow = (*boxIt).GetWindow();

                // Check if the property is at its default value (if it exists)
                if(currentWindow->isPropertyExist(pid) && !currentWindow->isPropertyDefault(pid))
                    return false;         
            }
        }

        // Supplied property is at its default value
        // NOTE: We're also returning true if the selection doesn't exist
        return true;
    }

    //------------------------------------------------------------------------
    void DialogMain::ApplyFakePropertyChange()
    {
        // We already know that the property actually changed, so we don't have to test this here.
        // Validations
        wxASSERT_MSG(m_grid != NULL, wxT("Can't apply fake properties on empty grid"));
        wxASSERT_MSG(m_lastSelectedWindow != NULL, wxT("Can't apply fake properties on empty window"));
        wxASSERT_MSG(m_document != NULL, wxT("Can't apply fake properties on empty document"));
        wxASSERT_MSG(m_tree != NULL, wxT("Can't apply fake properties on empty tree"));

        // The fake properties have row indexes 0 to 9; the first one being the name.
        if (m_lastSelectedRow == 0)
        {
            const String newName = StrHelper::ToNIIString(m_grid->GetCellValue(m_lastSelectedRow, COLUMN_VALUE));
            LogDebug (wxT("New name: '%s'"), StrHelper::ToWXString(newName).c_str());
            LogDebug (wxT("Current name: '%s'"), StrHelper::ToWXString(m_lastSelectedWindow->getName()).c_str());
            try
            {
                // update 29-12-2009: don't compare for a difference, because appearantly wxWidgets already did that.
                m_lastSelectedWindow->setName(newName);
                // Set document 'dirty'
                m_document->Modify(true);
                // Not via the Document, so refresh the frame ourselves
                wxGetApp().GetLayoutEditorFrame()->Refresh();

                // Update the tree item too
                const std::map<Widget *, wxTreeItemId>::iterator it = m_treeItemMapping.find(m_lastSelectedWindow);
                if (it != m_treeItemMapping.end())
                {
                    m_tree->SetItemText(it->second, m_grid->GetCellValue(m_lastSelectedRow, COLUMN_VALUE));
                }
            }
            // Add exception to log
            LOG_EXCEPTION("DialogMain::ApplyFakePropertyChange", "Error", "Unexpected standard exception while changing fake property", "Unexpected non-standard exception while changing fake property");
        }
        else
        {
            // Just update the entire 'area rect'.
            RelRectf newWindowArea;

            newWindowArea.mLeft.mScale = StrHelper::WXStringToFloat(m_grid->GetCellValue(1, COLUMN_VALUE));
            newWindowArea.mLeft.mOffset = StrHelper::WXStringToFloat(m_grid->GetCellValue(2, COLUMN_VALUE));
            newWindowArea.mTop.mScale = StrHelper::WXStringToFloat(m_grid->GetCellValue(3, COLUMN_VALUE));
            newWindowArea.mTop.mOffset = StrHelper::WXStringToFloat(m_grid->GetCellValue(4, COLUMN_VALUE));
            newWindowArea.mRight.mScale = StrHelper::WXStringToFloat(m_grid->GetCellValue(5, COLUMN_VALUE));
            newWindowArea.mRight.mOffset = StrHelper::WXStringToFloat(m_grid->GetCellValue(6, COLUMN_VALUE));
            newWindowArea.mBottom.mScale = StrHelper::WXStringToFloat(m_grid->GetCellValue(7, COLUMN_VALUE));
            newWindowArea.mBottom.mOffset = StrHelper::WXStringToFloat(m_grid->GetCellValue(8, COLUMN_VALUE));

            // Apply it to the GUI window
            WindowBox* const box = m_document->GetSelection()->FindBoxFor(m_lastSelectedWindow);
            if (box)
            {
                box->SetNewWindowArea(newWindowArea);
            }
            else
            {	// Not the same window. Update area anyway!
                m_lastSelectedWindow->setRelArea(newWindowArea);
            }
            
            // Set document 'dirty'
            m_document->Modify(true);
            // Not via the Document, so refresh the frame ourselves
            wxGetApp().GetLayoutEditorFrame()->Refresh();
        }

        // Await a new selection
        m_lastSelectedWindow = NULL;
        m_lastSelectedRow = -1;
    }

    /************************************************************************
     * MISC PRIVATE FUNCTIONS
     ************************************************************************/

    //------------------------------------------------------------------------
    void DialogMain::Reset() 
    {
        // Cleanup
        ClearTree();
        ClearGrid();

        m_addWindow->Disable();
        m_editText->Disable();

        m_currentRoot = NULL;
        mCurrent = NULL;

        m_lastSelectedWindow = NULL;
        m_lastSelectedRow = -1;
    }

    //------------------------------------------------------------------------
    void DialogMain::ClearTree()
    {
        if(m_tree)
        {
            m_tree->DeleteAllItems();
            m_treeItemMapping.clear();
        }
    }

    //------------------------------------------------------------------------
    void DialogMain::HighlightCurrentWindow()
    {
        // Validations
        wxASSERT_MSG(m_tree != NULL, wxT("Tree object is NULL"));
        wxASSERT_MSG(m_addWindow != NULL, wxT("Add Window button is NULL"));

        // Select + highlight (make bold) new item  
        const std::map<Widget *, wxTreeItemId>::iterator it = m_treeItemMapping.find (mCurrent);
        if (it != m_treeItemMapping.end())
        {
            // Un-bold the current item
            //if (m_boldItem.IsOk())
            const std::map<Widget *, wxTreeItemId>::iterator boldItemIt = m_treeItemMapping.find(m_boldItemWindow);
            if (boldItemIt != m_treeItemMapping.end())
            {
                //m_tree->SetItemBold(m_boldItem, false);
                m_tree->SetItemBold(boldItemIt->second, false);
            }

            // Bold and show new item
            m_tree->SelectItem(it->second) ;
            m_tree->EnsureVisible (it->second) ;
            m_tree->Expand(it->second) ;
            m_tree->SetItemBold(it->second, true);

            m_boldItemWindow = it->first;

            // Enable the Add Element button
            (void)m_addWindow->Enable() ;
        }
    }

    //------------------------------------------------------------------------
    void DialogMain::FindAvailableFonts () 
    {
        wxArrayString fonts;

        // Only do this once (assuming at least one default font)
        if (m_fonts == NULL)
        {
            const FontManager::FontMap & fontIt = N_Only(Font).getList();
            FontManager::FontMap::const_iterator i, iend = fontIt.end();
            for(i = fontIt.begin(); i != iend; ++i)
            {
                LogDebug (wxT("PropertyGrid::FindAvailableFonts - Found font: %s."), StrHelper::ToWXString(i->second->getName()).c_str());
                fonts.Add (StrHelper::ToWXString(i->second->getName())) ;
            }

            // Found any?
            if (fonts.GetCount() > 0)
            {
                m_fontCount = fonts.GetCount();
                m_fonts = new wxString[m_fontCount];
                if (m_fonts)
                {
                    for (size_t i=0; i<m_fontCount; i++)
                    {
                        m_fonts[i] = fonts[i];
                    }
                }
            }
            else
            {
                LogError (wxT("PropertyGrid::FindAvailableFonts - Serious memory problems while allocating %d strings."), m_fontCount);
            }
        }
    }

    //------------------------------------------------------------------------
    void DialogMain::InitLayoutTree (Widget* window, const wxTreeItemId& parent, const bool addThis) 
    {
        if (window == 0)
            return;
        // Validations 
        wxASSERT_MSG(m_tree != NULL, wxT("Tree object member is NULL"));

        wxTreeItemId node;
        // Either add a new node, or add to the parent. The latter allows us to 'skip' helper widgets,
        // such as _TabPane_'s.
        if (addThis)
        {
            node = m_tree->AppendItem(parent, StrHelper::ToWXString(window->getName())) ;
            // Cache the window / treeItem pair for for "bolding" and "un-bolding".
            AddTreeItemMapping (window, node) ;
        }
        else
        {
            node = parent;
        }

        if (window->isContainer())
        {
            Container * tempwin = static_cast<Container *>(window);
            // Add its children
            for (size_t i = 0; i < tempwin->getCount(); ++i)
            {	// tuan.kuranes@free.fr
                // even auto element may have editable properties (name)
                // others should be detected when edited.
                // Auto- or Tab-helpers should not have their own node. They get skipped, but their
                // children may be added. Same for PopupMenus, which are also skipped. 
                // (See the filtering in DialogAddWindow for more info).
                if (tempwin->get(i)->getName().find(_T("__auto_")) == String::npos &&
                    tempwin->get(i)->getName().find(_T("__TabPane__")) == String::npos &&
                    tempwin->get(i)->getType() != N_WIDGET_PopupMenu)
                {
                    // Just an ordinary widget; branch may be added
                    InitLayoutTree(tempwin->get(i), node);
                }
                // Does helper widget has children?
                else if (tempwin->get(i)->isContainer())
                {
                    Container * temp = static_cast<Container *>(tempwin->get(i));
                    if(temp->getCount() > 0)
                    // Yes: probably a _TabPane_ or _MenuBar_; branch may be added, but re-use the last node,
                    // effectively skipping the helper parent.
                    InitLayoutTree(temp, node, false);
                }
                else
                {
                    LogTrace(wxT("initLayoutTree - stopped expanding at '%s'."), StrHelper::ToWXString(tempwin->get(i)->getName()).c_str());
                }
            }
        }
    }
    /* OLD VERSION

    void DialogMain::InitLayoutTree (Window* window, wxTreeItemId& parent, bool addThis) 
    {
    wxTreeItemId node = m_tree->AppendItem(parent, StrHelper::ToWXString(window->getName())) ;

    // Cach the window / treeItem pair for for "bolding" and "un-bolding".
    AddTreeItemMapping (window, node) ;

    // Add it's children
    for (size_t i = 0; i < window->getChildCount(); ++i) 
    {    
    // Deny automatically added elements.
    //if (window->get (i)->getName().find("__auto_") == String::npos && window->get (i)->getName().find("__TabPane__") == String::npos)
    {      
    // This branche may be added
    InitLayoutTree (window->get (i), node) ;
    }
    //else 
    //{
    //LogTrace (wxT("initLayoutTree - denied (cut of) %s."), StrHelper::ToWXString(window->get (i)->getName()));
    //}
    }
    }*/


    //------------------------------------------------------------------------
    void DialogMain::AddTreeItemMapping (Widget* pWindow, const wxTreeItemId& treeItem)
    {
        // Validations
        wxASSERT_MSG(pWindow != NULL, wxT("Supplied window is NULL"));

        (void)m_treeItemMapping.insert (std::make_pair (pWindow, treeItem)) ;
        LogTrace (wxT("Cache element/tree pair for '%s'"), StrHelper::ToWXString(pWindow->getName()).c_str()) ;
    }

    //------------------------------------------------------------------------
    Window * DialogMain::FindWindow (const wxTreeItemId& treeItem)
    {
        std::map<Widget *, wxTreeItemId>::iterator it ;
        for(it = m_treeItemMapping.begin(); it != m_treeItemMapping.end (); ++it)
        {
            if(it->second == treeItem) 
            {
                return dynamic_cast<Window *>(it->first);
            }
        }
        return NULL;
    }
    //------------------------------------------------------------------------
    void DialogMain::SyncTree()
    {
        // Validations
        wxASSERT_MSG(m_tree != NULL, wxT("Tree object is NULL"));

        m_treeItemMapping.clear();
        m_tree->DeleteAllItems();

        // Build up a new tree?
        if (m_currentRoot)
        {
            const wxTreeItemId parent = m_tree->GetRootItem();
            // Add a parent, pass the root
            InitLayoutTree (m_currentRoot, parent);

            // Update the visibility and bold-ness state
            HighlightCurrentWindow();
        }
    }

    //------------------------------------------------------------------------
    bool DialogMain::WidgetLookExists(WidgetViewlID widget, StyleID look)
    {
        // Locate an iterator to the first pair object (widget/look) associated with the widget
        WidgetLookHash::iterator widgetLook =  m_widgetLooks.find(widget);
        if(widgetLook == m_widgetLooks.end())
            return false; // No looks associated with widget, so return immediately

        // Get an iterator to the element that is one past the last look associated with widget
        const WidgetLookHash::iterator lastWidgetLook = m_widgetLooks.upper_bound(widget);

        // For each element in the sequence [widget, lastWidgetElement)
        for ( ; widgetLook != lastWidgetLook; ++widgetLook)
        {
            // Is this the look we are looking for?
            if((*widgetLook).second == look)
                return true;
        }

        // Widget look not found
        return false;
    }

    //------------------------------------------------------------------------
    DialogMain::WidgetLooks DialogMain::GetWidgetLooks(WidgetViewlID widget)
    {
        WidgetLooks widgetLooks;

        //Locate an iterator to the first pair object (widget/look) associated with the widget
        WidgetLookHash::iterator widgetLook = m_widgetLooks.find(widget);
        if(widgetLook == m_widgetLooks.end())
            return widgetLooks; // No looks associated with widget, so return immediately

        // Get an iterator to the element that is one past the last look associated with widget
        const WidgetLookHash::iterator lastWidgetLook = m_widgetLooks.upper_bound(widget);

        // For each element in the sequence [widget, lastWidgetElement)
        for ( ; widgetLook != lastWidgetLook; ++widgetLook)
            widgetLooks.push_back((*widgetLook).second);

        // Return existing looks for supplied widget
        return widgetLooks;
    }
    //------------------------------------------------------------------------
    DialogMain::LookAndWidget DialogMain::GetLookAndWidgetFromType(Widget *window) const
    {
        LookAndWidget result;

        result.mWindow = window;
        if(!window)
            return result;

        // Store and return result
        result.mStyleView = window->getStyleView();
        result.mStyle = window->getStyle();
        result.mModel = window->getType();
        result.mView = window->getWidgetView()->getType();
        return result;
    }
    //------------------------------------------------------------------------
    void DialogMain::MapWidgetSelectionPossibleSkins()
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, wxT("Document object is NULL"));

        WidgetLooks widgetLooks;
        LookAndWidget currentLookAndWidget;

        // Check if we have a single or multi selection
        if (m_document->GetSelection()->Size() == 1)
        {
            // Single selection, so obtain and sort available looks for the selected widget
            currentLookAndWidget = GetLookAndWidgetFromType(mCurrent);
            widgetLooks = GetWidgetLooks(currentLookAndWidget.mView);
            widgetLooks.sort();
        }
        else
        {
            // Multi-selection, so we need to check the available looks for all the widgets
            // in the current selection. Let's start by retrieving the first one in the selection
            Selection::Boxes::iterator boxIt = m_document->GetSelection()->GetAllBoxes().begin();
            Widget *currentWindow = (*boxIt).GetWindow();

            // Obtain available looks for the first widget in the selection
            currentLookAndWidget = GetLookAndWidgetFromType(currentWindow);
            widgetLooks = GetWidgetLooks(currentLookAndWidget.mView);

            // Scan the rest of the selection
            for (; boxIt != m_document->GetSelection()->GetAllBoxes().end(); ++boxIt)
            {
                // Obtain current widget in the selection
                currentWindow = (*boxIt).GetWindow();

                // Obtain available looks for the current widget in the selection
                currentLookAndWidget = GetLookAndWidgetFromType(currentWindow);
                WidgetLooks currentWidgetLooks = GetWidgetLooks(currentLookAndWidget.mView);

                // Check currently chosen skin display type
                if(m_displayOnlyCommon)
                    // The user has chosen to only display skins that are common to ALL the 
                    // elements in the selection. Therefore, we eliminate from the global widgetLooks list
                    // all those that are not present in the current widget look list. In the end, 
                    // widgetLooks will only hold the common skins.
                    (void)widgetLooks.erase(std::remove_if( widgetLooks.begin(), widgetLooks.end(), NotIn(currentWidgetLooks)), widgetLooks.end());
                else
                {
                    // The user has chosen to display all skins, even if they do not apply to all the widgets
                    // in the selection. Therefore, we add the current widget look list to the global
                    // widgetLooks list (we'll deal with the duplicates later on)
                    widgetLooks.splice(widgetLooks.end(), currentWidgetLooks);
                }
            }

            // Sort the available looks of the widgets in the selection
            widgetLooks.sort();

            // If we are displaying all the skins, we need to eliminate the duplicate entries
            if(!m_displayOnlyCommon)
            {
                // Eliminate all duplicates
                // NOTE: Elements need to be sorted before calling this, otherwise the unique function 
                // (since it only processes consecutive duplicates) will have problems
                COMPILE_TODO("Lazy approach. See TODO on the splice method() above for more info")
                (void)widgetLooks.erase(std::unique(widgetLooks.begin(), widgetLooks.end()), widgetLooks.end());
            }
        }

        // We're going to repopulate the skin dropdown, so let's start by clearing it
        m_skinList->Clear();

        // Scan the available looks discovered earlier on
        WidgetLooks::iterator widgetIter = widgetLooks.begin();
        for(;widgetIter != widgetLooks.end(); widgetIter++)
        {
            bool valid = true;
            if(m_displayOnlyCommon)
            {
                // (Safety check) Make sure that the current look can be applied to each widget in the selection
                // NOTE: No problems should occur here, since the possible looks were already mapped at the
                // start of the application
                Selection::Boxes & boxes = m_document->GetSelection()->GetAllBoxes();
                for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
                {
                    // Build a look type based on the current widget and the current look
                    Widget * const currentWindow = (*boxIt).GetWindow();
                    currentLookAndWidget = GetLookAndWidgetFromType(currentWindow);

                    // Make sure that the type is valid one
                    // NOTE: This should never fail
                    if (!N_Only(Widget).isStyleViewExist(currentLookAndWidget.mModel, currentLookAndWidget.mStyle, *widgetIter))
                        valid = false;
                }
            }

            // Make sure we are allowed to proceed
            if (valid)
            {
                // Add look to the skin dropdown
                m_skinList->AppendString(StrHelper::ToWXString(N_conv(*widgetIter)));

                // If we are doing a single selection, check if the current look is the one
                // that is currently mapped on the selected widget. If so, select it.
                COMPILE_TODO("For multi-selection, also select value if all share the same skin")
                if(m_document->GetSelection()->Size() == 1)
                {
                    // Select look if it's the one currently mapped
                    // NOTE: No worries here, since this is a single selection, so the
                    // currentLookAndWidget struct will hold the look and widget we desire
                    if(*widgetIter == currentLookAndWidget.mStyle)
                        m_skinList->SetValue(StrHelper::ToWXString(N_conv(*widgetIter)));
                } // if
            } // if
        } // for

        // Should we allow the user to choose a skin based on the current selection?
        // NOTE: When in multi-selection mode, we want to keep the dropdown enabled 
        // even when only a single skin is available because not all selections might have
        // that skin applied
        if( (m_skinList->GetCount() > 1 && m_document->GetSelection()->Size() == 1) ||
            (m_skinList->GetCount() > 0 && m_document->GetSelection()->Size() > 1))
            (void)m_skinList->Enable(true);
        else
            (void)m_skinList->Enable(false);
    }
    //------------------------------------------------------------------------
    void DialogMain::UpdateSelectionInfo()
    {
        // Validations
        wxASSERT_MSG(m_skinList != NULL, wxT("Skin list wasn't properly initialized"));
        wxASSERT_MSG(m_typeText != NULL, wxT("Type text wasn't properly initialized"));
        wxASSERT_MSG(m_editText != NULL, wxT("Edit text wasn't properly initialized"));
        wxASSERT_MSG(m_displayModeRadio != NULL, wxT("Skin display radio wasn't properly initialized"));

        // The displayed info is totally dependent on the selection count, so let's start
        // by checking that
        if (m_document && m_document->GetSelection()->Size() == 1)
        {
            // Validations
            wxASSERT_MSG(mCurrent != NULL, wxT("Selection count is 1 and current window is empty"));
            
            // Single selection : Display currently selected widget type, text and skin and
            // allow the user to change the last two
            const LookAndWidget currentLookAndWidget = GetLookAndWidgetFromType(mCurrent);
            m_skinList->SetValue(StrHelper::ToWXString(N_conv(currentLookAndWidget.mStyle)));
            m_typeText->SetLabel(StrHelper::ToWXString(N_conv(mCurrent->getType())));
            // Mute OnChange events
            m_updating = true;
            if(mCurrent->isPropertyExist(N_PropertyUIWidget_Text))
            {	
                m_editText->SetValue(StrHelper::ToWXString(mCurrent->getPropertyValue(N_PropertyUIWidget_Text)));
            }
            else
            {
                m_editText->SetValue(wxT("Property \"Text\" not supported in current selection"));
            }
            m_updating = false;
            (void)m_displayModeRadio->Enable(false);
            (void)m_skinList->Enable();
            (void)m_editText->Enable();
        }
        else if (m_document && m_document->GetSelection()->Size() > 1)
        {
            // Multi-selection : Display common info about the selected elements
            // NOTE: For the moment, type and text don't support multi-selection, so 
            // let's inform the user and leave it at that
            COMPILE_TODO("Allow scrollable text area which displays the names of all the types in the selection")
            m_typeText->SetLabel(wxT("Multi-selection detected. Not yet supported."));
            //m_editText->SetValue(wxT("Multi-selection detected. Not yet supported."));
            (void)m_displayModeRadio->Enable(true);
            (void)m_skinList->Enable();
            //(void)m_editText->Disable();
        }
        else
        {
            // No selection : Clear all info and prevent user iteration
            m_typeText->SetLabel(wxT(""));
            m_editText->SetValue(wxT(""));
            m_skinList->Clear();
            (void)m_skinList->Disable();
            (void)m_displayModeRadio->Enable(false);
            (void)m_editText->Disable();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // DIALOG MAIN PROTECTED FUNCTIONS
    //////////////////////////////////////////////////////////////////////////

    /************************************************************************
     * EVENTS
     ************************************************************************/

    //------------------------------------------------------------------------
    // Do nothing. This effectively refuses to close the dialog when Escape is pressed.
    void DialogMain::OnCancel(wxCommandEvent& WXUNUSED(event))
    {
    }

    //------------------------------------------------------------------------
    // Do nothing. This effectively refuses to close the dialog when Enter is pressed.
    void DialogMain::OnOK(wxCommandEvent& WXUNUSED(event))
    {
    }

    //------------------------------------------------------------------------
    void DialogMain::OnAddWindow(wxCommandEvent& WXUNUSED(event)) 
    {
        try
        {
            // If we don't have a current window, just pick the root.
            if (!mCurrent )
            {
                mCurrent = N_Only(UI).getSheet(N_Engine().getView())->getActive();
            }
            
            bool allowed = false;
            if (mCurrent )
            {	// If we still don't have a current window, the new one will be the first one
                // Determine the context of the current window
                WindowContext windowContext (mCurrent);
                allowed = windowContext.AcceptsWindowAsChild();
            }
            else
            {	// Will be the first one, so no limitations yet
                allowed = true;
            }
            if (allowed)
            {    
                // Create new 'Add Window' Dialog
                DialogAddWindow *addWindow = new DialogAddWindow(this, mCurrent);

                // Present dialog to user (modal)
                Container * tempp = dynamic_cast<Container *>(mCurrent);
                if (addWindow->ShowModal () == wxID_OK && tempp)
                {
                    wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
                    // Add to Document via a command. This allows Undo and Redo.
                    m_document->AddWindow(tempp, addWindow->GetWindowType(), StrHelper::ToNIIString(addWindow->GetWindowName()));
                }

                addWindow->Destroy();
                delete addWindow;
            }
            else
            {
                LogWarning(wxT("This widget does not support children via XML.\nSee the GUI API for programmatically adding children to this widget."));
            }
        }
        // If something goes wrong, show user
        SHOW_EXCEPTION("DialogMain::OnAddWindow", "Error", "Unexpected standard exception while adding window", "Unexpected non-standard exception while adding window");
    }

    //------------------------------------------------------------------------
    void DialogMain::OnSaveSelection(wxCommandEvent& WXUNUSED(event))
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, wxT("Document is NULL"));

        // Restore directory before returning otherwise all file-access
        // will happen in this folder, including saving of layouts and such.
        const wxString currentDirectory = wxGetCwd();

        wxFileDialog dialog(this, wxT("Open a layout"), currentDirectory, wxT(""), wxT("Layout files (*.xml;*.layout;*.*)|*.xml;*.layout;*.*"), wxFD_SAVE);
        if (dialog.ShowModal() == wxID_OK)
        {
            N_Only(Widget).writeWidget(mCurrent, StrHelper::ToNIIString(dialog.GetPath()));
        }

        wxSetWorkingDirectory(currentDirectory);
    }

    //------------------------------------------------------------------------
    void DialogMain::OnSelchangedLayoutTree(wxTreeEvent& event) 
    {
        Window * selected;
        // Resolve the treeItem->window mapping

        // Because this event also gets generated when we call m_tree->SelectItem in the
        // HightlighWindow method, we "if" on the return value to deny a second call.
        selected = FindWindow(event.GetItem());

        // Deny first event.
        if (selected != mCurrent && selected != NULL)
        {
            mCurrent = selected;
            // This will eventually get the "elementSelected" method being called.
            wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
            m_document->SelectWindow (mCurrent);

            (void)m_addWindow->Enable() ;
        }
    }

    //------------------------------------------------------------------------
    void DialogMain::OnChangeText(wxCommandEvent& WXUNUSED(event))
    {
        if (m_updating)
            return;

        // Validations
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        wxASSERT_MSG(m_editText != NULL, wxT("Edit text object is NULL"));

        // Prevent any changes in multi-selection
        if (m_document->GetSelection()->Size() == 1)
        {
            try
            {	// http://www.cegui.org.uk/mantis/view.php?id=278 
                // Allow empty string!
                (void)m_document->SetProperty (N_PropertyUIWidget_Text, StrHelper::ToNIIString(m_editText->GetValue()), true);
            }
            SHOW_EXCEPTION("DialogMain::OnChangeText", "Error", "Unable to set the Text property of this widget", "Unable to set the Text property of this widget");
        }
    }

    //------------------------------------------------------------------------
    void DialogMain::OnGridCellChange(wxGridEvent& event)
    {	
        // This event gets called when stopping the edit of a cell,
        // by clicking with the mouse in another cell, or by hitting Enter.
        LogDebug (wxT("Saving row %d"), event.GetRow());
        SaveProperty(event.GetRow());
    }

    //------------------------------------------------------------------------
    void DialogMain::SaveProperty(int row)
    {
        // This method only gets called when a row actually changed. So wxWidgets
        // does a lot of work already.

        // Validations
        wxASSERT_MSG(m_grid != NULL, wxT("Grid object is NULL"));
        wxASSERT_MSG(m_document != NULL, wxT("Document is NULL"));

        // Update members for the future saving
        m_lastSelectedRow = row;
        m_lastSelectedWindow = mCurrent;

        if (m_lastSelectedWindow != NULL && m_lastSelectedRow >= 0)
        {
            // Is it a real property or a fake one?
            // NOTE: Fake properties have indices in the range [0, FAKE_PROPERTY_COUNT]
            if (m_lastSelectedRow < FAKE_PROPERTY_COUNT)
            {
                // Non existing property, handle manually
                ApplyFakePropertyChange();
            }
            else
            {
                // "Real" GUI property, set it directly.
                const wxString pidstr = m_grid->GetCellValue(m_lastSelectedRow, COLUMN_NAME);
                const wxString propertyValue = m_grid->GetCellValue(m_lastSelectedRow, COLUMN_VALUE);
                Ni32 pid;
                NII::StrConv::conv(StrHelper::ToNIIString(pidstr), pid);
                
                (void)m_document->SetProperty(pid, StrHelper::ToNIIString(propertyValue));

                // Set the correct font (normal if property is at default value in all elements in
                // the current selection, bold otherwise)
                SetPropertyValueFont(m_lastSelectedRow, pid, true);
            }
        }
    }

    //------------------------------------------------------------------------
    void DialogMain::OnGridColSize(wxGridSizeEvent& WXUNUSED(event))
    {
        // Re-arrange columns within current space
        UpdateColumnWidths();
    }

    //------------------------------------------------------------------------
    void DialogMain::OnResize(wxSizeEvent& event)
    {
        UpdateColumnWidths();
        // Let the parent handle this event as well(!)
        event.Skip();
    }

    //------------------------------------------------------------------------
    void DialogMain::OnChangeSkin(wxCommandEvent& WXUNUSED(event))
    {
        // Apply new Look'N'Feel to current widget selection
        Ni32 temp;
        NII::StrConv::conv(StrHelper::ToNIIString(m_skinList->GetValue()), temp);
        SetWidgetsLook(m_document->GetSelection(), temp);
    }

    //------------------------------------------------------------------------
    void DialogMain::OnDisplayModeSelection(wxCommandEvent& event) 
    {
        // Check new skin and property display type
        // NOTE: The comparison is used to avoid the annoying warning C4800 
        //       int' : forcing value to bool 'true' or 'false' (performance warning)
        m_displayOnlyCommon = (event.GetInt()!=0); 

        // Remap the available skins for the current selection to take in account the new display type
        MapWidgetSelectionPossibleSkins();

        // Do the same for the property panel
        UpdateProperties();
    }

    //////////////////////////////////////////////////////////////////////////
    // DIALOG MAIN PUBLIC FUNCTIONS
    //////////////////////////////////////////////////////////////////////////

    /************************************************************************
     * MISC PUBLIC FUNCTIONS
     ************************************************************************/

    //------------------------------------------------------------------------
    void DialogMain::SetDocument (EditorDocument* document) 
    {
        m_document = document;
        if (m_document)
        {
            m_document->AddObserver(this);
        }
        Reset();
    }

    //------------------------------------------------------------------------
    void DialogMain::WindowSelected (Widget * aWindow) 
    {
        // Validations
        wxASSERT_MSG(m_saveSelection != NULL, wxT("Save selection button is NULL"));

        mCurrent = aWindow;
        if (mCurrent)
        {
            mCurrent->moveFront();
            (void)m_saveSelection->Enable(true);
        }
        else
        {
            (void)m_saveSelection->Enable(false);
        }

        // Refresh the property set for the current selection
        UpdateProperties();

        HighlightCurrentWindow();
    }

    //------------------------------------------------------------------------
    void DialogMain::LayoutOpened (Window* aRoot) 
    {
        m_currentRoot = aRoot;
        mCurrent = aRoot;
        SyncTree();

        // Can add windows then
        (void)m_addWindow->Enable();
    }
    //------------------------------------------------------------------------
    void DialogMain::LayoutClosed () 
    {
        Reset();

        SyncTree();

        // Cannot add then
        (void)m_addWindow->Disable();
    }
    //------------------------------------------------------------------------
    void DialogMain::LayoutStarted (Window* aRoot) 
    {
        m_currentRoot = aRoot;
        mCurrent = aRoot;
        SyncTree();

        // Can add windows then
        (void)m_addWindow->Enable();
    }
    //------------------------------------------------------------------------
    void DialogMain::LayoutModified (Window* WXUNUSED(aRoot)) 
    {
    }
    //------------------------------------------------------------------------
    void DialogMain::WindowAdded (Widget * aWindow) 
    {
        if (aWindow) 
        {
            mCurrent = aWindow;
            if (mCurrent == N_Only(UI).getSheet(N_Engine().getView())->getActive())
            {
                m_currentRoot = static_cast<Window *>(mCurrent);
            }
        }
        SyncTree();
    }
    //------------------------------------------------------------------------
    void DialogMain::WindowsRemoved () 
    {
        mCurrent = NULL;
        // Did the root got removed?
        if (N_Only(UI).getSheet(N_Engine().getView())->getActive() == NULL)
        {
            m_currentRoot = NULL;
        }
        SyncTree();
    }
    //------------------------------------------------------------------------
    void DialogMain::SelectionDraggedOrResized()
    {
        LogDebug(wxT("DialogMain::SelectionDraggedOrResized"));
        // Refresh the property set for the current selection
        UpdateProperties();
    }
    //------------------------------------------------------------------------
    void DialogMain::StoreWidgetLooks(bool clearBeforeHand)
    {
        // Clear the widget look hash multimap if so desired
        if(clearBeforeHand)
            m_widgetLooks.clear();

        // Clear previous available looks
        // NOTE: We always need to clear this, since we want unique entries here
        m_availableLooks.Clear();

        // Scan all existing falagard mappings
        //const String separator("/");
        const WidgetManager::StyleViewList & vlist =  N_Only(Widget).getStyleViewList();
        WidgetManager::StyleViewList::const_iterator i, iend = vlist.end();
        for(i = vlist.begin(); i != iend; ++i)
        {
            // Obtain current falagard mapping (which corresponds to the widget type)
            //const String currentFal = falIt.getCurrentKey();

            // Retrieve the widget look and the widget kind
            //const String::size_type pos = currentFal.find(separator);
            StyleID look(i->second.mStyle);
            WidgetViewlID widget(i->second.mView);

            // Mark look as a possible one for the widget, if this hasn't already been done in the past
            if(!WidgetLookExists(widget, look))
                (void)m_widgetLooks.insert(std::make_pair(widget, look));

            // We also want to store this as a global available Look'N'Feel for later use (e.g. for 
            // instance, for the set layout skin operation). But we need to avoid duplicate entries.
            StrHelper::WXPushBackUnique(m_availableLooks, StrHelper::ToWXString(N_conv(look)));
        }
        // Sort the looks for easier user interaction
        m_availableLooks.Sort();
    }
    //------------------------------------------------------------------------
    void DialogMain::SetWidgetsLook(Selection* currentSelection, StyleID newLook)
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, wxT("Document is NULL"));
        if(!currentSelection)
            return;

        // Apply desired Look'N'Feel to all widgets currently selected
        Selection::Boxes& boxes = currentSelection->GetAllBoxes();
        for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
        {
            // Apply new Look'N'Feel to widget
            SetWidgetLook((*boxIt).GetWindow(), newLook, false);
        }

        // Refresh the main frame so that the Look'N'Feel changes are immediately visible
        wxGetApp().GetLayoutEditorFrame()->Refresh();

        // Update current selection displayed properties and info, making sure the skin mapping is not
        // rebuilt since this operation did not affect the selection itself
        UpdateProperties(false);
    }
    //------------------------------------------------------------------------
    void DialogMain::SetWidgetLook(Widget *currentWindow, StyleID newLook, bool refresh)
    {
        COMPILE_FIXME("Problem with static image... check if renderer problem")
        wxASSERT_MSG(currentWindow != NULL, wxT("Supplied current window is NULL"));

        StyleID currentType = currentWindow->getStyle();

        if(newLook == currentType)
            return;

        // Obtain falagard mapping for type
        const WidgetManager::StyleView * fwm = N_Only(Widget).getStyleView(currentWindow->getType(), currentWindow->getWidgetView()->getType(), newLook);
        if (fwm)
        {

            // Retrieve and store the currently assigned widget properties that are editable, so 
            // we that can restore them after setting the new Look'N'Feel. This ensures 
            // that changes made by the user are maintained.
            // NOTE: getPropertyIterator() makes sure the compiler doesn't confuse EventSet and PropertySet (it 
            // would if we used getIterator()) thus avoiding useless casts
            typedef std::map<PropertyID, String> PropMap; 
            PropMap originalProps;
            const PropertyCmdMap & temp = currentWindow->getProperty()->getList();
            PropertyCmdMap::const_iterator i, iend = temp.end();
            for(i = temp.begin(); i != iend ; ++i)
            {
                // Obtain current property name
                PropertyID pid = i->first;

                // Check if this property is editable (i.e. supported for user edition inside 
                // the LayoutEditor). If so, we want to store it for later reference
                if(this->GetPropertyType(pid) != UNSUPPORTED)
                    (void)originalProps.insert(std::make_pair(pid, currentWindow->getPropertyValue(pid)));

                // We also want to keep some properties, that while not directly editable by the user,
                // are affected by the new Look'N'Feel
                COMPILE_TODO("This is not very elegant, and needs to be changed as soon as the application supports more editable properties")
                else if (pid == N_PropertyUIWidget_TextColour || pid == N_PropertyUIWidget_Image ||
                pid == N_PropertyUICursorWidgetAide_MouseCursor || 
                pid == N_PropertyUIFrameWindow_TBSizingCursor || pid == N_PropertyUIFrameWindow_LRSizingCursor ||
                pid == N_PropertyUIFrameWindow_BRSizingCursor || pid == N_PropertyUIFrameWindow_BLSizingCursor ||
                pid == N_PropertyUIWidget_CursorMultiClick)
                    (void)originalProps.insert(std::make_pair(pid, currentWindow->getPropertyValue(pid)));
            }

            try
            {            
                currentWindow->setStyle(fwm->mStyle, fwm->mView);
            }
            // If something goes wrong, show user and return
            SHOW_EXCEPTION_RETURN("DialogMain::SetWidgetLook", "Error", "Unexpected standard exception while setting widget look", "Unexpected non-standard exception while setting widget look",);

            // Time to restore the original editable properties
            for(PropMap::iterator it = originalProps.begin(); it != originalProps.end(); it++)
            {
                // Obtain current property name
                const PropertyID pid = (*it).first;

                // Although the Look'N'Feel and WindowRender properties are not 
                // currently editable, we want to REALLY make sure they are not processed here (safety-check)
                if(pid != N_PropertyUIWidget_Style && pid != N_PropertyUIWidget_View)
                {
                    // Make sure this property is still valid with the new Look'N'Feel
                    if(currentWindow->isPropertyExist(pid))
                    {
                        // Restore property
                        currentWindow->setPropertyValue(pid, (*it).second);

                        // If the property wasn't correctly set (i.e. because the value
                        // is not valid), we want to remove it to prevent garbage
                        // NOTE: This is mostly for the MouseCursorImage property, whose
                        // set() implementation (see GUIWindowProperties.cpp) prevents
                        // empty entries.
                        if(currentWindow->getPropertyValue(pid) != (*it).second)
                        {
                            COMPILE_TODO("This is not very elegant, and needs to be changed as soon as the application supports more editable properties")
                                if (pid == N_PropertyUIWidget_Image ||
                                    pid == N_PropertyUICursorWidgetAide_MouseCursor ||
                                    pid == N_PropertyUIFrameWindow_TBSizingCursor || 
                                    pid == N_PropertyUIFrameWindow_LRSizingCursor ||
                                    pid == N_PropertyUIFrameWindow_BRSizingCursor || 
                                    pid == N_PropertyUIFrameWindow_BLSizingCursor)
                                    currentWindow->getProperty()->remove(pid);
                        } // if
                    } // if
                } // if
            } // for
        } // if
        // Do we need to refresh the screen?
        if(refresh)
            wxGetApp().GetLayoutEditorFrame()->Refresh();	
    }
    //------------------------------------------------------------------------
    void DialogMain::SetWidgetsLook(StyleID newLook)
    {
        // Apply desired Look'N'Feel to all widgets in the layout
        const Widgets & wlist = N_Only(Widget).getList();
        Widgets::const_iterator i, iend = wlist.end();
        for(i = wlist.begin(); i != iend; ++i)
        {
            // Apply new Look'N'Feel to widget and move on to the next one
            SetWidgetLook(*i, newLook, false);
        }

        // Refresh the main frame so that the Look'N'Feel changes are immediately visible
        wxGetApp().GetLayoutEditorFrame()->Refresh();

        // Update current selection displayed properties and info, making sure the skin mapping is not
        // rebuilt since this operation did not affect the selection itself
        UpdateProperties(false);
    }
    //------------------------------------------------------------------------
}