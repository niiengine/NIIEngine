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
#include "StreamSerialiserTests.h"
#include "OgreStreamSerialiser.h"
#include "OgreFileSystem.h"
#include "OgreException.h"
#include "OgreVector3.h"

using namespace Ogre;

// Regsiter the suite
CPPUNIT_TEST_SUITE_REGISTRATION( StreamSerialiserTests );

void StreamSerialiserTests::setUp()
{
}
void StreamSerialiserTests::tearDown()
{
}

void StreamSerialiserTests::testWriteBasic()
{
	FileSystemArchive arch("./", "FileSystem");
	arch.load();

	String fileName = "testSerialiser.dat";
	Vector3 aTestVector(0.3, 15.2, -12.0);
	String aTestString = "Some text here";
	int aTestValue = 99;
	uint32 chunkID = FileStreamSerializer::N_MAGIC4('T','E','S','T');
	// write the data
	{
		DataStream * stream = arch.create(fileName);

		FileStreamSerializer serialiser(stream);

		serialiser.writeChunkBegin(chunkID);
		serialiser.write(&aTestVector);
		serialiser.write(&aTestString);
		serialiser.write(&aTestValue);
		serialiser.writeChunkEnd(chunkID);
        N_delete stream;
	}

	// read it back
	{
		DataStream * stream = arch.open(fileName);

		FileStreamSerializer serialiser(stream);

		const FileStreamSerializer::Chunk* c = serialiser.readChunkBegin();

		CPPUNIT_ASSERT_EQUAL(chunkID, c->id);
		CPPUNIT_ASSERT_EQUAL(sizeof(Vector3) + sizeof(int) + aTestString.size() + 4, (size_t)c->length);

		Vector3 inVector;
		String inString;
		int inValue;

		serialiser.read(&inVector);
		serialiser.read(&inString);
		serialiser.read(&inValue);
		serialiser.readChunkEnd(chunkID);

		CPPUNIT_ASSERT_EQUAL(aTestVector, inVector);
		CPPUNIT_ASSERT_EQUAL(aTestString, inString);
		CPPUNIT_ASSERT_EQUAL(aTestValue, inValue);
        N_delete stream;
	}

	arch.remove(fileName);

	CPPUNIT_ASSERT(!arch.isExist(fileName));
}