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

#ifndef __RANGE_LIST_H_
#define __RANGE_LIST_H_

#include "NiiNetPreInclude.h"
#include "DS_OrderedList.h"
#include "NiiNetSerializer.h"

namespace DataStructures
{
	template <class range_type>
	struct RangeNode
    {
        RangeNode() {}
        ~RangeNode() {}
        RangeNode(range_type min, range_type max)
        {
            minIndex = min;
            maxIndex = max;
        }
        range_type minIndex;
        range_type maxIndex;
    };


    template <class range_type>
    int RangeNodeComp(const range_type &a, const RangeNode<range_type> &b)
    {
        if (a<b.minIndex)
            return -1;
        if (a==b.minIndex)
            return 0;
        return 1;
    }

	template <class range_type>
	class _EngineAPI RangeList
	{
	public:
		RangeList();
		~RangeList();
		void Insert(range_type index);
		void Clear(void);
		Nui32 Size(void) const;
		Nui32 RangeSum(void) const;
		NBitCount read(NetSerializer * out, NBitCount maxBits, bool clearSerialized);
		bool import(NetSerializer * in);

		DataStructures::OrderedList<range_type, RangeNode<range_type> , RangeNodeComp<range_type> > ranges;
	};

	template <class range_type> NBitCount RangeList<range_type>::read
    (NetSerializer * out, NBitCount maxBits, bool clearSerialized)
	{
		N_assert(ranges.Size() < (Nui16)-1);
		NetSerializer tempBS;
		NBitCount bitsWritten;
		Nui16 countWritten;
		Nui32 i;
		countWritten = 0;
		bitsWritten = 0;
		for(i = 0; i < ranges.Size(); ++i)
		{
			if(sizeof(Nui16) * 8 + bitsWritten + sizeof(range_type) * 8 * 2 + 1 > maxBits)
				break;
			Nui8 minEqualsMax;
			if (ranges[i].minIndex == ranges[i].maxIndex)
				minEqualsMax = 1;
			else
				minEqualsMax = 0;
			tempBS.write(minEqualsMax); // Use one byte, intead of one bit, for speed, as this is done a lot
			tempBS.write(ranges[i].minIndex);
			bitsWritten += sizeof(range_type) * 8 + 8;
			if(ranges[i].minIndex != ranges[i].maxIndex)
			{
				tempBS.write(ranges[i].maxIndex);
				bitsWritten += sizeof(range_type) * 8;
			}
			++countWritten;
		}

		out->writeAlign();
		NBitCount before = out->getWriteOffset();
		out->write(countWritten);
		bitsWritten += out->getWriteOffset() - before;
		out->write(&tempBS, tempBS.getWriteOffset());

		if (clearSerialized && countWritten)
		{
			Nui32 rangeSize = ranges.Size();
			for (i = 0; i < rangeSize - countWritten; ++i)
			{
				ranges[i] = ranges[i + countWritten];
			}
			ranges.RemoveFromEnd(countWritten);
		}

		return bitsWritten;
	}
	template <class range_type> bool RangeList<range_type>::import
    (NetSerializer *out)
	{
		ranges.Clear(true);
		Nui16 count;
		out->readAlign();
		out->read(count);
		Nui16 i;
		range_type min,max;
		Nui8 maxEqualToMin=0;

		for (i=0; i < count; ++i)
		{
			out->read(maxEqualToMin);
			if (out->read(min) == false)
				return false;
			if (maxEqualToMin == false)
			{
				if (out->read(max) == false)
					return false;
				if (max < min)
					return false;
			}
			else
				max=min;


			ranges.InsertAtEnd(RangeNode<range_type>(min,max));
		}
		return true;
	}

	template <class range_type>
	RangeList<range_type>::RangeList()
	{
		RangeNodeComp<range_type>(0, RangeNode<range_type>());
	}

	template <class range_type>
	RangeList<range_type>::~RangeList()
	{
		Clear();
	}

	template <class range_type>
	void RangeList<range_type>::Insert(range_type index)
	{
		if (ranges.Size() == 0)
		{
			ranges.Insert(index, RangeNode<range_type>(index, index), true);
			return;
		}

		bool objectExists;
		Nui32 insertionIndex = ranges.GetIndexFromKey(index, &objectExists);
		if (insertionIndex == ranges.Size())
		{
			if (index == ranges[insertionIndex - 1].maxIndex + (range_type)1)
				ranges[insertionIndex - 1].maxIndex++;
			else if (index > ranges[insertionIndex - 1].maxIndex + (range_type)1)
			{
				// Insert at end
				ranges.Insert(index, RangeNode<range_type>(index, index), true);
			}

			return;
		}

		if (index < ranges[insertionIndex].minIndex - (range_type)1)
		{
			// Insert here
			ranges.InsertAtIndex(RangeNode<range_type>(index, index), insertionIndex);

			return;
		}
		else if (index == ranges[insertionIndex].minIndex - (range_type)1)
		{
			// Decrease minIndex and join left
			ranges[insertionIndex].minIndex--;
			if (insertionIndex>0 && ranges[insertionIndex - 1].maxIndex + (range_type)1==ranges[insertionIndex].minIndex)
			{
				ranges[insertionIndex - 1].maxIndex = ranges[insertionIndex].maxIndex;
				ranges.RemoveAtIndex(insertionIndex);
			}

			return;
		}
		else if (index >= ranges[insertionIndex].minIndex && index <= ranges[insertionIndex].maxIndex)
		{
			// Already exists
			return;
		}
		else if (index == ranges[insertionIndex].maxIndex+(range_type)1)
		{
			// Increase maxIndex and join right
			ranges[insertionIndex].maxIndex++;
			if (insertionIndex<ranges.Size()-1 && ranges[insertionIndex+(range_type)1].minIndex==ranges[insertionIndex].maxIndex+(range_type)1)
			{
				ranges[insertionIndex+1].minIndex=ranges[insertionIndex].minIndex;
				ranges.RemoveAtIndex(insertionIndex);
			}

			return;
		}
	}

	template <class range_type>
	void RangeList<range_type>::Clear(void)
	{
		ranges.Clear(true);
	}

	template <class range_type>
	Nui32 RangeList<range_type>::Size(void) const
	{
		return ranges.Size();
	}

	template <class range_type>
	Nui32 RangeList<range_type>::RangeSum(void) const
	{
		Nui32 sum=0,i;
		for (i=0; i < ranges.Size(); i++)
			sum+=ranges[i].maxIndex-ranges[i].minIndex+1;
        return sum;
	}

}

#endif
