
# NII_DEPENDENCIES_DIR can be used to specify a single base
# folder where the required dependencies may be found.

message(STATUS "����ִ��Dependencies.cmake�ű�")

set(NII_DEPENDENCIES_DIR "" CACHE PATH "����NII�����ĵ�������·��")
include(FindPkgMacros)
# ��ȡ�ڲ�·��
getenv_path(NII_DEPENDENCIES_DIR)
# ��������·��
if(NII_BUILD_PLATFORM_IPHONE)
    set(NII_DEP_SEARCH_PATH 
        ${NII_DEPENDENCIES_DIR}
        ${ENV_NII_DEPENDENCIES_DIR}
        "${NII_BINARY_DIR}/iPhoneDependencies"
        "${NII_SOURCE_DIR}/iPhoneDependencies"
    )
else()
    set(NII_DEP_SEARCH_PATH ${NII_DEPENDENCIES_DIR} ${ENV_NII_DEPENDENCIES_DIR} "${NII_DEPEND_SEARCH_DIR}")
endif()

message(STATUS "����·��: ${NII_DEP_SEARCH_PATH}")

# Set hardcoded path guesses for various platforms
if (UNIX)
    set(NII_DEP_SEARCH_PATH ${NII_DEP_SEARCH_PATH} /usr/local)
    # Ubuntu 11.10 has an inconvenient path to OpenGL libraries
    set(NII_DEP_SEARCH_PATH ${NII_DEP_SEARCH_PATH} /usr/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu)
endif ()

# give guesses as hints to the find_package calls
set(CMAKE_PREFIX_PATH ${NII_DEP_SEARCH_PATH} ${CMAKE_PREFIX_PATH})
set(CMAKE_FRAMEWORK_PATH ${NII_DEP_SEARCH_PATH} ${CMAKE_FRAMEWORK_PATH})

#######################################################################
# ���ĵ�������
#######################################################################

# ��zlib
find_package(ZLIB)
macro_log_feature(ZLIB_FOUND "zlib" "�򵥵�����ѹ����" "http://www.zlib.net" FALSE "" "")
if (ZLIB_FOUND)
# ��zziplib
find_package(ZZip)
macro_log_feature(ZZip_FOUND "zziplib" "��zip�����н�ѹ����" "http://zziplib.sourceforge.net" FALSE "" "")
endif ()

# ��FreeImage
find_package(FreeImage)
macro_log_feature(FreeImage_FOUND "freeimage" "֧�ֳ�����ͼ��ͼƬ��ʽ" "http://freeimage.sourceforge.net" FALSE "" "")

# �ҵ� FreeType
find_package(Freetype)
macro_log_feature(FREETYPE_FOUND "freetype" "�����Ըߵ������" "http://www.freetype.org" TRUE "" "")

# �ҵ� X11
if (UNIX AND NOT NII_BUILD_PLATFORM_IPHONE)
    message(STATUS "����Ѱ��libx11")
    find_package(X11)
    macro_log_feature(X11_FOUND "X11" "X ����ϵͳ" "http://www.x.org" TRUE "" "")
    macro_log_feature(X11_Xt_FOUND "Xt" "X ���߼�" "http://www.x.org" TRUE "" "")
    find_library(XAW_LIBRARY NAMES Xaw Xaw7 PATHS ${NII_DEP_SEARCH_PATH} ${DEP_LIB_SEARCH_DIR} ${X11_LIB_SEARCH_PATH})
    macro_log_feature(XAW_LIBRARY "Xaw" "X11 Athena widget set" "http://www.x.org" TRUE "" "")
    mark_as_advanced(XAW_LIBRARY)
endif ()


#######################################################################
# RenderSystem dependencies
#######################################################################

# Ѱ��OpenGL
find_package(OpenGL)
macro_log_feature(OPENGL_FOUND "OpenGL" "֧��OpenGL��Ⱦϵͳ" "http://www.opengl.org/" FALSE "" "")

# Ѱ��OpenGL ES
find_package(OpenGLES)
macro_log_feature(OPENGLES_FOUND "OpenGL ES 1.x" "֧��OpenGL ES 1.x��Ⱦϵͳ" "http://www.khronos.org/opengles/" FALSE "" "")

# Ѱ��OpenGL ES 2.x
find_package(OpenGLES2)
macro_log_feature(OPENGLES2_FOUND "OpenGL ES 2.x" "֧��OpenGL ES 2.x ��Ⱦϵͳ" "http://www.khronos.org/opengles/" FALSE "" "")

# Ѱ��DirectX
if(WIN32)
	find_package(DirectX)
	macro_log_feature(DirectX_FOUND "DirectX" "֧��DirectX��Ⱦϵͳ" "http://msdn.microsoft.com/en-us/directx/" FALSE "" "")
endif()

#######################################################################
# Additional features
#######################################################################

# �ҵ�Cg
if (NOT NII_BUILD_PLATFORM_IPHONE)  
    find_package(Cg)
    macro_log_feature(Cg_FOUND "cg" "ͼ����ɫ����C��" "http://developer.nvidia.com/object/cg_toolkit.html" FALSE "" "")
endif (NOT NII_BUILD_PLATFORM_IPHONE)

# �ҵ�Boost
# Prefer static linking in all cases
if (WIN32 OR APPLE)
	set(Boost_USE_STATIC_LIBS TRUE)
else ()
	# Statically linking boost to a dynamic Nii build doesn't work on Linux 64bit
	set(Boost_USE_STATIC_LIBS ${NII_STATIC})
endif ()
if (APPLE AND NII_BUILD_PLATFORM_IPHONE)
    set(Boost_COMPILER "-xgcc42")
endif()
set(Boost_ADDITIONAL_VERSIONS "1.50" "1.50.0" "1.49" "1.49.0" "1.48" "1.48.0" "1.47" "1.47.0" "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" )
# Components that need linking (NB does not include header-only components like bind)
set(NII_BOOST_COMPONENTS thread date_time)
find_package(Boost COMPONENTS ${NII_BOOST_COMPONENTS} QUIET)
if (NOT Boost_FOUND)
	# Try again with the other type of libs
	if(Boost_USE_STATIC_LIBS)
		set(Boost_USE_STATIC_LIBS OFF)
	else()
		set(Boost_USE_STATIC_LIBS ON)
	endif()
	find_package(Boost COMPONENTS ${NII_BOOST_COMPONENTS} QUIET)
endif()
find_package(Boost QUIET)
macro_log_feature(Boost_FOUND "boost" "Boost (����)" "http://boost.org" FALSE "" "")
macro_log_feature(Boost_THREAD_FOUND "boost���߳�ģ��" "�����߳�֧��" "http://boost.org" FALSE "" "")
macro_log_feature(Boost_DATE_TIME_FOUND "boost��ʱ��ģ��" "�����߳�֧��" "http://boost.org" FALSE "" "")

# ��POCO
find_package(POCO)
macro_log_feature(POCO_FOUND "POCO" "POCO���" "http://pocoproject.org/" FALSE "" "")

# ��ThreadingBuildingBlocks
find_package(TBB)
macro_log_feature(TBB_FOUND "tbb" "Threading Building Blocks" "http://www.threadingbuildingblocks.org/" FALSE "" "")

# GLSL-Optimizer
find_package(GLSLOptimizer)
macro_log_feature(GLSL_Optimizer_FOUND "GLSL Optimizer" "GLSL Optimizer" "http://github.com/aras-p/glsl-optimizer/" FALSE "" "")

# HLSL2GLSL
find_package(HLSL2GLSL)
macro_log_feature(HLSL2GLSL_FOUND "HLSL2GLSL" "HLSL2GLSL" "http://hlsl2glslfork.googlecode.com/" FALSE "" "")

#**********************************************************************
# Samples dependencies
#**********************************************************************
#######################################################################
# Find OIS
#######################################################################
find_package(OIS)
macro_log_feature(OIS_FOUND "OIS" "�豸����⣬���������е�" "http://sourceforge.net/projects/wgois" FALSE "" "")

#######################################################################
# Tools
#######################################################################
find_package(Doxygen)
macro_log_feature(DOXYGEN_FOUND "Doxygen" "��������API�ĵ���" "http://doxygen.org" FALSE "" "")

# Ѱ��Softimage SDK
find_package(Softimage)
macro_log_feature(Softimage_FOUND "Softimage" "Softimage SDK needed for building XSIExporter" FALSE "6.0" "")
#######################################################################
# Tests
#######################################################################
find_package(CppUnit)
macro_log_feature(CppUnit_FOUND "CppUnit" "����ִ�е�Ԫ����" "http://cppunit.sourceforge.net" FALSE "" "")

#######################################################################
# Apple-specific
#######################################################################
if (APPLE)
    find_package(iOSSDK)
    macro_log_feature(iOSSDK_FOUND "iOS SDK" "iOS SDK" "http://developer.apple.com/ios" FALSE "" "")
    if (NOT NII_BUILD_PLATFORM_IPHONE)
        find_package(Carbon)
        macro_log_feature(Carbon_FOUND "Carbon" "Carbon" "http://developer.apple.com/mac" TRUE "" "")

        find_package(Cocoa)
        macro_log_feature(Cocoa_FOUND "Cocoa" "Cocoa" "http://developer.apple.com/mac" TRUE "" "")

        find_package(IOKit)
        macro_log_feature(IOKit_FOUND "IOKit" "IOKit HID framework needed by the samples" "http://developer.apple.com/mac" FALSE "" "")

        find_package(CoreVideo)
        macro_log_feature(CoreVideo_FOUND "CoreVideo" "CoreVideo" "http://developer.apple.com/mac" TRUE "" "")
    endif (NOT NII_BUILD_PLATFORM_IPHONE)
endif(APPLE)


# now see if we have a buildable Dependencies package in
# the source tree. If so, include that, and it will take care of
# setting everything up, including overriding any of the above
# findings.
set(NIIDEPS_RUNTIME_OUTPUT ${NII_RUNTIME_OUTPUT})
if (EXISTS "${NII_DEPEND_SEARCH_DIR}/CMakeLists.txt")
  add_subdirectory(depend)
endif ()

# ���ȱ�ٵ������⣬�ж�,��ʾ���
MACRO_DISPLAY_FEATURE_LOG()

# ����������ӿ�Ͱ���·��
include_directories(
  ${ZLIB_INCLUDE_DIRS}
  ${ZZip_INCLUDE_DIRS}
  ${FreeImage_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  ${OPENGLES_INCLUDE_DIRS}
  ${OPENGLES2_INCLUDE_DIRS}
  ${OIS_INCLUDE_DIRS}
  ${Cg_INCLUDE_DIRS}
  ${X11_INCLUDE_DIR}
  ${DirectX_INCLUDE_DIRS}
  ${CppUnit_INCLUDE_DIRS}
  ${Carbon_INCLUDE_DIRS}
  ${Cocoa_INCLUDE_DIRS}
  ${CoreVideo_INCLUDE_DIRS}
  ${GLSL_Optimizer_INCLUDE_DIRS}
  ${HLSL2GLSL_INCLUDE_DIRS}
)

link_directories(
  ${OPENGL_LIBRARY_DIRS}
  ${OPENGLES_LIBRARY_DIRS}
  ${OPENGLES2_LIBRARY_DIRS}
  ${Cg_LIBRARY_DIRS}
  ${X11_LIBRARY_DIRS}
  ${DirectX_LIBRARY_DIRS}
  ${CppUnit_LIBRARY_DIRS}
)

if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif ()

# ��װ��windows�еĵ�����������
include(InstallDependencies)
