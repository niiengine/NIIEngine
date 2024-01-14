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
#include "HLSLEditor.h"

HLSLEditor::HLSLEditor(wxWindow* parent, wxWindowID id /*= -1*/,
		const wxPoint& pos /*= wxDefaultPosition*/,
		const wxSize& size /*= wxDefaultSize*/,
		long style /*= wxVSCROLL*/
		) : ScintillaEditor(parent, id, pos, size, style)
{
	initialize();
}

HLSLEditor::~HLSLEditor()
{
}

void HLSLEditor::initialize()
{
	StyleClearAll();
	SetLexer(wxSCI_LEX_OMS);

	// Load keywords
	wxString path = wxT("../lexers/hlsl/keywords");
	loadKeywords(path);
	
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
