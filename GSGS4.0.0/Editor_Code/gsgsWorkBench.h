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
#ifndef GSGS_WorkBench_H
#define GSGS_WorkBench_H

#include "gsgsPreInclude.h"
#include "gsgsWorkspacePane.h"
#include "gsgsPerspectivePane.h"
#include "gsgsOutputPane.h"
#include "gsgsDebugPane.h"
#include "gsgsBook.h"

namespace gsgs
{
    class ScenePane;
    class DebugToolbar;
    class WorkBench
    {
        friend class Frame;
    public:
        WorkBench(wxPanel* parent, WindowManager * mag);
        ~WorkBench();
        inline Book* GetMainBook() const                    { return mEditBook; }
        inline OutputPane* GetOutputPane() const            { return mOPPane; }
        inline DebuggerPane * GetDebuggerPane() const       { return mDBPane; }
        inline WorkspacePane * GetWorkspacePane() const     { return mWSPane; }
        inline PerspectivePane * GetPerspectivePane() const { return mPSPane; }
        inline ScenePane * GetScenePane() const             { return mSPane; }
        inline SplitterWindow * GetMain() const             { return mMain; }

        void ShowScenePane(bool show);
    protected:
        void CreateViewImpl();
        void OnInitDone(wxCommandEvent& event);
        void OnDebugStarted(DebugEvent& event);
        void OnDebugEnded(DebugEvent& event);
        void OnNewWorkspace(Event& e);
        void OnOpenWorkspace(Event& e);
        void OnCloseWorkspace(Event& e);
    private:
        WindowManager * mManager;
        Book* mEditBook;
        DebugToolbar* mDebugTool;
        OutputPane* mOPPane;
        WorkspacePane* mWSPane;
        PerspectivePane* mPSPane;
        ScenePane * mSPane;
        wxPanel* mEPane;
        DebuggerPane* mDBPane;
        wxPanel* mPanel;
        SplitterWindow * mMain;
        int mFullSashPos;
        bool mWorkspaeOpen;
    };
}
#endif