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
#ifndef gsgs_StringUtil_h_
#define gsgs_StringUtil_h_

#include "gsgsPreInclude.h"

#if wxVERSION_NUMBER < 3100
#include <functional>
namespace std
{
    template <>
    struct hash<wxString>
    {
        std::size_t operator()(const wxString & s) const { return hash<std::wstring>{}(s.ToStdWstring()); }
    };
}
#endif

namespace gsgs
{
    class _gsgsAPI StringHistory
    {
    public:
        StringHistory();
        virtual ~StringHistory();

        /**
         * @brief can we move to the next item in the history?
         */
        bool CanNext();
        /**
         * @brief can we move to the previous item in the history?
         */
        bool CanPrev();

        /**
         * @brief initialize the history strings
         */
        void SetStrings ( const wxArrayString& strings ) {
            m_strings = strings;
            m_index = 0;
        }

        /**
         * @brief return the current item
         */
        bool Current( wxString &str );
        /**
         * @brief return the next item in the history
         * @param str [output]
         * @return true on success, false otherwise
         */
        bool Next( wxString &str );
        /**
         * @brief return the previous item in the history
         * @param str [output]
         * @return true on success, false otherwise
         */
        bool Previous( wxString &str );
    private:
        wxArrayString m_strings;
        int           m_index;
    };

    class _gsgsAPI XORString
    {
    public:
        XORString(const wxString &value);
        XORString();
        virtual ~XORString();
        
        wxString Decrypt(const wxChar byte = 's') const;
        wxString Encrypt(const wxChar byte = 's') const;
    protected:
        wxString toHexString(const wxString &value) const;
        wxString fromHexString(const wxString& hexString) const;
        wxString XOR(const wxString& str, const wxChar KEY) const;
    private:
        wxString m_value;
    };

    class _gsgsAPI StringUtils
    {
    public:
        /**
         * @brief remove from str string from offset 'from' with len
         */
        static void Remove(std::string& str, size_t from, size_t len);

        /**
         * @brief return true if str starts with what
         */
        static bool StartsWith(const std::string& str, const std::string& what);

        /**
         * @brief return true if str ends with what
         */
        static bool EndsWith(const std::string& str, const std::string& what);

        /**
         * @brief
         */
        static void Trim(std::string& str, bool fromRight = true);

        /**
         * @brief remove last count bytes
         */
        static void RemoveLast(std::string& str, size_t count);

        /**
         * @brief convert string into std::string
         */
        static std::string ToStdString(const wxString& str);
        
        /**
         * @brief remove terminal colours from buffer
         */
        static void StripTerminalColouring(const std::string& buffer, std::string& modbuffer);
        /**
         * @brief wxString version.
         * @param buffer
         * @param modbuffer
         */
        static void StripTerminalColouring(const wxString& buffer, wxString& modbuffer);

        static void MergeArrays(wxArrayString & out, const wxArrayString & arr1, const wxArrayString & arr2);

        static void MergeStringMaps(gsgsStringMap & out, const gsgsStringMap & map1, const gsgsStringMap & map2);

        static int wxStringToInt(const wxString& str, int defval, int min = -1, int max = -1);

        static wxString wxIntToString(int val);

        static unsigned int clUTF8Length(const wchar_t * uptr, unsigned int tlen);
    public:
        static bool Search(const wchar_t* input, int startOffset, const wchar_t* find_what, size_t flags, int& pos, int& matchLen);
        // overloaded method because of that ReplaceAll methods works on wxString and needs results in chars and other
        // methods
        // using selection needs results in bytes
        static bool Search(const wchar_t* input, int startOffset, const wchar_t* find_what, size_t flags, int& pos, int& matchLen, int& posInChars, int& matchLenInChars);
    protected:
        static wxString GetString(const wxString& input, int from, bool search_up);
        static bool DoRESearch(const wxString& input, int startOffset, const wxString& find_what, size_t flags, int& pos, int& matchLen);
        static bool DoWildcardSearch(const wxString& input, int startOffset, const wxString& find_what, size_t flags, int& pos, int& matchLen);
        static bool DoSimpleSearch(const wchar_t* pinput, int startOffset, const wchar_t* find_what, size_t flags, int& pos, int& matchLen);
    };
}
#endif // STRINGUTILS_H
