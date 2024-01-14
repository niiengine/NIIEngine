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

#include "NiiPreProcess.h"
#include "NiiString.h"
#include <stdarg.h>

namespace NII
{
    //utf8字符长度1-6，可以根据每个字符第一个字节判断整个字符长度
    //0xxxxxxx
    //110xxxxx 10xxxxxx
    //1110xxxx 10xxxxxx 10xxxxxx
    //11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    //111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //
    //定义查找表，长度256，表中的数值表示以此为起始字节的utf8字符长度
    Nui8 utf8_look_for_table[] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
    };

    Nui8 utf8_look_for_table2[] =
    {
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 2, 2
    };

    #define UTFLEN(x)  utf8_look_for_table[(x)]
    #define UTFWLEN(x)  utf8_look_for_table2[(x)]
    //-----------------------------------------------------------------------
    const WString StrUtil::WBLANK;
    const VString StrUtil::VBLANK;
    const String StrUtil::BLANK;
    const String StrUtil::Whitespace(_T(" \n\t\r"));
    const String StrUtil::AlphaAndNum(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"));
    const String StrUtil::Delimiter(_T(" \n\t\r"));
    //-----------------------------------------------------------------------
    void StrUtil::trim(WString & str, bool left, bool right)
    {
        static const String delims = L" \t\r";
        if(right)
            str.erase(str.find_last_not_of(delims) + 1); // trim right
        if(left)
            str.erase(0, str.find_first_not_of(delims)); // trim left
    }
    //-----------------------------------------------------------------------
    void StrUtil::trim(VString & str, bool left, bool right)
    {
        static const VString delims = " \t\r";
        if (right)
            str.erase(str.find_last_not_of(delims) + 1); // trim right
        if (left)
            str.erase(0, str.find_first_not_of(delims)); // trim left
    }
    //------------------------------------------------------------------------
    void StrUtil::trimLeft(String & str, const String & chars)
    {
        Nidx idx = str.find_first_not_of(chars);
        if (idx != String::npos)
        {
            str.erase(0, idx);
        }
        else
        {
            str.erase();
        }
    }
    //------------------------------------------------------------------------
    void StrUtil::trimRight(String & str, const String & chars)
    {
        Nidx idx = str.find_last_not_of(chars);
        if (idx != String::npos)
        {
            str.resize(idx + 1);
        }
        else
        {
            str.erase();
        }
    }
    //-----------------------------------------------------------------------
    void StrUtil::split(const WString & str, WStringList & out, const WString & delims,
        NCount maxSplits, bool preserveDelims)
    {
        // Pre-allocate some space for performance
        out.reserve(maxSplits ? maxSplits + 1 : 10);    // 10 is guessed capacity for most case

        Nui32 numSplits = 0;

        // Use STL methods
        NCount start, pos;
        start = 0;
        do
        {
            pos = str.find_first_of(delims, start);
            if (pos == start)
            {
                // Do nothing
                start = pos + 1;
            }
            else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
            {
                // Copy the rest of the string
                out.push_back(str.substr(start));
                break;
            }
            else
            {
                // Copy up to delimiter
                out.push_back(str.substr(start, pos - start));
                if(preserveDelims)
                {
                    // Sometimes there could be more than one delimiter in a row.
                    // Loop until we don't find any more delims
                    NCount delimStart = pos, delimPos;
                    delimPos = str.find_first_not_of(delims, delimStart);
                    if (delimPos == String::npos)
                    {
                        // Copy the rest of the string
                        out.push_back(str.substr(delimStart));
                    }
                    else
                    {
                        out.push_back(str.substr(delimStart, delimPos - delimStart));
                    }
                }
                start = pos + 1;
            }
            // parse up to next real data
            start = str.find_first_not_of(delims, start);
            ++numSplits;

        } while (pos != String::npos);
    }
    //-----------------------------------------------------------------------
    void StrUtil::split(const VString & str, VStringList & out, const VString & delims,
        NCount maxSplits, bool preserveDelims)
    {
        // Pre-allocate some space for performance
        out.reserve(maxSplits ? maxSplits + 1 : 10);    // 10 is guessed capacity for most case

        Nui32 numSplits = 0;

        // Use STL methods
        NCount start, pos;
        start = 0;
        do
        {
            pos = str.find_first_of(delims, start);
            if (pos == start)
            {
                // Do nothing
                start = pos + 1;
            }
            else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
            {
                // Copy the rest of the string
                out.push_back(str.substr(start));
                break;
            }
            else
            {
                // Copy up to delimiter
                out.push_back(str.substr(start, pos - start));
                if (preserveDelims)
                {
                    // Sometimes there could be more than one delimiter in a row.
                    // Loop until we don't find any more delims
                    NCount delimStart = pos;
                    NCount delimPos = str.find_first_not_of(delims, delimStart);
                    if (delimPos == String::npos)
                    {
                        // Copy the rest of the string
                        out.push_back(str.substr(delimStart));
                    }
                    else
                    {
                        out.push_back(str.substr(delimStart, delimPos - delimStart));
                    }
                }
                start = pos + 1;
            }
            // parse up to next real data
            start = str.find_first_not_of(delims, start);
            ++numSplits;

        } while (pos != String::npos);
    }
    //-----------------------------------------------------------------------
    void StrUtil::compact(const std::vector<WString> & tokens, WString & out)
    {
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            if (tokens[i].empty())
            {
                continue;
            }
            out.append(tokens[i]);
        }
    }
    //-----------------------------------------------------------------------
    void StrUtil::compact(const std::vector<VString> & tokens, VString & out)
    {
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            if (tokens[i].empty())
            {
                continue;
            }
            out.append(tokens[i]);
        }
    }
    //-----------------------------------------------------------------------
    void StrUtil::tokenise(const WString & str, WStringList & out, const WString & singleDelims,
        const WString & enddelims, Nui32 maxSplits)
    {
        out.reserve(maxSplits ? maxSplits + 1 : 10);

        Nui32 numSplits = 0;
        WString delims = singleDelims + enddelims;

        // Use STL methods
        NCount start, pos;
        Ntchar curDoubleDelim = 0;
        start = 0;
        do
        {
            if (curDoubleDelim != 0)
            {
                pos = str.find(curDoubleDelim, start);
            }
            else
            {
                pos = str.find_first_of(delims, start);
            }

            if (pos == start)
            {
                Ntchar curDelim = str.at(pos);
                if (enddelims.find_first_of(curDelim) != String::npos)
                {
                    curDoubleDelim = curDelim;
                }
                start = pos + 1;
            }
            else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
            {
                if (curDoubleDelim != 0)
                {
                    //Missing closer. Warn or throw exception?
                }
                // Copy the rest of the string
                out.push_back(str.substr(start));
                break;
            }
            else
            {
                if (curDoubleDelim != 0)
                {
                    curDoubleDelim = 0;
                }

                // Copy up to delimiter
                out.push_back(str.substr(start, pos - start));
                start = pos + 1;
            }
            if (curDoubleDelim == 0)
            {
                // parse up to next real data
                start = str.find_first_not_of(singleDelims, start);
            }

            ++numSplits;

        } while (start != String::npos);
    }
    //-----------------------------------------------------------------------
    void StrUtil::tokenise(const VString & str, VStringList & out, const VString & singleDelims,
        const VString & enddelims, Nui32 maxSplits)
    {
        out.reserve(maxSplits ? maxSplits + 1 : 10);

        Nui32 numSplits = 0;
        VString delims = singleDelims + enddelims;

        // Use STL methods
        NCount start, pos;
        Nchar curDoubleDelim = 0;
        start = 0;
        do
        {
            if (curDoubleDelim != 0)
            {
                pos = str.find(curDoubleDelim, start);
            }
            else
            {
                pos = str.find_first_of(delims, start);
            }

            if (pos == start)
            {
                Nchar curDelim = str.at(pos);
                if (enddelims.find_first_of(curDelim) != String::npos)
                {
                    curDoubleDelim = curDelim;
                }
                start = pos + 1;
            }
            else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
            {
                if (curDoubleDelim != 0)
                {
                    //Missing closer. Warn or throw exception?
                }
                // Copy the rest of the string
                out.push_back(str.substr(start));
                break;
            }
            else
            {
                if (curDoubleDelim != 0)
                {
                    curDoubleDelim = 0;
                }

                // Copy up to delimiter
                out.push_back(str.substr(start, pos - start));
                start = pos + 1;
            }
            if (curDoubleDelim == 0)
            {
                // parse up to next real data
                start = str.find_first_not_of(singleDelims, start);
            }

            ++numSplits;

        } while (start != String::npos);
    }
    //-----------------------------------------------------------------------
    const WString & StrUtil::format(WString & out, Nwchar * c, ...)
    {
        va_list ap;
        va_start(ap, c);
        out.resize(1024);
        int re = _vsnwprintf(&out[0], 1024, c, ap);
		re > 0 ? out.resize(re) : out.resize(0);
        va_end(ap);
        return out;
    }
    //-----------------------------------------------------------------------
    const VString & StrUtil::format(VString & out, Nchar * c, ...)
    {
        va_list ap;
        va_start(ap, c);
        out.resize(1024);
        int re = _vsnprintf(&out[0], 1024, c, ap);
        re > 0 ? out.resize(re) : out.resize(0);
        va_end(ap);
        return out;
    }
    //-----------------------------------------------------------------------
    void StrUtil::toLower(WString & str)
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
    }
    //-----------------------------------------------------------------------
    void StrUtil::toLower(VString & str)
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
    }
    //-----------------------------------------------------------------------
    void StrUtil::toUpper(WString & str)
    {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }
    //-----------------------------------------------------------------------
    void StrUtil::toUpper(VString & str)
    {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }
    //-----------------------------------------------------------------------
    bool StrUtil::beginsWith(const WString & str, const WString & pattern, bool lcase)
    {
        NCount patternLen = pattern.length();
        if (str.length() < patternLen || patternLen == 0)
            return false;

        WString startOfThis = str.substr(0, patternLen);
        if (lcase)
            StrUtil::toLower(startOfThis);

        return (startOfThis == pattern);
    }
    //-----------------------------------------------------------------------
    bool StrUtil::beginsWith(const VString & str, const VString & pattern, bool lcase)
    {
        NCount patternLen = pattern.length();
        if (str.length() < patternLen || patternLen == 0)
            return false;

        VString startOfThis = str.substr(0, patternLen);
        if (lcase)
            StrUtil::toLower(startOfThis);

        return (startOfThis == pattern);
    }
    //-----------------------------------------------------------------------
    bool StrUtil::endsWith(const WString & str, const WString & pattern, bool lcase)
    {
        NCount thisLen = str.length();
        NCount patternLen = pattern.length();
        if (thisLen < patternLen || patternLen == 0)
            return false;

        WString endOfThis = str.substr(thisLen - patternLen, patternLen);
        if (lcase)
            StrUtil::toLower(endOfThis);

        return (endOfThis == pattern);
    }
    //-----------------------------------------------------------------------
    bool StrUtil::endsWith(const VString & str, const VString & pattern, bool lcase)
    {
        NCount thisLen = str.length();
        NCount patternLen = pattern.length();
        if (thisLen < patternLen || patternLen == 0)
            return false;

        VString endOfThis = str.substr(thisLen - patternLen, patternLen);
        if (lcase)
            StrUtil::toLower(endOfThis);

        return (endOfThis == pattern);
    }
    //-----------------------------------------------------------------------
    String StrUtil::getNextWord(const String & str, Nidx start_idx, const String & delimiters)
    {
        Nidx word_start = str.find_first_not_of(delimiters, start_idx);

        if (word_start == String::npos)
        {
            word_start = start_idx;
        }

        Nidx word_end = str.find_first_of(delimiters, word_start);

        if (word_end == String::npos)
        {
            word_end = str.length();
        }

        return str.substr(start_idx, (word_end - start_idx));
    }
    //------------------------------------------------------------------------
    Nidx StrUtil::getWordBegin(const String & str, Nidx idx)
    {
        String temp = str.substr(0, idx);

        trimRight(temp, Whitespace);

        if (temp.length() <= 1)
        {
            return 0;
        }

        // identify the type of character at 'pos'
        if (String::npos != AlphaAndNum.find(temp[temp.length() - 1]))
        {
            idx = temp.find_last_not_of(AlphaAndNum);
        }
        // since whitespace was stripped, character must be a symbol
        else
        {
            idx = temp.find_last_of(AlphaAndNum + Whitespace);
        }

        // make sure we do not go past end of string (+1)
        if (idx == String::npos)
        {
            return 0;
        }
        else
        {
            return idx + 1;
        }

    }
    //------------------------------------------------------------------------
    Nidx StrUtil::getNextWordBegin(const String & str, Nidx idx)
    {
        Nidx str_len = str.length();
        if ((idx >= str_len) || (str_len == 0))
        {
            return str_len;
        }

        // is character at 'idx' alphanumeric
        if (String::npos != AlphaAndNum.find(str[idx]))
        {
            // find position of next character that is not alphanumeric
            idx = str.find_first_not_of(AlphaAndNum, idx);
        }
        // is character also not whitespace (therefore a symbol)
        else if (String::npos == Whitespace.find(str[idx]))
        {
            // find index of next character that is either alphanumeric or whitespace
            idx = str.find_first_of(AlphaAndNum + Whitespace, idx);
        }

        // check result at this stage.
        if (String::npos == idx)
        {
            idx = str_len;
        }
        else
        {
            // if character at 'idx' is whitespace
            if (String::npos != Whitespace.find(str[idx]))
            {
                // find next character that is not whitespace
                idx = str.find_first_not_of(Whitespace, idx);
            }

            if (String::npos == idx)
            {
                idx = str_len;
            }

        }

        return idx;
    }
    //-----------------------------------------------------------------------
    void StrUtil::convPath(const WString & init, WString & out)
    {
        out = init;

        std::replace(out.begin(), out.end(), L'\\', L'/');
        if(out[out.length() - 1] != L'/')
            out += '/';
    }
    //-----------------------------------------------------------------------
    void StrUtil::convPath(const VString & init, VString & out)
    {
        out = init;

        std::replace(out.begin(), out.end(), '\\', '/');
        if (out[out.length() - 1] != '/')
            out += '/';
    }
    //-----------------------------------------------------------------------
    String StrUtil::convPath2(const String & init, bool lcase)
    {
        const Ntchar * bufferSrc = init.c_str();
        NIIi pathLen = (NIIi)init.size();
        NIIi indexSrc = 0;
        NIIi indexDst = 0;
        NIIi metaPathArea = 0;

        Ntchar reservedBuf[1024];
        Ntchar * bufferDst = reservedBuf;
        bool isDestAllocated = false;
        if (pathLen > 1023)
        {
            isDestAllocated = true;
            bufferDst = N_alloc_t(Ntchar, pathLen + 1);
        }

        //The outer loop loops over directories
        while (indexSrc < pathLen)
        {
            if ((bufferSrc[indexSrc] == _T('\\')) || (bufferSrc[indexSrc] == _T('/')))
            {
                ++indexSrc;
                continue;
            }
            else
            {
                //check if there is a directory to skip of type ".\"
                if ((bufferSrc[indexSrc] == _T('.')) &&
                    ((bufferSrc[indexSrc + 1] == _T('\\')) || (bufferSrc[indexSrc + 1] == _T('/'))))
                {
                    indexSrc += 2;
                    continue;
                }

                //check if there is a directory to skip of type "..\"
                else if ((bufferSrc[indexSrc] == _T('.')) && (bufferSrc[indexSrc + 1] == _T('.')) &&
                    ((bufferSrc[indexSrc + 2] == _T('\\')) || (bufferSrc[indexSrc + 2] == _T('/'))))
                {
                    if (indexDst > metaPathArea)
                    {
                        //skip a directory backward in the destination path
                        do
                        {
                            --indexDst;
                        }
                        while ((indexDst > metaPathArea) && (bufferDst[indexDst - 1] != _T('/')));

                        indexSrc += 3;
                        continue;
                    }
                    else
                    {
                        //we are about to write "..\" to the destination buffer
                        //ensure we will not remove this in future "skip directories"
                        metaPathArea += 3;
                    }
                }
            }

            //transfer the current directory name from the source to the destination
            while (indexSrc < pathLen)
            {
                Ntchar curChar = bufferSrc[indexSrc];
                if(lcase) 
                    curChar = tolower(curChar);
                if((curChar == _T('\\')) || (curChar == _T('/')))
                    curChar = _T('/');
                bufferDst[indexDst] = curChar;
                ++indexDst;
                ++indexSrc;
                if (curChar == _T('/'))
                    break;
            }
        }
        bufferDst[indexDst] = 0;

        String normalized(bufferDst);
        if (isDestAllocated)
        {
            N_free(bufferDst);
        }

        return normalized;
    }
    //-----------------------------------------------------------------------
    void StrUtil::splitFilename(const WString & file, WString & name, WString & path)
    {
        WString fullpath = file;
        // Replace \ with / first
        std::replace(fullpath.begin(), fullpath.end(), L'\\', L'/');
        // split based on final /
        NCount i = fullpath.find_last_of(L'/');

        if(i == WString::npos)
        {
            path.clear();
            name = file;
        }
        else
        {
            name = fullpath.substr(i + 1, fullpath.size() - i - 1);
            path = fullpath.substr(0, i + 1);
        }
    }
    //-----------------------------------------------------------------------
    void StrUtil::splitFilename(const VString & file, VString & name, VString & path)
    {
        VString fullpath = file;
        // Replace \ with / first
        std::replace(fullpath.begin(), fullpath.end(), '\\', '/');
        // split based on final /
        NCount i = fullpath.find_last_of('/');

        if(i == VString::npos)
        {
            path.clear();
            name = file;
        }
        else
        {
            name = fullpath.substr(i + 1, fullpath.size() - i - 1);
            path = fullpath.substr(0, i + 1);
        }
    }
    //-----------------------------------------------------------------------
    void StrUtil::splitPath(const WString & file, WString & path)
    {
        WString fullpath = file;
        // Replace \ with / first
        std::replace(fullpath.begin(), fullpath.end(), L'\\', L'/');
        // split based on final /
        NCount i = fullpath.find_last_of(L'/');

        if (i == WString::npos)
        {
            path.clear();
        }
        else
        {
            path = fullpath.substr(0, i + 1);
        }
    }
    //-----------------------------------------------------------------------
    void StrUtil::splitPath(const VString & file, VString & path)
    {
        VString fullpath = file;
        // Replace \ with / first
        std::replace(fullpath.begin(), fullpath.end(), '\\', '/');
        // split based on final /
        NCount i = fullpath.find_last_of('/');

        if (i == VString::npos)
        {
            path.clear();
        }
        else
        {

            path = fullpath.substr(0, i + 1);
        }
    }
    //-----------------------------------------------------------------------
    void StrUtil::splitBaseFilename(const WString & name, WString & bname, WString & ext)
    {
        NCount i = name.find_last_of(L".");
        if (i == WString::npos)
        {
            ext.clear();
            bname = name;
        }
        else
        {
            ext = name.substr(i + 1);
            bname = name.substr(0, i);
        }
    }
    //-----------------------------------------------------------------------
    void StrUtil::splitBaseFilename(const VString & name, VString & bname, VString & ext)
    {
        NCount i = name.find_last_of(".");
        if (i == VString::npos)
        {
            ext.clear();
            bname = name;
        }
        else
        {
            ext = name.substr(i + 1);
            bname = name.substr(0, i);
        }
    }
    //-----------------------------------------------------------------------
    NCount StrUtil::getUtf8Length(Nutf8 * strin, NCount * bcnt, char delims)
    {
        NCount len = 0;
        Nutf8 * ptr;
        if (delims == '\0')
        {
            for (ptr = strin; *ptr != 0; len++, ptr += UTFLEN(*ptr));
        }
        else
        {
            for (ptr = strin; *ptr != 0; len++, ptr += UTFLEN(*ptr))
            {
                if (*ptr == delims)
                    break;
            }
        }
        if (bcnt)
            *bcnt = ptr - strin;
        return len;
    }
    //-----------------------------------------------------------------------
    NCount StrUtil::getUtf8Length(Nutf8 * strin, Nutf8 * strend, NCount * bcnt, Nchar delims)
    {
        NCount len = 0;
        Nutf8 * ptr;
        if (delims == '\0')
        {
            for (ptr = strin; *ptr != 0, strend > ptr; len++, ptr += UTFLEN(*ptr));
        }
        else
        {
            for (ptr = strin; *ptr != 0, strend > ptr; len++, ptr += UTFLEN(*ptr))
            {
                if (*ptr == delims)
                    break;
            }
        }
        if (bcnt)
            *bcnt = ptr - strin;
        return len;
    }
    //-----------------------------------------------------------------------
    NCount StrUtil::getUtf8Length(Nutf8 * strin, Nutf8 * strend, NCount * bcnt, NCount * wcnt, Nchar delims)
    {
        NCount len = 0;
        NCount wcount = 0;
        Nutf8 * ptr;
        if (delims == '\0')
        {
            for (ptr = strin; *ptr != 0, strend > ptr; len++, ptr += UTFLEN(*ptr), wcount += UTFWLEN(*ptr));
        }
        else
        {
            for (ptr = strin; *ptr != 0, strend > ptr; len++, ptr += UTFLEN(*ptr), wcount += UTFWLEN(*ptr))
            {
                if (*ptr == delims)
                    break;
            }
        }
        if (bcnt)
            *bcnt = ptr - strin;
        if (wcnt)
            *wcnt = wcount;
        return len;
    }
    //-----------------------------------------------------------------------
    Nutf8 * StrUtil::getUtf8Sub(Nutf8 * value, NCount b, NCount e)
    {
        NCount len = getUtf8Length(value);

        if(b >= len)
            return NULL;
        if(e > len)
            e = len;

        Nutf8 * sptr = value;
        for(NCount i = 0; i < b; ++i, sptr += UTFLEN(*sptr));

        Nutf8 * eptr = sptr;
        for(NCount i = b; i < e; ++i, eptr += UTFLEN(*eptr));

        int retLen = eptr - sptr;
        Nutf8 * re = N_alloc_t(Nutf8, retLen + 1);
        memcpy(re, sptr, retLen);
        re[retLen] = 0;

        return re;
    }
    // ----------------------------------------------------------------------
    void StrUtil::splitFilename(const WString & file, WString & bname, WString & ext, WString & path)
    {
        WString name;
        splitFilename(file, name, path);
        splitBaseFilename(name, bname, ext);
    }
    // ----------------------------------------------------------------------
    void StrUtil::splitFilename(const VString & file, VString & bname, VString & ext, VString & path)
    {
        VString name;
        splitFilename(file, name, path);
        splitBaseFilename(name, bname, ext);
    }
    //-----------------------------------------------------------------------
    bool StrUtil::match(const WString & str, const WString & pattern, bool lowerCase)
    {
        WString tmpStr = str;
        WString tmpPattern = pattern;
        if(lowerCase)
        {
            StrUtil::toLower(tmpStr);
            StrUtil::toLower(tmpPattern);
        }

        WString::const_iterator strIt = tmpStr.begin();
        WString::const_iterator patIt = tmpPattern.begin();
        WString::const_iterator lastWildCardIt = tmpPattern.end();
        while(strIt != tmpStr.end() && patIt != tmpPattern.end())
        {
            if(*patIt == L'*')
            {
                lastWildCardIt = patIt;
                // Skip over looking for next character
                ++patIt;
                if (patIt == tmpPattern.end())
                {
                    // Skip right to the end since * matches the entire rest of the string
                    strIt = tmpStr.end();
                }
                else
                {
                    // scan until we find next pattern character
                    while(strIt != tmpStr.end() && *strIt != *patIt)
                        ++strIt;
                }
            }
            else
            {
                if(*patIt != *strIt)
                {
                    if (lastWildCardIt != tmpPattern.end())
                    {
                        // The last wildcard can match this incorrect sequence
                        // rewind pattern to wildcard and keep searching
                        patIt = lastWildCardIt;
                        lastWildCardIt = tmpPattern.end();
                    }
                    else
                    {
                        // no wildwards left
                        return false;
                    }
                }
                else
                {
                    ++patIt;
                    ++strIt;
                }
            }

        }
        // If we reached the end of both the pattern and the string, we succeeded
        if(patIt == tmpPattern.end() && strIt == tmpStr.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    //-----------------------------------------------------------------------
    bool StrUtil::match(const VString & str, const VString & pattern, bool lowerCase)
    {
        VString tmpStr = str;
        VString tmpPattern = pattern;
        if (lowerCase)
        {
            StrUtil::toLower(tmpStr);
            StrUtil::toLower(tmpPattern);
        }

        VString::const_iterator strIt = tmpStr.begin();
        VString::const_iterator patIt = tmpPattern.begin();
        VString::const_iterator lastWildCardIt = tmpPattern.end();
        while (strIt != tmpStr.end() && patIt != tmpPattern.end())
        {
            if (*patIt == '*')
            {
                lastWildCardIt = patIt;
                // Skip over looking for next character
                ++patIt;
                if (patIt == tmpPattern.end())
                {
                    // Skip right to the end since * matches the entire rest of the string
                    strIt = tmpStr.end();
                }
                else
                {
                    // scan until we find next pattern character
                    while (strIt != tmpStr.end() && *strIt != *patIt)
                        ++strIt;
                }
            }
            else
            {
                if (*patIt != *strIt)
                {
                    if (lastWildCardIt != tmpPattern.end())
                    {
                        // The last wildcard can match this incorrect sequence
                        // rewind pattern to wildcard and keep searching
                        patIt = lastWildCardIt;
                        lastWildCardIt = tmpPattern.end();
                    }
                    else
                    {
                        // no wildwards left
                        return false;
                    }
                }
                else
                {
                    ++patIt;
                    ++strIt;
                }
            }

        }
        // If we reached the end of both the pattern and the string, we succeeded
        if (patIt == tmpPattern.end() && strIt == tmpStr.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    //-----------------------------------------------------------------------
    WString StrUtil::replaceAll(const WString & source, const WString & dst, const WString & target)
    {
        WString result = source;
        WString::size_type pos = 0;
        while(1)
        {
            pos = result.find(dst, pos);
            if (pos == WString::npos) break;
            result.replace(pos, dst.size(), target);
            pos += target.size();
        }
        return result;
    }
    //-----------------------------------------------------------------------
    VString StrUtil::replaceAll(const VString & source, const VString & dst, const VString & target)
    {
        VString result = source;
        VString::size_type pos = 0;
        while (1)
        {
            pos = result.find(dst, pos);
            if (pos == VString::npos) break;
            result.replace(pos, dst.size(), target);
            pos += target.size();
        }
        return result;
    }
    //-----------------------------------------------------------------------
}