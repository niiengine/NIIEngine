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
#ifndef _SELECTIONEVENTARGS_H_
#define _SELECTIONEVENTARGS_H_

#include <list>

#include <boost/any.hpp>

#include "EventArgs.h"

typedef std::list<boost::any> SelectionList;

class SelectionEventArgs : public EventArgs
{
public:
	SelectionEventArgs(SelectionList& selection);
	SelectionEventArgs(boost::any& source, SelectionList& selection);
	virtual ~SelectionEventArgs();
	
	const boost::any& getSource() const;
	const SelectionList& getSelection() const;

protected:
	boost::any mSource;
	SelectionList mSelection;
};

#endif _SELECTIONEVENTARGS_H_