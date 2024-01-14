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

#ifndef __quickoutlinedlg__
#define __quickoutlinedlg__

#include "symbol_tree.h"
#include "stack"
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/treectrl.h>
#include <wx/timer.h>

extern const wxEventType wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED;

namespace gsgs
{
    /// This class represents the GUI tree for the C++ symbols
    class CppSymbolTree : public SymbolTree
    {
    public:
        CppSymbolTree();

        /// Nothing special here, just call our parent constructor
        CppSymbolTree(wxWindow* parent,
            const wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS);

        /// destructor
        virtual ~CppSymbolTree() {};
    protected:
        virtual void OnItemActivated(wxTreeEvent& event);
        bool DoItemActivated(wxTreeItemId item, wxEvent& event, bool notify);
        void FindAndSelect(Editor* editor, wxString& pattern, const wxString& name);

        DECLARE_DYNAMIC_CLASS(CppSymbolTree)
    private:
        std::stack<wxTreeItemId> m_itemsStack;
    };

    ///////////////////////////////////////////////////////////////////////////////
    /// Class QuickOutlineDlg
    ///////////////////////////////////////////////////////////////////////////////
    class QuickOutlineDlg : public wxDialog
    {
    public:
        QuickOutlineDlg(wxWindow* parent, const wxString& fileName, int id = wxID_ANY, const wxString & title = wxEmptyString,
            const wxPoint & pos = wxDefaultPosition, const wxSize &size = gsgsMinDlgSize, int style = gsgsDlgStyle);
        virtual ~QuickOutlineDlg();
    protected:
        void OnItemSelected(wxCommandEvent& e);
        //void OnKeyDown(wxKeyEvent& e);
        void OnCharHook(wxKeyEvent& e);
        void DoParseActiveBuffer();
    protected:
        wxStaticLine* m_staticline1;
        CppSymbolTree* m_treeOutline;
        wxString m_fileName;
    };
}
#endif