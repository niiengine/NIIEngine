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

#ifndef gsgsEidtorTip_H
#define gsgsEidtorTip_H

#include "gsgsPreInclude.h"
#include "gsgsTipInfo.h"
#include <wx/bitmap.h>

namespace gsgs
{
    class _gsgsAPI EditorTipWindow : public wxPanel
    {
    public:
        struct TipInfo {
            gsgsTipInfoPtr tip;
            int highlightIndex;
        };
    public:
        EditorTipWindow(wxWindow* parent);
        virtual ~EditorTipWindow();

        // API
        ///////////////////////////////////////////////////
        void AddCallTip(gsgsTipInfoPtr tip);
        void SelectSignature(const wxString& signature);
        void Remove();
        gsgsTipInfoPtr GetTip();
        bool IsEmpty();
        void Clear();
        bool IsActive();
        void Activate(wxPoint pt, int lineHeight, wxColour parentBgColour);
        void Deactivate();
        void SelectNext(int argIdxToHilight);
        void SelectPrev(int argIdxToHilight);
        void Highlight(int argIdxToHilight);
        wxString GetText();

        DECLARE_EVENT_TABLE()
        void OnPaint(wxPaintEvent& e);
        void OnEraseBg(wxEraseEvent& e);
        void OnLeftDown(wxMouseEvent& e);
        void OnEditoConfigChanged(gsgs::Event& e);
    protected:
        wxSize DoGetTipSize();
        int DoGetTextLen(wxDC& dc, const wxString& txt);
        void DoAdjustPosition();
        void DoLayoutTip();
        /**
         * @brief make the tooltip span over multiple lines
         */
        void DoMakeMultipleLineTip();
    protected:
        std::vector<TipInfo> m_tips;
        wxString m_tipText;
        wxPoint m_point;
        wxColour m_parentBgColour;
        wxString m_selectedSignature;
        wxFont m_font;
        wxArrayString m_args;
        wxString m_footer; // the line that says  "1 of 2"
        wxString m_header; // The return value line
        int m_highlighIndex;
        int m_lineHeight = 0;
    };
}
#endif
