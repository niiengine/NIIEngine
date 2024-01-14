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
#include "FileSystemArchiveTests.h"
#include "OgreFileSystem.h"
#include "OgreException.h"

// Regsiter the suite
//CPPUNIT_TEST_SUITE_REGISTRATION( FileSystemArchiveTests );

void FileSystemArchiveTests::setUp()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    testPath = "../../../../Tests/OgreMain/misc/ArchiveTest/";
#else
    testPath = "../../Tests/OgreMain/misc/ArchiveTest/";
#endif
}

void FileSystemArchiveTests::tearDown()
{
}

void FileSystemArchiveTests::testListNonRecursive()
{
	try 
    {
		FileSystemArchive arch(testPath, "FileSystem");
		arch.load();
		StringList vec;
        arch.list(vec, false);

		CPPUNIT_ASSERT_EQUAL((unsigned int)2, (unsigned int)vec->size());
		CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), vec->at(0));
		CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), vec->at(1));
	}
	catch (Exception& e)
	{
		std::cout << e.getFullDescription();
	}

}
void FileSystemArchiveTests::testListRecursive()
{
    FileSystemArchive arch(testPath, "FileSystem");
    arch.load();
    StringList vec;
    arch.list(vec, true);

    CPPUNIT_ASSERT_EQUAL((size_t)48, vec->size()); // 48 including CVS folders!
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), vec->at(0));
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), vec->at(1));
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file.material"), vec->at(2));
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file2.material"), vec->at(3));
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file3.material"), vec->at(22));
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file4.material"), vec->at(23));
}
void FileSystemArchiveTests::testListFileInfoNonRecursive()
{
    FileSystemArchive arch(testPath, "FileSystem");
    arch.load();
    FileInfoList vec;
    arch.list(vec, false);

    //CPPUNIT_ASSERT_EQUAL((size_t)2, vec->size());
    //FileInfo& fi1 = vec->at(0);
    //CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFullName);
    //CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFileName);
    //CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi1.mPath);
    //CPPUNIT_ASSERT_EQUAL((size_t)130, fi1.mCompressSize);
    //CPPUNIT_ASSERT_EQUAL((size_t)130, fi1.mSize);

    //FileInfo& fi2 = vec->at(1);
    //CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFullName);
    //CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFileName);
    //CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi2.mPath);
    //CPPUNIT_ASSERT_EQUAL((size_t)156, fi2.mCompressSize);
    //CPPUNIT_ASSERT_EQUAL((size_t)156, fi2.mSize);
}
void FileSystemArchiveTests::testListFileInfoRecursive()
{
    FileSystemArchive arch(testPath, "FileSystem");
    arch.load();
    FileInfoList vec;
    arch.list(vec, true);

    CPPUNIT_ASSERT_EQUAL((size_t)48, vec->size()); // 48 including CVS folders!
    FileInfo& fi1 = vec->at(0);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi1.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)130, fi1.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)130, fi1.mSize);

    FileInfo& fi2 = vec->at(1);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi2.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)156, fi2.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)156, fi2.mSize);

    FileInfo& fi3 = vec->at(2);
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file.material"), fi3.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("file.material"), fi3.mFileName);
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/"), fi3.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi3.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi3.mSize);

    FileInfo& fi4 = vec->at(3);
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file2.material"), fi4.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("file2.material"), fi4.mFileName);
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/"), fi4.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi4.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi4.mSize);


    FileInfo& fi5 = vec->at(22);
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file3.material"), fi5.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("file3.material"), fi5.mFileName);
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/"), fi5.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi5.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi5.mSize);

    FileInfo& fi6 = vec->at(23);
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file4.material"), fi6.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("file4.material"), fi6.mFileName);
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/"), fi6.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi6.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi6.mSize);
}
void FileSystemArchiveTests::testFindNonRecursive()
{
    FileSystemArchive arch(testPath, "FileSystem");
    arch.load();
    StringList vec;
    arch.find(vec, "*.txt", false);

    CPPUNIT_ASSERT_EQUAL((size_t)2, vec->size());
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), vec->at(0));
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), vec->at(1));
}
void FileSystemArchiveTests::testFindRecursive()
{
    FileSystemArchive arch(testPath, "FileSystem");
    arch.load();
    StringList vec;
    arch.find(vec, "*.material", true);

    CPPUNIT_ASSERT_EQUAL((size_t)4, vec->size());
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file.material"), vec->at(0));
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file2.material"), vec->at(1));
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file3.material"), vec->at(2));
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file4.material"), vec->at(3));
}
void FileSystemArchiveTests::testFindFileInfoNonRecursive()
{
    FileSystemArchive arch(testPath, "FileSystem");
    arch.load();
    FileInfoList vec;
    arch.find(FileInfoList, "*.txt", false);

    CPPUNIT_ASSERT_EQUAL((size_t)2, vec->size());
    FileInfo& fi1 = vec->at(0);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi1.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)130, fi1.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)130, fi1.mSize);

    FileInfo& fi2 = vec->at(1);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi2.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)156, fi2.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)156, fi2.mSize);
}
void FileSystemArchiveTests::testFindFileInfoRecursive()
{
    FileSystemArchive arch(testPath, "FileSystem");
    arch.load();
    FileInfoList vec;
    arch.find(vec, "*.material", true);

    CPPUNIT_ASSERT_EQUAL((size_t)4, vec->size());

    FileInfo& fi3 = vec->at(0);
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file.material"), fi3.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("file.material"), fi3.mFileName);
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/"), fi3.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi3.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi3.mSize);

    FileInfo& fi4 = vec->at(1);
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file2.material"), fi4.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("file2.material"), fi4.mFileName);
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/"), fi4.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi4.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi4.mSize);


    FileInfo& fi5 = vec->at(2);
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file3.material"), fi5.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("file3.material"), fi5.mFileName);
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/"), fi5.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi5.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi5.mSize);

    FileInfo& fi6 = vec->at(3);
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file4.material"), fi6.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("file4.material"), fi6.mFileName);
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/"), fi6.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi6.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)0, fi6.mSize);
}
void FileSystemArchiveTests::testFileRead()
{
    FileSystemArchive arch(testPath, "FileSystem");
    arch.load();

    DataStream * stream = arch.open("rootfile.txt");
    CPPUNIT_ASSERT_EQUAL(String("this is line 1 in file 1"), stream->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 2 in file 1"), stream->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 3 in file 1"), stream->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 4 in file 1"), stream->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 5 in file 1"), stream->readLine());
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, stream->readLine()); // blank at end of file
    CPPUNIT_ASSERT(stream->end());
    N_delete stream;
}
void FileSystemArchiveTests::testReadInterleave()
{
    // Test overlapping reads from same archive

    FileSystemArchive arch(testPath, "FileSystem");
    arch.load();

    // File 1
    DataStream * stream1 = arch.open("rootfile.txt");
    CPPUNIT_ASSERT_EQUAL(String("this is line 1 in file 1"), stream1->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 2 in file 1"), stream1->readLine());
    N_delete stream1;
    // File 2
    DataStream * stream2 = arch.open("rootfile2.txt");
    CPPUNIT_ASSERT_EQUAL(String("this is line 1 in file 2"), stream2->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 2 in file 2"), stream2->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 3 in file 2"), stream2->readLine());
    N_delete stream2;

    // File 1
    CPPUNIT_ASSERT_EQUAL(String("this is line 3 in file 1"), stream1->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 4 in file 1"), stream1->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 5 in file 1"), stream1->readLine());
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, stream1->readLine()); // blank at end of file
    CPPUNIT_ASSERT(stream1->end());


    // File 2
    CPPUNIT_ASSERT_EQUAL(String("this is line 4 in file 2"), stream2->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 5 in file 2"), stream2->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 6 in file 2"), stream2->readLine());
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, stream2->readLine()); // blank at end of file
    CPPUNIT_ASSERT(stream2->end());

}

void FileSystemArchiveTests::testCreateAndRemoveFile()
{
	FileSystemArchive vfs("./", "FileSystem");
	vfs.load();

	CPPUNIT_ASSERT(!vfs.isReadOnly());

	String fileName = "a_test_file.txt";
	DataStream * stream = vfs.create(fileName);

	String testString = "Some text here";
	size_t written = stream->write((void*)testString.c_str(), testString.size());
	CPPUNIT_ASSERT_EQUAL(testString.size(), written);

	stream->close();
    N_delete stream;
    
	vfs.remove(fileName);

	CPPUNIT_ASSERT(!vfs.isExist(fileName));

}
