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
#ifndef GSGS_TipInfo_H
#define GSGS_TipInfo_H

#include "gsgsPreInclude.h"
#include "gsgsTag.h"
#include "gsgsEvent.h"
#include "debuggerobserver.h"
#include "wx/treectrl.h"

namespace gsgs
{
    class _gsgsAPI TipInfo
    {
    public:
        struct Info
        {
            wxString str;
            std::vector<std::pair<int, int> > paramLen;
        };
        std::vector<Info> m_tips;
        int m_curr;
        void Initialize(const std::vector<TagEntryPtr> &tags);

    public:
        TipInfo();
        TipInfo(const std::vector<TagEntryPtr> & tips );
        TipInfo(const TipInfo& rhs);
        virtual ~TipInfo() {}

        TipInfo& operator=(const TipInfo& o);

        wxString Next() ;
        wxString Prev() ;

        /**
         * return the first tip
         */
        wxString First();

        /**
         * @brief return the current tip
         * @return return the current tip
         */
        wxString Current();
        
        /**
         * @brief select the first tip that has at least argcount
         * @return true if we managed to find this tip, false otherwise
         */
        bool SelectTipToMatchArgCount(size_t argcount);
        
        /**
         * Return number of tips stored in this object
         * \return number of tips
         */
        int Count() const;

        /**
         * \brief return all tips as a single string
         */
        wxString All();

        /**
         * @brief get the highlight offset & width for the current tip
         * @param index paramter index
         * @param start [output]
         * @param len [output]
         */
        void GetHighlightPos(int index, int & start, int &len);

        inline int GetCurr() const { return m_curr; }
        
        /**
         * @brief set the tip to a specific tag
         */
        void SelectSiganture( const wxString & signature );
        
        /**
         * @brief format list of tags into calltips
         */
        static void FormatTagsToTips(const TagEntryPtrList & tags, std::vector<Info> & tips);

    private:
        wxString TipAt(int at);
    };
    
    class _gsgsAPI TipTreeData : public wxTreeItemData
    {
        wxString m_displayString;
    public:
        TipTreeData(const wxString &displayStr) : m_displayString(displayStr){}
        virtual ~TipTreeData(){}
    };

    class _gsgsAPI TipTree : public wxTreeCtrl
    {
    protected:
        void SortTree(std::map<void*, bool> & nodes);

    public:
        TipTree(wxWindow *parent, wxWindowID id, long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
        virtual ~TipTree();

        void BuildTree(TreeNode<wxString, NodeData> *tree);
    };
}
typedef gsgs::SharedPtr<gsgs::TipInfo> gsgsTipInfoPtr;
#endif
