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

#include "DataCompressor.h"
#include "DS_HuffmanEncodingTree.h"
#include <string.h> // Use string.h rather than memory.h for a console

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    void DataCompressor::Compress(Nui8 * userData, Nui32 sizeInBytes, NetSerializer * output)
    {
        // Don't use this for small files as you will just make them bigger!
        N_assert(sizeInBytes > 2048);

        Nui32 frequencyTable[256];
        NCount i;
        memset(frequencyTable, 0, 256 * sizeof(Nui32));
        for(i = 0; i < sizeInBytes; ++i)
            ++frequencyTable[userData[i]];

        HuffmanEncodingTree tree;
        NBitCount writeBOffset1, writeBOffset2, bitsBUsed1, bitsBUsed2;
        
        tree.GenerateFromFrequencyTable(frequencyTable);
        output->writeCompress(sizeInBytes);
        for(i = 0; i < 256; ++i)
            output->writeCompress(frequencyTable[i]);
        output->writeAlign();

        writeBOffset1 = output->getWriteOffset();
        output->write((Nui32)0);	// Dummy value
        bitsBUsed1 = output->getWriteOffset();
        tree.EncodeArray(userData, sizeInBytes, output);
        bitsBUsed2 = output->getWriteOffset();
        writeBOffset2 = output->getWriteOffset();
        output->seekWrite(writeBOffset1);
        output->write(bitsBUsed2 - bitsBUsed1); // Go back and write how many bits were used for the encoding
        output->seekWrite(writeBOffset2);
    }
    //------------------------------------------------------------------------
    Nui32 DataCompressor::DecompressAndAllocate(NetSerializer * input, Nui8 ** output)
    {
        HuffmanEncodingTree tree;
        Nui32 bitsUsed, destinationSizeInBytes;
        Nui32 decompressedBytes;
        Nui32 frequencyTable[256];
        NCount i;

        input->readCompress(destinationSizeInBytes);
        for (i=0; i < 256; ++i)
            input->readCompress(frequencyTable[i]);
        input->readAlign();
        if (input->read(bitsUsed) == false)
        {
            // read error
    #ifdef _DEBUG
            N_assert(0);
    #endif
            return 0;
        }
        *output = (Nui8 *)N_alloc(destinationSizeInBytes);
        tree.GenerateFromFrequencyTable(frequencyTable);
        decompressedBytes = tree.DecodeArray(input, bitsUsed, destinationSizeInBytes, *output);
        N_assert(decompressedBytes == destinationSizeInBytes);
        return destinationSizeInBytes;
    }
    //------------------------------------------------------------------------
}
}