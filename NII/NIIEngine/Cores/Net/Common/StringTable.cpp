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

#include "StringTable.h"
#include "NiiNetSerializer.h"
#include "DS_HuffmanEncodingTree.h"
#include "RakString.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    StringCompressor* StringCompressor::instance = 0;
    int StringCompressor::referenceCount = 0;
    //------------------------------------------------------------------------
    void StringCompressor::AddReference(void)
    {
        if (++referenceCount==1)
        {
            instance = N_new StringCompressor();
        }
    }
    //------------------------------------------------------------------------
    void StringCompressor::RemoveReference(void)
    {
        N_assert(referenceCount > 0, "error");

        if (referenceCount > 0)
        {
            if (--referenceCount==0)
            {
                N_delete instance;
                instance=0;
            }
        }
    }
    //------------------------------------------------------------------------
    StringCompressor* StringCompressor::Instance(void)
    {
        return instance;
    }
    //------------------------------------------------------------------------
    Nui32 englishCharacterFrequencies[ 256 ] =
    {
        0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            722,
            0,
            0,
            2,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            11084,
            58,
            63,
            1,
            0,
            31,
            0,
            317,
            64,
            64,
            44,
            0,
            695,
            62,
            980,
            266,
            69,
            67,
            56,
            7,
            73,
            3,
            14,
            2,
            69,
            1,
            167,
            9,
            1,
            2,
            25,
            94,
            0,
            195,
            139,
            34,
            96,
            48,
            103,
            56,
            125,
            653,
            21,
            5,
            23,
            64,
            85,
            44,
            34,
            7,
            92,
            76,
            147,
            12,
            14,
            57,
            15,
            39,
            15,
            1,
            1,
            1,
            2,
            3,
            0,
            3611,
            845,
            1077,
            1884,
            5870,
            841,
            1057,
            2501,
            3212,
            164,
            531,
            2019,
            1330,
            3056,
            4037,
            848,
            47,
            2586,
            2919,
            4771,
            1707,
            535,
            1106,
            152,
            1243,
            100,
            0,
            2,
            0,
            10,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
    };
    //------------------------------------------------------------------------
    StringCompressor::StringCompressor()
    {
        DataStructures::Map<int, HuffmanEncodingTree *>::IMPLEMENT_DEFAULT_COMPARISON();

        // Make a default tree immediately, since this is used for RPC possibly from multiple threads at the same time
        HuffmanEncodingTree *huffmanEncodingTree = N_new HuffmanEncodingTree();
        huffmanEncodingTree->GenerateFromFrequencyTable( englishCharacterFrequencies );

        huffmanEncodingTrees.Set(0, huffmanEncodingTree);
    }
    //------------------------------------------------------------------------
    void StringCompressor::GenerateTreeFromStrings( Nui8 *input, unsigned inputLength, Nui8 languageId )
    {
        HuffmanEncodingTree *huffmanEncodingTree;
        if (huffmanEncodingTrees.Has(languageId))
        {
            huffmanEncodingTree = huffmanEncodingTrees.Get(languageId);
            N_delete huffmanEncodingTree;
        }

        unsigned index;
        Nui32 frequencyTable[ 256 ];

        if ( inputLength == 0 )
            return ;

        // Zero out the frequency table
        memset( frequencyTable, 0, sizeof( frequencyTable ) );

        // Generate the frequency table from the strings
        for ( index = 0; index < inputLength; index++ )
            frequencyTable[ input[ index ] ] ++;

        // Build the tree
        huffmanEncodingTree = N_new HuffmanEncodingTree();
        huffmanEncodingTree->GenerateFromFrequencyTable( frequencyTable );
        huffmanEncodingTrees.Set(languageId, huffmanEncodingTree);
    }
    //------------------------------------------------------------------------
    StringCompressor::~StringCompressor()
    {
        for (unsigned i=0; i < huffmanEncodingTrees.Size(); i++)
            N_delete huffmanEncodingTrees[i];
    }
    //------------------------------------------------------------------------
    void StringCompressor::EncodeString(const char * input, int maxCharsToWrite,
        NetSerializer * output, Nui8 languageId)
    {
        HuffmanEncodingTree * huffmanEncodingTree;
        if (huffmanEncodingTrees.Has(languageId) == false)
            return;
        huffmanEncodingTree = huffmanEncodingTrees.Get(languageId);

        if (input == 0)
        {
            output->writeCompress((Nui32)0);
            return ;
        }

        NetSerializer encodedata;

        Nui32 stringBitLength;

        int charsToWrite;

        if (maxCharsToWrite<=0 || (int)strlen(input) < maxCharsToWrite)
            charsToWrite = (int)strlen(input);
        else
            charsToWrite = maxCharsToWrite - 1;

        huffmanEncodingTree->EncodeArray((Nui8 *)input, charsToWrite, &encodedata);

        stringBitLength = (Nui32) encodedata.getWriteOffset();

        output->writeCompress(stringBitLength);

        output->writeBit(encodedata.getData(), stringBitLength);
    }
    //------------------------------------------------------------------------
    bool StringCompressor::DecodeString(char * output, int maxCharsToWrite,
        NetSerializer * input, Nui8 languageId)
    {
        HuffmanEncodingTree *huffmanEncodingTree;
        if (huffmanEncodingTrees.Has(languageId) == false)
            return false;
        if (maxCharsToWrite <= 0)
            return false;
        huffmanEncodingTree = huffmanEncodingTrees.Get(languageId);

        Nui32 stringBitLength;
        int bytesInStream;

        output[0] = 0;

        if (input->readCompress(stringBitLength) == false)
            return false;

        if ((unsigned) input->getRemainCount() < stringBitLength)
            return false;

        bytesInStream = huffmanEncodingTree->DecodeArray(input, stringBitLength, maxCharsToWrite, ( Nui8* ) output );

        if (bytesInStream < maxCharsToWrite)
            output[bytesInStream] = 0;
        else
            output[maxCharsToWrite - 1] = 0;

        return true;
    }
    //------------------------------------------------------------------------
    #ifdef _CSTRING_COMPRESSOR
    void StringCompressor::EncodeString(const CString & input, int maxCharsToWrite,
        NetSerializer * output)
    {
        LPTSTR p = input;
        EncodeString(p, maxCharsToWrite*sizeof(TCHAR), output, languageID);
    }
    //------------------------------------------------------------------------
    bool StringCompressor::DecodeString(CString & output, int maxCharsToWrite,
        NetSerializer * input, Nui8 languageId)
    {
        LPSTR p = output.GetBuffer(maxCharsToWrite*sizeof(TCHAR));
        DecodeString(p,maxCharsToWrite*sizeof(TCHAR), input, languageID);
        output.ReleaseBuffer(0)

    }
    #endif
    //------------------------------------------------------------------------
    #ifdef _STD_STRING_COMPRESSOR
    void StringCompressor::EncodeString(const std::string &input, int maxCharsToWrite,
        NetSerializer * output, Nui8 languageId)
    {
        EncodeString(input.c_str(), maxCharsToWrite, output, languageId);
    }
    //------------------------------------------------------------------------
    bool StringCompressor::DecodeString(std::string *output, int maxCharsToWrite,
        NetSerializer * input, Nui8 languageId)
    {
        if (maxCharsToWrite <= 0)
        {
            output->clear();
            return true;
        }

        char * destinationBlock;
        bool out;
    #if N_AllocaStackSize != 0
        if (maxCharsToWrite < N_AllocaStackSize)
        {
            destinationBlock = (char*) alloca(maxCharsToWrite);
            out=DecodeString(destinationBlock, maxCharsToWrite, input, languageId);
            *output=destinationBlock;
        }
        else
    #endif
        {
            destinationBlock = (char*) N_alloc(maxCharsToWrite);
            out=DecodeString(destinationBlock, maxCharsToWrite, input, languageId);
            *output=destinationBlock;
            N_free(destinationBlock);
        }
        return out;
    }
    #endif
    //------------------------------------------------------------------------
    void StringCompressor::EncodeString(const RakString * input, int maxCharsToWrite,
        NetSerializer * output, Nui8 languageId)
    {
        EncodeString(input->C_String(), maxCharsToWrite, output, languageId);
    }
    //------------------------------------------------------------------------
    bool StringCompressor::DecodeString(RakString * output, int maxCharsToWrite,
        NetSerializer * input, Nui8 languageId)
    {
        if(maxCharsToWrite <= 0)
        {
            output->Clear();
            return true;
        }

        char * destinationBlock;
        bool out;
    #if N_AllocaStackSize !=0
        if (maxCharsToWrite < N_AllocaStackSize)
        {
            destinationBlock = (char *)alloca(maxCharsToWrite);
            out=DecodeString(destinationBlock, maxCharsToWrite, input, languageId);
            *output=destinationBlock;
        }
        else
    #endif
        {
            destinationBlock = (char *)N_alloc(maxCharsToWrite);
            out=DecodeString(destinationBlock, maxCharsToWrite, input, languageId);
            *output=destinationBlock;
            N_free(destinationBlock);
        }
        return out;
    }
    //------------------------------------------------------------------------
    
    //------------------------------------------------------------------------
    StringTable * StringTable::instance = 0;
    int StringTable::referenceCount = 0;
    //------------------------------------------------------------------------
    int NII::StrAndBoolComp(char * const & key, const StrAndBool & data)
    {
        return strcmp(key,(const char *)data.str);
    }
    //------------------------------------------------------------------------
    StringTable::StringTable()
    {

    }
    //------------------------------------------------------------------------
    StringTable::~StringTable()
    {
        NCount i;
        for (i = 0; i < orderedStringList.Size(); ++i)
        {
            if (orderedStringList[i].b)
                N_free(orderedStringList[i].str);
        }
    }
    //------------------------------------------------------------------------
    void StringTable::AddReference(void)
    {
        if (++referenceCount==1)
        {
            instance = N_new StringTable();
        }
    }
    //------------------------------------------------------------------------
    void StringTable::RemoveReference(void)
    {
        N_assert(referenceCount > 0);

        if (referenceCount > 0)
        {
            if (--referenceCount==0)
            {
                N_delete instance;
                instance=0;
            }
        }
    }
    //------------------------------------------------------------------------
    StringTable * StringTable::Instance(void)
    {
        return instance;
    }
    //------------------------------------------------------------------------
    void StringTable::AddString(const char * str, bool copyString)
    {
        StrAndBool sab;
        sab.b = copyString;
        if (copyString)
        {
            sab.str = (char *)N_alloc(strlen(str) + 1);
            strcpy(sab.str, str);
        }
        else
        {
            sab.str=(char *)str;
        }

        // If it asserts inside here you are adding duplicate strings.
        orderedStringList.Insert(sab.str,sab, true);

        // If this N_assert hits you need to increase the range of StringTableType
        N_assert(orderedStringList.Size() < (StringTableType)-1);
    }
    //------------------------------------------------------------------------
    void StringTable::EncodeString(const char * input, int maxCharsToWrite, NetSerializer * output)
    {
        Nui32 index;
        bool objectExists;
        // This is fast because the list is kept ordered.
        index=orderedStringList.GetIndexFromKey((char *)input, &objectExists);
        if (objectExists)
        {
            output->write1();
            output->write((StringTableType)index);
        }
        else
        {
            LogStringNotFound(input);
            output->write0();
            StringCompressor::Instance()->EncodeString(input, maxCharsToWrite, output);
        }
    }
    //------------------------------------------------------------------------
    bool StringTable::DecodeString(char * output, int maxCharsToWrite, NetSerializer * input)
    {
        bool hasIndex = false;
        N_assert(maxCharsToWrite > 0);

        if (maxCharsToWrite == 0)
            return false;
        if (!input->read(hasIndex))
            return false;
        if (hasIndex == false)
        {
            StringCompressor::Instance()->DecodeString(output, maxCharsToWrite, input);
        }
        else
        {
            StringTableType index;
            if (!input->read(index))
                return false;
            if (index >= orderedStringList.Size())
            {
    #ifdef _DEBUG
                // Critical error - got a string index out of range, which means AddString was called more times on the remote system than on this system.
                // All systems must call AddString the same number of types, with the same strings in the same order.
                N_assert(0);
    #endif
                return false;
            }

            strncpy(output, orderedStringList[index].str, maxCharsToWrite);
            output[maxCharsToWrite - 1] = 0;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void StringTable::LogStringNotFound(const char * strName)
    {
        (void) strName;
    #ifdef _DEBUG
        N_printf("Efficiency Warning! Unregistered String %s sent to StringTable.\n", strName);
    #endif
    }
    //------------------------------------------------------------------------
}
}