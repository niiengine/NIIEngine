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
#include "StringTests.h"
#include "OgreStringConverter.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreMatrix4.h"
#include "OgreColourValue.h"

using namespace Ogre;

// Regsiter the suite
CPPUNIT_TEST_SUITE_REGISTRATION( StringTests );

void StringTests::setUp()
{
	testFileNoPath = "testfile.txt";
	testFileRelativePathUnix = "this/is/relative/testfile.txt";
	testFileRelativePathWindows = "this\\is\\relative\\testfile.txt";
	testFileAbsolutePathUnix = "/this/is/absolute/testfile.txt";
	testFileAbsolutePathWindows = "c:\\this\\is\\absolute\\testfile.txt";
}
void StringTests::tearDown()
{
}

void StringTests::testSplitFileNameNoPath()
{
	String basename, path;
	StringUtil::splitFilename(testFileNoPath, basename, path);

    CPPUNIT_ASSERT_EQUAL(testFileNoPath, basename);
    CPPUNIT_ASSERT(path.empty());
}
void StringTests::testSplitFileNameRelativePath()
{
	String basename, path;
	// Unix
	StringUtil::splitFilename(testFileRelativePathUnix, basename, path);
    CPPUNIT_ASSERT_EQUAL(String("testfile.txt"), basename);
    CPPUNIT_ASSERT_EQUAL(String("this/is/relative/"), path);
	// Windows
	StringUtil::splitFilename(testFileRelativePathWindows, basename, path);
    CPPUNIT_ASSERT_EQUAL(String("testfile.txt"), basename);
    CPPUNIT_ASSERT_EQUAL(String("this/is/relative/"), path);

}
void StringTests::testSplitFileNameAbsolutePath()
{
	String basename, path;
	// Unix
	StringUtil::splitFilename(testFileAbsolutePathUnix, basename, path);
    CPPUNIT_ASSERT_EQUAL(String("testfile.txt"), basename);
    CPPUNIT_ASSERT_EQUAL(String("/this/is/absolute/"), path);
	// Windows
	StringUtil::splitFilename(testFileAbsolutePathWindows, basename, path);
    CPPUNIT_ASSERT_EQUAL(String("testfile.txt"), basename);
	CPPUNIT_ASSERT_EQUAL(String("c:/this/is/absolute/"), path);
}

void StringTests::testMatchCaseSensitive()
{
	// Test positive
	CPPUNIT_ASSERT(StringUtil::match(testFileNoPath, testFileNoPath, true));
	// Test negative
	String upperCase = testFileNoPath;
    StringUtil::toUpperCase(upperCase);
	CPPUNIT_ASSERT(!StringUtil::match(testFileNoPath, upperCase, true));
}
void StringTests::testMatchCaseInSensitive()
{
	// Test positive
	CPPUNIT_ASSERT(StringUtil::match(testFileNoPath, testFileNoPath, false));
	// Test positive
	String upperCase = testFileNoPath;
	StringUtil::toUpperCase(upperCase);
	CPPUNIT_ASSERT(StringUtil::match(testFileNoPath, upperCase, false));
}
void StringTests::testMatchGlobAll()
{
	CPPUNIT_ASSERT(StringUtil::match(testFileNoPath, "*", true));
}
void StringTests::testMatchGlobStart()
{
	CPPUNIT_ASSERT(StringUtil::match(testFileNoPath, "*stfile.txt", true));
	CPPUNIT_ASSERT(!StringUtil::match(testFileNoPath, "*astfile.txt", true));
}
void StringTests::testMatchGlobEnd()
{
	CPPUNIT_ASSERT(StringUtil::match(testFileNoPath, "testfile.*", true));
	CPPUNIT_ASSERT(!StringUtil::match(testFileNoPath, "testfile.d*", true));
}
void StringTests::testMatchGlobStartAndEnd()
{
	CPPUNIT_ASSERT(StringUtil::match(testFileNoPath, "*stfile.*", true));
	CPPUNIT_ASSERT(!StringUtil::match(testFileNoPath, "*astfile.d*", true));
}
void StringTests::testMatchGlobMiddle()
{
	CPPUNIT_ASSERT(StringUtil::match(testFileNoPath, "test*.txt", true));
	CPPUNIT_ASSERT(!StringUtil::match(testFileNoPath, "last*.txt*", true));
}
void StringTests::testMatchSuperGlobtastic()
{
	CPPUNIT_ASSERT(StringUtil::match(testFileNoPath, "*e*tf*e.t*t", true));
}
void StringTests::testParseReal()
{
	NIIf r = 23.454;

	String s = StrConv::conv(r);
	NIIf t = StringConverter::parseReal(s);

	CPPUNIT_ASSERT_EQUAL(r, t);


}
void StringTests::testParseInt()
{

	int r = 223546;

	String s = StrConv::conv(r);
	int t = StringConverter::parseInt(s);

	CPPUNIT_ASSERT_EQUAL(r, t);
}
void StringTests::testParseLong()
{
	long r = -2147483647;

	String s = StrConv::conv(r);
	long t = StringConverter::parseLong(s);

	CPPUNIT_ASSERT_EQUAL(r, t);

}
void StringTests::testParseUnsignedLong()
{
	unsigned long r = 4294967295UL;

	String s = StrConv::conv(r);
	unsigned long t = StringConverter::parseUnsignedLong(s);

	CPPUNIT_ASSERT_EQUAL(r, t);

}
void StringTests::testParseVector3()
{
	Vector3 r(0.12, 3.22, -4.04);

	String s = StrConv::conv(r);
	Vector3 t = StringConverter::parseVector3(s);

	CPPUNIT_ASSERT_EQUAL(r, t);

}
void StringTests::testParseMatrix4()
{
	Matrix4 r(1.12, 0, 0, 34, 0, 0.87, 0, 20, 0, 0, 0.56, 10, 0, 0, 0, 1);

	String s = StrConv::conv(r);
	Matrix4 t = StringConverter::parseMatrix4(s);

	CPPUNIT_ASSERT_EQUAL(r, t);

}

void StringTests::testParseQuaternion()
{
	Quaternion r(1.12, 0.87, 0.67, 1);

	String s = StrConv::conv(r);
	Quaternion t = StringConverter::parseQuaternion(s);

	CPPUNIT_ASSERT_EQUAL(r, t);
}

void StringTests::testParseBool()
{
	bool r = true;
	String s = StrConv::conv(r);
	bool t = StringConverter::parseBool(s);
	CPPUNIT_ASSERT_EQUAL(r, t);

	r = false;
	s = StrConv::conv(r);
	t = StringConverter::parseBool(s);
	CPPUNIT_ASSERT_EQUAL(r, t);

}
void StringTests::testParseColourValue()
{
	Colour r(0.34, 0.44, 0.77, 1.0);

	String s = StrConv::conv(r);
	Colour t = StringConverter::parseColourValue(s);
	CPPUNIT_ASSERT_EQUAL(r, t);

}
