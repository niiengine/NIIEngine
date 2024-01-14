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
#include "ZipArchiveTests.h"
#include "OgreZip.h"

using namespace Ogre;

// Regsiter the suite
CPPUNIT_TEST_SUITE_REGISTRATION( ZipArchiveTests );

void ZipArchiveTests::setUp()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    testPath = "../../../../Tests/OgreMain/misc/ArchiveTest.zip";
#else
    testPath = "../../../Tests/OgreMain/misc/ArchiveTest.zip";
#endif
}
void ZipArchiveTests::tearDown()
{
}

void ZipArchiveTests::testListNonRecursive()
{
    ZipArchive arch(testPath, "Zip");
    arch.load();
    StringList vec;
    arch.list(vec, false);

    CPPUNIT_ASSERT_EQUAL((size_t)2, vec->size());
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), vec->at(0));
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), vec->at(1));

}
void ZipArchiveTests::testListRecursive()
{
    ZipArchive arch(testPath, "Zip");
    arch.load();
    StringList vec;
    arch.list(vec, true);

    CPPUNIT_ASSERT_EQUAL((size_t)6, vec->size());
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file.material"), vec->at(0));
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file2.material"), vec->at(1));
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file3.material"), vec->at(2));
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file4.material"), vec->at(3));
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), vec->at(4));
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), vec->at(5));
}
void ZipArchiveTests::testListFileInfoNonRecursive()
{
    ZipArchive arch(testPath, "Zip");
    arch.load();
    FileInfoList vec;
    arch.list(vec, false);

    CPPUNIT_ASSERT_EQUAL((size_t)2, vec->size());
    FileInfo& fi1 = vec->at(0);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi1.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)40, fi1.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)130, fi1.mSize);

    FileInfo& fi2 = vec->at(1);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi2.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)45, fi2.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)156, fi2.mSize);
}
void ZipArchiveTests::testListFileInfoRecursive()
{
    ZipArchive arch(testPath, "Zip");
    arch.load();
    FileInfoList vec;
    arch.list(vec, true);

    CPPUNIT_ASSERT_EQUAL((size_t)6, vec->size());
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

    FileInfo& fi1 = vec->at(4);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi1.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)40, fi1.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)130, fi1.mSize);

    FileInfo& fi2 = vec->at(5);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi2.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)45, fi2.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)156, fi2.mSize);

}
void ZipArchiveTests::testFindNonRecursive()
{
    ZipArchive arch(testPath, "Zip");
    arch.load();
    StringList vec;
    arch.find(vec, "*.txt", false);

    CPPUNIT_ASSERT_EQUAL((size_t)2, vec->size());
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), vec->at(0));
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), vec->at(1));
}
void ZipArchiveTests::testFindRecursive()
{
    ZipArchive arch(testPath, "Zip");
    arch.load();
    StringList vec;
    arch.find(vec, "*.material", true);

    CPPUNIT_ASSERT_EQUAL((size_t)4, vec->size());
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file.material"), vec->at(0));
    CPPUNIT_ASSERT_EQUAL(String("level1/materials/scripts/file2.material"), vec->at(1));
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file3.material"), vec->at(2));
    CPPUNIT_ASSERT_EQUAL(String("level2/materials/scripts/file4.material"), vec->at(3));
}
void ZipArchiveTests::testFindFileInfoNonRecursive()
{
    ZipArchive arch(testPath, "Zip");
    arch.load();
    FileInfoList vec;
    arch.find(vec, "*.txt", false);

    CPPUNIT_ASSERT_EQUAL((size_t)2, vec->size());
    FileInfo& fi1 = vec->at(0);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile.txt"), fi1.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi1.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)40, fi1.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)130, fi1.mSize);

    FileInfo& fi2 = vec->at(1);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFullName);
    CPPUNIT_ASSERT_EQUAL(String("rootfile2.txt"), fi2.mFileName);
    CPPUNIT_ASSERT_EQUAL(StringUtil::BLANK, fi2.mPath);
    CPPUNIT_ASSERT_EQUAL((size_t)45, fi2.mCompressSize);
    CPPUNIT_ASSERT_EQUAL((size_t)156, fi2.mSize);
}
void ZipArchiveTests::testFindFileInfoRecursive()
{
    ZipArchive arch(testPath, "Zip");
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
void ZipArchiveTests::testFileRead()
{
    ZipArchive arch(testPath, "Zip");
    arch.load();

    DataStream * stream = arch.open("rootfile.txt");
    CPPUNIT_ASSERT_EQUAL(String("this is line 1 in file 1"), stream->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 2 in file 1"), stream->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 3 in file 1"), stream->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 4 in file 1"), stream->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 5 in file 1"), stream->readLine());
    CPPUNIT_ASSERT(stream->end());
    N_delete stream;

}
void ZipArchiveTests::testReadInterleave()
{
    // Test overlapping reads from same archive

    ZipArchive arch(testPath, "Zip");
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
    CPPUNIT_ASSERT(stream1->end());


    // File 2
    CPPUNIT_ASSERT_EQUAL(String("this is line 4 in file 2"), stream2->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 5 in file 2"), stream2->readLine());
    CPPUNIT_ASSERT_EQUAL(String("this is line 6 in file 2"), stream2->readLine());
    CPPUNIT_ASSERT(stream2->end());
}