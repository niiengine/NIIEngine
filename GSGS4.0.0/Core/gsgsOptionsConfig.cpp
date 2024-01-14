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
#include "gsgsEditorConfig.h"
#include "gsgsOptionsConfig.h"
#include "gsgsFileUtil.h"
#include "gsgsLogManager.h"
#include "xmlutils.h"
#include <wx/fontmap.h>
#include <wx/intl.h>

#ifdef __WXMSW__
#include <wx/msw/uxtheme.h>
#endif

namespace gsgs
{
    //----------------------------------------------------------------------
    const wxString defaultBookmarkLabels = wxString(';', CL_N0_OF_BOOKMARK_TYPES - 1) + "Find";
    // --------------------------------------------------------------
    wxString SetDefaultBookmarkColours()
    {
        // (Confusingly, the 'foreground' is actually just the rim of the marker; the background is the central bulk)
        // NB: We want the 'find' colour always to be the most significant, so add any future extra items *before* the last
        // one
        const wxString arr[] = { "#FF0080", "#0000FF", "#FF0000", "#00FF00", "#FFFF00" };
        wxString output;
        for(size_t n = 0; n < CL_N0_OF_BOOKMARK_TYPES; ++n) {
            if(n < sizeof(arr) / sizeof(wxString)) {
                output << arr[n] << ';';
            } else {
                output << "#FF0080" << ';';
            }
        }

        return output;
    }
    // --------------------------------------------------------------
    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------
    // --------------------------------------------------------------
    EditorConfigData::EditorConfigData()
        : m_rootFileFound(false)
    {
    }
    // --------------------------------------------------------------
    EditorConfigData::~EditorConfigData() {}
    // --------------------------------------------------------------
    bool EditorConfigData::LoadForFile(const wxFileName& filename, wxFileName& editorConfigFile)
    {
        editorConfigFile = wxFileName(filename.GetPath(), ".editorconfig");

        bool foundFile = false;
        while(editorConfigFile.GetDirCount()) {
            if(editorConfigFile.FileExists()) {
                foundFile = true;
                break;
            }
            editorConfigFile.RemoveLastDir();
        }

        if(!foundFile) return false;

        wxString content;
        if(!FileUtil::ReadFileContent(editorConfigFile, content)) {
            gsgsDEBUG() << "Failed to read file:" << editorConfigFile << clEndl;
            return false;
        }

        EditorConfigSection section;
        m_sections.push_back(section);
        EditorConfigSection* cursection = &(m_sections.back());
        wxUnusedVar(cursection); // for debug purposes
        wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.size(); ++i) {
            // Remove comments
            wxString strLine = lines.Item(i);
            strLine = strLine.BeforeFirst('#');
            strLine = strLine.BeforeFirst(';');

            strLine.Trim().Trim(false);

            if(strLine.IsEmpty()) continue;

            // Process the line
            if(strLine.StartsWith("[") && strLine.EndsWith("]")) {
                strLine.RemoveLast().Remove(0, 1); // remove the []
                EditorConfigSection section;
                section.patterns = ProcessSection(strLine);
                m_sections.push_back(section);
                cursection = &(m_sections.back());

            } else {
                ProcessDirective(strLine);
            }
        }
        gsgsDEBUG1() << "Using .editorconfig file:" << editorConfigFile << clEndl;
        return true;
    }
    // --------------------------------------------------------------
    struct clEditorConfigTreeNode {
        wxString data;
        typedef std::vector<clEditorConfigTreeNode*> Vec_t;
        clEditorConfigTreeNode::Vec_t children;

        clEditorConfigTreeNode* AddChild(const wxString& data)
        {
            clEditorConfigTreeNode* child = new clEditorConfigTreeNode;
            child->data = data;
            children.push_back(child);
            return child;
        }

    public:
        clEditorConfigTreeNode() {}
        ~clEditorConfigTreeNode()
        {
            std::for_each(children.begin(), children.end(), [&](clEditorConfigTreeNode* child) { delete child; });
            children.clear();
        }

    public:
        void GetPatterns(wxArrayString& patterns) { DoGetPatterns(this, patterns, data); }
        void Add(const wxString& pattern)
        {
            wxArrayString arr;
            arr.Add(pattern);
            Add(arr);
        }

        void Add(const wxArrayString& patterns)
        {
            clEditorConfigTreeNode::Vec_t leaves;
            DoGetLeaves(this, leaves);

            std::for_each(leaves.begin(), leaves.end(), [&](clEditorConfigTreeNode* leaf) {
                for(size_t i = 0; i < patterns.size(); ++i) {
                    leaf->AddChild(patterns.Item(i));
                }
            });
        }

        bool IsEmpty() const { return children.empty(); }

    private:
        void DoGetLeaves(clEditorConfigTreeNode* node, clEditorConfigTreeNode::Vec_t& leaves)
        {
            if(node->children.empty()) {
                // leaf node
                leaves.push_back(node);
            } else {
                for(size_t i = 0; i < node->children.size(); ++i) {
                    DoGetLeaves(node->children.at(i), leaves);
                }
            }
        }

        /**
         * @brief collect the patterns from the leaf nodes
         */
        void DoGetPatterns(clEditorConfigTreeNode* node, wxArrayString& patterns, const wxString& curpattern)
        {
            if(node->children.empty()) {
                // leaf node
                patterns.Add(curpattern + node->data);
            } else {
                for(size_t i = 0; i < node->children.size(); ++i) {
                    DoGetPatterns(node->children.at(i), patterns, curpattern + node->data);
                }
            }
        }

        /**
         * @brief collect the patterns from the leaf nodes
         */
        void DoAddToAllLeaves(clEditorConfigTreeNode* node, const wxString& pattern)
        {
            if(node->children.empty()) {
                // leaf node
                node->data << pattern;
            } else {
                for(size_t i = 0; i < node->children.size(); ++i) {
                    DoAddToAllLeaves(node->children.at(i), pattern);
                }
            }
        }
    };
    // --------------------------------------------------------------
    enum eEditorConfigState {
        kEC_STATE_NORMAL,
        kEC_STATE_IN_CURLYGRP,
        kEC_STATE_IN_SQUAREGRP,
    };
    // --------------------------------------------------------------
    wxArrayString EditorConfigData::ProcessSection(wxString& strLine)
    {
        eEditorConfigState state = kEC_STATE_NORMAL;

        clEditorConfigTreeNode* tree = new clEditorConfigTreeNode;
        std::vector<clEditorConfigTreeNode*> trees;
        trees.push_back(tree);

        wxString curpattern;
        while(!strLine.IsEmpty()) {
            switch(state) {
            case kEC_STATE_NORMAL: {
                wxChar ch = strLine.at(0);
                strLine.Remove(0, 1);
                switch(ch) {
                case '{':
                    state = kEC_STATE_IN_CURLYGRP;
                    break;
                case '[':
                    state = kEC_STATE_IN_SQUAREGRP;
                    break;
                case ',':
                    // new pattern
                    if(!curpattern.IsEmpty()) {
                        tree->Add(curpattern);

                        tree = new clEditorConfigTreeNode;
                        trees.push_back(tree);
                        curpattern.clear();
                    }
                    break;
                default:
                    curpattern << ch;
                    break;
                }
                break;
            }
            case kEC_STATE_IN_CURLYGRP: {
                // if we got something so far, add it before we continue
                if(!curpattern.IsEmpty()) {
                    tree->Add(curpattern);
                    curpattern.clear();
                }

                // read the buffer until we hit the closing brace
                wxString buffer;
                if(!ReadUntil('}', strLine, buffer)) {
                    return wxArrayString();
                }
                state = kEC_STATE_NORMAL;
                wxArrayString groupPatterns = ProcessSection(buffer);
                tree->Add(groupPatterns);
                break;
            }
            case kEC_STATE_IN_SQUAREGRP: {
                wxString buffer;
                if(!ReadUntil(']', strLine, buffer)) {
                    return wxArrayString();
                }
                state = kEC_STATE_NORMAL;
                break;
            }
            }
        }

        // Remainder
        if(!curpattern.IsEmpty()) {
            tree->Add(curpattern);
        }

        wxArrayString res;
        for(size_t i = 0; i < trees.size(); ++i) {
            wxArrayString patterns;
            trees.at(i)->GetPatterns(patterns);
            res.insert(res.end(), patterns.begin(), patterns.end());
            delete trees.at(i);
        }

        // Loop over the array and change "**" => "*"
        for(size_t i = 0; i < res.size(); ++i) {
            res.Item(i).Replace("**", "*");
        }
        return res;
    }
    // --------------------------------------------------------------
    #define IS_TRUE(value) ((value.CmpNoCase("true") == 0) || (value.CmpNoCase("yes") == 0) || (value.CmpNoCase("1") == 0))
    void EditorConfigData::ProcessDirective(wxString& strLine)
    {
        EditorConfigSection* cursection = &(m_sections.back());
        wxString key = strLine.BeforeFirst('=');
        wxString value = strLine.AfterFirst('=');
        key.Trim().Trim(false);
        value.Trim().Trim(false);

        if(key == "indent_style") {
            cursection->SetIndentStyle(value.Lower());

        } else if(key == "indent_size") {
            long lv = 4;
            value.ToCLong(&lv);
            cursection->SetIndentSize(lv);

        } else if(key == "tab_width") {
            long lv = 4;
            value.ToCLong(&lv);
            cursection->SetTabWidth(lv);

        } else if(key == "charset") {
            cursection->SetCharset(value.Lower());

        } else if(key == "trim_trailing_whitespace") {
            cursection->SetTrimTrailingWhitespace(IS_TRUE(value));

        } else if(key == "insert_final_newline") {
            cursection->SetInsertFinalNewline(IS_TRUE(value));

        } else if(key == "end_of_line") {
            cursection->SetEndOfLine(value.Lower());

        } else if(key == "root") {
            m_rootFileFound = IS_TRUE(value);
        }
    }
    // --------------------------------------------------------------
    bool EditorConfigData::ReadUntil(wxChar delim, wxString& strLine, wxString& output)
    {
        while(!strLine.IsEmpty()) {
            wxChar ch = strLine.at(0);
            strLine.Remove(0, 1);
            if(ch == delim) {
                return true;
            } else {
                output << ch;
            }
        }
        return false;
    }
    // --------------------------------------------------------------
    bool EditorConfigData::GetSectionForFile(const wxFileName& filename, EditorConfigSection& section)
    {
        wxFileName editorConfigFile;
        if(!LoadForFile(filename, editorConfigFile)) return false;
        section = EditorConfigSection();
        section.filename = editorConfigFile;
        bool match_found = false;
        std::for_each(m_sections.begin(), m_sections.end(), [&](const EditorConfigSection& sec) {
            for(size_t i = 0; i < sec.patterns.size(); ++i) {
                const wxString& pattern = sec.patterns.Item(i);
                bool is_wild = pattern.Contains("*");

                wxString fullpath = filename.GetFullPath(wxPATH_UNIX);
                wxString fullname = filename.GetFullName();

                if((is_wild && ::wxMatchWild(pattern, fullpath, false)) || (!is_wild && fullname == pattern)) {
                    match_found = true;
                    if(sec.IsCharsetSet()) {
                        section.SetCharset(sec.GetCharset());
                    }
                    if(sec.IsIndentSizeSet()) {
                        section.SetIndentSize(sec.GetIndentSize());
                    }
                    if(sec.IsIndentStyleSet()) {
                        section.SetIndentStyle(sec.GetIndentStyle());
                    }
                    if(sec.IsInsertFinalNewlineSet()) {
                        section.SetInsertFinalNewline(sec.IsInsertFinalNewline());
                    }
                    if(sec.IsSetEndOfLineSet()) {
                        section.SetEndOfLine(sec.GetEndOfLine());
                    }
                    if(sec.IsTabWidthSet()) {
                        section.SetTabWidth(sec.GetTabWidth());
                    }
                    if(sec.IsTrimTrailingWhitespaceSet()) {
                        section.SetTrimTrailingWhitespace(sec.IsTrimTrailingWhitespace());
                    }
                    break;
                }
            }
        });

        // Print the match to the log file
        if(match_found) {
            section.PrintToLog();
        }
        return match_found;
    }
    // --------------------------------------------------------------
    void EditorConfigSection::PrintToLog()
    {
        gsgsDEBUG1() << ".editorconfig (" << filename << ") :" << clEndl;
        if(IsCharsetSet()) {
            gsgsDEBUG1() << "charset:" << GetCharset() << clEndl;
        }
        if(IsIndentSizeSet()) {
            gsgsDEBUG1() << "indent_size:" << GetIndentSize() << clEndl;
        }
        if(IsIndentStyleSet()) {
            gsgsDEBUG1() << "indent_style:" << GetIndentStyle() << clEndl;
        }
        if(IsInsertFinalNewlineSet()) {
            gsgsDEBUG1() << "insert_final_newline:" << IsInsertFinalNewline() << clEndl;
        }
        if(IsSetEndOfLineSet()) {
            gsgsDEBUG1() << "end_of_line:" << GetEndOfLine() << clEndl;
        }
        if(IsTabWidthSet()) {
            gsgsDEBUG1() << "tab_width:" << GetTabWidth() << clEndl;
        }
        if(IsTrimTrailingWhitespaceSet()) {
            gsgsDEBUG1() << "trim_trailing_whitespace:" << IsTrimTrailingWhitespace() << clEndl;
        }
    }
    wxDEFINE_EVENT(wxEVT_EDITOR_CONFIG_LOADING, EditorConfigEvent);
    // --------------------------------------------------------------
    // --------------------------------------------------------------
    // EditorConfigEvent
    // --------------------------------------------------------------
    // --------------------------------------------------------------
    EditorConfigEvent::~EditorConfigEvent() {}
    // --------------------------------------------------------------
    EditorConfigEvent::EditorConfigEvent(const EditorConfigEvent& event) { *this = event; }
    // --------------------------------------------------------------
    EditorConfigEvent::EditorConfigEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    {
    }
    // --------------------------------------------------------------
    EditorConfigEvent& EditorConfigEvent::operator=(const EditorConfigEvent& src)
    {
        Event::operator=(src);
        m_editorConfigSection = src.m_editorConfigSection;
        return *this;
    }
    // --------------------------------------------------------------
    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------
    // --------------------------------------------------------------
    OptionsConfig::OptionsConfig(wxXmlNode* node)
        : m_displayFoldMargin(false)
        , m_underlineFoldLine(true)
        , m_scrollBeyondLastLine(true)
        , m_foldStyle(wxT("Arrows with Background Colour"))
        , m_displayBookmarkMargin(true)
        , m_bookmarkShape(wxT("Small Arrow"))
        , m_bookmarkBgColours(SetDefaultBookmarkColours())
        , m_bookmarkFgColours(SetDefaultBookmarkColours())
        , m_bookmarkLabels(defaultBookmarkLabels)
        , m_highlightCaretLine(true)
        , m_clearHighlitWordsOnFind(true)
        , m_displayLineNumbers(true)
        , m_relativeLineNumbers(false)
        , m_showIndentationGuidelines(false)
        , m_caretLineColour(gsgs_Style().GetActiveMarkerColour())
        , m_indentUsesTabs(true)
        , m_indentWidth(4)
        , m_tabWidth(4)
        , m_iconsSize(16)
        , m_extraAscent(-3)
        , m_AntiAliasing(true)
        , m_showWhitspaces(0 /*wxSCI_WS_INVISIBLE*/)
        , m_foldCompact(false)
        , m_foldAtElse(false)
        , m_foldPreprocessor(false)
        , m_edgeMode(0 /*wxSCI_EDGE_NONE*/)
        , m_edgeColumn(80)
        , m_edgeColour(wxColour(wxT("LIGHT GREY")))
        , m_highlightMatchedBraces(true)
        , m_foldBgColour(wxColour(240, 240, 240))
        , m_autoAdjustHScrollBarWidth(false)
        , m_caretWidth(2)
        , m_caretBlinkPeriod(500)
        , m_copyLineEmptySelection(true)
        , m_programConsoleCommand(TERMINAL_CMD)
        , m_eolMode(wxT("Default"))
        , m_hideChangeMarkerMargin(false)
        , m_hideOutpuPaneOnUserClick(false)
        , m_hideOutputPaneNotIfBuild(false)
        , m_hideOutputPaneNotIfSearch(true)
        , m_hideOutputPaneNotIfReplace(false)
        , m_hideOutputPaneNotIfReferences(false)
        , m_hideOutputPaneNotIfOutput(false)
        , m_hideOutputPaneNotIfTrace(false)
        , m_hideOutputPaneNotIfTasks(false)
        , m_hideOutputPaneNotIfBuildQ(true)
        , m_hideOutputPaneNotIfCppCheck(true)
        , m_hideOutputPaneNotIfSvn(true)
        , m_hideOutputPaneNotIfCscope(false)
        , m_hideOutputPaneNotIfGit(true)
        , m_hideOutputPaneNotIfDebug(true)
        , m_hideOutputPaneNotIfMemCheck(true)
        , m_findBarAtBottom(true)
        , m_showReplaceBar(true)
        , m_TrimLine(true)
        , m_AppendLF(true)
        , m_disableSmartIndent(false)
        , m_disableSemicolonShift(false)
        , m_caretLineAlpha(30)
        , m_dontAutoFoldResults(true)
        , m_dontOverrideSearchStringWithSelection(false)
        , m_showDebugOnRun(true)
        , m_caretUseCamelCase(true)
        , m_wordWrap(false)
        , m_dockingStyle(0)
        , m_preferredLocale(wxT("en_US"))
        , m_useLocale(0)
        , m_trimOnlyModifiedLines(true)
        , m_options(Opt_AutoCompleteCurlyBraces | Opt_AutoCompleteNormalBraces | Opt_NavKey_Shift | Opt_WrapBrackets |
                    Opt_WrapQuotes | Opt_AutoCompleteDoubleQuotes | Opt_FoldHighlightActiveBlock |
                    Opt_WrapCmdWithDoubleQuotes | Opt_TabStyleMinimal | Opt_HideDockingWindowCaption)
        , m_options2(0)
        , m_workspaceTabsDirection(wxUP)
        , m_outputTabsDirection(wxUP)
        , m_indentedComments(false)
        , m_nbTabHeight(nbTabHt_Short)
        , mImageTabSize(64, 64)
        , m_webSearchPrefix(wxT("https://www.google.com/search?q="))
    {
        gsgs_Style().LightColour("LIME GREEN", m_debuggerMarkerLine, 8.0);
        m_mswTheme = false;
        // set the default font name to be wxFONTENCODING_UTF8
        SetFileFontEncoding(wxFontMapper::GetEncodingName(wxFONTENCODING_UTF8));
        if(node) {
            SetFileFontEncoding(
                XmlUtils::ReadString(node, wxT("FileFontEncoding"), wxFontMapper::GetEncodingName(wxFONTENCODING_UTF8)));

            m_displayFoldMargin = XmlUtils::ReadBool(node, wxT("DisplayFoldMargin"), m_displayFoldMargin);
            m_underlineFoldLine = XmlUtils::ReadBool(node, wxT("UnderlineFoldedLine"), m_underlineFoldLine);
            m_foldStyle = XmlUtils::ReadString(node, wxT("FoldStyle"), m_foldStyle);
            m_displayBookmarkMargin = XmlUtils::ReadBool(node, wxT("DisplayBookmarkMargin"), m_displayBookmarkMargin);
            m_bookmarkShape = XmlUtils::ReadString(node, wxT("BookmarkShape"), m_bookmarkShape);
            m_bookmarkBgColours =
                XmlUtils::ReadString(node, wxT("BookmarkBgColours"), ""); // No default; we'll deal with this later
            m_bookmarkFgColours = XmlUtils::ReadString(node, wxT("BookmarkFgColours"), "");
            m_bookmarkLabels = XmlUtils::ReadString(node, wxT("BookmarkLabels"), defaultBookmarkLabels);
            m_clearHighlitWordsOnFind = XmlUtils::ReadBool(node, wxT("ClearHighlitWordsOnFind"), m_clearHighlitWordsOnFind);
            //m_highlightCaretLine = XmlUtils::ReadBool(node, wxT("HighlightCaretLine"), m_highlightCaretLine);
            m_displayLineNumbers = XmlUtils::ReadBool(node, wxT("ShowLineNumber"), m_displayLineNumbers);
            m_relativeLineNumbers = XmlUtils::ReadBool(node, wxT("RelativeLineNumber"), m_relativeLineNumbers);
            m_highlightCurLineNumber = XmlUtils::ReadBool(node, wxT("HighlightCurLineNumber"), m_highlightCurLineNumber);
            m_showIndentationGuidelines = XmlUtils::ReadBool(node, wxT("IndentationGuides"), m_showIndentationGuidelines);
            //m_caretLineColour =
            //    XmlUtils::ReadString(node, wxT("CaretLineColour"), m_caretLineColour.GetAsString(wxC2S_HTML_SYNTAX));
            m_indentUsesTabs = XmlUtils::ReadBool(node, wxT("IndentUsesTabs"), m_indentUsesTabs);
            m_indentWidth = XmlUtils::ReadLong(node, wxT("IndentWidth"), m_indentWidth);
            m_tabWidth = XmlUtils::ReadLong(node, wxT("TabWidth"), m_tabWidth);
            m_iconsSize = XmlUtils::ReadLong(node, wxT("ToolbarIconSize"), m_iconsSize);
            m_showWhitspaces = XmlUtils::ReadLong(node, wxT("ShowWhitespaces"), m_showWhitspaces);
            m_foldCompact = XmlUtils::ReadBool(node, wxT("FoldCompact"), m_foldCompact);
            m_foldAtElse = XmlUtils::ReadBool(node, wxT("FoldAtElse"), m_foldAtElse);
            m_foldPreprocessor = XmlUtils::ReadBool(node, wxT("FoldPreprocessor"), m_foldPreprocessor);
            m_edgeMode = XmlUtils::ReadLong(node, wxT("EdgeMode"), m_edgeMode);
            m_edgeColumn = XmlUtils::ReadLong(node, wxT("EdgeColumn"), m_edgeColumn);
            m_edgeColour = XmlUtils::ReadString(node, wxT("EdgeColour"), m_edgeColour.GetAsString(wxC2S_HTML_SYNTAX));
            m_highlightMatchedBraces = XmlUtils::ReadBool(node, wxT("HighlightMatchedBraces"), m_highlightMatchedBraces);
            m_foldBgColour = XmlUtils::ReadString(node, wxT("FoldBgColour"), m_foldBgColour.GetAsString(wxC2S_HTML_SYNTAX));
            m_autoAdjustHScrollBarWidth =
                XmlUtils::ReadBool(node, wxT("AutoAdjustHScrollBarWidth"), m_autoAdjustHScrollBarWidth);
            m_caretBlinkPeriod = XmlUtils::ReadLong(node, wxT("CaretBlinkPeriod"), m_caretBlinkPeriod);
            m_caretWidth = XmlUtils::ReadLong(node, wxT("CaretWidth"), m_caretWidth);
            m_copyLineEmptySelection = XmlUtils::ReadBool(node, wxT("CopyLineEmptySelection"), m_copyLineEmptySelection);
            m_programConsoleCommand = XmlUtils::ReadString(node, wxT("ConsoleCommand"), m_programConsoleCommand);
            m_eolMode = XmlUtils::ReadString(node, wxT("EOLMode"), m_eolMode);
            m_hideChangeMarkerMargin = XmlUtils::ReadBool(node, wxT("HideChangeMarkerMargin"));
            m_scrollBeyondLastLine = XmlUtils::ReadBool(node, wxT("ScrollBeyondLastLine"), m_scrollBeyondLastLine);
            m_hideOutpuPaneOnUserClick = XmlUtils::ReadBool(node, wxT("HideOutputPaneOnUserClick"));
            m_hideOutputPaneNotIfBuild = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfBuild"));
            m_hideOutputPaneNotIfSearch = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfSearch"));
            m_hideOutputPaneNotIfReplace = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfReplace"));
            m_hideOutputPaneNotIfReferences = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfReferences"));
            m_hideOutputPaneNotIfOutput = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfOutput"));
            m_hideOutputPaneNotIfTrace = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfTrace"));
            m_hideOutputPaneNotIfTasks = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfTasks"));
            m_hideOutputPaneNotIfBuildQ = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfBuildQ"));
            m_hideOutputPaneNotIfCppCheck = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfCppCheck"));
            m_hideOutputPaneNotIfSvn = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfSvn"));
            m_hideOutputPaneNotIfCscope = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfCscope"));
            m_hideOutputPaneNotIfGit = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfGit"));
            m_hideOutputPaneNotIfDebug = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfDebug"));
            m_hideOutputPaneNotIfMemCheck = XmlUtils::ReadBool(node, wxT("HideOutputPaneNotIfMemCheck"));
            m_findBarAtBottom = XmlUtils::ReadBool(node, wxT("FindBarAtBottom"), m_findBarAtBottom);
            m_showReplaceBar = XmlUtils::ReadBool(node, wxT("ShowReplaceBar"), m_showReplaceBar);
            m_disableSmartIndent = XmlUtils::ReadBool(node, wxT("DisableSmartIndent"), m_disableSmartIndent);
            m_disableSemicolonShift = XmlUtils::ReadBool(node, wxT("DisableSemicolonShift"), m_disableSemicolonShift);
            m_caretLineAlpha = XmlUtils::ReadLong(node, wxT("CaretLineAlpha"), m_caretLineAlpha);
            m_dontAutoFoldResults = XmlUtils::ReadBool(node, wxT("DontAutoFoldResults"), m_dontAutoFoldResults);
            m_dontOverrideSearchStringWithSelection = XmlUtils::ReadBool(node, wxT("DontOverrideSearchStringWithSelection"),
                                                                         m_dontOverrideSearchStringWithSelection);
            m_showDebugOnRun = XmlUtils::ReadBool(node, wxT("ShowDebugOnRun"), m_showDebugOnRun);
            m_caretUseCamelCase = XmlUtils::ReadBool(node, wxT("m_caretUseCamelCase"), m_caretUseCamelCase);
            m_wordWrap = XmlUtils::ReadBool(node, wxT("m_wordWrap"), m_wordWrap);
            m_dockingStyle = XmlUtils::ReadLong(node, wxT("m_dockingStyle"), m_dockingStyle);
            m_nbTabHeight = XmlUtils::ReadLong(node, wxT("m_nbTabHeight"), m_nbTabHeight);
            m_mswTheme = XmlUtils::ReadBool(node, wxT("m_mswTheme"), m_mswTheme);
            m_preferredLocale = XmlUtils::ReadString(node, wxT("m_preferredLocale"), m_preferredLocale);
            m_useLocale = XmlUtils::ReadBool(node, wxT("m_useLocale"), m_useLocale);
            m_trimOnlyModifiedLines = XmlUtils::ReadBool(node, wxT("m_trimOnlyModifiedLines"), m_trimOnlyModifiedLines);
            m_options = XmlUtils::ReadLong(node, wxT("m_options"), m_options);
            m_options2 = XmlUtils::ReadLong(node, wxT("m_options2"), m_options2);
            m_debuggerMarkerLine = XmlUtils::ReadString(node, wxT("m_debuggerMarkerLine"),
                                                        m_debuggerMarkerLine.GetAsString(wxC2S_HTML_SYNTAX));
            m_indentedComments = XmlUtils::ReadBool(node, wxT("IndentedComments"), m_indentedComments);

            // These hacks will likely be changed in the future. If so, we'll be able to remove the #include
            // "gsgsEditorConfig.h" too
            long trim = gsgs_EditorConfig().GetInteger(wxT("EditorTrimEmptyLines"), 0);
            m_TrimLine = (trim > 0);

            long appendLf = gsgs_EditorConfig().GetInteger(wxT("EditorAppendLf"), 0);
            m_AppendLF = (appendLf > 0);

            long dontTrimCaretLine = gsgs_EditorConfig().GetInteger(wxT("DontTrimCaretLine"), 0);
            m_dontTrimCaretLine = (dontTrimCaretLine > 0);

            m_outputTabsDirection =
                (wxDirection)XmlUtils::ReadLong(node, "OutputTabsDirection", (int)m_outputTabsDirection);
            m_workspaceTabsDirection =
                (wxDirection)XmlUtils::ReadLong(node, "WorkspaceTabsDirection", (int)m_workspaceTabsDirection);
    #ifdef __WXOSX__
            if(m_workspaceTabsDirection == wxLEFT) { m_workspaceTabsDirection = wxTOP; }
            if(m_workspaceTabsDirection == wxRIGHT) { m_workspaceTabsDirection = wxBOTTOM; }
    #endif

            m_webSearchPrefix = XmlUtils::ReadString(node, wxT("m_webSearchPrefix"), m_webSearchPrefix);
        }

        // Transitional calls. These checks are relevant for 2 years i.e. until the beginning of 2016
        if(m_bookmarkFgColours.empty()) {
            // This must be the first time with multiple BMs, so rescue any old user-set value
            m_bookmarkFgColours = SetDefaultBookmarkColours();
            wxString oldcolour = XmlUtils::ReadString(node, "BookmarkFgColour", "#FF0080");
            SetBookmarkFgColour(oldcolour, 0);
        }
        if(m_bookmarkBgColours.empty()) {
            m_bookmarkBgColours = SetDefaultBookmarkColours();
            wxString oldcolour = XmlUtils::ReadString(node, "BookmarkBgColour", "#FF0080");
            SetBookmarkBgColour(oldcolour, 0);
        }
    }
    // --------------------------------------------------------------
    OptionsConfig::~OptionsConfig(void) {}
    // --------------------------------------------------------------
    wxXmlNode* OptionsConfig::ToXml() const
    {
        wxXmlNode* n = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Options"));
        n->XmlAddProperty(wxT("DisplayFoldMargin"), BoolToString(m_displayFoldMargin));
        n->XmlAddProperty(wxT("UnderlineFoldedLine"), BoolToString(m_underlineFoldLine));
        n->XmlAddProperty(wxT("FoldStyle"), m_foldStyle);
        n->XmlAddProperty(wxT("DisplayBookmarkMargin"), BoolToString(m_displayBookmarkMargin));
        n->XmlAddProperty(wxT("BookmarkShape"), m_bookmarkShape);
        n->XmlAddProperty(wxT("BookmarkBgColours"), m_bookmarkBgColours);
        n->XmlAddProperty(wxT("BookmarkFgColours"), m_bookmarkFgColours);
        n->XmlAddProperty(wxT("BookmarkLabels"), m_bookmarkLabels);
        n->XmlAddProperty(wxT("ClearHighlitWordsOnFind"), BoolToString(m_clearHighlitWordsOnFind));
        n->XmlAddProperty(wxT("HighlightCaretLine"), BoolToString(m_highlightCaretLine));
        n->XmlAddProperty(wxT("ShowLineNumber"), BoolToString(m_displayLineNumbers));
        n->XmlAddProperty(wxT("RelativeLineNumber"), BoolToString(m_relativeLineNumbers));
        n->XmlAddProperty("HighlightCurLineNumber", BoolToString(m_highlightCurLineNumber));
        n->XmlAddProperty(wxT("IndentationGuides"), BoolToString(m_showIndentationGuidelines));
        n->XmlAddProperty(wxT("CaretLineColour"), m_caretLineColour.GetAsString(wxC2S_HTML_SYNTAX));
        n->XmlAddProperty(wxT("IndentUsesTabs"), BoolToString(m_indentUsesTabs));
        n->XmlAddProperty(wxT("FoldCompact"), BoolToString(m_foldCompact));
        n->XmlAddProperty(wxT("FoldAtElse"), BoolToString(m_foldAtElse));
        n->XmlAddProperty(wxT("FoldPreprocessor"), BoolToString(m_foldPreprocessor));
        n->XmlAddProperty(wxT("HighlightMatchedBraces"), BoolToString(m_highlightMatchedBraces));
        n->XmlAddProperty(wxT("FoldBgColour"), m_foldBgColour.GetAsString(wxC2S_HTML_SYNTAX));
        n->XmlAddProperty(wxT("AutoAdjustHScrollBarWidth"), BoolToString(m_autoAdjustHScrollBarWidth));
        n->XmlAddProperty(wxT("HideChangeMarkerMargin"), BoolToString(m_hideChangeMarkerMargin));
        n->XmlAddProperty(wxT("ScrollBeyondLastLine"), BoolToString(m_scrollBeyondLastLine));
        n->XmlAddProperty(wxT("HideOutputPaneOnUserClick"), BoolToString(m_hideOutpuPaneOnUserClick));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfBuild"), BoolToString(m_hideOutputPaneNotIfBuild));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfSearch"), BoolToString(m_hideOutputPaneNotIfSearch));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfReplace"), BoolToString(m_hideOutputPaneNotIfReplace));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfReferences"), BoolToString(m_hideOutputPaneNotIfReferences));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfOutput"), BoolToString(m_hideOutputPaneNotIfOutput));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfTrace"), BoolToString(m_hideOutputPaneNotIfTrace));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfTasks"), BoolToString(m_hideOutputPaneNotIfTasks));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfBuildQ"), BoolToString(m_hideOutputPaneNotIfBuildQ));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfCppCheck"), BoolToString(m_hideOutputPaneNotIfCppCheck));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfSvn"), BoolToString(m_hideOutputPaneNotIfSvn));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfCscope"), BoolToString(m_hideOutputPaneNotIfCscope));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfGit"), BoolToString(m_hideOutputPaneNotIfGit));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfDebug"), BoolToString(m_hideOutputPaneNotIfDebug));
        n->XmlAddProperty(wxT("HideOutputPaneNotIfMemCheck"), BoolToString(m_hideOutputPaneNotIfMemCheck));
        n->XmlAddProperty(wxT("FindBarAtBottom"), BoolToString(m_findBarAtBottom));
        n->XmlAddProperty(wxT("ShowReplaceBar"), BoolToString(m_showReplaceBar));
        n->XmlAddProperty(wxT("DisableSmartIndent"), BoolToString(m_disableSmartIndent));
        n->XmlAddProperty(wxT("DisableSemicolonShift"), BoolToString(m_disableSemicolonShift));
        n->XmlAddProperty(wxT("DontAutoFoldResults"), BoolToString(m_dontAutoFoldResults));
        n->XmlAddProperty(wxT("DontOverrideSearchStringWithSelection"), BoolToString(m_dontOverrideSearchStringWithSelection));
        n->XmlAddProperty(wxT("ShowDebugOnRun"), BoolToString(m_showDebugOnRun));
        n->XmlAddProperty(wxT("ConsoleCommand"), m_programConsoleCommand);
        n->XmlAddProperty(wxT("EOLMode"), m_eolMode);
        n->XmlAddProperty(wxT("m_caretUseCamelCase"), BoolToString(m_caretUseCamelCase));
        n->XmlAddProperty(wxT("m_wordWrap"), BoolToString(m_wordWrap));
        n->XmlAddProperty(wxT("m_dockingStyle"), wxString::Format(wxT("%d"), m_dockingStyle));
        n->XmlAddProperty(wxT("m_nbTabHeight"), wxString::Format(wxT("%d"), m_nbTabHeight));
        n->XmlAddProperty(wxT("m_mswTheme"), BoolToString(m_mswTheme));
        n->XmlAddProperty(wxT("m_preferredLocale"), m_preferredLocale);
        n->XmlAddProperty(wxT("m_useLocale"), BoolToString(m_useLocale));
        n->XmlAddProperty(wxT("m_trimOnlyModifiedLines"), BoolToString(m_trimOnlyModifiedLines));
        n->XmlAddProperty(wxT("m_debuggerMarkerLine"), m_debuggerMarkerLine.GetAsString(wxC2S_HTML_SYNTAX));
        n->XmlAddProperty(wxT("OutputTabsDirection"), wxString() << (int)m_outputTabsDirection);
        n->XmlAddProperty(wxT("WorkspaceTabsDirection"), wxString() << (int)m_workspaceTabsDirection);
        n->XmlAddProperty(wxT("IndentedComments"), BoolToString(m_indentedComments));
        n->XmlAddProperty(wxT("CopyLineEmptySelection"), BoolToString(m_copyLineEmptySelection));

        wxString tmp;
        tmp << m_indentWidth;
        n->XmlAddProperty(wxT("IndentWidth"), tmp);

        tmp.clear();
        tmp << m_tabWidth;
        n->XmlAddProperty(wxT("TabWidth"), tmp);

        tmp.clear();
        tmp << m_iconsSize;
        n->XmlAddProperty(wxT("ToolbarIconSize"), tmp);

        tmp.clear();
        tmp << m_showWhitspaces;
        n->XmlAddProperty(wxT("ShowWhitespaces"), tmp);

        tmp.clear();
        tmp << m_edgeMode;
        n->XmlAddProperty(wxT("EdgeMode"), tmp);

        tmp.clear();
        tmp << m_edgeColumn;
        n->XmlAddProperty(wxT("EdgeColumn"), tmp);

        n->XmlAddProperty(wxT("EdgeColour"), m_edgeColour.GetAsString(wxC2S_HTML_SYNTAX));

        tmp.clear();
        tmp << m_caretWidth;
        n->XmlAddProperty(wxT("CaretWidth"), tmp);

        tmp.clear();
        tmp << m_caretBlinkPeriod;
        n->XmlAddProperty(wxT("CaretBlinkPeriod"), tmp);

        tmp.clear();
        tmp << m_caretLineAlpha;
        n->XmlAddProperty(wxT("CaretLineAlpha"), tmp);

        tmp.clear();
        tmp = wxFontMapper::GetEncodingName(m_fileFontEncoding);
        n->XmlAddProperty(wxT("FileFontEncoding"), tmp);

        tmp.Clear();
        tmp << m_options;
        n->XmlAddProperty(wxT("m_options"), tmp);

        tmp.Clear();
        tmp << m_options2;
        n->XmlAddProperty(wxT("m_options2"), tmp);

        n->XmlAddProperty(wxT("m_webSearchPrefix"), m_webSearchPrefix);

        return n;
    }
    // --------------------------------------------------------------
    void OptionsConfig::SetFileFontEncoding(const wxString& strFileFontEncoding)
    {
        this->m_fileFontEncoding = wxFontMapper::Get()->CharsetToEncoding(strFileFontEncoding, false);

        if(wxFONTENCODING_SYSTEM == this->m_fileFontEncoding) { this->m_fileFontEncoding = wxFONTENCODING_UTF8; }
    }
    // --------------------------------------------------------------
    wxString OptionsConfig::GetEOLAsString() const
    {
        if(GetEolMode() == wxT("Unix (LF)")) {
            return "\n";

        } else if(GetEolMode() == wxT("Mac (CR)")) {
            return "\r";

        } else if(GetEolMode() == wxT("Windows (CRLF)")) {
            return "\r\n";

        } else {
            return "\n";
        }
    }
    // --------------------------------------------------------------
    wxColour OptionsConfig::GetBookmarkFgColour(size_t index) const
    {
        wxColour col;
        wxArrayString arr = wxSplit(m_bookmarkFgColours, ';');
        if(index < arr.GetCount()) { return wxColour(arr.Item(index)); }

        return col;
    }
    // --------------------------------------------------------------
    void OptionsConfig::SetBookmarkFgColour(wxColour c, size_t index)
    {
        wxArrayString arr = wxSplit(m_bookmarkFgColours, ';');
        if(index < arr.GetCount()) {
            arr.Item(index) = c.GetAsString(wxC2S_HTML_SYNTAX);
            m_bookmarkFgColours = wxJoin(arr, ';');
        }
    }
    // --------------------------------------------------------------
    wxColour OptionsConfig::GetBookmarkBgColour(size_t index) const
    {
        wxColour col;
        wxArrayString arr = wxSplit(m_bookmarkBgColours, ';');
        if(index < arr.GetCount()) { return wxColour(arr.Item(index)); }

        return col;
    }
    // --------------------------------------------------------------
    void OptionsConfig::SetBookmarkBgColour(wxColour c, size_t index)
    {
        wxArrayString arr = wxSplit(m_bookmarkBgColours, ';');
        if(index < arr.GetCount()) {
            arr.Item(index) = c.GetAsString(wxC2S_HTML_SYNTAX);
            m_bookmarkBgColours = wxJoin(arr, ';');
        }
    }
    // --------------------------------------------------------------
    wxString OptionsConfig::GetBookmarkLabel(size_t index) const
    {
        wxArrayString arr = wxSplit(m_bookmarkLabels, ';');
        if(index < arr.GetCount()) { return arr.Item(index); }

        return "";
    }
    // --------------------------------------------------------------
    void OptionsConfig::SetBookmarkLabel(const wxString& label, size_t index)
    {
        wxArrayString arr = wxSplit(m_bookmarkLabels, ';');
        if(index < arr.GetCount()) {
            arr.Item(index) = label;
            m_bookmarkLabels = wxJoin(arr, ';');
        }
    }
    // --------------------------------------------------------------
    void OptionsConfig::UpdateFromEditorConfig(const EditorConfigSection& section)
    {
        if(section.IsInsertFinalNewlineSet()) { this->SetAppendLF(section.IsInsertFinalNewline()); }
        if(section.IsSetEndOfLineSet()) {
            // Convert .editorconfig to gsgs strings
            wxString eolMode = "Unix (LF)"; // default
            if(section.GetEndOfLine() == "crlf") {
                eolMode = "Windows (CRLF)";
            } else if(section.GetEndOfLine() == "cr") {
                eolMode = "Mac (CR)";
            }
            this->SetEolMode(eolMode);
        }
        if(section.IsTabWidthSet()) { this->SetTabWidth(section.GetTabWidth()); }
        if(section.IsIndentStyleSet()) { this->SetIndentUsesTabs(section.GetIndentStyle() == "tab"); }
        if(section.IsTabWidthSet()) { this->SetTabWidth(section.GetTabWidth()); }
        if(section.IsIndentSizeSet()) { this->SetIndentWidth(section.GetIndentSize()); }
        if(section.IsCharsetSet()) {
            // TODO: fix the locale here
        }
    }
    // --------------------------------------------------------------
    bool OptionsConfig::IsTabColourDark() const
    {
    #if USE_AUI_NOTEBOOK
        return false;
    #else
        return HasOption(Opt_TabColourDark);
    #endif
    }
    // --------------------------------------------------------------
    bool OptionsConfig::IsTabColourMatchesTheme() const
    {
    #if USE_AUI_NOTEBOOK
        return true;
    #else
        return !HasOption(Opt_TabColourPersistent);
    #endif
    }
    // --------------------------------------------------------------
    // --------------------------------------------------------------
    //
    // --------------------------------------------------------------
    // --------------------------------------------------------------
    TreeBookNode::TreeBookNode()
    {
    }
    // --------------------------------------------------------------
    TreeBookNode::~TreeBookNode()
    {
    }
    // --------------------------------------------------------------
}