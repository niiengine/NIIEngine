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

#ifndef _HUFFMAN_ENCODING_TREE_FACTORY_
#define _HUFFMAN_ENCODING_TREE_FACTORY_

#include "NiiNetPreInclude.h"

namespace NII
{
namespace NII_NET
{

    /// \brief Creates instances of the class HuffmanEncodingTree
    /// \details This class takes a frequency table and given that frequence table, will generate an instance of HuffmanEncodingTree
    class HuffmanEncodingTreeFactory
    {
    public:
        /// Default constructor
        HuffmanEncodingTreeFactory();

        /// \brief Reset the frequency table.
        /// \details You don't need to call this unless you want to reuse the class for a new tree
        void Reset();

        /// \brief Pass an array of bytes to this to add those elements to the frequency table.
        /// \param[in] array the data to insert into the frequency table
        /// \param[in] size the size of the data to insert
        void AddToFrequencyTable(Nui8 *array, int size);

        /// \brief Copies the frequency table to the array passed. Retrieve the frequency table.
        /// \param[in] _frequency The frequency table used currently
        void GetFrequencyTable(Nui32 _frequency[ 256 ]);

        /// \brief Returns the frequency table as a pointer.
        /// \return the address of the frenquency table
        Nui32 * GetFrequencyTable();

        /// \brief Generate a HuffmanEncodingTree.
        /// \details You can also use GetFrequencyTable and GenerateFromFrequencyTable in the tree itself
        /// \return The generated instance of HuffmanEncodingTree
        HuffmanEncodingTree * GenerateTree();
    private:
        /// Frequency table
        Nui32 frequency[ 256 ];
    };
}
}
#endif