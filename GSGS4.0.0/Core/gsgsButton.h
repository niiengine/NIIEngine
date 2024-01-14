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
#ifndef gsgs_BUTTON_H
#define gsgs_BUTTON_H

#include "gsgsPreInclude.h"
#include "gsgsStyleManager.h"
#include <wx/validate.h>
#include <wx/control.h>
#include <wx/menu.h>
#include <wx/choice.h>

namespace gsgs
{
    class _gsgsAPI Button : public wxControl
    {
    public:
        enum State
        {
            BST_Disable = 0,
            BST_Visable = 1,
            BST_Active = 2,
            BST_Normal = 4,
            BST_Press = 8,
            BST_Hover = 16
        };

        enum eDrawingFlags {
            kDrawingFlagEnabled = (1 << 0),
            kDrawingFlagChecked = (1 << 1),
        };
    public:
        Button();
        Button(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& label = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
        bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& label = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
        virtual ~Button();

        void SetColours(const Colour& colours);

        const Colour& GetColours() const        { return m_colours; }

        void SetText(const wxString& text);

        const wxString& GetText() const         { return m_text; }

        void SetDefault();
        void SetHasDropDownMenu(bool hasDropDownMenu);
        bool HasDropDownMenu() const            { return m_hasDropDownMenu; }
        void SetBitmap(const wxBitmap& bmp);
        const wxBitmap& GetBitmap() const;

        /**
         * @brief display a menu for the user aligned to the button
         * @param menu
         */
        void ShowMenu(wxMenu& menu);
    protected:
        // Event handlers
        void OnPaint(wxPaintEvent& event);
        void OnErasebg(wxEraseEvent& event);
        void OnButton(wxCommandEvent& event);
        void OnLeftDown(wxMouseEvent& event);
        void OnLeftUp(wxMouseEvent& event);
        void OnCaptureLost(wxMouseCaptureLostEvent & e);
        void OnEnter(wxMouseEvent& event);
        void OnLeave(wxMouseEvent& event);
        void OnFocus(wxFocusEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnIdle(wxIdleEvent& event);
        void OnSize(wxSizeEvent& event);

        // Overridable
        virtual void Render(wxDC& dc);

        bool IsPressed() const                  { return m_state == Button::State::BST_Press; }
        bool IsHover() const                    { return m_state == Button::State::BST_Hover; }
        bool IsNormal() const                   { return m_state == Button::State::BST_Normal; }
        void SetPressed()                       { m_state = Button::State::BST_Press; }
        void SetNormal()                        { m_state = Button::State::BST_Normal; }
        void SetHover()                         { m_state = Button::State::BST_Hover; }
    protected:
        void Initialise();
        wxSize GetBestSize() const;
        void PostClickEvent();
        size_t GetDrawingFlags() const;
    protected:
        Button::State m_state = Button::State::BST_Normal;
        size_t m_buttonStyle = 0;
        size_t m_lastPaintFlags = 0;
        Colour m_colours;
        wxString m_text;
        wxBitmap m_bitmap;
        bool m_hasDropDownMenu = false;
    };

    class _gsgsAPI DefaultButton : public Button
    {
    public:
        DefaultButton();
        DefaultButton(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
        bool Create(wxWindow* parent, wxWindowID id, const wxString& label = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator, const wxString& name = "button");
        virtual ~DefaultButton();
    protected:
        void OnTheme(wxCommandEvent& event);
        void ApplyTheme();
    };

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CHOICE_MENU_SHOWING, wxNotifyEvent);

    class _gsgsAPI Choice : public Button
    {
    public:
        Choice();
        Choice(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices,
            long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxChoiceNameStr);
        bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices,
            long style = 0, const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr);
        virtual ~Choice();

        /**
         * @brief Finds an item whose label matches the given string.
         */
        int FindString(const wxString& s, bool caseSensitive = false) const;

        /**
         * @brief return the number of strings in the control
         * @return
         */
        size_t GetCount() const                         { return m_choices.size(); }

        /**
         * @brief return the selected item index
         * @return
         */
        int GetSelection() const                        { return m_selection;  }

        /**
         * @brief Returns the label of the item with the given index.
         */
        wxString GetString(size_t index) const;

        /**
         * @brief Sets the selection to the given item n
         */
        void SetSelection(size_t index);

        /**
         * @brief Sets the label for the given item.
         */
        void SetString(size_t index, const wxString& str);

        /**
         * @brief Returns the label of the selected item or an empty string if no item is selected.
         */
        wxString GetStringSelection() const             { return GetString((size_t)m_selection);  }

        /**
         * @brief Selects the item with the specified string in the control
         */
        void SetStringSelection(const wxString& str);

        /**
         * @brief append string
         */
        int Append(const wxString& str);

        /**
         * @brief append items
         */
        void Append(const wxArrayString& items);

        /**
         * @brief clear the current control strings
         */
        void Clear();

        /**
         * @brief replace the current items in the control
         */
        void Set(const wxArrayString& items, const wxString & text = gsgsEmptyString);
    protected:
        void OnClick(wxCommandEvent& event);

        void Render(wxDC& dc);
    private:
        std::vector<wxString> m_choices;
        int m_selection = wxNOT_FOUND;
        bool m_popupShown = false;
    };

    class _gsgsAPI DefaultChoice : public Choice
    {
    public:
        DefaultChoice();
        DefaultChoice(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr);
        bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices,
            long style = 0, const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr);
        virtual ~DefaultChoice();
    protected:
        void OnTheme(wxCommandEvent& event);
        void ApplyTheme();
    };

    class _gsgsAPI DialogButtonSizer : public wxBoxSizer
    {
    public:
        // Constructor just creates a new wxBoxSizer, not much else.
        // Box sizer orientation is automatically determined here:
        // vertical for PDAs, horizontal for everything else?
        DialogButtonSizer();

        // Checks button ID against system IDs and sets one of the pointers below
        // to this button. Does not do any sizer-related things here.
        void AddButton(Button *button);

        // Use these if no standard ID can/should be used
        void SetAffirmativeButton(Button *button);
        void SetNegativeButton(Button *button);
        void SetCancelButton(Button *button);

        // All platform-specific code here, checks which buttons exist and add
        // them to the sizer accordingly.
        // Note - one potential hack on Mac we could use here,
        // if m_buttonAffirmative is wxID_SAVE then ensure wxID_SAVE
        // is set to _("Save") and m_buttonNegative is set to _("Don't Save")
        // I wouldn't add any other hacks like that into here,
        // but this one I can see being useful.
        void Realize();

        Button *GetAffirmativeButton() const { return m_buttonAffirmative; }
        Button *GetApplyButton() const { return m_buttonApply; }
        Button *GetNegativeButton() const { return m_buttonNegative; }
        Button *GetCancelButton() const { return m_buttonCancel; }
        Button *GetHelpButton() const { return m_buttonHelp; }

    protected:
        Button *m_buttonAffirmative;  // wxID_OK, wxID_YES, wxID_SAVE go here
        Button *m_buttonApply;        // wxID_APPLY
        Button *m_buttonNegative;     // wxID_NO
        Button *m_buttonCancel;       // wxID_CANCEL, wxID_CLOSE
        Button *m_buttonHelp;         // wxID_HELP, wxID_CONTEXT_HELP

    private:
        DECLARE_CLASS(DialogButtonSizer)
        wxDECLARE_NO_COPY_CLASS(DialogButtonSizer);
    };
}
#endif
