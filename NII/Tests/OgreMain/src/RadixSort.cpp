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
#include "RadixSortTests.h"
#include "OgreRadixSort.h"
#include "OgreMath.h"

using namespace Ogre;

// Regsiter the suite
CPPUNIT_TEST_SUITE_REGISTRATION( RadixSortTests );

void RadixSortTests::setUp()
{
	srand(time(0));
}
void RadixSortTests::tearDown()
{
}

class FloatSortFunctor
{
public:
	float operator()(const float& p) const
	{
		return p;
	}

};
class IntSortFunctor
{
public:
	int operator()(const int& p) const
	{
		return p;
	}

};

class UnsignedIntSortFunctor
{
public:
	unsigned int operator()(const unsigned int& p) const
	{
		return p;
	}

};


void RadixSortTests::testFloatVector()
{
	std::vector<float> container;
	FloatSortFunctor func;
	RadixSort<std::vector<float>, float, float> sorter;

	for (int i = 0; i < 1000; ++i)
	{
		container.push_back((float)Math::random(-1e10, 1e10));
	}

	sorter.sort(container, func);

	std::vector<float>::iterator v = container.begin();
	float lastValue = *v++;
	for (;v != container.end(); ++v)
	{
		CPPUNIT_ASSERT(*v >= lastValue);
		lastValue = *v;
	}


}
void RadixSortTests::testFloatList()
{
	std::list<float> container;
	FloatSortFunctor func;
	RadixSort<std::list<float>, float, float> sorter;

	for (int i = 0; i < 1000; ++i)
	{
		container.push_back((float)Math::random(-1e10, 1e10));
	}

	sorter.sort(container, func);

	std::list<float>::iterator v = container.begin();
	float lastValue = *v++;
	for (;v != container.end(); ++v)
	{
		CPPUNIT_ASSERT(*v >= lastValue);
		lastValue = *v;
	}
}
void RadixSortTests::testUnsignedIntList()
{
	std::list<unsigned int> container;
	UnsignedIntSortFunctor func;
	RadixSort<std::list<unsigned int>, unsigned int, unsigned int> sorter;

	for (int i = 0; i < 1000; ++i)
	{
		container.push_back((unsigned int)Math::random(0, 1e10));
	}

	sorter.sort(container, func);

	std::list<unsigned int>::iterator v = container.begin();
	unsigned int lastValue = *v++;
	for (;v != container.end(); ++v)
	{
		CPPUNIT_ASSERT(*v >= lastValue);
		lastValue = *v;
	}
}
void RadixSortTests::testIntList()
{
	std::list<int> container;
	IntSortFunctor func;
	RadixSort<std::list<int>, int, int> sorter;

	for (int i = 0; i < 1000; ++i)
	{
		container.push_back((int)Math::random(-1e10, 1e10));
	}

	sorter.sort(container, func);

	std::list<int>::iterator v = container.begin();
	int lastValue = *v++;
	for (;v != container.end(); ++v)
	{
		CPPUNIT_ASSERT(*v >= lastValue);
		lastValue = *v;
	}
}
void RadixSortTests::testUnsignedIntVector()
{
	std::vector<unsigned int> container;
	UnsignedIntSortFunctor func;
	RadixSort<std::vector<unsigned int>, unsigned int, unsigned int> sorter;

	for (int i = 0; i < 1000; ++i)
	{
		container.push_back((unsigned int)Math::random(0, 1e10));
	}

	sorter.sort(container, func);

	std::vector<unsigned int>::iterator v = container.begin();
	unsigned int lastValue = *v++;
	for (;v != container.end(); ++v)
	{
		CPPUNIT_ASSERT(*v >= lastValue);
		lastValue = *v;
	}
}
void RadixSortTests::testIntVector()
{
	std::vector<int> container;
	IntSortFunctor func;
	RadixSort<std::vector<int>, int, int> sorter;

	for (int i = 0; i < 1000; ++i)
	{
		container.push_back((int)Math::random(-1e10, 1e10));
	}

	sorter.sort(container, func);

	std::vector<int>::iterator v = container.begin();
	int lastValue = *v++;
	for (;v != container.end(); ++v)
	{
		CPPUNIT_ASSERT(*v >= lastValue);
		lastValue = *v;
	}
}


