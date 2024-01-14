#include "gsgsStringUtil.h"
#include <wx/crt.h>
#include <wx/regex.h>
#include "gsgsSearchWorker.h"

namespace gsgs
{
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // StringHistory
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    StringHistory::StringHistory()
        : m_index ( 0 )
    {
    }
    //---------------------------------------------------------------------------
    StringHistory::~StringHistory()
    {
    }
    //---------------------------------------------------------------------------
    bool StringHistory::Current( wxString& str )
    {
        if ( m_strings.IsEmpty() || ( m_index < 0 || m_index >= ( int ) m_strings.GetCount() ) )
            return false;

        str = m_strings.Item( m_index );
        return true;
    }
    //---------------------------------------------------------------------------
    bool StringHistory::Next( wxString& str )
    {
        if ( CanNext() ) {
            ++m_index;
            str = m_strings.Item( m_index );
            return true;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool StringHistory::Previous( wxString& str )
    {
        if ( CanPrev() ) {
            --m_index;
            str = m_strings.Item( m_index );
            return true;
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool StringHistory::CanNext()
    {
        if ( m_strings.IsEmpty() ) return false;
        return ( m_index + 1 ) < ( int )m_strings.GetCount();
    }
    //---------------------------------------------------------------------------
    bool StringHistory::CanPrev()
    {
        if ( m_strings.IsEmpty() ) return false;
        return ( m_index - 1 ) >= 0;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // XORString
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    XORString::XORString()
    {
    }
    //---------------------------------------------------------------------------
    XORString::~XORString()
    {
    }
    //---------------------------------------------------------------------------
    wxString XORString::Decrypt(const wxChar byte) const
    {
        wxString dec = fromHexString(m_value);
        dec = XOR(dec, byte);
        return dec;
    }
    //---------------------------------------------------------------------------
    wxString XORString::Encrypt(const wxChar byte) const
    {
        wxString enc = XOR(m_value, byte);
        enc = toHexString(enc);
        return enc;
    }
    //---------------------------------------------------------------------------
    XORString::XORString(const wxString& value)
        : m_value(value)
    {
    }
    //---------------------------------------------------------------------------
    wxString XORString::toHexString(const wxString &value) const
    {
        wxString output;
        for(size_t i=0; i<value.length(); ++i) {
            wxChar buf[5] = {0, 0, 0, 0, 0};
            wxSprintf(buf, "%04X", (int)value[i]);
            output << buf;
        }
        return output;
    }
    //---------------------------------------------------------------------------
    wxString XORString::fromHexString(const wxString& hexString) const
    {
        wxString output;
        size_t cnt = hexString.length() / 4;
        for (size_t i = 0; cnt > i; ++i) {
            wxString dec = hexString.Mid(i*4, 4);
            int hexVal;
            wxSscanf(dec, "%X", &hexVal);
            output << (wxChar)hexVal;
        }
        return output;
    }
    //---------------------------------------------------------------------------
    wxString XORString::XOR(const wxString& str, const wxChar KEY) const
    {
        wxString output;
        for(size_t i=0; i<str.length(); ++i) {
            
            wxChar ch = str[i];
            wxChar x = (ch ^ KEY);
            output << x;
        }
        return output;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // StringUtils
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    int StringUtils::wxStringToInt(const wxString& str, int defval, int minval, int maxval)
    {
        long v;
        if (!str.ToLong(&v)) { return defval; }

        if (minval != -1 && v < minval) return defval;

        if (maxval != -1 && v > maxval) return defval;

        return v;
    }
    //---------------------------------------------------------------------------
    wxString StringUtils::wxIntToString(int val)
    {
        wxString s;
        s << val;
        return s;
    }
    //---------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // UTF8/16 conversions methods copied from wxScintilla
    ///////////////////////////////////////////////////////////////////////////////////////////////
    enum { SURROGATE_LEAD_FIRST = 0xD800 };
    enum { SURROGATE_TRAIL_FIRST = 0xDC00 };
    enum { SURROGATE_TRAIL_LAST = 0xDFFF };
    //---------------------------------------------------------------------------
    unsigned int StringUtils::clUTF8Length(const wchar_t* uptr, unsigned int tlen)
    {
        unsigned int len = 0;
        for (unsigned int i = 0; i < tlen && uptr[i];) {
            unsigned int uch = uptr[i];
            if (uch < 0x80) {
                len++;
            }
            else if (uch < 0x800) {
                len += 2;
            }
            else if ((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_TRAIL_LAST)) {
                len += 4;
                i++;
            }
            else {
                len += 3;
            }
            i++;
        }
        return len;
    }
    //---------------------------------------------------------------------------
    void StringUtils::Remove(std::string& str, size_t from, size_t len) { str.erase(from, len); }
    //---------------------------------------------------------------------------
    bool StringUtils::EndsWith(const std::string& str, const std::string& what)
    {
        if (str.length() >= what.length()) {
            return (0 == str.compare(str.length() - what.length(), what.length(), what));
        }
        else {
            return false;
        }
    }
    //---------------------------------------------------------------------------
    bool StringUtils::StartsWith(const std::string& str, const std::string& what)
    {
        if (str.length() >= what.length()) {
            return (0 == str.compare(0, what.length(), what));
        }
        else {
            return false;
        }
    }
    //---------------------------------------------------------------------------
    void StringUtils::Trim(std::string& str, bool fromRight)
    {
        static std::string trimString = " \t\r\n";
        if (fromRight) {
            str.erase(str.find_last_not_of(trimString) + 1);
        }
        else {
            str.erase(0, str.find_first_not_of(trimString));
        }
    }
    //---------------------------------------------------------------------------
    void StringUtils::RemoveLast(std::string& str, size_t count)
    {
        if (str.length() >= count) {
            str.erase(str.length() - count, count);
        }
    }
    //---------------------------------------------------------------------------
    std::string StringUtils::ToStdString(const wxString& str)
    {
        wxCharBuffer cb = str.ToAscii();
        const char* data = cb.data();
        if(!data) { data = str.mb_str(wxConvUTF8).data(); }
        if(!data) { data = str.To8BitData(); }

        std::string res;
        if(!data) { return res; }
        res = data;
        return res;
    }
    //---------------------------------------------------------------------------
    #define BUFF_STATE_NORMAL 0
    #define BUFF_STATE_IN_ESC 1
    #define BUFF_STATE_IN_OSC 2
    //---------------------------------------------------------------------------
    // see : https://en.wikipedia.org/wiki/ANSI_escape_code#Escape_sequences
    void StringUtils::StripTerminalColouring(const std::string& buffer, std::string& modbuffer)
    {
        modbuffer.reserve(buffer.length());
        short state = BUFF_STATE_NORMAL;
        for(const char& ch : buffer) {
            switch(state) {
            case BUFF_STATE_NORMAL:
                if(ch == 0x1B) { // found ESC char
                    state = BUFF_STATE_IN_ESC;
                } else {
                    modbuffer += ch;
                }
                break;
            case BUFF_STATE_IN_ESC:
                switch(ch) {
                case 'm':
                case 'K':
                case 'G':
                case 'J':
                case 'H':
                case 'X':
                case 'B':
                case 'C':
                case 'D':
                case 'd':
                    state = BUFF_STATE_NORMAL;
                    break;
                case ']':
                    // Operating System Command
                    state = BUFF_STATE_IN_OSC;
                    break;
                default:
                    break;
                }
                break;
            case BUFF_STATE_IN_OSC:
                if(ch == '\a') {
                    // bell, leave the current state
                    state = BUFF_STATE_NORMAL;
                }
                break;
            }
        }
        modbuffer.shrink_to_fit();
    }
    //---------------------------------------------------------------------------
    void StringUtils::StripTerminalColouring(const wxString& buffer, wxString& modbuffer)
    {
        std::string source = ToStdString(buffer);
        std::string output;
        StripTerminalColouring(source, output);
        if(!output.empty()) {
            modbuffer = wxString(output.c_str(), wxConvUTF8);
            if(modbuffer.IsEmpty()) { modbuffer = wxString::From8BitData(output.c_str()); }
        } else {
            modbuffer.Clear();
        }
    }
    //---------------------------------------------------------------------------
    static std::wstring Reverse(const std::wstring& str)
    {
        // reverse the string content
        std::wstring tmp;
        tmp.reserve(str.length());

        std::wstring::const_reverse_iterator riter = str.rbegin();
        for(; riter != str.rend(); riter++) {
            tmp += *riter;
        }
        return tmp;
    }
    //---------------------------------------------------------------------------
    void StringUtils::MergeArrays(wxArrayString & out, const wxArrayString & arr1, const wxArrayString & arr2)
    {
        wxArrayString sArr1, sArr2;
        //sArr1.insert(sArr1.end(), arr1.begin(), arr1.end());
        //sArr2.insert(sArr2.end(), arr2.begin(), arr2.end());
        sArr1 = arr1;
        sArr2 = arr2;
        // Sort the arrays
        std::sort(sArr1.begin(), sArr1.end());
        std::sort(sArr2.begin(), sArr2.end());

        //std::set_union(sArr1.begin(), sArr1.end(), sArr2.begin(), sArr2.end(), std::back_inserter(out));
        std::set_union(sArr1.begin(), sArr1.end(), sArr2.begin(), sArr2.end(), std::inserter(out, out.begin()));
    }
    //---------------------------------------------------------------------------
    void StringUtils::MergeStringMaps(gsgsStringMap & out, const gsgsStringMap& map1, const gsgsStringMap& map2)
    {
        out.insert(map1.begin(), map1.end());
        out.insert(map2.begin(), map2.end());
    }
    //---------------------------------------------------------------------------
    wxString StringUtils::GetString(const wxString& input, int from, bool search_up)
    {
        if(from < 0) { from = 0; }

        if(!search_up) {

            if(from >= (int)input.Len()) { return wxEmptyString; }
            return input.Mid((size_t)from);

        } else {
            if(from >= (int)input.Len()) { from = (int)input.Len(); }
            return input.Mid(0, (size_t)from);
        }
    }
    //---------------------------------------------------------------------------
    bool StringUtils::DoWildcardSearch(const wxString& input, int startOffset, const wxString& find_what,
                                              size_t flags, int& pos, int& matchLen)
    {
        // Conver the wildcard to regex
        wxString regexPattern = find_what;

        // Escape braces
        regexPattern.Replace("(", "\\(");
        regexPattern.Replace(")", "\\)");
        regexPattern.Replace("[", "\\[");
        regexPattern.Replace("]", "\\]");
        regexPattern.Replace("{", "\\{");
        regexPattern.Replace("}", "\\}");

        // Covnert match syntax to regular expression
        regexPattern.Replace("?", "."); // Any character
        regexPattern.Replace("*",
                             "[^\\n]*?"); // Non greedy wildcard '*', but don't allow matches to go beyond a single line

        return DoRESearch(input, startOffset, regexPattern, flags, pos, matchLen);
    }
    //---------------------------------------------------------------------------
    bool StringUtils::DoRESearch(const wxString& input, int startOffset, const wxString& find_what, size_t flags,
        int & pos, int & matchLen)
    {
        wxString str = GetString(input, startOffset, flags & wxSD_SEARCH_BACKWARD ? true : false);
        if(str.IsEmpty()) { return false; }

    #ifndef __WXMAC__
        int re_flags = wxRE_ADVANCED;
    #else
        int re_flags = wxRE_DEFAULT;
    #endif
        wxRegEx re;
        bool matchCase = flags & wxSD_MATCHCASE ? true : false;
        if(!matchCase) re_flags |= wxRE_ICASE;
        re_flags |= wxRE_NEWLINE; // Handle \n as a special character
        re.Compile(find_what, re_flags);

        // incase we are scanning NOT backwared, set the offset
        if(!(flags & wxSD_SEARCH_BACKWARD)) { pos = startOffset; }

        if(re.IsValid()) {
            if(flags & wxSD_SEARCH_BACKWARD) {
                size_t start(0), len(0);
                bool matched(false);

                // get the last match
                while(re.Matches(str)) {
                    re.GetMatch(&start, &len);
                    if(len == 0) {
                        matched = false;
                        break;
                    }
                    pos += start;
                    if(matched) { pos += matchLen; }
                    matchLen = len;
                    matched = true;
                    str = str.Mid(start + len);
                }

                if(matched) { return true; }

            } else if(re.Matches(str)) {
                size_t start, len;
                re.GetMatch(&start, &len);
                pos += start;
                matchLen = len;
                return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool StringUtils::DoSimpleSearch(const wchar_t* pinput, int startOffset, const wchar_t* pfind_what, size_t flags,
        int& pos, int& matchLen)
    {
    #if wxVERSION_NUMBER >= 2900
        std::wstring input(pinput);
        std::wstring find_what(pfind_what);

        int from = startOffset;
        if(from < 0) { from = 0; }
        std::wstring str;
        bool search_up = flags & wxSD_SEARCH_BACKWARD;
        if(!search_up) {

            if(from >= (int)input.length()) {
                str.clear();
                return false;
            }
            str = input.substr((size_t)from);

        } else {
            if(from >= (int)input.length()) { from = (int)input.length(); }
            str = input.substr(0, (size_t)from);
        }
    #else
        wxString input(pinput);
        wxString find_what(pfind_what);
        std::wstring str = GetString(pinput, startOffset, flags & wxSD_SEARCH_BACKWARD ? true : false).c_str();
    #endif

        size_t init_size = str.length();

        if(str.empty()) { return false; }

        std::wstring find_str(find_what);
        size_t offset(0);

        // incase we are scanning backwared, revert the strings
        if(flags & wxSD_SEARCH_BACKWARD) {
            find_str = Reverse(find_str);
            str = Reverse(str);
        } else {
            offset = startOffset;
        }

        bool matchCase = flags & wxSD_MATCHCASE ? true : false;
        if(!matchCase) {
            std::transform(find_str.begin(), find_str.end(), find_str.begin(), towlower);
            std::transform(str.begin(), str.end(), str.begin(), towlower);
        }

        size_t upos = str.find(find_str);

        while(upos != std::wstring::npos) {
            if(flags & wxSD_MATCHWHOLEWORD) {
                // full word match
                // test that the characeter at upos - 1 & the character at upos + find_str.Len() are not
                // valid word char [a-zA-Z0-9_]
                if(upos > 0) {
                    if(isalpha(str[upos - 1]) || isdigit(str[upos - 1]) || (int)str[upos - 1] == (int)'_') {
                        // remove the part that already been scanned
                        // and search again
                        str = str.substr(upos + find_what.length());
                        offset += upos + find_what.length();
                        upos = str.find(find_str);
                        continue;
                    }
                }
                int charAfterOff = upos + find_str.length();
                if(charAfterOff < (int)str.length()) {
                    if(isalpha(str[charAfterOff]) || isdigit(str[charAfterOff]) || (int)str[charAfterOff] == (int)'_') {
                        // remove the part that already been scanned
                        // and search again
                        str = str.substr(upos + find_what.length());
                        offset += upos + find_what.length();
                        upos = str.find(find_str);
                        continue;
                    }
                }

                matchLen = (int)find_str.length();
                // mirror the result as well
                if(flags & wxSD_SEARCH_BACKWARD) {
                    upos = (init_size - (upos + offset + matchLen));
                } else {
                    upos += offset;
                }
                pos = (int)upos;
                return true;
            } else {
                // we got a match
                matchLen = (int)find_str.length();
                if(flags & wxSD_SEARCH_BACKWARD) {
                    upos = (init_size - (upos + offset + matchLen));
                } else {
                    upos += offset;
                }
                pos = (int)upos;
                return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool StringUtils::Search(const wchar_t* input, int startOffset, const wchar_t* find_what, size_t flags, int& pos,
                                    int& matchLen, int& posInChars, int& matchLenInChars)
    {
        // adjust startOffset due to it is in bytes but should be in chars
        int iSO = startOffset;
        bool isUTF8 = false;
        if(iSO) {
            int utfLen = clUTF8Length(input, iSO);
            isUTF8 = (utfLen != iSO);
        }

        while(isUTF8 && (iSO > 0) && ((int)clUTF8Length(input, iSO) > startOffset))
            iSO--;
        startOffset = iSO;

        bool bResult = false;
        if(flags & wxSD_WILDCARD) {
            bResult = DoWildcardSearch(input, startOffset, find_what, flags, posInChars, matchLenInChars);
            flags |= wxSD_REGULAREXPRESSION;

        } else if(flags & wxSD_REGULAREXPRESSION) {
            bResult = DoRESearch(input, startOffset, find_what, flags, posInChars, matchLenInChars);

        } else {
            bResult = DoSimpleSearch(input, startOffset, find_what, flags, posInChars, matchLenInChars);
        }

        // correct search Pos and Length owing to non plain ASCII multibyte characters
        if(bResult) {
            pos = clUTF8Length(input, posInChars);
            if(flags & wxSD_REGULAREXPRESSION) {
                matchLen = clUTF8Length(input, posInChars + matchLenInChars) - pos;
            } else {
                matchLen = clUTF8Length(find_what, matchLenInChars);
            }
        }
        return bResult;
    }
    //---------------------------------------------------------------------------
    bool StringUtils::Search(const wchar_t* input, int startOffset, const wchar_t* find_what, size_t flags, int& pos,
                                    int& matchLen)
    {
        int posInChars(0), matchLenInChars(0);
        return StringUtils::Search(input, startOffset, find_what, flags, pos, matchLen, posInChars, matchLenInChars);
    }
    //---------------------------------------------------------------------------
}