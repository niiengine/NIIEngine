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

#ifndef __RAK_W_STRING_H
#define __RAK_W_STRING_H

#include "NiiNetCommon.h" // Ni64
#include "RakString.h"

namespace NII
{
    /// \brief String class for Unicode
    class _EngineAPI RakWString
    {
    public:
        // Constructors
        RakWString();
        RakWString(const RakString & right);
        RakWString(const wchar_t * input);
        RakWString(const RakWString & right);
        RakWString(const char * input);
        ~RakWString();

        /// Implicit return of wchar_t*
        operator wchar_t* () const {if (c_str) return c_str; return (wchar_t*) L"";}

        /// Same as std::string::c_str
        const wchar_t* C_String(void) const {if (c_str) return c_str; return (const wchar_t*) L"";}

        /// Assignment operators
        RakWString& operator = ( const RakWString& right );
        RakWString& operator = ( const RakString& right );
        RakWString& operator = ( const wchar_t * const str );
        RakWString& operator = ( wchar_t *str );
        RakWString& operator = ( const char * const str );
        RakWString& operator = ( char *str );

        /// Concatenation
        RakWString& operator +=( const RakWString& right);
        RakWString& operator += ( const wchar_t * const right );
        RakWString& operator += ( wchar_t *right );

        /// Equality
        bool operator==(const RakWString &right) const;

        // Comparison
        bool operator < ( const RakWString& right ) const;
        bool operator <= ( const RakWString& right ) const;
        bool operator > ( const RakWString& right ) const;
        bool operator >= ( const RakWString& right ) const;

        /// Inequality
        bool operator!=(const RakWString &right) const;

        /// Set the value of the string
        void Set( wchar_t *str );

        /// Returns if the string is empty. Also, C_String() would return ""
        bool IsEmpty(void) const;

        /// Returns the length of the string
        size_t GetLength(void) const;

        /// Has the string into an Nui32
        static Nul ToInteger(const RakWString &rs);

        /// Compare strings (case sensitive)
        int StrCmp(const RakWString &right) const;

        /// Compare strings (not case sensitive)
        int StrICmp(const RakWString &right) const;

        /// Clear the string
        void Clear(void);

        /// Print the string to the screen
        void Printf(void);

        /// Print the string to a file
        void FPrintf(FILE * fp);

        /// export to a bitstream, uncompressed (slightly faster)
        /// \param[out] bs Bitstream to serialize to
        void export(NetSerializer * out) const;

        /// Static version of the export function
        static void export(const wchar_t * const in, NetSerializer * out);

        /// import what was written by export
        /// \param[in] bs Bitstream to serialize from
        /// \return true if the deserialization was successful
        bool import(NetSerializer * in);

        /// Static version of the import() function
        static bool import(wchar_t * out, NetSerializer * in);
    protected:
        wchar_t * c_str;
        size_t c_strCharLength;
    };
const RakWString _EngineAPI operator+(const RakWString &lhs, const RakWString &rhs);    
}

#endif