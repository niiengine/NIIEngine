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
#include "RakString.h"
#include "NiiNetSerializer.h"
#include <stdarg.h>
#include <string.h>
#include "LinuxStrings.h"
#include "StringTable.h"
#include "NiiThreadManager.h"
#include <stdlib.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    RakString::SharedString RakString::emptyString={0,0,0,(char*) "",(char*) ""};
    DataStructures::List<RakString::SharedString*> RakString::freeList;
    //------------------------------------------------------------------------
    class RakStringCleanup
    {
    public:
        ~RakStringCleanup()
        {
            RakString::FreeMemoryNoMutex();
        }
    };
    //------------------------------------------------------------------------
    static RakStringCleanup cleanup;
    //------------------------------------------------------------------------
    ThreadMutex & GetPoolMutex(void)
    {
        static ThreadMutex poolMutex;
        return poolMutex;
    }
    //------------------------------------------------------------------------
    int RakString::RakStringComp( RakString const &key, RakString const &data )
    {
        return key.StrCmp(data);
    }
    //------------------------------------------------------------------------
    RakString::RakString()
    {
        sharedString=&emptyString;
    }
    //------------------------------------------------------------------------
    RakString::RakString( RakString::SharedString *_sharedString )
    {
        sharedString=_sharedString;
    }
    //------------------------------------------------------------------------
    RakString::RakString(char input)
    {
        char str[2];
        str[0]=input;
        str[1]=0;
        Assign(str);
    }
    //------------------------------------------------------------------------
    RakString::RakString(Nui8 input)
    {
        char str[2];
        str[0]=(char) input;
        str[1]=0;
        Assign(str);
    }
    //------------------------------------------------------------------------
    RakString::RakString(const Nui8 *format, ...){
        va_list ap;
        va_start(ap, format);
        Assign((const char*) format,ap);
    }
    //------------------------------------------------------------------------
    RakString::RakString(const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        Assign(format,ap);
    }
    //------------------------------------------------------------------------
    RakString::RakString( const RakString & rhs)
    {
        if (rhs.sharedString==&emptyString)
        {
            sharedString=&emptyString;
            return;
        }

        rhs.sharedString->refCountMutex->Lock();
        if (rhs.sharedString->mRefCount==0)
        {
            sharedString=&emptyString;
        }
        else
        {
            rhs.sharedString->mRefCount++;
            sharedString=rhs.sharedString;
        }
        rhs.sharedString->refCountMutex->Unlock();
    }
    //------------------------------------------------------------------------
    RakString::~RakString()
    {
        Free();
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator = ( const RakString& rhs )
    {
        Free();
        if (rhs.sharedString==&emptyString)
            return *this;

        rhs.sharedString->refCountMutex->Lock();
        if (rhs.sharedString->mRefCount==0)
        {
            sharedString=&emptyString;
        }
        else
        {
            sharedString=rhs.sharedString;
            sharedString->mRefCount++;
        }
        rhs.sharedString->refCountMutex->Unlock();
        return *this;
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator = ( const char *str )
    {
        Free();
        Assign(str);
        return *this;
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator = (char * str)
    {
        return operator = ((const char *)str);
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator = (const Nui8 * str)
    {
        return operator = ((const char*)str);
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator = (Nui8 * str)
    {
        return operator = ((const char*)str);
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator = (const char c)
    {
        char buff[2];
        buff[0]=c;
        buff[1]=0;
        return operator = ((const char*)buff);
    }
    //------------------------------------------------------------------------
    void RakString::Realloc(SharedString *sharedString, size_t bytes)
    {
        if (bytes<=sharedString->bytesUsed)
            return;

        N_assert(bytes>0);
        size_t oldBytes = sharedString->bytesUsed;
        size_t newBytes;
        const size_t smallStringSize = 128-sizeof(Nui32)-sizeof(size_t)-sizeof(char*)*2;
        newBytes = GetSizeToAllocate(bytes);
        if (oldBytes <=(size_t) smallStringSize && newBytes > (size_t) smallStringSize)
        {
            sharedString->bigString=(char*) N_alloc(newBytes);
            strcpy(sharedString->bigString, sharedString->smallString);
            sharedString->c_str=sharedString->bigString;
        }
        else if (oldBytes > smallStringSize)
        {
            sharedString->bigString = (char *)N_realloc(sharedString->bigString,newBytes); // 需要修改
            sharedString->c_str=sharedString->bigString;
        }
        sharedString->bytesUsed=newBytes;
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator +=( const RakString& rhs)
    {
        if (rhs.IsEmpty())
            return *this;

        if (IsEmpty())
        {
            return operator=(rhs);
        }
        else
        {
            Clone();
            size_t strLen=rhs.GetLength()+GetLength()+1;
            Realloc(sharedString, strLen+GetLength());
            strcat(sharedString->c_str,rhs.C_String());
        }
        return *this;
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator +=( const char *str )
    {
        if (str==0 || str[0]==0)
            return *this;

        if (IsEmpty())
        {
            Assign(str);
        }
        else
        {
            Clone();
            size_t strLen=strlen(str)+GetLength()+1;
            Realloc(sharedString, strLen);
            strcat(sharedString->c_str,str);
        }
        return *this;
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator +=( char *str )
    {
        return operator += ((const char*)str);
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator +=( const Nui8 *str )
    {
        return operator += ((const char*)str);
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator +=( Nui8 *str )
    {
        return operator += ((const char*)str);
    }
    //------------------------------------------------------------------------
    RakString& RakString::operator +=( const char c )
    {
        char buff[2];
        buff[0]=c;
        buff[1]=0;
        return operator += ((const char*)buff);
    }
    //------------------------------------------------------------------------
    Nui8 RakString::operator[] ( const Nui32 position ) const
    {
        N_assert(position<GetLength());
        return sharedString->c_str[position];
    }
    //------------------------------------------------------------------------
    bool RakString::operator==(const RakString &rhs) const
    {
        return strcmp(sharedString->c_str,rhs.sharedString->c_str)==0;
    }
    //------------------------------------------------------------------------
    bool RakString::operator==(const char *str) const
    {
        return strcmp(sharedString->c_str,str)==0;
    }
    //------------------------------------------------------------------------
    bool RakString::operator==(char *str) const
    {
        return strcmp(sharedString->c_str,str)==0;
    }
    //------------------------------------------------------------------------
    bool RakString::operator < ( const RakString& right ) const
    {
        return strcmp(sharedString->c_str,right.C_String()) < 0;
    }
    //------------------------------------------------------------------------
    bool RakString::operator <= ( const RakString& right ) const
    {
        return strcmp(sharedString->c_str,right.C_String()) <= 0;
    }
    //------------------------------------------------------------------------
    bool RakString::operator > ( const RakString& right ) const
    {
        return strcmp(sharedString->c_str,right.C_String()) > 0;
    }
    //------------------------------------------------------------------------
    bool RakString::operator >= ( const RakString& right ) const
    {
        return strcmp(sharedString->c_str,right.C_String()) >= 0;
    }
    //------------------------------------------------------------------------
    bool RakString::operator!=(const RakString &rhs) const
    {
        return strcmp(sharedString->c_str,rhs.sharedString->c_str)!=0;
    }
    //------------------------------------------------------------------------
    bool RakString::operator!=(const char *str) const
    {
        return strcmp(sharedString->c_str,str)!=0;
    }
    //------------------------------------------------------------------------
    bool RakString::operator!=(char *str) const
    {
        return strcmp(sharedString->c_str,str)!=0;
    }
    //------------------------------------------------------------------------
    const RakString operator+(const RakString &lhs, const RakString &rhs)
    {
        if (lhs.IsEmpty() && rhs.IsEmpty())
        {
            return RakString(&RakString::emptyString);
        }
        if (lhs.IsEmpty())
        {
            rhs.sharedString->refCountMutex->Lock();
            if (rhs.sharedString->mRefCount==0)
            {
                rhs.sharedString->refCountMutex->Unlock();
                lhs.sharedString->refCountMutex->Lock();
                lhs.sharedString->mRefCount++;
                lhs.sharedString->refCountMutex->Unlock();
                return RakString(lhs.sharedString);
            }
            else
            {
                rhs.sharedString->mRefCount++;
                rhs.sharedString->refCountMutex->Unlock();
                return RakString(rhs.sharedString);
            }
            // rhs.sharedString->refCountMutex->Unlock();
        }
        if (rhs.IsEmpty())
        {
            lhs.sharedString->refCountMutex->Lock();
            lhs.sharedString->mRefCount++;
            lhs.sharedString->refCountMutex->Unlock();
            return RakString(lhs.sharedString);
        }

        size_t len1 = lhs.GetLength();
        size_t len2 = rhs.GetLength();
        size_t allocatedBytes = len1 + len2 + 1;
        allocatedBytes = RakString::GetSizeToAllocate(allocatedBytes);
        RakString::SharedString *sharedString;

        RakString::LockMutex();
        if (RakString::freeList.Size() == 0)
        {
            Nui32 i;
            for (i = 0; i < 128; ++i)
            {
                RakString::SharedString * ss;
                ss = (RakString::SharedString *) N_alloc(sizeof(RakString::SharedString));
                ss->refCountMutex = N_new ThreadMutex();
                RakString::freeList.Insert(ss);

            }
        }
        sharedString = RakString::freeList[RakString::freeList.Size()-1];
        RakString::freeList.RemoveAtIndex(RakString::freeList.Size()-1);
        RakString::UnlockMutex();

        const int smallStringSize = 128-sizeof(Nui32)-sizeof(size_t)-sizeof(char*)*2;
        sharedString->bytesUsed=allocatedBytes;
        sharedString->mRefCount=1;
        if (allocatedBytes <= (size_t) smallStringSize)
        {
            sharedString->c_str=sharedString->smallString;
        }
        else
        {
            sharedString->bigString=(char*)N_alloc(sharedString->bytesUsed);
            sharedString->c_str=sharedString->bigString;
        }

        strcpy(sharedString->c_str, lhs);
        strcat(sharedString->c_str, rhs);

        return RakString(sharedString);
    }
    //------------------------------------------------------------------------
    const char * RakString::ToLower(void)
    {
        Clone();

        size_t strLen = strlen(sharedString->c_str);
        Nui32 i;
        for (i=0; i < strLen; i++)
            sharedString->c_str[i]=ToLower(sharedString->c_str[i]);
        return sharedString->c_str;
    }
    //------------------------------------------------------------------------
    const char * RakString::ToUpper(void)
    {
        Clone();

        size_t strLen = strlen(sharedString->c_str);
        Nui32 i;
        for (i=0; i < strLen; i++)
            sharedString->c_str[i]=ToUpper(sharedString->c_str[i]);
        return sharedString->c_str;
    }
    //------------------------------------------------------------------------
    void RakString::Set(const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);
        Clear();
        Assign(format,ap);
    }
    //------------------------------------------------------------------------
    bool RakString::IsEmpty(void) const
    {
        return sharedString==&emptyString;
    }
    //------------------------------------------------------------------------
    size_t RakString::GetLength(void) const
    {
        return strlen(sharedString->c_str);
    }
    //------------------------------------------------------------------------
    // http://porg.es/blog/counting-characters-in-utf-8-strings-is-faster
    int porges_strlen2(char *s)
    {
        int i = 0;
        int iBefore = 0;
        int count = 0;

        while (s[i] > 0)
    ascii:  i++;

        count += i-iBefore;
        while (s[i])
        {
            if (s[i] > 0)
            {
                iBefore = i;
                goto ascii;
            }
            else
                switch (0xF0 & s[i])
            {
                case 0xE0: i += 3; break;
                case 0xF0: i += 4; break;
                default:   i += 2; break;
            }
            ++count;
        }
        return count;
    }
    //------------------------------------------------------------------------
    size_t RakString::GetLengthUTF8(void) const
    {
        return porges_strlen2(sharedString->c_str);
    }
    //------------------------------------------------------------------------
    void RakString::Replace(Nui32 index, Nui32 count, Nui8 c)
    {
        N_assert(index+count < GetLength());
        Clone();
        Nui32 countIndex=0;
        while (countIndex<count)
        {
            sharedString->c_str[index]=c;
            index++;
            countIndex++;
        }

    }
    //------------------------------------------------------------------------
    void RakString::SetChar( Nui32 index, Nui8 c )
    {
        N_assert(index < GetLength());
        Clone();
        sharedString->c_str[index]=c;
    }
    //------------------------------------------------------------------------
    void RakString::SetChar( Nui32 index, RakString s )
    {
        N_assert(index < GetLength());
        Clone();
        RakString firstHalf = SubStr(0, index);
        RakString secondHalf = SubStr(index+1, (Nui32)-1);
        *this = firstHalf;
        *this += s;
        *this += secondHalf;
    }
    //------------------------------------------------------------------------
    #if N_PLAT == N_PLAT_WIN32
    WCHAR * RakString::ToWideChar(void)
    {
        //
        // Special case of NULL or empty input string
        //
        if ( (sharedString->c_str == NULL) || (*sharedString->c_str == '\0') )
        {
            // Return empty string
            return L"";
        }

        //
        // Get size of destination UTF-16 buffer, in WCHAR's
        //
        int cchUTF16 = ::MultiByteToWideChar(
            CP_UTF8,                // convert from UTF-8
            0,						// Flags
            sharedString->c_str,            // source UTF-8 string
            GetLength()+1,                 // total length of source UTF-8 string,
            // in CHAR's (= bytes), including end-of-string \0
            NULL,                   // unused - no conversion done in this step
            0                       // request size of destination buffer, in WCHAR's
            );

        if ( cchUTF16 == 0 )
        {
            N_assert("RakString::ToWideChar exception from cchUTF16==0" && 0);
            return 0;
        }

        //
        // Allocate destination buffer to store UTF-16 string
        //
        WCHAR * pszUTF16 = N_alloc_t(WCHAR, cchUTF16);

        //
        // Do the conversion from UTF-8 to UTF-16
        //
        int result = ::MultiByteToWideChar(
            CP_UTF8,                // convert from UTF-8
            0,						// Buffer
            sharedString->c_str,            // source UTF-8 string
            GetLength()+1,                 // total length of source UTF-8 string,
            // in CHAR's (= bytes), including end-of-string \0
            pszUTF16,               // destination buffer
            cchUTF16                // size of destination buffer, in WCHAR's
            );

        if ( result == 0 )
        {
            N_assert("RakString::ToWideChar exception from MultiByteToWideChar" && 0);
            return 0;
        }

        return pszUTF16;
    }
    //------------------------------------------------------------------------
    void RakString::DeallocWideChar(WCHAR * w)
    {
        N_free(w);
    }
    //------------------------------------------------------------------------
    void RakString::FromWideChar(const wchar_t *source)
    {
        Clear();
        int bufSize = wcslen(source)*4;
        Allocate(bufSize);
        WideCharToMultiByte (CP_ACP,                // ANSI code page
                              WC_COMPOSITECHECK,     // Check for accented characters

                              source,         // Source Unicode string
                              -1,                    // -1 means string is zero-terminated
                              sharedString->c_str,          // Destination char string
                              bufSize,  // Size of buffer
                              NULL,                  // No default character
                              NULL);                // Don't care about this flag
    }
    //------------------------------------------------------------------------
    RakString RakString::FromWideChar_S(const wchar_t * source)
    {
        RakString rs;
        rs.FromWideChar(source);
        return rs;
    }
    #endif
    //------------------------------------------------------------------------
    size_t RakString::Find(const char *stringToFind,size_t pos)
    {
        size_t len=GetLength();
        if (pos>=len || stringToFind==0 || stringToFind[0]==0)
        {
            return (size_t) -1;
        }
        size_t matchLen= strlen(stringToFind);
        size_t matchPos=0;
        size_t iStart=0;

        for (size_t i=pos;i<len;i++)
        {
            if (stringToFind[matchPos]==sharedString->c_str[i])
            {
                if(matchPos==0)
                {
                    iStart=i;
                }
                matchPos++;
            }
            else
            {
                matchPos=0;
            }

            if (matchPos>=matchLen)
            {
                return iStart;
            }
        }

        return (size_t) -1;
    }
    //------------------------------------------------------------------------
    void RakString::TruncateUTF8(Nui32 length)
    {
        int i = 0;
        Nui32 count = 0;

        while (sharedString->c_str[i]!=0)
        {
            if (count==length)
            {
                sharedString->c_str[i]=0;
                return;
            }
            else if (sharedString->c_str[i]>0)
            {
                i++;
            }
            else
            {
                switch (0xF0 & sharedString->c_str[i])
                {
                case 0xE0: i += 3; break;
                case 0xF0: i += 4; break;
                default:   i += 2; break;
                }
            }

            count++;
        }
    }
    //------------------------------------------------------------------------
    void RakString::Truncate(Nui32 length)
    {
        if (length < GetLength())
        {
            SetChar(length, 0);
        }
    }
    //------------------------------------------------------------------------
    RakString RakString::SubStr(Nui32 index, Nui32 count) const
    {
        size_t length = GetLength();
        if (index >= length || count==0)
            return RakString();
        RakString copy;
        size_t numBytes = length-index;
        if (count < numBytes)
            numBytes=count;
        copy.Allocate(numBytes+1);
        size_t i;
        for (i=0; i < numBytes; i++)
            copy.sharedString->c_str[i]=sharedString->c_str[index+i];
        copy.sharedString->c_str[i]=0;
        return copy;
    }
    //------------------------------------------------------------------------
    void RakString::Erase(Nui32 index, Nui32 count)
    {
        size_t len = GetLength();
        N_assert(index+count <= len);

        Clone();
        Nui32 i;
        for (i=index; i < len-count; i++)
        {
            sharedString->c_str[i]=sharedString->c_str[i+count];
        }
        sharedString->c_str[i]=0;
    }
    //------------------------------------------------------------------------
    void RakString::TerminateAtLastCharacter(char c)
    {
        int i, len=(int) GetLength();
        for (i=len-1; i >= 0; i--)
        {
            if (sharedString->c_str[i]==c)
            {
                Clone();
                sharedString->c_str[i]=0;
                return;
            }
        }
    }
    //------------------------------------------------------------------------
    void RakString::TerminateAtFirstCharacter(char c)
    {
        Nui32 i, len=(Nui32) GetLength();
        for (i=0; i < len; i++)
        {
            if (sharedString->c_str[i]==c)
            {
                Clone();
                sharedString->c_str[i]=0;
                return;
            }
        }
    }
    //------------------------------------------------------------------------
    int RakString::GetCharacterCount(char c)
    {
        int count = 0;
        Nui32 i, len = (Nui32) GetLength();
        for (i = 0; i < len; i++)
        {
            if (sharedString->c_str[i]==c)
            {
                ++count;
            }
        }
        return count;
    }
    //------------------------------------------------------------------------
    void RakString::RemoveCharacter(char c)
    {
        if (c==0)
            return;

        Nui32 readIndex, writeIndex=0;
        for (readIndex=0; sharedString->c_str[readIndex]; readIndex++)
        {
            if (sharedString->c_str[readIndex]!=c)
                sharedString->c_str[writeIndex++]=sharedString->c_str[readIndex];
            else
                Clone();
        }
        sharedString->c_str[writeIndex]=0;
        if (writeIndex==0)
            Clear();
    }
    //------------------------------------------------------------------------
    int RakString::StrCmp(const RakString &rhs) const
    {
        return strcmp(sharedString->c_str, rhs.C_String());
    }
    //------------------------------------------------------------------------
    int RakString::StrNCmp(const RakString &rhs, size_t num) const
    {
        return strncmp(sharedString->c_str, rhs.C_String(), num);
    }
    //------------------------------------------------------------------------
    int RakString::StrICmp(const RakString &rhs) const
    {
        return _stricmp(sharedString->c_str, rhs.C_String());
    }
    //------------------------------------------------------------------------
    void RakString::Printf(void)
    {
        N_printf("%s", sharedString->c_str);
    }
    //------------------------------------------------------------------------
    void RakString::FPrintf(FILE *fp)
    {
        fprintf(fp,"%s", sharedString->c_str);
    }
    //------------------------------------------------------------------------
    /*bool RakString::IPAddressMatch(const char * IP)
    {
        Nui32 characterIndex;

        if (IP == 0 || IP[ 0 ] == 0 || strlen( IP ) > 15)
            return false;

        characterIndex = 0;

        while (1)
        {
            if (sharedString->c_str[ characterIndex ] == IP[ characterIndex ] )
            {
                // Equal characters
                if ( IP[ characterIndex ] == 0 )
                {
                    // End of the string and the strings match

                    return true;
                }
                characterIndex++;
            }

            else
            {
                if ( sharedString->c_str[ characterIndex ] == 0 || IP[ characterIndex ] == 0 )
                {
                    // End of one of the strings
                    break;
                }
                // Characters do not match
                if ( sharedString->c_str[ characterIndex ] == '*' )
                {
                    // Domain is banned.
                    return true;
                }
                // Characters do not match and it is not a *
                break;
            }
        }
        return false;
    }*/
    //------------------------------------------------------------------------
    bool RakString::ContainsNonprintableExceptSpaces(void) const
    {
        size_t strLen = strlen(sharedString->c_str);
        Nui32 i;
        for (i=0; i < strLen; i++)
        {
            if (sharedString->c_str[i] < ' ' || sharedString->c_str[i] >126)
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool RakString::IsEmailAddress(void) const
    {
        if (IsEmpty())
            return false;
        size_t strLen = strlen(sharedString->c_str);
        if (strLen < 6) // a@b.de
            return false;
        if (sharedString->c_str[strLen-4]!='.' && sharedString->c_str[strLen-3]!='.') // .com, .net., .org, .de
            return false;
        Nui32 i;
        // Has non-printable?
        for (i = 0; i < strLen; i++)
        {
            if (sharedString->c_str[i] <= ' ' || sharedString->c_str[i] >126)
                return false;
        }
        int atCount = 0;
        for (i = 0; i < strLen; i++)
        {
            if (sharedString->c_str[i]=='@')
            {
                atCount++;
            }
        }
        if (atCount != 1)
            return false;
        int dotCount = 0;
        for (i = 0; i < strLen; i++)
        {
            if (sharedString->c_str[i]=='.')
            {
                dotCount++;
            }
        }
        if (dotCount == 0)
            return false;

        // There's more I could check, but this is good enough
        return true;
    }
    //------------------------------------------------------------------------
    RakString& RakString::URLEncode(void)
    {
        RakString result;
        size_t strLen = strlen(sharedString->c_str);
        result.Allocate(strLen*3);
        char *output=result.sharedString->c_str;
        Nui32 outputIndex=0;
        Nui32 i;
        Nui8 c;
        for (i=0; i < strLen; i++)
        {
            c=sharedString->c_str[i];
            if (
                (c<=47) ||
                (c>=58 && c<=64) ||
                (c>=91 && c<=96) ||
                (c>=123)
                )
            {
                char buff[3];
                i_a(c, buff, 16);
                output[outputIndex++]='%';
                output[outputIndex++]=buff[0];
                output[outputIndex++]=buff[1];
            }
            else
            {
                output[outputIndex++]=c;
            }
        }

        output[outputIndex]=0;

        *this = result;
        return *this;
    }
    //------------------------------------------------------------------------
    RakString& RakString::URLDecode(void)
    {
        RakString result;
        size_t strLen = strlen(sharedString->c_str);
        result.Allocate(strLen);
        char *output=result.sharedString->c_str;
        Nui32 outputIndex=0;
        char c;
        char hexDigits[2];
        char hexValues[2];
        Nui32 i;
        for (i=0; i < strLen; i++)
        {
            c=sharedString->c_str[i];
            if (c=='%')
            {
                hexDigits[0]=sharedString->c_str[++i];
                hexDigits[1]=sharedString->c_str[++i];

                if (hexDigits[0]==' ')
                    hexValues[0]=0;

                if (hexDigits[0]>='A' && hexDigits[0]<='F')
                    hexValues[0]=hexDigits[0]-'A'+10;
                if (hexDigits[0]>='a' && hexDigits[0]<='f')
                    hexValues[0]=hexDigits[0]-'a'+10;
                else
                    hexValues[0]=hexDigits[0]-'0';

                if (hexDigits[1]>='A' && hexDigits[1]<='F')
                    hexValues[1]=hexDigits[1]-'A'+10;
                if (hexDigits[1]>='a' && hexDigits[1]<='f')
                    hexValues[1]=hexDigits[1]-'a'+10;
                else
                    hexValues[1]=hexDigits[1]-'0';

                output[outputIndex++]=hexValues[0]*16+hexValues[1];
            }
            else
            {
                output[outputIndex++]=c;
            }
        }

        output[outputIndex]=0;

        *this = result;
        return *this;
    }
    //------------------------------------------------------------------------
    void RakString::SplitURI(RakString &header, RakString &domain, RakString &path)
    {
        header.Clear();
        domain.Clear();
        path.Clear();

        size_t strLen = strlen(sharedString->c_str);

        char c;
        Nui32 i=0;
        if (strncmp(sharedString->c_str, "http://", 7)==0)
            i+=(Nui32) strlen("http://");
        else if (strncmp(sharedString->c_str, "https://", 8)==0)
            i+=(Nui32) strlen("https://");

        if (strncmp(sharedString->c_str, "www.", 4)==0)
            i+=(Nui32) strlen("www.");

        if (i!=0)
        {
            header.Allocate(i+1);
            strncpy(header.sharedString->c_str, sharedString->c_str, i);
            header.sharedString->c_str[i]=0;
        }


        domain.Allocate(strLen-i+1);
        char *domainOutput=domain.sharedString->c_str;
        Nui32 outputIndex=0;
        for (; i < strLen; i++)
        {
            c=sharedString->c_str[i];
            if (c=='/')
            {
                break;
            }
            else
            {
                domainOutput[outputIndex++]=sharedString->c_str[i];
            }
        }

        domainOutput[outputIndex]=0;

        path.Allocate(strLen-header.GetLength()-outputIndex+1);
        outputIndex=0;
        char *pathOutput=path.sharedString->c_str;
        for (; i < strLen; i++)
        {
            pathOutput[outputIndex++]=sharedString->c_str[i];
        }
        pathOutput[outputIndex]=0;
    }
    //------------------------------------------------------------------------
    RakString& RakString::SQLEscape(void)
    {
        int strLen=(int)GetLength();
        int escapedCharacterCount=0;
        int index;
        for (index=0; index < strLen; index++)
        {
            if (sharedString->c_str[index]=='\'' ||
                sharedString->c_str[index]=='"' ||
                sharedString->c_str[index]=='\\')
                escapedCharacterCount++;
        }
        if (escapedCharacterCount==0)
            return *this;

        Clone();
        Realloc(sharedString, strLen+escapedCharacterCount);
        int writeIndex, readIndex;
        writeIndex = strLen+escapedCharacterCount;
        readIndex=strLen;
        while (readIndex>=0)
        {
            if (sharedString->c_str[readIndex]=='\'' ||
                sharedString->c_str[readIndex]=='"' ||
                sharedString->c_str[readIndex]=='\\')
            {
                sharedString->c_str[writeIndex--]=sharedString->c_str[readIndex--];
                sharedString->c_str[writeIndex--]='\\';
            }
            else
            {
                sharedString->c_str[writeIndex--]=sharedString->c_str[readIndex--];
            }
        }
        return *this;
    }
    //------------------------------------------------------------------------
    RakString RakString::FormatForPUTOrPost(const char* type, const char* uri, const char* contentType, const char* body, const char* extraHeaders)
    {
        RakString out;
        RakString host;
        RakString remotePath;
        RakString header;
        RakString uriRs;
        uriRs = uri;
        uriRs.SplitURI(header, host, remotePath);

        if (host.IsEmpty() || remotePath.IsEmpty())
            return out;

        if (extraHeaders!=0 && extraHeaders[0])
        {
            out.Set("%s %s HTTP/1.1\r\n"
                "%s\r\n"
                "Host: %s\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %u\r\n"
                "\r\n"
                "%s",
                type,
                remotePath.C_String(),
                extraHeaders,
                host.C_String(),
                contentType,
                //bodyEncoded.GetLength(),
                //bodyEncoded.C_String());
                strlen(body),
                body);
        }
        else
        {
            out.Set("%s %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %u\r\n"
                "\r\n"
                "%s",
                type,
                remotePath.C_String(),
                host.C_String(),
                contentType,
                //bodyEncoded.GetLength(),
                //bodyEncoded.C_String());
                strlen(body),
                body);
        }

        return out;
    }
    //------------------------------------------------------------------------
    RakString RakString::FormatForPOST(const char* uri, const char* contentType, const char* body, const char* extraHeaders)
    {
        return FormatForPUTOrPost("POST", uri, contentType, body, extraHeaders);
    }
    //------------------------------------------------------------------------
    RakString RakString::FormatForPUT(const char* uri, const char* contentType, const char* body, const char* extraHeaders)
    {
        return FormatForPUTOrPost("PUT", uri, contentType, body, extraHeaders);
    }
    //------------------------------------------------------------------------
    RakString RakString::FormatForGET(const char* uri, const char* extraHeaders)
    {
        RakString out;
        RakString host;
        RakString remotePath;
        RakString header;
        RakString uriRs;
        uriRs = uri;

        uriRs.SplitURI(header, host, remotePath);
        if (host.IsEmpty() || remotePath.IsEmpty())
            return out;

        if (extraHeaders && extraHeaders[0])
        {
            out.Set("GET %s HTTP/1.1\r\n"
                "%s\r\n"
                "Host: %s\r\n"
                "\r\n",
                remotePath.C_String(),
                extraHeaders,
                host.C_String());
        }
        else
        {
            out.Set("GET %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "\r\n",
                remotePath.C_String(),
                host.C_String());

        }
        return out;
    }
    //------------------------------------------------------------------------
    RakString RakString::FormatForDELETE(const char* uri, const char* extraHeaders)
    {
        RakString out;
        RakString host;
        RakString remotePath;
        RakString header;
        RakString uriRs;
        uriRs = uri;

        uriRs.SplitURI(header, host, remotePath);
        if (host.IsEmpty() || remotePath.IsEmpty())
            return out;

        if (extraHeaders && extraHeaders[0])
        {
            out.Set("DELETE %s HTTP/1.1\r\n"
                "%s\r\n"
                "Content-Length: 0\r\n"
                "Host: %s\r\n"
                "Connection: close\r\n"
                "\r\n",
                remotePath.C_String(),
                extraHeaders,
                host.C_String());
        }
        else
        {
            out.Set("DELETE %s HTTP/1.1\r\n"
                "Content-Length: 0\r\n"
                "Host: %s\r\n"
                "Connection: close\r\n"
                "\r\n",
                remotePath.C_String(),
                host.C_String());
        }

        return out;
    }
    //------------------------------------------------------------------------
    RakString& RakString::MakeFilePath(void)
    {
        if (IsEmpty())
            return *this;

        RakString fixedString = *this;
        fixedString.Clone();
        for (int i=0; fixedString.sharedString->c_str[i]; i++)
        {
    #if N_PLAT == N_PLAT_WIN32
            if (fixedString.sharedString->c_str[i]=='/')
                fixedString.sharedString->c_str[i]='\\';
    #else
            if (fixedString.sharedString->c_str[i]=='\\')
                fixedString.sharedString->c_str[i]='/';
    #endif
        }

    #if N_PLAT == N_PLAT_WIN32
        if (fixedString.sharedString->c_str[strlen(fixedString.sharedString->c_str)-1]!='\\')
        {
            fixedString+='\\';
        }
    #else
        if (fixedString.sharedString->c_str[strlen(fixedString.sharedString->c_str)-1]!='/')
        {
            fixedString+='/';
        }
    #endif

        if (fixedString!=*this)
            *this = fixedString;
        return *this;
    }
    //------------------------------------------------------------------------
    void RakString::FreeMemory(void)
    {
        LockMutex();
        FreeMemoryNoMutex();
        UnlockMutex();
    }
    //------------------------------------------------------------------------
    void RakString::FreeMemoryNoMutex(void)
    {
        for(Nui32 i = 0; i < freeList.Size(); ++i)
        {
            N_delete freeList[i]->refCountMutex;
            N_free(freeList[i]);
        }
        freeList.Clear(false);
    }
    //------------------------------------------------------------------------
    void RakString::export(NetSerializer * out) const
    {
        export(sharedString->c_str, out);
    }
    //------------------------------------------------------------------------
    void RakString::export(const char * in, NetSerializer * out)
    {
        Nui16 l = (Nui16) strlen(in);
        out->write(l);
        out->write((const Nui8 *)in, (const Nui32)l);
    }
    //------------------------------------------------------------------------
    void RakString::SerializeCompressed(NetSerializer * bs, Nui8 languageId,
        bool writeLanguageId) const
    {
        SerializeCompressed(C_String(), bs, languageId, writeLanguageId);
    }
    //------------------------------------------------------------------------
    void RakString::SerializeCompressed(const char * str, NetSerializer * bs,
        Nui8 languageId, bool writeLanguageId)
    {
        if(writeLanguageId)
            bs->writeCompress(languageId);
        StringCompressor::Instance()->EncodeString(str,0xFFFF,bs,languageId);
    }
    //------------------------------------------------------------------------
    bool RakString::import(NetSerializer * in)
    {
        Clear();

        bool b;
        Nui16 l;
        b = in->read(l);
        if(l > 0)
        {
            Allocate(((Nui32) l)+1);
            b = in->read((Nui8 *)sharedString->c_str, l);
            if(b)
                sharedString->c_str[l]=0;
            else
                Clear();
        }
        else
            in->readAlign();
        return b;
    }
    //------------------------------------------------------------------------
    bool RakString::import(char * out, NetSerializer * in)
    {
        bool b;
        Nui16 l;
        b = in->read(l);
        if (b && l>0)
            b = in->read((Nui8 *)out, l);

        if (b == false)
            out[0] = 0;

        out[l] = 0;
        return b;
    }
    //------------------------------------------------------------------------
    bool RakString::DeserializeCompressed(NetSerializer * bs, bool readLanguageId)
    {
        Nui8 languageId;
        if (readLanguageId)
            bs->readCompress(languageId);
        else
            languageId = 0;
        return StringCompressor::Instance()->DecodeString(this, 0xFFFF, bs, languageId);
    }
    //------------------------------------------------------------------------
    bool RakString::DeserializeCompressed(char * str, NetSerializer * bs, bool readLanguageId)
    {
        Nui8 languageId;
        if (readLanguageId)
            bs->readCompress(languageId);
        else
            languageId = 0;
        return StringCompressor::Instance()->DecodeString(str, 0xFFFF, bs, languageId);
    }
    //------------------------------------------------------------------------
    const char * RakString::ToString(Ni64 i)
    {
        static int index=0;
        static char buff[64][64];
    #if N_PLAT == N_PLAT_WIN32
        sprintf(buff[index], "%I64d", i);
    #else
        sprintf(buff[index], "%lld", (Nui64) i);
    #endif
        int lastIndex=index;
        if (++index==64)
            index=0;
        return buff[lastIndex];
    }
    //------------------------------------------------------------------------
    const char * RakString::ToString(Nui64 i)
    {
        static int index=0;
        static char buff[64][64];
    #if N_PLAT == N_PLAT_WIN32
        sprintf(buff[index], "%I64u", i);
    #else
        sprintf(buff[index], "%llu", (Nui64)i);
    #endif
        int lastIndex=index;
        if (++index==64)
            index=0;
        return buff[lastIndex];
    }
    //------------------------------------------------------------------------
    void RakString::Clear(void)
    {
        Free();
    }
    //------------------------------------------------------------------------
    void RakString::Allocate(size_t len)
    {
        RakString::LockMutex();
        if (RakString::freeList.Size()==0)
        {
            Nui32 i;
            for (i=0; i < 128; i++)
            {
                RakString::SharedString * ss;
                ss = (RakString::SharedString *) N_alloc(sizeof(RakString::SharedString));
                ss->refCountMutex = N_new ThreadMutex();
                RakString::freeList.Insert(ss);
            }
        }
        sharedString = RakString::freeList[RakString::freeList.Size()-1];
        RakString::freeList.RemoveAtIndex(RakString::freeList.Size()-1);
        RakString::UnlockMutex();

        const size_t smallStringSize = 128-sizeof(Nui32)-sizeof(size_t)-sizeof(char*)*2;
        sharedString->mRefCount=1;
        if (len <= smallStringSize)
        {
            sharedString->bytesUsed=smallStringSize;
            sharedString->c_str=sharedString->smallString;
        }
        else
        {
            sharedString->bytesUsed = len << 1;
            sharedString->bigString = (char *)N_alloc(sharedString->bytesUsed);
            sharedString->c_str = sharedString->bigString;
        }
    }
    //------------------------------------------------------------------------
    void RakString::Assign(const char *str)
    {
        if (str==0 || str[0]==0)
        {
            sharedString=&emptyString;
            return;
        }

        size_t len = strlen(str)+1;
        Allocate(len);
        memcpy(sharedString->c_str, str, len);
    }
    //------------------------------------------------------------------------
    void RakString::Assign(const char *str, va_list ap)
    {
        if (str==0 || str[0]==0)
        {
            sharedString=&emptyString;
            return;
        }

        char stackBuff[512];
        if (_vsnprintf(stackBuff, 512, str, ap)!=-1
    #if N_PLAT == N_PLAT_WIN32
            // Here Windows will return -1 if the string is too long; Linux just truncates the string.
            && strlen(str) <511
    #endif
            )
        {
            Assign(stackBuff);
            return;
        }
        char *buff=0, *newBuff;
        size_t buffSize=8096;
        while (1)
        {
            newBuff = (char *)N_realloc(buff, buffSize);// 需要修改
            if (newBuff==0)
            {
                if (buff!=0)
                {
                    Assign(buff);
                    N_free(buff);
                }
                else
                {
                    Assign(stackBuff);
                }
                return;
            }
            buff=newBuff;
            if (_vsnprintf(buff, buffSize, str, ap)!=-1)
            {
                Assign(buff);
                N_free(buff);
                return;
            }
            buffSize*=2;
        }
    }
    //------------------------------------------------------------------------
    RakString RakString::Assign(const char *str,size_t pos, size_t n )
    {
        size_t incomingLen=strlen(str);

        Clone();

        if (str==0 || str[0]==0||pos>=incomingLen)
        {
            sharedString=&emptyString;
            return (*this);
        }

        if (pos+n>=incomingLen)
        {
        n=incomingLen-pos;

        }
        const char * tmpStr=&(str[pos]);

        size_t len = n+1;
        Allocate(len);
        memcpy(sharedString->c_str, tmpStr, len);
        sharedString->c_str[n]=0;

        return (*this);
    }
    //------------------------------------------------------------------------
    RakString RakString::NonVariadic(const char *str)
    {
        RakString rs;
        rs=str;
        return rs;
    }
    //------------------------------------------------------------------------
    Nul RakString::ToInteger(const char *str)
    {
        Nul hash = 0;
        int c;

        while ((c = *str++))
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
    }
    //------------------------------------------------------------------------
    Nul RakString::ToInteger(const RakString &rs)
    {
        return RakString::ToInteger(rs.C_String());
    }
    //------------------------------------------------------------------------
    int RakString::ReadIntFromSubstring(const char *str, size_t pos, size_t n)
    {
        char tmp[32];
        if (n >= 32)
            return 0;
        for (size_t i=0; i < n; i++)
            tmp[i]=str[i+pos];
        return atoi(tmp);
    }
    //------------------------------------------------------------------------
    void RakString::AppendBytes(const char *bytes, Nui32 count)
    {
        if (IsEmpty())
        {
            Allocate(count);
            memcpy(sharedString->c_str, bytes, count+1);
            sharedString->c_str[count]=0;
        }
        else
        {
            Clone();
            Nui32 length=(Nui32) GetLength();
            Realloc(sharedString, count+length+1);
            memcpy(sharedString->c_str+length, bytes, count);
            sharedString->c_str[length+count]=0;
        }
    }
    //------------------------------------------------------------------------
    void RakString::Clone(void)
    {
        N_assert(sharedString!=&emptyString);
        if (sharedString==&emptyString)
        {
            return;
        }

        // Empty or solo then no point to cloning
        sharedString->refCountMutex->Lock();
        if (sharedString->mRefCount==1)
        {
            sharedString->refCountMutex->Unlock();
            return;
        }

        sharedString->mRefCount--;
        sharedString->refCountMutex->Unlock();
        Assign(sharedString->c_str);
    }
    //------------------------------------------------------------------------
    void RakString::Free(void)
    {
        if (sharedString==&emptyString)
            return;
        sharedString->refCountMutex->Lock();
        sharedString->mRefCount--;
        if (sharedString->mRefCount==0)
        {
            sharedString->refCountMutex->Unlock();
            const size_t smallStringSize = 128-sizeof(Nui32)-sizeof(size_t)-sizeof(char*)*2;
            if (sharedString->bytesUsed>smallStringSize)
                N_free(sharedString->bigString);

            RakString::LockMutex();
            RakString::freeList.Insert(sharedString);
            RakString::UnlockMutex();

            sharedString=&emptyString;
        }
        else
        {
            sharedString->refCountMutex->Unlock();
        }
        sharedString=&emptyString;
    }
    //------------------------------------------------------------------------
    Nui8 RakString::ToLower(Nui8 c)
    {
        if (c >= 'A' && c <= 'Z')
            return c-'A'+'a';
        return c;
    }
    //------------------------------------------------------------------------
    Nui8 RakString::ToUpper(Nui8 c)
    {
        if (c >= 'a' && c <= 'z')
            return c-'a'+'A';
        return c;
    }
    //------------------------------------------------------------------------
    void RakString::LockMutex(void)
    {
        GetPoolMutex().Lock();
    }
    //------------------------------------------------------------------------
    void RakString::UnlockMutex(void)
    {
        GetPoolMutex().Unlock();
    }
    //------------------------------------------------------------------------
}
}
/*
#include "RakString.h"
#include <string>

using namespace NII;

int main(void)
{
	RakString s3("Hello world");
	RakString s5=s3;

	RakString s1;
	RakString s2('a');

	RakString s4("%i %f", 5, 6.0);

	RakString s6=s3;
	RakString s7=s6;
	RakString s8=s6;
	RakString s9;
	s9=s9;
	RakString s10(s3);
	RakString s11=s10 + s4 + s9 + s2;
	s11+=RakString("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	RakString s12("Test");
	s12+=s11;
	bool b1 = s12==s12;
	s11=s5;
	s12.ToUpper();
	s12.ToLower();
	RakString s13;
	bool b3 = s13.IsEmpty();
	s13.Set("blah %s", s12.C_String());
	bool b4 = s13.IsEmpty();
	size_t i1=s13.GetLength();
	s3.Clear();
	s4.Clear();
	s5.Clear();
	s5.Clear();
	s6.Printf();
	s7.Printf();
	N_printf("\n");

	static const int repeatCount=750;
	DataStructures::List<RakString> rakStringList;
	DataStructures::List<std::string> stdStringList;
	DataStructures::List<char*> referenceStringList;
	char *c;
	Nui32 i;
	TimeDurMS beforeReferenceList, beforeRakString, beforeStdString, afterStdString;

	Nui32 loop;
	for (loop=0; loop<2; loop++)
	{
		beforeReferenceList=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
		{
			c = N_alloc_t(char, 56);
			strcpy(c, "Aalsdkj alsdjf laksdjf ;lasdfj ;lasjfd");
			referenceStringList.Insert(c);
		}
		beforeRakString=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
			rakStringList.Insert("Aalsdkj alsdjf laksdjf ;lasdfj ;lasjfd");
		beforeStdString=N_Engine().getTimer().getMS();

		for (i=0; i < repeatCount; i++)
			stdStringList.Insert("Aalsdkj alsdjf laksdjf ;lasdfj ;lasjfd");
		afterStdString=N_Engine().getTimer().getMS();
		N_printf("Insertion 1 Ref=%i Rak=%i, Std=%i\n", beforeRakString-beforeReferenceList, beforeStdString-beforeRakString, afterStdString-beforeStdString);

		beforeReferenceList=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
		{
			N_free(referenceStringList[0]);
			referenceStringList.RemoveAtIndex(0);
		}
		beforeRakString=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
			rakStringList.RemoveAtIndex(0);
		beforeStdString=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
			stdStringList.RemoveAtIndex(0);
		afterStdString=N_Engine().getTimer().getMS();
		N_printf("RemoveHead Ref=%i Rak=%i, Std=%i\n", beforeRakString-beforeReferenceList, beforeStdString-beforeRakString, afterStdString-beforeStdString);

		beforeReferenceList=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
		{
			c = N_alloc_t(char, 56);
			strcpy(c, "Aalsdkj alsdjf laksdjf ;lasdfj ;lasjfd");
			referenceStringList.Insert(0);
		}
		beforeRakString=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
			rakStringList.Insert("Aalsdkj alsdjf laksdjf ;lasdfj ;lasjfd");
		beforeStdString=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
			stdStringList.Insert("Aalsdkj alsdjf laksdjf ;lasdfj ;lasjfd");
		afterStdString=N_Engine().getTimer().getMS();
		N_printf("Insertion 2 Ref=%i Rak=%i, Std=%i\n", beforeRakString-beforeReferenceList, beforeStdString-beforeRakString, afterStdString-beforeStdString);

		beforeReferenceList=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
		{
			N_free(referenceStringList[referenceStringList.Size()-1]);
			referenceStringList.RemoveAtIndex(referenceStringList.Size()-1);
		}
		beforeRakString=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
			rakStringList.RemoveAtIndex(rakStringList.Size()-1);
		beforeStdString=N_Engine().getTimer().getMS();
		for (i=0; i < repeatCount; i++)
			stdStringList.RemoveAtIndex(stdStringList.Size()-1);
		afterStdString=N_Engine().getTimer().getMS();
		N_printf("RemoveTail Ref=%i Rak=%i, Std=%i\n", beforeRakString-beforeReferenceList, beforeStdString-beforeRakString, afterStdString-beforeStdString);

	}

	printf("Done.");
	char str[128];
	Gets(str, sizeof(str));
	return 1;
}
*/
