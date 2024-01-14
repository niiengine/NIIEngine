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

#include "DS_Table.h"
#include "DS_OrderedList.h"
#include <string.h>

using namespace DataStructures;

#ifdef _MSC_VER
#pragma warning( push )
#endif
    //------------------------------------------------------------------------
    void ExtendRows(Table::Row* input, int index)
    {
        (void) index;
        input->cells.Insert(N_new Table::Cell());
    }
    //------------------------------------------------------------------------
    void FreeRow(Table::Row* input, int index)
    {
        (void) index;

        Nui32 i;
        for (i=0; i < input->cells.Size(); i++)
        {
            N_delete input->cells[i];
        }
        N_delete input;
    }
    //------------------------------------------------------------------------
    Table::Cell::Cell()
    {
        isEmpty=true;
        c=0;
        ptr=0;
        i=0.0;
    }
    //------------------------------------------------------------------------
    Table::Cell::~Cell()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    Table::Cell& Table::Cell::operator = ( const Table::Cell& input )
    {
        isEmpty=input.isEmpty;
        i=input.i;
        ptr=input.ptr;
        if (c)
            N_free(c);
        if (input.c)
        {
            c = (char *) N_alloc((NCount) i);
            memcpy(c, input.c, (NCount)i);
        }
        else
            c=0;
        return *this;
    }
    //------------------------------------------------------------------------
    Table::Cell::Cell( const Table::Cell & input)
    {
        isEmpty=input.isEmpty;
        i=input.i;
        ptr=input.ptr;
        if (input.c)
        {
            if (c)
                N_free(c);
            c =  (char*) N_alloc((NCount)i);
            memcpy(c, input.c, (NCount)i);
        }
    }
    //------------------------------------------------------------------------
    void Table::Cell::Set(NIId input)
    {
        Clear();
        i=input;
        c=0;
        ptr=0;
        isEmpty=false;
    }
    //------------------------------------------------------------------------
    void Table::Cell::Set(Nui32 input)
    {
        Set((int) input);
    }
    //------------------------------------------------------------------------
    void Table::Cell::Set(int input)
    {
        Clear();
        i = (NIId)input;
        c = 0;
        ptr = 0;
        isEmpty = false;
    }
    //------------------------------------------------------------------------
    void Table::Cell::Set(const char * input)
    {
        Clear();

        if (input)
        {
            i=(int)strlen(input)+1;
            c =  (char*) N_alloc((NCount)i);
            strcpy(c, input);
        }
        else
        {
            c=0;
            i=0;
        }
        ptr=0;
        isEmpty=false;
    }
    //------------------------------------------------------------------------
    void Table::Cell::Set(const char * input, int inputLength)
    {
        Clear();
        if (input)
        {
            c = (char*) N_alloc(inputLength);
            i=inputLength;
            memcpy(c, input, inputLength);
        }
        else
        {
            c=0;
            i=0;
        }
        ptr=0;
        isEmpty=false;
    }
    //------------------------------------------------------------------------
    void Table::Cell::SetPtr(void* p)
    {
        Clear();
        c=0;
        ptr=p;
        isEmpty=false;
    }
    //------------------------------------------------------------------------
    void Table::Cell::Get(int *output)
    {
        N_assert(isEmpty==false);
        int o = (int) i;
        *output=o;
    }
    //------------------------------------------------------------------------
    void Table::Cell::Get(NIId *output)
    {
        N_assert(isEmpty==false);
        *output=i;
    }
    //------------------------------------------------------------------------
    void Table::Cell::Get(char *output)
    {
        N_assert(isEmpty==false);
        strcpy(output, c);
    }
    //------------------------------------------------------------------------
    void Table::Cell::Get(char *output, int *outputLength)
    {
        N_assert(isEmpty==false);
        memcpy(output, c, (int) i);
        if (outputLength)
            *outputLength=(int) i;
    }
    //------------------------------------------------------------------------
    RakString Table::Cell::ToString(ColumnType columnType)
    {
        if (isEmpty)
            return RakString();

        if (columnType==NUMERIC)
        {
            return RakString("%f", i);
        }
        else if (columnType==STRING)
        {
            return RakString(c);
        }
        else if (columnType==BINARY)
        {
            return RakString("<Binary>");
        }
        else if (columnType==POINTER)
        {
            return RakString("%p", ptr);
        }

        return RakString();
    }
    //------------------------------------------------------------------------
    Table::Cell::Cell(NIId numericValue, char *charValue, void *ptr, ColumnType type)
    {
        SetByType(numericValue,charValue,ptr,type);
    }
    //------------------------------------------------------------------------
    void Table::Cell::SetByType(NIId numericValue, char *charValue, void *ptr, ColumnType type)
    {
        isEmpty=true;
        if (type==NUMERIC)
        {
            Set(numericValue);
        }
        else if (type==STRING)
        {
            Set(charValue);
        }
        else if (type==BINARY)
        {
            Set(charValue, (int) numericValue);
        }
        else if (type==POINTER)
        {
            SetPtr(ptr);
        }
        else
        {
            ptr=(void*) charValue;
        }
    }
    //------------------------------------------------------------------------
    Table::ColumnType Table::Cell::EstimateColumnType(void) const
    {
        if (c)
        {
            if (i!=0.0f)
                return BINARY;
            else
                return STRING;
        }

        if (ptr)
            return POINTER;
        return NUMERIC;
    }
    //------------------------------------------------------------------------
    void Table::Cell::Clear(void)
    {
        if (isEmpty==false && c)
        {
            N_free(c);
            c=0;
        }
        isEmpty=true;
    }
    //------------------------------------------------------------------------
    Table::ColumnDescriptor::ColumnDescriptor()
    {

    }
    //------------------------------------------------------------------------
    Table::ColumnDescriptor::~ColumnDescriptor()
    {

    }
    //------------------------------------------------------------------------
    Table::ColumnDescriptor::ColumnDescriptor(const char cn[_TABLE_MAX_COLUMN_NAME_LENGTH], ColumnType ct)
    {
        columnType=ct;
        strcpy(columnName, cn);
    }
    //------------------------------------------------------------------------
    void Table::Row::UpdateCell(Nui32 columnIndex, NIId value)
    {
        cells[columnIndex]->Clear();
        cells[columnIndex]->Set(value);

    //    cells[columnIndex]->i=value;
    //    cells[columnIndex]->c=0;
    //    cells[columnIndex]->isEmpty=false;
    }
    //------------------------------------------------------------------------
    void Table::Row::UpdateCell(Nui32 columnIndex, const char *str)
    {
        cells[columnIndex]->Clear();
        cells[columnIndex]->Set(str);
    }
    //------------------------------------------------------------------------
    void Table::Row::UpdateCell(Nui32 columnIndex, int byteLength, const char *data)
    {
        cells[columnIndex]->Clear();
        cells[columnIndex]->Set(data,byteLength);
    }
    //------------------------------------------------------------------------
    Table::Table()
    {
    }
    //------------------------------------------------------------------------
    Table::~Table()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    Nui32 Table::AddColumn(const char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH], ColumnType columnType)
    {
        if (columnName[0]==0)
            return (Nui32) -1;

        // Add this column.
        columns.Insert(Table::ColumnDescriptor(columnName, columnType));

        // Extend the rows by one
        rows.ForEachData(ExtendRows);

        return columns.Size()-1;
    }
    //------------------------------------------------------------------------
    void Table::RemoveColumn(Nui32 columnIndex)
    {
        if (columnIndex >= columns.Size())
            return;

        columns.RemoveAtIndex(columnIndex);

        // Remove this index from each row.
        int i;
        DataStructures::Page<Nui32, Row*, _TABLE_BPLUS_TREE_ORDER> *cur = rows.GetListHead();
        while (cur)
        {
            for (i=0; i < cur->size; i++)
            {
                N_delete cur->mData[i]->cells[columnIndex];
                cur->mData[i]->cells.RemoveAtIndex(columnIndex);
            }

            cur=cur->next;
        }
    }
    //------------------------------------------------------------------------
    Nui32 Table::ColumnIndex(const char *columnName) const
    {
        Nui32 columnIndex;
        for (columnIndex=0; columnIndex<columns.Size(); columnIndex++)
            if (strcmp(columnName, columns[columnIndex].columnName)==0)
                return columnIndex;
        return (Nui32)-1;
    }
    //------------------------------------------------------------------------
    Nui32 Table::ColumnIndex(char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH]) const
    {
        return ColumnIndex((const char *) columnName);
    }
    //------------------------------------------------------------------------
    char * Table::ColumnName(Nui32 index) const
    {
        if (index >= columns.Size())
            return 0;
        else
            return (char*)columns[index].columnName;
    }
    //------------------------------------------------------------------------
    Table::ColumnType Table::GetColumnType(Nui32 index) const
    {
        if (index >= columns.Size())
            return (Table::ColumnType) 0;
        else
            return columns[index].columnType;
    }
    //------------------------------------------------------------------------
    Nui32 Table::GetColumnCount(void) const
    {
        return columns.Size();
    }
    //------------------------------------------------------------------------
    Nui32 Table::GetRowCount(void) const
    {
        return rows.Size();
    }
    //------------------------------------------------------------------------
    Table::Row* Table::AddRow(Nui32 rowId)
    {
        Row *newRow;
        newRow = N_new Row();
        if (rows.Insert(rowId, newRow)==false)
        {
            N_delete newRow;
            return 0; // Already exists
        }
        Nui32 rowIndex;
        for (rowIndex=0; rowIndex < columns.Size(); rowIndex++)
            newRow->cells.Insert(N_new Table::Cell());
        return newRow;
    }
    //------------------------------------------------------------------------
    Table::Row* Table::AddRow(Nui32 rowId, DataStructures::List<Cell> &initialCellValues)
    {
        Row *newRow = N_new Row();
        Nui32 rowIndex;
        for (rowIndex=0; rowIndex < columns.Size(); rowIndex++)
        {
            if (rowIndex < initialCellValues.Size() && initialCellValues[rowIndex].isEmpty==false)
            {
                Table::Cell *c;
                c = N_new Table::Cell();
                c->SetByType(initialCellValues[rowIndex].i,initialCellValues[rowIndex].c,initialCellValues[rowIndex].ptr,columns[rowIndex].columnType);
                newRow->cells.Insert(c);
            }
            else
                newRow->cells.Insert(N_new Table::Cell());
        }
        rows.Insert(rowId, newRow);
        return newRow;
    }
    //------------------------------------------------------------------------
    Table::Row * Table::AddRow(Nui32 rowId, DataStructures::List<Cell*> &initialCellValues, bool copyCells)
    {
        Row * newRow = N_new Row();
        Nui32 rowIndex;
        for (rowIndex=0; rowIndex < columns.Size(); rowIndex++)
        {
            if (rowIndex < initialCellValues.Size() && initialCellValues[rowIndex] && initialCellValues[rowIndex]->isEmpty==false)
            {
                if (copyCells==false)
                    newRow->cells.Insert(N_new Table::Cell(initialCellValues[rowIndex]->i,
                        initialCellValues[rowIndex]->c, initialCellValues[rowIndex]->ptr,
                            columns[rowIndex].columnType));
                else
                {
                    Table::Cell *c = N_new Table::Cell();
                    newRow->cells.Insert(c);
                    *c=*(initialCellValues[rowIndex]);
                }
            }
            else
                newRow->cells.Insert(N_new Table::Cell());
        }
        rows.Insert(rowId, newRow);
        return newRow;
    }
    //------------------------------------------------------------------------
    Table::Row * Table::AddRowColumns(Nui32 rowId, Row *row, DataStructures::List<Nui32> columnIndices)
    {
        Row *newRow = N_new Row();
        Nui32 columnIndex;
        for (columnIndex=0; columnIndex < columnIndices.Size(); columnIndex++)
        {
            if (row->cells[columnIndices[columnIndex]]->isEmpty==false)
            {
                newRow->cells.Insert(N_new Table::Cell(
                    row->cells[columnIndices[columnIndex]]->i,
                    row->cells[columnIndices[columnIndex]]->c,
                    row->cells[columnIndices[columnIndex]]->ptr,
                    columns[columnIndex].columnType
                    ));
            }
            else
            {
                newRow->cells.Insert(N_new Table::Cell());
            }
        }
        rows.Insert(rowId, newRow);
        return newRow;
    }
    //------------------------------------------------------------------------
    bool Table::RemoveRow(Nui32 rowId)
    {
        Row *out;
        if (rows.Delete(rowId, out))
        {
            DeleteRow(out);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void Table::RemoveRows(Table *tableContainingRowIDs)
    {
        Nui32 i;
        DataStructures::Page<Nui32, Row*, _TABLE_BPLUS_TREE_ORDER> *cur = tableContainingRowIDs->GetRows().GetListHead();
        while (cur)
        {
            for (i=0; i < (Nui32)cur->size; i++)
            {
                rows.Delete(cur->keys[i]);
            }
            cur=cur->next;
        }
        return;
    }
    //------------------------------------------------------------------------
    bool Table::UpdateCell(Nui32 rowId, Nui32 columnIndex, int value)
    {
        N_assert(columns[columnIndex].columnType==NUMERIC);

        Row *row = GetRowByID(rowId);
        if (row)
        {
            row->UpdateCell(columnIndex, value);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool Table::UpdateCell(Nui32 rowId, Nui32 columnIndex, char *str)
    {
        N_assert(columns[columnIndex].columnType==STRING);

        Row *row = GetRowByID(rowId);
        if (row)
        {
            row->UpdateCell(columnIndex, str);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool Table::UpdateCell(Nui32 rowId, Nui32 columnIndex, int byteLength, char *data)
    {
        N_assert(columns[columnIndex].columnType==BINARY);

        Row *row = GetRowByID(rowId);
        if (row)
        {
            row->UpdateCell(columnIndex, byteLength, data);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool Table::UpdateCellByIndex(Nui32 rowIndex, Nui32 columnIndex, int value)
    {
        N_assert(columns[columnIndex].columnType==NUMERIC);

        Row *row = GetRowByIndex(rowIndex,0);
        if (row)
        {
            row->UpdateCell(columnIndex, value);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool Table::UpdateCellByIndex(Nui32 rowIndex, Nui32 columnIndex, char *str)
    {
        N_assert(columns[columnIndex].columnType==STRING);

        Row *row = GetRowByIndex(rowIndex,0);
        if (row)
        {
            row->UpdateCell(columnIndex, str);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool Table::UpdateCellByIndex(Nui32 rowIndex, Nui32 columnIndex, int byteLength, char *data)
    {
        N_assert(columns[columnIndex].columnType==BINARY);

        Row *row = GetRowByIndex(rowIndex,0);
        if (row)
        {
            row->UpdateCell(columnIndex, byteLength, data);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void Table::GetCellValueByIndex(Nui32 rowIndex, Nui32 columnIndex, int *output)
    {
        N_assert(columns[columnIndex].columnType==NUMERIC);

        Row *row = GetRowByIndex(rowIndex,0);
        if (row)
        {
            row->cells[columnIndex]->Get(output);
        }
    }
    //------------------------------------------------------------------------
    void Table::GetCellValueByIndex(Nui32 rowIndex, Nui32 columnIndex, char *output)
    {
        N_assert(columns[columnIndex].columnType==STRING);

        Row *row = GetRowByIndex(rowIndex,0);
        if (row)
        {
            row->cells[columnIndex]->Get(output);
        }
    }
    //------------------------------------------------------------------------
    void Table::GetCellValueByIndex(Nui32 rowIndex, Nui32 columnIndex, char *output, int *outputLength)
    {
        N_assert(columns[columnIndex].columnType==BINARY);

        Row *row = GetRowByIndex(rowIndex,0);
        if (row)
        {
            row->cells[columnIndex]->Get(output, outputLength);
        }
    }
    //------------------------------------------------------------------------
    Table::FilterQuery::FilterQuery()
    {
        columnName[0]=0;
    }
    //------------------------------------------------------------------------
    Table::FilterQuery::~FilterQuery()
    {

    }
    //------------------------------------------------------------------------
    Table::FilterQuery::FilterQuery(Nui32 column, Cell *cell, FilterQueryType op)
    {
        columnIndex=column;
        cellValue=cell;
        operation=op;
    }
    //------------------------------------------------------------------------
    Table::Row * Table::GetRowByID(Nui32 rowId) const
    {
        Row * row;
        if (rows.Get(rowId, row))
            return row;
        return 0;
    }
    //------------------------------------------------------------------------
    Table::Row * Table::GetRowByIndex(Nui32 rowIndex, Nui32 *key) const
    {
        DataStructures::Page<Nui32, Row *, _TABLE_BPLUS_TREE_ORDER> * cur = rows.GetListHead();
        while (cur)
        {
            if (rowIndex < (Nui32)cur->size)
            {
                if (key)
                    *key=cur->keys[rowIndex];
                return cur->mData[rowIndex];
            }
            if (rowIndex <= (Nui32)cur->size)
                rowIndex-=cur->size;
            else
                return 0;
            cur=cur->next;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void Table::QueryTable(Nui32 * columnIndicesSubset, Nui32 numColumnSubset,
        FilterQuery * inclusionFilters, Nui32 numInclusionFilters, Nui32 * rowIds,
            Nui32 numRowIDs, Table *result)
    {
        Nui32 i;
        DataStructures::List<Nui32> columnIndicesToReturn;

        // Clear the result table.
        result->Clear();

        if (columnIndicesSubset && numColumnSubset>0)
        {
            for (i=0; i < numColumnSubset; i++)
            {
                if (columnIndicesSubset[i]<columns.Size())
                    columnIndicesToReturn.Insert(columnIndicesSubset[i]);
            }
        }
        else
        {
            for (i=0; i < columns.Size(); i++)
                columnIndicesToReturn.Insert(i);
        }

        if (columnIndicesToReturn.Size()==0)
            return; // No valid columns specified

        for (i=0; i < columnIndicesToReturn.Size(); i++)
        {
            result->AddColumn(columns[columnIndicesToReturn[i]].columnName,columns[columnIndicesToReturn[i]].columnType);
        }

        // Get the column indices of the filter queries.
        DataStructures::List<Nui32> inclusionFilterColumnIndices;
        if (inclusionFilters && numInclusionFilters>0)
        {
            for (i=0; i < numInclusionFilters; i++)
            {
                if (inclusionFilters[i].columnName[0])
                    inclusionFilters[i].columnIndex=ColumnIndex(inclusionFilters[i].columnName);
                if (inclusionFilters[i].columnIndex<columns.Size())
                    inclusionFilterColumnIndices.Insert(inclusionFilters[i].columnIndex);
                else
                    inclusionFilterColumnIndices.Insert((Nui32)-1);
            }
        }

        if (rowIds==0 || numRowIDs==0)
        {
            // All rows
            DataStructures::Page<Nui32, Row*, _TABLE_BPLUS_TREE_ORDER> *cur = rows.GetListHead();
            while (cur)
            {
                for (i=0; i < (Nui32)cur->size; i++)
                {
                    QueryRow(inclusionFilterColumnIndices, columnIndicesToReturn, cur->keys[i], cur->mData[i], inclusionFilters, result);
                }
                cur=cur->next;
            }
        }
        else
        {
            // Specific rows
            Row *row;
            for (i=0; i < numRowIDs; i++)
            {
                if (rows.Get(rowIds[i], row))
                {
                    QueryRow(inclusionFilterColumnIndices, columnIndicesToReturn, rowIds[i], row, inclusionFilters, result);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Table::QueryRow(DataStructures::List<Nui32> & inclusionFilterColumnIndices,
        DataStructures::List<Nui32> &columnIndicesToReturn, Nui32 key, Table::Row * row,
            FilterQuery * inclusionFilters, Table * result)
    {
        bool pass=false;
        Nui32 columnIndex;
        Nui32 j;

        // If no inclusion filters, just add the row
        if (inclusionFilterColumnIndices.Size()==0)
        {
            result->AddRowColumns(key, row, columnIndicesToReturn);
        }
        else
        {
            // Go through all inclusion filters.  Only add this row if all filters pass.
            for (j=0; j<inclusionFilterColumnIndices.Size(); j++)
            {
                columnIndex=inclusionFilterColumnIndices[j];
                if (columnIndex!=(Nui32)-1 && row->cells[columnIndex]->isEmpty==false )
                {
                    if (columns[inclusionFilterColumnIndices[j]].columnType==STRING &&
                        (row->cells[columnIndex]->c==0 ||
                        inclusionFilters[j].cellValue->c==0)    )
                        continue;

                    switch (inclusionFilters[j].operation)
                    {
                    case QF_EQUAL:
                        switch(columns[inclusionFilterColumnIndices[j]].columnType)
                        {
                        case NUMERIC:
                            pass=row->cells[columnIndex]->i==inclusionFilters[j].cellValue->i;
                            break;
                        case STRING:
                            pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)==0;
                            break;
                        case BINARY:
                            pass=row->cells[columnIndex]->i==inclusionFilters[j].cellValue->i &&
                                memcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c, (int) row->cells[columnIndex]->i)==0;
                            break;
                        case POINTER:
                            pass=row->cells[columnIndex]->ptr==inclusionFilters[j].cellValue->ptr;
                            break;
                        }
                        break;
                    case QF_NOT_EQUAL:
                        switch(columns[inclusionFilterColumnIndices[j]].columnType)
                        {
                        case NUMERIC:
                            pass=row->cells[columnIndex]->i!=inclusionFilters[j].cellValue->i;
                            break;
                        case STRING:
                            pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)!=0;
                            break;
                        case BINARY:
                            pass=row->cells[columnIndex]->i==inclusionFilters[j].cellValue->i &&
                                memcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c, (int) row->cells[columnIndex]->i)==0;
                            break;
                        case POINTER:
                            pass=row->cells[columnIndex]->ptr!=inclusionFilters[j].cellValue->ptr;
                            break;
                        }
                        break;
                    case QF_GREATER_THAN:
                        switch(columns[inclusionFilterColumnIndices[j]].columnType)
                        {
                        case NUMERIC:
                            pass=row->cells[columnIndex]->i>inclusionFilters[j].cellValue->i;
                            break;
                        case STRING:
                            pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)>0;
                            break;
                        case BINARY:
                              break;
                        case POINTER:
                            pass=row->cells[columnIndex]->ptr>inclusionFilters[j].cellValue->ptr;
                            break;
                        }
                        break;
                    case QF_GREATER_THAN_EQ:
                        switch(columns[inclusionFilterColumnIndices[j]].columnType)
                        {
                        case NUMERIC:
                            pass=row->cells[columnIndex]->i>=inclusionFilters[j].cellValue->i;
                            break;
                        case STRING:
                            pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)>=0;
                            break;
                        case BINARY:
                            break;
                        case POINTER:
                            pass=row->cells[columnIndex]->ptr>=inclusionFilters[j].cellValue->ptr;
                            break;
                        }
                        break;
                    case QF_LESS_THAN:
                        switch(columns[inclusionFilterColumnIndices[j]].columnType)
                        {
                        case NUMERIC:
                            pass=row->cells[columnIndex]->i<inclusionFilters[j].cellValue->i;
                            break;
                        case STRING:
                            pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)<0;
                            break;
                        case BINARY:
                          break;
                        case POINTER:
                            pass=row->cells[columnIndex]->ptr<inclusionFilters[j].cellValue->ptr;
                            break;
                        }
                        break;
                    case QF_LESS_THAN_EQ:
                        switch(columns[inclusionFilterColumnIndices[j]].columnType)
                        {
                        case NUMERIC:
                            pass=row->cells[columnIndex]->i<=inclusionFilters[j].cellValue->i;
                            break;
                        case STRING:
                            pass=strcmp(row->cells[columnIndex]->c,inclusionFilters[j].cellValue->c)<=0;
                            break;
                        case BINARY:
                            break;
                        case POINTER:
                            pass=row->cells[columnIndex]->ptr<=inclusionFilters[j].cellValue->ptr;
                            break;
                        }
                        break;
                    case QF_IS_EMPTY:
                        pass=false;
                        break;
                    case QF_NOT_EMPTY:
                        pass=true;
                        break;
                    default:
                        pass=false;
                        N_assert(0);
                        break;
                    }
                }
                else
                {
                    if (inclusionFilters[j].operation==QF_IS_EMPTY)
                        pass=true;
                    else
                        pass=false; // No value for this cell
                }

                if (pass==false)
                    break;
            }

            if (pass)
            {
                result->AddRowColumns(key, row, columnIndicesToReturn);
            }
        }
    }
    //------------------------------------------------------------------------
    static Table::SortQuery *_sortQueries;
    static Nui32 _numSortQueries;
    static DataStructures::List<Nui32> *_columnIndices;
    static DataStructures::List<Table::ColumnDescriptor> *_columns;
    //------------------------------------------------------------------------
    int RowSort(Table::Row* const &first, Table::Row* const &second) // first is the one inserting, second is the one already there.
    {
        Nui32 i, columnIndex;
        for (i=0; i<_numSortQueries; i++)
        {
            columnIndex=(*_columnIndices)[i];
            if (columnIndex==(Nui32)-1)
                continue;

            if (first->cells[columnIndex]->isEmpty==true && second->cells[columnIndex]->isEmpty==false)
                return 1; // Empty cells always go at the end

            if (first->cells[columnIndex]->isEmpty==false && second->cells[columnIndex]->isEmpty==true)
                return -1; // Empty cells always go at the end

            if (_sortQueries[i].operation==Table::QS_INCREASING_ORDER)
            {
                if ((*_columns)[columnIndex].columnType==Table::NUMERIC)
                {
                    if (first->cells[columnIndex]->i>second->cells[columnIndex]->i)
                        return 1;
                    if (first->cells[columnIndex]->i<second->cells[columnIndex]->i)
                        return -1;
                }
                else
                {
                    // String
                    if (strcmp(first->cells[columnIndex]->c,second->cells[columnIndex]->c)>0)
                        return 1;
                    if (strcmp(first->cells[columnIndex]->c,second->cells[columnIndex]->c)<0)
                        return -1;
                }
            }
            else
            {
                if ((*_columns)[columnIndex].columnType==Table::NUMERIC)
                {
                    if (first->cells[columnIndex]->i<second->cells[columnIndex]->i)
                        return 1;
                    if (first->cells[columnIndex]->i>second->cells[columnIndex]->i)
                        return -1;
                }
                else
                {
                    // String
                    if (strcmp(first->cells[columnIndex]->c,second->cells[columnIndex]->c)<0)
                        return 1;
                    if (strcmp(first->cells[columnIndex]->c,second->cells[columnIndex]->c)>0)
                        return -1;
                }
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void Table::SortTable(Table::SortQuery *sortQueries, Nui32 numSortQueries, Table::Row** out)
    {
        Nui32 i;
        Nui32 outLength;
        DataStructures::List<Nui32> columnIndices;
        _sortQueries=sortQueries;
        _numSortQueries=numSortQueries;
        _columnIndices=&columnIndices;
        _columns=&columns;
        bool anyValid=false;

        for (i=0; i < numSortQueries; i++)
        {
            if (sortQueries[i].columnIndex<columns.Size() && columns[sortQueries[i].columnIndex].columnType!=BINARY)
            {
                columnIndices.Insert(sortQueries[i].columnIndex);
                anyValid=true;
            }
            else
                columnIndices.Insert((Nui32)-1); // Means don't check this column
        }

        DataStructures::Page<Nui32, Row*, _TABLE_BPLUS_TREE_ORDER> *cur;
        cur = rows.GetListHead();
        if (anyValid==false)
        {
            outLength=0;
            while (cur)
            {
                for (i=0; i < (Nui32)cur->size; i++)
                {
                    out[(outLength)++]=cur->mData[i];
                }
                cur=cur->next;
            }
            return;
        }

        // Start adding to ordered list.
        DataStructures::OrderedList<Row*, Row*, RowSort> orderedList;
        while (cur)
        {
            for (i=0; i < (Nui32)cur->size; i++)
            {
                N_assert(cur->mData[i]);
                orderedList.Insert(cur->mData[i],cur->mData[i], true);
            }
            cur=cur->next;
        }

        outLength=0;
        for (i=0; i < orderedList.Size(); i++)
            out[(outLength)++]=orderedList[i];
    }
    //------------------------------------------------------------------------
    void Table::PrintColumnHeaders(char *out, int outLength, char columnDelineator) const
    {
        if (outLength<=0)
            return;
        if (outLength==1)
        {
            *out=0;
            return;
        }

        Nui32 i;
        out[0]=0;
        int len;
        for (i=0; i < columns.Size(); i++)
        {
            if (i!=0)
            {
                len = (int) strlen(out);
                if (len < outLength-1)
                    sprintf(out+len, "%c", columnDelineator);
                else
                    return;
            }

            len = (int) strlen(out);
            if (len < outLength-(int) strlen(columns[i].columnName))
                sprintf(out+len, "%s", columns[i].columnName);
            else
                return;
        }
    }
    //------------------------------------------------------------------------
    void Table::PrintRow(char * out, int outLength, char columnDelineator, bool printDelineatorForBinary, Table::Row* inputRow) const
    {
        if (outLength<=0)
            return;
        if (outLength==1)
        {
            *out=0;
            return;
        }

        if (inputRow->cells.Size()!=columns.Size())
        {
            strncpy(out, "Cell width does not match column width.\n", outLength);
            out[outLength-1]=0;
            return;
        }

        char buff[512];
        Nui32 i;
        int len;
        out[0]=0;
        for (i=0; i < columns.Size(); i++)
        {
            if (columns[i].columnType==NUMERIC)
            {
                if (inputRow->cells[i]->isEmpty==false)
                {
                    sprintf(buff, "%f", inputRow->cells[i]->i);
                    len=(int)strlen(buff);
                }
                else
                    len=0;
                if (i+1!=columns.Size())
                    buff[len++]=columnDelineator;
                buff[len]=0;
            }
            else if (columns[i].columnType==STRING)
            {
                if (inputRow->cells[i]->isEmpty==false && inputRow->cells[i]->c)
                {
                    strncpy(buff, inputRow->cells[i]->c, 512-2);
                    buff[512-2]=0;
                    len=(int)strlen(buff);
                }
                else
                    len=0;
                if (i+1!=columns.Size())
                    buff[len++]=columnDelineator;
                buff[len]=0;
            }
            else if (columns[i].columnType==POINTER)
            {
                if (inputRow->cells[i]->isEmpty==false && inputRow->cells[i]->ptr)
                {
                    sprintf(buff, "%p", inputRow->cells[i]->ptr);
                    len=(int)strlen(buff);
                }
                else
                    len=0;
                if (i+1!=columns.Size())
                    buff[len++]=columnDelineator;
                buff[len]=0;
            }
            else
            {
                if (printDelineatorForBinary)
                {
                    if (i+1!=columns.Size())
                        buff[0]=columnDelineator;
                    buff[1]=0;
                }
                else
                    buff[0]=0;

            }

            len=(int)strlen(out);
            if (outLength==len+1)
                break;
            strncpy(out+len, buff, outLength-len);
            out[outLength-1]=0;
        }
    }
    //------------------------------------------------------------------------
    void Table::Clear(void)
    {
        rows.ForEachData(FreeRow);
        rows.Clear();
        columns.Clear(true);
    }
    //------------------------------------------------------------------------
    const List<Table::ColumnDescriptor>& Table::GetColumns(void) const
    {
        return columns;
    }
    //------------------------------------------------------------------------
    const DataStructures::BPlusTree<Nui32, Table::Row*, _TABLE_BPLUS_TREE_ORDER>& Table::GetRows(void) const
    {
        return rows;
    }
    //------------------------------------------------------------------------
    DataStructures::Page<Nui32, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> * Table::GetListHead(void)
    {
        return rows.GetListHead();
    }
    //------------------------------------------------------------------------
    Nui32 Table::GetAvailableRowId(void) const
    {
        bool setKey=false;
        Nui32 key=0;
        int i;
        DataStructures::Page<Nui32, Row*, _TABLE_BPLUS_TREE_ORDER> *cur = rows.GetListHead();

        while (cur)
        {
            for (i=0; i < cur->size; i++)
            {
                if (setKey==false)
                {
                    key=cur->keys[i]+1;
                    setKey=true;
                }
                else
                {
                    if (key!=cur->keys[i])
                        return key;
                    key++;
                }
            }

            cur=cur->next;
        }
        return key;
    }
    //------------------------------------------------------------------------
    void Table::DeleteRow(Table::Row * row)
    {
        Nui32 rowIndex;
        for (rowIndex=0; rowIndex < row->cells.Size(); rowIndex++)
        {
            N_delete row->cells[rowIndex];
        }
        N_delete row;
    }
    //------------------------------------------------------------------------
    Table& Table::operator = (const Table & input)
    {
        Clear();

        Nui32 i;
        for (i=0; i < input.GetColumnCount(); i++)
            AddColumn(input.ColumnName(i), input.GetColumnType(i));

        DataStructures::Page<Nui32, Row*, _TABLE_BPLUS_TREE_ORDER> *cur = input.GetRows().GetListHead();
        while (cur)
        {
            for (i=0; i < (Nui32) cur->size; i++)
            {
                AddRow(cur->keys[i], cur->mData[i]->cells, false);
            }

            cur=cur->next;
        }

        return *this;
    }
    //------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning( pop )
#endif
