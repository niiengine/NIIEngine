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
#include "SQLiteClientLoggerPlugin.h"
#include "NiiNetCommon.h"
#include "NetworkObj.h"

static const char COLUMN_NAMES_DELIMITER = ',';
static const int MAX_COLUMN_NAMES_LENGTH = 512;

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    SQLiteClientLoggerPlugin * SQLiteClientLoggerPlugin::logger;
    //------------------------------------------------------------------------
    SQLiteClientLoggerPlugin::SQLiteClientLoggerPlugin()
    {
        logger=this;
        tickCount=0;
        recursiveCheck=false;
        memoryConstraint=0;
    }
    //------------------------------------------------------------------------
    SQLiteClientLoggerPlugin::~SQLiteClientLoggerPlugin()
    {
        if (logger==this)
            logger=0;
    }
    //------------------------------------------------------------------------
    void SQLiteClientLoggerPlugin::SetServerParameters(const Address & address, RakString _dbIdentifier)
    {
        serverAddress = address.clone();
        dbIdentifier=_dbIdentifier;
    }
    //------------------------------------------------------------------------
    void SQLiteClientLoggerPlugin::SetMemoryConstraint(unsigned int constraint)
    {
        memoryConstraint=constraint;
    }
    //------------------------------------------------------------------------
    void SQLiteClientLoggerPlugin::IncrementAutoTickCount(void)
    {
        tickCount++;
    }
    //------------------------------------------------------------------------
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, 
        const char * tableName, const char * columnNames, const char * file, const int line,
            const ParameterListHelper & parameterList)
    {
        if (recursiveCheck==true)
            return SQLLR_RECURSION;
        recursiveCheck=true;

        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, parameterList.paramCount);

        if (parameterList.paramCount>=1)
            parameterList.p0.read(&bitStream);
        if (parameterList.paramCount>=2)
            parameterList.p1.read(&bitStream);
        if (parameterList.paramCount>=3)
            parameterList.p2.read(&bitStream);
        if (parameterList.paramCount>=4)
            parameterList.p3.read(&bitStream);
        if (parameterList.paramCount>=5)
            parameterList.p4.read(&bitStream);
        if (parameterList.paramCount>=6)
            parameterList.p5.read(&bitStream);
        if (parameterList.paramCount>=7)
            parameterList.p6.read(&bitStream);
        if (parameterList.paramCount>=8)
            parameterList.p7.read(&bitStream);
        if (parameterList.paramCount>=9)
            parameterList.p8.read(&bitStream);
        if (parameterList.paramCount>=10)
            parameterList.p9.read(&bitStream);
        if (parameterList.paramCount>=11)
            parameterList.p10.read(&bitStream);
        if (parameterList.paramCount>=12)
            parameterList.p11.read(&bitStream);
        if (parameterList.paramCount>=13)
            parameterList.p12.read(&bitStream);
        if (parameterList.paramCount>=14)
            parameterList.p13.read(&bitStream);
        if (parameterList.paramCount>=15)
            parameterList.p14.read(&bitStream);


        if (memoryConstraint!=0 && mPrc)
        {
            if (mPrc->getOutputSize(*serverAddress) + bitStream.getSize()>=memoryConstraint)
            {
                recursiveCheck = false;
                return SQLLR_WOULD_EXCEED_MEMORY_CONSTRAINT;
            }
        }

        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        recursiveCheck=false;
        return SQLLR_OK;
    }
    /*
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 0);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 1);
        p1->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 2);
        p1->read(&bitStream);
        p2->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 3);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 4);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        p4->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 5);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        p4->read(&bitStream);
        p5->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 6);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        p4->read(&bitStream);
        p5->read(&bitStream);
        p6->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 7);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        p4->read(&bitStream);
        p5->read(&bitStream);
        p6->read(&bitStream);
        p7->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 8);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        p4->read(&bitStream);
        p5->read(&bitStream);
        p6->read(&bitStream);
        p7->read(&bitStream);
        p8->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 9);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        p4->read(&bitStream);
        p5->read(&bitStream);
        p6->read(&bitStream);
        p7->read(&bitStream);
        p8->read(&bitStream);
        p9->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9, const LogParameter *p10 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 10);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        p4->read(&bitStream);
        p5->read(&bitStream);
        p6->read(&bitStream);
        p7->read(&bitStream);
        p8->read(&bitStream);
        p9->read(&bitStream);
        p10->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9, const LogParameter *p10, const LogParameter *p11 )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 11);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        p4->read(&bitStream);
        p5->read(&bitStream);
        p6->read(&bitStream);
        p7->read(&bitStream);
        p8->read(&bitStream);
        p9->read(&bitStream);
        p10->read(&bitStream);
        p11->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    SQLLogResult SQLiteClientLoggerPlugin::SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9, const LogParameter *p10, const LogParameter *p11, const LogParameter *p12  )
    {
        NetSerializer bitStream;
        SerializeHeader(&bitStream, isFunctionCall, tableName, columnNames, file, line, 12);
        p1->read(&bitStream);
        p2->read(&bitStream);
        p3->read(&bitStream);
        p4->read(&bitStream);
        p5->read(&bitStream);
        p6->read(&bitStream);
        p7->read(&bitStream);
        p8->read(&bitStream);
        p9->read(&bitStream);
        p10->read(&bitStream);
        p11->read(&bitStream);
        p12->read(&bitStream);
        send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
        return SQLLR_OK;
    }
    */
    //------------------------------------------------------------------------
    SQLLogResult SQLiteClientLoggerPlugin::CheckQuery(bool isFunction, const char *tableName, const char *columnNames, int numParameters)
    {
        (void) isFunction;

        if (recursiveCheck==true)
            return SQLLR_RECURSION;

        if (tableName==0 || tableName[0]==0)
            return SQLLR_TABLE_NAME_BLANK;
        if (isFunction==true)
            return SQLLR_OK;

        if (columnNames==0 || columnNames[0]==0)
        {
            if (numParameters==0)
                return SQLLR_OK;
            return SQLLR_TABLE_DESCRIPTOR_FORMAT_WRONG_PARAMETER_COUNT;
        }

        recursiveCheck=true;

        if (dbIdentifier.IsEmpty())
        {
            recursiveCheck=false;
            return SQLLR_NO_DATABASE_SET;
        }

        unsigned int parameterCount=1;
        unsigned int columnNamesIndex=0;
        for (columnNamesIndex=1; columnNamesIndex < 512 && columnNames[columnNamesIndex]; columnNamesIndex++)
        {
            if (columnNames[columnNamesIndex]==COLUMN_NAMES_DELIMITER)
            {
                if (columnNames[columnNamesIndex-1]==COLUMN_NAMES_DELIMITER)
                {
                    recursiveCheck=false;
                    return SQLLR_TABLE_DESCRIPTOR_FORMAT_INVALID_SYNTAX;
                }
                parameterCount++;
            }
        }

        recursiveCheck=false;
        if (columnNamesIndex==MAX_COLUMN_NAMES_LENGTH)
        {
            return SQLLR_COLUMN_NAMES_NOT_TERMINATED;
        }

        if (parameterCount!=numParameters)
        {
            return SQLLR_TABLE_DESCRIPTOR_FORMAT_WRONG_PARAMETER_COUNT;
        }

        return SQLLR_OK;
    }
    //------------------------------------------------------------------------
    void SQLiteClientLoggerPlugin::SerializeHeader(NetSerializer * bitStream, 
        bool isFunctionCall, const char * tableName, const char * columnNames,
            const char * file, const int line, unsigned char parameterCount) const
    {
        bitStream->write((Token) ID_SQLLITE_LOGGER);
        bitStream->write(dbIdentifier);
        bitStream->write(tableName);
        bitStream->write(line);
        bitStream->write(file);
        bitStream->write(tickCount);
        bitStream->write(N_Engine().getTimer().getMS());
        bitStream->write(isFunctionCall);
        bitStream->write(parameterCount);
        if (isFunctionCall==false && parameterCount>=1)
        {
            int stringIndices[64];
            int strIndex=0;
            stringIndices[strIndex++]=0;
            char columnNamesCopy[MAX_COLUMN_NAMES_LENGTH];
            N_assert(strlen(columnNames)<MAX_COLUMN_NAMES_LENGTH);
            strncpy(columnNamesCopy, columnNames, MAX_COLUMN_NAMES_LENGTH);
            columnNamesCopy[MAX_COLUMN_NAMES_LENGTH-1]=0;
            for (NCount i=0; columnNamesCopy[i]; ++i)
            {
                if (columnNamesCopy[i]==COLUMN_NAMES_DELIMITER)
                {
                    columnNamesCopy[i]=0;
                    stringIndices[strIndex++]=i+1;
                }
            }
            N_assert(strIndex==parameterCount);
            for (NCount i=0; i < parameterCount; ++i)
            {
                bitStream->write((char*)columnNamesCopy + stringIndices[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    void SQLiteClientLoggerPlugin::update()
    {
        SQLite3ClientPlugin::update();
    }
    //------------------------------------------------------------------------
}
}