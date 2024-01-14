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

#ifndef _DS_TREE_H_
#define _DS_TREE_H_

#include "NiiNetPreInclude.h"
#include "DS_List.h"
#include "DS_Queue.h"

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
    template <class TreeType>
    class _EngineAPI Tree
    {
    public:
        Tree();
        Tree(TreeType &inputData);
        ~Tree();
        void LevelOrderTraversal(DataStructures::List<Tree*> &output);
        void AddChild(TreeType &newData);
        void DeleteDecendants(void);

        TreeType mData;
        DataStructures::List<Tree *> children;
    };

    template <class TreeType>
    Tree<TreeType>::Tree()
    {

    }

    template <class TreeType>
    Tree<TreeType>::Tree(TreeType &inputData)
    {
        mData=inputData;
    }

    template <class TreeType>
    Tree<TreeType>::~Tree()
    {
        DeleteDecendants();
    }

    template <class TreeType>
    void Tree<TreeType>::LevelOrderTraversal(DataStructures::List<Tree*> &output)
    {
        Nui32 i;
        Tree<TreeType> *node;
        DataStructures::Queue<Tree<TreeType>*> queue;

        for (i=0; i < children.Size(); i++)
            queue.Push(children[i]);

        while (queue.Size())
        {
            node=queue.Pop();
            output.Insert(node);
            for (i=0; i < node->children.Size(); i++)
                queue.Push(node->children[i]);
        }
    }

    template <class TreeType>
    void Tree<TreeType>::AddChild(TreeType &newData)
    {
        children.Insert(N_new Tree());
    }

    template <class TreeType>
    void Tree<TreeType>::DeleteDecendants(void)
    {
        /*
        DataStructures::List<Tree*> output;
        LevelOrderTraversal(output);
        Nui32 i;
        for (i=0; i < output.Size(); i++)
            N_delete output[i];
*/

        // Already recursive to do this
        Nui32 i;
        for (i=0; i < children.Size(); i++)
            N_delete children[i];
    }
}

#endif
