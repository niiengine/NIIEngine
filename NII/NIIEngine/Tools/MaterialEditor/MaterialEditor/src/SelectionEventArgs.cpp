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
#include "SelectionEventArgs.h"

#include <boost/any.hpp>

SelectionEventArgs::SelectionEventArgs(SelectionList& selection)
: mSelection(selection)
{
}

SelectionEventArgs::SelectionEventArgs(boost::any& source, SelectionList& selection)
: mSource(source), mSelection(selection)
{
}

SelectionEventArgs::~SelectionEventArgs()
{
}
	
const boost::any& SelectionEventArgs::getSource() const
{
	return mSource;
}

const SelectionList& SelectionEventArgs::getSelection() const
{
	return mSelection;
}