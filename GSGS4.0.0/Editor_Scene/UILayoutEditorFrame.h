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

#ifndef _UILayoutEDITOR_FRAME_H_
#define _UILayoutEDITOR_FRAME_H_

#include "gsgsPreInclude.h"
#include "NiiUIManager.h"
#include <wx/event.h>
#include <wx/docview.h>
#include <wx/checkbox.h>

using namespace NII::UI;
using namespace NII;

namespace gsgs
{
    class EditorDocument;
    class EditorCanvas;
    class DialogMain;

    /** This dialog prompts the user with Grid settings. It shows the current settings,
    * and allows for modifying them. It reads & stores information through the COptions class,
    * so they end up in the INI file.
    */
    class DialogGrid: public wxDialog
    {
    public:
        DialogGrid (wxWindow* parent);

        ~DialogGrid() {};
    private:
        // Control ID(s)
        enum {
            ID_OK_BUTTON = wxID_HIGHEST,
            ID_CANCEL_BUTTON
        };

        /** Called during construction to represent the current grid setup.*/
        void ShowCurrentSettings();

        /** Initializes the dialog.
         * @access private 
         * @qualifier none
         * @return void
         */
        void Setup();

        /** Stores the (new) settings and closes the dialog.*/
        void OnOk(wxCommandEvent& event);

        /** Discards the (changed) settings and closes the dialog.*/
        void OnCancel(wxCommandEvent& event);

        // Event handling stuff
        DECLARE_EVENT_TABLE();

        /** To update the grid's visibility state.*/
        wxCheckBox*	m_visible;

        /** To update the snap type.*/
        wxRadioBox*	m_snapModeChoice;

        /** To update the grid's size.*/
        wxTextCtrl*	m_size;
    };

    class UILayoutEditorFrame : public wxDocParentFrame
    {
        DECLARE_CLASS(UILayoutEditorFrame)
    public:
        UILayoutEditorFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long type);
        ~UILayoutEditorFrame();
        
        /** Separate method so we can call it after the entire frame has been initialized,
        * because for the canvas it must be visible, which doesn't look that nice when
        * creating the frame.*/
        EditorCanvas *AttachCanvas(wxView *view);

        /** Retrieves the current OpenGL render canvas.
         * @access public 
         * @qualifier const
         * @return EditorCanvas* Current OpenGL render canvas
         */
        EditorCanvas* GetCanvas() const
        {
            return m_renderCanvas;
        }

        /** Receives the document from the attached view for accessing by the menu-events.*/
        void SetDocument(EditorDocument* document);

        /** Overloaded. We manually refresh the canvas when this is requested.*/
        virtual void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL);

        /** we can get the DialoMain, because we need to get the currently selected window */
        DialogMain *GetDialogMain(){return m_dialogMain;};

        /** Toggles between the open and close locked icons depending on the current lock status.
         * @access public 
         * @qualifier none
         * @param document Document which contains the current lock status
         * @return void
         */
        void UpdateLockedSelectionIcon(const EditorDocument* const document) const;
    private:
        /** Defines the menu items. The doc-view items (open, save, exit, cut/copy/paste) are provided
         * by wxWidgets and therefore not found here. Note that Undo and Redo items are updated by wx itself!*/
        enum {
            ID_OPEN_CUSTOM,		// We open from a listbox, not a file dialog
            ID_ALIGN,        // Sub-menu ID does nothing itself.
            ID_ALIGN_LEFT,
            ID_ALIGN_RIGHT,
            ID_ALIGN_TOP,
            ID_ALIGN_BOTTOM,
            ID_ALIGN_WIDTH,
            ID_ALIGN_HEIGHT,
            // Handy dialog stuff
            ID_VIEW_SHOW_HIDE_DIALOG,
            //ID_VIEW_MIN_MAX_DIALOG,
            // View stuff
            ID_VIEW_640x480,
            ID_VIEW_800x600,
            ID_VIEW_1024x786,
            ID_VIEW_1280x800, // Wide
            ID_VIEW_FULLSCREEN,	// This setting is not saved to INI since it's mostly used temporarily
            ID_VIEW_SET_BACKGROUND,
            ID_VIEW_SHOW_BACKGROUND,
            ID_VIEW_SET_GRID,
            ID_VIEW_SET_FONT,
            ID_SKINS,
            ID_LOCK_SELECTION
        };
        UIManager * m_GUISystem;
        DialogMain * m_dialogMain;

        /** OpenGL rendering canvas.*/
        EditorCanvas * m_renderCanvas;
        EditorDocument * m_document;

        /** We store the edit menu, to pass to the CommandProcessor.*/
        wxMenu * mEditMenu;
        
        /** Attaches a menubar to the frame.*/
        void AttachMenubar();

        /** Attaches a toolbar to the frame.*/
        void AttachToolbar();

        /** Applies size from ini file.*/
        void RestoreIniSize();

        /** Handled wxSizeEvent. Propagate the new resolution to the gui renderer.
        */
        void OnResize(wxSizeEvent& event);

        /** Initializes the GUI core + renderer, loads available "looks" and
         * sets a default font to use. We don't cleanup the frame when new view is spawned,
         * so this is a good place to put the GUI functionality.*/
        void InitializeGUI();

        /** Callback that renders the background imagery */
        void RQHandler(const EventArgs* args);

        /** Return a wxString that holds the root directory where the GUI data
         * files can be found.  It will use the following options when trying to
         * establish where it can find some data files:
         * - If the INI file has paths specified, automatically use those
         * (returning an empty wxString).
         * - Check for a 'datafiles' directory in the resources location.
         * (returning the full path to the found datafiles directory)
         * - Ask the user to specify the datafiles location.
         * (returning the selected directory, or an empty string if user cancelled
         * the dialog).
         */ 
        wxString GetGUIDataRoot();

        /** Points the editor to a valid folder with GUI datafiles.*/
        void InitializePaths();

        /** Loads available .scheme and .font files from cegui's datafiles directory.
         */
        void LoadData() const;

        /** General initialization method. Should be called after the constructor.
         * @access private 
         * @qualifier none
         * @return void
         */
        void Setup();

        bool GetFileNames(const wxString& path, const wxString& wildcard, wxArrayString& filenames) const;

        /** We listen to the Edit events (including the Align part) of the menubar.*/
        DECLARE_EVENT_TABLE();

        /** Prompts the user with a list of layouts.*/
        void OnOpenCustom(wxCommandEvent& event);

        // Convenience canvas sizers
        void Resize(int aWidth, int aHeight);
        
        // Some dialog options, since nice docking is not available in default wxWidgets
        void OnShowOrHideDialog(wxCommandEvent& event);
        //void OnMinOrMaxDialog(wxCommandEvent& event);
        
        // Resulutions
        void On640x480(wxCommandEvent& event);
        void On800x600(wxCommandEvent& event);
        void On1024x768(wxCommandEvent& event);
        void On1280x800(wxCommandEvent& event);
        //
        void OnFullScreen(wxCommandEvent& event);
        //
        void OnUpdateEditCut(wxUpdateUIEvent& event);
        void OnEditCut(wxCommandEvent& event);
        //
        void OnUpdateEditCopy(wxUpdateUIEvent& event);
        void OnEditCopy(wxCommandEvent& event);
        //
        void OnUpdateEditPaste(wxUpdateUIEvent& event);
        void OnEditPaste(wxCommandEvent& event);
        //
        void OnUpdateEditDelete(wxUpdateUIEvent& event);
        void OnEditDelete(wxCommandEvent& event);
        //
        void OnUpdateSelectAll(wxUpdateUIEvent& event);
        void OnSelectAll(wxCommandEvent& event);
        //
        void OnUpdateSetBackground(wxUpdateUIEvent& event);
        void OnSetBackground(wxCommandEvent& event);
        //
        void OnUpdateShowBackground(wxUpdateUIEvent& event);
        void OnShowBackground(wxCommandEvent& event);
        //
        void OnSetGrid(wxCommandEvent& event);
        void OnSetFont(wxCommandEvent& event);

        // The Align item is a "root". So we don't need separate enable/disables per sub-item
        void OnUpdateAlign(wxUpdateUIEvent& event);
        void OnAlignLeft(wxCommandEvent& event);
        void OnAlignRight(wxCommandEvent& event);
        void OnAlignTop(wxCommandEvent& event);
        void OnAlignBottom(wxCommandEvent& event);
        void OnAlignWidth(wxCommandEvent& event);
        void OnAlignHeight(wxCommandEvent& event);

        /** Processes the event raised when the user chooses the menu option 'Apply Layout Skin'.
         * @access protected 
         * @qualifier none
         * @param event Object that holds the event data.
         * @return void
         */
        void OnLayoutSkin(wxCommandEvent& event);

        /** Processes the event raised when the application updates the 'Apply Layout Skin' menu option
         * @access protected 
         * @qualifier none
         * @param event Object that holds the event data.
         * @return void
         */
        void OnUpdateLayoutSkin(wxUpdateUIEvent& event);

        /** Processes the event raised when the application updates the lock/unlock toolbar icon.
         * @access protected 
         * @qualifier none
         * @param event Object that holds the event data.
         * @return void
         */
        void OnUpdateLockSelection(wxUpdateUIEvent& event);

        /** Processes the event raised when the user chooses the lock/unlock toolbar icon.
         * @access protected 
         * @qualifier none
         * @param event Object that holds the event data.
         * @return void
         */
        void OnLockSelection(wxCommandEvent& event);
    };
}
#endif // _EDITOR_FRAME_H_
