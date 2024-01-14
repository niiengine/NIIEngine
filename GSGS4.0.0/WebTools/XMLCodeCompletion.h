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

#ifndef XMLCODECOMPLETION_H
#define XMLCODECOMPLETION_H

#include "gsgsPreInclude.h"
#include "gsgsSharedPtr.h"
#include "gsgsEvent.h"

class WebTools;
class XMLCodeCompletion : public ServiceProvider
{
public:
    typedef SharedPtr<XMLCodeCompletion> Ptr_t;
    struct HtmlCompletion {
        wxString m_tag;
        wxString m_comment;

        HtmlCompletion(const wxString& tag, const wxString& comment)
            : m_tag(tag)
            , m_comment(comment)
        {
        }
        typedef std::vector<HtmlCompletion> Vec_t;
    };
    gsgsStringMap m_completePattern;

protected:
    enum eCompleteReason {
        kNone = -1,
        kHtmlOpenSequence = 0, // User typed "<" in an HTML document
        kCloseSequence,        // user typed "</"
    };

    HtmlCompletion::Vec_t m_htmlCompletions;
    eCompleteReason m_completeReason;
    bool m_xmlCcEnabled;
    bool m_htmlCcEnabeld;
    WebTools* m_plugin = nullptr;
    
protected:
    void PrepareHtmlCompletions();
    wxString GetCompletePattern(const wxString& tag) const;
    /**
     * @brief Does 'tag' has a sepcial insert pattern?
     * an example is the <a>: "<a href=""></a>"
     */
    bool HasSpecialInsertPattern(const wxString& tag) const;
    void SuggestClosingTag(IEditor* editor, bool html);
    void OnCodeCompleted(CodeCompleteEvent& event);

    int GetWordStartPos(IEditor* editor);
    void OnCodeComplete(CodeCompleteEvent& event);

public:
    XMLCodeCompletion(WebTools* plugin);
    virtual ~XMLCodeCompletion();

    void XmlCodeComplete(IEditor* editor);
    void HtmlCodeComplete(IEditor* editor);

    void Reload();
};

#endif // XMLCODECOMPLETION_H
