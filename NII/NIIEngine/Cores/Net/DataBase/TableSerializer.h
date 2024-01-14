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

#ifndef __TABLE_SERIALIZER_H
#define __TABLE_SERIALIZER_H

#include "NiiNetPreInclude.h"
#include "DS_Table.h"

namespace NII
{
namespace NII_NET
{
    class _EngineAPI TableSerializer
    {
    public:
        static void SerializeTable(DataStructures::Table * in, NetSerializer * out);
        static bool DeserializeTable(Nui8 * serializedTable, Nui32 dataLength, DataStructures::Table *out);
        static bool DeserializeTable(NetSerializer * in, DataStructures::Table * out);
        static void SerializeColumns(DataStructures::Table * in, NetSerializer * out);
        static void SerializeColumns(DataStructures::Table * in, NetSerializer * out, DataStructures::List<int> &skipColumnIndices);
        static bool DeserializeColumns(NetSerializer * in, DataStructures::Table * out);
        static void SerializeRow(DataStructures::Table::Row * in, Nui32 keyIn, const DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns, NetSerializer *out);
        static void SerializeRow(DataStructures::Table::Row * in, Nui32 keyIn, const DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns, NetSerializer *out, DataStructures::List<int> &skipColumnIndices);
        static bool DeserializeRow(NetSerializer * in, DataStructures::Table * out);
        static void SerializeCell(NetSerializer * out, DataStructures::Table::Cell * cell, DataStructures::Table::ColumnType columnType);
        static bool DeserializeCell(NetSerializer * in, DataStructures::Table::Cell * cell, DataStructures::Table::ColumnType columnType);
        static void SerializeFilterQuery(NetSerializer * in, DataStructures::Table::FilterQuery *query);
        // Note that this allocates query->cell->c!
        static bool DeserializeFilterQuery(NetSerializer * out, DataStructures::Table::FilterQuery *query);
        static void SerializeFilterQueryList(NetSerializer * in, DataStructures::Table::FilterQuery *query, Nui32 numQueries, Nui32 maxQueries);
        // Note that this allocates queries, cells, and query->cell->c!. Use DeallocateQueryList to free.
        static bool DeserializeFilterQueryList(NetSerializer * out, DataStructures::Table::FilterQuery **query, Nui32 *numQueries, Nui32 maxQueries, int allocateExtraQueries=0);
        static void DeallocateQueryList(DataStructures::Table::FilterQuery * query, Nui32 numQueries);
    };
}
}

#endif

// Test code for the table
/*
#include "LightweightDatabaseServer.h"
#include "LightweightDatabaseClient.h"
#include "TableSerializer.h"
#include "NiiNetSerializer.h"
#include "StringTable.h"
#include "DS_Table.h"
void main(void)
{
    DataStructures::Table table;
    DataStructures::Table::Row *row;
    Nui32 dummydata=12345;

    // Add columns Name (string), IP (binary), score (int), and players (int).
    table.AddColumn("Name", DataStructures::Table::STRING);
    table.AddColumn("IP", DataStructures::Table::BINARY);
    table.AddColumn("Score", DataStructures::Table::NUMERIC);
    table.AddColumn("Players", DataStructures::Table::NUMERIC);
    table.AddColumn("Empty Test Column", DataStructures::Table::STRING);
    N_assert(table.GetColumnCount()==5);
    row=table.AddRow(0);
    N_assert(row);
    row->UpdateCell(0,"Kevin Jenkins");
    row->UpdateCell(1,sizeof(dummydata), (char*)&dummydata);
    row->UpdateCell(2,5);
    row->UpdateCell(3,10);
    //row->UpdateCell(4,"should be unique");

    row=table.AddRow(1);
    row->UpdateCell(0,"Kevin Jenkins");
    row->UpdateCell(1,sizeof(dummydata), (char*)&dummydata);
    row->UpdateCell(2,5);
    row->UpdateCell(3,15);

    row=table.AddRow(2);
    row->UpdateCell(0,"Kevin Jenkins");
    row->UpdateCell(1,sizeof(dummydata), (char*)&dummydata);
    row->UpdateCell(2,5);
    row->UpdateCell(3,20);

    row=table.AddRow(3);
    N_assert(row);
    row->UpdateCell(0,"Kevin Jenkins");
    row->UpdateCell(1,sizeof(dummydata), (char*)&dummydata);
    row->UpdateCell(2,15);
    row->UpdateCell(3,5);
    row->UpdateCell(4,"col index 4");

    row=table.AddRow(4);
    N_assert(row);
    row->UpdateCell(0,"Kevin Jenkins");
    row->UpdateCell(1,sizeof(dummydata), (char*)&dummydata);
    //row->UpdateCell(2,25);
    row->UpdateCell(3,30);
    //row->UpdateCell(4,"should be unique");

    row=table.AddRow(5);
    N_assert(row);
    row->UpdateCell(0,"Kevin Jenkins");
    row->UpdateCell(1,sizeof(dummydata), (char*)&dummydata);
    //row->UpdateCell(2,25);
    row->UpdateCell(3,5);
    //row->UpdateCell(4,"should be unique");

    row=table.AddRow(6);
    N_assert(row);
    row->UpdateCell(0,"Kevin Jenkins");
    row->UpdateCell(1,sizeof(dummydata), (char*)&dummydata);
    row->UpdateCell(2,35);
    //row->UpdateCell(3,40);
    //row->UpdateCell(4,"should be unique");

    row=table.AddRow(7);
    N_assert(row);
    row->UpdateCell(0,"Bob Jenkins");

    row=table.AddRow(8);
    N_assert(row);
    row->UpdateCell(0,"Zack Jenkins");

    // Test multi-column sorting
    DataStructures::Table::Row *rows[30];
    DataStructures::Table::SortQuery queries[4];
    queries[0].columnIndex=0;
    queries[0].mOP=DataStructures::Table::QS_INCREASING_ORDER;
    queries[1].columnIndex=1;
    queries[1].mOP=DataStructures::Table::QS_INCREASING_ORDER;
    queries[2].columnIndex=2;
    queries[2].mOP=DataStructures::Table::QS_INCREASING_ORDER;
    queries[3].columnIndex=3;
    queries[3].mOP=DataStructures::Table::QS_DECREASING_ORDER;
    table.SortTable(queries, 4, rows);
    Nui32 i;
    char out[256];
    N_printf("Sort: Ascending except for column index 3\n");
    for (i=0; i < table.GetRowCount(); i++)
    {
        table.PrintRow(out,256,',',true, rows[i]);
        N_printf("%s\n", out);
    }

    // Test query:
    // Don't return column 3, and swap columns 0 and 2
    Nui32 columnsToReturn[4];
    columnsToReturn[0]=2;
    columnsToReturn[1]=1;
    columnsToReturn[2]=0;
    columnsToReturn[3]=4;
    DataStructures::Table resultsTable;
    table.QueryTable(columnsToReturn,4,0,0,&resultsTable);
    N_printf("Query: Don't return column 3, and swap columns 0 and 2:\n");
    for (i=0; i < resultsTable.GetRowCount(); i++)
    {
        resultsTable.PrintRow(out,256,',',true, resultsTable.GetRowByIndex(i));
        N_printf("%s\n", out);
    }

    // Test filter:
    // Only return rows with column index 4 empty
    DataStructures::Table::FilterQuery inclusionFilters[3];
    inclusionFilters[0].columnIndex=4;
    inclusionFilters[0].mOP=DataStructures::Table::QF_IS_EMPTY;
    // inclusionFilters[0].cellValue; // Unused for IS_EMPTY
    table.QueryTable(0,0,inclusionFilters,1,&resultsTable);
    N_printf("Filter: Only return rows with column index 4 empty:\n");
    for (i=0; i < resultsTable.GetRowCount(); i++)
    {
        resultsTable.PrintRow(out,256,',',true, resultsTable.GetRowByIndex(i));
        N_printf("%s\n", out);
    }

    // Column 5 empty and column 0 == Kevin Jenkins
    inclusionFilters[0].columnIndex=4;
    inclusionFilters[0].mOP=DataStructures::Table::QF_IS_EMPTY;
    inclusionFilters[1].columnIndex=0;
    inclusionFilters[1].mOP=DataStructures::Table::QF_EQUAL;
    inclusionFilters[1].cellValue.Set("Kevin Jenkins");
    table.QueryTable(0,0,inclusionFilters,2,&resultsTable);
    N_printf("Filter: Column 5 empty and column 0 == Kevin Jenkins:\n");
    for (i=0; i < resultsTable.GetRowCount(); i++)
    {
        resultsTable.PrintRow(out,256,',',true, resultsTable.GetRowByIndex(i));
        N_printf("%s\n", out);
    }

    NetSerializer bs;
    N_printf("PreSerialize:\n");
    for (i=0; i < table.GetRowCount(); i++)
    {
        table.PrintRow(out,256,',',true, table.GetRowByIndex(i));
        N_printf("%s\n", out);
    }
    StringCompressor::AddReference();
    TableSerializer::read(&table, &bs);
    TableSerializer::import(&bs, &table);
    StringCompressor::RemoveReference();
    N_printf("PostDeserialize:\n");
    for (i=0; i < table.GetRowCount(); i++)
    {
        table.PrintRow(out,256,',',true, table.GetRowByIndex(i));
        N_printf("%s\n", out);
    }
    int a=5;
}
*/
