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

#ifndef gsgs_TREE_H
#define gsgs_TREE_H

#include <map>
#include <iostream>
#include <vector>

namespace gsgs
{
    template <class TKey, class TData>
    class TreeNode
    {
        TKey  m_key;
        TData m_data;
        TreeNode* m_parent;
        std::map<TreeNode*, TreeNode*> m_childs;
        typename std::map<TreeNode*, TreeNode*>::iterator m_pos;

    public:
        /**
        * Constructor, constructs a tree node with key and data.
        * If parent is left NULL, node is assumed to be root.
        * \param key Node key, must have operator =, <
        * \param data Node data, this class must have operator=
        * \param parent Parent node
        */
        TreeNode(const TKey& key, const TData& data, TreeNode* parent = NULL);

        /**
        * Destructor, deletes this node and all its children.
        */
        virtual ~TreeNode();

        //---------------------------------------------------
        // Setters / Getters
        //---------------------------------------------------

        /**
        * Get node data.
        * \return node data
        */
        inline TData& GetData()
        {
            return m_data;
        };

        /**
        * Get node key.
        * \return key
        */
        inline TKey& GetKey()
        {
            return m_key;
        };

        /**
        * Set node data.
        * \param data data
        */
        inline void SetData(const TData& data)
        {
            m_data = data;
        };

        /**
        * Set node key.
        * \param key Key
        */
        inline void SetKey(const TKey& key)
        {
            m_key = key;
        };

        /**
         * Return the parent node of this node (or null if root).
         * \return pointer to parent node
         */
        inline TreeNode* GetParent()
        {
            return m_parent;
        }

        //---------------------------------------------
        // Misc
        //---------------------------------------------

        /**
        * Print the tree to stream (default is stdout).
        */
        void Print(std::ostream& stream = std::cout, int depth = 0);

        /**
        * Check if this node is root.
        * \return true if node is root
        */
        inline bool IsRoot() const
        {
            return m_parent == NULL;
        };

        //----------------------------------------------
        // Operations
        //----------------------------------------------
        /**
        * Add new child node to this node.
        * Duplicate nodes are allowed. However, Remove() will remove the first occurance of a node by a given key.
        * \param key Node key
        * \param data Node data
        * \return newly added node
        */
        TreeNode* AddChild(const TKey& key, const TData& data);

        /**
         * \brief Append new child to this tree node
         * \param newNode node to append, must be allocated on the heap
         * \return the newly added tree node
         */
        TreeNode* AddChild(TreeNode* newNode);

        /**
        * Remove first occurance of node with a given key.
        * If the node to be removed is the root, a std::exception* is thrown, which must be deleted by caller.
        * Remove does not delete the memory allocated for the node. The user must delete the removed node.
        * \param key Node key
        * \return removed node
        */
        TreeNode* Remove(const TKey& key);

        /**
        * Find a node by a key.
        * \param key Node key
        * \return node, or NULL if non exist
        */
        TreeNode* Find(const TKey& key);

        /**
         * Set the pointer to the first child of this node .
         */
        void First();

        /**
         * Return the next child of this node.
         */
        TreeNode* Next();

        /**
         * Test if this node has more childs.
         * This call is usually used before issuing a Next() call.
         * \return true if the next call to Next() will return a valid child pointer
         */
        bool HasMore();
        inline std::map<TreeNode*, TreeNode*>& GetChilds() { return m_childs; }
    };

    template <class TKey, class TData>
    TreeNode<TKey, TData>::TreeNode(const TKey& key, const TData& data, TreeNode* parent)
        : m_key(key), m_data(data), m_parent(parent)
    {
    }

    template <class TKey, class TData>
    TreeNode<TKey, TData>::~TreeNode()
    {
        typename std::map<TreeNode*, TreeNode*>::iterator iter = m_childs.begin();
        for (; iter != m_childs.end(); iter++)
        {
            delete iter->second;
        }
        m_childs.clear();
    }

    template <class TKey, class TData>
    TreeNode<TKey, TData>* TreeNode<TKey, TData>::AddChild(const TKey& key, const TData& data)
    {
        TreeNode* newNode = new TreeNode(key, data, this);
        m_childs[newNode] = newNode;
        return newNode;
    }

    template <class TKey, class TData>
    TreeNode<TKey, TData>* TreeNode<TKey, TData>::AddChild(TreeNode* newNode)
    {
        m_childs[newNode] = newNode;
        return newNode;
    }

    template <class TKey, class TData>
    TreeNode<TKey, TData>* TreeNode<TKey, TData>::Remove(const TKey& key)
    {
        TreeNode* node = Find(key);
        if (node)
        {
            if (NULL == node->m_parent)
            {
                // Cant remove root
                return NULL;
            }

            typename std::map<TreeNode*, TreeNode*>::iterator iter = node->m_parent->m_childs.find(node);
            if (iter != node->m_parent->m_childs.end())
                node->m_parent->m_childs.erase(iter);
            return node;
        }
        return NULL;
    }

    template <class TKey, class TData>
    TreeNode<TKey, TData>* TreeNode<TKey, TData>::Find(const TKey& key)
    {
        if (m_key == key)
            return this;

        typename std::map<TreeNode*, TreeNode*>::iterator iter;

        // Scan first the childs of this node
        for (iter = m_childs.begin(); iter != m_childs.end(); iter++)
        {
            if (((TreeNode*)iter->second)->GetKey() == key)
                return iter->second;
        }

        // Scan level below
        for (iter = m_childs.begin(); iter != m_childs.end(); iter++)
        {
            TreeNode* node = NULL;
            node = iter->second->Find(key);
            if (node) return node;
        }
        return NULL;
    }

    template <class TKey, class TData>
    void TreeNode<TKey, TData>::Print(std::ostream& stream, int depth)
    {
        std::string tab = " ";
        for (int i = 0; i < depth; i++)
            tab += "    ";

        stream << tab.c_str() << m_key << std::endl;
        depth++;

        typename std::map<TreeNode*, TreeNode*>::iterator iter = m_childs.begin();
        for (; iter != m_childs.end(); iter++)
            iter->second->Print(stream, depth);
    }

    /// Prepare for tree iteration in the current node
    template <class TKey, class TData>
    void TreeNode<TKey, TData>::First()
    {
        m_pos = m_childs.begin();
    }

    template <class TKey, class TData>
    TreeNode<TKey, TData>* TreeNode<TKey, TData>::Next()
    {
        if (!HasMore())
            return NULL;
        TreeNode* nextElem = m_pos->second;
        m_pos++;
        return nextElem;
    }

    template <class TKey, class TData>
    bool TreeNode<TKey, TData>::HasMore()
    {
        return (m_pos != m_childs.end());
    }

    //----------------------------------------------------------------
    // TreeWalker class
    //----------------------------------------------------------------

    template <class TKey, class TData>
    class TreeWalker
    {
    private:
        void GetChildren(TreeNode<TKey, TData>* node);
        std::vector<TreeNode<TKey, TData>* > m_children;
        size_t m_pos;

    public:
        TreeWalker(TreeNode<TKey, TData>* node);
        virtual ~TreeWalker();
        bool End();
        void operator++(int);
        TreeNode<TKey, TData>* GetNode()
        {
            if (m_pos < m_children.size())
                return m_children[m_pos];
            return NULL;
        }
    };

    template <class TKey, class TData>
    TreeWalker<TKey, TData>::TreeWalker(TreeNode<TKey, TData>* node)
        : m_pos(0)
    {
        m_children.push_back(node);
        GetChildren(node);
    }

    template <class TKey, class TData>
    TreeWalker<TKey, TData>::~TreeWalker()
    {
    }

    template <class TKey, class TData>
    void TreeWalker<TKey, TData>::GetChildren(TreeNode<TKey, TData>* node)
    {
        if (node == NULL)
            return;
        typename std::map<TreeNode<TKey, TData>*, TreeNode<TKey, TData>*>::iterator iter = node->GetChilds().begin();
        for (; iter != node->GetChilds().end(); iter++)
        {
            m_children.push_back(iter->second);
            GetChildren(iter->second);
        }
    }

    template <class TKey, class TData>
    bool TreeWalker<TKey, TData>::End()
    {
        return m_pos == m_children.size();
    }

    template <class TKey, class TData>
    void TreeWalker<TKey, TData>::operator++(int)
    {
        m_pos++;
    }

    template <typename TKey, typename TData>
    class Tree
    {
        std::map<TKey, TreeNode<TKey, TData>*> m_nodes;
        TreeNode<TKey, TData>* m_root;
    public:
        /**
         * Construct a tree with root node with Key and data.
         * \param key Root node key
         * \param data Root node data
         */
        Tree(const TKey& key, const TData& data);

        /**
         * Destructor, deletes all tree and its sub nodes.
         */
        virtual ~Tree();

        /**
         * Find node in the tree with Key.
         * \param key Search key
         * \return node
         */
        TreeNode<TKey, TData>* Find(const TKey& key);

        /**
         * Remove node (and all its sub nodes) from the tree using key as the criteria.
         * This function does not delete the removed node.
         * \param key search key
         * \return remove node
         */
        TreeNode<TKey, TData>* Remove(const TKey& key);

        /**
         * Add child node to the tree.
         * \param key New node key
         * \param data New node data
         * \param parent New node parent, if left NULL parent is set to root
         * \return newly added node
         */
        TreeNode<TKey, TData>* AddChild(const TKey& key, const TData& data, TreeNode<TKey, TData>* parent = NULL);

        /**
         * Returns tree root.
         * \return root node
         */
        TreeNode<TKey, TData>* GetRoot() { return m_root; }

        /**
         * Prints the tree in tabulated format to stream.
         * \param stream Output stream (default is set to stdout)
         * \param depth Tab depth (for internal use)
         */
        void Print(std::ostream& stream = std::cout, int depth = 0);

        /**
         * Compare this tree against another tree.
         * \param targetTree Target tree to compare with
         * \param deletedItems Array of pairs of items which exist in this tree and not in target tree
         * \param modifiedItems Array of pairs of items which have the same key but differnt data
         * \param newItems Aarray of pairs of items which exist in the target tree but not in this tree
         * \param fromNode If its set to null, comparison will start from this tree root node, if not null,
         * comparison will compare sub-tree which root of its fromNode
         */
        void Compare(Tree* targetTree, std::vector<std::pair<TKey, TData> >& deletedItems, std::vector<std::pair<TKey, TData> >& modifiedItems, std::vector<std::pair<TKey, TData> >& newItems, TreeNode<TKey, TData>* fromNode = NULL);

        /**
         * Serialize the tree to vector
         * \param vec output vector
         */
        void ToVector(std::vector<std::pair<TKey, TData> >& vec);
    };

    template <typename TKey, typename TData>
    Tree<TKey, TData>::Tree(const TKey& key, const TData& data)
    {
        m_root = new TreeNode<TKey, TData>(key, data);
    }

    template <typename TKey, typename TData>
    Tree<TKey, TData>::~Tree()
    {
        delete m_root;
    }

    template <typename TKey, typename TData>
    TreeNode<TKey, TData>* Tree<TKey, TData>::Find(const TKey& key)
    {
        typename std::map<TKey, TreeNode<TKey, TData>*>::const_iterator iter = m_nodes.find(key);
        if (iter == m_nodes.end())
            return NULL;
        return iter->second;
    }

    template <typename TKey, typename TData>
    TreeNode<TKey, TData>* Tree<TKey, TData>::AddChild(const TKey& key, const TData& data, TreeNode<TKey, TData>* parent /*NULL*/)
    {
        TreeNode<TKey, TData>* parentNode;
        (parent == NULL) ? parentNode = m_root : parentNode = parent;
        TreeNode<TKey, TData>* node = parentNode->AddChild(key, data);
        m_nodes[key] = node;
        return node;
    }

    template <typename TKey, typename TData>
    TreeNode<TKey, TData>* Tree<TKey, TData>::Remove(const TKey& key)
    {
        typename std::map<TKey, TreeNode<TKey, TData>*>::const_iterator iter = m_nodes.find(key);
        if (iter == m_nodes.end())
            return NULL;

        // Remove from the map all sub-nodes of the tree
        TreeWalker<TKey, TData> walker(iter->second);

        for (; !walker.End(); walker++)
        {
            typename std::map<TKey, TreeNode<TKey, TData>*>::iterator it = m_nodes.find(walker.GetNode()->GetKey());
            if (it != m_nodes.end())
                m_nodes.erase(it);
        }
        return m_root->Remove(key);
    }
    template <typename TKey, typename TData>
    void Tree<TKey, TData>::Print(std::ostream& stream, int depth)
    {
        m_root->Print(stream, depth);
    }

    template <typename TKey, typename TData>
    void Tree<TKey, TData>::ToVector(std::vector<std::pair<TKey, TData> >& vec)
    {
        TreeWalker<TKey, TData> walker(GetRoot());
        for (; !walker.End(); walker++)
        {
            if (walker.GetNode()->IsRoot())
                continue;

            std::pair<TKey, TData> itemPair;
            itemPair.first = walker.GetNode()->GetKey();
            itemPair.second = walker.GetNode()->GetData();
            vec.push_back(itemPair);
        }
    }

    template <typename TKey, typename TData>
    void Tree<TKey, TData>::Compare(Tree* targetTree, std::vector<std::pair<TKey, TData> >& deletedItems, std::vector<std::pair<TKey, TData> >& modifiedItems, std::vector<std::pair<TKey, TData> >& newItems, TreeNode<TKey, TData>* fromNode)
    {
        // we break generic for the sake of thread safety:
        // we explicitly calling TKey.c_str(), which means that we assume that TKey has such member
        if (!targetTree) {
            return;
        }

        deletedItems.clear(); newItems.clear(); modifiedItems.clear();

        TreeNode<TKey, TData>* node;

        fromNode == NULL ? node = GetRoot() : node = fromNode;
        TreeWalker<TKey, TData> sourceTreeWalker(node);
        TreeWalker<TKey, TData> targetTreeWalker(targetTree->GetRoot());

        for (; !sourceTreeWalker.End(); sourceTreeWalker++)
        {
            if (sourceTreeWalker.GetNode()->IsRoot())
                continue;

            TreeNode<TKey, TData>* node = targetTree->Find(sourceTreeWalker.GetNode()->GetKey());
            if (node == NULL)
            {
                // Item does not exist in target tree which means it must been deleted
                std::pair<TKey, TData> itemPair;

                itemPair.first = sourceTreeWalker.GetNode()->GetKey().c_str();
                itemPair.second = sourceTreeWalker.GetNode()->GetData();
                deletedItems.push_back(itemPair);
            }
            else
            {
                // Compare data
                if (node->GetData() == sourceTreeWalker.GetNode()->GetData())
                    continue;

                // Data was modified
                std::pair<TKey, TData> itemPair;
                itemPair.first = sourceTreeWalker.GetNode()->GetKey().c_str();
                itemPair.second = node->GetData();
                modifiedItems.push_back(itemPair);
            }
        }

        for (; !targetTreeWalker.End(); targetTreeWalker++)
        {
            if (targetTreeWalker.GetNode()->IsRoot())
                continue;

            if (Find(targetTreeWalker.GetNode()->GetKey()) == NULL)
            {
                // Node from target tree does not exist in this tree
                // which means that this node is new
                // Data was modified
                std::pair<TKey, TData> itemPair;
                itemPair.first = targetTreeWalker.GetNode()->GetKey().c_str();
                itemPair.second = targetTreeWalker.GetNode()->GetData();
                newItems.push_back(itemPair);
            }
        }
    }
}
#endif
