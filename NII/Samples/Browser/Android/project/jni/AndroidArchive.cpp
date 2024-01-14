#include "AndroidArchive.h"

#include <OgreStringConverter.h>
#include <OgreLogManager.h>

#include "acpwrapper.h"

#include <android/log.h>

#define  LOG_TAG    "AndroidArchive"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

namespace Ogre
{
	AndroidArchive::AndroidArchive(const String& name, const String& archType, JNIEnv *env)
		:Archive(name, archType), mBuffer(0), mEnv(env), mBufferSize(0)
	{
	}

	AndroidArchive::~AndroidArchive()
	{
		if(mBuffer)
		{
			OGRE_FREE(mBuffer, MEMCATEGORY_GENERAL);
			mBuffer = 0;
		}
	}

	bool AndroidArchive::isMatchCase() const
	{
		return false;
	}

	void AndroidArchive::peek()
	{
		if(acp_has_file(mEnv, mName.c_str()) != 0)
		{
			LogManager::getSingleton().logMessage(mName + " package found");

			int size = 0;
			if(acp_get_file(mEnv, mName.c_str(), &mBuffer, &size) == 0)
			{
				LogManager::getSingleton().logMessage(mName + " package loaded");
				mBufferSize = size;

				mFile.load(mBuffer, mBufferSize);
			}
		}
	}

	DataStream * AndroidArchive::open(const Ogre::String & filename, bool readOnly) const
	{
		DataStream * stream;

		size_t i = mFile.findChunk(filename);
		if(i != ACPFile::INVALID_INDEX)
		{
			ACPChunk *chunk = mFile.getChunk(i);
			if(chunk->getCompressed())
				chunk->uncompress();

			stream = N_new MemDataStream(chunk->getBuffer(), chunk->getSize(), false, readOnly);
		}
        else
            stream = 0;
		return stream;
	}

	DataStream * AndroidArchive::create(const Ogre::String &filename) const
	{
		return 0;
	}

	void AndroidArchive::remove(const String &filename) const
	{

	}

	void AndroidArchive::list(StringList & out, bool r, bool dirs)
	{
		for(NCount i = 0; i < mFile.getChunkCount(); ++i)
			out->push_back(mFile.getChunk(i)->getName());
	}

	void AndroidArchive::list(FileInfoList & out, bool r, bool dirs)
	{
		for(NCount i = 0; i < mFile.getChunkCount(); ++i)
		{
			ACPChunk * chunk = mFile.getChunk(i);

			FileInfo info;
			info.archive = this;
			info.mFullName = chunk->getName();
			info.mPath = chunk->getName();
			info.mFileName = chunk->getName();
			info.mCompressSize = chunk->getSize();
			info.mSize = chunk->getFullSize();
			out->push_back(info);
		}
	}

	void AndroidArchive::find(StringList & out, const String & pattern, bool r, bool dirs)
	{
		for(NCount i = 0; i < mFile.getChunkCount(); ++i)
		{
			ACPChunk *chunk = mFile.getChunk(i);

			if(StringUtil::match(chunk->getName(), pattern))
				out->push_back(chunk->getName());
		}
	}

	void AndroidArchive::find(FileInfoList & out, const String & pattern, bool r, bool dirs)
	{
		for(NCount i = 0; i < mFile.getChunkCount(); ++i)
		{
			ACPChunk *chunk = mFile.getChunk(i);
			if(StringUtil::match(chunk->getName(), pattern))
			{
				FileInfo info;
				info.archive = this;
				info.mFileName = info.mFullName = info.mPath = chunk->getName();
				info.mCompressSize = chunk->getSize();
				info.mSize = chunk->getFullSize();
				out->push_back(info);
			}
		}
	}

	bool AndroidArchive::isExist(const String& filename)
	{
		for(NCount i = 0; i < mFile.getChunkCount(); ++i)
		{
			ACPChunk *chunk = mFile.getChunk(i);
			if(chunk->getName() == filename)
				return true;
		}
		return false;
	}

	time_t AndroidArchive::getModifiedTime(const Ogre::String &filename)
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////

	const String &AndroidArchiveFactory::getType() const
	{
		static String type = "Android";
		return type;
	}

}