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

#ifndef GSGS_SYMBOL_TREE_H
#define GSGS_SYMBOL_TREE_H

#include "gsgsPreInclude.h"
#include "gsgsImageManager.h"
#include "gsgsTreeCtrl.h"
#include "gsgsTagManager.h"
#include "gsgsTag.h"
#include "gsgsParseWorker.h"

namespace gsgs
{
    class _gsgsAPI MyTreeItemData : public wxTreeItemData
    {
    public:
        MyTreeItemData(const wxString& filename, const wxString& pattern, int lineno = wxNOT_FOUND)
            : m_fileName(filename)
            , m_pattern(pattern)
            , m_lineno(lineno)
        {
        }

        inline const wxString& GetFileName() const { return m_fileName; }
        inline const wxString& GetPattern() const { return m_pattern; }
        inline int GetLine() const { return m_lineno; }
    private:
        wxString m_fileName;
        wxString m_pattern;
        int m_lineno;
    };

    /**
     * Class SymbolTree, a tree that contains the language symbols from SQLite database.
     *
     * \date 08-18-2006
     * \author Eran
     *
     */
    class _gsgsAPI SymbolTree : public DefaultTreeCtrl
    {
    public:
        SymbolTree();

        /**
         * Parameterized constructor.
         * \param parent Tree parent window
         * \param id Window id
         * \param pos Window position
         * \param size Window size
         * \param style Window style
         */
        SymbolTree(wxWindow* parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0);

        /**
         * @brief clear the tree content
         */
        virtual void Clear();

        /**
         * Destructor .
         */
        virtual ~SymbolTree();

        /**
         * Create tree, usually called after constructing SymbolTree with default constructor.
         * \param parent Tree parent window
         * \param id Window id
         * \param pos Window position
         * \param size Window size
         * \param style Window style
         */
        /*virtual void Create(wxWindow* parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTR_HIDE_ROOT);*/

        /**
         * Construct a outline tree for fileName
         * possible return values:
         *
         */
        virtual void BuildTree(const wxFileName& fileName, const TagEntryPtrList& tags, bool forceBuild = false);

        void AddSymbols(const std::vector<std::pair<wxString, TagEntry> >& items);

        void DeleteSymbols(const std::vector<std::pair<wxString, TagEntry> >& items);

        void UpdateSymbols(const std::vector<std::pair<wxString, TagEntry> >& items);

        /**
         * return the file name assocaited with this symbol tree
         */
        inline const wxFileName& GetFilename() const { return m_fileName; }

        /**
         * \brief select item by its name and select it. If multiple matches
         * fits 'name' the first one is selected
         * \param name disply name of the item to be selected (can be partial name)
         */
        void SelectItemByName(const wxString& name);

        inline void SetSortByLineNumber(bool num) { m_sortByLineNumber = num; }
        inline bool IsSortByLineNumber() const { return m_sortByLineNumber; }

    protected:
        bool Matches(const wxTreeItemId& item, const wxString& patter);

        void GetItemChildrenRecursive(wxTreeItemId& parent, std::map<void*, bool>& deletedMap);

        /**
         * Add an item to the gui tree.
         * \param node Node to add
         */
        void AddItem(TagNode* node);

        /**
         * Return the icon index according to item kind and access.
         * \param kind Item kind (class, namespace etc)
         * \param access One of $public$, $private$, $protected$, or $wxEmptyString$
         * \return icon index
         */
        int GetItemIconIndex(const wxString& kind, const wxString& access = wxEmptyString);

        /**
         * Initialise the image: icon index map.
         */
        void InitialiseSymbolMap();

        /**
         * Update gui item with new data
         * \param data new data
         * \param key node key
         */
        void UpdateGuiItem(TagEntry& data, const wxString& key);

        void SelectFirstItem();
    protected:
        std::map<wxString, int> m_imagesMap;
        std::map<wxString, bool> m_globalsKind;
        std::map<wxString, void*> m_items;
        wxTreeItemId m_globalsNode;
        wxTreeItemId m_prototypesNode;
        wxTreeItemId m_macrosNode;
        wxFileName m_fileName;
        TagTreePtr m_tree;
        TagEntryPtrList m_currentTags;
        bool m_sortByLineNumber;
    };
}
#endif
