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
#ifndef SFTPGREP_H
#define SFTPGREP_H

#include "UI.h"

class GrepData
{
public:
    GrepData() {}
    ~GrepData() {}

    void SetFindWhat(const wxString& findWhat)      { this->m_findWhat = findWhat; }
    void SetIgnoreCase(bool ignoreCase)             { this->m_ignoreCase = ignoreCase; }
    void SetSearchIn(const wxString& searchIn)      { this->m_searchIn = searchIn; }
    void SetWholeWord(bool wholeWord)               { this->m_wholeWord = wholeWord; }
    const wxString& GetFindWhat() const             { return m_findWhat; }
    bool IsIgnoreCase() const                       { return m_ignoreCase; }
    const wxString& GetSearchIn() const             { return m_searchIn; }
    bool IsWholeWord() const                        { return m_wholeWord; }
    
    wxString GetGrepCommand(const wxString& path) const;
private:
    wxString m_findWhat;
    wxString m_searchIn;
    bool m_ignoreCase = false;
    bool m_wholeWord = true;
};

class SFTPGrep : public SFTPGrepBase
{
public:
    SFTPGrep(wxWindow* parent);
    virtual ~SFTPGrep();
    GrepData GetData() const;

protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
};
#endif // SFTPGREP_H
