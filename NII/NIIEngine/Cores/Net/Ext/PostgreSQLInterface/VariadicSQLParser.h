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

#ifndef _VARIADIC_SQL_PARSER_H_
#define _VARIADIC_SQL_PARSER_H_

#include "DS_List.h"

#include <stdarg.h>

namespace VariadicSQLParser
{
    struct IndexAndType
    {
        Nui32 strIndex;
        Nui32 typeMappingIndex;
    };
    const char * GetTypeMappingAtIndex(int i);
    void GetTypeMappingIndices(const char *format, DataStructures::List<IndexAndType> & indices);
    // Given an SQL string with variadic arguments, allocate argumentBinary and argumentLengths, and hold the parameters in binary format
    // Last 2 parameters are out parameters
    void ExtractArguments(va_list argptr, const DataStructures::List<IndexAndType> & indices, char *** argumentBinary, int **argumentLengths);
    void FreeArguments(const DataStructures::List<IndexAndType> & indices, char ** argumentBinary, int * argumentLengths);
}
#endif