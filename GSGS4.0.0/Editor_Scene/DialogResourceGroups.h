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
#ifndef _DIALOG_RESOURCE_GROUPS_H_
#define _DIALOG_RESOURCE_GROUPS_H_

#include "gsgsPreInclude.h"
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/hashmap.h>
#include "gsgsButton.h"

//--------------------------------------------------------------------------------
#define ID_RESOURCE_GROUPS_DIALOG 10000
#define SYMBOL_DIALOGRESOURCEGROUPS_STYLE wxCAPTION|wxRESIZE_BORDER
#define SYMBOL_DIALOGRESOURCEGROUPS_TITLE wxT("Resource Groups")
#define SYMBOL_DIALOGRESOURCEGROUPS_IDNAME ID_RESOURCE_GROUPS_DIALOG
#define SYMBOL_DIALOGRESOURCEGROUPS_SIZE wxSize(400, 300)
#define SYMBOL_DIALOGRESOURCEGROUPS_POSITION wxDefaultPosition
#define ID_GROUPS_GRID 10001
#define ID_ADDGROUP_BUTTON 10002
#define ID_REMOVEGROUP_BUTTON 10003
#define ID_DEFAULTGROUP_TEXTCTRL 10004
#define ID_DEFAULTGROUP_BUTTON 1005

// forward refs
class wxButton;
class wxTextCtrl;
class wxBoxSizer;

namespace gsgs
{
    /** This is a custom wxGridCellEditor for use with the wxWidgets wxGrid component.
     * it allows us to have a grid cell that displays a small 'browse button' when
     * edited - pressing the browse button will invoke the directory selection
     * common dialog - upon selecting something in that dialog, the path text
     * is copied into the cell.  Other than this, the cell operates the same as a
     * normal text cell.
    */
    class wxPathCellEditor : public wxEvtHandler, public wxGridCellEditor
    {
    public:
        wxPathCellEditor();

        virtual void Create(wxWindow* parent,
            wxWindowID id,
            wxEvtHandler* evtHandler);
        virtual void SetSize(const wxRect& rect);

        virtual void PaintBackground(const wxRect& rectCell, wxGridCellAttr *attr);

        virtual bool IsAcceptedKey(wxKeyEvent& event);

        virtual void BeginEdit(int row, int col, wxGrid* grid);
        virtual bool EndEdit(int row, int col, const wxGrid* grid, const wxString& oldval, wxString *newval);
        virtual void ApplyEdit(int row, int col, wxGrid* grid);

        virtual void Reset();
        virtual void StartingKey(wxKeyEvent& event);
        virtual void HandleReturn(wxKeyEvent& event);

        // parameters string format is "max_width"
        virtual void SetParameters(const wxString& params);

        virtual wxGridCellEditor* Clone() const { return new wxGridCellTextEditor; }

        virtual wxString GetValue() const;

        void OnBrowseButtonClick(wxCommandEvent& event);

    protected:
        wxTextCtrl* Text() const { return m_text; }

        void DoBeginEdit(const wxString& startValue);
        void DoReset(const wxString& startValue);

    private:
        //! max number of chars allowed
        size_t m_maxChars;
        wxString m_startValue;

        wxButton* m_browse;
        wxTextCtrl* m_text;
        wxBoxSizer* m_sizer;

        DECLARE_EVENT_TABLE()

        DECLARE_NO_COPY_CLASS(wxPathCellEditor)
    };
    //--------------------------------------------------------------------------------
    class DialogResourceGroups: public wxDialog
    {
        DECLARE_EVENT_TABLE()
    public:
        WX_DECLARE_STRING_HASH_MAP( wxString, ResourceGroupsMap );

        //! Constructor
        DialogResourceGroups( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGRESOURCEGROUPS_IDNAME, const wxString& caption = SYMBOL_DIALOGRESOURCEGROUPS_TITLE, const wxPoint& pos = SYMBOL_DIALOGRESOURCEGROUPS_POSITION, const wxSize& size = SYMBOL_DIALOGRESOURCEGROUPS_SIZE, long style = SYMBOL_DIALOGRESOURCEGROUPS_STYLE );

        //! Creation
        bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGRESOURCEGROUPS_IDNAME, const wxString& caption = SYMBOL_DIALOGRESOURCEGROUPS_TITLE, const wxPoint& pos = SYMBOL_DIALOGRESOURCEGROUPS_POSITION, const wxSize& size = SYMBOL_DIALOGRESOURCEGROUPS_SIZE, long style = SYMBOL_DIALOGRESOURCEGROUPS_STYLE );

        //! Creates the controls and sizers
        void CreateControls();

        //! wxEVT_GRID_EDITOR_SHOWN event handler for ID_GROUPS_GRID
        void OnEditorShown( wxGridEvent& event );

        //! wxEVT_GRID_CELL_EDIT event handler for ID_GROUPS_GRID
        void OnCellEdit( wxGridEvent& event );

        //! wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADDGROUP_BUTTON
        void OnAddgroupButtonClick( wxCommandEvent& event );

        //! wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_REMOVEGROUP_BUTTON
        void OnRemovegroupButtonClick( wxCommandEvent& event );

        //! wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DEFAULTGROUP_BUTTON
        void OnSetDefaultGroupButtonClick( wxCommandEvent& event );

        //! wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
        void OnOkClick( wxCommandEvent& event );

        //! wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
        void OnCancelClick( wxCommandEvent& event );

        //! Retrieves bitmap resources
        wxBitmap GetBitmapResource( const wxString& name );

        //! Retrieves icon resources
        wxIcon GetIconResource( const wxString& name );

        //! Should we show tooltips?
        static bool ShowToolTips();

        /** Invokes the dialog, this should be called to show the dialog, as
         * opposed to some other methods which may also appear to work.
        */
        void invoke();

        bool isResourceGroupDefined( const wxString& groupName ) const;
        bool renameResourceGroup( const wxString& currentName, const wxString& newName );
        bool addGroupEntry( const wxString& groupName, const wxString& directory );
        bool deleteGroupEntry( const wxString& groupName );
        bool setGroupDirectory( const wxString& groupName, const wxString& directory );
        wxString getGroupDirectory( const wxString& groupName ) const;

        bool setDefaultGroup( const wxString& groupName );
        const wxString& getDefaultGroup() const;

        ResourceGroupsMap::const_iterator getGroupIteratorStart() const;
        ResourceGroupsMap::const_iterator getGroupIteratorEnd() const;
    protected:
        //! generate a new group name
        wxString getUniqueGroupName();
        //! get index of a group name in the grid, or -1 for none.
        int getInternalGroupIndex( const wxString& groupName ) const;
        //! clears the grid (but not the internal map of groups)
        void clearGroupsGrid();
        //! populates the grid based on the internal map of groups
        void populateGrid();

        //! Map of groups defined
        ResourceGroupsMap m_resourceGroups;
        //! The current default resource group selected
        wxString m_defaultGroupName;
        //! The pre-editing map of groups defined
        ResourceGroupsMap m_backupResourceGroups;
        //! The pre-editing default resource group selected
        wxString m_backupDefaultGroupName;
        //! old group name (used for tracking edits and renames of groups)
        wxString m_oldGroupName;
        //! Read-only text control that shows the selected default group
        wxTextCtrl* m_defaultGroupTextCtrl;
        //! button to select the default group
        Button* m_setDefaultButton;
        //! the grid of defined groups and their directories.
        wxGrid* m_groupsGrid;
        //! button to add a new group
        Button* m_addGroupButton;
        //! button to delete selected group
        Button* m_removeGroupButton;
        //! main dialog ok button
        Button* m_okButton;
        //! main dialog cancel button
        Button* m_cancelButton;
        //! value used in generating unique group names (when adding a new group)
        int m_groupGUID;
    };
}
#endif
