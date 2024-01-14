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
#include "gsgsPreProcess.h"
#include "gsgsTagManager.h"
#include "gsgsTipInfo.h"
#include "wx/settings.h"

//#ifdef __VISUALC__
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
//#endif

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // tagCallTipInfo
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    struct tagCallTipInfo
    {
        wxString sig;
        wxString retValue;
        std::vector<std::pair<int, int> > paramLen;
    };
    //-----------------------------------------------------------------------------------
    TipInfo::TipInfo() : m_curr(0){}
    //-----------------------------------------------------------------------------------
    TipInfo::TipInfo(const std::vector<TagEntryPtr> & tips)
        : m_curr(0)
    {
        Initialize(tips);
    }
    //-----------------------------------------------------------------------------------
    TipInfo::TipInfo(const TipInfo & rhs) 
    { 
        *this = rhs; 
    }
    //-----------------------------------------------------------------------------------
    TipInfo & TipInfo::operator=(const TipInfo & rhs)
    {
        if (this == &rhs) 
            return *this;
        m_tips = rhs.m_tips;
        m_curr = rhs.m_curr;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    wxString TipInfo::First()
    {
        m_curr = 0;
        if (m_tips.empty()) 
            return wxEmptyString;
        return TipAt(0);
    }
    //-----------------------------------------------------------------------------------
    wxString TipInfo::TipAt(int at)
    {
        wxString tip;
        if (m_tips.size() > 1) {
            tip << m_tips.at(at).str;
        }
        else {
            tip << m_tips.at(0).str;
        }
        return tip;
    }
    //-----------------------------------------------------------------------------------
    wxString TipInfo::Next()
    {
        // format a tip string and return it
        if (m_tips.empty()) return wxEmptyString;

        m_curr++;
        if (m_curr >= (int)m_tips.size()) 
        {
            m_curr = 0;
        } // if( m_curr >= m_tips.size() )

        return TipAt(m_curr);
    }
    //-----------------------------------------------------------------------------------
    wxString TipInfo::Prev()
    {
        // format a tip string and return it
        if (m_tips.empty()) return wxEmptyString;

        m_curr--;
        if (m_curr < 0) {
            m_curr = (int)m_tips.size() - 1;
        }
        return TipAt(m_curr);
    }
    //-----------------------------------------------------------------------------------
    wxString TipInfo::All()
    {
        wxString tip;
        for (size_t i = 0; i < m_tips.size(); i++) {
            tip << m_tips.at(i).str << wxT("\n");
        }
        tip.RemoveLast();
        return tip;
    }
    //-----------------------------------------------------------------------------------
    int TipInfo::Count() const 
    { 
        return (int)m_tips.size(); 
    }
    //-----------------------------------------------------------------------------------
    void TipInfo::Initialize(const std::vector<TagEntryPtr>& tags) 
    { 
        FormatTagsToTips(tags, m_tips); 
    }
    //-----------------------------------------------------------------------------------
    void TipInfo::GetHighlightPos(int index, int& start, int& len)
    {
        start = wxNOT_FOUND;
        len = wxNOT_FOUND;
        if (m_curr >= 0 && m_curr < (int)m_tips.size()) 
        {
            Info ti = m_tips.at(m_curr);
            int base = ti.str.Find(wxT("("));

            // sanity
            if (base != wxNOT_FOUND && index < (int)ti.paramLen.size() && index >= 0) {
                start = ti.paramLen.at(index).first + base;
                len = ti.paramLen.at(index).second;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    wxString TipInfo::Current()
    {
        // format a tip string and return it
        if (m_tips.empty()) 
            return wxEmptyString;

        if (m_curr >= (int)m_tips.size() || m_curr < 0) 
        {
            m_curr = 0;
        }
        return TipAt(m_curr);
    }
    //-----------------------------------------------------------------------------------
    void TipInfo::SelectSiganture(const wxString& signature)
    {
        // search for a match
        for (size_t i = 0; i < m_tips.size(); ++i) 
        {
            if (m_tips.at(i).str == signature) {
                m_curr = i;
                break;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void TipInfo::FormatTagsToTips(const TagEntryPtrList& tags, std::vector<Info>& tips)
    {
        std::map<wxString, tagCallTipInfo> mymap;
        for (size_t i = 0; i < tags.size(); i++) 
        {
            tagCallTipInfo cti;
            TagEntryPtr t = tags.at(i);

            // Use basic signature
            if (t->GetFlags() & TagEntry::Tag_No_Signature_Format) 
            {
                wxString raw_sig = t->GetSignature();
                int startOffset(0);
                if (raw_sig.Find(wxT("(")) != wxNOT_FOUND) {
                    startOffset = raw_sig.Find(wxT("(")) + 1;
                }

                // Remove the open / close brace
                wxString tmpsig = raw_sig;
                tmpsig.Trim().Trim(false); // remove any whitespaces from right

                size_t j = 0;
                size_t depth = -1; // we start collecting after we find the open paren
                for (; j < tmpsig.length(); ++j) {
                    if (tmpsig.at(j) == '(') {
                        ++depth;
                    }
                    else if (tmpsig.at(j) == ')') {
                        --depth;
                    }
                    else if (tmpsig.GetChar(j) == wxT(',') && (depth == 0)) {
                        std::pair<int, int> p;
                        p.first = startOffset;
                        p.second = (j - startOffset);
                        cti.paramLen.push_back(p);
                        startOffset = j + 1;
                    }
                }

                if (startOffset != (int)j) {
                    std::pair<int, int> p;
                    p.first = startOffset;
                    p.second = (j - startOffset -
                        1); // -1 here since its likely that the signature ends with a ")" so don't include it
                    cti.paramLen.push_back(p);
                }
                cti.sig = raw_sig;
                mymap[raw_sig] = cti;

            }
            else {
                if (t->IsMethod()) {

                    wxString raw_sig(t->GetSignature().Trim().Trim(false));

                    // evaluate the return value of the tag
                    cti.retValue = TagsManagerST::Get()->GetFunctionReturnValueFromPattern(t);

                    bool hasDefaultValues = (raw_sig.Find(wxT("=")) != wxNOT_FOUND);

                    // the key for unique entries is the function prototype without the variables names and
                    // any default values
                    wxString key = TagsManagerST::Get()->NormalizeFunctionSig(raw_sig, Normalize_Func_Reverse_Macro);

                    // the signature that we want to keep is one with name & default values, so try and get the maximum out
                    // of the
                    // function signature
                    wxString full_signature = TagsManagerST::Get()->NormalizeFunctionSig(
                        raw_sig,
                        Normalize_Func_Name | Normalize_Func_Default_value | Normalize_Func_Reverse_Macro,
                        &cti.paramLen);
                    cti.sig = full_signature;

                    if (hasDefaultValues) {
                        // incase default values exist in this prototype,
                        // update/insert this signature
                        mymap[key] = cti;
                    }

                    // make sure we dont add duplicates
                    if (mymap.find(key) == mymap.end()) {
                        // add it
                        mymap[key] = cti;
                    }

                }
                else {
                    // macro
                    wxString macroName = t->GetName();
                    wxString pattern = t->GetPattern();

                    int where = pattern.Find(macroName);
                    if (where != wxNOT_FOUND) {
                        // remove the #define <name> from the pattern
                        pattern = pattern.Mid(where + macroName.Length());
                        pattern = pattern.Trim().Trim(false);
                        if (pattern.StartsWith(wxT("("))) {
                            // this macro has the form of a function
                            pattern = pattern.BeforeFirst(wxT(')'));
                            pattern.Append(wxT(')'));
                            cti.sig = pattern.Trim().Trim(false);
                            mymap[cti.sig] = cti;
                        }
                    }
                }
            }
        }

        std::map<wxString, tagCallTipInfo>::iterator iter = mymap.begin();
        tips.clear();
        for (; iter != mymap.end(); iter++)
        {
            wxString tip;
            if (iter->second.retValue.empty() == false)
            {
                tip << iter->second.retValue.Trim(false).Trim() << wxT(" : ");
            }
            tip << iter->second.sig;
            Info ti;
            ti.paramLen = iter->second.paramLen;
            ti.str = tip;
            tips.push_back(ti);
        }
    }
    //-----------------------------------------------------------------------------------
    bool TipInfo::SelectTipToMatchArgCount(size_t argcount)
    {
        for (size_t i = 0; i < m_tips.size(); ++i)
        {
            if (m_tips.at(i).paramLen.size() > argcount)
            {
                m_curr = i;
                return true;
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    TipTree::TipTree(wxWindow *parent, wxWindowID id, long style)
    : wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style)
    {
        SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    }
    //-----------------------------------------------------------------------------------
    TipTree::~TipTree()
    {
        DeleteAllItems();
    }
    //-----------------------------------------------------------------------------------
    void TipTree::BuildTree(TreeNode<wxString, NodeData> *tree)
    {
        if(!tree){
            return;
        }
        //Walk over the tree and construct it
        Freeze();
        DeleteAllItems();
        std::map<void*, bool> m_sortItems;
        std::list< wxTreeItemId > rootChilds;
        
        //create the tree
        wxTreeItemId root = AddRoot(tree->GetData().name);
        tree->GetData().itemId = root;
        TreeWalker<wxString, NodeData> walker(tree);
        
        for(; !walker.End(); walker++)
        {
            // Add the item to the tree
            TreeNode<wxString, NodeData>* node = walker.GetNode();

            // Skip root node
            if(node->IsRoot())
                continue;
            
            wxTreeItemId parentHti = node->GetParent()->GetData().itemId;
            if(parentHti.IsOk() == false){
                parentHti = root;
            }

            //add the item to the tree
            node->GetData().itemId = AppendItem(
                                                    parentHti,				// parent
                                                    node->GetData().name,	// display name
                                                    wxNOT_FOUND,			// item image index
                                                    wxNOT_FOUND,			// selected item image
                                                    new TipTreeData(node->GetData().name)
                                                );
            m_sortItems[parentHti.m_pItem] = true;
            if(parentHti == root){
                rootChilds.push_back(node->GetData().itemId);
            }
        }
        SortTree(m_sortItems);
        
        //expand all the root direct children
        std::list<wxTreeItemId>::iterator iter = rootChilds.begin();
        for(; iter != rootChilds.end(); iter++){
            wxTreeItemId item = (*iter);
            if(item.IsOk() && ItemHasChildren(item)){
                Expand(item);
            }
        }
        delete tree;
        Thaw();
    }
    //-----------------------------------------------------------------------------------
    void TipTree::SortTree(std::map<void*, bool> & nodes)
    {
        std::map<void*, bool>::iterator iter = nodes.begin();
        for(; iter != nodes.end(); iter++){
            wxTreeItemId item = iter->first;
            if(item.IsOk()){
                // Does this node has children?
                if( GetChildrenCount( item ) == 0 )
                    continue;
                SortChildren(item); 
            }
        }
    }
    //-----------------------------------------------------------------------------------
}