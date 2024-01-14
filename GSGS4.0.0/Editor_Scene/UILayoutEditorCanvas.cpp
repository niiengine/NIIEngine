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

#include "UILayoutEditorCanvas.h"
#include "UILayoutDialogMain.h"
#include "UILayoutConfig.h"
#include "NiiUIWidgetManager.h"
#include "NiiGBufferManager.h"
#include "NiiPixelBufferManager.h"
#include "UILayoutEditorFrame.h"
#include "ExceptionManager.h"
#include "NiiUIMenubar.h"
#include "NiiUIStrConv.h"
#include "StrHelper.h"
#include "NiiEngine.h"

#include <wx/colordlg.h>
#include "app.h"

#define NULL_WINDOW_MSG wxT("Window may not be NULL here!")
//#define DELETE_ROOT_WARNING wxT("You can't delete the 'root' window.")
#define NO_BACKGROUND_WARNING wxT("There is currently no mapping loaded for a StaticImage. Therefore it is not possible to set a background image.")
#define ADD_WINDOW_HELPER 4

namespace gsgs
{
    // wxWidgets document/view stuff
    IMPLEMENT_DYNAMIC_CLASS(EditorDocument, wxDocument)
    //------------------------------------------------------------------------
    // EditorDocument construction/destruction
    EditorDocument::EditorDocument() :
        m_activeLayout(wx_static_cast(Window*, NULL)),
        m_observers(),
        m_hoveredResizePoint(RESIZE_POINT_NONE),
        m_previousHoveredResizePoint(RESIZE_POINT_NONE),
        m_mousePressed(false),

        // Not zero to avoid division exceptions
        m_width(1),
        m_height(1),

        m_mouseX(0),
        m_mouseY(0),
        m_previousMouseX(0),
        m_previousMouseY(0),
        mHover(wx_static_cast(Window*, NULL)),

        m_selection(),
        m_copiedSelection(),

        // Initially, no layout widgets are selected, so we have nothing to lock. 
        m_lockedSelection(false),
        wxDocument()
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

        // Clean up selection + resizers
        m_selection.ClearSelection();

        // Initially, no layout widgets are selected, so we have nothing to lock. 
        wxGetApp().GetLayoutEditorFrame()->UpdateLockedSelectionIcon(this);
    }
    //------------------------------------------------------------------------
    /** Handles the wxID_OPEN menu event.
    * The system will check whether modifications were made and give the option to save the
    * current layout first (if any).
    */
    bool EditorDocument::OnOpenDocument(const wxString& fileName)
    {
        UILayoutOptions* const options = UILayoutOptions::GetInstancePtr();
        // wx needs the full path
        wxString path = options->GetPathToLayouts();
        wxString fullPath = path + fileName;

        if (fileName.empty() || !wxDocument::OnOpenDocument(fullPath))
            return false;

        // Cleanup any current windows to avoid name clashes
        m_activeLayout = NULL;
        N_Only(Widget).destroyAll();

        try
        {
            // Layout will get loaded from the set path (see INI file)
            Window* const layout = static_cast<Window *>(N_Only(Widget).load(StrHelper::ToNIIString(fileName)));
            SetActiveLayout(layout);

            // Tell observers. Disabled because the view is already notified.
            for (size_t i = 0; i < m_observers.size(); ++i)
            {
                m_observers[i]->LayoutOpened(layout);
            }

            // Store this setting for application reload. Just the filename, because the path will get appended above!
            options->SetCurrentLayout(fileName);

            SetFilename(fullPath, true);
            SetTitle(fullPath); // no path
            return true;
        }
        catch (NII::Exception& e)
        {
            // Cleanup so far to avoid name clashes for correctly loaded windows
            N_Only(Widget).destroyAll();

            // Show exception info, composed of basic info (description, location, message), line and 
            // file info (GUI exception specific) and VEH info if under Windows (stack trace, exception code & flags)
            LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(wxT("EditorDocument::OnOpenDocument"), wxT("Unexpected Error"), StrHelper::ToWXString(e.getVerbose()), StrHelper::ToWXString(e.getFile()), e.getLine()));

            return false;
        }
    }
    //------------------------------------------------------------------------
    // Handles the wxID_CLOSE menu event.
    bool EditorDocument::OnCloseDocument()
    {
        // Reset any selection related stuff
        m_hoveredResizePoint = RESIZE_POINT_NONE;
        m_previousHoveredResizePoint = RESIZE_POINT_NONE;
        mHover = NULL;
        m_selection.ClearSelection();

        // Cleanup current windows to avoid name clashes
        N_Only(Widget).destroyAll();

        // Store this setting for application reload
        // COptions::getInstancePtr()->setCurrentLayout("");
        SetFilename(wxT(""), true);
        return true;
    }
    //------------------------------------------------------------------------
    // Handles the wxID_NEW menu event.
    bool EditorDocument::OnNewDocument()
    {
        wxDocument::OnNewDocument();

        Window* layout = NULL; // Might override with a root

        // Does the user want a fullscreen root?
        wxMessageDialog dlg(NULL, wxT("Do you want to start with a fullscreen root window?"), wxT("Add default root?"),
            wxYES_NO);
        if (dlg.ShowModal() == wxID_YES)
        {	// Start with fullscreen root
            try
            {
                layout = static_cast<Window*>(N_OnlyPtr(Widget)->create(N_WIDGET_DefaultWindow, NII::FastHash(_T("Root"))));
            }
            // If something goes wrong, show user and return false
            SHOW_EXCEPTION_RETURN("EditorDocument::OnNewDocument", "Error", "Unexpected standard exception while adding new document", "Unexpected non-standard exception while adding new document", false);
        }

        SetActiveLayout(layout);

        // Tell observers
        for (size_t i = 0; i < m_observers.size(); ++i)
        {
            m_observers[i]->LayoutStarted(layout);
        }

        EditorDocument::Modify(true);
        return true;
    }
    //------------------------------------------------------------------------
    Widget * EditorDocument::AddWindow(Container* parentWidget, StyleViewID aType, const String & aName)
    {
        // Lookup the given name and keep numbering until unique.
        String newName = aName;
        int nCounter = 1;

        while (N_Only(Widget).isExist(newName))
        {
            wxChar p_chNamePostfix[100];
            (void)wxSnprintf(p_chNamePostfix, 100, wxT("%d"), nCounter++);
            newName = aName + StrHelper::ToNIIString(p_chNamePostfix);
        }

        try
        {
            if (!parentWidget)
            {
                LogDebug(wxT("EditorDocument::addWindow - create first window: '%s'(%s)."),
                    StrHelper::ToWXString(newName).c_str(), StrHelper::ToWXString(N_conv(aType)).c_str());
            }
            else
            {	// Print info about parent and newly to create window.
                LogDebug(wxT("EditorDocument::addWindow - parent: '%s'(%s), new child: '%s'(%s)."),
                    StrHelper::ToWXString(parentWidget->getName()).c_str(), 
                    StrHelper::ToWXString(N_conv(parentWidget->getType())).c_str(), 
                    StrHelper::ToWXString(newName).c_str(), StrHelper::ToWXString(N_conv(aType)).c_str());
            }

            // Create the window
            Widget* newWindow = N_Only(Widget).create(aType, 0);
            newWindow->setName(newName);
            InitNewWindow(newWindow, parentWidget);

            // When no parent is given, the new window will become the first window (root)
            if (!parentWidget)
            {
                SetActiveLayout(dynamic_cast<Window *>(newWindow));
            }
            else
            {
                parentWidget->add(newWindow);
            }

            // Tell observers
            for (size_t i = 0; i < m_observers.size(); ++i)
            {
                m_observers[i]->WindowAdded(newWindow);
            }

            // Select it
            SelectWindow(newWindow);
            EditorDocument::Modify(true);
            return newWindow;
        }
        // If something goes wrong, show user and return NULL
        SHOW_EXCEPTION_RETURN("EditorDocument::AddWindow", "Error", "Unexpected standard exception while adding new window", "Unexpected non-standard exception while adding new window", NULL);
    }
    //------------------------------------------------------------------------
    void EditorDocument::InitNewWindow(Widget* newWidget, Container* parentWidget)
    {
        // Some initializing. TabControls get default pre-population
        if (newWidget->getType() == N_WIDGET_TabControl)
        {
            InitTabControl(dynamic_cast<TabControl *>(newWidget));
        }
        else if (newWidget->getType() == N_WIDGET_Menubar)
        {
            //InitMenubar(newWidget);
            // If we add a MenuBar, we need to position and size it for a bit.
            newWidget->setPos(RelVector2f(RelPosf(0.0f, 0.0f), RelPosf(0.12f, 0.0f)));
            newWidget->setSize(RelPlaneSizef(RelPosf(1.0f, 0.0f), RelPosf(0.1f, 0.0f)));
        }
        // Some things can only be done with a parent (already checked in the AddWindowDlg)
        if (parentWidget)
        {	// Menus need some specific handling...
            if (newWidget->getType() == N_WIDGET_MenuItem)
            {	// Just put some text in the caption
                newWidget->setText(_T("MenuItem"));
                if (parentWidget->getType() == N_WIDGET_MenuItem)
                {   // We never explicitly add PopupMenus; when the parent is a MenuItem, we have two options:
                    // 1. When the parent item does not have a PopupMenu yet, we automagically add a PopupMenu and the new item
                    // 2. When the parent item already has a PopupMenu, we add the new item to that popupmenu
                    PopupMenu* popup = NULL;
                    // (This cast to MenuItem is safe now)
                    if (!(wx_static_cast(MenuItem*, parentWidget))->getDest())
                    {   // Create popup menu for this item
                        popup = wx_static_cast(PopupMenu*, N_Only(Widget).create(newWidget->getStyleViewGroup() + N_WIDGET_PopupMenu, 0));
                        popup->setName(parentWidget->getName() + _T("/AutoPopup"));
                        (wx_static_cast(MenuItem*, parentWidget))->setDest(popup);
                    }
                    else
                    {   // Retrieve popup menu for this item
                        popup = (wx_static_cast(MenuItem*, parentWidget))->getDest();
                    }
                    // Tricky part: because we put the new window into a helper popup menu, we don't have to add to too
                    // the parent as well!
                    popup->add(newWidget);
                }
            }
            // Same for tab pages: we default them to 1,1 (relative)
            // DefaultGUISheet gets classname "DefaultWindow"!
            if (newWidget->getType() == N_WIDGET_DefaultWindow && parentWidget->getType() == N_WIDGET_TabControl)
            {
                newWidget->setText(_T("Tab"));
                newWidget->setSize(RelPlaneSizef(RelPosf(1.0f, 0.0f), RelPosf(1.0f, 0.0f)));
            }
            // Except for menuitems (whos size depens on the text), we create a default position and size.
            else if (newWidget->getType() != N_WIDGET_MenuItem)
            {   // Set initial (relative) position in the usual top-left to bottom-right apprauch
                // Make the size a bit fair to the parent size, and wrap around maximum ADD_WINDOW_HELPER
                const float newStartXY = 0.2f + ((parentWidget->getCount() ? parentWidget->getCount() : 1) % ADD_WINDOW_HELPER) * 0.1f;

                // No metrics mode anymore
                newWidget->setPos(RelVector2f(RelPosf(newStartXY, 0.0f), RelPosf(newStartXY, 0.0f)));
                newWidget->setSize(RelPlaneSizef(RelPosf(1.0f / ADD_WINDOW_HELPER, 0.0f), RelPosf(1.0f / ADD_WINDOW_HELPER, 0.0f)));
            }
        }
        else
        {	// No parent, set a position
            newWidget->setPos(RelVector2f(RelPosf(0.2f, 0.0f), RelPosf(0.2f, 0.0f)));
            newWidget->setSize(RelPlaneSizef(RelPosf(0.25f, 0.0f), RelPosf(0.25f, 0.0f)));
        }

        // Apply the default font (if set)
        const wxString & defaultFont = UILayoutOptions::GetInstancePtr()->GetDefaultFont();
        if (defaultFont != wxT(""))
        {
            newWidget->setPropertyValue(N_PropertyUIWidget_Font, StrHelper::ToNIIString(defaultFont));
        }
    }
    //------------------------------------------------------------------------
    void EditorDocument::InitTabControl(TabControl* widget)
    {
        // Validations
        wxASSERT_MSG(widget != NULL, wxT("Supplied tab control is NULL"));

        #define TAB_COUNT 4

        const wxChar* const tabCaptions[TAB_COUNT] = { wxT("Global"), wxT("Team"), wxT("Friends"), wxT("Admin") };
        wxChar tabIndex[10];
        // Simulate a chat window with mutiple channels.
        if (widget->getType() == N_WIDGET_TabControl)
        {
            Window *newTab;
            const String & controlName = widget->getName();

            for (int i = 0; i < TAB_COUNT; i++)
            {
                // Just call our helper method
                (void)wxSprintf(tabIndex, wxT("/Tab %d"), i + 1);
                // Create the new tab
                newTab = static_cast<Window *>(N_Only(Widget).create(widget->getStyleViewGroup(), N_WIDGET_DefaultWindow, N_VIEW_FrameWindow, 0, widget));
                newTab->setName(controlName + StrHelper::ToNIIString(tabIndex));
                InitNewWindow(newTab, widget);
                newTab->setText(StrHelper::ToNIIString(tabCaptions[i]));
                widget->addTab(newTab);
            }
        }
    }
    //------------------------------------------------------------------------
    void EditorDocument::InitMenubar(Window* aMenubar)
    {
        // Validations
    /*    wxASSERT_MSG(aMenubar != NULL, wxT("Supplied menu bar is NULL"));

        #define ITEM_COUNT 17

        const wxChar* const menuCaptions[ITEM_COUNT] = {wxT("_root_"), wxT("File"), wxT("New"), wxT("Open"), wxT("Close"), wxT("Save"), wxT("Exit"),
            wxT("_root_"), wxT("Edit"), wxT("Cut"), wxT("Copy"), wxT("Paste"), wxT("Delete"),
            wxT("_root_"), wxT("Help"), wxT("Contents..."), wxT("About...")};
        wxChar itemIndex [10];
        // Simulate a simple menu
        if (aMenubar->testClassName("Menubar"))
        {
            Window* currentRoot = NULL;
            Window* newItem = NULL;
            const String controlName = aMenubar->getName();
            const String look = aMenubar->getStyle().substr(0, aMenubar->getStyle().find("/"));
            int i=0;
            while (i<ITEM_COUNT)
            {
                (void)wxSprintf(itemIndex, wxT("/Item%d"), i+1);
                if(wxStrcmp(menuCaptions[i], wxT("_root_")) == 0)
                {
                    // New root item
                    currentRoot = WindowManager::getSingleton().createWindow (look + String ("/MenuItem"), controlName + StrHelper::ToNIIString(itemIndex));
                    InitNewWindow(currentRoot, aMenubar);
                    aMenubar->addChildWindow(newItem);
                    newItem = currentRoot;
                    i++;    // Don't use _root_ as captions.
                }
                else
                {   // Add to current root item
                    if(currentRoot)
                    {
                        newItem = WindowManager::getSingleton().createWindow (look + String ("/MenuItem"), currentRoot->getName() + StrHelper::ToNIIString(itemIndex));
                        InitNewWindow(newItem, currentRoot);
                        currentRoot->addChildWindow(newItem);
                    }
                    else
                    {
                        // Something went wrong
                        LogError(wxT("EditorDocument::InitMenubar - Root window not found!"));
                        return;
                    }
                }

                // Safety-Check: If a root item is set as the last menu caption (which
                // would be a mistake), it could lead to an out-of-bounds access.
                if(i<ITEM_COUNT)
                    newItem->setText(StrHelper::ToNIIString(menuCaptions[i]));
                i++;    // next
            }
        }*/
    }
    //------------------------------------------------------------------------
    bool EditorDocument::RemoveWindow(Widget * widget, bool notifyListeners)
    {
        // You may not remove the root window, otherwise many other stuff might go wrong ;-)
    /*    if (widget == m_activeLayout)
        {
            LogWarning (DELETE_ROOT_WARNING);
            // Don't delete or unselect then.
            return false;
        }*/

        // Remove froms selection too (if selected)
        m_selection.Unselect(widget);

        // (Allows NULL)
        N_Only(Widget).destroy(widget);

        if (widget == m_activeLayout)
        {
            SetActiveLayout(NULL);
        }
        if (notifyListeners)
        {
            // Tell listeners
            for (size_t i = 0; i < m_observers.size(); ++i)
            {
                m_observers[i]->WindowsRemoved();
            }
        }

        EditorDocument::Modify(true);
        return true;
    }
    //------------------------------------------------------------------------
    void EditorDocument::SetActiveLayout(Window* aLayout)
    {
        // Store & tell Cegui
        m_activeLayout = aLayout;
        N_Only(UI).getSheet(N_Engine().getView())->setActive(aLayout);
    }
    //------------------------------------------------------------------------
    void EditorDocument::DeactivateCurrentLayout()
    {
        if (m_activeLayout)
        {
            m_activeLayout = NULL;
        }
    }
    //------------------------------------------------------------------------
    void EditorDocument::AddObserver(IDocumentObserver* anObserver)
    {
        m_observers.push_back(anObserver);
    }
    //------------------------------------------------------------------------
    void EditorDocument::Reset()
    {
        m_hoveredResizePoint = RESIZE_POINT_NONE;
        m_previousHoveredResizePoint = RESIZE_POINT_NONE;
        mHover = NULL;
        m_activeLayout = NULL;
        m_mousePressed = false;
        m_mouseX = 0;
        m_mouseY = 0;
        m_previousMouseX = 0;
        m_previousMouseY = 0;

        // Not zero to avoid devision exceptions
        m_width = 1;
        m_height = 1;

        // Clean up selection + resizers
        m_selection.ClearSelection();

        // Initially, no layout widgets are selected, so we have nothing to lock. 
        m_lockedSelection = false;
        wxGetApp().GetLayoutEditorFrame()->UpdateLockedSelectionIcon(this);
    }
    //------------------------------------------------------------------------
    void EditorDocument::HandleMouseMoved(int aMouseX, int aMouseY)
    {
        COMPILE_TODO("view->GetSelection(); or something.")
            SelectionMover mover(&m_selection);

        wxChar statusInfo[100];
        bool bChanges = false;
        float pixelDeltaX, pixelDeltaY;

        // Copy parameters into members
        m_mouseX = aMouseX;
        m_mouseY = aMouseY;

        // Minimum status bar text
        (void)wxSnprintf(statusInfo, 100, wxT("[%d, %d][%3.2f, %3.2f]"), aMouseX, aMouseY, wx_static_cast(double, aMouseX) / m_width, wx_static_cast(double, aMouseY) / m_height);

        // Only proceed when the mouse is pressed
        if (m_mousePressed)
        {
            // Calculate both value-type, so the code below can choose,
            // based on the metric modes of the elements.
            (void)wxSnprintf(statusInfo, 100, wxT("%s - dragging"), statusInfo);

            // Calculate pixel differences with the previous mouse position.
            pixelDeltaX = wx_static_cast(float, m_mouseX - m_previousMouseX);
            pixelDeltaY = wx_static_cast(float, m_mouseY - m_previousMouseY);

            if (m_hoveredResizePoint != RESIZE_POINT_NONE)
            {
                // The mouse is pressed on a resize point; resize each selected window accordingly.
                switch (m_hoveredResizePoint)
                {
                case RESIZE_POINT_WN:
                    // Upper left
                    mover.UpdateWindowAreas(pixelDeltaX, pixelDeltaY, 0.0f, 0.0f);
                    break;
                case RESIZE_POINT_N:
                    // Upper middle
                    mover.UpdateWindowAreas(0.0f, pixelDeltaY, 0.0f, 0.0f);
                    break;
                case RESIZE_POINT_NE:
                    // Upper right
                    mover.UpdateWindowAreas(0.0f, pixelDeltaY, pixelDeltaX, 0.0f);
                    break;
                case RESIZE_POINT_E:
                    // Middle right
                    mover.UpdateWindowAreas(0.0f, 0.0f, pixelDeltaX, 0.0f);
                    break;
                case RESIZE_POINT_ES:
                    // Bottom right
                    mover.UpdateWindowAreas(0.0f, 0.0f, pixelDeltaX, pixelDeltaY);
                    break;
                case RESIZE_POINT_S:
                    // Bottom middle
                    mover.UpdateWindowAreas(0.0f, 0.0f, 0.0f, pixelDeltaY);
                    break;
                case RESIZE_POINT_SW:
                    // Bottom left
                    mover.UpdateWindowAreas(pixelDeltaX, 0.0f, 0.0f, pixelDeltaY);
                    break;
                case RESIZE_POINT_W:
                    // Middle left
                    mover.UpdateWindowAreas(pixelDeltaX, 0.0f, 0.0f, 0.0f);
                    break;
                default:
                    break;
                }
                bChanges = true;
            }
            else if (mHover)
            {
                // The mouse is pressed inside a window; drag entire selection.
                // We drag, so the size doesn't change. Therefore we pass the same two parameters for the right bottom values.
                mover.UpdateWindowAreas(pixelDeltaX, pixelDeltaY, pixelDeltaX, pixelDeltaY);
                bChanges = true;
            }

            if (bChanges)
            {
                EditorDocument::Modify(true);
            }
        }

        else
        {	// Mouse not pressed; find hovered resize point or window
            if (m_activeLayout)
            {
                Selection::Boxes & boxes = m_selection.GetAllBoxes();
                for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
                {
                    m_hoveredResizePoint = boxIt->GetPointAtPosition(Vector2f(m_mouseX, m_mouseY));
                    if (m_hoveredResizePoint != RESIZE_POINT_NONE)
                    {
                        break;
                    }
                }

                if (m_hoveredResizePoint == RESIZE_POINT_NONE)
                {
                    // Didn't find resize point, try window
                    mHover = static_cast<Window*>(m_activeLayout->get(Vector2f(m_mouseX, m_mouseY)));
                }

                // Still null? Check if the mouse is in the root window
                if (!mHover && m_activeLayout == N_Only(UI).getSheet(N_Engine().getView())->getActive())
                {
                    Rectf pixelRc = m_activeLayout->getIntersectArea();
                    if (pixelRc.intersects(Vector2f(m_mouseX, m_mouseY)))
                    {
                        mHover = m_activeLayout;
                    }
                }
            }
        }

        // Hovering is exclusive; either a resize point or a window. Never both.
        if (m_hoveredResizePoint != RESIZE_POINT_NONE)
        {
            mHover = NULL;
            (void)wxSnprintf(statusInfo, 100, wxT("%s ResizePoint: %d"), statusInfo, m_hoveredResizePoint);
        }
        else if (mHover != NULL)
        {
            m_hoveredResizePoint = RESIZE_POINT_NONE;
            (void)wxSnprintf(statusInfo, 100, wxT("%s Window: %s"), statusInfo, StrHelper::ToWXString(mHover->getName()).c_str());
            // Selectable?
            if (mHover->getName().find(_T("__auto_")) != String::npos || mHover->getName().find(_T("__TabPane__")) != String::npos)
            {
                // No, add to status info
                (void)wxSnprintf(statusInfo, 100, wxT("%s (not selectable)"), statusInfo);
            }
        }

        UpdateStatusBar(statusInfo);
        UpdateCursor();

        // Reset previous values
        m_previousMouseX = m_mouseX;
        m_previousMouseY = m_mouseY;
    }
    //------------------------------------------------------------------------
    void EditorDocument::HandleMousePressed(const int WXUNUSED(aMouseX), const int WXUNUSED(aMouseY), const bool aMustReset)
    {
        // An overlay must be open
        if (!m_activeLayout)
        {
            return;
        }

        if (mHover)
        {
            LogDebug(wxT("EditorDocument::HandleMousePressed - '%s'"), StrHelper::ToWXString(mHover->getName()).c_str());
            // Helper methods
            if (!PressedTabButton() && !PressedMenuItem())
            {
                while (mHover->getName().find(_T("__auto_")) != String::npos ||
                    mHover->getName().find(_T("__TabPane__")) != String::npos)
                {   // Current window is an __auto_, goto parent (if any)
                    if (mHover->getParent())
                    {
                        mHover = mHover->getParent();
                    }
                }
            }
            LogDebug(wxT("EditorDocument::HandleMousePressed - pressed: '%s'"), StrHelper::ToWXString(mHover->getName()).c_str());
            // Call helper method
            SelectWindow(mHover, aMustReset);
        }
        else
        {
            COMPILE_TODO("Move this to a seperate helper");
            //HandleColoursUpdate(aMustReset);
        }
        // Save this state for during mouse movements
        m_mousePressed = true;
    }
    //------------------------------------------------------------------------
    // tuan.kuranes@free.Fr
    bool EditorDocument::PressedTabButton()
    {
        // Validations
        wxASSERT_MSG(mHover != NULL, wxT("Hovered window is NULL"));

        // Find tab window (parent parent of a tab button)
        // Then find the tab pane corresponding to the button clicked.
        // Hide Other TabPane, show Clicked tabpane.
        const String & name = mHover->getName();    // For a tab button, this becomes "__auto_btn<MyPageName>".

        // Here we test whether a tab button was selected. Selecting a different page happens in the same way
        // as with a run-time tabcontrol.
        const size_t posInString = name.find(_T("__auto_btn"));
        if (posInString != String::npos &&
            mHover->getParent() &&     // For a tab button, parent is a <MyTabControlName>__auto_TabPane__Buttons.
            mHover->getParent()->getName().find(_T("__auto_TabPane__Buttons")) != String::npos)
        {
            LogDebug(wxT("You clicked a tab button."));
            mHover = mHover->getParent()->getParent();    // This is <MyTabControlName>
            if (mHover)
            {
                Container * tempw = static_cast<Container *>(mHover);
                NCount hoverChildCnt = tempw->getCount();

                Widget *newWin = 0;

                // We have "walked up" the TabControl; now find <MyTabControlName>__auto_TabPane__,
                // which contains the content pages which correspond to the buttons.
                for (NCount i = 0; i < hoverChildCnt; i++)
                {
                    newWin = tempw->get(i);
                    if (newWin->getName().find(_T("__auto_TabPane__Buttons")) != String::npos)
                        continue;
                    if (newWin->getName().find(_T("__auto_TabPane_")) != String::npos)
                    {   // Found it!
                        mHover = newWin;
                        tempw = static_cast<Container *>(newWin);
                        break;
                    }
                }
                if (mHover == newWin && newWin)
                {
                    // then find the tab pane corresponding to the button clicked.
                    LogDebug(wxT("Name: ") + StrHelper::ToWXString(name));
                    String autoButton(_T("__auto_btn"));
                    size_t endAutoPos = name.rfind(autoButton) + autoButton.length();

                    String searchedTabName = name.substr(endAutoPos);   // The button name minus "__auto_btn" is the content pane we need.
                
                    NCount size = tempw->getCount();
                    for (size_t i = 0; i < size; i++)
                    {
                        Widget *searchedTab = static_cast<Container *>(newWin)->get(i);
                        if (searchedTab->getName().find(searchedTabName) != String::npos)
                        {
                            mHover = searchedTab;
                        }
                        searchedTab->setVisible(false);
                    }
                    // Others TabPane are now hidden; show Clicked tabpane.
                    mHover->setVisible(true);
                    return true;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool EditorDocument::PressedMenuItem()
    {
        // Validations
        wxASSERT_MSG(mHover != NULL, wxT("Hovered Window is NULL"));

        if (mHover->getType() == N_WIDGET_MenuItem)
        {   // When this item is a root item, meaning that it is a direct child of the Menubar, 
            // the Menubar itself takes care of hiding the previous one :-)
            // (The menu item will check for a NULL situation.)
            (wx_static_cast(MenuItem*, mHover))->openPopupMenu();
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void EditorDocument::HandleColoursUpdate(const bool shiftPressed) const
    {
        // currently selected (GUI)Window
        Widget * const win = wxGetApp().GetLayoutEditorFrame()->GetDialogMain()->GetCurrentWindow();
        if (win != NULL && shiftPressed && m_hoveredResizePoint != -1 && win->isPropertyExist(N_PropertyUIWidget_TextColour))
        {
            // get the color rect
            ColourArea rect;
            UI::StrConv::conv(win->getPropertyValue(N_PropertyUIWidget_TextColour), rect);

            wxColourData cd;
            cd.SetChooseFull(true);

            // and add it to the dialog's custom colors
            cd.SetCustomColour(0, wxColour(
                wx_static_cast(wxColourBase::ChannelType, rect.mTL.getRed() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.mTL.getGreen() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.mTL.getBlue() * 255)));
            cd.SetCustomColour(1, wxColour(
                wx_static_cast(wxColourBase::ChannelType, rect.mTR.getRed() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.mTR.getGreen() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.mTR.getBlue() * 255)));
            cd.SetCustomColour(2, wxColour(
                wx_static_cast(wxColourBase::ChannelType, rect.mBR.getRed() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.mBR.getGreen() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.mBR.getBlue() * 255)));
            cd.SetCustomColour(3, wxColour(
                wx_static_cast(wxColourBase::ChannelType, rect.mBL.getRed() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.mBL.getGreen() * 255),
                wx_static_cast(wxColourBase::ChannelType, rect.mBL.getBlue() * 255)));

            // set the default color
            switch (m_hoveredResizePoint)
            {
            case RESIZE_POINT_WN:
                // Upper left
                cd.SetColour(wxColour(
                    wx_static_cast(wxColourBase::ChannelType, rect.mTL.getRed() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mTL.getGreen() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mTL.getBlue() * 255)));
                break;
            case RESIZE_POINT_NE:
                // Upper right
                cd.SetColour(wxColour(
                    wx_static_cast(wxColourBase::ChannelType, rect.mTR.getRed() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mTR.getGreen() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mTR.getBlue() * 255)));
                break;
            case RESIZE_POINT_ES:
                // Bottom right
                cd.SetColour(wxColour(
                    wx_static_cast(wxColourBase::ChannelType, rect.mBR.getRed() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mBR.getGreen() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mBR.getBlue() * 255)));
                break;
            case RESIZE_POINT_SW:
                // Bottom left
                cd.SetColour(wxColour(
                    wx_static_cast(wxColourBase::ChannelType, rect.mBL.getRed() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mBL.getGreen() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mBL.getBlue() * 255)));
                break;
            default:
                // default we get from the Upper left of the rect
                cd.SetColour(wxColour(
                    wx_static_cast(wxColourBase::ChannelType, rect.mTL.getRed() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mTL.getGreen() * 255),
                    wx_static_cast(wxColourBase::ChannelType, rect.mTL.getBlue() * 255)));
                break;
            }
            wxColourDialog dlg(wxGetApp().GetLayoutEditorFrame(), &cd);
            // show the color dialog
            if (dlg.ShowModal() != wxID_OK)
                return;

            wxColour &clr = dlg.GetColourData().GetColour();
            // and convert the data
            NII::Colour cl;
            cl.setRed(clr.Red() / 255.0f);
            cl.setGreen(clr.Green() / 255.0f);
            cl.setBlue(clr.Blue() / 255.0f);

            // set up the alpha, then switch the color
            switch (m_hoveredResizePoint)
            {
            case RESIZE_POINT_WN:
                // Upper left
                cl.setAlpha(rect.mTL.getAlpha());
                rect.mTL = cl;
                break;
            case RESIZE_POINT_NE:
                // Upper right
                cl.setAlpha(rect.mTR.getAlpha());
                rect.mTR = cl;
                break;
            case RESIZE_POINT_ES:
                // Bottom right
                cl.setAlpha(rect.mBR.getAlpha());
                rect.mBR = cl;
                break;
            case RESIZE_POINT_SW:
                // Bottom left
                cl.setAlpha(rect.mBL.getAlpha());
                rect.mBL = cl;
                break;
            default:
                rect = cl;
                break;
            }
            // finally we apply the whole color rect, and refresh
            win->setPropertyValue(N_PropertyUIWidget_TextColour, NUI_conv(rect));
            wxGetApp().GetLayoutEditorFrame()->Refresh();
        }
    }
    //------------------------------------------------------------------------
    void EditorDocument::HandleMouseReleased()
    {
        const bool mustNotify = (m_hoveredResizePoint != RESIZE_POINT_NONE || mHover);
        m_mousePressed = false;
        m_hoveredResizePoint = RESIZE_POINT_NONE;
        mHover = NULL;
        if (mustNotify)
        {
            for (size_t i = 0; i < m_observers.size(); ++i)
            {
                m_observers[i]->SelectionDraggedOrResized();
            }
        }
    }
    //------------------------------------------------------------------------
    void EditorDocument::SelectWindow(Widget* widget, bool aMustReset)
    {
        // Is the current selection locked? If so, we can't make changes.
        if (m_lockedSelection)
        {
            // Verify if there is anything currently selected and if so ignore the operation (locked). 
            // Otherwise, remove the lock.
            if (m_selection.Size() == 0)
            {
                // The user unselected the last selection element (either by a cut, unselect or delete
                // operation). We need to remove the lock and update the toolbar lock icon to its
                // unlock state
                m_lockedSelection = false;
                wxGetApp().GetLayoutEditorFrame()->UpdateLockedSelectionIcon(this);
            }
            else
            {	// The current selection is locked, so we return immediately
                return;
            }
        }

        // Validations
        if (widget == NULL)
        {
            LogError(wxT("Cannot select a NULL Window. Please contact support."));
            return;
        }

        // Simulate common selection behaviour:
        //  When something is selected, and SHIFT is not pressed, undo the entire current selection
        if (aMustReset)
        {
            m_selection.ClearSelection();
        }

        // Either the new window or the first window (when the given window was unselected)
        Widget * newSelectedWindow = NULL;

        // Add (first) window.
        if (m_selection.Select(widget))
        {
            newSelectedWindow = widget;
            // Feedback. Mantis ticket #34
            UpdateStatusBar(wxT("Selected a '%s'"), StrHelper::ToWXString(N_conv(widget->getType())).c_str());
            // Lock?
            if (RequiresLock(widget))
            {	// Block changing of position and size.
                m_selection.FindBoxFor(widget)->Lock();
            }
        }
        else
        {	// We unselected the window. Use the first selection for the notification.
            if (m_selection.Size() > 0)
            {	// Ptr to window inside the box
                newSelectedWindow = m_selection.GetAllBoxes().begin()->GetWindow();
            }
            // else stays NULL
            UpdateStatusBar(wxT("Unselected '%s'"), StrHelper::ToWXString(N_conv(widget->getType())).c_str());
        }
        // Tell observers about the selection
        for (size_t i = 0; i < m_observers.size(); ++i)
        {
            m_observers[i]->WindowSelected(newSelectedWindow);
        }


        // Do we have a valid new selection?
        // NOTE: This can be null if the user unselected the top/last window
        if (newSelectedWindow)
        {
            LogDebug(wxT("EditorDocument::SelectWindow - Newselected: '%s'"), StrHelper::ToWXString(newSelectedWindow->getName()).c_str());

            // tuan.kuranes@free.fr
            // if selecting from dialog tree
            // an invisible item, put it back to visible
            if (!newSelectedWindow->isVisible())
            {
                Container *currWindow = dynamic_cast<Container *>(newSelectedWindow);
                while (currWindow && !currWindow->isVisible())
                {
                    // find tab window (parent of current window)
                    if (currWindow->getParent() &&
                        currWindow->getParent()->getName().find(_T("__auto_TabPane__")) != String::npos)
                    {
                        // hide brother tab
                        NCount size = currWindow->getParent()->getCount();
                        for (NCount parentChild = 0; parentChild < size; ++parentChild)
                        {
                            currWindow->getParent()->get(parentChild)->setVisible(false);
                        }

                        // show all tab children 
                        size = currWindow->getCount();
                        for (NCount child = 0; child < size; ++child)
                        {
                            currWindow->get(child)->setVisible(true);
                        }
                    }
                    currWindow->setVisible(true);
                    currWindow = currWindow->getParent();
                } // while
            } // if 
        } // if

        m_selection.Update();
        wxDocument::UpdateAllViews();
    }
    //------------------------------------------------------------------------
    void EditorDocument::SelectAll()
    {
        // Not when locked
        if (!m_lockedSelection)
        {
            // [quote by Van] "Don't get smarter then your users" [/quote]
            // So a Select All is a select all, including the root sheet and such.
            // (Which actually makes sence in case the user want to move everythin into a fresh layout)
            Window* root = N_Only(UI).getSheet(N_Engine().getView())->getActive();
            if (root)
            {
                m_selection.SelectAll(root);
            }

            EditorDocument::Modify(true);
        }
    }
    //------------------------------------------------------------------------
    bool EditorDocument::RequiresLock(const Widget* widget) const
    {
        // Validations
        wxASSERT_MSG(widget != NULL, wxT("Supplied window is NULL"));

        //LogDebug ( wxT("MenuItem: %d"), (int) widget->testClassName("MenuItem"));
        //LogDebug ( wxT("DefaultWindow: %d"), (int) widget->testClassName("DefaultWindow"));
        //LogDebug ( wxT("ParentType: %s"), StrHelper::ToWXString(widget->getParent()->getType()));
        return (widget->getType() == N_WIDGET_MenuItem ||
            (widget->getType() == N_WIDGET_DefaultWindow && widget->getParent() && 
                widget->getParent()->getType() == N_WIDGET_TabControl));
    }
    //------------------------------------------------------------------------
    void EditorDocument::UpdateStatusBar(const wxChar *format, ...) const
    {
        wxChar buffer[1024];

        // Initialize variable arguments list
        va_list arguments;
        va_start(arguments, format);
        // Create the status bar text
        (void)wxVsnprintf(buffer, sizeof(buffer), format, arguments);
        // Cleanup arguments.
        va_end(arguments);

        wxGetApp().GetLayoutEditorFrame()->GetStatusBar()->SetStatusText(buffer);
    }
    //------------------------------------------------------------------------
    void EditorDocument::UpdateCursor()
    {
        wxWindow* const frame = wxGetApp().GetLayoutEditorFrame();
        if (m_hoveredResizePoint != m_previousHoveredResizePoint)
        {
            //LogDebug(wxT("Updating cursor."));
            switch (m_hoveredResizePoint)
            {
            case RESIZE_POINT_WN:
                // Upper left
                (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENWSE));
                break;
            case RESIZE_POINT_N:
                // Upper middle
                (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENS));
                break;
            case RESIZE_POINT_NE:
                // Upper right
                (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENESW));
                break;
            case RESIZE_POINT_E:
                // Middle right
                (void)frame->SetCursor(wxCursor(wxCURSOR_SIZEWE));
                break;
            case RESIZE_POINT_ES:
                // Bottom right
                (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENWSE));
                break;
            case RESIZE_POINT_S:
                // Bottom middle
                (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENS));
                break;
            case RESIZE_POINT_SW:
                // Bottom left
                (void)frame->SetCursor(wxCursor(wxCURSOR_SIZENESW));
                break;
            case RESIZE_POINT_W:
                // Middle left
                (void)frame->SetCursor(wxCursor(wxCURSOR_SIZEWE));
                break;
            default:
                // Default cursor
                (void)frame->SetCursor(wxCursor(wxCURSOR_ARROW));
                break;
            }
        }
        m_previousHoveredResizePoint = m_hoveredResizePoint;
    }
    //-----------------------------------------------------------------------
    void EditorDocument::GetRelevantProperties(RelevantProperties* commProp, RelevantProperties* noncommProp,
        const bool onlyCommon)
    {
        // Validations
        wxASSERT_MSG(commProp != NULL, wxT("Supplied common properties are NULL"));
        wxASSERT_MSG(noncommProp != NULL, wxT("Supplied non common properties are NULL"));

        // Make sure no properties exist initially (safety-check)
        commProp->clear();
        noncommProp->clear();

        // Obtain the first window in the current selection
        Selection::Boxes::iterator boxIt = m_selection.GetAllBoxes().begin();
        const Widget *curWidget = (*boxIt).GetWindow();

        // No window exists, so the user hasn't selected anything (shouldn't really happen)
        if (!curWidget)
            return;

        try
        {
            // Iterate through all the properties of the first window in order to mark them
            // as common properties for starters
            // NOTE: getPropertyIterator() makes sure the compiler doesn't confuse EventSet and 
            // PropertySet (it would if we used getIterator()) thus avoiding useless casts
            const PropertyCmdMap & temp = curWidget->getProperty()->getList();
            PropertyCmdMap::const_iterator i, iend = temp.end();
            for (i = temp.begin(); i != iend; ++i)
            {
                // Obtain current property name and store it
                // NOTE: We store each property in the common properties map initially because we start
                // by assuming that every property present in the first element in the selection
                // will be common to EVERY other element in the selection. Of course, if we find this assumption
                // to be erroneous later on, we delete it from the common map and optionally move the 
                // affected property to the non common properties map (if onlyCommon parameter 
                // is set to false). 
                // This method also guarantees that single selection is treated in the same way as 
                // multi-selection since every property in a single selection will be common (only one 
                // widget selected!)
                PropertyID propertyName = i->first;
                commProp->insert(std::make_pair(propertyName, curWidget->getPropertyValue(propertyName)));
            }

            // Scan the rest of the selection (if it exists)
            for (; boxIt != m_selection.GetAllBoxes().end(); ++boxIt)
            {
                // Obtain current widget in the selection
                curWidget = (*boxIt).GetWindow();

                // NOTE: Must be done before the other iteration
                // Are we to process only the common properties in the current selection?
                if (!onlyCommon)
                {
                    // No, so we need to add all the new properties of the current window
                    // that are not present in the first window to the non common property map      
                    // NOTE: getPropertyIterator() makes sure the compiler doesn't confuse EventSet and 
                    // PropertySet (it would if we used getIterator()) thus avoiding useless casts
                    const PropertyCmdMap & temp2 = curWidget->getProperty()->getList();
                    iend = temp.end();
                    for (i = temp2.begin(); i != iend; ++i)
                    {
                        // Obtain current property name
                        const PropertyID propertyName = i->first;

                        // If the property isn't present in the common property map, we add
                        // it to the non-common property map since there is at least one 
                        // window (the first one) that doesn't contain it
                        const RelevantPropertiesIterator propIter = commProp->find(propertyName);
                        if (propIter == commProp->end())
                            noncommProp->insert(std::make_pair(propertyName, curWidget->getPropertyValue(propertyName)));
                    }

                    // The other task that remains for us to do is to check the non 
                    // common property map and for every property whose value is not
                    // common between all elements in the selection, mark it as empty.
                    // NOTE: If the property isn't present in a window, it isn't
                    // considered. This effectively means that if we have 3 windows
                    // selected, and the first and third one contain property X with
                    // value Y, and the second one doesn't contain it, the displayed
                    // value with still be Y. If the third window had value Z instead,
                    // the displayed value would be empty
                    for (RelevantPropertiesIterator iter = noncommProp->begin(); iter != noncommProp->end();)
                    {
                        // Obtain non-common property name
                        const PropertyID propertyName = (*iter).first;

                        // Does this property exist in this window? If not, ignore and move to
                        // the next one
                        if (curWidget->isPropertyExist(propertyName))
                        {
                            // Property exists, so we need to make sure its value (stored in the non-common
                            // property map) is the same as the one assigned to the current window
                            const String propertyValue = (*iter).second;
                            const String currentWindowPropertyValue = curWidget->getPropertyValue(propertyName);
                            if (propertyValue != currentWindowPropertyValue)
                                // Values are different, so set property as empty
                                (*iter).second = _T("");
                        }

                        // Move on to the next property
                        ++iter;
                    }
                }

                // We now need to iterate through the common properties of the first window in the selection 
                // stored above in order to perform two different actions:
                // 1) Every property in the common property map that is not present in the current
                // window is no longer common and must be removed from the common map. If we
                // are processing all properties (onlyCommon parameter is set to false), we also
                // need to move it to the non common map before deleting it.
                // 2) For properties that are present but whose value is not common (i.e. not exactly
                // the same in all windows that contain it), we need to assign them as empty.
                for (RelevantPropertiesIterator iter = commProp->begin(); iter != commProp->end();)
                {
                    // Obtain property name
                    const PropertyID propertyName = (*iter).first;

                    // Is the property present in the current window?
                    if (!curWidget->isPropertyExist(propertyName))
                    {
                        // If we are processing all properties, we need to add the current property
                        // to the non-common properties map before deleting it from the common 
                        //properties one
                        if (!onlyCommon)
                            (void)noncommProp->insert(std::make_pair(propertyName, (*iter).second));

                        // Property is not common to ALL elements in the selection, so delete it
                        // from the common property map
                        // NOTE: The erase() function in associative containers doesn't invalidate
                        // any iterators except those that point to elements being erased (that's
                        // also true for 'std::list' and 'std::set' but not 'std::vector'). For 
                        // this reason, we don't really need map::erase() to return an iterator
                        (void)commProp->erase(iter++);
                    }
                    else
                    {
                        // Property exists, so we need to make sure its value (stored in the common
                        // property map) is the same as the one assigned to the current window
                        const String propertyValue = (*iter).second;
                        const String currentWindowPropertyValue = curWidget->getPropertyValue(propertyName);
                        if (propertyValue != currentWindowPropertyValue)
                            // Values are different, so set property as empty
                            (*iter).second = _T("");

                        // Move on to next property
                        ++iter;
                    }
                }
            }
        }
        // Ouch, something went wrong
        SHOW_EXCEPTION("EditorDocument::GetRelevantProperties", "Error", "Unexpected standard exception while getting relevant properties", "Unexpected non-standard exception while getting relevant properties");
    }
    //------------------------------------------------------------------------
    bool EditorDocument::SetProperty(PropertyID aName, const String& newValue, const bool allowEmpty)
    {
        bool bChanges = false;

        // This check is added because the main dialog does quite some calls with empty values,
        // for example when launching the app (initializing the editboxes).
        if (newValue == "" && !allowEmpty)
        {
            return true;
        }

        Selection::Boxes & boxes = m_selection.GetAllBoxes();
        for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
        {
            Widget * temp = boxIt->GetWindow();
            // Does this window have the given property?
            if (temp->isPropertyExist(aName))
            {
                // Yes, is it different? (this check is done to avoid a SetModified when a window is only selected).
                if (temp->getPropertyValue(aName) != newValue)
                {
                    // Yes, set the new value
                    LogDebug(wxT("'%s' set to '%s'"), StrHelper::ToWXString(temp->getPropertyName(aName)).c_str(), StrHelper::ToWXString(newValue).c_str());
                    temp->setPropertyValue(aName, newValue);
                    // In case it was a position- or size value.
                    boxIt->Reset();
                    bChanges = true;
                }
            }
        }
        if (bChanges)
        {
            EditorDocument::Modify(true);
        }

        return bChanges;
    }

    //------------------------------------------------------------------------
    bool EditorDocument::OnSaveDocument(const wxString& filePath)
    {
        // Validations
        wxASSERT_MSG(m_activeLayout != NULL, wxT("Active layout is NULL and can't be saved!"));

        // Pre-save
        OnPreSave();

        try
        {
            // Save it, now using GUI itself to do it! :-)
            N_Only(Widget).writeWidget(m_activeLayout, StrHelper::ToNIIString(filePath));
            // Store this setting for application reload
            wxString fileName = wxFileName(filePath).GetFullName();
            // Just save the filename, because the path will get appended when its opened. This implies
            // that you can only load this setting when you saved to the INI's Layout directory.
            UILayoutOptions::GetInstancePtr()->SetCurrentLayout(fileName);
        }
        // If something goes wrong, show user
        SHOW_EXCEPTION("EditorDocument::OnSaveDocument", "Error", "Unexpected standard exception while saving document", "Unexpected non-standard exception while saving document");

        // Tell the document it's "clean" again
        EditorDocument::Modify(false);
        return true;
    }

    //------------------------------------------------------------------------
    void EditorDocument::OnPreSave() const
    {
        WidgetManager * winMgr = N_OnlyPtr(Widget);
        const Widgets & windowIt = winMgr->getList();
        Widgets::const_iterator i, iend = windowIt.end();
        for(i = windowIt.begin(); i != iend; ++i)
        {
            Widget * const window = *i;
            //COMPILE_HACK("This is a hack to overcome the problem that GUI writes the __auto_* windows. Needs to be patched into GUI itself, but after RC2")
            if (window->getName().find(_T("__auto_")) != String::npos)
            {
                // COMPILE_HACK("A hack within a hack; a so-called 'nested hack'. Otherwise ScrollablePane does not save its children.")
                if (window->getName().find(_T("__auto_container__")) == String::npos)
                {
                    window->setDataWrite(false);   // __auto_*
                    LogDebug(wxT("Surpress writing of: '%s'(%s)"), StrHelper::ToWXString(window->getName()).c_str(),
                        StrHelper::ToWXString(N_conv(window->getType())).c_str());
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void EditorDocument::CutSelection()
    {
        // Same as copy
        CopySelection();

        // Remove the selection
        DeleteSelection();
    }
    //------------------------------------------------------------------------
    void EditorDocument::CopySelection()
    {
        // Undo previous result.
        m_copiedSelection.Reset();

        // Use the moveables as a base for our recursion
        Selection::Boxes& boxes = m_selection.GetMoveableBoxes();
        for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
        {
            m_copiedSelection.Copy(boxIt->GetWindow());
        }
    }

    //------------------------------------------------------------------------
    void EditorDocument::PasteCopiedSelection()
    {
        // Paste the selection on the first selected window (if any) or the layout root.
        if (m_selection.Size())
        {
            m_copiedSelection.Paste(m_selection.GetAllBoxes().begin()->GetWindow());
        }
        else
        {
            m_copiedSelection.Paste(m_activeLayout);
        }
        // Common selection behaviour: after a paste, nothing is selected anymore
        m_selection.ClearSelection();
        // Tell observers
        for (size_t i = 0; i < m_observers.size(); ++i)
        {
            m_observers[i]->WindowAdded(NULL);
        }
    }

    //------------------------------------------------------------------------
    void EditorDocument::DeleteSelection()
    {
        // Don't use an iterator loop, since the removeWindow method may unselect windows
        // and thereby invalidate the iteration(!)
        while (m_selection.GetMoveableBoxes().size() > 0)
        {
            // Delete the top-most windows. Only notify on the last one.
            if (!RemoveWindow(m_selection.GetMoveableBoxes().begin()->GetWindow(), m_selection.GetMoveableBoxes().size() == 1))
            {
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void EditorDocument::Modify(bool modify)
    {
        wxDocument::Modify(modify);
        wxDocument::UpdateAllViews();
    }
    //------------------------------------------------------------------------
    IMPLEMENT_DYNAMIC_CLASS(EditorView, wxView)
    //------------------------------------------------------------------------
    EditorView::EditorView() :
        m_frame(wx_static_cast(UILayoutEditorFrame*, NULL)),
        m_renderCanvas(wx_static_cast(EditorCanvas*, NULL)),
        m_background(wx_static_cast(GeometryPixel*, NULL)),
        wxView(),
        mBackgroundID(0)
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
    }
    //------------------------------------------------------------------------
    bool EditorView::IsBackgroundAvailable() const
    {
        return (m_background != NULL);
    }
    //------------------------------------------------------------------------
    void EditorView::InitBackground()
    {
        // First call?
        if (!m_background)
        {
            try
            {
                N_Only(GBuffer).create(m_background);
                // Apply last stored settings
                SetBackgroundImage(UILayoutOptions::GetInstancePtr()->GetCurrentBackground());
            }
            // If something goes wrong, show user
            SHOW_EXCEPTION("EditorView::InitBackground", "Error", "Unexpected standard exception while creating background widget", "Unexpected non-standard exception while creating background widget");
        }
    }
    //------------------------------------------------------------------------
    void EditorView::SetBackgroundImage(const wxString& fullpath)
    {   // Only process when a file is given
        if (!fullpath.IsEmpty())
        {   // Remove current imageset. This call just silently bails out when not found.
            N_Only(PixelBuffer).destroy(mBackgroundID);
            try
            {
                // Just to be sure.
                InitBackground();
                // Okay, behold... because ImageSets get loaded from the predefined resource group,
                // we change it for the moment.
                wxFileName filename(fullpath);
                const wxString path = filename.GetPath() + wxT("/");
                const wxString file = filename.GetFullName();
                mBackgroundResID = N_Only(ResourceScheme).create(0, StrHelper::ToNIIString(path));
                mBackgroundID = N_Only(PixelBuffer).create(0, StrHelper::ToNIIString(file), mBackgroundResID)->getGroupID();

                // Store this setting (full path this time, because this method will be called for it)
                UILayoutOptions::GetInstancePtr()->SetCurrentBackground(fullpath);

                UpdateBackground();
                DrawBackground();
                // Always make visible, otherwise the user wouldn't be picking a background image
                SetBackgroundVisibility(true);
            }
            // If something goes wrong, show user
            SHOW_EXCEPTION("EditorView::SetBackgroundImage", "Error",
                "Unexpected standard exception while setting background image",
                "Unexpected non-standard exception while setting background image");
        }
    }
    //------------------------------------------------------------------------
    void EditorView::SetBackgroundVisibility(bool visible)
    {
        // Validations
        wxASSERT_MSG(m_background != NULL, wxT("Background is empty or invalid!"));

        if (m_background)
        {
            // Store this setting
            UILayoutOptions::GetInstancePtr()->SetBackgroundVisible(visible);

            N_Only(UI).getSheet(N_Engine().getView())->redraw();
            wxGetApp().GetLayoutEditorFrame()->Refresh();
        }
    }
    //------------------------------------------------------------------------
    void EditorView::DrawBackground()
    {
        if (UILayoutOptions::GetInstancePtr()->IsBackgroundVisible())
            m_background->flush();
    }
    //------------------------------------------------------------------------
    void EditorView::UpdateBackground()
    {
        if (!m_background || !N_Only(PixelBuffer).isExist(mBackgroundID))
            return;

        PlaneSizei temp = N_Only(UI).getDisplaySize(N_Engine().getView());
        Rectf dest(0, 0, temp.mWidth, temp.mHeight);

        m_background->setClip(Recti(0, 0, temp.mWidth, temp.mHeight));

        try
        {
            PixelBuffer * bgis = N_Only(PixelBuffer).get(mBackgroundID << N_Com_Escape);
            // Pre-draw the image into the geometry buffer ready for rendering
            m_background->add(*bgis, dest, 0, ColourArea());
        }
        // If something goes wrong, show user
        SHOW_EXCEPTION("EditorView::UpdateBackground", "Error",
            "Unexpected standard exception while updating background image",
            "Unexpected non-standard exception while updating background image");
    }
    //------------------------------------------------------------------------
    bool EditorView::OnCreate(wxDocument *document, long WXUNUSED(flags))
    {
        // Single-window mode
        m_frame = wxGetApp().GetLayoutEditorFrame();
        m_renderCanvas = m_frame->GetCanvas();
        m_renderCanvas->setView(this);

        // Associate the appropriate frame with this view.
        SetFrame(m_frame);

        // Tell the frame about the document
        m_frame->SetDocument(wx_static_cast(EditorDocument*, document));

        UpdateFrameTitle();

        // initialise support for background imagery
        InitBackground();

        // Make sure the document manager knows that this is the
        // current view.
        wxView::Activate(true);

        return true;
    }
    //------------------------------------------------------------------------
    bool EditorView::OnClose(bool WXUNUSED(deleteWindow))
    {
        if (!GetDocument()->Close())
        {
            return false;
        }

        if (m_renderCanvas)
        {
            m_renderCanvas->ClearBackground();
            m_renderCanvas->setView(wx_static_cast(wxView*, NULL));
            m_renderCanvas = wx_static_cast(EditorCanvas*, NULL);
        }
        SetFrame(wx_static_cast(wxFrame*, NULL));

        // Tell the frame about the document
        if (m_frame)
        {
            m_frame->SetDocument(wx_static_cast(EditorDocument*, NULL));
            m_frame = wx_static_cast(UILayoutEditorFrame*, NULL);
        }

        // Cleanup the background buffer
        if (m_background)
        {
            m_background->clear();
            N_Only(GBuffer).destroy(m_background);
            m_background = wx_static_cast(GeometryPixel*, NULL);
        }

        wxView::Activate(false);

        UpdateFrameTitle();

        return true;
    }
    //------------------------------------------------------------------------
    void EditorView::OnUpdate(wxView* WXUNUSED(sender), wxObject* WXUNUSED(hint))
    {
        if (m_frame && m_frame->IsShown() && m_renderCanvas)
        {
            //LogDebug(wxT("Needs repaint."));
            m_renderCanvas->Refresh(false);
            m_renderCanvas->Render();
        }
    }
    //------------------------------------------------------------------------
    void EditorView::OnChangeFilename()
    {
        UpdateFrameTitle();
    }
    //------------------------------------------------------------------------
    void EditorView::UpdateFrameTitle()
    {

        wxString title(wxTheApp->GetAppName());

        if (GetDocument())
        {
            title << wxT(" - [") << GetDocument()->GetFilename() << wxT("]");
        }
        if (m_frame)
        {
            m_frame->SetTitle(title);
        }
    }
    //------------------------------------------------------------------------
    void EditorView::OnDraw(wxDC* WXUNUSED(dc))
    {
    }
    //////////////////////////////////////////////////////////////////////////
    // EDITORCANVAS EVENT TABLE
    //////////////////////////////////////////////////////////////////////////
    //-----------------------------------------------------------------------
    BEGIN_EVENT_TABLE(EditorCanvas, wxGLCanvas)
    EVT_PAINT				(EditorCanvas::OnPaint)
    EVT_MOTION				(EditorCanvas::OnMouseMotion)
    EVT_LEFT_DOWN			(EditorCanvas::OnLeftDown)
    EVT_LEFT_UP				(EditorCanvas::OnLeftUp)
    EVT_KEY_DOWN			(EditorCanvas::OnKeyDown)
    END_EVENT_TABLE()
    //////////////////////////////////////////////////////////////////////////
    // EDITORCANVAS FUNCTIONS
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    EditorCanvas::EditorCanvas(wxView *v, wxFrame *frame, const wxPoint& pos, const wxSize& size) :
        // Store view
        m_view(v),

        // Initial (invalid) values
        mCurrentWidth(-1),
        mCurrentHeight(-1),
        mLastMouseX(-1),
        mLastMouseY(-1),

        wxGLCanvas(frame, wx_static_cast(wxGLCanvas*, NULL), -1, pos, size, wxNO_BORDER )
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

        // Init one-time-only stuff
        SetCurrent();

        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);

        // Set the clear color to a grey value
        glClearColor (0.75f, 0.75f, 0.75f, 1.0f);

        // Depth buffer setup
        glClearDepth(1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    //------------------------------------------------------------------------
    void EditorCanvas::changeSize(int newWidth, int newHeight)
    {
        // Tell the canvas!
        SetSize(0, 0, newWidth, newHeight);

        mCurrentWidth = newWidth;
        mCurrentHeight = newHeight;

        // Tell openGL
        glViewport(0, 0, wx_static_cast(GLsizei, newWidth), wx_static_cast(GLsizei, newHeight));

        // Calculate aspect ratio
        //gluPerspective(45.0,wx_static_cast(GLdouble, newWidth)/wx_static_cast(GLdouble, newHeight),0.1,100.0);
    }
    //------------------------------------------------------------------------
    void EditorCanvas::OnMouseMotion(wxMouseEvent& event)
    {
        // Check this, because we might get this event during document closing
        if (m_view && m_view->GetDocument() && (event.GetX() != mLastMouseX || event.GetY() != mLastMouseY))
        {
            //logDebug(wxT("Mouse move accepted"));

            EditorDocument* const document = wx_static_cast(EditorDocument*, m_view->GetDocument());

            document->SetDimensions(mCurrentWidth, mCurrentHeight);

            // Update
            mLastMouseX = event.GetX();
            mLastMouseY = event.GetY();

            document->HandleMouseMoved (mLastMouseX, mLastMouseY) ;
        }
    }
    //------------------------------------------------------------------------
    void EditorCanvas::OnLeftDown(wxMouseEvent& event)
    {
        // Check this, because we might get this event during document closing
        if (m_view && m_view->GetDocument())
        {
            EditorDocument* const document = wx_static_cast(EditorDocument*, m_view->GetDocument());
            document->HandleMousePressed (event.GetX(), event.GetY(), !event.ControlDown()) ;
        }
    }
    //------------------------------------------------------------------------
    void EditorCanvas::OnLeftUp(wxMouseEvent& WXUNUSED(event))
    {
        // Check this, because we might get this event during document closing
        if (m_view && m_view->GetDocument())
        {
            EditorDocument* const document = wx_static_cast(EditorDocument*, m_view->GetDocument());
            document->HandleMouseReleased () ;
        }
    }
    //------------------------------------------------------------------------
    void EditorCanvas::Render()
    {
        if (m_view && m_view->GetDocument())
        {
            SetCurrent();

            // clear color and depth buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Prepare 2D operations
            Set2DMode();

            // GUI rendering
            RenderLayout();		

            // Resizer rendering
            DrawResizers();

            glFlush();    
            SwapBuffers();
        }
    }
    //------------------------------------------------------------------------
    void EditorCanvas::Set2DMode() const
    {
        // Prepare 2D operations
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Prepare for 2D drawing
        glPointSize(1.0f);
        glLineWidth(1.0f);
    }
    //------------------------------------------------------------------------
    void EditorCanvas::DrawGrid() const
    {
        // Only when required/relevant to do
        const bool gridVisible = UILayoutOptions::GetInstancePtr()->IsGridVisible();
        const int stepSize = UILayoutOptions::GetInstancePtr()->GetGridSize();
        if (gridVisible && stepSize > 1)
        {
            int stepsX, stepsY;
            if (mCurrentHeight % stepSize == 0)
            {
                stepsY = mCurrentHeight / stepSize;
            }
            else
            {
                stepsY = (mCurrentHeight / stepSize) + 1;
            }
            if (mCurrentWidth % stepSize == 0)
            {
                stepsX = mCurrentWidth / stepSize;
            }
            else
            {
                stepsX = (mCurrentWidth / stepSize) + 1;
            }

            // Lines
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
            //glColor4f (0.9f, 0.9f, 0.9f, 0.5f);
            glColor4f (0.0f, 0.0f, 0.5f, 0.5f);
            glBegin(GL_LINES);

            // Horz
            for (int y=0; y<stepsY; y++)
            {
                GLfloat yTopAndBottom = 1.0f - ( (wx_static_cast(GLfloat, (y * stepSize) + 1) / mCurrentHeight) * 2.0f);

                // Two points form a line
                glVertex2f(-1.0, yTopAndBottom);
                glVertex2f(1.0, yTopAndBottom);
            }

            // Vert
            for (int x=0; x<stepsX; x++)
            {
                GLfloat xLeftRight = -1.0f + ( (wx_static_cast(GLfloat, x * stepSize) / mCurrentWidth) * 2.0f);

                // Two points form a line
                glVertex2f(xLeftRight, -1.0);
                glVertex2f(xLeftRight, 1.0);
            }

            glEnd(); // GL_LINES;
            glDisable(GL_BLEND);
        }
    }
    //------------------------------------------------------------------------
    void EditorCanvas::DrawResizers() const
    {
        // Validations
        wxASSERT_MSG(m_view != NULL, wxT("View must exist in order to draw resizers"));

        GLfloat topLeftX, topLeftY, bottomRightX, bottomRightY;

        //logDebug(wxT("Painting resizers."));

        EditorDocument* const document = wx_static_cast(EditorDocument*, m_view->GetDocument());

        Selection::Boxes& boxes = document->GetSelection()->GetAllBoxes();
        for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
        {
            // Draw a rectangle around the window. Don't use DrawRectangle because it fills
            // the area. We don't want that, just lines.
            const wxRect winRc = boxIt->GetWindowRect();

            // Non-filled RED rectangle
            glColor3f (1.0f, 0.0f, 0.0f);
            glBegin(GL_LINE_LOOP);
            // Little pixel tweaking to nicely surround the window
            topLeftX = -1.0f + ( (wx_static_cast(GLfloat, winRc.x - 1) / mCurrentWidth) * 2.0f);
            topLeftY = 1.0f - ( (wx_static_cast(GLfloat, winRc.y) / mCurrentHeight) * 2.0f);
            bottomRightX = -1.0f + ( (wx_static_cast(GLfloat, winRc.GetRight()) / mCurrentWidth) * 2.0f);
            bottomRightY = 1.0f - ( (wx_static_cast(GLfloat, winRc.GetBottom() + 1) / mCurrentHeight) * 2.0f);

            glVertex2f(topLeftX, topLeftY);
            glVertex2f(bottomRightX, topLeftY);
            glVertex2f(bottomRightX, bottomRightY);
            glVertex2f(topLeftX, bottomRightY);
            glEnd();	// GL_LINE_LOOP

            // When the window is not locked, draw the eight points as white rectangles with a red border.
            if (!boxIt->IsLocked() && boxIt->IsResizable())
            {
                for (int i = 0; i < NUM_RESIZE_POINTS; ++i)
                {
                    const wxRect pointRc = boxIt->GetPointRect(i);

                    // WHITE filled rectangle for the inside
                    glColor3f (1.0f, 1.0f, 1.0f);
                    glBegin(GL_QUADS);
                    topLeftX = -1.0f + ( (wx_static_cast(GLfloat, pointRc.x) / mCurrentWidth) * 2.0f);
                    topLeftY = 1.0f - ( (wx_static_cast(GLfloat, pointRc.y) / mCurrentHeight) * 2.0f);
                    bottomRightX = -1.0f + ( (wx_static_cast(GLfloat, pointRc.GetRight()) / mCurrentWidth) * 2.0f);
                    bottomRightY = 1.0f - ( (wx_static_cast(GLfloat, pointRc.GetBottom()) / mCurrentHeight) * 2.0f);

                    glVertex2f(topLeftX, topLeftY);
                    glVertex2f(bottomRightX, topLeftY);
                    glVertex2f(bottomRightX, bottomRightY);
                    glVertex2f(topLeftX, bottomRightY);
                    glEnd();	// GL_QUADS

                    // RED surrounding rectangle for the border (just draw over the filled rectangle)
                    glColor3f (1.0f, 0.0f, 0.0f);
                    glBegin(GL_LINE_LOOP);
                    topLeftX = -1.0f + ( (wx_static_cast(GLfloat, pointRc.x) / mCurrentWidth) * 2.0f);
                    topLeftY = 1.0f - ( (wx_static_cast(GLfloat, pointRc.y) / mCurrentHeight) * 2.0f);
                    bottomRightX = -1.0f + ( (wx_static_cast(GLfloat, pointRc.GetRight()) / mCurrentWidth) * 2.0f);
                    bottomRightY = 1.0f - ( (wx_static_cast(GLfloat, pointRc.GetBottom()+1) / mCurrentHeight) * 2.0f);

                    glVertex2f(topLeftX, topLeftY);
                    glVertex2f(bottomRightX, topLeftY);
                    glVertex2f(bottomRightX, bottomRightY);
                    glVertex2f(topLeftX, bottomRightY);
                    glEnd();	// GL_LINE_LOOP

                    // logDebug(wxT("Draw rectangle at %f, %f, %f, %f"), x0, y0, x1, y1);
                }
            }

            // Lines to udim 'origin'
            for (int dir=0; dir<NUM_RESIZE_DIRS; ++dir)
            {
                const wxPoint boxPos = boxIt->GetSidePoint(dir, false);
                const wxPoint basePos = boxIt->GetSidePoint(dir, true);

                // Make sure side points are valid
                if( (boxPos.x == -1 && boxPos.y == -1) || (basePos.x == -1 && basePos.y == -1))
                    return;

                topLeftX = -1.0f + ( (wx_static_cast(GLfloat, basePos.x) / mCurrentWidth) * 2.0f);
                topLeftY = 1.0f - ( (wx_static_cast(GLfloat, basePos.y) / mCurrentHeight) * 2.0f);
                bottomRightX = -1.0f + ( (wx_static_cast(GLfloat, boxPos.x) / mCurrentWidth) * 2.0f);
                bottomRightY = 1.0f - ( (wx_static_cast(GLfloat, boxPos.y) / mCurrentHeight) * 2.0f);

                glColor3f (0.0f, 1.0f, 0.0f);
                glBegin(GL_LINE_LOOP);
                glVertex2f(topLeftX, topLeftY);
                glVertex2f(bottomRightX, bottomRightY);
                glEnd();
            }
        }
    }
    //------------------------------------------------------------------------
    void EditorCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
    {
        // We need to do this to avoid a flood of paint events
        wxPaintDC paintDC(this);

        Render();
    }
    //------------------------------------------------------------------------
    void EditorCanvas::RenderLayout() const
    {
        // Grid rendering
        DrawGrid();

        // Render the current layout
        N_Only(UI).render();
    }
    //------------------------------------------------------------------------
    void EditorCanvas::OnKeyDown(wxKeyEvent& event)
    {
        // Make sure no key is processed if the view hasn't been
        // initialized yet
        if(!m_view)
            return;

        int pixelDeltaX = 0, pixelDeltaY = 0;
        const int key = event.GetKeyCode();

        // Restore fullscreen mode
        if (event.GetKeyCode() == WXK_ESCAPE && wxGetApp().GetLayoutEditorFrame()->IsFullScreen())
        {
            (void)wxGetApp().GetLayoutEditorFrame()->ShowFullScreen(false);
            wxGetApp().GetLayoutEditorFrame()->Refresh();
        }

        if (key == 'g' || key == 'G')	// 'g' for "grid" ('s' is already used)
        {
            SelectionMover((wx_static_cast(EditorDocument*, m_view->GetDocument()))->GetSelection()).SnapToGrid();
            m_view->GetDocument()->Modify(true);
            // Done
            return;
        }
        if (key == 'w' || key == 'W' || key == WXK_UP)
        {
            pixelDeltaY = -1;
        }
        else if (key == 's' || key == 'S' || key == WXK_DOWN)
        {
            pixelDeltaY = 1;
        }
        else if (key == 'a' || key == 'A' || key == WXK_LEFT)
        {
            pixelDeltaX = -1;
        }
        else if (key == 'd' || key == 'D' || key == WXK_RIGHT)
        {
            pixelDeltaX = 1;
        }

        // Was a relevant selection mover key pressed?
        if (pixelDeltaX || pixelDeltaY)
        {
            // Control key is used to change selection by keys to small steps (when you're near your target location.)
            if (!event.ControlDown())
            {
                pixelDeltaX *= UILayoutOptions::GetInstancePtr()->GetBigKeyStep();
                pixelDeltaY *= UILayoutOptions::GetInstancePtr()->GetBigKeyStep();
            }

            SelectionMover((wx_static_cast(EditorDocument*, m_view->GetDocument()))->GetSelection()).UpdateWindowAreas(wx_static_cast(float, pixelDeltaX), wx_static_cast(float, pixelDeltaY), wx_static_cast(float, pixelDeltaX), wx_static_cast(float, pixelDeltaY));
            m_view->GetDocument()->Modify(true);
        }
    }
    //------------------------------------------------------------------------
}