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
#include "SQLite3PluginCommon.h"

namespace NII
{
namespace NII_NET
{
    SQLite3Table::SQLite3Table()
    {

    }
    SQLite3Table::~SQLite3Table()
    {
        for(NCount i = 0; i < rows.Size(); i++)
            OP_DELETE(rows[i]);
    }

    void SQLite3Table::read(NetSerializer * out)
    {
        out->write(columnNames.Size());
        NCount idx1, idx2;
        for(idx1 = 0; idx1 < columnNames.Size(); idx1++)
        {
            out->write(columnNames[idx1]);
        }
        out->write(rows.Size());
        for(idx1 = 0; idx1 < rows.Size(); idx1++)
        {
            for(idx2 = 0; idx2 < rows[idx1]->entries.Size(); idx2++)
            {
                out->write(rows[idx1]->entries[idx2]);
            }
        }
    }
    void SQLite3Table::write(NetSerializer * in)
    {
        for(unsigned int i=0; i < rows.Size(); i++)
            OP_DELETE(rows[i]);
        rows.Clear(true);
        columnNames.Clear(true);

        unsigned int numColumns, numRows;
        in->read(numColumns);
        unsigned int idx1,idx2;
        RakString inputStr;
        for (idx1=0; idx1 < numColumns; idx1++)
        {
            in->read(inputStr);
            columnNames.Push(inputStr);
        }
        in->read(numRows);
        for (idx1=0; idx1 < numRows; idx1++)
        {
            SQLite3Row * row = OP_NEW<SQLite3Row>();
            rows.Push(row);
            for (idx2=0; idx2 < numColumns; idx2++)
            {
                in->read(inputStr);
                row->entries.Push(inputStr);
            }
        }
    }
}
}