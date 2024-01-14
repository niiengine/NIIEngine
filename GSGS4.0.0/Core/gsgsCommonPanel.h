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
#ifndef GSGS_COMMONPANEL_H
#define GSGS_COMMONPANEL_H

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"
#include "gsgsButton.h"
#include "gsgsSSH.h"
#include "wxterminal.h"
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/frame.h>
#include <wx/gdicmn.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/dataview.h>
#include <wx/stattext.h>

namespace gsgs
{
    class _gsgsAPI TableWithPagination : public wxPanel
    {
    public:
        TableWithPagination(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                              const wxString& name = wxPanelNameStr);
        virtual ~TableWithPagination();

        void SetLinesPerPage(int numLines);

        /**
         * @brief define the columns for this table
         * Calling this functions clears all the data from the table!
         */
        void SetColumns(const wxArrayString& columns);

        /**
         * @brief
         * @param line
         */
        void SetData(std::vector<wxArrayString>& data);

        /**
         * @brief clear all data and columns from the table
         */
        void ClearAll();

        /**
         * @brief display the nth page
         */
        void ShowPage(int nPage);
    protected:
        bool CanNext() const;
        bool CanPrev() const;

        void ClearAllItems();
        wxString MakeDisplayString(const wxString& str) const;
        void OnLineActivated(wxDataViewEvent& event);
    private:
        int m_linesPerPage;
        int m_currentPage;
        std::vector<wxArrayString> m_data;
        wxArrayString m_columns;
        gsgs::ThemedListCtrl* m_ctrl = nullptr;
        Button* m_btnNextPage = nullptr;
        Button* m_btnPrevPage = nullptr;
        wxStaticText* m_staticText = nullptr;
    };
    
    class _gsgsAPI ConsoleFrame : public wxFrame
    {
    public:
        ConsoleFrame(wxWindow* parent);
    #if USE_SFTP
        ConsoleFrame(wxWindow* parent, gsgs::SSH::Ptr_t ssh);
    #endif
        virtual ~ConsoleFrame();
        void Execute(const wxString& command, const wxString& wd);
    protected:
        void CreateViewImpl();
    #if USE_SFTP
        void OnExecuteRemoteCommand(gsgs::Event& event);
        void OnChannelReadError(gsgs::Event& event);
        void OnChannelRead(gsgs::Event& event);
        void OnChannelClosed(gsgs::Event& event);
    #endif
        void OnExitWhenDone(gsgs::Event& event);
        void OnTerminalCtrlC(gsgs::Event& event);
    protected:
        wxTerminal* m_terminal;
    #if USE_SFTP
        gsgs::SSH::Ptr_t m_ssh;
        gsgs::SSHChannel::Ptr_t m_channel;
    #endif
    };
}
#endif
