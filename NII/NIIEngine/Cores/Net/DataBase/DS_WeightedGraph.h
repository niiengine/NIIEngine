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

#ifndef _WEIGHTED_GRAPH_H_
#define _WEIGHTED_GRAPH_H_

#include "NiiNetPreInclude.h"
#include "DS_OrderedList.h"
#include "DS_Map.h"
#include "DS_Heap.h"
#include "DS_Queue.h"
#include "DS_Tree.h"
#ifdef _DEBUG
#include <stdio.h>
#endif

#ifdef _MSC_VER
#pragma warning( push )
#endif

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
	template <class node_type, class weight_type, bool allow_unlinkedNodes>
	class _EngineAPI WeightedGraph
	{
	public:
		static void IMPLEMENT_DEFAULT_COMPARISON(void) {DataStructures::defaultMapKeyComparison<node_type>(node_type(),node_type());}

		WeightedGraph();
		~WeightedGraph();
		WeightedGraph( const WeightedGraph& original_copy );
		WeightedGraph& operator= ( const WeightedGraph& original_copy );
		void AddNode(const node_type &node);
		void RemoveNode(const node_type &node);
		void AddConnection(const node_type &node1, const node_type &node2, weight_type weight);
		void RemoveConnection(const node_type &node1, const node_type &node2);
		bool HasConnection(const node_type &node1, const node_type &node2);
		void Print(void);
		void Clear(void);
		bool GetShortestPath(DataStructures::List<node_type> &path, node_type startNode, node_type endNode, weight_type INFINITE_WEIGHT);
		bool GetSpanningTree(DataStructures::Tree<node_type> &outTree, DataStructures::List<node_type> *inputNodes, node_type startNode, weight_type INFINITE_WEIGHT );
		Nui32 GetNodeCount(void) const;
		Nui32 GetConnectionCount(Nui32 nodeIndex) const;
		void GetConnectionAtIndex(Nui32 nodeIndex, Nui32 connectionIndex, node_type &outNode, weight_type &outWeight) const;
		node_type GetNodeAtIndex(Nui32 nodeIndex) const;

	protected:
		void ClearDijkstra(void);
		void GenerateDisjktraMatrix(node_type startNode, weight_type INFINITE_WEIGHT);

		DataStructures::Map<node_type, DataStructures::Map<node_type, weight_type> *> adjacencyLists;

        bool isValidPath;
        node_type rootNode;
        DataStructures::OrderedList<node_type, node_type> costMatrixIndices;
        weight_type *costMatrix;
        node_type *leastNodeArray;

		struct NodeAndParent
		{
			DataStructures::Tree<node_type>*node;
			DataStructures::Tree<node_type>*parent;
		};
	};

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::WeightedGraph()
	{
		isValidPath=false;
		costMatrix=0;
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::~WeightedGraph()
	{
		Clear();
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
	WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::WeightedGraph( const WeightedGraph& original_copy )
	{
		adjacencyLists=original_copy.adjacencyLists;

		isValidPath=original_copy.isValidPath;
		if (isValidPath)
		{
			rootNode=original_copy.rootNode;
			costMatrixIndices=original_copy.costMatrixIndices;
			costMatrix = N_alloc_t(weight_type, costMatrixIndices.Size() * costMatrixIndices.Size());
			leastNodeArray = N_alloc_t(node_type, costMatrixIndices.Size());
			memcpy(costMatrix, original_copy.costMatrix, costMatrixIndices.Size() * costMatrixIndices.Size() * sizeof(weight_type));
			memcpy(leastNodeArray, original_copy.leastNodeArray, costMatrixIndices.Size() * sizeof(weight_type));
		}
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
	WeightedGraph<node_type, weight_type, allow_unlinkedNodes>& WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::operator=( const WeightedGraph& original_copy )
	{
		adjacencyLists=original_copy.adjacencyLists;

		isValidPath=original_copy.isValidPath;
		if (isValidPath)
		{
			rootNode=original_copy.rootNode;
			costMatrixIndices=original_copy.costMatrixIndices;
			costMatrix = N_alloc_t(weight_type, costMatrixIndices.Size() * costMatrixIndices.Size());
			leastNodeArray = N_alloc_t(node_type, costMatrixIndices.Size());
			memcpy(costMatrix, original_copy.costMatrix, costMatrixIndices.Size() * costMatrixIndices.Size() * sizeof(weight_type));
			memcpy(leastNodeArray, original_copy.leastNodeArray, costMatrixIndices.Size() * sizeof(weight_type));
		}

		return *this;
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		void WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::AddNode(const node_type &node)
	{
		adjacencyLists.SetNew(node, N_new DataStructures::Map<node_type, weight_type>());
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		void WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::RemoveNode(const node_type &node)
	{
		Nui32 i;
		DataStructures::Queue<node_type> removeNodeQueue;

		removeNodeQueue.Push(node);
		while (removeNodeQueue.Size())
		{
			N_delete adjacencyLists.Pop(removeNodeQueue.Pop());

			// Remove this node from all of the other lists as well
			for (i=0; i < adjacencyLists.Size(); i++)
			{
				adjacencyLists[i]->Delete(node);

#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
				if (allow_unlinkedNodes==false && adjacencyLists[i]->Size()==0)
					removeNodeQueue.Push(adjacencyLists.GetKeyAtIndex(i));
			}
		}

		ClearDijkstra();
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
	bool WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::HasConnection(const node_type &node1, const node_type &node2)
	{
		if (node1==node2)
			return false;
		if (adjacencyLists.Has(node1)==false)
			return false;
		return adjacencyLists.Get(node1)->Has(node2);
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		void WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::AddConnection(const node_type &node1, const node_type &node2, weight_type weight)
	{
		if (node1==node2)
			return;

		if (adjacencyLists.Has(node1)==false)
			AddNode(node1);
		adjacencyLists.Get(node1)->Set(node2, weight);
		if (adjacencyLists.Has(node2)==false)
			AddNode(node2);
		adjacencyLists.Get(node2)->Set(node1, weight);
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		void WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::RemoveConnection(const node_type &node1, const node_type &node2)
	{
		adjacencyLists.Get(node2)->Delete(node1);
		adjacencyLists.Get(node1)->Delete(node2);

#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
		if (allow_unlinkedNodes==false) // If we do not allow _unlinked nodes, then if there are no connections, remove the node
		{
			if (adjacencyLists.Get(node1)->Size()==0)
				RemoveNode(node1); // Will also remove node1 from the adjacency list of node2
			if (adjacencyLists.Has(node2) && adjacencyLists.Get(node2)->Size()==0)
				RemoveNode(node2);
		}

		ClearDijkstra();
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		void WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::Clear(void)
	{
		Nui32 i;
		for (i=0; i < adjacencyLists.Size(); i++)
			N_delete adjacencyLists[i];
		adjacencyLists.Clear();

		ClearDijkstra();
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		bool WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::GetShortestPath(DataStructures::List<node_type> &path, node_type startNode, node_type endNode, weight_type INFINITE_WEIGHT)
	{
		path.Clear(false);
		if (startNode==endNode)
		{
			path.Insert(startNode);
			path.Insert(endNode);
			return true;
		}

		if (isValidPath==false || rootNode!=startNode)
		{
			ClearDijkstra();
			GenerateDisjktraMatrix(startNode, INFINITE_WEIGHT);
		}

		// return the results
		bool objectExists;
		Nui32 col,row;
		weight_type currentWeight;
		DataStructures::Queue<node_type> outputQueue;
		col=costMatrixIndices.GetIndexFromKey(endNode, &objectExists);
		if (costMatrixIndices.Size()<2)
		{
			return false;
		}
		if (objectExists==false)
		{
			return false;
		}
		node_type vertex;
		row=costMatrixIndices.Size()-2;
		if (row==0)
		{
			path.Insert(startNode);
			path.Insert(endNode);
			return true;
		}
		currentWeight=costMatrix[row*adjacencyLists.Size() + col];
		if (currentWeight==INFINITE_WEIGHT)
		{
			// No path
			return true;
		}
		vertex=endNode;
		outputQueue.PushAtHead(vertex, 0);
		row--;
#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
		while (1)
		{
			while (costMatrix[row*adjacencyLists.Size() + col] == currentWeight)
			{
				if (row==0)
				{
					path.Insert(startNode);
					for (col=0; outputQueue.Size(); col++)
						path.Insert(outputQueue.Pop());
					return true;
				}
				--row;
			}

			vertex=leastNodeArray[row];
			outputQueue.PushAtHead(vertex, 0);
			if (row==0)
				break;
			col=costMatrixIndices.GetIndexFromKey(vertex, &objectExists);
			currentWeight=costMatrix[row*adjacencyLists.Size() + col];
		}

		path.Insert(startNode);
		for (col=0; outputQueue.Size(); col++)
			path.Insert(outputQueue.Pop());
		return true;
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		node_type WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::GetNodeAtIndex(Nui32 nodeIndex) const
	{
		return adjacencyLists.GetKeyAtIndex(nodeIndex);
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
	Nui32 WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::GetNodeCount(void) const
	{
		return adjacencyLists.Size();
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
	Nui32 WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::GetConnectionCount(Nui32 nodeIndex) const
	{
        return adjacencyLists[nodeIndex]->Size();
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
	void WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::GetConnectionAtIndex(Nui32 nodeIndex, Nui32 connectionIndex, node_type &outNode, weight_type &outWeight) const
	{
		outWeight=adjacencyLists[nodeIndex]->operator[](connectionIndex);
		outNode=adjacencyLists[nodeIndex]->GetKeyAtIndex(connectionIndex);
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
	bool WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::GetSpanningTree(DataStructures::Tree<node_type> &outTree, DataStructures::List<node_type> *inputNodes, node_type startNode, weight_type INFINITE_WEIGHT )
	{
		// Find the shortest path from the start node to each of the input nodes.  Add this path to a new WeightedGraph if the result is reachable
		DataStructures::List<node_type> path;
		DataStructures::WeightedGraph<node_type, weight_type, allow_unlinkedNodes> outGraph;
		bool res;
		Nui32 i,j;
		for (i=0; i < inputNodes->Size(); i++)
		{
			res=GetShortestPath(path, startNode, (*inputNodes)[i], INFINITE_WEIGHT);
			if (res && path.Size()>0)
			{
				for (j=0; j < path.Size()-1; j++)
				{
					// Don't bother looking up the weight
 					outGraph.AddConnection(path[j], path[j+1], INFINITE_WEIGHT);
				}
			}
		}

		// Copy the graph to a tree.
		DataStructures::Queue<NodeAndParent> nodesToProcess;
		DataStructures::Tree<node_type> *current;
		DataStructures::Map<node_type, weight_type> *adjacencyList;
		node_type key;
		NodeAndParent nap, nap2;
		outTree.DeleteDecendants();
		outTree.mData=startNode;
		current=&outTree;
		if (outGraph.adjacencyLists.Has(startNode)==false)
			return false;
		adjacencyList = outGraph.adjacencyLists.Get(startNode);

		for (i=0; i < adjacencyList->Size(); i++)
		{
			nap2.node = N_new DataStructures::Tree<node_type>();
			nap2.node->mData=adjacencyList->GetKeyAtIndex(i);
			nap2.parent=current;
			nodesToProcess.Push(nap2);
			current->children.Insert(nap2.node);
		}

		while (nodesToProcess.Size())
		{
			nap=nodesToProcess.Pop();
			current=nap.node;
			adjacencyList = outGraph.adjacencyLists.Get(nap.node->mData);

			for (i=0; i < adjacencyList->Size(); i++)
			{
				key=adjacencyList->GetKeyAtIndex(i);
				if (key!=nap.parent->mData)
				{
					nap2.node = N_new DataStructures::Tree<node_type>();
					nap2.node->mData=key;
					nap2.parent=current;
					nodesToProcess.Push(nap2);
					current->children.Insert(nap2.node);
				}
			}
		}

		return true;
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		void WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::GenerateDisjktraMatrix(node_type startNode, weight_type INFINITE_WEIGHT)
	{
		if (adjacencyLists.Size()==0)
			return;

		costMatrix = N_alloc_t(weight_type,adjacencyLists.Size() * adjacencyLists.Size());
		leastNodeArray = N_alloc_t(node_type,adjacencyLists.Size());

		node_type currentNode;
		Nui32 col, row, row2, openSetIndex;
		node_type adjacentKey;
		Nui32 adjacentIndex;
		weight_type edgeWeight, currentNodeWeight, adjacentNodeWeight;
		DataStructures::Map<node_type, weight_type> *adjacencyList;
		DataStructures::Heap<weight_type, node_type, false> minHeap;
		DataStructures::Map<node_type, weight_type> openSet;

		for (col=0; col < adjacencyLists.Size(); col++)
		{
			// This should be already sorted, so it's a bit inefficient to do an insertion sort, but what the heck
			costMatrixIndices.Insert(adjacencyLists.GetKeyAtIndex(col),adjacencyLists.GetKeyAtIndex(col), true);
		}
		for (col=0; col < adjacencyLists.Size() * adjacencyLists.Size(); col++)
			costMatrix[col]=INFINITE_WEIGHT;
		currentNode=startNode;
		row=0;
		currentNodeWeight=0;
		rootNode=startNode;

		// Clear the starting node column
		if (adjacencyLists.Size())
		{
			adjacentIndex=adjacencyLists.GetIndexAtKey(startNode);
			for (row2=0; row2 < adjacencyLists.Size(); row2++)
				costMatrix[row2*adjacencyLists.Size() + adjacentIndex]=0;
		}

		while (row < adjacencyLists.Size()-1)
		{
			adjacencyList = adjacencyLists.Get(currentNode);
			// Go through all connections from the current node.  If the new weight is less than the current weight, then update that weight.
			for (col=0; col < adjacencyList->Size(); col++)
			{
				edgeWeight=(*adjacencyList)[col];
				adjacentKey=adjacencyList->GetKeyAtIndex(col);
				adjacentIndex=adjacencyLists.GetIndexAtKey(adjacentKey);
				adjacentNodeWeight=costMatrix[row*adjacencyLists.Size() + adjacentIndex];

				if (currentNodeWeight + edgeWeight < adjacentNodeWeight)
				{
					// Update the weight for the adjacent node
					for (row2=row; row2 < adjacencyLists.Size(); row2++)
						costMatrix[row2*adjacencyLists.Size() + adjacentIndex]=currentNodeWeight + edgeWeight;
					openSet.Set(adjacentKey, currentNodeWeight + edgeWeight);
				}
			}

			// Find the lowest in the open set
			minHeap.Clear(true);
			for (openSetIndex=0; openSetIndex < openSet.Size(); openSetIndex++)
				minHeap.Push(openSet[openSetIndex], openSet.GetKeyAtIndex(openSetIndex));

			if (minHeap.Size()==0)
			{
				// Unreachable nodes
				isValidPath=true;
				return;
			}

			currentNodeWeight=minHeap.PeekWeight(0);
			leastNodeArray[row]=minHeap.Pop(0);
			currentNode=leastNodeArray[row];
			openSet.Delete(currentNode);
			row++;
		}

		isValidPath=true;
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		void WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::ClearDijkstra(void)
	{
		if (isValidPath)
		{
			isValidPath=false;
			N_free(costMatrix);
			N_free(leastNodeArray);
			costMatrixIndices.Clear(false);
		}
	}

	template <class node_type, class weight_type, bool allow_unlinkedNodes>
		void WeightedGraph<node_type, weight_type, allow_unlinkedNodes>::Print(void)
	{
#ifdef _DEBUG
		Nui32 i,j;
		for (i = 0; i < adjacencyLists.Size(); ++i)
		{
            String temp;
            adjacencyLists.GetKeyAtIndex(i).mAddress.read(temp);
			N_printf("%s connected to ", temp.c_str());

			if (adjacencyLists[i]->Size()==0)
				N_printf("<Empty>");
			else
			{
				for (j=0; j < adjacencyLists[i]->Size(); ++j)
                {
                    adjacencyLists[i]->GetKeyAtIndex(j).mAddress.read(temp);
					N_printf("%s (%.2f) ", temp.c_str(), (NIIf) adjacencyLists[i]->operator[](j) );
                }
			}

			N_printf("\n");
		}
#endif
	}
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
