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

#include "gsgsPreInclude.h"
#include "wxcrafter.h"
#include "gsgsScrolledPanel.h"

namespace gsgs
{
    class SelectIndexLinkButton : public wxCommandLinkButton
    {
    public:
        SelectIndexLinkButton() : wxCommandLinkButton() { }

        SelectIndexLinkButton(wxWindow *parent,
            wxWindowID id,
            int idx,
            const wxString& mainLabel = wxEmptyString,
            const wxString& note = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxButtonNameStr)
            : wxCommandLinkButton()
        {
            Create(parent, idx, id, mainLabel, note, pos, size, style, validator, name);
        }

        bool Create(wxWindow *parent,
            wxWindowID id, int idx,
            const wxString& mainLabel = wxEmptyString,
            const wxString& note = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxButtonNameStr)
        {
            mIndex = idx;
            return wxCommandLinkButton::Create(parent, id, mainLabel, note, pos, size, style, validator, name);
        }

        int GetSelectIndex() const                      { return mIndex; }
    protected:
        int mIndex;
    };
    typedef std::vector<wxCommandLinkButton *> LinkButtonList;
    class Present : public wxPanel
    {
    public:
        Present(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
        virtual ~Present();

        void RefreshRecentList();

        void RefreshCreationList();

        void RefreshPluginList();

        void RefreshServerList();
        
        wxStaticBitmap* GetStaticBitmap1()      { return m_staticBitmap1; }
        wxStaticLine* GetStaticLine7954()       { return m_staticLine7954; }
        wxStaticText* GetStaticText7991()       { return m_staticText7991; }
        wxGenericHyperlinkCtrl* GetHyperLink862()        { return m_hyperLink862; }
        wxGenericHyperlinkCtrl* GetHyperLink864()        { return m_hyperLink864; }
        wxGenericHyperlinkCtrl* GetHyperLink866()        { return m_hyperLink866; }
        wxStaticText* GetStaticText811()        { return m_staticText811; }
        wxSearchCtrl* GetSearchCtrl815()        { return m_searchCtrl815; }
        ScrolledWindow* GetScrollWin813()       { return m_scrollWin813; }
        wxCommandLinkButton* GetCmdLnkBtn855()  { return m_cmdLnkBtn855; }
        wxStaticText* GetStaticText835()        { return m_staticText835; }
        ScrolledWindow* GetScrollWin837()       { return m_scrollWin837; }
        wxPanel* GetPanel1913()                 { return m_panel1913; }
        wxStaticBitmap* GetStaticBitmap2()      { return m_staticBitmap2; }
        wxStaticLine* GetStaticLine6593()       { return m_staticLine6593; }
        wxStaticText* GetStaticText6534()       { return m_staticText6534; }
        wxStaticText* GetStaticText681()        { return m_staticText681; }
        wxStaticText* GetStaticText685()        { return m_staticText685; }
        wxStaticText* GetStaticText687()        { return m_staticText687; }
        wxStaticText* GetStaticText689()        { return m_staticText689; }
        wxStaticLine* GetStaticLine787()        { return m_staticLine787; }
        wxStaticText* GetStaticText745()        { return m_staticText745; }
        wxCommandLinkButton* GetCmdLnkBtn697() { return m_cmdLnkBtn697; }
        wxCommandLinkButton* GetCmdLnkBtn699() { return m_cmdLnkBtn699; }
        wxCommandLinkButton* GetCmdLnkBtn701() { return m_cmdLnkBtn701; }
        wxCommandLinkButton* GetCmdLnkBtn747() { return m_cmdLnkBtn747; }
        wxCommandLinkButton* GetCmdLnkBtn749() { return m_cmdLnkBtn749; }
        wxCommandLinkButton* GetCmdLnkBtn751() { return m_cmdLnkBtn751; }
        wxCommandLinkButton* GetCmdLnkBtn753() { return m_cmdLnkBtn753; }
        wxCommandLinkButton* GetCmdLnkBtn755() { return m_cmdLnkBtn755; }
        wxCommandLinkButton* GetCmdLnkBtn757() { return m_cmdLnkBtn757; }
        wxCommandLinkButton* GetCmdLnkBtn759() { return m_cmdLnkBtn759; }
        wxCommandLinkButton* GetCmdLnkBtn761() { return m_cmdLnkBtn761; }
        wxCommandLinkButton* GetCmdLnkBtn763() { return m_cmdLnkBtn763; }
        wxCommandLinkButton* GetCmdLnkBtn765() { return m_cmdLnkBtn765; }
        wxCommandLinkButton* GetCmdLnkBtn767() { return m_cmdLnkBtn767; }
        wxCommandLinkButton* GetCmdLnkBtn769() { return m_cmdLnkBtn769; }
        wxCommandLinkButton* GetCmdLnkBtn771() { return m_cmdLnkBtn771; }
        wxCommandLinkButton* GetCmdLnkBtn773() { return m_cmdLnkBtn773; }
        wxCommandLinkButton* GetCmdLnkBtn775() { return m_cmdLnkBtn775; }
        wxStaticBitmap* GetStaticBitmap3()      { return m_staticBitmap3; }
        wxStaticLine* GetStaticLine661()        { return m_staticLine661; }
        wxStaticText* GetStaticText657()        { return m_staticText657; }
        wxCommandLinkButton* GetCmdLnkBtn703() { return m_cmdLnkBtn703; }
        wxCommandLinkButton* GetCmdLnkBtn705() { return m_cmdLnkBtn705; }
        wxCommandLinkButton* GetCmdLnkBtn707() { return m_cmdLnkBtn707; }
        wxCommandLinkButton* GetCmdLnkBtn709() { return m_cmdLnkBtn709; }
        wxCommandLinkButton* GetCmdLnkBtn711() { return m_cmdLnkBtn711; }
        wxCommandLinkButton* GetCmdLnkBtn713() { return m_cmdLnkBtn713; }
        wxCommandLinkButton* GetCmdLnkBtn715() { return m_cmdLnkBtn715; }
        wxCommandLinkButton* GetCmdLnkBtn717() { return m_cmdLnkBtn717; }
        wxCommandLinkButton* GetCmdLnkBtn719() { return m_cmdLnkBtn719; }
        wxCommandLinkButton* GetCmdLnkBtn721() { return m_cmdLnkBtn721; }
        wxCommandLinkButton* GetCmdLnkBtn723() { return m_cmdLnkBtn723; }
        wxCommandLinkButton* GetCmdLnkBtn725() { return m_cmdLnkBtn725; }
        wxCommandLinkButton* GetCmdLnkBtn727() { return m_cmdLnkBtn727; }
        wxCommandLinkButton* GetCmdLnkBtn729() { return m_cmdLnkBtn729; }
        wxCommandLinkButton* GetCmdLnkBtn731() { return m_cmdLnkBtn731; }
        wxCommandLinkButton* GetCmdLnkBtn733() { return m_cmdLnkBtn733; }
        wxCommandLinkButton* GetCmdLnkBtn735() { return m_cmdLnkBtn735; }
        wxCommandLinkButton* GetCmdLnkBtn737() { return m_cmdLnkBtn737; }
        wxCommandLinkButton* GetCmdLnkBtn739() { return m_cmdLnkBtn739; }
        wxCommandLinkButton* GetCmdLnkBtn741() { return m_cmdLnkBtn741; }
        ScrolledWindow* GetScrollWin667()       { return m_scrollWin667; }

        void InitItem();
        void UninitItem();
        void SetRecentItem(const wxArrayString & blist);
        void SetCreationItem(const LinkButtonList & blist);
        void SetPluginItem(const LinkButtonList & blist);
        void SetServerItem(const LinkButtonList & blist);
    protected:
        virtual void OnSize(wxSizeEvent& event);

        virtual void OnOpenHomepage(wxCommandEvent& event);
        virtual void OnOpenForums(wxCommandEvent& event);
        virtual void OnOpenApi(wxCommandEvent& event);
        virtual void OnOpenManual(wxCommandEvent& event);
        virtual void OnOpenLicense(wxCommandEvent& event);

        void OnTheme(wxCommandEvent& e);

        virtual void OnOpenWorkspace(wxCommandEvent& event);

        virtual void OnNewWorkspace(wxCommandEvent& event);

        virtual void OnRecentItem(wxCommandEvent& event);

        virtual void OnRecentUI(wxUpdateUIEvent& event);

        //virtual void OnIdle(wxIdleEvent& event);

        int DoGetPopupMenuSelection(wxCommandLinkButton* btn, const wxArrayString& strings, const wxString& menuTitle);
    private:
        typedef std::map<int, wxString> IntStringMap_t;
        IntStringMap_t m_idToName;
        wxStaticBitmap* m_staticBitmap1;
        wxStaticLine* m_staticLine7954;
        wxStaticText* m_staticText7991;
        wxGenericHyperlinkCtrl* m_hyperLink862;
        wxGenericHyperlinkCtrl* m_hyperLink864;
        wxGenericHyperlinkCtrl* m_hyperLink866;
        wxGenericHyperlinkCtrl* m_hyperLink868;
        wxGenericHyperlinkCtrl* m_hyperLink870;
        wxStaticText* m_staticText810;
        wxStaticText* m_staticText811;
        wxSearchCtrl* m_searchCtrl815;
        LinkButtonList mRecentList;
        LinkButtonList mPluginList;
        LinkButtonList mServerList;
        LinkButtonList mCreationList;
        wxPanel * mWorkspacePanel;
        wxPanel * mRecentPanel;
        wxPanel * mPluginPanel;
        wxPanel * mServerPanel;
        wxPanel * mCreationPanel;
        wxArrayString mRecentFileList;
        wxString mRecentFile;
        DefaultScrolledWindow * m_scrollWin2471;
        DefaultScrolledWindow* m_scrollWin813;
        DefaultScrolledWindow* m_scrollWin6655;
        wxCommandLinkButton * mOpenWS;
        wxCommandLinkButton* m_cmdLnkBtn855;
        wxCommandLinkButton* m_cmdLnkBtn856;
        wxCommandLinkButton* m_cmdLnkBtn857;
        wxCommandLinkButton* m_cmdLnkBtn858;
        wxCommandLinkButton* m_cmdLnkBtn859;
        wxCommandLinkButton* m_cmdLnkBtn860;
        wxCommandLinkButton* m_cmdLnkBtn861;
        wxCommandLinkButton* m_cmdLnkBtn862;
        wxCommandLinkButton* m_cmdLnkBtn863;
        wxCommandLinkButton* m_cmdLnkBtn864;
        wxCommandLinkButton* m_cmdLnkBtn865;
        wxCommandLinkButton* m_cmdLnkBtn866;
        wxCommandLinkButton* m_cmdLnkBtn867;
        wxCommandLinkButton* m_cmdLnkBtn868;
        wxCommandLinkButton* m_cmdLnkBtn869;
        wxCommandLinkButton* m_cmdLnkBtn870;
        wxCommandLinkButton* m_cmdLnkBtn871;
        wxCommandLinkButton* m_cmdLnkBtn872;
        wxStaticText* m_staticText835;
        DefaultScrolledWindow* m_scrollWin837;
        wxPanel* m_panel1913;
        wxStaticBitmap* m_staticBitmap2;
        wxStaticLine* m_staticLine6593;
        wxStaticText* m_staticText6534;
        wxStaticText* m_staticText681;
        wxStaticText* m_staticText685;
        wxStaticText* m_staticText687;
        wxStaticText* m_staticText689;
        wxStaticLine* m_staticLine787;
        wxStaticText* m_staticText745;
        wxCommandLinkButton* m_cmdLnkBtn697;
        wxCommandLinkButton* m_cmdLnkBtn699;
        wxCommandLinkButton* m_cmdLnkBtn701;
        wxCommandLinkButton* m_cmdLnkBtn747;
        wxCommandLinkButton* m_cmdLnkBtn749;
        wxCommandLinkButton* m_cmdLnkBtn751;
        wxCommandLinkButton* m_cmdLnkBtn753;
        wxCommandLinkButton* m_cmdLnkBtn755;
        wxCommandLinkButton* m_cmdLnkBtn757;
        wxCommandLinkButton* m_cmdLnkBtn759;
        wxCommandLinkButton* m_cmdLnkBtn761;
        wxCommandLinkButton* m_cmdLnkBtn763;
        wxCommandLinkButton* m_cmdLnkBtn765;
        wxCommandLinkButton* m_cmdLnkBtn767;
        wxCommandLinkButton* m_cmdLnkBtn769;
        wxCommandLinkButton* m_cmdLnkBtn771;
        wxCommandLinkButton* m_cmdLnkBtn773;
        wxCommandLinkButton* m_cmdLnkBtn775;
        wxStaticBitmap* m_staticBitmap3;
        wxStaticLine* m_staticLine661;
        wxStaticText* m_staticText657;
        DefaultScrolledWindow* m_scrollWin667;
        wxCommandLinkButton* m_cmdLnkBtn703;
        wxCommandLinkButton* m_cmdLnkBtn705;
        wxCommandLinkButton* m_cmdLnkBtn707;
        wxCommandLinkButton* m_cmdLnkBtn709;
        wxCommandLinkButton* m_cmdLnkBtn711;
        wxCommandLinkButton* m_cmdLnkBtn713;
        wxCommandLinkButton* m_cmdLnkBtn715;
        wxCommandLinkButton* m_cmdLnkBtn717;
        wxCommandLinkButton* m_cmdLnkBtn719;
        wxCommandLinkButton* m_cmdLnkBtn721;
        wxCommandLinkButton* m_cmdLnkBtn723;
        wxCommandLinkButton* m_cmdLnkBtn725;
        wxCommandLinkButton* m_cmdLnkBtn727;
        wxCommandLinkButton* m_cmdLnkBtn729;
        wxCommandLinkButton* m_cmdLnkBtn731;
        wxCommandLinkButton* m_cmdLnkBtn733;
        wxCommandLinkButton* m_cmdLnkBtn735;
        wxCommandLinkButton* m_cmdLnkBtn737;
        wxCommandLinkButton* m_cmdLnkBtn739;
        wxCommandLinkButton* m_cmdLnkBtn741;
    };
}