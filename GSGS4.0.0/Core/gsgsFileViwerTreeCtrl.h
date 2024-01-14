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

#ifndef gsgsFILEVIWERTREECTRL_H
#define gsgsFILEVIWERTREECTRL_H

#include "gsgsPreInclude.h"
#include "gsgsTreeCtrl.h"
#include <wx/treectrl.h>

namespace gsgs
{
    /**
     * @class TreeNodeIndex
     * @brief a helper class for fast searching a folder in the tree view
     */
    class _gsgsAPI TreeNodeIndex
    {
    public:
        TreeNodeIndex() {}
        virtual ~TreeNodeIndex() {}

        wxTreeItemId Find(const wxString& path)
        {
            if(m_children.count(path)) {
                return m_children.find(path)->second;
            }
            return wxTreeItemId();
        }
        void Add(const wxString& path, const wxTreeItemId& item) { m_children.insert(std::make_pair(path, item)); }
        void Delete(const wxString& name) { m_children.erase(name); }
        void Clear() { m_children.clear(); }
    private:
        std::unordered_map<wxString, wxTreeItemId> m_children;
    };

    // Item data class
    class _gsgsAPI TreeCtrlData : public wxTreeItemData
    {
    public:
        enum eKind {
            kDummy = -1,
            kRoot,   // the hidden root folder
            kFile,   // a file
            kFolder, // a folder
        };
    public:
        TreeCtrlData(eKind kind)
            : m_kind(kind)
            , m_index(NULL)
        {
            if(IsFolder()) {
                m_index = new TreeNodeIndex();
            }
        }
        virtual ~TreeCtrlData() { wxDELETE(m_index); }

        TreeNodeIndex* GetIndex() { return m_index; }

        void SetKind(const eKind& kind) { this->m_kind = kind; }
        const eKind& GetKind() const { return m_kind; }

        /**
         * @brief set the path and update the display name
         */
        void SetPath(const wxString& path)
        {
            this->m_path = path;
            if(IsFolder()) {
                wxFileName fn(m_path, "");
                if(fn.GetDirCount()) {
                    m_name = fn.GetDirs().Last();
                } else {
                    m_name = m_path;
                }
            } else if(IsFile()) {
                wxFileName fn(m_path);
                m_name = fn.GetFullName();
            } else {
                m_name = wxEmptyString;
            }
        }

        /**
         * @brief return the item's path. if it is a folder, return the folder path. If it is a file
         * return the file's fullpath
         */
        const wxString& GetPath() const { return m_path; }

        /**
         * @brief return the display name
         */
        const wxString& GetName() const { return m_name; }

        // Aliases
        bool IsFolder() const { return m_kind == kFolder; }
        bool IsFile() const { return m_kind == kFile; }
        bool IsDummy() const { return m_kind == kDummy; }
    private:
        eKind m_kind;
        // If file, contains the fullpath otherwise, contains the path
        wxString m_path;
        wxString m_name;
        TreeNodeIndex* m_index;
    };

    class _gsgsAPI FileViewerTreeCtrl : public DefaultTreeCtrl
    {
    public:
        FileViewerTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTR_DEFAULT_STYLE | wxTR_MULTIPLE | wxTR_HIDE_ROOT | wxBORDER_STATIC);
        virtual ~FileViewerTreeCtrl();
    };
}
#endif
