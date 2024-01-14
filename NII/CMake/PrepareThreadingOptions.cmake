

#######################################################################
# This file determines which threading options are available for the
# build, depending on the dependencies found.
#######################################################################

message(STATUS "����ִ��PrepareThreadingOptions.cmake�ű�")

set(NII_THREAD_SUPPORT_AVAILABLE FALSE)
set(NII_THREAD_DEFAULT_PROVIDER "none")

if (Boost_THREAD_FOUND AND Boost_DATE_TIME_FOUND)
	set(Boost_THREADING TRUE)
endif ()

if (Boost_THREADING AND NOT NII_THREAD_SUPPORT_AVAILABLE)
	set(NII_THREAD_SUPPORT_AVAILABLE TRUE)
	set(NII_THREAD_DEFAULT_PROVIDER "boost")
endif ()

if (POCO_FOUND AND NOT NII_THREAD_SUPPORT_AVAILABLE)
	set(NII_THREAD_SUPPORT_AVAILABLE TRUE)
	set(NII_THREAD_DEFAULT_PROVIDER "poco")
endif ()

if (TBB_FOUND AND NOT NII_THREAD_SUPPORT_AVAILABLE)
	set(NII_THREAD_SUPPORT_AVAILABLE TRUE)
	set(NII_THREAD_DEFAULT_PROVIDER "tbb")
endif ()

if (NII_THREAD_SUPPORT_AVAILABLE)
	set(NII_CONFIG_THREADS 2 CACHE STRING 
		"����NII��̨�����߳�֧��.����:
		0 - Threading off.
		1 - Full background loading.
		2 - Background resource preparation."
	)
	set(NII_CONFIG_THREAD_PROVIDER ${NII_THREAD_DEFAULT_PROVIDER} CACHE STRING
		"��ѡ��һ���߳�֧�ֿ�.����:
		boost - Boost thread library.
		poco  - Poco thread library.
		tbb   - ThreadingBuildingBlocks library."
	)
else ()
	set(NII_CONFIG_THREADS 0)
	set(NII_CONFIG_THREAD_PROVIDER "none")
endif ()


# sanitise threading choices
if (NOT NII_CONFIG_THREADS)
	set(NII_CONFIG_THREAD_PROVIDER "none")
	include_directories(${NII_BUILDALL_DIR}/NIIEngineSource/include)
else ()
	if (NII_CONFIG_THREAD_PROVIDER STREQUAL "boost")
		if (NOT Boost_THREADING)
			message(STATUS "����: boost�̲߳�����.ʹ��${NII_THREAD_DEFAULT_PROVIDER}��Ϊ�߳�.")
			set(NII_CONFIG_THREAD_PROVIDER ${NII_THREAD_DEFAULT_PROVIDER})
		endif ()
	elseif (NII_CONFIG_THREAD_PROVIDER STREQUAL "poco")
		if (NOT POCO_FOUND)
			message(STATUS "����: poco������.ʹ��${NII_THREAD_DEFAULT_PROVIDER}��Ϊ�߳�.")
			set(NII_CONFIG_THREAD_PROVIDER ${NII_THREAD_DEFAULT_PROVIDER})
		endif ()
	elseif (NII_CONFIG_THREAD_PROVIDER STREQUAL "tbb")
		if (NOT TBB_FOUND)
			message(STATUS "����: tbb������.ʹ��${NII_THREAD_DEFAULT_PROVIDER}��Ϊ�߳�.")
			set(NII_CONFIG_THREAD_PROVIDER ${NII_THREAD_DEFAULT_PROVIDER})
		endif ()
	else ()
		message(STATUS "����: ûѡ���߳�֧��.Ĭ��Ϊ ${NII_THREAD_DEFAULT_PROVIDER}.")
	endif ()	
endif ()
