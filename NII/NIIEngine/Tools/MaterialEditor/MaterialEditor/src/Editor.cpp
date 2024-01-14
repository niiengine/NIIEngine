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
#include "Editor.h"

#include <wx/control.h>

#include "EditorEventArgs.h"
#include "EditorInput.h"

Editor::Editor()
: mEditorInput(NULL), mName("Editor")
{
	registerEvents();
}

Editor::Editor(EditorInput* input)
: mEditorInput(input)
{
	registerEvents();
}

Editor::~Editor()
{
}

wxControl* Editor::getControl() const
{
	return mControl;
}

void Editor::setControl(wxControl* control)
{
	mControl = control;
}

void Editor::registerEvents()
{
	registerEvent(NameChanged);
	registerEvent(DirtyStateChanged);
}


EditorInput* Editor::getEditorInput() const
{
	return mEditorInput;
}

void Editor::setEditorInput(EditorInput* input)
{
	mEditorInput = input;
}

EditorContributor* Editor::getEditorContributor() const
{
	return NULL;
}

const wxString& Editor::getName() const
{
	return mName;
}

void Editor::setName(const wxString& name)
{
	mName = name;

	fireEvent(NameChanged, EditorEventArgs(this));
}

void Editor::activate()
{
	// Do nothing
}

void Editor::deactivate()
{
	// Do nothing
}

bool Editor::isDirty()
{
	return false;
}

void Editor::save()
{
	// Do nothing
}

void Editor::saveAs()
{
	// Do nothing
}

bool Editor::isSaveAsAllowed()
{
	return false;
}

bool Editor::isRedoable()
{
	return false;
}

void Editor::redo()
{
	// Do nothing
}

bool Editor::isUndoable()
{
	return false;
}

void Editor::undo()
{
	// Do nothing
}

bool Editor::isCuttable()
{
	return false;
}

void Editor::cut()
{
	// Do nothing
}

bool Editor::isCopyable()
{
	return false;
}

void Editor::copy()
{
	// Do nothing
}

bool Editor::isPastable()
{
	return false;
}

void Editor::paste()
{
	// Do nothing
}
