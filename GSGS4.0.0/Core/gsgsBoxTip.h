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

#ifndef gsgsBOXTIPWINDOW_H
#define gsgsBOXTIPWINDOW_H

#include "gsgsPreInclude.h"
#include <wx/popupwin.h>

_gsgsAPI extern const wxEventType wxEVT_TIP_BTN_CLICKED_UP;
_gsgsAPI extern const wxEventType wxEVT_TIP_BTN_CLICKED_DOWN;

namespace gsgs
{
    class _gsgsAPI BoxTipWindow : public wxPopupWindow
    {
    public:
        BoxTipWindow(wxWindow* parent, bool manipulateText, const wxString& tip, size_t numOfTips,
                       bool simpleTip = false);
        BoxTipWindow(wxWindow* parent, bool manipulateText, const wxString& tip);
        virtual ~BoxTipWindow();

        /**
         * @brief position the tip next to 'win' and show it
         * if focusEditor is NOT null, the editor will gain the focus once
         * the tip is shown
         */
        void PositionRelativeTo(wxWindow* win, wxPoint caretPos, IEditor* focusEdior = NULL);

        /**
         * @brief position this window to the left of 'win'
         * if focusEditor is NOT null, the editor will gain the focus once
         * the tip is shown
         */
        void PositionLeftTo(wxWindow* win, IEditor* focusEditor = NULL);
        /**
         * @brief position and show the tip at a given location
         */
        void PositionAt(const wxPoint& pt, IEditor* focusEdior = NULL);
    protected:
        void OnPaint(wxPaintEvent& e);
        void OnEraseBG(wxEraseEvent& e);
        void OnMouseLeft(wxMouseEvent& e);
        wxRect DoPrintText(wxDC& dc, wxString& text, wxPoint& pt, size_t& maxWidth);
        wxString DoStripMarkups();
        void DoInitialize(const wxString& tip, size_t numOfTips, bool simpleTip);
        void DoDrawTip(wxDC& dc, size_t& max_width);
    protected:
        class Links
        {
        public:
            wxString m_url;
            wxRect m_rect;
        };

        wxString m_tip;
        wxFont m_codeFont;
        wxFont m_commentFont;
        wxRect m_leftTipRect;
        wxRect m_rightTipRect;
        std::vector<Links> m_links;
        int m_lineHeight;
        size_t m_numOfTips;
        bool m_useLightColours;
    };
}
#endif