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

#include "FileList.h"

#if _RAKNET_SUPPORT_FileOperations == 1
    #include <stdio.h> // N_printf
    #if defined(ANDROID)
    #include <asm/io.h>
#elif (N_PLAT == N_PLAT_WIN32) || defined(__MINGE32__)
    #include <io.h>
#elif (N_PLAT != N_PLAT_OSX) && (N_PLAT != N_PLAT_IOS) && !defined ( __APPLE_CC__ ) && !defined ( __PPC__ ) && !defined ( __FreeBSD__ ) && !defined ( __S3E__ )
    #include <sys/io.h>
#endif

#ifdef N_PLAT == N_PLAT_WIN32
    // For mkdir
    #include <direct.h>
#else
    #include <sys/stat.h>
#endif

#include "NiiPreProcess.h"
#include "DS_Queue.h"
#include "StringTable.h"
#include "NiiNetSerializer.h"
#include "NiiVFS.h"
#include "LinuxStrings.h"

#if N_PLAT == N_PLAT_WIN32
    #include <malloc.h>
#else
	#if !defined ( __FreeBSD__ )
	#include <alloca.h>
	#endif
    #include <unistd.h>
    #include <stdlib.h>
    #include <sys/stat.h>
    #include "_FindFirst.h"
    #include <stdint.h> //defines intptr_t
#endif

namespace NII
{
namespace NII_NET
{
    #define MAX_FILENAME_LENGTH 512
    static const Nui32 HASH_LENGTH = 4;
    //------------------------------------------------------------------------
    /// First callback called when FileList::AddFilesFromDirectory() starts
    void FLP_Printf::OnAddFilesFromDirectoryStarted(FileList * fileList, char * dir)
    {
        (void) fileList;
        N_printf("Adding files from directory %s\n",dir);
    }
    //------------------------------------------------------------------------
    /// Called for each directory, when that directory begins processing
    void FLP_Printf::OnDirectory(FileList * fileList, char * dir, Nui32 directoriesRemaining) {
        (void) fileList;
        N_printf("Adding %s. %i remaining.\n", dir, directoriesRemaining);}
    //------------------------------------------------------------------------
    void FLP_Printf::OnFilePushesComplete(Address * address, Nui16 setID)
    {
        (void) setID;

        String str;
        address.read(str, true);
        N_printf("File pushes complete to %s\n", str.c_str());
    }
    //------------------------------------------------------------------------
    void FLP_Printf::OnSendAborted(Address * address)
    {
        String str;
        address.read(str, true);
        N_printf("Send aborted to %s\n", str.c_str());
    }
    //------------------------------------------------------------------------
    FileList::FileList()
    {
    }
    //------------------------------------------------------------------------
    FileList::~FileList()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void FileList::AddFile(const String & filepath, const char *filename, FileListNodeContext context)
    {
        if(filepath == String(""))
            return;

        char *data;
        //std::fstream file;
        //file.open(filename, std::ios::in | std::ios::binary);

        FILE *fp = fopen(filepath.c_str(), "rb");
        if (fp==0)
            return;
        fseek(fp, 0, SEEK_END);
        int length = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        if (length > (int) ((Nui32)-1 / 8))
        {
            // If this N_assert hits, split up your file. You could also change NBitCount in NiiNetCommon.h to unsigned long long but this is not recommended for performance reasons
            N_assert("Cannot add files over 536 MB" && 0);
            fclose(fp);
            return;
        }


    #if N_AllocaStackSize !=0
        bool usedAlloca=false;
        if (length < N_AllocaStackSize)
        {
            data = (char *) alloca(length);
            usedAlloca=true;
        }
        else
    #endif
        {
            data = (char *)N_alloc(length);
            N_assert(data);
        }

        fread(data, 1, length, fp);
        AddFile(filename, filepath, data, length, length, context);
        fclose(fp);

    #if N_AllocaStackSize !=0
        if (usedAlloca==false)
    #endif
            N_free(data);
    }
    //------------------------------------------------------------------------
    void FileList::AddFile(const char *filename, const char *fullPathToFile,
        const char *data, const Nui32 dataLength, const Nui32 fileLength,
            FileListNodeContext context, bool isAReference, bool takeDataPointer)
    {
        if (filename==0)
            return;
        if (strlen(filename)>MAX_FILENAME_LENGTH)
        {
            // Should be enough for anyone
            N_assert(0);
            return;
        }
        // If adding a reference, do not send data
        N_assert(isAReference==false || data==0);
        // Avoid duplicate insertions unless the data is different, in which case overwrite the old data
        Nui32 i;
        for (i=0; i<fileList.Size();i++)
        {
            if (strcmp(fileList[i].filename, filename)==0)
            {
                if (fileList[i].fileLengthBytes==fileLength && fileList[i].dataLengthBytes==dataLength &&
                    (dataLength==0 || fileList[i].mData==0 ||
                    memcmp(fileList[i].mData, data, dataLength)==0
                    ))
                    // Exact same file already here
                    return;

                // File of the same name, but different contents, so overwrite
                N_free(fileList[i].mData);
                fileList.RemoveAtIndex(i);
                break;
            }
        }

        FileListNode n;
        if (dataLength && data)
        {
            if (takeDataPointer)
            {
                n.mData=(char*) data;
            }
            else
            {
                n.mData=(char*) N_alloc(dataLength);
                N_assert(n.mData);
                memcpy(n.mData, data, dataLength);
            }
        }
        else
            n.mData=0;
        n.dataLengthBytes=dataLength;
        n.fileLengthBytes=fileLength;
        n.isAReference=isAReference;
        n.context=context;
        if (n.context.dataPtr==0)
            n.context.dataPtr=n.mData;
        if (n.context.dataLength==0)
            n.context.dataLength=dataLength;
        n.filename=filename;
        n.fullPathToFile=fullPathToFile;

        fileList.Insert(n);
    }
    //------------------------------------------------------------------------
    void FileList::AddFilesFromDirectory(const char *applicationDirectory, const char *subDirectory, bool writeHash, bool writeData, bool recursive, FileListNodeContext context)
    {
        DataStructures::Queue<char*> dirList;
        char root[260];
        char fullPath[520];
        _finddata_t fileInfo;
        intptr_t dir;
        FILE *fp;
        char *dirSoFar, *fileData;
        dirSoFar=(char*) N_alloc(520);
        N_assert(dirSoFar);

        if (applicationDirectory)
            strcpy(root, applicationDirectory);
        else
            root[0]=0;

        int rootLen=(int)strlen(root);
        if (rootLen)
        {
            strcpy(dirSoFar, root);
            if (FixEndingSlash(dirSoFar))
                rootLen++;
        }
        else
            dirSoFar[0]=0;

        if (subDirectory)
        {
            strcat(dirSoFar, subDirectory);
            FixEndingSlash(dirSoFar);
        }
        for (Nui32 flpcIndex=0; flpcIndex < fileListProgressCallbacks.Size(); flpcIndex++)
            fileListProgressCallbacks[flpcIndex]->OnAddFilesFromDirectoryStarted(this, dirSoFar);

        dirList.Push(dirSoFar);
        while (dirList.Size())
        {
            dirSoFar=dirList.Pop();
            strcpy(fullPath, dirSoFar);
            // Changed from *.* to * for Linux compatibility
            strcat(fullPath, "*");


                    dir=_findfirst(fullPath, &fileInfo );
            if (dir==-1)
            {
                _findclose(dir);
                N_free(dirSoFar);
                Nui32 i;
                for (i=0; i < dirList.Size(); i++)
                    N_free(dirList[i]);
                return;
            }

    //		N_printf("Adding %s. %i remaining.\n", fullPath, dirList.Size());
            for (Nui32 flpcIndex=0; flpcIndex < fileListProgressCallbacks.Size(); flpcIndex++)
                fileListProgressCallbacks[flpcIndex]->OnDirectory(this, fullPath, dirList.Size());

            do
            {
                        // no guarantee these entries are first...
                        if (strcmp("." , fileInfo.name) == 0 ||
                            strcmp("..", fileInfo.name) == 0)
                        {
                            continue;
                        }

                if ((fileInfo.attrib & (_A_HIDDEN | _A_SUBDIR | _A_SYSTEM))==0)
                {
                    strcpy(fullPath, dirSoFar);
                    strcat(fullPath, fileInfo.name);
                    fileData=0;

                    for (Nui32 flpcIndex=0; flpcIndex < fileListProgressCallbacks.Size(); flpcIndex++)
                        fileListProgressCallbacks[flpcIndex]->OnFile(this, dirSoFar, fileInfo.name, fileInfo.size);

                    if (writeData && writeHash)
                    {
                        fp = fopen(fullPath, "rb");
                        if (fp)
                        {
                            fileData= (char*) N_alloc(fileInfo.size+HASH_LENGTH);
                            N_assert(fileData);
                            fread(fileData+HASH_LENGTH, fileInfo.size, 1, fp);
                            fclose(fp);

                            Nui32 hash = FastHash(fileData+HASH_LENGTH, fileInfo.size);
                            if(NetSerializer::isFlipData())
                                Serializer::mirror((Nui8*) &hash, sizeof(hash));
                            memcpy(fileData, &hash, HASH_LENGTH);

                            //					sha1.Reset();
                            //					sha1.update( ( Nui8* ) fileData+HASH_LENGTH, fileInfo.size );
                            //					sha1.Final();
                            //					memcpy(fileData, sha1.GetHash(), HASH_LENGTH);
                            // File data and hash
                            AddFile((const char*)fullPath+rootLen, fullPath, fileData, fileInfo.size+HASH_LENGTH, fileInfo.size, context);
                        }
                    }
                    else if (writeHash)
                    {
    //					sha1.Reset();
    //					DR_SHA1.hashFile((char*)fullPath);
    //					sha1.Final();
                        Nui32 hash = 0;
                        FILE *fp = fopen(fullPath, "rb");
                        if (fp!=0)
                            hash = FastHash(fp);
                        fclose(fp);

                        if(NetSerializer::isFlipData())
                            Serializer::mirror((Nui8*) &hash, sizeof(hash));

                        // Hash only
                    //	AddFile((const char*)fullPath+rootLen, (const char*)sha1.GetHash(), HASH_LENGTH, fileInfo.size, context);
                        AddFile((const char*)fullPath+rootLen, fullPath, (const char*)&hash, HASH_LENGTH, fileInfo.size, context);
                    }
                    else if (writeData)
                    {
                        fileData= (char*) N_alloc(fileInfo.size);
                        N_assert(fileData);
                        fp = fopen(fullPath, "rb");
                        fread(fileData, fileInfo.size, 1, fp);
                        fclose(fp);

                        // File data only
                        AddFile(fullPath+rootLen, fullPath, fileData, fileInfo.size, fileInfo.size, context);
                    }
                    else
                    {
                        // Just the filename
                        AddFile(fullPath+rootLen, fullPath, 0, 0, fileInfo.size, context);
                    }

                    if (fileData)
                        N_free(fileData);
                }
                else if ((fileInfo.attrib & _A_SUBDIR) && (fileInfo.attrib & (_A_HIDDEN | _A_SYSTEM))==0 && recursive)
                {
                    char *newDir=(char*) N_alloc(520);
                    N_assert(newDir);
                    strcpy(newDir, dirSoFar);
                    strcat(newDir, fileInfo.name);
                    strcat(newDir, "/");
                    dirList.Push(newDir);
                }

            } while (_findnext(dir, &fileInfo ) != -1);

            _findclose(dir);
            N_free(dirSoFar);
        }
    }
    //------------------------------------------------------------------------
    void FileList::Clear(void)
    {
        Nui32 i;
        for (i = 0; i < fileList.Size(); ++i)
        {
            N_free(fileList[i].mData);
        }
        fileList.Clear(false);
    }
    //------------------------------------------------------------------------
    void FileList::read(NetSerializer * out)
    {
        out->writeCompress(fileList.Size());
        NCount i;
        for(i = 0; i < fileList.Size(); ++i)
        {
            out->writeCompress(fileList[i].context.op);
            out->writeCompress(fileList[i].context.flnc_extraData1);
            out->writeCompress(fileList[i].context.flnc_extraData2);
            StringCompressor::Instance()->EncodeString(fileList[i].filename.C_String(), MAX_FILENAME_LENGTH, out);

            bool writeFileData = (fileList[i].dataLengthBytes>0) == true;
            out->write(writeFileData);
            if (writeFileData)
            {
                out->writeCompress(fileList[i].dataLengthBytes);
                out->write(fileList[i].mData, fileList[i].dataLengthBytes);
            }

            out->write((bool)(fileList[i].fileLengthBytes == fileList[i].dataLengthBytes));
            if (fileList[i].fileLengthBytes!=fileList[i].dataLengthBytes)
                out->writeCompress(fileList[i].fileLengthBytes);
        }
    }
    //------------------------------------------------------------------------
    bool FileList::import(NetSerializer * in)
    {
        bool b, dataLenNonZero=false, fileLenMatchesDataLen=false;
        char filename[512];
        Nui32 fileListSize;
        FileListNode n;
        b=in->readCompress(fileListSize);
    #ifdef _DEBUG
        N_assert(b);
        N_assert(fileListSize < 10000);
    #endif
        if(b == false || fileListSize > 10000)
            return false; // Sanity check
        Clear();
        Nui32 i;
        for(i = 0; i < fileListSize; ++i)
        {
            in->readCompress(n.context.op);
            in->readCompress(n.context.flnc_extraData1);
            in->readCompress(n.context.flnc_extraData2);
            StringCompressor::Instance()->DecodeString((char*)filename, MAX_FILENAME_LENGTH, in);
            in->read(dataLenNonZero);
            if (dataLenNonZero)
            {
                in->readCompress(n.dataLengthBytes);
                // sanity check
                if (n.dataLengthBytes>2000000000)
                {
    #ifdef _DEBUG
                    N_assert(n.dataLengthBytes<=2000000000);
    #endif
                    return false;
                }
                n.mData=(char*) N_alloc((size_t) n.dataLengthBytes);
                N_assert(n.mData);
                in->read(n.mData, n.dataLengthBytes);
            }
            else
            {
                n.dataLengthBytes=0;
                n.mData=0;
            }

            b = in->read(fileLenMatchesDataLen);
            if(fileLenMatchesDataLen)
                n.fileLengthBytes=(Nui32) n.dataLengthBytes;
            else
                b = in->readCompress(n.fileLengthBytes);
    #ifdef _DEBUG
            N_assert(b);
    #endif
            if(b == 0)
            {
                Clear();
                return false;
            }
            n.filename = filename;
            n.fullPathToFile = filename;
            fileList.Insert(n);
        }

        return true;
    }
    //------------------------------------------------------------------------
    void FileList::GetDeltaToCurrent(FileList *input, FileList *output, const char *dirSubset, const char *remoteSubdir)
    {
        // For all files in this list that do not match the input list, write them to the output list.
        // dirSubset allows checking only a portion of the files in this list.
        Nui32 thisIndex, inputIndex;
        Nui32 dirSubsetLen, localPathLen, remoteSubdirLen;
        bool match;
        if (dirSubset)
            dirSubsetLen = (Nui32) strlen(dirSubset);
        else
            dirSubsetLen = 0;
        if (remoteSubdir && remoteSubdir[0])
        {
            remoteSubdirLen=(Nui32) strlen(remoteSubdir);
            if (FileSystem::isPathSlash(remoteSubdir[remoteSubdirLen-1]))
                remoteSubdirLen--;
        }
        else
            remoteSubdirLen=0;

        for (thisIndex=0; thisIndex < fileList.Size(); thisIndex++)
        {
            localPathLen = (Nui32) fileList[thisIndex].filename.GetLength();
            while (localPathLen>0)
            {
                if (FileSystem::isPathSlash(fileList[thisIndex].filename[localPathLen-1]))
                {
                    localPathLen--;
                    break;
                }
                localPathLen--;
            }

            // fileList[thisIndex].filename has to match dirSubset and be shorter or equal to it in length.
            if (dirSubsetLen>0 &&
                (localPathLen<dirSubsetLen ||
                _strnicmp(fileList[thisIndex].filename.C_String(), dirSubset, dirSubsetLen)!=0 ||
                (localPathLen>dirSubsetLen && FileSystem::isPathSlash(fileList[thisIndex].filename[dirSubsetLen])==false)))
                continue;

            match=false;
            for (inputIndex=0; inputIndex < input->fileList.Size(); inputIndex++)
            {
                // If the filenames, hashes, and lengths match then skip this element in fileList.  Otherwise write it to output
                if (_stricmp(input->fileList[inputIndex].filename.C_String()+remoteSubdirLen,fileList[thisIndex].filename.C_String()+dirSubsetLen)==0)
                {
                    match=true;
                    if (input->fileList[inputIndex].fileLengthBytes==fileList[thisIndex].fileLengthBytes &&
                        input->fileList[inputIndex].dataLengthBytes==fileList[thisIndex].dataLengthBytes &&
                        memcmp(input->fileList[inputIndex].mData,fileList[thisIndex].mData,(size_t) fileList[thisIndex].dataLengthBytes)==0)
                    {
                        // File exists on both machines and is the same.
                        break;
                    }
                    else
                    {
                        // File exists on both machines and is not the same.
                        output->AddFile(fileList[thisIndex].filename, fileList[thisIndex].fullPathToFile, 0,0, fileList[thisIndex].fileLengthBytes, FileListNodeContext(0,0,0,0), false);
                        break;
                    }
                }
            }
            if (match==false)
            {
                // Other system does not have the file at all
                output->AddFile(fileList[thisIndex].filename, fileList[thisIndex].fullPathToFile, 0,0, fileList[thisIndex].fileLengthBytes, FileListNodeContext(0,0,0,0), false);
            }
        }
    }
    //------------------------------------------------------------------------
    void FileList::ListMissingOrChangedFiles(const char *applicationDirectory, FileList *missingOrChangedFiles, bool alwaysWriteHash, bool neverWriteHash)
    {
        Nui32 fileLength;
    //	CSHA1 sha1;
        FILE *fp;
        char fullPath[512];
        Nui32 i;
    //	char *fileData;

        for (i=0; i < fileList.Size(); i++)
        {
            strcpy(fullPath, applicationDirectory);
            FixEndingSlash(fullPath);
            strcat(fullPath,fileList[i].filename);
            fp=fopen(fullPath, "rb");
            if (fp==0)
            {
                missingOrChangedFiles->AddFile(fileList[i].filename, fileList[i].fullPathToFile, 0, 0, 0, FileListNodeContext(0,0,0,0), false);
            }
            else
            {
                fseek(fp, 0, SEEK_END);
                fileLength = ftell(fp);
                fseek(fp, 0, SEEK_SET);

                if (fileLength != fileList[i].fileLengthBytes && alwaysWriteHash==false)
                {
                    missingOrChangedFiles->AddFile(fileList[i].filename, fileList[i].fullPathToFile, 0, 0, fileLength, FileListNodeContext(0,0,0,0), false);
                }
                else
                {

    //				fileData= (char*) N_alloc(fileLength);
    //				fread(fileData, fileLength, 1, fp);

    //				sha1.Reset();
    //				sha1.update( ( Nui8* ) fileData, fileLength );
    //				sha1.Final();

    //				N_free(fileData);

                    Nui32 hash = FastHash(fp);
                    if(NetSerializer::isFlipData())
                        Serializer::mirror((Nui8*) &hash, sizeof(hash));

                    //if (fileLength != fileList[i].fileLength || memcmp( sha1.GetHash(), fileList[i].mData, HASH_LENGTH)!=0)
                    if (fileLength != fileList[i].fileLengthBytes || memcmp( &hash, fileList[i].mData, HASH_LENGTH)!=0)
                    {
                        if (neverWriteHash==false)
                        //	missingOrChangedFiles->AddFile((const char*)fileList[i].filename, (const char*)sha1.GetHash(), HASH_LENGTH, fileLength, 0);
                            missingOrChangedFiles->AddFile((const char*)fileList[i].filename, (const char*)fileList[i].fullPathToFile, (const char *) &hash, HASH_LENGTH, fileLength, FileListNodeContext(0,0,0,0), false);
                        else
                            missingOrChangedFiles->AddFile((const char*)fileList[i].filename, (const char*)fileList[i].fullPathToFile, 0, 0, fileLength, FileListNodeContext(0,0,0,0), false);
                    }
                }
                fclose(fp);
            }
        }
    }
    //------------------------------------------------------------------------
    void FileList::PopulateDataFromDisk(const char *applicationDirectory, bool writeFileData, bool writeFileHash, bool removeUnknownFiles)
    {
        FILE * fp;
        char fullPath[512];
        Nui32 i;
        i=0;
        while (i < fileList.Size())
        {
            N_free(fileList[i].mData);
            strcpy(fullPath, applicationDirectory);
            FixEndingSlash(fullPath);
            strcat(fullPath,fileList[i].filename.C_String());
            fp=fopen(fullPath, "rb");
            if (fp)
            {
                if (writeFileHash || writeFileData)
                {
                    fseek(fp, 0, SEEK_END);
                    fileList[i].fileLengthBytes = ftell(fp);
                    fseek(fp, 0, SEEK_SET);
                    if (writeFileHash)
                    {
                        if (writeFileData)
                        {
                            // Hash + data so offset the data by HASH_LENGTH
                            fileList[i].mData=(char*) N_alloc(fileList[i].fileLengthBytes+HASH_LENGTH);
                            N_assert(fileList[i].mData);
                            fread(fileList[i].mData+HASH_LENGTH, fileList[i].fileLengthBytes, 1, fp);
    //						sha1.Reset();
    //						sha1.update((Nui8*)fileList[i].mData+HASH_LENGTH, fileList[i].fileLength);
    //						sha1.Final();
                            Nui32 hash = FastHash(fileList[i].mData+HASH_LENGTH, fileList[i].fileLengthBytes);
                            if(NetSerializer::isFlipData())
                                Serializer::mirror((Nui8*) &hash, sizeof(hash));
    //						memcpy(fileList[i].mData, sha1.GetHash(), HASH_LENGTH);
                            memcpy(fileList[i].mData, &hash, HASH_LENGTH);
                        }
                        else
                        {
                            // Hash only
                            fileList[i].dataLengthBytes=HASH_LENGTH;
                            if (fileList[i].fileLengthBytes < HASH_LENGTH)
                                fileList[i].mData=(char*) N_alloc(HASH_LENGTH);
                            else
                                fileList[i].mData=(char*) N_alloc(fileList[i].fileLengthBytes);
                            N_assert(fileList[i].mData);
                            fread(fileList[i].mData, fileList[i].fileLengthBytes, 1, fp);
                    //		sha1.Reset();
                    //		sha1.update((Nui8*)fileList[i].mData, fileList[i].fileLength);
                    //		sha1.Final();
                            Nui32 hash = FastHash(fileList[i].mData, fileList[i].fileLengthBytes);
                            if(NetSerializer::isFlipData())
                                Serializer::mirror((Nui8*) &hash, sizeof(hash));
                            // memcpy(fileList[i].mData, sha1.GetHash(), HASH_LENGTH);
                            memcpy(fileList[i].mData, &hash, HASH_LENGTH);
                        }
                    }
                    else
                    {
                        // Data only
                        fileList[i].dataLengthBytes=fileList[i].fileLengthBytes;
                        fileList[i].mData=(char*) N_alloc(fileList[i].fileLengthBytes);
                        N_assert(fileList[i].mData);
                        fread(fileList[i].mData, fileList[i].fileLengthBytes, 1, fp);
                    }

                    fclose(fp);
                    i++;
                }
                else
                {
                    fileList[i].mData=0;
                    fileList[i].dataLengthBytes=0;
                }
            }
            else
            {
                if (removeUnknownFiles)
                {
                    fileList.RemoveAtIndex(i);
                }
                else
                    i++;
            }
        }
    }
    //------------------------------------------------------------------------
    void FileList::FlagFilesAsReferences(void)
    {
        for (Nui32 i=0; i < fileList.Size(); i++)
        {
            fileList[i].isAReference=true;
            fileList[i].dataLengthBytes=fileList[i].fileLengthBytes;
        }
    }
    //------------------------------------------------------------------------
    void FileList::WriteDataToDisk(const char *applicationDirectory)
    {
        char fullPath[512];
        Nui32 i,j;

        for (i=0; i < fileList.Size(); i++)
        {
            strcpy(fullPath, applicationDirectory);
            FixEndingSlash(fullPath);
            strcat(fullPath,fileList[i].filename.C_String());

            // Security - Don't allow .. in the filename anywhere so you can't write outside of the root directory
            for (j=1; j < fileList[i].filename.GetLength(); j++)
            {
                if (fileList[i].filename[j]=='.' && fileList[i].filename[j-1]=='.')
                {
    #ifdef _DEBUG
                    N_assert(0);
    #endif
                    // Just cancel the write entirely
                    return;
                }
            }

            FileSystem::wirteFile(fullPath, fileList[i].mData, (Nui32) fileList[i].dataLengthBytes);
        }
    }
    //------------------------------------------------------------------------
    #ifdef _MSC_VER
    #pragma warning( disable : 4996 ) // unlink declared deprecated by Microsoft in order to make it harder to be cross platform.  I don't agree it's deprecated.
    #endif
    void FileList::DeleteFiles(const char *applicationDirectory)
    {



        char fullPath[512];
        Nui32 i,j;

        for (i=0; i < fileList.Size(); i++)
        {
            // The filename should not have .. in the path - if it does ignore it
            for (j=1; j < fileList[i].filename.GetLength(); j++)
            {
                if (fileList[i].filename[j]=='.' && fileList[i].filename[j-1]=='.')
                {
    #ifdef _DEBUG
                    N_assert(0);
    #endif
                    // Just cancel the deletion entirely
                    return;
                }
            }

            strcpy(fullPath, applicationDirectory);
            FixEndingSlash(fullPath);
            strcat(fullPath, fileList[i].filename.C_String());

            // Do not rename to _unlink as linux uses unlink
    #if N_PLAT == N_PLAT_WIN8_PHONE || N_PLAT == N_PLAT_WINRT
            int result = _unlink(fullPath);
    #else
            int result = unlink(fullPath);
    #endif
            if (result!=0)
            {
                N_printf("FileList::DeleteFiles: unlink (%s) failed.\n", fullPath);
            }
        }

    }
    //------------------------------------------------------------------------
    void FileList::AddCallback(FileListProgress *cb)
    {
        if (cb==0)
            return;

        if ((Nui32) fileListProgressCallbacks.GetIndexOf(cb)==(Nui32)-1)
            fileListProgressCallbacks.Push(cb);
    }
    //------------------------------------------------------------------------
    void FileList::RemoveCallback(FileListProgress *cb)
    {
        Nui32 idx = fileListProgressCallbacks.GetIndexOf(cb);
        if (idx!=(Nui32) -1)
            fileListProgressCallbacks.RemoveAtIndex(idx);
    }
    //------------------------------------------------------------------------
    void FileList::ClearCallbacks(void)
    {
        fileListProgressCallbacks.Clear(true);
    }
    //------------------------------------------------------------------------
    void FileList::GetCallbacks(DataStructures::List<FileListProgress *> & callbacks)
    {
        callbacks = fileListProgressCallbacks;
    }
    //------------------------------------------------------------------------
    bool FileList::FixEndingSlash(char * str)
    {
    #if N_PLAT == N_PLAT_WIN32
        if (str[strlen(str)-1]!='/' && str[strlen(str)-1]!='\\')
        {
            strcat(str, "\\"); // Only \ works with system commands, used by AutopatcherClient
            return true;
        }
    #else
        if (str[strlen(str)-1]!='\\' && str[strlen(str)-1]!='/')
        {
            strcat(str, "/"); // Only / works with Linux
            return true;
        }
    #endif

        return false;
    }
    //------------------------------------------------------------------------
}
}
#endif