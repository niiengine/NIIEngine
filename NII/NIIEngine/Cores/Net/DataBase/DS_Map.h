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

#ifndef __RAKNET_MAP_H_
#define __RAKNET_MAP_H_

#include "NiiNetPreInclude.h"
#include "DS_OrderedList.h"

// If I want to change this to a red-black tree, this is a good site: http://www.cs.auckland.ac.nz/software/AlgAnim/red_black.html
// This makes insertions and deletions faster.  But then traversals are slow, while they are currently fast.

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
    /// The default comparison has to be first so it can be called as a default parameter.
    /// It then is followed by MapNode, followed by NodeComparisonFunc
    template <class key_type>
        int defaultMapKeyComparison(const key_type &a, const key_type &b)
    {
        if (a<b) return -1; if (a==b) return 0; return 1;
    }

    /// \note IMPORTANT! If you use defaultMapKeyComparison then call IMPLEMENT_DEFAULT_COMPARISON or you will get an unresolved external linker error.
    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&, const key_type&)=defaultMapKeyComparison<key_type> >
    class _EngineAPI Map
    {
    public:
        static void IMPLEMENT_DEFAULT_COMPARISON(void) {DataStructures::defaultMapKeyComparison<key_type>(key_type(),key_type());}

        struct MapNode
        {
            MapNode() {}
            MapNode(key_type _key, data_type _data) : mapNodeKey(_key), mapNodeData(_data) {}
            MapNode& operator = ( const MapNode& input ) {mapNodeKey=input.mapNodeKey; mapNodeData=input.mapNodeData; return *this;}
            MapNode( const MapNode & input) {mapNodeKey=input.mapNodeKey; mapNodeData=input.mapNodeData;}
            key_type mapNodeKey;
            data_type mapNodeData;
        };

        // Has to be a static because the comparison callback for DataStructures::OrderedList is a C function
        static int NodeComparisonFunc(const key_type &a, const MapNode &b)
        {
#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
            return key_comparison_func(a, b.mapNodeKey);
        }

        Map();
        ~Map();
        Map( const Map& original_copy );
        Map& operator= ( const Map& original_copy );

        data_type& Get(const key_type &key) const; 
        data_type Pop(const key_type &key);
        // Add if needed
        void Set(const key_type &key, const data_type &data);
        // Must already exist
        void SetExisting(const key_type &key, const data_type &data);
        // Must add
        void SetNew(const key_type &key, const data_type &data);
        bool Has(const key_type &key) const;
        bool Delete(const key_type &key);
        data_type& operator[] ( const Nui32 position ) const;
        key_type GetKeyAtIndex( const Nui32 position ) const;
        Nui32 GetIndexAtKey( const key_type &key );
        void RemoveAtIndex(const Nui32 index);
        void Clear(void);
        Nui32 Size(void) const;

    protected:
        DataStructures::OrderedList< key_type,MapNode,&Map::NodeComparisonFunc > mapNodeList;

        void SaveLastSearch(const key_type &key, Nui32 index) const;
        bool HasSavedSearchResult(const key_type &key) const;

        Nui32 lastSearchIndex;
        key_type lastSearchKey;
        bool lastSearchIndexValid;
    };

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    Map<key_type, data_type, key_comparison_func>::Map()
    {
        lastSearchIndexValid=false;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    Map<key_type, data_type, key_comparison_func>::~Map()
    {
        Clear();
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    Map<key_type, data_type, key_comparison_func>::Map( const Map& original_copy )
    {
        mapNodeList=original_copy.mapNodeList;
        lastSearchIndex=original_copy.lastSearchIndex;
        lastSearchKey=original_copy.lastSearchKey;
        lastSearchIndexValid=original_copy.lastSearchIndexValid;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    Map<key_type, data_type, key_comparison_func>& Map<key_type, data_type, key_comparison_func>::operator= ( const Map& original_copy )
    {
        mapNodeList=original_copy.mapNodeList;
        lastSearchIndex=original_copy.lastSearchIndex;
        lastSearchKey=original_copy.lastSearchKey;
        lastSearchIndexValid=original_copy.lastSearchIndexValid;
        return *this;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    data_type& Map<key_type, data_type, key_comparison_func>::Get(const key_type &key) const
    {
        if (HasSavedSearchResult(key))
            return mapNodeList[lastSearchIndex].mapNodeData;

        bool objectExists;
        Nui32 index;
        index=mapNodeList.GetIndexFromKey(key, &objectExists);
        N_assert(objectExists);
        SaveLastSearch(key,index);
        return mapNodeList[index].mapNodeData;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    Nui32 Map<key_type, data_type, key_comparison_func>::GetIndexAtKey( const key_type &key )
    {
        if (HasSavedSearchResult(key))
            return lastSearchIndex;

        bool objectExists;
        Nui32 index;
        index=mapNodeList.GetIndexFromKey(key, &objectExists);
        if (objectExists==false)
        {
            N_assert(objectExists);
        }
        SaveLastSearch(key,index);
        return index;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    void Map<key_type, data_type, key_comparison_func>::RemoveAtIndex(const Nui32 index)
    {
        mapNodeList.RemoveAtIndex(index);
        lastSearchIndexValid=false;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
        data_type Map<key_type, data_type, key_comparison_func>::Pop(const key_type &key)
    {
        bool objectExists;
        Nui32 index;
        if (HasSavedSearchResult(key))
            index=lastSearchIndex;
        else
        {
            index=mapNodeList.GetIndexFromKey(key, &objectExists);
            N_assert(objectExists);
        }        
        data_type tmp = mapNodeList[index].mapNodeData;
        mapNodeList.RemoveAtIndex(index);
        lastSearchIndexValid=false;
        return tmp;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    void Map<key_type, data_type, key_comparison_func>::Set(const key_type &key, const data_type &data)
    {
        bool objectExists;
        Nui32 index;

        if (HasSavedSearchResult(key))
        {
            mapNodeList[lastSearchIndex].mapNodeData=data;
            return;
        }
        
        index=mapNodeList.GetIndexFromKey(key, &objectExists);

        if (objectExists)
        {
            SaveLastSearch(key,index);
            mapNodeList[index].mapNodeData=data;
        }
        else
        {
            SaveLastSearch(key,mapNodeList.Insert(key,MapNode(key,data), true));
        }
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    void Map<key_type, data_type, key_comparison_func>::SetExisting(const key_type &key, const data_type &data)
    {
        bool objectExists;
        Nui32 index;

        if (HasSavedSearchResult(key))
        {
            index=lastSearchIndex;
        }
        else
        {
            index=mapNodeList.GetIndexFromKey(key, &objectExists);
            N_assert(objectExists);
            SaveLastSearch(key,index);
        }        

        mapNodeList[index].mapNodeData=data;
    }    

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    void Map<key_type, data_type, key_comparison_func>::SetNew(const key_type &key, const data_type &data)
    {
#ifdef _DEBUG
        bool objectExists;
        mapNodeList.GetIndexFromKey(key, &objectExists);
        N_assert(objectExists==false);
#endif
        SaveLastSearch(key,mapNodeList.Insert(key,MapNode(key,data), true));
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    bool Map<key_type, data_type, key_comparison_func>::Has(const key_type &key) const
    {
        if (HasSavedSearchResult(key))
            return true;

        bool objectExists;
        Nui32 index;
        index=mapNodeList.GetIndexFromKey(key, &objectExists);
        if (objectExists)
            SaveLastSearch(key,index);
        return objectExists;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    bool Map<key_type, data_type, key_comparison_func>::Delete(const key_type &key)
    {
        if (HasSavedSearchResult(key))
        {
            lastSearchIndexValid=false;
            mapNodeList.RemoveAtIndex(lastSearchIndex);   
            return true;
        }

        bool objectExists;
        Nui32 index;
        index=mapNodeList.GetIndexFromKey(key, &objectExists);
        if (objectExists)
        {
            lastSearchIndexValid=false;
            mapNodeList.RemoveAtIndex(index);
            return true;
        }
        else
            return false;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    void Map<key_type, data_type, key_comparison_func>::Clear(void)
    {
        lastSearchIndexValid=false;
        mapNodeList.Clear(false);
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    data_type& Map<key_type, data_type, key_comparison_func>::operator[]( const Nui32 position ) const
    {
        return mapNodeList[position].mapNodeData;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
        key_type Map<key_type, data_type, key_comparison_func>::GetKeyAtIndex( const Nui32 position ) const
    {
        return mapNodeList[position].mapNodeKey;
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    Nui32 Map<key_type, data_type, key_comparison_func>::Size(void) const
    {
        return mapNodeList.Size();
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    void Map<key_type, data_type, key_comparison_func>::SaveLastSearch(const key_type &key, const Nui32 index) const
    {
        (void) key;
        (void) index;

        /*
        lastSearchIndex=index;
        lastSearchKey=key;
        lastSearchIndexValid=true;
        */
    }

    template <class key_type, class data_type, int (*key_comparison_func)(const key_type&,const key_type&)>
    bool Map<key_type, data_type, key_comparison_func>::HasSavedSearchResult(const key_type &key) const
    {
        (void) key;

        // Not threadsafe!
        return false;
        // return lastSearchIndexValid && key_comparison_func(key,lastSearchKey)==0;
    }
}

#endif