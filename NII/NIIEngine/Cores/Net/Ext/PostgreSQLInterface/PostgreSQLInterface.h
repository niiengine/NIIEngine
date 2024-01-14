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
#ifndef __POSTGRESQL_INTERFACE_H
#define __POSTGRESQL_INTERFACE_H

struct pg_conn;
typedef struct pg_conn PGconn;

struct pg_result;
typedef struct pg_result PGresult;

#include "RakString.h"
#include "DS_OrderedList.h"

namespace NII
{
namespace NII_NET
{
    class PostgreSQLInterface
    {
    public:
        PostgreSQLInterface();
        virtual ~PostgreSQLInterface();

        /// Connect to the database using the connection string
        /// \param[in] conninfo See the postgre docs
        /// \return True on success, false on failure.
        bool Connect(const char *conninfo);

        /// Use a connection allocated elsewehre
        void AssignConnection(PGconn *_pgConn);

        /// Get the instance of PGconn
        PGconn *GetPGConn(void) const;

        /// Disconnect from the database
        void Disconnect(void);

        /// If any of the above functions fail, the error string is stored internally.  Call this to get it.
        virtual const char *GetLastError(void) const;

        // Returns  DEFAULT EXTRACT(EPOCH FROM current_timestamp))
        long long GetEpoch(void);

        // Returns a string containing LOCALTIMESTAMP on the server
        char * GetLocalTimestamp(void);

        static bool PQGetValueFromBinary(int * output, PGresult * result,
            unsigned int rowIndex, const char * columnName);
        static bool PQGetValueFromBinary(int * output, PGresult * result,
            int rowIndex, const char *columnName);
        static bool PQGetValueFromBinary(unsigned int * output, PGresult * result,
            int rowIndex, const char *columnName);
        static bool PQGetValueFromBinary(long long * output, PGresult * result,
            int rowIndex, const char *columnName);
        static bool PQGetValueFromBinary(NIIf * output, PGresult * result,
            int rowIndex, const char *columnName);
        static bool PQGetValueFromBinary(NIId * output, PGresult * result,
            int rowIndex, const char *columnName);
        static bool PQGetValueFromBinary(bool * output, PGresult * result,
            int rowIndex, const char *columnName);
        static bool PQGetValueFromBinary(RakString * output, PGresult * result,
            int rowIndex, const char *columnName);
        static bool PQGetValueFromBinary(char ** output, unsigned int *outputLength,
            PGresult *result, int rowIndex, const char *columnName);
        static bool PQGetValueFromBinary(char ** output, int * outputLength,
            PGresult * result, int rowIndex, const char *columnName);

        static void EncodeQueryInput(const char * colName, unsigned int value,
            RakString & paramTypeStr, RakString &valueStr, int &numParams,
                char **paramData, int * paramLength, int * paramFormat);
        static void EncodeQueryInput(const char * colName, bool value,
            RakString & paramTypeStr, RakString &valueStr, int &numParams,
                char **paramData, int * paramLength, int * paramFormat);
        static void EncodeQueryInput(const char * colName, int value,
            RakString & paramTypeStr, RakString &valueStr, int &numParams,
                char **paramData, int * paramLength, int * paramFormat);
        static void EncodeQueryInput(const char * colName, NIIf value,
            RakString & paramTypeStr, RakString &valueStr, int &numParams,
                char **paramData, int * paramLength, int * paramFormat);
        static void EncodeQueryInput(const char * colName, char * binaryData,
            int count, RakString & paramTypeStr, RakString & valueStr,
                int & numParams, char ** paramData, int * paramLength, int * paramFormat,
                    bool writeEmpty);
        static void EncodeQueryInput(const char * colName, const char *str,
            RakString &paramTypeStr, RakString &valueStr, int &numParams,
                char **paramData, int *paramLength, int *paramFormat, bool writeEmpty,
                    const char *type = "text");
        static void EncodeQueryInput(const char * colName, const RakString &str,
            RakString &paramTypeStr, RakString &valueStr, int &numParams,
                char **paramData, int *paramLength, int *paramFormat, bool writeEmpty,
                    const char *type = "text");

        static void EncodeQueryUpdate(const char * colName, unsigned int value,
            RakString & valueStr, int & numParams, char ** paramData,
                int * paramLength, int * paramFormat);
        static void EncodeQueryUpdate(const char * colName, int value,
            RakString & valueStr, int & numParams, char ** paramData,
                int * paramLength, int * paramFormat);
        static void EncodeQueryUpdate(const char * colName, NIIf value,
            RakString &valueStr, int &numParams, char **paramData,
                int * paramLength, int *paramFormat);
        static void EncodeQueryUpdate(const char * colName, char *binaryData,
            int count, RakString &valueStr, int &numParams, char **paramData,
                int * paramLength, int *paramFormat);
        static void EncodeQueryUpdate(const char * colName, const char *str,
            RakString &valueStr, int &numParams, char **paramData,
                int * paramLength, int *paramFormat, const char *type = "text");
        static void EncodeQueryUpdate(const char * colName, const RakString &str,
            RakString &valueStr, int &numParams, char **paramData,
                int * paramLength, int *paramFormat, const char *type = "text");

        // Standard query
        PGresult * QueryVariadic(const char * input, ...);
        static void ClearResult(PGresult * result);

        // Pass queries to the server
        bool ExecuteBlockingCommand(const char * command, PGresult ** result,
            bool rollbackOnFailure);
        bool IsResultSuccessful(PGresult * result, bool rollbackOnFailure);
        void Rollback(void);
        static void EndianSwapInPlace(char * data, int dataLength);
        RakString GetEscapedString(const char * input) const;
    protected:
        PGconn * pgConn;
        bool pgConnAllocatedHere;
        bool isConnected;
        char lastError[1024];
        // Connection parameters
        RakString _conninfo;
        DataStructures::List<RakString> preparedQueries;
    };
}
}
#endif
