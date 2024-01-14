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

#ifndef __HUFFMAN_ENCODING_TREE
#define __HUFFMAN_ENCODING_TREE

#include "NiiNetPreInclude.h"
#include "DS_HuffmanEncodingTreeNode.h"
#include "NiiNetSerializer.h"
#include "DS_LinkedList.h"

namespace NII
{
namespace NII_NET
{
    /// This generates special cases of the huffman encoding tree using 8 bit keys with the additional condition that unused combinations of 8 bits are treated as a frequency of 1
    class _EngineAPI HuffmanEncodingTree
    {
    public:
        HuffmanEncodingTree();
        ~HuffmanEncodingTree();

        /// \brief Pass an array of bytes to array and a preallocated NetSerializer to receive the output.
        /// \param [in] input Array of bytes to encode
        /// \param [in] sizeInBytes size of \a input
        /// \param [out] output The bitstream to write to
        void EncodeArray(Nui8 *input, size_t sizeInBytes, NetSerializer * output);

        // \brief Decodes an array encoded by EncodeArray().
        Nui32 DecodeArray(NetSerializer * input, NBitCount sizeInBits, size_t maxCharsToWrite, Nui8 *output );
        void DecodeArray(Nui8 *input, NBitCount sizeInBits, NetSerializer * output);

        /// \brief Given a frequency table of 256 elements, all with a frequency of 1 or more, generate the tree.
        void GenerateFromFrequencyTable(Nui32 frequencyTable[256]);

        /// \brief Free the memory used by the tree.
        void FreeMemory(void);
    private:
        /// The root node of the tree
        HuffmanEncodingTreeNode *root;

        /// Used to hold bit encoding for one character
        struct CharacterEncoding
        {
            Nui8* encoding;
            Nui16 bitLength;
        };

        CharacterEncoding encodingTable[256];

        void InsertNodeIntoSortedList(HuffmanEncodingTreeNode * node, DataStructures::LinkedList<HuffmanEncodingTreeNode *> *huffmanEncodingTreeNodeList) const;
    };
}
}
#endif