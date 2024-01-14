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
#ifndef LEXER_CONFIGURATION_H
#define LEXER_CONFIGURATION_H

#include "gsgsPreInclude.h"
#include "gsgsSharedPtr.h"
#include "gsgsJSON.h"
#include "wx/xml/xml.h"
#include <wx/stc/stc.h>

// Set default font size per-OS
#if defined(__WXGTK__)
#define FONT_SIZE 12
#elif defined(__WXMSW__)
#define FONT_SIZE 12
#else
#define FONT_SIZE 14
#endif

// Special attributes IDs
#define FOLD_MARGIN_ATTR_ID -1
#define SEL_TEXT_ATTR_ID -2
#define CARET_ATTR_ID -3
#define WHITE_SPACE_ATTR_ID -4
#define LINE_NUMBERS_ATTR_ID 33
#define STYLE_PROPERTY_NULL_ID -999

namespace gsgs
{
    class _gsgsAPI StyleProperty
    {
    public:
        enum eStyleFlags {
            kNone = 0,
            kItalic = (1 << 0),
            kBold = (1 << 1),
            kUnderline = (1 << 2),
            kEolFilled = (1 << 3),
        };

        typedef std::map<long, StyleProperty> Map_t;
        struct FindByName {
            wxString m_name;
            FindByName(const wxString& name)
                : m_name(name)
            {
            }
            bool operator()(const std::pair<long, StyleProperty>& other) const { return m_name == other.second.GetName(); }
        };

        struct FindByID {
            int m_id;

            FindByID(int id)
                : m_id(id) {}

            bool operator()(const std::pair<long, StyleProperty>& other) const { return m_id == other.first; }
        };

    public:
        StyleProperty(int id, const wxString & fgColour, const wxString & bgColour,
            const int fontSize, const wxString & name, const wxString & face, bool bold,
            bool italic, bool underline, bool eolFilled, int alpha);

        StyleProperty();
        StyleProperty(const StyleProperty & o) { *this = o; };
        StyleProperty & operator=(const StyleProperty & o);
        ~StyleProperty() {}

        void FromJSON(JSONObject json);

        JSONObject ToJSON(bool portable = false) const;

        inline bool IsNull() const { return m_id == STYLE_PROPERTY_NULL_ID; }

        inline bool IsBold() const { return HasFlag(kBold); }

        inline void SetAlpha(int alpha) { m_alpha = alpha; }

        inline int GetAlpha() const { return m_alpha; }

        const wxString & GetFgColour() const { return m_fgColour; }
        const wxString & GetBgColour() const { return m_bgColour; }

        inline void SetEolFilled(bool eolFilled) { EnableFlag(kEolFilled, eolFilled); }
        inline bool GetEolFilled() const { return HasFlag(kEolFilled); }
        inline long GetFontSize() const
        {
            if (m_fontSize <= 0) return FONT_SIZE;
            return m_fontSize;
        }

        inline void SetFaceName(const wxString& face) { m_faceName = face; }
        inline const wxString& GetFaceName() const { return m_faceName; }
        inline void SetName(const wxString& name) { m_name = name; }
        inline const wxString& GetName() const { return m_name; }
        inline int GetId() const { return m_id; }
        inline void SetBgColour(const wxString& colour) { m_bgColour = colour; }
        inline void SetFgColour(const wxString& colour) { m_fgColour = colour; }
        inline void SetFontSize(long size) { m_fontSize = size; }
        inline void SetBold(bool bold) { EnableFlag(kBold, bold); }
        inline void SetId(int id) { m_id = id; }
        inline void SetItalic(bool italic) { EnableFlag(kItalic, italic); }
        inline bool GetItalic() const { return HasFlag(kItalic); }
        inline void SetUnderlined(bool underlined) { EnableFlag(kUnderline, underlined); }
        inline bool GetUnderlined() const { return HasFlag(kUnderline); }
    protected:
        inline void EnableFlag(eStyleFlags flag, bool b)
        {
            if (b) {
                m_flags |= flag;
            }
            else {
                m_flags &= ~flag;
            }
        }

        inline bool HasFlag(eStyleFlags flag) const { return m_flags & flag; }
    private:
        int m_id;
        int m_fontSize;
        int m_alpha;
        wxString m_fgColour;
        wxString m_bgColour;
        wxString m_name;
        wxString m_faceName;
        size_t m_flags;
    };

#define ANNOTATION_STYLE_WARNING 210
#define ANNOTATION_STYLE_ERROR 211
#define ANNOTATION_STYLE_CC_ERROR 212

    class _gsgsAPI LexerStyle
    {
    public:
        typedef SharedPtr<LexerStyle> Ptr_t;

        struct FindByNameAndTheme {
            wxString m_name;
            wxString m_theme;
            FindByNameAndTheme(const wxString& name, const wxString& theme)
                : m_name(name)
                , m_theme(theme)
            {
            }

            bool operator()(LexerStyle::Ptr_t lexer) const
            {
                return lexer->GetName() == m_name && lexer->GetThemeName() == m_theme;
            }
        };
    public:
        LexerStyle();
        virtual ~LexerStyle();

        void SetUseCustomTextSelectionFgColour(bool b) { EnableFlag(kUseCustomTextSelectionFgColour, b); }
        bool IsUseCustomTextSelectionFgColour() const { return HasFlag(kUseCustomTextSelectionFgColour); }
        void SetStyleWithinPreProcessor(bool b) { EnableFlag(kStyleInPP, b); }
        bool GetStyleWithinPreProcessor() const { return HasFlag(kStyleInPP); }
        void SetIsActive(bool b) { EnableFlag(kIsActive, b); }
        bool IsActive() const { return HasFlag(kIsActive); }

        void SetThemeName(const wxString& themeName) { this->m_themeName = themeName; }
        const wxString& GetThemeName() const { return m_themeName; }

        /**
         * @brief return true if the colours represented by this lexer are a "dark" theme
         */
        bool IsDark() const;
        /**
         * @brief apply the current lexer configuration on an input
         * wxStyledTextCtrl
         */
        void Apply(wxStyledTextCtrl* ctrl, bool applyKeywords = false);

        /**
         * Get the lexer ID, which should be in sync with values of Scintilla
         * \return
         */
        int GetLexerId() const { return m_lexerId; }

        /**
         * Set the lexer ID
         * \param id
         */
        void SetLexerId(int id) { m_lexerId = id; }

        /**
         * Return the lexer description as described in the XML file
         */
        const wxString& GetName() const { return m_name; }

        void SetName(const wxString& name) { m_name = name; }
        /**
         * Return the lexer keywords
         * \return
         */
        const wxString& GetKeyWords(int set) const { return m_keyWords[set]; }

        void SetKeyWords(const wxString& keywords, int set);

        /**
         * File patterns that this lexer should apply to
         */
        const wxString& GetFileSpec() const { return m_extension; }
        /**
         * Return a list of the lexer properties
         * \return
         */
        const StyleProperty::Map_t& GetLexerProperties() const { return m_properties; }

        /**
         * Return a list of the lexer properties
         * \return
         */
        StyleProperty::Map_t& GetLexerProperties() { return m_properties; }

        /**
         * @brief return property. Check for IsNull() to make sure we got a valid property
         * @param propertyId
         * @return
         */
        StyleProperty& GetProperty(int propertyId);

        const StyleProperty& GetProperty(int propertyId) const;

        /**
         * @brief set the line numbers colour
         */
        void SetLineNumbersFgColour(const wxColour& colour);

        /**
         * @brief set the default fg colour
         */
        void SetDefaultFgColour(const wxColour& colour);

        /**
         * Set the lexer properties
         * \param &properties
         */
        void SetProperties(StyleProperty::Map_t& properties) { m_properties.swap(properties); }
        /**
         * Set file spec for the lexer
         * \param &spec
         */
        void SetFileSpec(const wxString& spec) { m_extension = spec; }

        /**
         * @brief return the font for a given style id
         * @return return wxNullFont if error occurred or could locate the style
         */
        wxFont GetFontForSyle(int styleId) const;

        // Return an xml representation from this object
        wxXmlNode* ToXml() const;

        // Parse lexer object from xml node
        void FromXml(wxXmlNode* node);

        /**
         * @brief convert the lexer settings into a JSON object
         */
        JSONObject ToJSON(bool forExport = false) const;

        /**
         * @brief construt this object from a JSON object
         * @param json
         */
        void FromJSON(const JSONObject& json);
    protected:
        enum eLexerConfFlags {
            kNone = 0,
            kStyleInPP = (1 << 0),
            kIsActive = (1 << 1),
            kUseCustomTextSelectionFgColour = (1 << 2),
        };

        inline void EnableFlag(eLexerConfFlags flag, bool b)
        {
            if (b) {
                m_flags |= flag;
            }
            else {
                m_flags &= ~flag;
            }
        }

        inline bool HasFlag(eLexerConfFlags flag) const { return m_flags & flag; }
    private:
        int m_lexerId;
        StyleProperty::Map_t m_properties;
        wxString m_name;
        wxString m_extension;
        wxString m_keyWords[10];
        wxString m_themeName;
        size_t m_flags;
    };
}
#endif
