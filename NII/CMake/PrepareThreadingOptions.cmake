

#######################################################################
# This file determines which threading options are available for the
# build, depending on the dependencies found.
#######################################################################

message(STATUS "正在执行PrepareThreadingOptions.cmake脚本")

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
		"启用NII后台加载线程支持.如下:
		0 - Threading off.
		1 - Full background loading.
		2 - Background resource preparation."
	)
	set(NII_CONFIG_THREAD_PROVIDER ${NII_THREAD_DEFAULT_PROVIDER} CACHE STRING
		"请选择一个线程支持库.如下:
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
			message(STATUS "警告: boost线程不可用.使用${NII_THREAD_DEFAULT_PROVIDER}作为线程.")
			set(NII_CONFIG_THREAD_PROVIDER ${NII_THREAD_DEFAULT_PROVIDER})
		endif ()
	elseif (NII_CONFIG_THREAD_PROVIDER STREQUAL "poco")
		if (NOT POCO_FOUND)
			message(STATUS "警告: poco不可用.使用${NII_THREAD_DEFAULT_PROVIDER}作为线程.")
			set(NII_CONFIG_THREAD_PROVIDER ${NII_THREAD_DEFAULT_PROVIDER})
		endif ()
	elseif (NII_CONFIG_THREAD_PROVIDER STREQUAL "tbb")
		if (NOT TBB_FOUND)
			message(STATUS "警告: tbb不可用.使用${NII_THREAD_DEFAULT_PROVIDER}作为线程.")
			set(NII_CONFIG_THREAD_PROVIDER ${NII_THREAD_DEFAULT_PROVIDER})
		endif ()
	else ()
		message(STATUS "警告: 没选择线程支持.默认为 ${NII_THREAD_DEFAULT_PROVIDER}.")
	endif ()	
endif ()
