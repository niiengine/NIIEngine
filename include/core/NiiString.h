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

#ifndef _NII_String_H_
#define _NII_String_H_

#include "NiiPreInclude.h"
#include "NiiUTFString.h"

#if N_COMPILER == N_CPP_GNUC && N_COMPILER_VER >= 310 && !defined(STLPORT)
    #if N_COMPILER_VER >= 430
        #include <tr1/unordered_map>
    #else
        #include <ext/hash_map>
    namespace __gnu_cxx
    {
        template <> struct hash<NII::String>
        {
            NCount operator()(const NII::String & o) const
            {
                /* This is the PRO-STL way, but it seems to cause problems with VC7.1
                   and in some other cases (although I can't recreate it)
                hash<const NIIb*> H;
                return H(o.c_str());
                */
                register NCount ret = 0;
                NII::String::const_iterator it, itend = o.end()
                for(it = o.begin(); it != itend; ++it)
                    ret = 5 * ret + *it;

                return ret;
            }
        };
    }
    #endif
#endif

#if N_UNICODE_SUPPORT
    typedef ThirdPart::UTFString    ViewString;
    #define N_Font_Char(it)         it.getCharacter()
#else
    typedef NII::String             ViewString;
    #define N_Font_Char(it)         *it
#endif

#if N_PLAT == N_PLAT_WIN32
    #define N_PATH_SEP    ';'
    #define N_DIR_SEP_C   '\\'
    #define N_DIR_SEP_S   _T("\\")
#else
    #define N_PATH_SEP    ':'
    #define N_DIR_SEP_C   '/'
    #define N_DIR_SEP_S   _T("/")
#endif

namespace NII
{
    /** 字符串辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StrUtil
    {
    public:
        /** 转换字符串为小写字母
        @version NIIEngine 3.0.0
        */
        static void toLower(WString & str);

        /** 转换字符串为小写字母
        @version NIIEngine 3.0.0
        */
        static void toLower(VString & str);

        /** 转换字符串为大写字母
        @version NIIEngine 3.0.0
        */
        static void toUpper(WString & str);

        /** 转换字符串为大写字母
        @version NIIEngine 3.0.0
        */
        static void toUpper(VString & str);

        /** 移去任何空白字符,可能为空格/tabs等
        @remark 可以选择移去左边或右边(默认两边都移去)
        @version NIIEngine 3.0.0
        */
        static void trim(WString & str, bool left = true, bool right = true);

        /** 移去任何空白字符,可能为空格/tabs等
        @remark 可以选择移去左边或右边(默认两边都移去)
        @version NIIEngine 3.0.0
        */
        static void trim(VString & str, bool left = true, bool right = true);
        
        /** 移去头部指定字符
        @version NIIEngine 3.0.0
        */
        static void trimLeft(String & str, const String & delims);

        /** 移去尾部指定字符
        @version NIIEngine 3.0.0
        */
        static void trimRight(String & str, const String & delims);

        /** 分割所有由定界符参数的字符串
        @param in 输入字符串
        @param delims 定界符
        @param cnt 分割次数， 默认不限次数
        @param keepDelims 保留定界符
        @return 字符串列表
        @version NIIEngine 3.0.0
        */
        static void split(const WString & in, WStringList & out, const WString & delims = L"\t\n ",
            NCount cnt = 0, bool keepDelims = false);

        /** 分割所有由定界符参数的字符串
        @param in 输入字符串
        @param delims 定界符
        @param cnt 分割次数， 默认不限次数
        @param keepDelims 保留定界符
        @return 字符串列表
        @version NIIEngine 3.0.0
        */
        static void split(const VString & in, VStringList & out, const VString & delims = "\t\n ",
            NCount cnt = 0, bool keepDelims = false);

        /** 合并字符串
        @version NIIEngine 3.0.0
        */
        static void compact(const std::vector<WString> & tokens, WString & out);

        /** 合并字符串
        @version NIIEngine 3.0.0
        */
        static void compact(const std::vector<VString> & tokens, VString & out);

        /** 使用定界符分割字符串(包含定界符)
        @param delims 定界符
        @param enddelims 附加定界符
        @param cnt 分割次数,默认0不限次数
        @version NIIEngine 3.0.0
        */
        static void tokenise(const WString & str, WStringList & out, const WString & delims = L"\t\n ", 
            const WString & enddelims = L"\"", Nui32 cnt = 0);

        /** 使用定界符分割字符串(包含定界符)
        @param delims 定界符
        @param enddelims 附加定界符
        @param cnt 分割次数,默认0不限次数
        @version NIIEngine 3.0.0
        */
        static void tokenise(const VString & str, VStringList & out, const VString & delims = "\t\n ",
            const VString & enddelims = "\"", Nui32 cnt = 0);

        /** 格式化字符串
        @param c 仅支持不大于1024个字符
        @version NIIEngine 4.0.0
        */
        static const WString & format(WString & out, Nwchar * in, ...);
        
        /** 格式化字符串
        @param c 仅支持不大于1024个字符
        @version NIIEngine 4.0.0
        */
        static const VString & format(VString & out, Nchar * in, ...);

        /** 检测字符串是否以指定匹配结尾
        @param[in] match 匹配
        @param[in] lowerCase 把字符串转换成小写字母去匹配
        @version NIIEngine 3.0.0
        */
        static bool beginsWith(const WString & str, const WString & match, bool lowerCase = true);

        /** 检测字符串是否以指定匹配结尾
        @param[in] match 匹配
        @param[in] lowerCase 把字符串转换成小写字母去匹配
        @version NIIEngine 3.0.0
        */
        static bool beginsWith(const VString & str, const VString & match, bool lowerCase = true);

        /** 检测字符串是否以指定匹配结尾
        @param[in] match 匹配
        @param[in] lowerCase 把字符串转换成小写字母去匹配
        @version NIIEngine 3.0.0
        */
        static bool endsWith(const WString & str, const WString & match, bool lowerCase = true);

        /** 检测字符串是否以指定匹配结尾
        @param[in] match 匹配
        @param[in] lowerCase 把字符串转换成小写字母去匹配
        @version NIIEngine 3.0.0
        */
        static bool endsWith(const VString & str, const VString & match, bool lowerCase = true);
        
        /** 获取所在索引的单词的开始(索引)
        @version NIIEngine 3.0.0
        */
        static Nidx getWordBegin(const String & str, Nidx begin);

        /** 获取所在索引的单词的下一个单词的开始(索引)
        @version NIIEngine 3.0.0
        */
        static Nidx getNextWordBegin(const String & str, Nidx begin);
        
        /** 获取所在索引的单词的下一个单词
        @version NIIEngine 3.0.0
        */
        static String getNextWord(const String & str, Nidx begin = 0, const String & delims = Whitespace);

        /** 转换文件路径
        @remark 双斜杠转单斜杠
        @param[in] src 文件路径
        @version NIIEngine 3.0.0
        */
        static void convPath(const WString & src, WString & out);

        /** 转换文件路径
        @remark 双斜杠转单斜杠
        @param[in] src 文件路径
        @version NIIEngine 3.0.0
        */
        static void convPath(const VString & src, VString & out);

        /** 转换文件路径
        @param[in] src 文件路径
        @param[in] lowerCase 转换字符串为小写字母
        */
        static String convPath2(const String & src, bool lowerCase = true);

        /** 分割路径文件名成分
        @param[in] file 完整的文件路径名
        @param[out] name 文件名(含扩展名)
        @param[out] path 路径
        @version NIIEngine 3.0.0
        */
        static void splitFilename(const WString & file, WString & name, WString & path);

        /** 分割路径文件名成分
        @param[in] file 完整的文件路径名
        @param[out] name 文件名(含扩展名)
        @param[out] path 路径
        @version NIIEngine 3.0.0
        */
        static void splitFilename(const VString & file, VString & name, VString & path);

        /** 分割路径成分
        extension and path.
        @param[in] file 完整的文件路径名
        @param[out] nameNoext 文件名(不含扩展名)
        @param[out] fileExt 扩展名
        @param[out] path 路径
        @version NIIEngine 3.0.0
        */
        static void splitFullFilename(const String & file, String & nameNoext, String & fileExt, String & path);

        /** 分割文件名成分
        @param[in] name 文件名(含扩展名)
        @param[out] nameNoext 文件名(不含扩展名)
        @param[out] fileExt 扩展名
        @version NIIEngine 3.0.0
        */
        static void splitBaseFilename(const String & name, String & nameNoext, String & fileExt);

        /** 获取utf8字符串长度(包含分割符\结束符)
        @param[in] strin 输入字符串(必须是utf8编码)
        @param[out] bcnt 实际读取字节数(包含分割符\结束符)
        @param[in] delims 分割符号
        @version NIIEngine 3.0.0
        */
        static NCount getUtf8Length(Nutf8 * strin, NCount * bcnt = NULL, char delims = '\0');

        /** 获取utf8字符串长度(包含分割符\结束符)
        @param[in] strin 输入字符串(必须是utf8编码)
        @param[in] strend 字符串结束位置
        @param[out] bcnt 实际读取字节数(包含分割符\结束符)
        @param[in] delims 分割符号
        @version NIIEngine 3.0.0
        */
        static NCount getUtf8Length(Nutf8 * strin, Nutf8 * strend, NCount * bcnt, Nchar delims = '\0');

        /** 获取utf8字符串长度(包含分割符\结束符)
        @param[in] strin 输入字符串(必须是utf8编码)
        @param[in] strend 字符串结束位置
        @param[out] bcnt 实际读取字节数(包含分割符\结束符)
        @param[out] wcnt wchar_t字节数(包含分割符\结束符)
        @param[in] delims 分割符号
        @version NIIEngine 3.0.0
        */
        static NCount getUtf8Length(Nutf8 * strin, Nutf8 * strend, NCount * bcnt, NCount * wcnt, Nchar delims = '\0');

        /** 获取utf8字符串的子串
        @return 需要使用 N_free 释放
        @version NIIEngine 3.0.0
        */
        static Nutf8 * getUtf8Sub(Nutf8 * strin, NCount b, NCount e);

        /** 通配符匹配字符串
        @param[in] pattern 通配符可包含'*'
        @version NIIEngine 3.0.0
        */
        static bool match(const WString & src, const WString & pattern, bool lowerCase = true);

        /** 通配符匹配字符串
        @param[in] pattern 通配符可包含'*'
        @version NIIEngine 3.0.0
        */
        static bool match(const VString & src, const VString & pattern, bool lowerCase = true);

        /** 替换字符串
        @param[in] dst 需要替换的字符串
        @param[in] target 替换目标字符串
        @version NIIEngine 3.0.0
        */
        static WString replaceAll(const WString & src, const WString & dst, const WString & target);

        /** 替换字符串
        @param[in] dst 需要替换的字符串
        @param[in] target 替换目标字符串
        @version NIIEngine 3.0.0
        */
        static VString replaceAll(const VString & src, const VString & dst, const VString & target);

        static const String     BLANK;
        static const WString    WBLANK;
        static const VString    VBLANK;
        static const String     Whitespace;
        static const String     AlphaAndNum;
        static const String     Delimiter;
    };

    #if N_COMPILER == N_CPP_GNUC && N_COMPILER_VER >= 310 && !defined(STLPORT)
    #if N_COMPILER_VER < 430
        typedef ::__gnu_cxx::hash<String>   _StringHash;
    #else
        typedef ::std::tr1::hash<String>    _StringHash;
    #endif
    #elif N_COMPILER == N_CPP_GCCE
    #if defined(_LIBCPP_VERSION)
        typedef ::std::hash<String>         _StringHash;
    #else
        typedef ::std::tr1::hash<String>    _StringHash;
    #endif
    #elif N_COMPILER == N_CPP_MSVC && N_COMPILER_VER >= 1600 && !defined(STLPORT) // VC++ 10.0
        typedef ::std::tr1::hash<String>    _StringHash;
    #elif !defined(_STLP_HASH_FUN_H)
        typedef stdext::hash_compare<String, std::less<String> > _StringHash;
    #else
        typedef std::hash<String>           _StringHash;
    #endif
}
#endif