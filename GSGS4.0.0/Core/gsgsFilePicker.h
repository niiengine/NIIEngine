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
#ifndef gsgs_FILEPICKER_H
#define gsgs_FILEPICKER_H

#include "gsgsPreInclude.h"
#include "gsgsButton.h"
#include "wx/textctrl.h"
#include "wx/filedlg.h"
#include "wx/combobox.h"

namespace gsgs
{
    enum { wxDP_USE_TEXTCTRL = 1, wxDP_USE_COMBOBOX = 2 };

    /**
     * This control allows the user to select a directory. with a text control on its right side
     */
    class _gsgsAPI DirPicker : public wxPanel
    {
        wxTextCtrl* m_path;
        wxComboBox* m_combo;

        Button* m_button;
        wxString m_buttonCaption;
        wxString m_dlgCaption;
        long m_style;
        wxString m_defaultPos;

    private:
        void CreateControls();
        void ConnectEvents();

    protected:
        /**
         * Handle button click event
         * \param &event
         */
        virtual void OnButtonClicked(wxCommandEvent& event);
        virtual void OnText(wxCommandEvent& event);

    public:
        DirPicker(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxString& buttonCaption = wxT("..."),
                  const wxString& defaultPos = wxEmptyString,
                  const wxString& message = _("Select a folder:"),
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxDP_USE_TEXTCTRL);

        virtual ~DirPicker();

        /**
         * \return the path
         */
        wxString GetPath();

        /**
         * set a value into the text control
         * \param path path to set
         */
        void SetPath(const wxString& path);

        /**
         * Set combobox values, this option is available only when using wxDP_USE_COMBOBOX
         */
        void SetValues(const wxArrayString& values, int sel = 0);

        /**
         * Get combobox selection as int; this option is available only when using wxDP_USE_COMBOBOX
         * \return the current selection's index, or wxNOT_FOUND if there's none
         */
        int GetCurrentSelection() const;

        wxArrayString GetValues() const;
        DECLARE_EVENT_TABLE()
    };

    /**
     * This control allows the user to select a directory. with a text control on its right side
     */
    class _gsgsAPI FilePicker : public wxPanel {
    public:
        /**
         * \param parent control parent
         * \param id control id
         * \param message the dialog message
         * \param buttonCaption the browse button caption
         * \param style control style, same as wxFileDialog styles
         */
        FilePicker(wxWindow *parent, 
                   wxWindowID id = wxID_ANY, 
                   const wxString &defaultFile = wxEmptyString,
                   const wxString &message = _("Select a file:"), 
                   const wxString &wildCard = wxT("*.*"), 
                   const wxString &buttonCaption = _("Browse"), 
                   const wxPoint& pos = wxDefaultPosition, 
                   const wxSize& size = wxDefaultSize, 
                   long style = wxFD_DEFAULT_STYLE );

        virtual ~FilePicker();

        void SetPath(const wxString &path) { m_path->SetValue(path); }
        
        wxString GetPath() const { return m_path->GetValue(); }
    protected:
        /**
         * Handle button click event
         * \param &event 
         */
        virtual void OnButtonClicked(wxCommandEvent &event);
    private:
        void CreateControls();
        void ConnectEvents();
    private:
        wxTextCtrl *m_path;
        Button *m_button;
        wxString m_buttonCaption;
        wxString m_dlgCaption;
        long m_dlgStyle;
        wxString m_defaultFile;
        wxString m_wildCard;
    };
}
#endif