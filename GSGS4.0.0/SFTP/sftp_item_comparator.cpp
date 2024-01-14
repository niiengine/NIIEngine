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

#include "sftp_item_comparator.h"

/*SFTPItemComparator::SFTPItemComparator()
{
}

SFTPItemComparator::~SFTPItemComparator()
{
}

int SFTPItemComparator::Compare(wxTreeListCtrl* treelist, unsigned column, wxTreeListItem first, wxTreeListItem second)
{
    // get the item data
    MyClientData *cdOne = dynamic_cast<MyClientData *>(treelist->GetItemData(first));
    MyClientData *cdTwo = dynamic_cast<MyClientData *>(treelist->GetItemData(second));
    
    if ( column != 0 )
        return 0;
    
    if ( !cdOne || !cdTwo )
        return 0;
    
    if ( cdOne->IsFolder() && !cdTwo->IsFolder())
        return -1;
        
    else if ( !cdOne->IsFolder() && cdTwo->IsFolder() )
        return 1;
    else 
        return cdOne->GetFullName() > cdTwo->GetFullName();
}*/
