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

#include "gsgsFrame.h"
#include "gsgsManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsEditor.h"
#include "context_diff.h"
#include <wx/xrc/xmlres.h>
#include <wx/regex.h>

namespace gsgs
{

    ContextDiff::ContextDiff()
        : ContextBase(wxT("Diff"))
    {
    }

    ContextDiff::ContextDiff(Editor *container)
        : ContextBase(container)
    {
        SetName(wxT("diff"));
        ApplySettings();
    }

    ContextDiff::~ContextDiff()
    {
    }

    ContextBase* ContextDiff::NewInstance(Editor* container)
    {
        return new ContextDiff(container);
    }

    void ContextDiff::ApplySettings()
    {
        LexerStyle::Ptr_t lexPtr;
        if (gsgs_EditorConfig().IsOk()) {
            lexPtr = gsgs_EditorConfig().GetLexer(GetName());
        }
        GetCtrl().SetLexer(lexPtr ? lexPtr->GetLexerId() : wxSTC_LEX_NULL);
        DoApplySettings(lexPtr);
    }

    int ContextDiff::GetHyperlinkRange(int pos, int& start, int& end)
    {
        Editor &rCtrl = GetCtrl();
        int lineNum = rCtrl.LineFromPosition(pos);
        wxString line = rCtrl.GetLine(lineNum);
        wxString fileName;
        if (line.StartsWith(wxT("Index: "), &fileName) || line.StartsWith(wxT("--- "), &fileName) || line.StartsWith(wxT("+++ "), &fileName)) {
            fileName = fileName.BeforeFirst(wxT('(')).Trim(false).Trim();
            start = rCtrl.PositionFromLine(lineNum) + line.find(fileName);
            end = start + fileName.size();
            return start <= pos && pos <= end ? XRCID("open_working_copy") : wxID_NONE;
        }
        wxString edit;
        if (line.StartsWith(wxT("@@ ")) && line.Trim().EndsWith(wxT(" @@"))) {
            start = rCtrl.PositionFromLine(lineNum);
            end = rCtrl.GetLineEndPosition(lineNum);
            return XRCID("open_at_line");
        }
        return wxID_NONE;
    }

    void ContextDiff::GoHyperlink(int start, int end, int type, bool alt)
    {
        Editor &rCtrl = GetCtrl();

        wxString text = rCtrl.GetTextRange(start, end);
        wxString fileName;
        int lineNum = wxNOT_FOUND;

        if (type == XRCID("open_working_copy")) {
            fileName = text;
        } else if (type == XRCID("open_at_line")) {
            wxRegEx re(wxT("@@ -[0-9]+,[0-9]+ \\+([0-9]+),[0-9]+ @@"));
            size_t start, len;
            long line;
            if (re.IsValid() && re.Matches(text) && re.GetMatch(&start, &len, 1) && text.Mid(start,len).ToLong(&line)) {
                lineNum = line;
            }
            // FIXME: search backwards for "+++ filename", since this could be a directory diff
            rCtrl.GetFileName().GetFullName().EndsWith(wxT(".diff"), &fileName);
        }
        
        wxFileName fn = gsgs_Manager()->FindFile(fileName);
        if (fn.IsOk()) {
            gsgs_Frame()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString, lineNum);
        }
    }
}