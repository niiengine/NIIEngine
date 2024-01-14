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
#ifndef _gsgs_progressctrl__
#define _gsgs_progressctrl__

#include "gsgsPreInclude.h"

namespace gsgs
{
    class _gsgsAPI ProgressCtrl : public wxPanel
    {
    public:
        ProgressCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBORDER_NONE);
        virtual ~ProgressCtrl();

        void Update(size_t value, const wxString& msg);

        void Clear();

        inline void SetMaxRange(size_t maxRange) { m_maxRange = maxRange; }
        inline const size_t& GetMaxRange() const { return m_maxRange; }

        inline void SetMsg(const wxString& msg) { m_msg = msg; }
        inline const wxString& GetMsg() const { return m_msg; }

        inline void SetFillCol(const wxColour& col) { m_fillCol = col; }

        void OnPaint(wxPaintEvent& e);
        void OnEraseBg(wxEraseEvent& e);
        void OnSize(wxSizeEvent& event);
    private:
        wxString m_msg;
        wxColour m_fillCol;
        size_t m_maxRange;
        size_t m_currValue;
    };
}
#endif
