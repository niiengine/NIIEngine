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
#include "MaterialScriptEditor.h"

BEGIN_EVENT_TABLE(MaterialScriptEditor, ScintillaEditor)
	// Scintilla
	EVT_SCI_CHARADDED(-1,   MaterialScriptEditor::OnCharAdded)
END_EVENT_TABLE()

MaterialScriptEditor::MaterialScriptEditor(wxWindow* parent, wxWindowID id /*= -1*/,
		const wxPoint& pos /*= wxDefaultPosition*/,
		const wxSize& size /*= wxDefaultSize*/,
		long style /*= wxVSCROLL*/
		) : ScintillaEditor(parent, id, pos, size, style)
{
	initialize();
}

MaterialScriptEditor::~MaterialScriptEditor()
{
}

void MaterialScriptEditor::initialize()
{
	//StyleClearAll();
	SetLexer(wxSCI_LEX_OMS);

	// Load keywords
	wxString path = wxT("../lexers/oms/keywords");
	loadKeywords(path);

	// Load call tips
	path = wxT("../lexers/oms/calltips");
	getCallTipManager().load(path);
	wxChar trigger(' ');
	getCallTipManager().addTrigger(trigger);

	// Load docs
	path = wxT("../lexers/oms/docs");
	getDocManager().load(path);
	
	// Set styles
	StyleSetForeground(wxSCI_OMS_DEFAULT, wxColour(0, 0, 0));
	StyleSetFontAttr(wxSCI_OMS_DEFAULT, 10, "Courier New", false, false, false);
	StyleSetForeground(wxSCI_OMS_COMMENT, wxColour(0, 128, 0));
	StyleSetFontAttr(wxSCI_OMS_COMMENT, 10, "Courier New", false, false, false);
	StyleSetForeground(wxSCI_OMS_PRIMARY, wxColour(0, 0, 255));
	StyleSetFontAttr(wxSCI_OMS_PRIMARY, 10, "Courier New", true, false, false);
	StyleSetForeground(wxSCI_OMS_ATTRIBUTE, wxColour(136, 0, 0));
	StyleSetFontAttr(wxSCI_OMS_ATTRIBUTE, 10, "Courier New", true, false, false);
	StyleSetForeground(wxSCI_OMS_VALUE, wxColour(160, 0, 160));
	StyleSetFontAttr(wxSCI_OMS_VALUE, 10, "Courier New", false, false, false);
	StyleSetForeground(wxSCI_OMS_NUMBER, wxColour(0, 0, 128));
	StyleSetFontAttr(wxSCI_OMS_NUMBER, 10, "Courier New", false, false, false);
}

void MaterialScriptEditor::OnCharAdded(wxScintillaEvent &event)
{
	ScintillaEditor::OnCharAdded(event);

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
				if(ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t' && ch != '{' && ch != '}') word.Prepend(ch);
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



