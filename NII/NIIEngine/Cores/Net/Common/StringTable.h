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

#ifndef __STRING_TABLE_H
#define __STRING_TABLE_H

#include "NiiNetPreInclude.h"
#include "DS_OrderedList.h"
#include "DS_Map.h"

#ifdef _STD_STRING_COMPRESSOR
#include <string>
#endif

/// StringTableType should be the smallest type possible, or else it defeats the purpose of the StringTable class, which is to save bandwidth.
typedef Nui8 StringTableType;

/// The string plus a bool telling us if this string was copied or not.
struct StrAndBool
{
    char *str;
    bool b;
};

namespace NII
{
namespace NII_NET
{
    /// \brief Writes and reads strings to and from bitstreams.
    ///
    /// Only works with ASCII strings.  The default compression is for English.
    /// You can call GenerateTreeFromStrings to compress and decompress other languages efficiently as well.
    class _EngineAPI StringCompressor
    {
    public:
        // Destructor
        ~StringCompressor();

        /// static function because only static functions can access static members
        /// The NetworkSys constructor adds a reference to this class, so don't call this until an instance of NetworkSys exists, or unless you call AddReference yourself.
        /// \return the unique instance of the StringCompressor
        static StringCompressor * Instance(void);

        /// Given an array of strings, such as a chat log, generate the optimal encoding tree for it.
        /// This function is optional and if it is not called a default tree will be used instead.
        /// \param[in] in An array of bytes which should point to text.
        /// \param[in] inputLength Length of \a in
        /// \param[in] languageID An identifier for the language / string table to generate the tree for.  English is automatically created with ID 0 in the constructor.
        void GenerateTreeFromStrings(Nui8 * in, Nui32 inputLength, Nui8 languageId);

        /// Writes in to out, compressed.  Takes care of the null terminator for you.
        /// \param[in] in Pointer to an ASCII string
        /// \param[in] maxCharsToWrite The max number of bytes to write of \a in.  Use 0 to mean no limit.
        /// \param[out] out The bitstream to write the compressed string to
        /// \param[in] languageID Which language to use
        void EncodeString(const char * in, int maxCharsToWrite, NetSerializer * out,
            Nui8 languageId = 0);

        /// Writes in to out, uncompressed.  Takes care of the null terminator for you.
        /// \param[out] out A block of bytes to receive the out
        /// \param[in] maxCharsToWrite Size, in bytes, of \a out .  A NULL terminator will always be appended to the out string.  If the maxCharsToWrite is not large enough, the string will be truncated.
        /// \param[in] in The bitstream containing the compressed string
        /// \param[in] languageID Which language to use
        bool DecodeString(char * out, int maxCharsToWrite, NetSerializer *in,
            Nui8 languageId = 0);

    #ifdef _CSTRING_COMPRESSOR
        void EncodeString(const CString & in, int maxCharsToWrite, NetSerializer * out,
            Nui8 languageId = 0);
        bool DecodeString(CString & out, int maxCharsToWrite, NetSerializer * in,
            Nui8 languageId = 0);
    #endif

    #ifdef _STD_STRING_COMPRESSOR
        void EncodeString(const std::string & in, int maxCharsToWrite, NetSerializer * out,
            Nui8 languageId = 0);
        bool DecodeString(std::string * out, int maxCharsToWrite, NetSerializer * in,
            Nui8 languageId = 0);
    #endif

        void EncodeString(const RakString * in, int maxCharsToWrite, NetSerializer * out, Nui8 languageId = 0);
        bool DecodeString(RakString * out, int maxCharsToWrite, NetSerializer * in,  Nui8 languageId = 0);

        /// Used so I can allocate and deallocate this singleton at runtime
        static void AddReference();

        /// Used so I can allocate and deallocate this singleton at runtime
        static void RemoveReference();

        StringCompressor();
    private:
        /// Singleton instance
        static StringCompressor * instance;

        /// Pointer to the huffman encoding trees.
        DataStructures::Map<int, HuffmanEncodingTree *> huffmanEncodingTrees;

        static int referenceCount;
    };
    
    int _EngineAPI StrAndBoolComp(char * const & key, const StrAndBool & data);

    /// \details This is an even more efficient alternative to StringCompressor in that it writes a single byte from a lookup table and only does compression.<BR>
    /// if the string does not already exist in the table.<BR>
    /// All string tables must match on all systems - hence you must add all the strings in the same order on all systems.<BR>
    /// Furthermore, this must be done before sending packets that use this class, since the strings are ordered for fast lookup.  Adding after that time would mess up all the indices so don't do it.<BR>
    /// Don't use this class to write strings which were not previously registered with AddString, since you just waste bandwidth then.  Use StringCompressor instead.
    /// \brief Writes a string index, instead of the whole string
    class _EngineAPI StringTable
    {
    public:
        ~StringTable();

        /// static function because only static functions can access static members
        /// The NetworkSys constructor adds a reference to this class, so don't call this until an instance of NetworkSys exists, or unless you call AddReference yourself.
        /// \return the unique instance of the StringTable
        static StringTable * Instance(void);

        /// Add a string to the string table.
        /// \param[in] str The string to add to the string table
        /// \param[in] copyString true to make a copy of the passed string (takes more memory), false to not do so (if your string is in static memory).
        void AddString(const char * str, bool copyString);

        /// Writes input to output, compressed.  Takes care of the null terminator for you.
        /// Relies on the StringCompressor class, which is automatically reference counted in the constructor and destructor in NetworkSys.  You can call the reference counting functions yourself if you wish too.
        /// \param[in] input Pointer to an ASCII string
        /// \param[in] maxCharsToWrite The size of \a input
        /// \param[out] output The serializer to write the compressed string to
        void EncodeString(const char * input, int maxCharsToWrite, NetSerializer * output);

        /// Writes input to output, uncompressed.  Takes care of the null terminator for you.
        /// Relies on the StringCompressor class, which is automatically reference counted in the constructor and destructor in NetworkSys.  You can call the reference counting functions yourself if you wish too.
        /// \param[out] output A block of bytes to receive the output
        /// \param[in] maxCharsToWrite Size, in bytes, of \a output .  A NULL terminator will always be appended to the output string.  If the maxCharsToWrite is not large enough, the string will be truncated.
        /// \param[in] input The serializer containing the compressed string
        bool DecodeString(char * output, int maxCharsToWrite, NetSerializer * input);

        /// Used so I can allocate and deallocate this singleton at runtime
        static void AddReference();

        /// Used so I can allocate and deallocate this singleton at runtime
        static void RemoveReference();

        /// Private Constructor
        StringTable();
    protected:
        /// Called when you mess up and send a string using this class that was not registered with AddString
        /// \param[in] maxCharsToWrite Size, in bytes, of \a output .  A NULL terminator will always be appended to the output string.  If the maxCharsToWrite is not large enough, the string will be truncated.
        void LogStringNotFound(const char * strName);

        /// Singleton instance
        static StringTable *instance;
        static int referenceCount;

        DataStructures::OrderedList<char *, StrAndBool, StrAndBoolComp> orderedStringList;
    };
}
}
#endif