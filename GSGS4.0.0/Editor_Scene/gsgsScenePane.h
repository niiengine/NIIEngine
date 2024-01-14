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
#ifndef gsgs_Scene_PANE_H
#define gsgs_Scene_PANE_H

#include "gsgsCommonHandler.h"
#include "gsgsNotebook.h"
#include <wx/bitmap.h>
#include <wx/timer.h>
#include <wx/slider.h>

#include "Nii.h"

using namespace NII;
namespace gsgs
{
    class SceneViewPanel : public ScrolledWindow
    {
        friend class ScenePane;
    public:
        SceneViewPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0);
        virtual ~SceneViewPanel();

        /**
         * @brief scroll to set 'firstLine' as the first visible line in the view
         */
        virtual void ScrollToRow(int firstLine);

        /**
         * @brief scroll to set 'firstColumn' as the first column in the view
         */
        virtual void ScollToColumn(int firstColumn);

        void initView(wxBoxSizer * target);

        void setView(ViewWindow * view);

        ViewWindow * getView() const    { return mView; }

        wxPanel * getSceneFrame() const { return mScene; }

        wxPanel * GetVRuler() const     { return mVRuler; }

        wxPanel * GetHRuler() const     { return mHRuler; }

        Button * GetButton() const      { return mOpen; }
    protected:
        virtual void DoSizeRuler();
    protected:
        void OnSize(wxSizeEvent& event);
        wxPanel * mScene;

        Button * mOpen;
        wxPanel * mVRuler;
        wxPanel * mHRuler;
        ViewWindow * mView;
    };
    class ScenePane : public wxPanel
    {
    public:
        ScenePane(wxWindow * parent, const wxString & caption, wxPanel * editor);
        ~ScenePane();

        const wxString & GetCaption() const { return m_caption; }

        /// Start the example
        virtual void go(void);

        virtual bool setup(void);

        virtual bool configure(void);

        virtual void chooseSceneManager(void)
        {
            // Create the SpaceManager, in this case a generic one
            mSceneMgr = mRoot->createSpaceManager((Nmark)SpaceTypeVerbose::ST_Normal, _T("ExampleSMInstance"));
        }
        virtual void createCamera(void);

        virtual void createViewports(void);
    protected:
        void CreateViewImpl();
        void OnInitDone(wxCommandEvent& event);
        void OnLeftClick(wxMouseEvent& event);
    private:
        wxString m_caption;
        wxPanel * mEditor;
        AuiCaptionEnabler m_captionEnabler;
        Engine * mRoot;
        Camera* mCamera;
        SpaceManager* mSceneMgr;
        ViewWindow * mWindow;
        String mResourcePath;
        String mConfigPath;
        SceneViewPanel * mScrollWindow;
        Choice * mWindowChoice;
        Choice * mViewportChoice;
        Choice * mCameraChoice;
        Choice * mActionChoice;
        Choice * mSpaceChoice;
        Choice * mNodeChoice;
        Choice * mObjectChoice;
        Choice * mScriptChoice;
        //int mFullSashPos;
        int mMidSashPos;
        int mSashPos;
        int mFullXThumbPos;
        int mFullYThumbPos;
        int mMidXThumbPos;
        int mMidYThumbPos;
        int mXThumbPos;
        int mYThumbPos;
        bool mLastRClick;
    };
}
#endif
