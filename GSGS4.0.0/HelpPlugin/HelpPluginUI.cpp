//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: HelpPluginUI.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "HelpPluginUI.h"


// Declare the bitmap loading function
extern void wxC10DEInitBitmapResources();

static bool bBitmapLoaded = false;


HelpPluginMessageBaseDlg::HelpPluginMessageBaseDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC10DEInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer2);
    
    wxFlexGridSizer* flexGridSizer12 = new wxFlexGridSizer(0, 2, 0, 0);
    flexGridSizer12->SetFlexibleDirection( wxBOTH );
    flexGridSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    
    boxSizer2->Add(flexGridSizer12, 1, wxALL|wxEXPAND, 5);
    
    m_staticBitmap14 = new wxStaticBitmap(this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("error-64")), wxDefaultPosition, wxSize(-1,-1), 0 );
    
    flexGridSizer12->Add(m_staticBitmap14, 0, wxALL, 5);
    
    m_staticText = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1,-1), 0);
    
    flexGridSizer12->Add(m_staticText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    
    m_hyperLink = new wxHyperlinkCtrl(this, wxID_ANY, _("Download Zeal"), wxT("http://www.niiengine.com"), wxDefaultPosition, wxSize(-1,-1), wxHL_DEFAULT_STYLE);
    m_hyperLink->SetNormalColour(wxColour(wxT("#0000FF")));
    m_hyperLink->SetHoverColour(wxColour(wxT("#0000FF")));
    m_hyperLink->SetVisitedColour(wxColour(wxT("#FF0000")));
    
    boxSizer2->Add(m_hyperLink, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_stdBtnSizer4 = new wxStdDialogButtonSizer();
    
    boxSizer2->Add(m_stdBtnSizer4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_button6 = new wxButton(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_button6->SetDefault();
    m_stdBtnSizer4->AddButton(m_button6);
    m_stdBtnSizer4->Realize();
    
    SetName(wxT("HelpPluginMessageBaseDlg"));
    SetMinSize( wxSize(500,-1) );
    SetSizeHints(500,-1);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    CentreOnParent(wxBOTH);
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
}

HelpPluginMessageBaseDlg::~HelpPluginMessageBaseDlg()
{
}

HelpPluginSettingsDlgBase::HelpPluginSettingsDlgBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxC10DEInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    wxBoxSizer* boxSizer24 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer24);
    
    wxBoxSizer* boxSizer32 = new wxBoxSizer(wxVERTICAL);
    
    boxSizer24->Add(boxSizer32, 1, wxALL|wxEXPAND, 5);
    
    wxArrayString m_pgMgr34Arr;
    wxUnusedVar(m_pgMgr34Arr);
    wxArrayInt m_pgMgr34IntArr;
    wxUnusedVar(m_pgMgr34IntArr);
    m_pgMgr34 = new wxPropertyGridManager(this, wxID_ANY, wxDefaultPosition, wxSize(500,300), wxPG_DESCRIPTION|wxPG_SPLITTER_AUTO_CENTER|wxPG_BOLD_MODIFIED);
    
    boxSizer32->Add(m_pgMgr34, 1, wxALL|wxEXPAND, 5);
    
    m_pgProp36 = m_pgMgr34->Append(  new wxPropertyCategory( _("Docsets") ) );
    m_pgProp36->SetHelpString(wxT(""));
    
    m_pgPropCxx = m_pgMgr34->AppendIn( m_pgProp36,  new wxLongStringProperty( _("C/C++"), wxPG_LABEL, _("c++,net,boost,qt 4,qt 5,cvcpp,cocos2dx,c,manpages")) );
    m_pgPropCxx->SetHelpString(_("Set a list of docsets to use when requesting help while inside a C/C++ files (comma separated list)"));
    
    m_pgPropPHP = m_pgMgr34->AppendIn( m_pgProp36,  new wxLongStringProperty( _("PHP"), wxPG_LABEL, _("php,wordpress,drupal,zend,laravel,yii,joomla,ee,codeigniter,cakephp,phpunit,symfony,typo3,twig,smarty,phpp,html,statamic,mysql,sqlite,mongodb,psql,redis,zend framework 1,zend framework 2")) );
    m_pgPropPHP->SetHelpString(_("Set a list of docsets to use when requesting help while inside a PHP files (comma separated list)"));
    
    m_pgPropCMake = m_pgMgr34->AppendIn( m_pgProp36,  new wxLongStringProperty( _("CMake"), wxPG_LABEL, _("cmake")) );
    m_pgPropCMake->SetHelpString(_("Set a list of docsets to use when requesting help while inside a CMake files (comma separated list)"));
    
    m_pgPropHtml = m_pgMgr34->AppendIn( m_pgProp36,  new wxLongStringProperty( _("HTML"), wxPG_LABEL, _("html,svg,css,bootstrap,less,foundation,awesome,statamic,javascript,jquery,jqueryui,jquerym,angularjs,backbone,marionette,meteor,moo,prototype,ember,lodash,underscore,sencha,extjs,knockout,zepto,cordova,phonegap,yu")) );
    m_pgPropHtml->SetHelpString(_("Set a list of docsets to use when requesting help while inside an HTML files (comma separated list)"));
    
    m_pgPropCSS = m_pgMgr34->AppendIn( m_pgProp36,  new wxLongStringProperty( _("CSS"), wxPG_LABEL, _("html,svg,css,bootstrap,less,foundation,awesome,statamic,javascript,jquery,jqueryui,jquerym,angularjs,backbone,marionette,meteor,moo,prototype,ember,lodash,underscore,sencha,extjs,knockout,zepto,cordova,phonegap,yu")) );
    m_pgPropCSS->SetHelpString(_("Set a list of docsets to use when requesting help while inside an CSS files (comma separated list)"));
    
    m_pgPropJS = m_pgMgr34->AppendIn( m_pgProp36,  new wxLongStringProperty( _("JavaScript"), wxPG_LABEL, _("html,svg,css,bootstrap,less,foundation,awesome,statamic,javascript,jquery,jqueryui,jquerym,angularjs,backbone,marionette,meteor,moo,prototype,ember,lodash,underscore,sencha,extjs,knockout,zepto,cordova,phonegap,yu")) );
    m_pgPropJS->SetHelpString(_("Set a list of docsets to use when requesting help while inside a JavaScript files (comma separated list)"));
    
    m_pgPropJava = m_pgMgr34->AppendIn( m_pgProp36,  new wxLongStringProperty( _("Java"), wxPG_LABEL, _("java,javafx,grails,groovy,playjava,spring,cvj,processing")) );
    m_pgPropJava->SetHelpString(_("Set a list of docsets to use when requesting help while inside a Java files (comma separated list)"));
    
    m_stdBtnSizer26 = new wxStdDialogButtonSizer();
    
    boxSizer24->Add(m_stdBtnSizer26, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    
    m_button28 = new wxButton(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_stdBtnSizer26->AddButton(m_button28);
    
    m_button30 = new wxButton(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
    m_button30->SetDefault();
    m_stdBtnSizer26->AddButton(m_button30);
    m_stdBtnSizer26->Realize();
    
    SetName(wxT("HelpPluginSettingsDlgBase"));
    SetSizeHints(-1,-1);
    if ( GetSizer() ) {
         GetSizer()->Fit(this);
    }
    CentreOnParent(wxBOTH);
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
    // Connect events
    m_pgMgr34->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(HelpPluginSettingsDlgBase::OnDocsetChanged), NULL, this);
    m_button30->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(HelpPluginSettingsDlgBase::OnOKUI), NULL, this);
    m_button30->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(HelpPluginSettingsDlgBase::OnOK), NULL, this);
    
}

HelpPluginSettingsDlgBase::~HelpPluginSettingsDlgBase()
{
    m_pgMgr34->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(HelpPluginSettingsDlgBase::OnDocsetChanged), NULL, this);
    m_button30->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(HelpPluginSettingsDlgBase::OnOKUI), NULL, this);
    m_button30->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(HelpPluginSettingsDlgBase::OnOK), NULL, this);
    
}