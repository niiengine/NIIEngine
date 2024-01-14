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
#include "SQLiteServerLoggerPlugin.h"
#include "NiiNetLinkBase.h"
#include "NetworkObj.h"
#include "NiiNetCommon.h"
#include "SQLiteLoggerCommon.h"
#include "jpeglib.h"
#include "jpeg_memory_dest.h"
#include "NiiVFS.h"
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "DXTCompressor.h"

// http://web.utk.edu/~jplyon/sqlite/SQLite_optimization_FAQ.html

// See jmorecfg.h
/*
* Ordering of RGB data in scanlines passed to or from the application.
* If your application wants to deal with data in the order B,G,R, just
* change these macros.  You can also deal with formats such as R,G,B,X
* (one extra byte per pixel) by changing RGB_PIXELSIZE.  Note that changing
* the offsets will also change the order in which colormap data is organized.
* RESTRICTIONS:
* 1. The sample applications cjpeg,djpeg do NOT support modified RGB formats.
* 2. These macros only affect RGB<=>YCbCr color conversion, so they are not
*    useful if you are using JPEG color spaces other than YCbCr or grayscale.
* 3. The color quantizer modules will not behave desirably if RGB_PIXELSIZE
*    is not 3 (they don't understand about dummy color components!).  So you
*    can't use color quantization if you change that value.
*/

//#define RGB_RED		0	/* Offset of Red in an RGB scanline element */
//#define RGB_GREEN	1	/* Offset of Green */
//#define RGB_BLUE	2	/* Offset of Blue */
//#define RGB_PIXELSIZE	3	/* JSAMPLEs per RGB scanline element */

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    // JPEG ENCODING ERRORS
    struct my_error_mgr
    {
        struct jpeg_error_mgr pub;
    };
    //------------------------------------------------------------------------
    METHODDEF(void) my_error_exit (j_common_ptr cinfo) {}
    //------------------------------------------------------------------------
    #define FILE_COLUMN "SourceFile"
    #define LINE_COLUMN "SourceLine"
    #define TICK_COUNT_COLUMN "AutoTickCount"
    #define AUTO_IP_COLUMN "AutoIPAddress"
    #define TIMESTAMP_NUMERIC_COLUMN "TimestampNumeric"
    #define TIMESTAMP_TEXT_COLUMN "TimestampText"
    #define FUNCTION_CALL_FRIENDLY_TEXT "FunctionCallFriendlyText"
    #define FUNCTION_CALL_TABLE "'functionCalls'"
    #define FUNCTION_CALL_PARAMETERS_TABLE "'functionCallParameters'"
    //------------------------------------------------------------------------
    // Store packets in the CPUThreadInput until at most this much time has elapsed. This is so
    // batch processing can occur on multiple image sources at once
    static const TimeDurMS MAX_TIME_TO_BUFFER_PACKETS=1000;
    //------------------------------------------------------------------------
    // WTF am I getting this?
    // 2>SQLiteServerLoggerPlugin.obj : error LNK2019: unresolved external symbol _GetSqlDataTypeName referenced in function "struct SQLite3ServerPlugin::SQLExecThreadOutput __cdecl ExecSQLLoggingThread(struct SQLite3ServerPlugin::SQLExecThreadInput,bool *,void *)" (?ExecSQLLoggingThread@@YA?AUExecThreadOutput@SQLite3ServerPlugin@RakNet@@UExecThreadInput@23@PA_NPAX@Z)
    // 2>C:\RakNet\Debug\SQLiteServerLogger.exe : fatal error LNK1120: 1 unresolved externals
    static const char *sqlDataTypeNames[SQLLPDT_COUNT] =
    {
        "INTEGER",
        "INTEGER",
        "NUMERIC",
        "TEXT",
        "BLOB",
        "BLOB",
    };
    //------------------------------------------------------------------------
    const char *GetSqlDataTypeName2(SQLLoggerPrimaryDataType idx)
    {
        return sqlDataTypeNames[(int)idx];
    }
    //------------------------------------------------------------------------
    void CompressAsJpeg(char ** cptrInOut, uint32_t * sizeInOut, uint16_t imageWidth,
        uint16_t imageHeight, int16_t linePitch, unsigned char input_components)
    {
        TimeDurUS t1=N_Engine().getTimer().getUS();

        // Compress to jpg
        // http://www.google.com/codesearch/p?hl=en#I-_InJ6STRE/gthumb-1.108/libgthumb/pixbuf-utils.c&q=jpeg_create_compress
        // http://ftp.gnome.org	/ pub/	GNOME	/	sources	/gthumb	/1.108/	gthumb-1.108.tar.gz/

        struct jpeg_compress_struct cinfo;
        struct my_error_mgr jerr;
        cinfo.err = jpeg_std_error(&jerr.pub);
        jerr.pub.error_exit = my_error_exit;
        jpeg_create_compress(&cinfo);
        cinfo.smoothing_factor = 0;
        cinfo.optimize_coding = false;
        cinfo.image_width = imageWidth;
        cinfo.image_height = imageHeight;
        cinfo.input_components = input_components;
        cinfo.in_color_space = JCS_RGB;

        jpeg_set_defaults (&cinfo);
        cinfo.dct_method = JDCT_FLOAT;
        // Not sure why they made RGB_PIXELSIZE a global define. I added this so it only uses my value in this one place, to not break other things
        cinfo.hack_use_input_components_as_RGB_PIXELSIZE = 1;
        jpeg_set_quality (&cinfo, 75, TRUE);
        int jpegSizeAfterCompression = 0; //size of jpeg after compression
        char * storage = (char *)rakMalloc_Ex(*sizeInOut + 50000);
        jpeg_memory_dest(&cinfo, (JOCTET *)storage, *sizeInOut + 50000, &jpegSizeAfterCompression);

        JSAMPROW row_pointer[1];
        jpeg_start_compress (&cinfo, TRUE);
        while( cinfo.next_scanline < cinfo.image_height )
        {
            row_pointer[0] = (JSAMPROW) &((*cptrInOut)[cinfo.next_scanline * linePitch ]);
            jpeg_write_scanlines(&cinfo, row_pointer, 1);

        }

        /* finish off */
        jpeg_finish_compress (&cinfo);
        jpeg_destroy_compress(&cinfo);

        rakFree_Ex(*cptrInOut);
        *cptrInOut = (char*) rakRealloc_Ex(storage, jpegSizeAfterCompression);
        *sizeInOut=jpegSizeAfterCompression;

        TimeDurUS t2=N_Engine().getTimer().getUS();
        TimeDurUS diff=t2-t1;
    }
    //------------------------------------------------------------------------
    static bool needsDxtInit = true;
    static bool dxtCompressionSupported = false;
    static bool dxtCompressionEnabled = false;
    //------------------------------------------------------------------------
    class SQLiteServerLoggerJob : public Job
    {
    public:
        SQLiteServerLoggerJob(SQLiteServerLoggerPlugin::CPUThreadInput * in, bool set) : 
            Job(0), 
            mInput(in), 
            mDXTCompressor(set){}
        ~SQLiteServerLoggerJob()
        {
            for (j=0; j < mInput->arraySize; j++)
            {
                N_delete mInput->cpuInputArray[j].mMsg;
            }
            OP_DELETE(mInput);
        }
        
        SQLiteServerLoggerPlugin::CPUThreadInput * mInput;
        bool mDXTCompressor;
    };
    
    class SQLiteServerLoggerJobResult : public JobResult
    {
    public:
        SQLiteServerLoggerJobResult(Job * src, SQLiteServerLoggerPlugin::CPUThreadOutput * out) : 
            JobResult(src), 
            mOutput(out){}
        ~SQLiteServerLoggerJobResult()
        {
            for (j = 0; j < mOutput->arraySize; ++j)
            {
                CPUThreadOutputNode * cpuThreadOutputNode = mOutput->cpuOutputNodeArray[j];
                N_delete cpuThreadOutputNode->mMsg;
                for (k = 0; k < cpuThreadOutputNode->parameterCount; ++k)
                    cpuThreadOutputNode->parameterList[k].Free();
                OP_DELETE(cpuThreadOutputNode);
            }
            OP_DELETE(mOutput);
        }
        
        void onPrcBegin(Job * jop)
        {
            if(dxtCompressionEnabled)
            {
                if (needsDxtInit == true)
                {
                    dxtCompressionSupported = DXTCompressor::Initialize();
                    if (dxtCompressionSupported == false)
                    {
                        printf("Warning, DXT compressor failed to start.\nImages will be compressed with jpg instead.\n");
                    }
                }
                needsDxtInit = false;
            }
        }
        
        void onPrcEnd(Job * jop)
        {
            if(dxtCompressionEnabled)
            {
                if(dxtCompressionSupported)
                {
                    dxtCompressionSupported = false;
                    DXTCompressor::Shutdown();
                }
                needsDxtInit = true;
            }
        }
        
        SQLiteServerLoggerPlugin::CPUThreadOutput * mOutput;
    };
    
    class SQLiteServerLoggerJobPrc : public JobPrc
    {
    public:
        SQLiteServerLoggerJobPrc(SQLiteServerLoggerPlugin * server) : JobPrc(true), mServer(server){}
        
        JobResult * handle(Job * jop);
        
        void handle(JobResult * result);
        
        SQLiteServerLoggerPlugin * mServer;
    };
    //------------------------------------------------------------------------
    JobResult * SQLiteServerLoggerJobPrc::handle(Job * jop)
    {
        SQLiteServerLoggerPlugin::CPUThreadInput * cpuThreadInput = static_cast<SQLiteServerLoggerJob *>(jop)->mInput;
        SQLiteServerLoggerPlugin::CPUThreadOutput * cpuThreadOutput = OP_NEW<SQLiteServerLoggerPlugin::CPUThreadOutput>();
        cpuThreadOutput->arraySize = cpuThreadInput->arraySize;
        
        int i;
        for(i = 0; i < cpuThreadInput->arraySize; ++i)
        {
            cpuThreadOutput->cpuOutputNodeArray[i] = OP_NEW<SQLiteServerLoggerPlugin::CPUThreadOutputNode>();
            SQLiteServerLoggerPlugin::CPUThreadOutputNode * outputNode = cpuThreadOutput->cpuOutputNodeArray[i];
            SocketMessage * msg = cpuThreadInput->cpuInputArray[i].mMsg;
            RakString dbIdentifier = cpuThreadInput->cpuInputArray[i].dbIdentifier;
            // outputNode->whenMessageArrived = cpuThreadInput->cpuInputArray[i].whenMessageArrived;
            outputNode->mMsg = msg;

            msg->mAddress.read(outputNode->ipAddressString, true);
            NetSerializer in(msg->data, msg->length, false);
            in.skipRead(1, 0);
            in.read(outputNode->dbIdentifier);
            in.read(outputNode->tableName);
            outputNode->tableName.SQLEscape();
            in.read(outputNode->line);
            in.read(outputNode->file);
            in.read(outputNode->tickCount);
            in.read(outputNode->clientSendingTime);
            in.read(outputNode->isFunctionCall);
            in.read(outputNode->parameterCount);
            if (outputNode->isFunctionCall==false)
            {
                RakString columnName;
                for (NCount i = 0; i < outputNode->parameterCount; ++i)
                {
                    in.read(columnName);
                    columnName.SQLEscape();
                    columnName.RemoveCharacter(' ');
                    outputNode->insertingColumnNames.Push(columnName);
                }
            }
            int parameterCountIndex = 0;

            while (parameterCountIndex < outputNode->parameterCount)
            {
                outputNode->parameterList[parameterCountIndex].write(&in);

                if (outputNode->parameterList[parameterCountIndex].size>0)
                {
                    ++parameterCountIndex;
                }
                else
                {
                    // Skip empty parameters
                    if (outputNode->isFunctionCall == false)
                        outputNode->insertingColumnNames.RemoveAtIndex(parameterCountIndex);
                    outputNode->parameterCount--;
                }
            }

            for (parameterCountIndex=0; parameterCountIndex < outputNode->parameterCount; parameterCountIndex++)
            {
                if (outputNode->parameterList[parameterCountIndex].type==SQLLPDT_IMAGE)
                {

                    bool dxtCompressSuccess=false;
                    if (dxtCompressionSupported)
                    {
                        char *outputData;
                        int bufferSize = DXTCompressor::GetBufferSize(DXT1,
                            outputNode->parameterList[parameterCountIndex].imageWidth,
                            outputNode->parameterList[parameterCountIndex].imageHeight);
                        int ddsHeaderSize = DXTCompressor::GetDDSHeaderSize();
                        outputData = (char*) rakMalloc_Ex(bufferSize + ddsHeaderSize);
                        dxtCompressSuccess = DXTCompressor::CompressImageData(
                        DXT1,
                        outputNode->parameterList[parameterCountIndex].data.cptr,
                        outputNode->parameterList[parameterCountIndex].imageWidth,
                        outputNode->parameterList[parameterCountIndex].imageHeight,
                        outputData+ddsHeaderSize, false, outputNode->parameterList[parameterCountIndex].sourceFormatIsBGRA );

                        if (dxtCompressSuccess)
                        {
                            rakFree_Ex(outputNode->parameterList[parameterCountIndex].data.cptr);
                            DXTCompressor::WriteDDSHeader(DXT1,
                                outputNode->parameterList[parameterCountIndex].imageWidth,
                                outputNode->parameterList[parameterCountIndex].imageHeight,
                                bufferSize,
                                outputData);
                            outputNode->parameterList[parameterCountIndex].data.cptr=outputData;
                            outputNode->parameterList[parameterCountIndex].size=bufferSize + ddsHeaderSize;
                        }
                        else
                        {
                            rakFree_Ex(outputData);
                        }
                    }

                    if (dxtCompressSuccess==false)
                    {
                        if (outputNode->parameterList[parameterCountIndex].sourceFormatIsBGRA)
                        {
                            // Endian swap each color component. Input should be RGBA
                            //					int pixelIndex;
                            //					int rowIndex;
                            int imageHeight = outputNode->parameterList[parameterCountIndex].imageHeight;
                            //					int imageWidth = outputNode->parameterList[parameterCountIndex].imageWidth;
                            int mPixelSize = outputNode->parameterList[parameterCountIndex].input_components;
                            int linePitch = outputNode->parameterList[parameterCountIndex].linePitch;
                            char *dataPtr = outputNode->parameterList[parameterCountIndex].data.cptr;
                            int totalBytes=linePitch*imageHeight;
                            char *endPtr = dataPtr+totalBytes;
                            unsigned char temp1;
                            if (mPixelSize==3)
                            {
                                while (dataPtr!=endPtr)
                                {
                                    temp1=dataPtr[2];
                                    dataPtr[2]=dataPtr[0];
                                    dataPtr[0]=temp1;
                                    dataPtr+=3;
                                }
                            }
                            else
                            {
                                N_assert(mPixelSize==4);
                                while (dataPtr!=endPtr)
                                {
                                    temp1=dataPtr[2];
                                    dataPtr[2]=dataPtr[0];
                                    dataPtr[0]=temp1;
                                    dataPtr+=4;
                                }
                            }
                        }

                        CompressAsJpeg(
                            &outputNode->parameterList[parameterCountIndex].data.cptr,
                            &outputNode->parameterList[parameterCountIndex].size,
                            outputNode->parameterList[parameterCountIndex].imageWidth,
                            outputNode->parameterList[parameterCountIndex].imageHeight,
                            outputNode->parameterList[parameterCountIndex].linePitch,
                            outputNode->parameterList[parameterCountIndex].input_components
                            );
                    }
                }
            }
        }
        OP_DELETE(cpuThreadInput);
        SQLiteServerLoggerJobResult * re = N_new SQLiteServerLoggerJobResult(job, cpuThreadOutput);
        return re;
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerJobPrc::handle(JobResult * result)
    {
        //while (cpuLoggerThreadPool.getOutputSize())
        //{
            CPUThreadOutput * cpuThreadOutput = static_cast<>(result)->mOutput;
            for (arrayIndex = 0; arrayIndex < cpuThreadOutput->arraySize; arrayIndex++)
            {
                ExecSQLLoggingJob * sqlThreadInput = N_new ExecSQLLoggingJob();
                CPUThreadOutputNode * outputNode = cpuThreadOutput->cpuOutputNodeArray[arrayIndex];
                sqlThreadInput->cpuOutputNode = outputNode;
                // bool alreadyHasLoggedInSession=false;
                int sessionIndex;
                for (sessionIndex = 0; sessionIndex < loggedInSessions.Size(); sessionIndex++)
                {
                    if (loggedInSessions[sessionIndex].mAddress==outputNode->mMsg->mAddress &&
                        loggedInSessions[sessionIndex].sessionName==outputNode->dbIdentifier)
                    {
                        break;
                    }
                }

                NCount idx;
                if (sessionManagementMode==USE_ANY_DB_HANDLE)
                {
                    if (dbHandles.GetSize()>0)
                        idx=0;
                    else
                        idx=-1;
                }
                else
                {
                    idx = dbHandles.GetIndexOf(outputNode->dbIdentifier);
                    if (sessionIndex==loggedInSessions.Size() && (createDirectoryForFile==true || idx==-1) && sessionManagementMode==CREATE_EACH_NAMED_DB_HANDLE)
                    {
                        // Create it, and set idx to the new one
                        idx = CreateDBHandle(outputNode->dbIdentifier);
                    }
                    else if (idx==-1)
                    {
                        if (sessionManagementMode==CREATE_EACH_NAMED_DB_HANDLE || sessionManagementMode==CREATE_SHARED_NAMED_DB_HANDLE)
                        {
                            // Create it, and set idx to the new one
                            idx = CreateDBHandle(outputNode->dbIdentifier);
                        }
                        if (sessionManagementMode==USE_NAMED_DB_HANDLE)
                        {
                            N_assert("Can't find named DB handle\n" && 0);
                        }
                    }
                    else
                    {
                        // Use idx
                    }
                }

                if (idx==-1)
                {
                    N_delete outputNode->mMsg;
                    OP_DELETE(outputNode);
                }
                else
                {
                    if (sessionIndex == loggedInSessions.Size())
                    {
                        SessionNameAndSystemAddress sassy;
                        sassy.sessionName = outputNode->dbIdentifier;
                        sassy.mAddress = outputNode->mMsg->mAddress;
                        sassy.referencedPointer = dbHandles[idx].dbHandle;
                        TimeDurMS curTime = N_Engine().getTimer().getMS();
                        TimeDurMS dbAge = curTime - dbHandles[idx].whenCreated;
                        sassy.timestampDelta = dbAge - outputNode->clientSendingTime ;
                        loggedInSessions.Push(sassy);
                        sessionIndex = loggedInSessions.Size()-1;
                    }

                    N_delete outputNode->mMsg;
                    sqlThreadInput->dbHandle = dbHandles[idx].dbHandle;
                    outputNode->clientSendingTime += loggedInSessions[sessionIndex].timestampDelta;
                    ExecSQLLoggingJobPrc * prc = N_new ExecSQLLoggingJobPrc(mServer);
                    mServer->sqlLoggerThreadPool.add(sqlThreadInput, prc);
                }
            }
            OP_DELETE(cpuThreadOutput);
        //}
    }
    //------------------------------------------------------------------------
    struct SQLPreparedStatements
    {
        SQLPreparedStatements()
        {
            selectNameFromMaster = 0;
            insertIntoFunctionCalls = 0;
            insertIntoFunctionCallParameters = 0;
        }
        sqlite3_stmt *selectNameFromMaster;
        sqlite3_stmt *insertIntoFunctionCalls;
        sqlite3_stmt *insertIntoFunctionCallParameters;
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    void DeleteBlobOrText(void * v)
    {
        LogParameter::Free(v);
    }
    //------------------------------------------------------------------------
    class ExecSQLLoggingJob : public Job
    {
    public:
        ExecSQLLoggingJob() : Job(0)
        {}
        
        ~ExecSQLLoggingJob()
        {
            OP_DELETE(cpuOutputNode);
            cpuOutputNode = 0;
        }
        
        sqlite3 * dbHandle;
        CPUThreadOutputNode * cpuOutputNode;
    };
    class ExecSQLLoggingJobResult : public JobResult
    {
    public:
        ExecSQLLoggingJobResult(Job * src, SQLiteServerLoggerPlugin * server) : JobResult(Job * src), mServer(server){}
        
        ~ExecSQLLoggingJobResult()
        {
            OP_DELETE(cpuOutputNode);
            cpuOutputNode = 0;
            
            mServer->CloseUnreferencedSessions();
        }
        
        // cpuOutputNode gets deallocated here
        CPUThreadOutputNode * cpuOutputNode;
        SQLiteServerLoggerPlugin * mServer;
    };
    class ExecSQLLoggingJobPrc : public JobPrc
    {
    public:
        ExecSQLLoggingJobPrc(SQLiteServerLoggerPlugin * server) : mPreparedStatements(0), mServer(server){}
        
        JobResult * handle(Job * jop);
        
        void handle(JobResult * result);
        
        virtual void onPrcBegin(Job * jop)
        {
            mPreparedStatements = OP_NEW<SQLPreparedStatements>();
        }
        
        virtual void onPrcEnd(Job * jop)
        {
            if (mPreparedStatements->selectNameFromMaster) 
                sqlite3_finalize(mPreparedStatements->selectNameFromMaster);
            if (mPreparedStatements->insertIntoFunctionCalls) 
                sqlite3_finalize(mPreparedStatements->insertIntoFunctionCalls);
            if (mPreparedStatements->insertIntoFunctionCallParameters) 
                sqlite3_finalize(mPreparedStatements->insertIntoFunctionCallParameters);
            OP_DELETE(mPreparedStatements);
        }
        
        SQLPreparedStatements * mPreparedStatements;
        SQLiteServerLoggerPlugin * mServer;
    };
    //------------------------------------------------------------------------
    JobResult * ExecSQLLoggingJobPrc::handle(Job * job)
    {
        ExecSQLLoggingJob * sqlThreadInput = static_cast<ExecSQLLoggingJob *>(job);
        SQLiteServerLoggerPlugin::CPUThreadOutputNode * cpuOutputNode = sqlThreadInput->cpuOutputNode;
        sqlThreadInput->cpuOutputNode = 0;
        ExecSQLLoggingJobResult * sqlThreadOutput = N_new ExecSQLLoggingJobResult(job, mServer);
        sqlThreadOutput->cpuOutputNode = cpuOutputNode;
        sqlite3 * dbHandle = sqlThreadInput->dbHandle;
        char * errorMsg;

        sqlite3_exec(dbHandle,"BEGIN TRANSACTION", 0, 0, 0);

        int rc;
        if (cpuOutputNode->isFunctionCall)
        {
            if (mPreparedStatements->selectNameFromMaster==0)
            {
                // Create function tables if they are not there already
                if (sqlite3_prepare_v2(
                    dbHandle,
                    "SELECT name FROM sqlite_master WHERE type='table' AND name="FUNCTION_CALL_TABLE" ",
                    -1,
                    &mPreparedStatements->selectNameFromMaster,
                    0
                    )!=SQLITE_OK)
                {
                    N_assert("Failed PRAGMA table_info for function tables in SQLiteServerLoggerPlugin.cpp" && 0);
                    for (int i=0; i < cpuOutputNode->parameterCount; i++)
                        cpuOutputNode->parameterList[i].Free();
                    sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
                    return sqlThreadOutput;
                }
            }
            rc = sqlite3_step(mPreparedStatements->selectNameFromMaster);
            sqlite3_reset(mPreparedStatements->selectNameFromMaster);

            if (rc!=SQLITE_ROW)
            {
                // Create table if it isn't there already
                rc = sqlite3_exec(dbHandle,"CREATE TABLE "FUNCTION_CALL_TABLE" (functionId_pk INTEGER PRIMARY KEY, "FUNCTION_CALL_FRIENDLY_TEXT" TEXT, functionName TEXT, "FILE_COLUMN" TEXT, "LINE_COLUMN" INTEGER, "TICK_COUNT_COLUMN" INTEGER, "AUTO_IP_COLUMN" TEXT, "TIMESTAMP_TEXT_COLUMN" TIMESTAMP DATE DEFAULT (datetime('now','localtime')), "TIMESTAMP_NUMERIC_COLUMN" NUMERIC )", 0, 0, &errorMsg);
                N_assert(rc==SQLITE_OK);
                sqlite3_free(errorMsg);
                // See sqlDataTypeNames for *val
                rc = sqlite3_exec(dbHandle,"CREATE TABLE "FUNCTION_CALL_PARAMETERS_TABLE" (fcpId_pk INTEGER PRIMARY KEY, functionId_fk integer NOT NULL, value TEXT, FOREIGN KEY (functionId_fk) REFERENCES "FUNCTION_CALL_TABLE" (functionId_pk))", 0, 0, &errorMsg);
                N_assert(rc==SQLITE_OK);
                sqlite3_free(errorMsg);
            }
            else
            {
                // Table already there
            }

            // Insert into function calls
            int parameterCountIndex;
            RakString functionCallFriendlyText("%s(", cpuOutputNode->tableName.C_String());
            for (parameterCountIndex=0; parameterCountIndex < cpuOutputNode->parameterCount; parameterCountIndex++)
            {
                if (parameterCountIndex!=0)
                    functionCallFriendlyText+=", ";
                switch (cpuOutputNode->parameterList[parameterCountIndex].type)
                {
                case SQLLPDT_POINTER:
                    if (cpuOutputNode->parameterList[parameterCountIndex].size==4)
                        functionCallFriendlyText+=RakString("%p", cpuOutputNode->parameterList[parameterCountIndex].data.i);
                    else
                        functionCallFriendlyText+=RakString("%p", cpuOutputNode->parameterList[parameterCountIndex].data.ll);
                    break;
                case SQLLPDT_INTEGER:
                    switch (cpuOutputNode->parameterList[parameterCountIndex].size)
                    {
                    case 1:
                        functionCallFriendlyText+=RakString("%i", cpuOutputNode->parameterList[parameterCountIndex].data.c);
                        break;
                    case 2:
                        functionCallFriendlyText+=RakString("%i", cpuOutputNode->parameterList[parameterCountIndex].data.s);
                        break;
                    case 4:
                        functionCallFriendlyText+=RakString("%i", cpuOutputNode->parameterList[parameterCountIndex].data.i);
                        break;
                    case 8:
                        functionCallFriendlyText+=RakString("%i", cpuOutputNode->parameterList[parameterCountIndex].data.ll);
                        break;
                    }
                    break;
                case SQLLPDT_REAL:
                    if (cpuOutputNode->parameterList[parameterCountIndex].size==sizeof(float))
                        functionCallFriendlyText+=RakString("%f", cpuOutputNode->parameterList[parameterCountIndex].data.f);
                    else
                        functionCallFriendlyText+=RakString("%d", cpuOutputNode->parameterList[parameterCountIndex].data.d);
                    break;
                case SQLLPDT_TEXT:
                    functionCallFriendlyText+='"';
                    if (cpuOutputNode->parameterList[parameterCountIndex].size>0)
                        functionCallFriendlyText.AppendBytes(cpuOutputNode->parameterList[parameterCountIndex].data.cptr, cpuOutputNode->parameterList[parameterCountIndex].size);
                    functionCallFriendlyText+='"';
                    break;
                case SQLLPDT_IMAGE:
                    functionCallFriendlyText+=RakString("<%i byte image>", cpuOutputNode->parameterList[parameterCountIndex].size, cpuOutputNode->parameterList[parameterCountIndex].data.cptr);
                    break;
                case SQLLPDT_BLOB:
                    functionCallFriendlyText+=RakString("<%i byte binary>", cpuOutputNode->parameterList[parameterCountIndex].size, cpuOutputNode->parameterList[parameterCountIndex].data.cptr);
                    break;
                }
            }

            functionCallFriendlyText+=");";

            if (mPreparedStatements->insertIntoFunctionCalls==0)
            {
                rc = sqlite3_prepare_v2(dbHandle, "INSERT INTO "FUNCTION_CALL_TABLE" ("FUNCTION_CALL_FRIENDLY_TEXT", "FILE_COLUMN", "LINE_COLUMN", "TICK_COUNT_COLUMN", "AUTO_IP_COLUMN", "TIMESTAMP_NUMERIC_COLUMN" ,functionName) VALUES (?,?,?,?,?,?,?)", -1, &mPreparedStatements->insertIntoFunctionCalls, 0);
                if (rc!=SQLITE_DONE && rc!=SQLITE_OK)
                {
                    N_assert("Failed INSERT INTO "FUNCTION_CALL_PARAMETERS_TABLE" in SQLiteServerLoggerPlugin.cpp" && 0);
                }
            }
            sqlite3_bind_text(mPreparedStatements->insertIntoFunctionCalls, 1, functionCallFriendlyText.C_String(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(mPreparedStatements->insertIntoFunctionCalls, 2, cpuOutputNode->file.C_String(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(mPreparedStatements->insertIntoFunctionCalls, 3, cpuOutputNode->line);
            sqlite3_bind_int(mPreparedStatements->insertIntoFunctionCalls, 4, cpuOutputNode->tickCount);
            sqlite3_bind_text(mPreparedStatements->insertIntoFunctionCalls, 5, cpuOutputNode->ipAddressString, -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(mPreparedStatements->insertIntoFunctionCalls, 6, (uint32_t) (cpuOutputNode->clientSendingTime));
            sqlite3_bind_text(mPreparedStatements->insertIntoFunctionCalls, 7, cpuOutputNode->tableName.C_String(), -1, SQLITE_TRANSIENT);
            rc = sqlite3_step(mPreparedStatements->insertIntoFunctionCalls);
            sqlite3_reset(mPreparedStatements->insertIntoFunctionCalls);
            if (rc != SQLITE_DONE && rc!=SQLITE_OK)
            {
                N_assert("Failed binding parameters to functionCalls in SQLiteServerLoggerPlugin.cpp" && 0);
            }
            // read last row id
            // Requires that this thread has its own connection
            sqlite3_int64 lastRowId = sqlite3_last_insert_rowid(dbHandle);

            if (mPreparedStatements->insertIntoFunctionCallParameters==0)
            {
                rc = sqlite3_prepare_v2(dbHandle, "INSERT INTO functionCallParameters (functionId_fk, value) VALUES (?,?);", -1, &mPreparedStatements->insertIntoFunctionCallParameters, 0);
                N_assert(rc==SQLITE_OK);
                sqlite3_bind_int64(mPreparedStatements->insertIntoFunctionCallParameters, 1, lastRowId);
            }

            // Insert into parameters table
            for (parameterCountIndex=0; parameterCountIndex < cpuOutputNode->parameterCount; parameterCountIndex++)
            {
                switch (cpuOutputNode->parameterList[parameterCountIndex].type)
                {
                case SQLLPDT_POINTER:
                case SQLLPDT_INTEGER:
                    switch (cpuOutputNode->parameterList[parameterCountIndex].size)
                    {
                    case 1:
                        sqlite3_bind_int(mPreparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.c);
                        break;
                    case 2:
                        sqlite3_bind_int(mPreparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.s);
                        break;
                    case 4:
                        sqlite3_bind_int(mPreparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.i);
                        break;
                    case 8:
                        sqlite3_bind_int64(mPreparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.ll);
                        break;
                    }
                    break;
                case SQLLPDT_REAL:
                    if (cpuOutputNode->parameterList[parameterCountIndex].size==sizeof(float))
                        sqlite3_bind_double(mPreparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.f);
                    else
                        sqlite3_bind_double(mPreparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.d);
                    break;
                case SQLLPDT_TEXT:
                    sqlite3_bind_text(mPreparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.cptr, cpuOutputNode->parameterList[parameterCountIndex].size, 0);
                    break;
                case SQLLPDT_IMAGE:
                case SQLLPDT_BLOB:
                    sqlite3_bind_blob(mPreparedStatements->insertIntoFunctionCallParameters, 2, cpuOutputNode->parameterList[parameterCountIndex].data.vptr, cpuOutputNode->parameterList[parameterCountIndex].size, DeleteBlobOrText);
                    cpuOutputNode->parameterList[parameterCountIndex].DoNotFree();
                    break;
                default:
                    N_assert("Hit invalid default in case label in SQLiteServerLoggerPlugin.cpp" && 0);
                }
                rc = sqlite3_step(mPreparedStatements->insertIntoFunctionCallParameters);
                sqlite3_reset(mPreparedStatements->insertIntoFunctionCallParameters);
                if (rc!=SQLITE_DONE && rc!=SQLITE_OK)
                {
                    N_assert("Failed sqlite3_step to bind functionCall parameters in SQLiteServerLoggerPlugin.cpp" && 0);
                }
            }
        }
        else
        {
            sqlite3_stmt * pragmaTableInfo;
            RakString pragmaQuery("PRAGMA table_info(%s)",cpuOutputNode->tableName.C_String());
            if (sqlite3_prepare_v2(
                dbHandle,
                pragmaQuery.C_String(),
                -1,
                &pragmaTableInfo,
                0
                )!=SQLITE_OK)
            {
                N_assert("Failed PRAGMA table_info for tableName in SQLiteServerLoggerPlugin.cpp" && 0);
                for (int i=0; i < cpuOutputNode->parameterCount; i++)
                    cpuOutputNode->parameterList[i].Free();
                sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
                return sqlThreadOutput;
            }

            int rc = sqlite3_step(pragmaTableInfo);
            DataStructures::List<RakString> existingColumnNames;
            DataStructures::List<RakString> existingColumnTypes;
            char *errorMsg;
            while (rc == SQLITE_ROW)
            {
                const int nameColumn = 1;
                const int typeColumn = 2;
                N_assert(strcmp(sqlite3_column_name(pragmaTableInfo,nameColumn),"name")==0);
                N_assert(strcmp(sqlite3_column_name(pragmaTableInfo,typeColumn),"type")==0);
                RakString columnName = sqlite3_column_text(pragmaTableInfo,nameColumn);
                RakString columnType = sqlite3_column_text(pragmaTableInfo,typeColumn);
                existingColumnNames.Push(columnName);
                existingColumnTypes.Push(columnType);

                rc = sqlite3_step(pragmaTableInfo);
            }
            sqlite3_reset(pragmaTableInfo);
            sqlite3_finalize(pragmaTableInfo);
            if (rc==SQLITE_ERROR)
            {
                N_assert("Failed sqlite3_step in SQLiteServerLoggerPlugin.cpp" && 0);
                for (int i=0; i < cpuOutputNode->parameterCount; i++)
                    cpuOutputNode->parameterList[i].Free();
                sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
                return sqlThreadOutput;
            }

            int existingColumnNamesIndex,insertingColumnNamesIndex;
            if (existingColumnNames.Size()==0)
            {
                RakString createQuery("CREATE TABLE %s (rowId_pk INTEGER PRIMARY KEY, "FILE_COLUMN" TEXT, "LINE_COLUMN" INTEGER, "TICK_COUNT_COLUMN" INTEGER, "AUTO_IP_COLUMN" TEXT, "TIMESTAMP_TEXT_COLUMN" TIMESTAMP DATE DEFAULT (datetime('now','localtime')), "TIMESTAMP_NUMERIC_COLUMN" NUMERIC",cpuOutputNode->tableName.C_String());

                for (int i=0; i < cpuOutputNode->parameterCount; i++)
                {
                    createQuery+=", ";
                    createQuery+=cpuOutputNode->insertingColumnNames[i];
                    createQuery+=" ";
                    createQuery+=GetSqlDataTypeName2(cpuOutputNode->parameterList[i].type);
                }
                createQuery+=" )";

                sqlite3_exec(dbHandle,
                    createQuery.C_String(),
                    0, 0, &errorMsg);
                N_assert(errorMsg==0);
                sqlite3_free(errorMsg);
            }
            else
            {
                // Compare what is there (columnNames,columnTypes) to what we are adding. Add what is missing
                bool alreadyExists;
                for (insertingColumnNamesIndex=0; insertingColumnNamesIndex<(int) cpuOutputNode->insertingColumnNames.Size(); insertingColumnNamesIndex++)
                {
                    alreadyExists=false;
                    for (existingColumnNamesIndex=0; existingColumnNamesIndex<(int) existingColumnNames.Size(); existingColumnNamesIndex++)
                    {
                        if (existingColumnNames[existingColumnNamesIndex]==cpuOutputNode->insertingColumnNames[insertingColumnNamesIndex])
                        {
                            // Type mismatch? If so, abort
                            if (existingColumnTypes[existingColumnNamesIndex]!=GetSqlDataTypeName2(cpuOutputNode->parameterList[insertingColumnNamesIndex].type))
                            {
                                printf("Error: Column type mismatch. TableName=%s. ColumnName%s. Existing=%s. New=%s\n",
                                    cpuOutputNode->tableName.C_String(),
                                    existingColumnNames[existingColumnNamesIndex].C_String(),
                                    existingColumnTypes[existingColumnNamesIndex].C_String(),
                                    GetSqlDataTypeName2(cpuOutputNode->parameterList[insertingColumnNamesIndex].type)
                                    );
                                for (int i=0; i < cpuOutputNode->parameterCount; i++)
                                    cpuOutputNode->parameterList[i].Free();
                                sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
                                return sqlThreadOutput;
                            }

                            alreadyExists=true;
                            break;
                        }
                    }

                    if (alreadyExists==false)
                    {
                        sqlite3_exec(dbHandle,
                            RakString("ALTER TABLE %s ADD %s %s",
                            cpuOutputNode->tableName.C_String(),
                            cpuOutputNode->insertingColumnNames[insertingColumnNamesIndex].C_String(),
                            GetSqlDataTypeName2(cpuOutputNode->parameterList[insertingColumnNamesIndex].type)
                            ).C_String(),
                            0, 0, &errorMsg);
                        N_assert(errorMsg==0);
                        sqlite3_free(errorMsg);
                    }
                }
            }

            // Insert new row
            RakString insertQuery("INSERT INTO %s (", cpuOutputNode->tableName.C_String());
            int parameterCountIndex;
            for (parameterCountIndex=0; parameterCountIndex<cpuOutputNode->parameterCount; parameterCountIndex++)
            {
                if (parameterCountIndex!=0)
                    insertQuery+=", ";
                insertQuery+=cpuOutputNode->insertingColumnNames[parameterCountIndex].C_String();
            }
            // Add file and line to the end
            insertQuery+=", "FILE_COLUMN", "LINE_COLUMN", "TICK_COUNT_COLUMN", "AUTO_IP_COLUMN", "TIMESTAMP_NUMERIC_COLUMN" ) VALUES (";

            for (parameterCountIndex=0; parameterCountIndex<cpuOutputNode->parameterCount+5; parameterCountIndex++)
            {
                if (parameterCountIndex!=0)
                    insertQuery+=", ?";
                else
                    insertQuery+="?";
            }
            insertQuery+=")";

            sqlite3_stmt *customStatement;
            if (sqlite3_prepare_v2(
                dbHandle,
                insertQuery.C_String(),
                -1,
                &customStatement,
                0
                )!=SQLITE_OK)
            {
                N_assert("Failed second sqlite3_prepare_v2 in SQLiteServerLoggerPlugin.cpp" && 0);
                for (int i=0; i < cpuOutputNode->parameterCount; i++)
                    cpuOutputNode->parameterList[i].Free();
                sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
                return sqlThreadOutput;
            }

            for (parameterCountIndex=0; parameterCountIndex<cpuOutputNode->parameterCount; parameterCountIndex++)
            {
                switch (cpuOutputNode->parameterList[parameterCountIndex].type)
                {
                    case SQLLPDT_POINTER:
                    case SQLLPDT_INTEGER:
                        switch (cpuOutputNode->parameterList[parameterCountIndex].size)
                        {
                        case 1:
                            sqlite3_bind_int(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.c);
                            break;
                        case 2:
                            sqlite3_bind_int(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.s);
                            break;
                        case 4:
                            sqlite3_bind_int(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.i);
                            break;
                        case 8:
                            sqlite3_bind_int64(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.ll);
                            break;
                        }
                        break;
                    case SQLLPDT_REAL:
                        if (cpuOutputNode->parameterList[parameterCountIndex].size==sizeof(float))
                            sqlite3_bind_double(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.f);
                        else
                            sqlite3_bind_double(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.d);
                        break;
                    case SQLLPDT_TEXT:
                        sqlite3_bind_text(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.cptr, cpuOutputNode->parameterList[parameterCountIndex].size, DeleteBlobOrText);
                        cpuOutputNode->parameterList[parameterCountIndex].DoNotFree();
                        break;
                    case SQLLPDT_IMAGE:
                    case SQLLPDT_BLOB:
                        sqlite3_bind_blob(customStatement, parameterCountIndex+1, cpuOutputNode->parameterList[parameterCountIndex].data.vptr, cpuOutputNode->parameterList[parameterCountIndex].size, DeleteBlobOrText);
                        cpuOutputNode->parameterList[parameterCountIndex].DoNotFree();
                        break;
                }
            }

            // Add file and line to the end
            sqlite3_bind_text(customStatement, parameterCountIndex+1, cpuOutputNode->file.C_String(), (int) cpuOutputNode->file.GetLength(), SQLITE_TRANSIENT);
            sqlite3_bind_int(customStatement, parameterCountIndex+2, cpuOutputNode->line);
            sqlite3_bind_int(customStatement, parameterCountIndex+3, cpuOutputNode->tickCount);
            sqlite3_bind_text(customStatement, parameterCountIndex+4, cpuOutputNode->ipAddressString, -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(customStatement, parameterCountIndex+5, (uint32_t) (cpuOutputNode->clientSendingTime));

            rc = sqlite3_step(customStatement);
            if (rc!=SQLITE_DONE && rc!=SQLITE_OK)
            {
                N_assert("Failed sqlite3_step to bind blobs in SQLiteServerLoggerPlugin.cpp" && 0);
                for (int i=0; i < cpuOutputNode->parameterCount; i++)
                    cpuOutputNode->parameterList[i].Free();
                sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);
                return sqlThreadOutput;
            }
            sqlite3_finalize(customStatement);
        }
        sqlite3_exec(dbHandle,"END TRANSACTION", 0, 0, 0);

        for (int i=0; i < cpuOutputNode->parameterCount; i++)
            cpuOutputNode->parameterList[i].Free();

        return sqlThreadOutput;
    }
    //------------------------------------------------------------------------
    SQLiteServerLoggerPlugin::SQLiteServerLoggerPlugin()
    {
        sessionManagementMode = CREATE_EACH_NAMED_DB_HANDLE;
        createDirectoryForFile = true;
        cpuThreadInput = 0;
        //dxtCompressionEnabled = false;
    }
    //------------------------------------------------------------------------
    SQLiteServerLoggerPlugin::~SQLiteServerLoggerPlugin()
    {
        StopCPUSQLThreads();
        OP_DELETE(cpuThreadInput);
        CloseAllSessions();
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::update()
    {
        SQLite3ServerPlugin::update();
        bool hadOutput = false;
        int arrayIndex;

        PushCpuThreadInputIfNecessary();
    }
    //------------------------------------------------------------------------
    ConnectReceiveType SQLiteServerLoggerPlugin::onMessage(SocketMessage * msg)
    {
        ConnectReceiveType prr = SQLite3ServerPlugin::onMessage(msg);
        if (prr != CRT_Normal)
            return prr;

        switch (msg->data[0])
        {
        case ID_SQLLITE_LOGGER:
            {
                NetSerializer in(msg->data, msg->length, false);
                in.skipRead(1, 0);
                RakString dbIdentifier;
                in.read(dbIdentifier);

                if (sessionManagementMode == CREATE_EACH_NAMED_DB_HANDLE)
                {
                    String senderAddr;
                    msg->mAddress.read(senderAddr, true);
                    dbIdentifier += ':';
                    dbIdentifier += senderAddr;
                }

                CPUThreadInput * ti = LockCpuThreadInput();
                ti->cpuInputArray[ti->arraySize].mMsg = msg;
                ti->cpuInputArray[ti->arraySize].dbIdentifier = dbIdentifier;
                UnlockCpuThreadInput();

                return CRT_Inner;
            }
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::onConnectClose(const Address & address,
        UniqueID id, ConnectCloseType lostConnectionReason)
    {
        RakString removedSession;
        NCount i = 0;
        while (i < loggedInSessions.Size())
        {
            if (loggedInSessions[i].mAddress == address)
            {
                removedSession = loggedInSessions[i].sessionName;
                loggedInSessions.RemoveAtIndexFast(i);
            }
            else
                ++i;
        }

        CloseUnreferencedSessions();
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::CloseUnreferencedSessions(void)
    {
        DataStructures::List<sqlite3 *> sessionNames;
        NCount j;
        for (j = 0; j < loggedInSessions.Size(); ++j)
        {
            if (sessionNames.GetIndexOf(loggedInSessions[j].referencedPointer) == -1)
                sessionNames.Push(loggedInSessions[j].referencedPointer);
        }

        DataStructures::List<sqlite3*> unreferencedHandles;
        bool isReferenced;
        for (NCount i = 0; i < dbHandles.GetSize(); ++i)
        {
            if (dbHandles[i].dbAutoCreated)
            {
                j = 0;
                isReferenced=false;
                for(j = 0; j < sessionNames.Size(); ++j)
                {
                    if (sessionNames[j] == dbHandles[i].dbHandle)
                    {
                        isReferenced = true;
                        break;
                    }
                }

                if(isReferenced == false)
                {
                    unreferencedHandles.Push(dbHandles[i].dbHandle);
                }
            }
        }

        if (unreferencedHandles.Size())
        {
            if (!sqlLoggerThreadPool.getInputSize())
            {
                Nsleep(100);
                while (sqlLoggerThreadPool.getWorkingCount() > 0)
                    Nsleep(30);
                for (NCount k = 0; k < unreferencedHandles.Size(); ++k)
                {
                    RemoveDBHandle(unreferencedHandles[k], true);
                }
            }

            if (dbHandles.GetSize()==0)
                StopCPUSQLThreads();
        }
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::CloseAllSessions()
    {
        loggedInSessions.Clear(false);
        CloseUnreferencedSessions();
    }
    //------------------------------------------------------------------------
    NCount SQLiteServerLoggerPlugin::CreateDBHandle(RakString dbIdentifier)
    {
        if (sessionManagementMode!=CREATE_EACH_NAMED_DB_HANDLE && sessionManagementMode!=CREATE_SHARED_NAMED_DB_HANDLE)
            return dbHandles.GetIndexOf(dbIdentifier);

        RakString filePath = newDatabaseFilePath;
        if (createDirectoryForFile)
        {
            filePath+=dbIdentifier;
            filePath.TerminateAtLastCharacter('.');
            filePath.MakeFilePath();

            time_t     now;
            struct tm  *ts;
            char       buf[80];

            /* Get the current time */
            now = time(NULL);

            /* Format and print the time, "ddd yyyy-mm-dd hh:mm:ss zzz" */
            ts = localtime(&now);
            strftime(buf, sizeof(buf), "__%a_%Y-%m-%d__%H;%M", ts);

            filePath+=buf;
            filePath+=RakString("__%i", N_Engine().getTimer().getMS());

            filePath.MakeFilePath();
        }
        // With no file data, just creates the directory structure
        FileSystem::wirteFile(filePath.C_String(), 0, 0);

        RakString fileSafeDbIdentifier = dbIdentifier;
        fileSafeDbIdentifier.TerminateAtLastCharacter(':');
        RakString fileNameWithPath=filePath+fileSafeDbIdentifier;

        // SQL Open this file, and register it
        sqlite3 *database;
        if (sqlite3_open_v2(fileNameWithPath.C_String(), &database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0)!=SQLITE_OK)
        {
            N_assert("sqlite3_open_v2 failed in SQLiteServerLoggerPlugin.cpp" && 0);
            return -1;
        }
        if (AddDBHandle(dbIdentifier, database, true))
        {

            char *errorMsg;
            int rc = sqlite3_exec(database,"PRAGMA synchronous=OFF", 0, 0, &errorMsg);
            N_assert(rc==SQLITE_OK);
            sqlite3_free(errorMsg);
            rc = sqlite3_exec(database,"PRAGMA count_changes=OFF", 0, 0, &errorMsg);
            N_assert(rc==SQLITE_OK);
            sqlite3_free(errorMsg);

            printf("Created %s\n", fileNameWithPath.C_String());
            return dbHandles.GetIndexOf(dbIdentifier);
        }
        else
        {
            N_assert("Failed to call AddDbHandle" && 0);
            return -1;
        }
        return -1;
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::SetSessionManagementMode(
        SessionManagementMode _sessionManagementMode, bool _createDirectoryForFile,
            const char *_newDatabaseFilePath)
    {
        sessionManagementMode=_sessionManagementMode;
        createDirectoryForFile=_createDirectoryForFile;
        newDatabaseFilePath=_newDatabaseFilePath;
        newDatabaseFilePath.MakeFilePath();
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::shutdown()
    {
        CloseAllSessions();
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::StopCPUSQLThreads()
    {
        NCount i;
        cpuLoggerThreadPool.shutdown();
        ClearCpuThreadInput();
        cpuLoggerThreadPool.removeAllInput();
        cpuLoggerThreadPool.removeAllOutput();

        sqlLoggerThreadPool.shutdown();
        sqlLoggerThreadPool.removeAllInput();
        sqlLoggerThreadPool.removeAllOutput();
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::GetProcessingStatus(ProcessingStatus * processingStatus)
    {
        if (cpuThreadInput)
            processingStatus->packetsBuffered = cpuThreadInput->arraySize;
        else
            processingStatus->packetsBuffered = 0;
        processingStatus->cpuPendingProcessing = cpuLoggerThreadPool.getInputSize();
        processingStatus->cpuProcessedAwaitingDeallocation = cpuLoggerThreadPool.getOutputSize();
        processingStatus->cpuNumThreadsWorking = cpuLoggerThreadPool.getWorkingCount();
        processingStatus->sqlPendingProcessing = sqlLoggerThreadPool.getInputSize();
        processingStatus->sqlProcessedAwaitingDeallocation = sqlLoggerThreadPool.getOutputSize();
        processingStatus->sqlNumThreadsWorking = sqlLoggerThreadPool.getWorkingCount();
    }
    //------------------------------------------------------------------------
    SQLiteServerLoggerPlugin::CPUThreadInput * SQLiteServerLoggerPlugin::LockCpuThreadInput(void)
    {
        if (cpuThreadInput == 0)
        {
            cpuThreadInput = OP_NEW<CPUThreadInput>();
            cpuThreadInput->arraySize = 0;
            whenCpuThreadInputAllocated=N_Engine().getTimer().getMS();
        }
        return cpuThreadInput;
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::ClearCpuThreadInput(void)
    {
        if (cpuThreadInput != 0)
        {
            for (NCount i = 0; i < cpuThreadInput->arraySize; ++i)
                N_delete cpuThreadInput->cpuInputArray[i].mMsg;
            OP_DELETE(cpuThreadInput);
            cpuThreadInput = 0;
        }
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::UnlockCpuThreadInput()
    {
        cpuThreadInput->arraySize++;
        if (cpuThreadInput->arraySize == MAX_PACKETS_PER_CPU_INPUT_THREAD)
            PushCpuThreadInput();
        else
            PushCpuThreadInputIfNecessary();
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::CancelLockCpuThreadInput()
    {
        if (cpuThreadInput->arraySize == 0)
        {
            OP_DELETE(cpuThreadInput);
            cpuThreadInput = 0;
        }
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::PushCpuThreadInput(void)
    {
        // cpu threads can probably be as many as I want
        if (cpuLoggerThreadPool.isRun() == false)
            cpuLoggerThreadPool.startup(1);

        // sql logger threads should probably be limited to 1 since I'm doing transaction locks and calling sqlite3_last_insert_rowid
        if (sqlLoggerThreadPool.isRun() == false)
            sqlLoggerThreadPool.startup(1);

        SQLiteServerLoggerJob * job = N_new SQLiteServerLoggerJob(cpuThreadInput);
        SQLiteServerLoggerJobPrc * prc = N_new SQLiteServerLoggerJobPrc(this);
        cpuLoggerThreadPool.add(job, prc);
        cpuThreadInput = 0;
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::PushCpuThreadInputIfNecessary(void)
    {
        TimeDurMS curTime = N_Engine().getTimer().getMS();
        if (cpuThreadInput && curTime - whenCpuThreadInputAllocated > MAX_TIME_TO_BUFFER_PACKETS)
            PushCpuThreadInput();
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::onAttach()
    {
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::onDetach()
    {
    }
    //------------------------------------------------------------------------
    void SQLiteServerLoggerPlugin::SetEnableDXTCompression(bool enable)
    {
        dxtCompressionEnabled = enable;
    }
    //------------------------------------------------------------------------
}
}