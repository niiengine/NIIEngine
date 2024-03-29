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

#ifndef WXCODECOMPLETIONBOXENTRY_H
#define WXCODECOMPLETIONBOXENTRY_H

#include "gsgsPreInclude.h"
#include "gsgsTag.h"
#include <wx/clntdata.h>
#include <wx/gdicmn.h>
#include <wx/bitmap.h>
#include "LSP/basic_types.h"

class wxStyledTextCtrl;
namespace gsgs
{
    class _gsgsAPI wxCodeCompletionBoxEntry
    {
        friend class CodeBox;
        wxString m_text;
        wxString m_comment;
        wxString m_insertText;
        LSP::Range m_insertRage;
        int m_imgIndex;
        wxClientData* m_clientData;
        wxRect m_itemRect;
        TagEntryPtr m_tag; // Internal
        int m_weight;
        wxBitmap m_alternateBitmap;
        bool m_isFunction = false;
        bool m_isTemplateFunction = false;
        wxString m_signature; // when IsFunction() is true

    public:
        typedef wxSharedPtr<wxCodeCompletionBoxEntry> Ptr_t;
        typedef std::vector<wxCodeCompletionBoxEntry::Ptr_t> Vec_t;

    public:
        wxCodeCompletionBoxEntry(const wxString& text, int imgId = wxNOT_FOUND, wxClientData* userData = NULL)
            : m_text(text)
            , m_insertText(text)
            , m_imgIndex(imgId)
            , m_clientData(userData)
            , m_weight(0)
        {
        }

        wxCodeCompletionBoxEntry(const wxString& text, const wxBitmap& bmp, wxClientData* userData = NULL)
            : m_text(text)
            , m_insertText(text)
            , m_imgIndex(wxNOT_FOUND)
            , m_clientData(userData)
            , m_weight(0)
            , m_alternateBitmap(bmp)
        {
        }

        wxCodeCompletionBoxEntry(const wxString& text, const wxBitmap& bmp, const wxString& helpText,
            wxClientData* userData = NULL)
            : m_text(text)
            , m_comment(helpText)
            , m_insertText(text)
            , m_imgIndex(wxNOT_FOUND)
            , m_clientData(userData)
            , m_weight(0)
            , m_alternateBitmap(bmp)
        {
        }

        virtual ~wxCodeCompletionBoxEntry()
        {
            wxDELETE(m_clientData);
            m_imgIndex = wxNOT_FOUND;
            m_text.Clear();
        }

        void SetInsertRange(const LSP::Range& insertRage) { this->m_insertRage = insertRage; }
        void SetInsertText(const wxString& insertText) { this->m_insertText = insertText; }
        const LSP::Range& GetInsertRange() const { return m_insertRage; }
        const wxString& GetInsertText() const { return m_insertText; }

        void SetIsFunction(bool isFunction) { this->m_isFunction = isFunction; }
        bool IsFunction() const { return m_isFunction; }

        void SetIsTemplateFunction(bool isTemplateFunction) { this->m_isTemplateFunction = isTemplateFunction; }
        bool IsTemplateFunction() const { return m_isTemplateFunction; }
        void SetSignature(const wxString& signature) { this->m_signature = signature; }
        const wxString& GetSignature() const { return m_signature; }

        /**
         * @brief return the associated tag (might be null)
         */
        TagEntryPtr GetTag() const { return m_tag; }

        /**
         * @brief helper method for allocating wxCodeCompletionBoxEntry::Ptr
         */
        static wxCodeCompletionBoxEntry::Ptr_t New(const wxString& text, int imgId = wxNOT_FOUND,
            wxClientData* userData = NULL)
        {
            wxCodeCompletionBoxEntry::Ptr_t pEntry(new wxCodeCompletionBoxEntry(text, imgId, userData));
            return pEntry;
        }

        /**
         * @brief helper method for allocating wxCodeCompletionBoxEntry::Ptr
         */
        static wxCodeCompletionBoxEntry::Ptr_t New(const wxString& text, const wxBitmap& bmp, wxClientData* userData = NULL)
        {
            wxCodeCompletionBoxEntry::Ptr_t pEntry(new wxCodeCompletionBoxEntry(text, bmp, userData));
            return pEntry;
        }

        /**
         * @brief helper method for allocating wxCodeCompletionBoxEntry::Ptr
         */
        static wxCodeCompletionBoxEntry::Ptr_t New(const wxString& text, const wxString& helpText, const wxBitmap& bmp,
            wxClientData* userData = NULL)
        {
            wxCodeCompletionBoxEntry::Ptr_t pEntry(new wxCodeCompletionBoxEntry(text, bmp, helpText, userData));
            return pEntry;
        }

        void SetWeight(int weight) { this->m_weight = weight; }
        int GetWeight() const { return m_weight; }
        void SetImgIndex(int imgIndex) { this->m_imgIndex = imgIndex; }
        void SetText(const wxString& text) { this->m_text = text; }
        int GetImgIndex() const { return m_imgIndex; }
        const wxString& GetText() const { return m_text; }
        /**
         * @brief set client data, deleting the old client data
         * @param clientData
         */
        void SetClientData(wxClientData* clientData)
        {
            wxDELETE(this->m_clientData);
            this->m_clientData = clientData;
        }
        wxClientData* GetClientData() { return m_clientData; }
        void SetComment(const wxString& comment) { this->m_comment = comment; }
        const wxString& GetComment() const { return m_comment; }
        void SetAlternateBitmap(const wxBitmap& alternateBitmap) { this->m_alternateBitmap = alternateBitmap; }
        const wxBitmap& GetAlternateBitmap() const { return m_alternateBitmap; }
    };
}
#endif
