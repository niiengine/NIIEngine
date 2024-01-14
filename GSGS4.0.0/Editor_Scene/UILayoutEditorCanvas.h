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

#ifndef _UILAYOUTEDITOR_CANVAS_H_
#define _UILAYOUTEDITOR_CANVAS_H_

#include "gsgsPreInclude.h"
#include "NiiPreInclude.h"
#include "UILayoutCopiedSelection.h"
#include "UILayoutOptions.h"
#include "NiiGeometryPixel.h"
#include "wx/glcanvas.h"
#include "wx/docview.h"

using namespace NII;
using namespace NII::UI;

namespace gsgs
{
    class UILayoutEditorFrame;
    class IDocumentObserver;
    class EditorCanvas;
    /** EditorDocument - The application's Document implementation.
    *
    * This class is used as interface between the application and the GUI system. It is important to know that this
    * class is like a "thin client". It does not catch anything (except for a few pointers). Each call is made directly
    * on GUI, so no separate administrations have to be maintained, which makes this class remarkably easy to understand
    * I think.
    *
    */
    class EditorDocument : public wxDocument
    {
        DECLARE_DYNAMIC_CLASS(EditorDocument)
    public:
        EditorDocument();

         /** Returns the current width of the render window.*/
        int GetWidth()                          { return m_width; }

        /** Returns the current height of the render window.*/
        int GetHeight()                         { return m_height; }

        /** @Returns container with selected GUI windows.*/
        Selection* GetSelection()               { return &m_selection; }

        /** @Returns container with deep copied GUI windows.*/
        CopiedSelection* GetCopiedSelection()   { return &m_copiedSelection; }

        /** @Returns the currently active (loaded) layout.*/
        Window* GetActiveLayout() const         { return m_activeLayout; }

        /** Sets/Removes the locked window selection status.
         * @access public
         * @qualifier none
         * @param lockedSelection New selection lock status
         * @return void
         * @remarks In locked mode, the user can't choose another selection without removing the lock
         */
        void SetLockedSelection(bool locks)     { m_lockedSelection = locks; }

        /** Obtains the current window selection lock status.
         * @access public
         * @qualifier const
         * @return bool Selection lock status
         * @remarks In locked mode, the user can't choose another selection without removing the lock
         */
        bool GetLockedSelection() const         { return m_lockedSelection; }

        /** Swaps the current value of the window selection lock status.
         * @access public
         * @qualifier none
         * @return void
         */
        void SwitchLockedSelection()            { m_lockedSelection = !m_lockedSelection; }

        /************************************************************************
         * PUBLIC FUNCTIONS
         ************************************************************************/

         /** Opens the given layout file as editable.
          */
        bool OnOpenDocument(const wxString& fileName);

        /** Starts a new layout.
         */
        bool OnNewDocument();

        /** Saves the current layout to its filepath.
         */
        bool OnSaveDocument(const wxString& filePath);

        /** The current layout has been closed. The system has prompted the user to save changes, if any.
         */
        bool OnCloseDocument();

        /** Sets the document to being 'dirty' and updates the attached views.
         */
        void Modify(bool modify);

        /** Sets the dimensions of the current view settings. This depends on the resolution. This values
         * are used to normalize the X and Y coordinates.
         * @param iWidth the width of the render window
         * @param iHeight the height of the render window*/
        void SetDimensions(int aWidth, int aHeight)
        {
            m_width = aWidth;
            m_height = aHeight;
        }

        /** Resets the document for editing eg. after a resolution-change which re-starts Ogre.*/
        void Reset();

        /** Adds the given observer.
         * @param observer.*/
        void AddObserver(IDocumentObserver* pObserver);

        /** Adds a window of the given type, with the given name, to the given parent.
         *
         * @param pParentWindow the parent of the new window.
         * @param aType the window type e.g. "Checkbox"
         * @param aName the window name e.g. "Female".
         * @return the newly created window, or NULL if failed.
         */
        Widget* AddWindow(Container* pParentWindow, StyleViewID aType, const String & aName);

        /** Removes the given window from the document.
         *
         * @param aWindow the window to remove.
         * @param notifyListeners can be used as a performance gain to only notify on the last in a loop.
         * @return whether the window could be removed. For example the 'root' cannot be removed.
         */
        bool RemoveWindow(Widget * aWindow, bool notifyListeners = true);

        /** Sets the given 'root' as the active layout.
         *
         * @param  pRoot
         */
        void SetActiveLayout(Window* aRoot);

        /** Delegate for mouse-moving events.
         * @param mouseX current mouse position.
         * @param mouseY current mouse position.*/
        void HandleMouseMoved(int aMouseX, int aMouseY);

        /** Delegate for mouse-pressing events. Returns wheter changes have been made.
         * @param mouseX current mouse position.
         * @param mouseY current mouse position.
         * @param whether CTRL is pressed.*/
        void HandleMousePressed(const int aMouseX, const int aMouseY, const bool aMustReset);

        /** Invoked when the mouse has been released.*/
        void HandleMouseReleased();

        /** Selects the given window, and optionally resets the current selection first.
         * @todo needs to be moved to the VIEW*/
        void SelectWindow(Widget* aWindow, bool aMustReset = true);

        /* Selects all windows in the layout */
        void SelectAll();

        // Relevant Parameters
        typedef std::map<PropertyID, String> RelevantProperties;
        typedef RelevantProperties::iterator RelevantPropertiesIterator;

        /** Builds a collection (in the formal sense of the meaning) of relevant
         * properties of the current window selection. Depending on the value of the onlyCommon
         * parameter, the returned collection will only include the properties that are
         * common to ALL elements in the current user selection (true) or every property
         * that is present in at least one element in the user selection, including the common
         * ones (false).
         * @access public
         * @qualifier none
         * @param commonProperties Collection of properties common to ALL widgets in the current
         * user selection
         * @param nonCommonProperties Collection of properties that are present in at least one element
         * in the current user selection but not in ALL of them (only filled if onlyCommon is set to true)
         * @param onlyCommon Indicates if only common properties are to be retrieved (true) or not (false)
         * @return void
         */
        void GetRelevantProperties(RelevantProperties* commonProperties, RelevantProperties* nonCommonProperties, const bool onlyCommon = true);

        /** Sets the given property to the given value.
         * @param aName, eg. \"Alpha\".
         * @param newValue, eg. \"0.5\".
         * @param allowEmpty denotes whether an empty values is allowed.
         */
        bool SetProperty(PropertyID pid, const String& newValue, const bool allowEmpty = false);

        /** Cuts the currently selected windows from the GUI system.*/
        void CutSelection();

        /** Copies the currently selected windows from the GUI system.*/
        void CopySelection();

        /** Pastes the currently selected windows from the GUI system onto
         * the first window of the current selection. TODO: After that, the pasted
         * elements become the new selection. (Powerpoint style).
         */
        void PasteCopiedSelection();

        /** Deletes the currently selected windows from the GUI system.*/
        void DeleteSelection();
    private:
        /** Currently active layout.*/
        Window*  m_activeLayout;

        /** Stores observers.*/
        std::vector<IDocumentObserver*>   m_observers;

        /** Resize point above which the mouse cursor is. */
        int m_hoveredResizePoint;
        int m_previousHoveredResizePoint;

        /** Is the mouse currently pressed? */
        bool  m_mousePressed;

        /** Current render window width.*/
        int   m_width;

        /** Current render window height.*/
        int   m_height;

        /** Current mouseX position. Integer because it's as accurate as it gets.*/
        int   m_mouseX;

        /** Current mouseX position. Integer because it's as accurate as it gets.*/
        int   m_mouseY;

        /** Previous mouseX position, to see differences. Integer because it's as accurate as it gets.*/
        int   m_previousMouseX;

        /** Previous mouseY position, to see differences. Integer because it's as accurate as it gets.*/
        int   m_previousMouseY;

        /** Window above which the mouse cursor is. */
        Widget* mHover;

        /** This object manages the selections. Selects elements and also allows copy / paste / delete operations.
         * It is public to avoid forwarding of its methods.*/
        Selection m_selection;

        /** This object contains the result of a copy or cut.*/
        CopiedSelection m_copiedSelection;

        /** Determines if the user can change the current window selection by clicking on other windows
        * outside the selection (false) or if the selection remains constant independently of the user's
        * actions (true).*/
        bool m_lockedSelection;

        /************************************************************************
         * PRIVATE FUNCTIONS
         ************************************************************************/

         /** Initialises a newly created window, so we can do some initialization
          * based on the type. For example a Menubar root item required a Popupmenu immediately.*/
        void InitNewWindow(Widget * aNewWindow, Container* aParentWindow);

        /** Convenience method: pre-populated the given tabcontrol.*/
        void InitTabControl(TabControl * aTabControl);

        /** Convenience method: pre-populated the given menu bar.*/
        void InitMenubar(Window* aMenuBar);

        /** Very minimal locking behaviour; when the user selects a window, we check whether it may be resized.
         * We use it to block resizing of auto-calculated children, such as menu items and tab buttons.*/
        bool RequiresLock(const Widget * aWindow) const;

        /** Unactivates the current layout. Undoes selection and such.*/
        void DeactivateCurrentLayout();

        /** Updates the status bar.
        * @todo needs to be moved to the VIEW*/
        void UpdateStatusBar(const wxChar *format, ...) const;

        /** Sets the cursor according to the hovered resize point.
         * @todo needs to be moved to the VIEW*/
        void UpdateCursor();

        /** Checks if mHover is a TabButton, which is a special case. If so, the mHover,
         * will be updated to point to the actual ContentPane. */
        bool PressedTabButton();

        /** Checks if mHover is a MenuItem, which is a special case. If so, the menu item's
         * PopupMenu will get toggled. */
        bool PressedMenuItem();

        /** Checks if the user want to change the colours of selected widget.
         * @todo testing!!*/
        void HandleColoursUpdate(const bool shiftPressed) const;

        /** Called before saving, so we can make some just-in-time modifications if we want.
         */
        void OnPreSave() const;
    };

    /** A "view" on a "document". Note that views are not created when the application
     * starts, but only when the user chooses "file->new" or "file->open".
     * Also note that the dialog is not a real view.
     */
    class EditorView : public wxView
    {
        // Needed for the doc/view manager
        DECLARE_DYNAMIC_CLASS(EditorView)
    public:
         /** Constructor.*/
        EditorView();

         /** Returns whether we have a valid background widget loaded.*/
        bool IsBackgroundAvailable() const;

        /** Applies the given filename as a background image. */
        void SetBackgroundImage(const wxString& filepath);

        /** Toggles background visibility. */
        void SetBackgroundVisibility(bool visible);

        /** Draw the background geometry / imagery if enabled */
        void DrawBackground();

        /** Update background geometry and clipping */
        void UpdateBackground();
    private:
         /** The view is attached to a frame.*/
        UILayoutEditorFrame  *m_frame;

        /** OpenGL rendering canvas.*/
        EditorCanvas *m_renderCanvas;

        /** Geometry buffer for background */
        GeometryPixel* m_background;

        Ni32 mBackgroundID;
        Ni32 mBackgroundResID;

        /************************************************************************
         * OVERRIDDEN PRIVATE FUNCTIONS
         ************************************************************************/

         /** Overridden because the base class is a Window. We don't use it here.*/
        void OnDraw(wxDC *dc);

        /** Notifies us that a new document has been created, either through
         * File->New or File->Open.*/
        bool OnCreate(wxDocument *doc, long flags);

        /** Notifies that the user closes the current document. Either by an explicit
         * closing, or through File->New or File->Open.
         */
        bool OnClose(bool deleteWindow = true);

        /** Notification of the UpdateAllViews call in a document.
         */
        void OnUpdate(wxView* sender, wxObject* hint);

        /** Called by the document when the document's name changes, e.g. on Save or New.*/
        void OnChangeFilename();

        /************************************************************************
         * PRIVATE FUNCTIONS
         ************************************************************************/

         /** Updates the mainframe's title bar according to the currently open document.*/
        void UpdateFrameTitle();

        /** Creates a GeometryPixel object to hold geometry for background image. */
        void InitBackground();
    };

    /** Provides OpenGL output. This canvas is the actual visible part of our "view"
    * on the "document". The GUI system must be initialized when rendering, which is verified.
    */
    class EditorCanvas : public wxGLCanvas
    {
    public:
        EditorCanvas(wxView *view, wxFrame *frame, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
        
        /** Updates the openGL viewport setup. Called by the frame when it gets resized.*/
        void changeSize(int newWidth, int newHeight);

        void setView (wxView* aView)
        {
            m_view = aView;
        }

        /** Forces a re-draw.*/
        void Render();
    private:
        /** The view to which we are attached.*/
        wxView *m_view;

        /** Current canvas width. Used a lot so it's cached.*/
        int mCurrentWidth;

        /** Current canvas height. Used a lot so it's cached.*/
        int mCurrentHeight;

        /** Last known mouseX position.*/
        long mLastMouseX;

        /** Last known mouseY position.*/
        long mLastMouseY;
        
        /** Renders the current layout.*/
        void RenderLayout() const;

        /** Prepares for 2D openGL rendering.*/
        void Set2DMode() const;

        /** Draws the grid, if visible.*/
        void DrawGrid() const;

        /** Draws the current resizes.*/
        void DrawResizers() const;

        /** Event handler: notifies us that the canvas needs a repaint. We render the current layout.*/
        void OnPaint(wxPaintEvent& event);

        /** Event handler: we handle mouse input here.*/
        void OnMouseMotion(wxMouseEvent& event);

        /** Event handler: we handle mouse input here.*/
        void OnLeftDown(wxMouseEvent& event);

        /** Event handler: we handle mouse input here.*/
        void OnLeftUp(wxMouseEvent& event);

        /** Event handler: handles key-down events. We use it to move the current selection.*/
        void OnKeyDown(wxKeyEvent& event);

        // Event handling stuff
        DECLARE_EVENT_TABLE()
    };
}
#endif // _EDITOR_CANVAS_H_
