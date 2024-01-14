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

#include "NiiPreProcess.h"
#include "TableSerializer.h"
#include "DS_Table.h"
#include "NiiNetSerializer.h"
#include "StringTable.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    void TableSerializer::SerializeTable(DataStructures::Table * in, NetSerializer * out)
    {
        DataStructures::Page<Nui32, DataStructures::Table::Row *, _TABLE_BPLUS_TREE_ORDER> *cur = in->GetRows().GetListHead();
        const DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns=in->GetColumns();
        SerializeColumns(in, out);
        out->write((Nui32)in->GetRows().Size());
        NCount rowIndex;
        while(cur)
        {
            for(rowIndex = 0; rowIndex < (Nui32)cur->size; ++rowIndex)
            {
                SerializeRow(cur->mData[rowIndex], cur->keys[rowIndex], columns, out);
            }
            cur = cur->next;
        }
    }
    //------------------------------------------------------------------------
    void TableSerializer::SerializeColumns(DataStructures::Table * in, NetSerializer * out)
    {
        const DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns=in->GetColumns();
        out->write((Nui32)columns.Size());
        Nui32 i;
        for (i=0; i<columns.Size(); i++)
        {
            StringCompressor::Instance()->EncodeString(columns[i].columnName, _TABLE_MAX_COLUMN_NAME_LENGTH, out);
            out->write((Nui8)columns[i].columnType);
        }
    }
    //------------------------------------------------------------------------
    void TableSerializer::SerializeColumns(DataStructures::Table * in, NetSerializer * out,
        DataStructures::List<int> & skipColumnIndices)
    {
        const DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns=in->GetColumns();
        out->write((Nui32)columns.Size()-skipColumnIndices.Size());
        NCount i;
        for (i = 0; i<columns.Size(); ++i)
        {
            if (skipColumnIndices.GetIndexOf(i)==(Nui32)-1)
            {
                StringCompressor::Instance()->EncodeString(columns[i].columnName, _TABLE_MAX_COLUMN_NAME_LENGTH, out);
                out->write((Nui8)columns[i].columnType);
            }
        }
    }
    //------------------------------------------------------------------------
    bool TableSerializer::DeserializeTable(Nui8 * serializedTable, Nui32 dataLength,
        DataStructures::Table * out)
    {
        NetSerializer in((Nui8 *)serializedTable, dataLength, false);
        return DeserializeTable(&in, out);
    }
    //------------------------------------------------------------------------
    bool TableSerializer::DeserializeTable(NetSerializer * in, DataStructures::Table * out)
    {
        Nui32 rowSize;
        DeserializeColumns(in,out);
        if (in->read(rowSize)==false || rowSize>100000)
        {
            N_assert(0);
            return false; // Hacker crash prevention
        }

        Nui32 rowIndex;
        for (rowIndex=0; rowIndex < rowSize; rowIndex++)
        {
            if (DeserializeRow(in, out)==false)
                return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool TableSerializer::DeserializeColumns(NetSerializer * in,
        DataStructures::Table * out)
    {
        Nui32 columnSize;
        Nui8 columnType;
        char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH];
        if (in->read(columnSize)==false || columnSize > 10000)
            return false; // Hacker crash prevention

        out->Clear();
        Nui32 i;
        for (i=0; i<columnSize; i++)
        {
            StringCompressor::Instance()->DecodeString(columnName, 32, in);
            in->read(columnType);
            out->AddColumn(columnName, (DataStructures::Table::ColumnType)columnType);
        }
        return true;
    }
    //------------------------------------------------------------------------
    void TableSerializer::SerializeRow(DataStructures::Table::Row * in, Nui32 keyIn,
        const DataStructures::List<DataStructures::Table::ColumnDescriptor> & columns,
            NetSerializer * out)
    {
        Nui32 cellIndex;
        out->write(keyIn);
        Nui32 columnsSize = columns.Size();
        out->write(columnsSize);
        for (cellIndex=0; cellIndex<columns.Size(); cellIndex++)
        {
            out->write(cellIndex);
            SerializeCell(out, in->cells[cellIndex], columns[cellIndex].columnType);
        }
    }
    //------------------------------------------------------------------------
    void TableSerializer::SerializeRow(DataStructures::Table::Row * in, Nui32 keyIn,
        const DataStructures::List<DataStructures::Table::ColumnDescriptor> & columns,
            NetSerializer * out, DataStructures::List<int> & skipColumnIndices)
    {
        NCOunt cellIndex;
        out->write(keyIn);
        Nui32 numEntries = 0;
        for(cellIndex = 0; cellIndex<columns.Size(); ++cellIndex)
        {
            if (skipColumnIndices.GetIndexOf(cellIndex) == (Nui32)-1)
            {
                ++numEntries;
            }
        }
        out->write(numEntries);

        for (cellIndex=0; cellIndex<columns.Size(); ++cellIndex)
        {
            if (skipColumnIndices.GetIndexOf(cellIndex) == (Nui32)-1)
            {
                out->write(cellIndex);
                SerializeCell(out, in->cells[cellIndex], columns[cellIndex].columnType);
            }
        }
    }
    //------------------------------------------------------------------------
    bool TableSerializer::DeserializeRow(NetSerializer * in, DataStructures::Table * out)
    {
        const DataStructures::List<DataStructures::Table::ColumnDescriptor> &columns=out->GetColumns();
        Nui32 numEntries;
        DataStructures::Table::Row *row;
        Nui32 key;
        if (in->read(key) == false)
            return false;
        row = out->AddRow(key);
        NCount cnt;
        in->read(numEntries);
        for (cnt = 0; cnt < numEntries; ++cnt)
        {
            Nui32 cellIndex;
            in->read(cellIndex);
            if (DeserializeCell(in, row->cells[cellIndex], columns[cellIndex].columnType)==false)
            {
                out->RemoveRow(key);
                return false;
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    void TableSerializer::SerializeCell(NetSerializer * out,
        DataStructures::Table::Cell * cell,
            DataStructures::Table::ColumnType columnType)
    {
        out->write(cell->isEmpty);
        if (cell->isEmpty == false)
        {
            if (columnType == DataStructures::Table::NUMERIC)
            {
                out->write(cell->i);
            }
            else if (columnType == DataStructures::Table::STRING)
            {
                StringCompressor::Instance()->EncodeString(cell->c, 65535, out);
            }
            else if (columnType == DataStructures::Table::POINTER)
            {
                out->write(cell->ptr);
            }
            else
            {
                // Binary
                N_assert(columnType == DataStructures::Table::BINARY);
                N_assert(cell->i > 0);
                Nui32 count;
                count = (Nui32)cell->i;
                out->write(count);
                out->write((const Nui8 *)cell->c, (Nui32)cell->i);
            }
        }
    }
    //------------------------------------------------------------------------
    bool TableSerializer::DeserializeCell(NetSerializer * in,
        DataStructures::Table::Cell * cell,
            DataStructures::Table::ColumnType columnType)
    {
        bool isEmpty = false;
        NIId value;
        void * ptr;
        char tempString[65535];
        cell->Clear();

        if(in->read(isEmpty) == false)
            return false;
        if(isEmpty == false)
        {
            if(columnType == DataStructures::Table::NUMERIC)
            {
                if(in->read(value) == false)
                    return false;
                cell->Set(value);
            }
            else if(columnType == DataStructures::Table::STRING)
            {
                if(StringCompressor::Instance()->DecodeString(tempString, 65535, in) == false)
                    return false;
                cell->Set(tempString);
            }
            else if(columnType == DataStructures::Table::POINTER)
            {
                if(in->read(ptr) == false)
                    return false;
                cell->SetPtr(ptr);
            }
            else
            {
                Nui32 count;
                // Binary
                N_assert(columnType == DataStructures::Table::BINARY);
                if (in->read(count) == false || count > 10000000)
                    return false; // Sanity check to max binary cell of 10 megabytes
                in->readAlign();
                if (N_B_TO_b(in->getRemainCount()) < count)
                    return false;
                cell->Set(in->getData() + N_B_TO_b(in->getReadOffset()), count);
                in->skipWrite(count, 0);
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    void TableSerializer::SerializeFilterQuery(NetSerializer * in,
        DataStructures::Table::FilterQuery * query)
    {
        StringCompressor::Instance()->EncodeString(query->columnName,
            _TABLE_MAX_COLUMN_NAME_LENGTH, in, 0);

        in->writeCompress(query->columnIndex);
        in->write((Nui8) query->operation);
        in->write(query->cellValue->isEmpty);
        if (query->cellValue->isEmpty == false)
        {
            Nui8 * data = query->cellValue->c;
            in->write(query->cellValue->i);

            if (data == 0 || query->cellValue->i == 0)
            {
                in->writeCompress((Nui32)0);
                return;
            }
            in->writeCompress((Nui32)query->cellValue->i);
            in->write(data, query->cellValue->i < 10000000 ? query->cellValue->i : 10000000);

            in->write(query->cellValue->ptr);

        }
    }
    //------------------------------------------------------------------------
    bool TableSerializer::DeserializeFilterQuery(NetSerializer * out,
        DataStructures::Table::FilterQuery * query)
    {
        bool b;
        N_assert(query->cellValue);
        StringCompressor::Instance()->DecodeString(query->columnName,_TABLE_MAX_COLUMN_NAME_LENGTH,out,0);
        out->readCompress(query->columnIndex);
        Nui8 op;
        out->read(op);
        query->operation = (DataStructures::Table::FilterQueryType) op;
        query->cellValue->Clear();
        b = out->read(query->cellValue->isEmpty);
        if (query->cellValue->isEmpty == false)
        {
            // HACK - cellValue->i is used for integer, character, and binary data. However, for character and binary c will be 0. So use that to determine if the data was integer or not.
            out->read(query->cellValue->i);
            Nui32 count;
            N_free(query->cellValue->c);
            query->cellValue->c = 0;
            if (out->readCompress(count) == false)
                return false;
            if (count > 10000000)
                count = 10000000;
            if (count == 0)
                return true;
            query->cellValue->c = N_alloc((NCount)count);
            out->read((Nui8 *)query->cellValue->c, count);

            if (query->cellValue->c)
                query->cellValue->i = count;
            b = out->read(query->cellValue->ptr);
        }
        return b;
    }
    //------------------------------------------------------------------------
    void TableSerializer::SerializeFilterQueryList(NetSerializer * in,
        DataStructures::Table::FilterQuery * query, Nui32 numQueries, Nui32 maxQueries)
    {
        (void) maxQueries;
        in->write((bool)(query && numQueries > 0));
        if (query == 0 || numQueries <= 0)
            return;

        N_assert(numQueries <= maxQueries);
        in->writeCompress(numQueries);
        NCount i;
        for (i = 0; i < numQueries; ++i)
        {
            SerializeFilterQuery(in, query);
        }
    }
    //------------------------------------------------------------------------
    bool TableSerializer::DeserializeFilterQueryList(NetSerializer * out,
        DataStructures::Table::FilterQuery ** query, Nui32 * numQueries,
            Nui32 maxQueries, int allocateExtraQueries)
    {
        bool b, anyQueries=false;
        out->read(anyQueries);
        if (anyQueries==false)
        {
            if (allocateExtraQueries<=0)
                *query = 0;
            else
                *query = N_new DataStructures::Table::FilterQuery[allocateExtraQueries];

            *numQueries=0;
            return true;
        }
        b = out->readCompress(*numQueries);
        if(*numQueries>maxQueries)
        {
            N_assert(0);
            *numQueries=maxQueries;
        }
        if(*numQueries==0)
            return b;

        *query = N_new DataStructures::Table::FilterQuery[*numQueries+allocateExtraQueries];
        DataStructures::Table::FilterQuery *queryPtr = *query;

        NCount i;
        for (i=0; i < *numQueries; ++i)
        {
            queryPtr[i].cellValue = N_new DataStructures::Table::Cell;
            b=DeserializeFilterQuery(out, queryPtr+i);
        }

        return b;
    }
    //------------------------------------------------------------------------
    void TableSerializer::DeallocateQueryList(DataStructures::Table::FilterQuery * query,
        Nui32 numQueries)
    {
        if (query==0 || numQueries==0)
            return;

        NCount i;
        for (i=0; i < numQueries; ++i)
            N_delete query[i].cellValue;
        N_delete []query;
    }
    //------------------------------------------------------------------------
}
}