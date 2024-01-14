#ifndef __ADPArchive_H__
#define __ADPArchive_H__

#include <OgreArchive.h>
#include <OgreArchiveFactory.h>

#include <jni.h>
#include "ACPFile.h"

namespace NII
{
	class AndroidArchive : public VFS
	{
	public:
		AndroidArchive(const String & name, const String & archType, JNIEnv *env);
        ~AndroidArchive();

        /// @copydoc Archive::peek
        void peek();

        /// @copydoc Archive::isMatchCase
        bool isMatchCase() const;

        /// @copydoc Archive::open
        DataStream * open(const String & filename, bool readOnly = true) const;

		/// @copydoc Archive::create
		DataStream * create(const String & filename) const;

		/// @copydoc Archive::delete
		void remove(const String & filename) const;

		/// @copydoc Archive::list
        void list(StringList & out, bool r = true, bool dirs = false);

        /// @copydoc Archive::list
        void list(FileInfoList & out, bool r = true, bool dirs = false);

        /// @copydoc Archive::find
        void find(StringList & out, const String & pattern, bool r = true, bool dirs = false);

        /// @copydoc Archive::find
        void find(FileInfoList & out, const String& pattern, bool r = true, bool dirs = false);

        /// @copydoc Archive::exists
        bool isExist(const String& filename);

		/// @copydoc Archive::getModifiedTime
		time_t getModifiedTime(const String& filename);
	private:
		JNIEnv *mEnv;
		void *mBuffer;
		size_t mBufferSize;
		ACPFile mFile;
	};

	class AndroidArchiveFactory : public ArchiveFactory
    {
	private:
	JNIEnv *mEnv;
    public:
		AndroidArchiveFactory(JNIEnv *env):mEnv(env){}
        virtual ~AndroidArchiveFactory() {}
        /// @copydoc FactoryObj::getType
        const String& getType(void) const;
        /// @copydoc FactoryObj::createInstance
        Archive *createInstance( const String& name )
        {
            return N_new AndroidArchive(name, getType(), mEnv);
        }
        /// @copydoc FactoryObj::destroyInstance
        void destroyInstance( Archive* arch) { N_delete arch; }
    };
}

#endif