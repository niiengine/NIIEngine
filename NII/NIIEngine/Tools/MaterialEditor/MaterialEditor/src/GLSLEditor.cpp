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
#include "GLSLEditor.h"

BEGIN_EVENT_TABLE(GLSLEditor, ScintillaEditor)
	// Scintilla
	EVT_SCI_CHARADDED (-1,   GLSLEditor::OnCharAdded)
END_EVENT_TABLE()

GLSLEditor::GLSLEditor(wxWindow* parent, wxWindowID id /*= -1*/,
		const wxPoint& pos /*= wxDefaultPosition*/,
		const wxSize& size /*= wxDefaultSize*/,
		long style /*= wxVSCROLL*/
		) : ScintillaEditor(parent, id, pos, size, style)
{
	initialize();
}

GLSLEditor::~GLSLEditor()
{
}

void GLSLEditor::initialize()
{
	StyleClearAll();
	SetLexer(wxSCI_LEX_CPP);

	// Load keywords
	wxString path = wxT("../lexers/glsl/keywords");
	loadKeywords(path);

	// Load call tips
	path = wxT("../lexers/glsl/calltips");
	getCallTipManager().load(path);
	wxChar trigger('(');
	getCallTipManager().addTrigger(trigger);

	// Set styles
	StyleSetForeground(wxSCI_C_COMMENT, wxColour(0, 128, 0));
	StyleSetFontAttr(wxSCI_C_COMMENT, 10, "Courier New", false, false, false);
	StyleSetForeground(wxSCI_C_COMMENTLINE, wxColour(0, 128, 0));
	StyleSetFontAttr(wxSCI_C_COMMENTLINE, 10, "Courier New", false, false, false);
	StyleSetForeground(wxSCI_C_NUMBER, wxColour(0, 0, 128));
	StyleSetFontAttr(wxSCI_C_NUMBER, 10, "Courier New", false, false, false);
	StyleSetForeground(wxSCI_C_STRING, wxColour(200, 200, 200));
	StyleSetFontAttr(wxSCI_C_STRING, 10, "Courier New", false, false, false);
	StyleSetForeground(wxSCI_C_WORD, wxColour(0, 0, 255));
	StyleSetFontAttr(wxSCI_C_WORD, 10, "Courier New", false, false, false);
	StyleSetForeground(wxSCI_C_WORD2, wxColour(136, 0, 0));
	StyleSetFontAttr(wxSCI_C_WORD2, 10, "Courier New", false, false, false);
}

void GLSLEditor::OnCharAdded(wxScintillaEvent &event)
{
	char ch = event.GetKey();
	if(getCallTipManager().isTrigger(ch))
	{
		int lineNum = GetCurrentLine();
		if(lineNum != -1)
		{
			wxString line = GetLine(lineNum);
			int pos = GetCurrentPos() - 1;

			wxString word("");
			wxChar ch;
			while(pos)
			{
				ch = GetCharAt(--pos);
				if(ch != ' ' || ch != '\n') word.Prepend(ch);
				else break;
			}

			wxString* tips = getCallTipManager().find(word);
			if(tips != NULL)
			{
				CallTipShow(pos, *tips);
			}
		}
	}
}