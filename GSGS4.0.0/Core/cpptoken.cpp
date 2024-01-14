
#include "cpptoken.h"
#include <wx/crt.h>
#include <wx/progdlg.h>
#include "cppwordscanner.h"
#include "gsgsProgressDialog.h"
#include "gsgsWorkspace.h"

namespace gsgs
{
    //-----------------------------------------------------------------
    CppToken::CppToken() { reset(); }
    //-----------------------------------------------------------------
    CppToken::~CppToken() {}
    //-----------------------------------------------------------------
    bool CppToken::operator<(const CppToken& rhs) const
    {
        return this->getName() < rhs.getName();
    }
    //-----------------------------------------------------------------
    void CppToken::reset()
    {
        name.clear();
        offset = wxString::npos;
        m_id = wxNOT_FOUND;
        lineNumber = wxString::npos;
        filename.clear();
    }
    //-----------------------------------------------------------------
    void CppToken::append(wxChar ch) { name << ch; }
    //-----------------------------------------------------------------
    void CppToken::print() { wxPrintf(wxT("%s | %ld\n"), name.c_str(), offset); }
    //-----------------------------------------------------------------
    int CppToken::store(wxSQLite3Database* db, wxLongLong fileID) const
    {
        try {

            wxSQLite3Statement st = db->PrepareStatement(
                "REPLACE INTO TOKENS_TABLE (ID, NAME, OFFSET, FILE_ID, LINE_NUMBER) VALUES(NULL, ?, ?, ?, ?)");
            st.Bind(1, getName());
            st.Bind(2, (int)getOffset());
            st.Bind(3, fileID);
            st.Bind(4, (int)getLineNumber());
            st.ExecuteUpdate();
            return db->GetLastRowId().ToLong();

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
        return wxNOT_FOUND;
    }
    //-----------------------------------------------------------------
    CppToken::Vec_t CppToken::loadByNameAndFile(wxSQLite3Database* db, const wxString& name, wxLongLong fileID)
    {
        CppToken::Vec_t matches;
        try {
            wxSQLite3Statement st = db->PrepareStatement("select * from TOKENS_TABLE where FILE_ID=? AND NAME=?");
            st.Bind(1, fileID);
            st.Bind(2, name);
            wxSQLite3ResultSet res = st.ExecuteQuery();
            while(res.NextRow()) {
                CppToken token(res);
                matches.push_back(token);
            }

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
        return matches;
    }
    //-----------------------------------------------------------------
    CppToken::CppToken(wxSQLite3ResultSet& res)
    {
        setId(res.GetInt(0));
        setName(res.GetString(1));
        setOffset(res.GetInt(2));
        setLineNumber(res.GetInt(4));
    }
    //-----------------------------------------------------------------
    CppToken::Vec_t CppToken::loadByName(wxSQLite3Database* db, const wxString& name)
    {
        CppToken::Vec_t matches;
        std::unordered_map<size_t, wxString> fileIdToFile;
        try {
            wxSQLite3Statement st = db->PrepareStatement("select * from TOKENS_TABLE where NAME=?");
            st.Bind(1, name);
            wxSQLite3ResultSet res = st.ExecuteQuery();
            while(res.NextRow()) {
                CppToken token(res);
                size_t fileID = res.GetInt64(3).ToLong();
                if(fileIdToFile.count(fileID)) {
                    token.setFilename(fileIdToFile.find(fileID)->second);
                } else {
                    // load from the db
                    wxSQLite3Statement st1 = db->PrepareStatement("SELECT FILE_NAME FROM FILES WHERE ID=? LIMIT 1");
                    st1.Bind(1, wxLongLong(fileID));
                    wxSQLite3ResultSet res1 = st1.ExecuteQuery();
                    if(res1.NextRow()) {
                        token.setFilename(res1.GetString(0));
                        fileIdToFile.insert(std::make_pair(fileID, token.getFilename()));
                    }
                }
                matches.push_back(token);
            }

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
        return matches;
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // CppTokensMap
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    CppTokensMap::CppTokensMap() {}
    //-----------------------------------------------------------------
    CppTokensMap::~CppTokensMap() { clear(); }
    //-----------------------------------------------------------------
    void CppTokensMap::addToken(const wxString& name, const CppToken::Vec_t& list)
    {
        // try to locate an entry with this name
        std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.find(name);
        std::vector<CppToken>* tokensList(NULL);
        if(iter != m_tokens.end()) {
            tokensList = iter->second;
        } else {
            // create new list and add it to the map
            tokensList = new std::vector<CppToken>;
            m_tokens.insert(std::make_pair(name, tokensList));
        }
        tokensList->insert(tokensList->end(), list.begin(), list.end());
    }
    //-----------------------------------------------------------------
    void CppTokensMap::addToken(const CppToken& token)
    {
        // try to locate an entry with this name
        std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.find(token.getName());
        std::vector<CppToken>* tokensList(NULL);
        if(iter != m_tokens.end()) {
            tokensList = iter->second;
        } else {
            // create new list and add it to the map
            tokensList = new std::vector<CppToken>;
            m_tokens[token.getName()] = tokensList;
        }
        tokensList->push_back(token);
    }
    //-----------------------------------------------------------------
    bool CppTokensMap::contains(const wxString& name)
    {
        std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.find(name);
        return iter != m_tokens.end();
    }
    //-----------------------------------------------------------------
    void CppTokensMap::findTokens(const wxString& name, std::vector<CppToken>& tokens)
    {
        std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.find(name);
        //	std::vector<CppToken> *tokensList(NULL);
        if(iter != m_tokens.end()) { tokens = *(iter->second); }
    }
    //-----------------------------------------------------------------
    void CppTokensMap::clear()
    {
        std::unordered_map<wxString, std::vector<CppToken>*>::iterator iter = m_tokens.begin();
        for(; iter != m_tokens.end(); ++iter) {
            delete iter->second;
        }
        m_tokens.clear();
    }
    //-----------------------------------------------------------------
    bool CppTokensMap::is_empty() { return m_tokens.empty(); }
    //-----------------------------------------------------------------
    //#define POST_NEW_STATUS(msg, value, act)
    //	status = new RefactorIndexBuildJobInfo;
    //	status->filename = msg;
    //	status->status = value;
    //	status->action = act;
    //	Post(status);
    //-----------------------------------------------------------------
    RefactorIndexBuildJob::RefactorIndexBuildJob(const std::vector<wxFileName> &files, const wxChar *word)
            : m_files( files )
            , m_word(word)
    {
    }
    //-----------------------------------------------------------------
    RefactorIndexBuildJob::~RefactorIndexBuildJob()
    {
    }
    //-----------------------------------------------------------------
    void RefactorIndexBuildJob::Parse(const wxString &word, gsgs::CppTokensMap &l)
    {
        gsgs::ProgressDlg* prgDlg = NULL;
        // Create a progress dialog
        prgDlg = new gsgs::ProgressDlg (NULL, _("Gathering required information..."), wxT(""), (int)m_files.size());
        prgDlg->Update(0, _("Gathering required information..."));

        size_t i=0;
        for ( ; i<m_files.size(); i++) {

            wxFileName fn = m_files.at(i);
            CppWordScanner scanner(fn.GetFullPath().mb_str().data());

            wxString msg;
            msg << _("Parsing: ") << fn.GetFullName();
            // update the progress bar
            if (!prgDlg->Update(i, msg)){
                prgDlg->Destroy();
                l.clear();
                return;
            }

            scanner.Match(word.mb_str().data(), l);
        }
        prgDlg->Destroy();
    }
    //-----------------------------------------------------------------
    void RefactorIndexBuildJob::Process(wxThread* thread)
    {
        CppTokensMap l;
        Parse(m_word.c_str(), l);
    }
    //-----------------------------------------------------------------
}