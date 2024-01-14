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
#ifndef gsgsSerializer_h
#define gsgsSerializer_h

#include "gsgsPreInclude.h"
#include "gsgsJSON.h"
#include "gsgsSharedPtr.h"
#if wxUSE_GUI
#include <wx/gdicmn.h>
#include <wx/font.h>
#endif

class wxXmlNode;

namespace gsgs
{
    class BreakpointInfo;
    WX_DECLARE_STRING_HASH_MAP(wxString, StringMap);

    class _gsgsAPI ObjectData
    {
    public:
        ObjectData() {}
        ObjectData(const wxString & name) : mName(name) {}
        virtual ~ObjectData() {}

        inline void SetName(const wxString & name)                      { mName = name; }

        inline const wxString & GetName() const                         { return mName; }

        virtual void SetVersion(const wxString & ver)                   { mVersion = ver; }

        virtual const wxString & GetVersion() const                     { return mVersion; }

        virtual void FromXml(wxXmlNode *) const                         {}

        virtual wxXmlNode * ToXml() const                               { return 0; }

        virtual void FromJSON(const JSONObject &)                       {}

        virtual JSONObject ToJSON() const                               { return JSONObject(0); }

        virtual void Serialize(Serializer &)                            {}

        virtual void DeSerialize(Serializer &)                          {}
    private:
        wxString mVersion;
        wxString mName;
    };
    typedef SharedPtr<ObjectData> ConfObjectPtr;

    class _gsgsAPI TabInfoData : public ObjectData
    {
    public:
        TabInfoData();
        virtual ~TabInfoData();

        void Serialize(Serializer & arch);
        void DeSerialize(Serializer & arch);

        inline void SetFileName(const wxString & str)                   { mFileName = str; }
        inline const wxString & GetFileName() const                     { return mFileName; }

        inline void SetFirstVisibleLine(int line)                       { m_firstVisibleLine = line; }
        inline const int GetFirstVisibleLine() const                    { return m_firstVisibleLine; }

        inline void SetCurrentLine(int cLine)                           { m_currentLine = cLine; }
        inline const int GetCurrentLine() const                         { return m_currentLine; }

        inline void SetBookmarks(const wxArrayString & strs)            { m_bookmarks = strs; }
        inline const wxArrayString & GetBookmarks() const               { return m_bookmarks; }

        inline void SetCollapsedFolds(const std::vector<int> & folds)   { m_folds = folds; }
        inline const std::vector<int> & GetCollapsedFolds() const       { return m_folds; }
    private:
        wxString mFileName;
        std::vector<int> m_folds;
        int m_firstVisibleLine;
        int m_currentLine;
        wxArrayString m_bookmarks;
    };
    
    enum SettersGetters 
    {
        SG_KEEP_UNDERSCORES,
        SG_LOWER_CASE_LETTER,
        SG_REMOVE_M_PREFIX
    };

    class _gsgsAPI Serializer
    {
    public:
        Serializer();
        virtual ~Serializer();

        void SetXmlNode(wxXmlNode* node);

        bool Write(const wxString& name, ObjectData* in);
        bool Write(const wxString& name, int in);
        bool Write(const wxString& name, bool in);
        bool Write(const wxString& name, long in);
        bool Write(const wxString& name, const wxString & in);
        bool Write(const wxString& name, const wxArrayString & in);
        bool Write(const wxString& name, const wxFileName & in);
        bool Write(const wxString& name, size_t in);
        bool Write(const wxString& name, const StringMap & in);
        bool Write(const wxString& name, std::vector<TabInfoData> & in);
        bool Write(const wxString& name, std::vector<int> & in);
        bool Write(const wxString& name, const gsgsStringMap & in);
        bool Write(const wxString& name, const gsgsStringSet & in);
        bool WriteCData(const wxString& name, const wxString & in);

        bool Read(const wxString& name, ObjectData * out);
        bool Read(const wxString& name, int & out);
        bool Read(const wxString& name, bool & out);
        bool Read(const wxString& name, long & out);
        bool Read(const wxString& name, wxString & out);
        bool Read(const wxString& name, wxArrayString & out);
        bool Read(const wxString& name, wxFileName & out);
        bool Read(const wxString& name, size_t & out);
        bool Read(const wxString& name, StringMap & out);
        bool Read(const wxString& name, std::vector<TabInfoData> & out);
        bool Read(const wxString& name, std::vector<int> & out);
        bool Read(const wxString& name, gsgsStringMap & out);
        bool Read(const wxString& name, gsgsStringSet & out);
        bool ReadCData(const wxString & name, wxString & out);

#if wxUSE_GUI
        bool Write(const wxString& name, wxSize in);
        bool Write(const wxString& name, wxPoint in);
        bool Write(const wxString& name, const wxFont & in);
        bool Write(const wxString& name, const wxColour & in);
        bool Read(const wxString& name, wxPoint & out);
        bool Read(const wxString& name, wxSize & out);
        bool Read(const wxString& name, wxColour & out);
        bool Read(const wxString& name, wxFont & out, const wxFont & defaultFont = wxNullFont);
#endif
    private:
        bool WriteSimple(long in, const wxString & type, const wxString & name);
        bool ReadSimple(long & out, const wxString & type, const wxString & name);
    private:
        wxXmlNode * m_root;
    };

    class _gsgsAPI SettersGettersData : public ObjectData
    {
        size_t m_flags;
    public:
        SettersGettersData();
        virtual ~SettersGettersData();

        inline size_t GetFlags() const { return m_flags; }
        inline void SetFlags(size_t flags) { m_flags = flags; }

        void Serialize(Serializer &arch);
        void DeSerialize(Serializer &arch);
    };

    class _gsgsAPI PhpOptions : public ObjectData
    {
    public:
        PhpOptions();
        virtual ~PhpOptions();

        virtual void FromJSON(const JSONObject& json);
        virtual JSONObject ToJSON() const;

        PhpOptions& Load();
        PhpOptions& Save();

        void SetErrorReporting(const wxString& str) { m_errorReporting = str; }
        const wxString & GetErrorReporting() const { return m_errorReporting; }

        void SetIncludePaths(const wxArrayString& path) { m_includePaths = path; }
        const wxArrayString & GetIncludePaths() const { return m_includePaths; }

        void SetPhpExe(const wxString & exec) { m_phpExe = exec; }
        const wxString & GetPhpExe() const { return m_phpExe; }
    private:
        wxString m_phpExe;
        wxString m_errorReporting;
        wxArrayString m_includePaths;
    };
}
#endif // ARCHIVE_H
