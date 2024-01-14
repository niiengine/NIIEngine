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

#ifndef CXXVARIABLE_H
#define CXXVARIABLE_H

#include "gsgsPreInclude.h"
#include "CxxLexerAPI.h"
#include "gsgsSharedPtr.h"

namespace gsgs
{
    class _gsgsAPI CxxVariable
    {
    public:
        struct LexerToken {
            int type;
            int _depth;
            wxString text;
            wxString comment;

            LexerToken()
                : type(0)
                , _depth(0)
            {
            }

            LexerToken(const CxxLexerToken& token, int depth)
            {
                FromCxxLexerToken(token);
                this->_depth = depth;
            }

            int GetType() const { return type; }
            void FromCxxLexerToken(const CxxLexerToken& token)
            {
                this->type = token.GetType();
                this->comment = token.GetWXComment();
                this->text = token.GetWXString();
            }
            typedef std::vector<CxxVariable::LexerToken> Vec_t;
        };

        enum eFlags {
            kToString_None = 0,
            // Include the variable name
            kToString_Name = (1 << 0),
            // Include the default value
            kToString_DefaultValue = (1 << 1),
            kToString_Default = kToString_Name,
            // Revert back type->macro (e.g. wxWindowMSW -> wxWindow)
            kToString_ReverseMacros = (1 << 2),
        };

    protected:
        wxString m_name;
        CxxVariable::LexerToken::Vec_t m_type;
        wxString m_defaultValue;
        eCxxStandard m_standard;
        wxString m_pointerOrReference;
        bool m_isAuto;

    public:
        typedef SharedPtr<CxxVariable> Ptr_t;
        typedef std::vector<CxxVariable::Ptr_t> Vec_t;
        typedef std::unordered_map<wxString, CxxVariable::Ptr_t> Map_t;

    public:
        CxxVariable(eCxxStandard standard);
        CxxVariable()
            : m_standard(eCxxStandard::kCxx11)
            , m_isAuto(false)
        {
        }

        virtual ~CxxVariable();

        void SetName(const wxString& name) { this->m_name = name; }
        void SetType(const CxxVariable::LexerToken::Vec_t& type) { this->m_type = type; }
        const wxString& GetName() const { return m_name; }
        const CxxVariable::LexerToken::Vec_t& GetType() const { return m_type; }
        // @see ToString() for description about 'table'
        wxString GetTypeAsString(const gsgsStringMap& table = gsgsStringMap()) const;
        // @see ToString() for description about 'table'
        wxString GetTypeAsCxxString(const gsgsStringMap& table = gsgsStringMap()) const;

        static wxString PackType(const CxxVariable::LexerToken::Vec_t& type, eCxxStandard standard,
                                 bool omitClassKeyword = false, const gsgsStringMap& table = gsgsStringMap());

        /**
         * @brief return true if this variable was constructed from a statement like:
         * using MyInt = int;
         */
        bool IsUsing() const { return GetTypeAsString() == "using"; }

        /**
         * @brief is this a valid variable?
         */
        bool IsOk() const { return !m_name.IsEmpty() && !m_type.empty(); }

        /**
         * @brief is this an "auto" variable?
         */
        bool IsAuto() const { return m_isAuto; }

        void SetIsAuto(bool b) { m_isAuto = b; }
        /**
         * @brief return a string representation for this variable
         * @param flags see values in eFlags
         * @param table macros table - reversed. i.e. the actual type is the key and the value is the macro name
         * this table is used when the flag kToString_ReverseMacros is passed
         */
        wxString ToString(size_t flags = CxxVariable::kToString_Default,
                          const gsgsStringMap& table = gsgsStringMap()) const;

        void SetDefaultValue(const wxString& defaultValue) { this->m_defaultValue = defaultValue; }
        const wxString& GetDefaultValue() const { return m_defaultValue; }

        void SetPointerOrReference(const wxString& pointerOrReference) { this->m_pointerOrReference = pointerOrReference; }
        const wxString& GetPointerOrReference() const { return m_pointerOrReference; }
    };
}
#endif