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
#include "VectorTests.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreVector4.h"

// Register the suite
CPPUNIT_TEST_SUITE_REGISTRATION( VectorTests );

using namespace Ogre;

void VectorTests::setUp()
{
}

void VectorTests::tearDown()
{
}


void VectorTests::testVector2Scaler()
{
    CPPUNIT_ASSERT_EQUAL(Vector2(1, 1) + Vector2(2, 2), Vector2(3, 3));
    CPPUNIT_ASSERT_EQUAL(1 + Vector2(2), Vector2(3, 3));
    Vector2 v1;
    v1 = 1;
    CPPUNIT_ASSERT_EQUAL(v1, Vector2(1, 1));
    v1 = 0.0;
    CPPUNIT_ASSERT_EQUAL(v1, Vector2::ZERO);
    v1 += 3;
    CPPUNIT_ASSERT_EQUAL(v1, Vector2(3, 3));

    v1 = 3 - Vector2(2);
    CPPUNIT_ASSERT_EQUAL(v1, Vector2(1));
    v1 = Vector2(5) - 7;
    CPPUNIT_ASSERT_EQUAL(v1, Vector2(-2));
    v1 -= 4;
    CPPUNIT_ASSERT_EQUAL(v1, Vector2(-6));
}

void VectorTests::testVector3Scaler()
{
    CPPUNIT_ASSERT_EQUAL(Vector3(1, 1, 1) + Vector3(2, 2, 2), Vector3(3, 3, 3));
    CPPUNIT_ASSERT_EQUAL(1 + Vector3(2), Vector3(3, 3, 3));
    Vector3 v1;
    v1 = 1;
    CPPUNIT_ASSERT_EQUAL(v1, Vector3(1));
    v1 = 0.0;
    CPPUNIT_ASSERT_EQUAL(v1, Vector3::ZERO);
    v1 += 3;
    CPPUNIT_ASSERT_EQUAL(v1, Vector3(3));

    v1 = 3 - Vector3(2);
    CPPUNIT_ASSERT_EQUAL(v1, Vector3(1));
    v1 = Vector3(5) - 7;
    CPPUNIT_ASSERT_EQUAL(v1, Vector3(-2));
    v1 -= 4;
    CPPUNIT_ASSERT_EQUAL(v1, Vector3(-6));
}

void VectorTests::testVector4Scaler()
{
    CPPUNIT_ASSERT_EQUAL(Vector4(1, 1, 1, 1) + Vector4(2, 2, 2, 2), Vector4(3, 3, 3, 3));
    CPPUNIT_ASSERT_EQUAL(1 + Vector4(2, 2, 2, 2), Vector4(3, 3, 3, 3));
    Vector4 v1;
    v1 = 1;
    CPPUNIT_ASSERT_EQUAL(v1, Vector4(1, 1, 1, 1));
    v1 = 0.0;
    CPPUNIT_ASSERT_EQUAL(v1, Vector4(0,0,0,0));
    v1 += 3;
    CPPUNIT_ASSERT_EQUAL(v1, Vector4(3,3,3,3));

    v1 = 3 - Vector4(2,2,2,2);
    CPPUNIT_ASSERT_EQUAL(v1, Vector4(1,1,1,1));
    v1 = Vector4(5,5,5,5) - 7;
    CPPUNIT_ASSERT_EQUAL(v1, Vector4(-2,-2,-2,-2));
    v1 -= 4;
    CPPUNIT_ASSERT_EQUAL(v1, Vector4(-6,-6,-6,-6));
}
