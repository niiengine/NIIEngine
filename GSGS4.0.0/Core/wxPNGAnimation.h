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

#ifndef WXPNGANIMATION_H
#define WXPNGANIMATION_H

#include "gsgsPreInclude.h"
#include <wx/bitmap.h>
#include <wx/vector.h>
#include <wx/timer.h>
#include <wx/colour.h>

class _gsgsAPI wxPNGAnimation : public wxPanel
{
    wxVector<wxBitmap> m_bitmaps;
    size_t m_index;
    wxTimer* m_timer;
    wxColour m_bgColour;

protected:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBG(wxEraseEvent& event);
    void OnTimer(wxTimerEvent& event);

public:
    /**
     * @brief construct a bitmap spinner from a bitmap map
     * The bitmap map must contains all bitmaps that consists the animation
     * Each bitmap in the map must have the same size (height and width)
     * @param parent parent window
     * @param pngSprite the bitmap map
     * @param spriteOrientation the sprite orientation (wxVERTICAL || wxHORIZONTAL)
     * @param singleImageSize the size of a single bitmap within the map
     * @param id the control ID
     */
    wxPNGAnimation(wxWindow* parent,
                   const wxBitmap& pngSprite,
                   wxOrientation spriteOrientation,
                   const wxSize& singleImageSize,
                   wxWindowID id = wxID_ANY);
    virtual ~wxPNGAnimation();

    /**
     * @brief start the animation
     * @param refreshRate refresh the animation every milliseconds
     */
    void Start(int refereshRate);
    /**
     * @brief stop the animation
     */
    void Stop();

    /**
     * @brief set the animation background colour
     * @param colour
     */
    void SetPanelColour(const wxColour& colour);
    
    /**
     * @brief is the animation running?
     */
    bool IsRunning() const {
        return m_timer && m_timer->IsRunning();
    }
};

#endif // WXPNGANIMATION_H
