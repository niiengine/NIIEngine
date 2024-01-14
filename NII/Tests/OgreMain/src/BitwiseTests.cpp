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
#include "BitwiseTests.h"
#include "OgreBitwise.h"
// Register the suite

using namespace Ogre;

CPPUNIT_TEST_SUITE_REGISTRATION( BitwiseTests );

void BitwiseTests::setUp()
{
}

void BitwiseTests::tearDown()
{
}


void BitwiseTests::testFixedPointConversion()
{
    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFixed(0x0,1,8), (unsigned int)0x00);
    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFixed(0x1,1,8), (unsigned int)0xFF);
    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFixed(0x2,2,8), (unsigned int)0xAA); // 10101010
    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFixed(0x1,2,8), (unsigned int)0x55); // 01010101
    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFixed(0x2,2,9), (unsigned int)0x155); // 1 01010101
    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFixed(0x1,2,9), (unsigned int)0x0AA); // 0 10101010
    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFixed(0xFE,8,3), (unsigned int)0x7); // 111
    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFixed(0xFE,8,9), (unsigned int)0x1FD); // 111111101

    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFloat(0xFF,8), 1.0f);
    CPPUNIT_ASSERT_EQUAL(Bitwise::fixedToFloat(0x00,8), 0.0f);

    CPPUNIT_ASSERT_EQUAL(Bitwise::floatToFixed(1.0f,8), (unsigned int)0xFF);
    CPPUNIT_ASSERT_EQUAL(Bitwise::floatToFixed(0.0f,8), (unsigned int)0x00);

    // Test clamping
    CPPUNIT_ASSERT_EQUAL(Bitwise::floatToFixed(-1.0f,8), (unsigned int)0x00);
    CPPUNIT_ASSERT_EQUAL(Bitwise::floatToFixed(2.0f,8), (unsigned int)0xFF);

    // Test circular conversion
    bool failed = false;
    for(unsigned int x=0; x<0x100; x++)
        if(Bitwise::floatToFixed(Bitwise::fixedToFloat(x,8),8) != x)
            failed = true;
    CPPUNIT_ASSERT_MESSAGE("circular floatToFixed/fixedToFloat for 8 bit failed",!failed);

    failed = false;
    for(unsigned int x=0; x<0x10; x++)
        if(Bitwise::floatToFixed(Bitwise::fixedToFloat(x,4),4) != x)
            failed = true;
    CPPUNIT_ASSERT_MESSAGE("circular floatToFixed/fixedToFloat for 4 bit failed",!failed);

    failed = false;
    for(unsigned int x=0; x<0x1000; x++)
        if(Bitwise::floatToFixed(Bitwise::fixedToFloat(x,12),12) != x)
            failed = true;
    CPPUNIT_ASSERT_MESSAGE("circular floatToFixed/fixedToFloat for 12 bit failed",!failed);
}

void BitwiseTests::testIntReadWrite()
{
    // Test reading and writing ints
    uint32 testje = 0x12345678;
    assert(Bitwise::intRead(&testje, 4) == 0x12345678);
    uint16 testje2 = 0x1234;
    assert(Bitwise::intRead(&testje2, 2) == 0x1234);
    uint8 testje3 = 0xD3;
    assert(Bitwise::intRead(&testje3, 1) == 0xD3);
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
    uint8 testje4[] = {0x12, 0x34, 0x56};
#else
    uint8 testje4[] = {0x56, 0x34, 0x12};
#endif
    assert(Bitwise::intRead(&testje4, 3) == 0x123456);

    Bitwise::intWrite(&testje, 4, 0x87654321);
    assert(testje == 0x87654321);

    Bitwise::intWrite(&testje2, 2, 0x4321);
    assert(testje2 == 0x4321);

    Bitwise::intWrite(&testje3, 1, 0x12);
    assert(testje3 == 0x12);
}

void BitwiseTests::testHalf()
{
    /*
    for(int x=0; x<0x100; x++)
    {
        float f = (float)x/255.0f;
        uint32 fi = *reinterpret_cast<uint32*>(&f);
        uint16 g = Bitwise::floatToHalf(f);
        float h = Bitwise::halfToFloat(g);
        uint32 hi = *reinterpret_cast<uint32*>(&h);
        int i = h*256.0f;
        std::cerr << x << " " << fi << " " << std::hex << std::setw(4) << g << " " << hi << " " << i << std::endl;
    }
    */
}
