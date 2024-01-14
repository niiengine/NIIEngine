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
#include "PostgreSQLInterface.h"
#include "VariadicSQLParser.h"

// libpq-fe.h is part of PostgreSQL which must be installed on this computer to use the PostgreRepository
#include "libpq-fe.h"

#ifdef _CONSOLE_1
#include "Console1Includes.h"
#elif defined(_WIN32)
#include <winsock2.h> // htonl
#elif defined(_CONSOLE_2)
#include "Console2Includes.h"
#else
#include <arpa/inet.h>
#endif

// alloca
#ifdef _COMPATIBILITY_1
#elif defined(_WIN32)
#include <malloc.h>
#else
//#include <stdlib.h>
#endif

#include "RakString.h"
#include "N_assert.h"
#include "NiiNetSerializer.h"
#include "LinuxStrings.h"

#define PQEXECPARAM_FORMAT_TEXT		0
#define PQEXECPARAM_FORMAT_BINARY	1

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    PostgreSQLInterface::PostgreSQLInterface()
    {
        pgConn = 0;
        isConnected = false;
        lastError[0] = 0;
        pgConnAllocatedHere = false;
    }
    //------------------------------------------------------------------------
    PostgreSQLInterface::~PostgreSQLInterface()
    {
        if (isConnected && pgConnAllocatedHere)
            PQfinish(pgConn);
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::Connect(const char * conninfo)
    {
        if (isConnected == false)
        {
            _conninfo = conninfo;

            pgConn = PQconnectdb(conninfo);
            ConnStatusType status = PQstatus(pgConn);
            isConnected = status == CONNECTION_OK;
            if (isConnected == false)
            {
                PQfinish(pgConn);
                return false;
            }
            pgConnAllocatedHere = true;
        }

        return isConnected;
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::AssignConnection(PGconn * _pgConn)
    {
        pgConnAllocatedHere = false;
        pgConn = _pgConn;
        ConnStatusType status = PQstatus(pgConn);
        isConnected = status == CONNECTION_OK;
    }
    //------------------------------------------------------------------------
    PGconn * PostgreSQLInterface::GetPGConn(void) const
    {
        return pgConn;
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::Disconnect(void)
    {
        if (isConnected)
        {
            PQfinish(pgConn);
            isConnected = false;
        }
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::Rollback(void)
    {
        PGresult * result = PQexec(pgConn, "ROLLBACK;");
        PQclear(result);
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::IsResultSuccessful(PGresult * result, bool rollbackOnFailure)
    {
        if (result == 0)
            return false;

        bool success = false;
        ExecStatusType execStatus = PQresultStatus(result);
        strcpy(lastError,PQresultErrorMessage(result));
        switch (execStatus)
        {
        case PGRES_COMMAND_OK:
            success=true;
            break;
        case PGRES_EMPTY_QUERY:
            break;
        case PGRES_TUPLES_OK:
            success=true;
            break;
        case PGRES_COPY_OUT:
            break;
        case PGRES_COPY_IN:
            break;
        case PGRES_BAD_RESPONSE:
            break;
        case PGRES_NONFATAL_ERROR:
            break;
        case PGRES_FATAL_ERROR:
            if (rollbackOnFailure)
                Rollback();
            break;
        }
        return success;
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::ExecuteBlockingCommand(const char * command,
        PGresult ** result, bool rollbackOnFailure)
    {
        *result = PQexec(pgConn, command);
        return IsResultSuccessful(*result, rollbackOnFailure);
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(int * output, PGresult * result,
        unsigned int rowIndex, const char * columnName)
    {
        return PQGetValueFromBinary(output,result,(int) rowIndex,columnName);
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(int * output, PGresult * result,
        int rowIndex, const char * columnName)
    {
        int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
        char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
        if (binaryData==0)
            return false;
        if (binaryData)
        {
            N_assert(PQgetlength(result, rowIndex, columnIndex)==sizeof(int));
            memcpy(output, binaryData, sizeof(int));
            EndianSwapInPlace((char*)output, sizeof(int));
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(unsigned int * output,
        PGresult * result, int rowIndex, const char * columnName)
    {
        int columnIndex = PQfnumber(result, columnName);
        if(columnIndex == -1)
            return false;
        char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
        if (binaryData == 0 || PQgetlength(result, rowIndex, columnIndex)==0)
            return false;
        if (binaryData)
        {
            N_assert(PQgetlength(result, rowIndex, columnIndex)==sizeof(unsigned int));
            memcpy(output, binaryData, sizeof(unsigned int));
            EndianSwapInPlace((char*)output, sizeof(unsigned int));
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(long long *output, PGresult * result,
        int rowIndex, const char * columnName)
    {
        int columnIndex = PQfnumber(result, columnName);
        if(columnIndex == -1)
            return false;
        char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
        if (binaryData == 0)
            return false;
        if (binaryData)
        {
            N_assert(PQgetlength(result, rowIndex, columnIndex) == sizeof(long long));
            memcpy(output, binaryData, sizeof(long long));
            EndianSwapInPlace((char*)output, sizeof(long long));
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(NIIf * output, PGresult * result,
        int rowIndex, const char * columnName)
    {
        int columnIndex = PQfnumber(result, columnName);
        if (columnIndex == -1)
            return false;
        char * binaryData = PQgetvalue(result, rowIndex, columnIndex);
        if (binaryData == 0)
            return false;
        int len = PQgetlength(result, rowIndex, columnIndex);
        N_assert(len == sizeof(NIIf));
        N_assert(binaryData);
        if (binaryData)
        {
            memcpy(output, binaryData, sizeof(NIIf));
            EndianSwapInPlace((char *)output, sizeof(NIIf));
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(NIId * output,
        PGresult * result, int rowIndex, const char *columnName)
    {
        int columnIndex = PQfnumber(result, columnName);
        if(columnIndex==-1)
            return false;
        char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
        if (binaryData==0)
            return false;
        int len = PQgetlength(result, rowIndex, columnIndex);
        N_assert(len==sizeof(NIId));
        N_assert(binaryData);
        if (binaryData)
        {
            memcpy(output, binaryData, sizeof(NIId));
            EndianSwapInPlace((char*)output, sizeof(NIId));
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(bool *output, PGresult * result,
        int rowIndex, const char * columnName)
    {
        int columnIndex = PQfnumber(result, columnName);
        if(columnIndex == -1)
            return false;
        char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
        if(binaryData == 0)
            return false;
        *output = binaryData[0] != 0;

        return true;
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(RakString * output,
        PGresult * result, int rowIndex, const char * columnName)
    {
        int columnIndex = PQfnumber(result, columnName);
            if (columnIndex == -1)
                return false;
        char * gv;
        gv = PQgetvalue(result, rowIndex, columnIndex);
        if (gv && gv[0])
            *output=gv;
        else
            output->Clear();
        return output->IsEmpty()==false;
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(char ** output,
        unsigned int * outputLength, PGresult * result, int rowIndex,
            const char * columnName)
    {
        return PQGetValueFromBinary(output, (int*) outputLength,result, rowIndex,columnName);
    }
    //------------------------------------------------------------------------
    bool PostgreSQLInterface::PQGetValueFromBinary(char ** output, int * outputLength,
        PGresult * result, int rowIndex, const char * columnName)
    {
        int columnIndex = PQfnumber(result, columnName);
        if (columnIndex != -1)
        {
            *outputLength = PQgetlength(result, rowIndex, columnIndex);
            if(*outputLength == 0)
            {
                *output = 0;
                return false;
            }
            else
            {
                *output = (char *)rakMalloc_Ex(*outputLength);
                memcpy(*output, PQgetvalue(result, rowIndex, columnIndex), *outputLength);
                return true;
            }
        }
        else
            return false;

    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EndianSwapInPlace(char * data, int dataLength)
    {
        static bool alreadyNetworkOrder = (htonl(12345) == 12345);
        if (alreadyNetworkOrder)
            return;
        int i;
        char tmp;
        for (i=0; i < dataLength/2; ++i)
        {
            tmp=data[i];
            data[i]=data[dataLength-1-i];
            data[dataLength-1-i]=tmp;
        }
    }
    //------------------------------------------------------------------------
    char *PostgreSQLInterface::GetLocalTimestamp(void)
    {
        static char strRes[512];

        PGresult *result;
        if (ExecuteBlockingCommand("SELECT LOCALTIMESTAMP", &result, false))
        {
            char *ts=PQgetvalue(result, 0, 0);
            if (ts)
            {
                sprintf(strRes,"Local timestamp is: %s\n", ts);
            }
            else
            {
                sprintf(strRes,"Can't read current time\n");
            }
            PQclear(result);
        }
        else
            sprintf(strRes,"Failed to read LOCALTIMESTAMP\n");

        return (char*)strRes;
    }
    //------------------------------------------------------------------------
    long long PostgreSQLInterface::GetEpoch(void)
    {
        PGresult *result;
        long long out;
        result = QueryVariadic("EXTRACT(EPOCH FROM current_timestamp) as out);");
        PostgreSQLInterface::PQGetValueFromBinary(&out, result, 0, "out");
        PQclear(result);
        return out;
    }
    //------------------------------------------------------------------------
    const char * PostgreSQLInterface::GetLastError(void) const
    {
        return (char *)lastError;
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryInput(const char * colName, unsigned int value,
        RakString & paramTypeStr, RakString & valueStr,
            int & numParams, char ** paramData, int * paramLength, int * paramFormat)
    {
        (void)numParams;
        (void)paramData;
        (void)paramLength;
        (void)paramFormat;
        String temp;
        if (paramTypeStr.IsEmpty()==false)
        {
            paramTypeStr += ", ";
            valueStr += ", ";
        }
        paramTypeStr += colName;
        valueStr += StrUtil::format(temp, "%i", value);
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryUpdate(const char * colName, unsigned int value,
        RakString & valueStr, int & numParams, char ** paramData, int * paramLength,
            int * paramFormat)
    {
        (void)numParams;
        (void)paramData;
        (void)paramLength;
        (void)paramFormat;
        String temp;
        if (valueStr.IsEmpty()==false)
        {
            valueStr += ", ";
        }
        valueStr += colName;
        valueStr += " = ";
        valueStr += StrUtil::format(temp, "%i", value);
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryInput(const char * colName, int value,
        RakString & paramTypeStr, RakString & valueStr, int & numParams,
            char ** paramData, int * paramLength, int * paramFormat)
    {
        (void)numParams;
        (void)paramData;
        (void)paramLength;
        (void)paramFormat;
        String temp;
        if (paramTypeStr.IsEmpty()==false)
        {
            paramTypeStr += ", ";
            valueStr += ", ";
        }
        paramTypeStr += colName;
        valueStr += StrUtil::format(temp, "%i", value);
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryInput(const char *colName, bool value, RakString &paramTypeStr, RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
    {
        (void)numParams;
        (void)paramData;
        (void)paramLength;
        (void)paramFormat;

        if (paramTypeStr.IsEmpty()==false)
        {
            paramTypeStr += ", ";
            valueStr += ", ";
        }
        paramTypeStr += colName;
        if (value)
            valueStr += "true";
        else
            valueStr += "false";
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, int value, RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
    {
        (void)numParams;
        (void)paramData;
        (void)paramLength;
        (void)paramFormat;
        String temp;
        if (valueStr.IsEmpty()==false)
        {
            valueStr += ", ";
        }
        valueStr += colName;
        valueStr += " = ";
        valueStr += StrUtil::format(temp, "%i", value);
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryInput(const char *colName, NIIf value, RakString &paramTypeStr, RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
    {
        (void)numParams;
        (void)paramData;
        (void)paramLength;
        (void)paramFormat;
        String temp;
        if (paramTypeStr.IsEmpty()==false)
        {
            paramTypeStr += ", ";
            valueStr += ", ";
        }
        paramTypeStr += colName;
        valueStr += StrUtil::format(temp, "%f", value);
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, NIIf value, RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
    {
        (void)numParams;
        (void)paramData;
        (void)paramLength;
        (void)paramFormat;
        String temp;
        if (valueStr.IsEmpty()==false)
        {
            valueStr += ", ";
        }
        valueStr += colName;
        valueStr += " = ";
        valueStr += StrUtil::format(temp, "%f", value);
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryInput(const char * colName,
        char * binaryData, int count, RakString & paramTypeStr,
            RakString & valueStr, int & numParams, char ** paramData,
                int * paramLength, int * paramFormat, bool writeEmpty)
    {
        String temp;
        if (writeEmpty == false && (binaryData == 0 || count == 0))
            return;

        if (binaryData == 0)
            count = 0;

        if (paramTypeStr.IsEmpty() == false)
        {
            paramTypeStr += ", ";
            valueStr += ", ";
        }
        paramTypeStr += colName;
        valueStr += StrUtil::format(temp, "$%i::bytea", numParams+1);

        paramData[numParams] = binaryData;
        paramLength[numParams] = count;
        paramFormat[numParams] = PQEXECPARAM_FORMAT_BINARY;
        ++numParams;
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryUpdate(const char * colName, char * binaryData,
        int count, RakString & valueStr, int & numParams,
            char ** paramData, int * paramLength, int * paramFormat)
    {
        String temp;
        if (binaryData == 0 || count == 0)
            return;

        if (binaryData == 0)
            count = 0;

        if (valueStr.IsEmpty() == false)
        {
            valueStr += ", ";
        }
        valueStr += colName;
        valueStr += " = ";
        valueStr += StrUtil::format(temp, "$%i::bytea", numParams+1);

        paramData[numParams] = binaryData;
        paramLength[numParams] = count;
        paramFormat[numParams] = PQEXECPARAM_FORMAT_BINARY;
        ++numParams;
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryInput(const char * colName, const char * str,
        RakString & paramTypeStr, RakString & valueStr, int & numParams,
            char ** paramData, int * paramLength, int * paramFormat, bool writeEmpty,
                const char * type)
    {
        String temp;
        if (writeEmpty==false && (str==0 || str[0]==0))
            return;

        static char *emptyStr=(char*)"";
        static char *emptyDate=(char*)"01/01/01";

        if (paramTypeStr.IsEmpty()==false)
        {
            paramTypeStr += ", ";
            valueStr += ", ";
        }
        paramTypeStr += colName;
        valueStr+=StrUtil::format(temp, "$%i::%s", numParams+1, type);

        if (writeEmpty && (str==0 || str[0]==0))
        {
            if (strcmp(type,"date")==0)
            {
                paramData[numParams]=emptyDate;
                paramLength[numParams]=(int)strlen(emptyDate);
            }
            else
            {
                paramData[numParams]=emptyStr;
                paramLength[numParams]=0;
            }
        }
        else
        {
            paramData[numParams]=(char*) str;
            paramLength[numParams]=(int) strlen(str);
        }
        paramFormat[numParams]=PQEXECPARAM_FORMAT_TEXT;
        ++numParams;
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, 
        const char *str, RakString &valueStr, int &numParams, char **paramData,
            int *paramLength, int *paramFormat, const char *type)
    {
        String temp;
        if (str==0 || str[0]==0)
            return;

        if (valueStr.IsEmpty()==false)
        {
            valueStr += ", ";
        }
        valueStr += colName;
        valueStr+=" = ";
        valueStr+=StrUtil::format(temp, "$%i::%s", numParams+1, type);

        paramData[numParams]=(char*) str;
        paramLength[numParams]=(int) strlen(str);

        paramFormat[numParams]=PQEXECPARAM_FORMAT_TEXT;
        ++numParams;
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryInput(const char *colName, 
        const RakString &str, RakString &paramTypeStr, RakString &valueStr,
            int &numParams, char **paramData, int *paramLength, int *paramFormat,
                bool writeEmpty, const char *type)
    {
        EncodeQueryInput(colName, str.C_String(), paramTypeStr, valueStr, numParams, paramData, paramLength, paramFormat, writeEmpty, type);
    }
    //------------------------------------------------------------------------
    void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, const RakString &str, RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, const char *type)
    {
        EncodeQueryUpdate(colName, str.C_String(), valueStr, numParams, paramData, paramLength, paramFormat, type);
    }
    //------------------------------------------------------------------------
    RakString PostgreSQLInterface::GetEscapedString(const char *input) const
    {
        unsigned long len = (unsigned long) strlen(input);
        char *fn = (char*) rakMalloc_Ex(len * 2 + 1);
        int error;
        PQescapeStringConn(pgConn, fn, input, len, &error);
        RakString output;
        // Use assignment so it doesn't parse printf escape strings
        output = fn;
        rakFree_Ex(fn);
        return output;
    }
    //------------------------------------------------------------------------
    PGresult * PostgreSQLInterface::QueryVariadic( const char * input, ... )
    {
        RakString query;
        PGresult *result;
        DataStructures::List<VariadicSQLParser::IndexAndType> indices;
        if ( input==0 || input[0]==0 )
            return 0;

        // Lookup this query in the stored query table. If it doesn't exist, prepare it.
        RakString inputStr;
        inputStr=input;
        unsigned int preparedQueryIndex;
        for (preparedQueryIndex=0; preparedQueryIndex < preparedQueries.Size(); preparedQueryIndex++)
        {
            if (preparedQueries[preparedQueryIndex].StrICmp(inputStr)==0)
                break;
        }

        // Find out how many params there are
        // Find out the type of each param (%f, %s)
        indices.Clear(false);
        GetTypeMappingIndices(input, indices);

        if (preparedQueryIndex == preparedQueries.Size())
        {
            RakString formatCopy;
            RakString insertion;
            formatCopy=input;
            unsigned int i;
            unsigned int indexOffset=0;
            for (i=0; i < indices.Size(); i++)
            {
                formatCopy.SetChar(indices[i].strIndex+indexOffset, '$');
                insertion=RakString("%i::%s", i+1, VariadicSQLParser::GetTypeMappingAtIndex(indices[i].typeMappingIndex));
                formatCopy.SetChar(indices[i].strIndex+1+indexOffset, insertion);
                indexOffset+=(unsigned int) insertion.GetLength()-1;
            }
            query += formatCopy;
            formatCopy += ";\n";
            result = PQprepare(pgConn, RakString("PGSQL_ExecuteVariadic_%i", preparedQueries.Size()), formatCopy.C_String(), indices.Size(), NULL);
            if (IsResultSuccessful(result, false))
            {
                PQclear(result);
                preparedQueries.Insert(inputStr);
            }
            else
            {
                printf(formatCopy.C_String());
                printf("\n");
                printf(lastError);
                N_assert(0);
                PQclear(result);
                return 0;
            }
        }

        va_list argptr;
        va_start(argptr, input);
        char **paramData;
        int *paramLength;
        int *paramFormat;
        ExtractArguments(argptr, indices, &paramData, &paramLength);
        paramFormat=OP_NEW_ARRAY<int>(indices.Size());
        for (unsigned int i=0; i < indices.Size(); i++)
            paramFormat[i]=PQEXECPARAM_FORMAT_BINARY;
        result = PQexecPrepared(pgConn, RakString("PGSQL_ExecuteVariadic_%i", preparedQueryIndex), indices.Size(), paramData, paramLength, paramFormat, PQEXECPARAM_FORMAT_BINARY );
        VariadicSQLParser::FreeArguments(indices, paramData, paramLength);
        OP_DELETE_ARRAY(paramFormat);
        va_end(argptr);

        if (IsResultSuccessful(result, false)==false)
        {
            printf(lastError);
            PQclear(result);
            return 0;
        }
        return result;
    }
    //------------------------------------------------------------------------
    /*
    PGresult * PostgreSQLInterface::QueryVariadic( const char * input, ... )
    {
        RakString query;
        PGresult *result;
        unsigned int i;
        DataStructures::List<IndexAndType> indices;
        if ( input==0 || input[0]==0 )
            return 0;

        // Find out how many params there are
        // Find out the type of each param (%f, %s)
        GetTypeMappingIndices( input, indices );

        char *paramData[512];
        int paramLength[512];
        int paramFormat[512];

        va_list argptr;
        int variadicArgIndex;
        va_start(argptr, input);
        for (variadicArgIndex=0, i=0; i < indices.Size(); i++, variadicArgIndex++)
        {
            if (typeMappings[indices[i].typeMappingIndex].inputType=='i' ||
                typeMappings[indices[i].typeMappingIndex].inputType=='d')
            {
                int val = va_arg( argptr, int );
                paramData[i]=(char*) &val;
                paramLength[i]=sizeof(val);
                if(NetSerializer::isNetEndian()==false) 
                    Serializer::mirror((unsigned char*) paramData[i], paramLength[i]);
            }
            else if (typeMappings[indices[i].typeMappingIndex].inputType=='s')
            {
                char* val = va_arg( argptr, char* );
                paramData[i]=val;
                paramLength[i]=(int) strlen(val);
            }
            else if (typeMappings[indices[i].typeMappingIndex].inputType=='b')
            {
                bool val = va_arg( argptr, bool );
                paramData[i]=(char*) &val;
                paramLength[i]=sizeof(bool);
                if(NetSerializer::isNetEndian()==false) 
                    Serializer::mirror((unsigned char*) paramData[i], paramLength[i]);
            }
            else if (typeMappings[indices[i].typeMappingIndex].inputType=='f')
            {
                NIIf val = va_arg( argptr, NIIf );
                paramData[i]=(char*) &val;
                paramLength[i]=sizeof(NIIf);
                if(NetSerializer::isNetEndian()==false) 
                    Serializer::mirror((unsigned char*) paramData[i], paramLength[i]);
            }
            else if (typeMappings[indices[i].typeMappingIndex].inputType=='g')
            {
                NIId val = va_arg( argptr, NIId );
                paramData[i]=(char*) &val;
                paramLength[i]=sizeof(NIId);
                if (NetSerializer::isNetEndian()==false) 
                    Serializer::mirror((unsigned char*) paramData[i], paramLength[i]);
            }
            else if (typeMappings[indices[i].typeMappingIndex].inputType=='a')
            {
                char* val = va_arg( argptr, char* );
                paramData[i]=val;
                variadicArgIndex++;
                paramLength[i]=va_arg( argptr, unsigned int );
            }
            paramFormat[i]=PQEXECPARAM_FORMAT_BINARY;
        }

        // Replace each %whatever with $index::type
        RakString inputCopy;
        inputCopy=input;
        unsigned int lastIndex=0;
        for (i=0; i < indices.Size(); i++)
        {
            query+=inputCopy.SubStr(lastIndex, indices[i].strIndex-lastIndex);
            query+=RakString("$%i::", i+1);
            query+=typeMappings[indices[i].typeMappingIndex].type;
            lastIndex=indices[i].strIndex+2; // +2 is to skip the %whateverCharacter
        }
        query+=inputCopy.SubStr(lastIndex, (unsigned int)-1);

        result = PQexecParams(pgConn, query.C_String(),indices.Size(),0,paramData,paramLength,paramFormat,PQEXECPARAM_FORMAT_BINARY);
        if (IsResultSuccessful(result, false)==false)
        {
            PQclear(result);
            return 0;
        }
        return result;
    }
    */
    void PostgreSQLInterface::ClearResult(PGresult * result)
    {
        PQclear(result);
    }
    //------------------------------------------------------------------------
}
}