# 需要的cmake的最小版本
cmake_minimum_required(VERSION 2.8.8)
# 设置不包含之前库的路径
cmake_policy(SET CMP0003 NEW)

# 使用相对路径
# 这主要是在Windows命令行的限制，以减少路径大小
if(WIN32)
    # This seems to break Xcode projects so definitely don't enable on Apple builds
    set(CMAKE_USE_RELATIVE_PATHS true)
    set(CMAKE_SUPPRESS_REGENERATION true)
endif()

# Assign compiler for Mac OS X-based systems
# NOTE: This logic is order-dependent and must occur here because the
# configuration phase of CMake projects will fall into an infinite loop
# if compiler assignments are modified after issuing a project(...) command.
#
if (NII_BUILD_PLATFORM_IPHONE)
    # Force gcc <= 4.2 on iPhone
    include(CMakeForceCompiler)
    CMAKE_FORCE_C_COMPILER(clang GNU)
    CMAKE_FORCE_CXX_COMPILER(clang++ GNU)
    SET(CMAKE_SIZEOF_VOID_P 4)
    set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
endif ()
#######################################################################
project(NII)
#######################################################################
# 设置中间环节
set(NII_INTERMEDIATE_DIR "${NII_SOURCE_DIR}/INTERMEDIATEDIR")
# 设置目标安装环节
set(NII_BUILDALL_DIR "${NII_SOURCE_DIR}/NIIBuild")
# 设置第三方库搜索路径
set(NII_DEPEND_SEARCH_DIR "${NII_SOURCE_DIR}/Depend")
#######################################################################
# Include necessary submodules
set(CMAKE_MODULE_PATH 
    "${NII_SOURCE_DIR}/CMake"
    "${NII_SOURCE_DIR}/CMake/Utils" 
    "${NII_SOURCE_DIR}/CMake/Packages"
)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
# 记录配置的临时文件工具
include(MacroLogFeature)
include(NiiAddTargets)
# 配置项目生成库路径
include(NiiConfigTargets)
# 获取版本号工具
include(PreprocessorUtils)
#包含预编译头函数包
include(PrecompiledHeader)
#复制源代码工具
include(InstallSource)
# 设置模版路径
set(NII_TEMPLATES_DIR "${NII_SOURCE_DIR}/CMake/Templates")
set(NII_WORK_DIR ${NII_BINARY_DIR})
#####################################################################
# Set up the basic build environment
#####################################################################

if (CMAKE_BUILD_TYPE STREQUAL "")
    # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
    # differentiation between debug and release builds.
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "选择构建类型, 选项有: None (CMAKE_CXX_FLAGS 或 CMAKE_C_FLAGS 使用) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# 输出nii 版本号
include(NiiGetVersion)
nii_get_version(${NII_SOURCE_DIR}/NIIEngine/Prerequisites/NiiPrerequisites.h)
message(STATUS "@@@@@@@@@@@@@@@@@@@@@@@@@@")
message(STATUS "配置 ${NII_VERSION} ...")
message(STATUS "@@@@@@@@@@@@@@@@@@@@@@@@@@")
# 使用脚本配置版号文件
configure_file("${NII_TEMPLATES_DIR}/version.txt.in" "${NII_BUILDALL_DIR}/NIIEngine/version.txt" @ONLY)

# 确定我们编译的是32位或64位系统
include(CheckTypeSize)
CHECK_TYPE_SIZE("void*" NII_PTR_SIZE BUILTIN_TYPES_ONLY)
if (NII_PTR_SIZE EQUAL 8)
    set(NII_PLATFORM_X64 TRUE)
else ()
    set(NII_PLATFORM_X64 FALSE)
endif ()

if (NOT APPLE)
    # 使用_d后缀创建调试库
    set(CMAKE_DEBUG_POSTFIX "_debug")
endif ()

# 设置编译器指定编译标志
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    check_cxx_compiler_flag(-msse NII_GCC_HAS_SSE)
    if (NII_GCC_HAS_SSE)
        add_definitions(-msse)
    endif ()
    # This is a set of sensible warnings that provide meaningful output
    #set(NII_WARNING_FLAGS "-Wno-deprecated -Wall -Wctor-dtor-privacy -Winit-self -Wno-overloaded-virtual -Wcast-qual -Wwrite-strings -Wextra -Wno-unused-parameter -pedantic -Wshadow -Wno-missing-field-initializers -Wno-long-long")
    if (NOT APPLE)
        set(NII_WARNING_FLAGS "${NII_WARNING_FLAGS} -Wno-unused-but-set-parameter")
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NII_WARNING_FLAGS}")    
endif ()
if (MSVC)
    if (CMAKE_BUILD_TOOL STREQUAL "nmake")
        # set variable to state that we are using nmake makefiles
        set(NMAKE TRUE)
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")
    # Enable intrinsics on MSVC in debug mode
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Oi")
    if (CMAKE_CL_64)
        # Visual Studio bails out on debug builds in 64bit mode unless
        # this flag is set...
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
    endif ()
    if (NII_UNITY_BUILD)
        # object files can get large with Unity builds
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    endif ()  
endif ()
if (MINGW)
    add_definitions(-D_WIN32_WINNT=0x0500)
    # set architecture to i686, since otherwise some versions of MinGW can't link 
    # the atomic primitives
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686")
    # disable this optimisation because it breaks release builds (reason unknown)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-tree-slp-vectorize")    
endif ()

if ((CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX) AND NOT MINGW)
    # 测试GCC的visibility编译选项
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-fvisibility=hidden NII_GCC_VISIBILITY)
    if (NII_GCC_VISIBILITY)
        # 检测gcc版本
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE NII_GCC_VERSION)
        message(STATUS "检测 GNU g++ ${NII_GCC_VERSION}")
        message(STATUS "启用 GCC api接口可见标志")
        set(NII_GCC_VISIBILITY_FLAGS "-DNII_GCC_VISIBILITY -fvisibility=hidden")
        set(XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
  
        # 测试是否支持 -fvisibility-inlines-hidden
        string(TOLOWER "${CMAKE_BUILD_TYPE}" NII_BUILD_TYPE)
        if (NII_BUILD_TYPE STREQUAL "debug" AND NII_GCC_VERSION VERSION_LESS "4.2")
            message(STATUS "跳过 -fvisibility-inlines-hidden 因为在g++ < 4.2中可能有问题")
        else ()
            set(NII_GCC_VISIBILITY_FLAGS "${NII_GCC_VISIBILITY_FLAGS} -fvisibility-inlines-hidden")
            set(XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "YES")
        endif ()
    endif (NII_GCC_VISIBILITY)
  
    # 修复Linux上的x64问题
    if(NII_PLATFORM_X64 AND NOT APPLE)
        add_definitions(-fPIC)
    endif()  
endif ((CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX) AND NOT MINGW)

# 确定系统字节序
if (MSVC)
    # 这个在VS2010中不能工作，MSVC只编译成intel处理器类型的
    set(NII_TEST_BIG_ENDIAN FALSE)
else()
    include(TestBigEndian)
    test_big_endian(NII_TEST_BIG_ENDIAN)
endif()

# 添加 NIIEngine 到路径中
include_directories("${NII_SOURCE_DIR}/NIIEngine")
include_directories("${NII_BINARY_DIR}")
include_directories("${NII_INTERMEDIATE_DIR}")

if (APPLE)
    if (NII_BUILD_PLATFORM_IPHONE)
        # Set static early for proper dependency detection
        set(NII_STATIC TRUE)
    endif ()
endif (APPLE)

set(NII_DEPENDENCE_PREFIX_COMMAND_OPTION " --prefix=${NII_DEPEND_SEARCH_DIR} ")

#生成图形用户界面第三方库
add_subdirectory(GUICore)

#生成图象解码第三方库
add_subdirectory(ImageCore)

#生成音频/视频解码第三方库
add_subdirectory(MediaCore)

#生成网络第三方库
add_subdirectory(NetCore)

#生成数据库第三方库
add_subdirectory(SqlCore)

#生成几何模型库第三方库
add_subdirectory(ModelCore)

# 找到第三方库
include(Dependencies)
# 用于sample事例
set(NII_LIBRARIES NIIEngine) 
set(NII_Paging_LIBRARIES NIIPaging)
set(NII_Terrain_LIBRARIES NIITerrain)
set(NII_Plugin_PCZSceneManager_LIBRARIES Plugin_PCZSceneManager)
set(NII_Plugin_OctreeZone_LIBRARIES Plugin_OctreeZone)

# 指定编译路径(临时)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${NII_INTERMEDIATE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${NII_INTERMEDIATE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${NII_INTERMEDIATE_DIR}/bin")

if (WIN32 OR APPLE)
    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        # 不安装到默认的系统路径,用于sdk
        set(CMAKE_INSTALL_PREFIX "${NII_BUILDALL_DIR}" CACHE PATH "NII引擎安装路径" FORCE)
    endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif(WIN32 OR APPLE)

# 设立iPhone.
if (NII_BUILD_PLATFORM_IPHONE)
    # Set build variables
    set(CMAKE_OSX_SYSROOT iphoneos)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "")
    set(CMAKE_EXE_LINKER_FLAGS "-framework Foundation -framework CoreGraphics -framework QuartzCore -framework UIKit")
    set(XCODE_ATTRIBUTE_SDKROOT iphoneos)
    set(XCODE_ATTRIBUTE_GCC_UNROLL_LOOPS "YES")
    set(XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
    set(XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
    set(NII_BUILD_RENDERSYSTEM_GLES TRUE CACHE BOOL "Forcing OpenGL ES RenderSystem for iOS" FORCE)
    set(NII_BUILD_RENDERSYSTEM_GLES2 TRUE CACHE BOOL "Forcing OpenGL ES 2 RenderSystem for iOS" FORCE)
    set(NII_STATIC TRUE CACHE BOOL "Forcing static build for iOS" FORCE)    
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.\${PRODUCT_NAME:rfc1034identifier}")
    set(NII_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE TRUE CACHE BOOL "在iPhone中强制启用视口方向模式" FORCE)

    set(CMAKE_OSX_ARCHITECTURES ${ARCHS_STANDARD_32_BIT})
    # armv7 only
    #set(CMAKE_OSX_ARCHITECTURES ${ARCHS_UNIVERSAL_IPHONE_OS})

    remove_definitions(-msse)
elseif (APPLE AND NOT NII_BUILD_PLATFORM_IPHONE)
    # Set 10.5 as the base SDK by default
    set(XCODE_ATTRIBUTE_SDKROOT macosx)
    set(CMAKE_OSX_SYSROOT macosx)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.5)

    if (NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES ${ARCHS_STANDARD_32_64_BIT})
    endif()

    # Make sure that the OpenGL render system is selected for non-iPhone Apple builds
    set(NII_BUILD_RENDERSYSTEM_GL TRUE)
    set(NII_BUILD_RENDERSYSTEM_GLES FALSE)
    set(NII_BUILD_RENDERSYSTEM_GLES2 FALSE)
elseif (SYMBIAN)  
    set(NII_BUILD_RENDERSYSTEM_GLES TRUE CACHE BOOL "Forcing OpenGL ES 1.x RenderSystem for Symbian" FORCE)      
endif ()

# Enable the PVRTC codec if OpenGL ES is being built
if(NII_BUILD_RENDERSYSTEM_GLES OR NII_BUILD_RENDERSYSTEM_GLES2)
    set(NII_CONFIG_ENABLE_PVRTC TRUE CACHE BOOL "在OpenGL ES强制使用PVRTC编码" FORCE)
endif()

######################################################################
# Provide user options to customise the build process
######################################################################

# Customise what to build
cmake_dependent_option(NII_BUILD_RENDERSYSTEM_D3D9 "构建Direct3D9渲染系统" TRUE "WIN32;DirectX_FOUND" FALSE)
cmake_dependent_option(NII_BUILD_RENDERSYSTEM_D3D11 "构建Direct3D11渲染系统[实验中]" FALSE "WIN32;DirectX_D3D11_FOUND" FALSE)
cmake_dependent_option(NII_BUILD_RENDERSYSTEM_GL "构建OpenGL渲染系统" TRUE "OPENGL_FOUND;NOT NII_BUILD_PLATFORM_IPHONE;NOT SYMBIAN" FALSE)
cmake_dependent_option(NII_BUILD_RENDERSYSTEM_GLES "构建OpenGL ES 1.x渲染系统" FALSE "OPENGLES_FOUND" FALSE)
cmake_dependent_option(NII_BUILD_RENDERSYSTEM_GLES2 "构建OpenGL ES 2.x渲染系统" FALSE "OPENGLES2_FOUND" FALSE)
cmake_dependent_option(NII_BUILD_PLATFORM_IPHONE "构建NII的IOS" FALSE "iOSSDK_FOUND;OPENGLES_FOUND;OPENGLES2_FOUND" FALSE)
cmake_dependent_option(NII_BUILD_PLATFORM_NACL "构建NII使用的Google's 原生客户端(NaCl)" FALSE "OPENGLES2_FOUND" FALSE)

option(NII_BUILD_PLUGIN_BSP "构建BSP场景管理器插件" FALSE)
option(NII_BUILD_PLUGIN_OCTREE "构建Octree场景管理器插件" TRUE)
option(NII_BUILD_PLUGIN_PFX "构建ParticleFX插件" TRUE)
cmake_dependent_option(NII_BUILD_PLUGIN_PCZ "构建PCZ SceneManager插件" TRUE "NOT SYMBIAN" FALSE)
cmake_dependent_option(NII_BUILD_COMPONENT_PAGING "构建Paging成分" TRUE "NOT SYMBIAN" FALSE)
cmake_dependent_option(NII_BUILD_COMPONENT_TERRAIN "构建Terrain成分" TRUE "NOT SYMBIAN" FALSE)

cmake_dependent_option(NII_BUILD_COMPONENT_PROPERTY "构建Property成分" TRUE "Boost_FOUND" FALSE)
cmake_dependent_option(NII_BUILD_PLUGIN_CG "构建Cg插件" TRUE "Cg_FOUND;NOT NII_BUILD_PLATFORM_IPHONE" FALSE)
option(NII_BUILD_COMPONENT_RTSHADERSYSTEM "构建RTShader渲染系统成分" TRUE)
cmake_dependent_option(NII_BUILD_RTSHADERSYSTEM_CORE_SHADERS "构建RTShader渲染系统的FFP核心着色器" TRUE "NII_BUILD_COMPONENT_RTSHADERSYSTEM" FALSE)
cmake_dependent_option(NII_BUILD_RTSHADERSYSTEM_EXT_SHADERS "构建RTShader渲染系统的扩展着色器" TRUE "NII_BUILD_COMPONENT_RTSHADERSYSTEM" FALSE)

cmake_dependent_option(NII_BUILD_TOOLS "构建命令行工具" TRUE "NOT NII_BUILD_PLATFORM_IPHONE;NOT SYMBIAN" FALSE)
cmake_dependent_option(NII_BUILD_XSIEXPORTER "构建Softimage导出工具" FALSE "Softimage_FOUND" FALSE)
option(NII_CONFIG_DOUBLE "在NII引擎中使用doubles代替floats" FALSE)
option(NII_BUILD_SAMPLES "构建Nii例子演示" TRUE)
option(NII_BUILD_TESTS "构建单元测试和PlayPen" FALSE)

if (SYMBIAN)
    # Symbian can only use the standard allocator
    set(NII_CONFIG_ALLOCATOR 1)
else ()
set(NII_CONFIG_ALLOCATOR 4 CACHE STRING
    "指定内存分配策略.可以选择如下:
    1 - Standard allocator
    2 - nedmalloc
    3 - User-provided allocator
    4 - nedmalloc with pooling"
)
endif()

cmake_dependent_option(NII_CONFIG_CONTAINERS_USE_CUSTOM_ALLOCATOR "STL containers in NII use the custom allocator" TRUE "NOT SYMBIAN" FALSE)
option(NII_CONFIG_STRING_USE_CUSTOM_ALLOCATOR "NII字符串使用自定义分配器" FALSE)
option(NII_CONFIG_MEMTRACK_DEBUG "在调试版本中启用NII's内存跟踪" FALSE)
option(NII_CONFIG_MEMTRACK_RELEASE "在发布版本中启用Nii's内存跟踪" FALSE)

# 确定线程选项
include(PrepareThreadingOptions)

cmake_dependent_option(NII_CONFIG_ENABLE_FREEIMAGE "构建FreeImage编码.如果你关闭这个选项,你需要提供自己的图片处理编码." TRUE "FreeImage_FOUND" FALSE)
option(NII_CONFIG_ENABLE_DDS "构建DDS编码." TRUE)
option(NII_CONFIG_ENABLE_PVRTC "构建PVRTC编码." FALSE)
cmake_dependent_option(NII_CONFIG_ENABLE_ZIP "构建ZIP档包支持.如果你关闭这个选项,你不能使用ZIP档包资源定位.例子演示将不能工作." TRUE "ZZip_FOUND" FALSE)
cmake_dependent_option(NII_USE_BOOST "使用Boost扩展" TRUE "Boost_FOUND" FALSE)
option(NII_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE "包含视口方向模式支持." FALSE)
option(NII_CONFIG_NEW_COMPILERS "使用新的脚本编译器." TRUE)
cmake_dependent_option(NII_CONFIG_ENABLE_GLES2_CG_SUPPORT "启用Cg支持GLES 2渲染系统" FALSE "NII_BUILD_RENDERSYSTEM_GLES2" FALSE)
cmake_dependent_option(NII_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER "启用GLSL优化器到GLES 2渲染系统中" FALSE "NII_BUILD_RENDERSYSTEM_GLES2" FALSE)
# 自定义安装需要
option(NII_INSTALL_SAMPLES "安装NII例子演示." TRUE)
option(NII_INSTALL_TOOLS "安装NII工具." TRUE)
option(NII_INSTALL_DOCS "安装文档." FALSE)
option(NII_INSTALL_SAMPLES_SOURCE "安装例子演示资源文件." FALSE)
# 编译器生成
cmake_dependent_option(NII_INSTALL_PDB "安装调试的pdb文件" FALSE "MSVC" FALSE)
cmake_dependent_option(NII_FULL_RPATH "从他们的安装路径中使用完整路径构建可运行时." FALSE "NOT WIN32" FALSE)
option(NII_PROFILING "起用内部单元时间测试支持." FALSE)
cmake_dependent_option(NII_CONFIG_STATIC_LINK_CRT "静态连接MS CRT dlls (msvcrt)" FALSE "MSVC" FALSE)
set(NII_LIB_DIRECTORY "lib${LIB_SUFFIX}" CACHE STRING "生成库安装路径,例如'lib64'是在一些64位Linux的发行版.")

# Unity build options
# A Unity build includes all sources files in just a few actual compilation units
# to potentially speed up the compilation.
option(NII_UNITY_BUILD "Enable unity build for NII." FALSE)
set(NII_UNITY_FILES_PER_UNIT "50" CACHE STRING "Number of files per compilation unit in Unity build.")

if (SYMBIAN)
SET (NII_SYMBIAN_UID_3 "0xA0007218" CACHE STRING 
    "Application UID (or UID3) is used to uniquely identify the binary (EXE or DLL) within the system. The purpose of UID 3 is to prevent one executable from interfering the operation of another executable.")
endif ()
# 隐藏高级选项
mark_as_advanced(
    NII_BUILD_RTSHADERSYSTEM_CORE_SHADERS 
    NII_BUILD_RTSHADERSYSTEM_EXT_SHADERS
    NII_CONFIG_DOUBLE
    NII_CONFIG_ALLOCATOR
    NII_CONFIG_CONTAINERS_USE_CUSTOM_ALLOCATOR
    NII_CONFIG_STRING_USE_CUSTOM_ALLOCATOR
    NII_CONFIG_MEMTRACK_DEBUG
    NII_CONFIG_MEMTRACK_RELEASE
    NII_CONFIG_NEW_COMPILERS
    NII_CONFIG_ENABLE_DDS
    NII_CONFIG_ENABLE_FREEIMAGE
    NII_CONFIG_ENABLE_PVRTC
    NII_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE
    NII_CONFIG_ENABLE_ZIP
    NII_CONFIG_ENABLE_GLES2_CG_SUPPORT
    NII_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER
    NII_USE_BOOST
    NII_INSTALL_SAMPLES_SOURCE
    NII_FULL_RPATH
    NII_PROFILING
    NII_CONFIG_STATIC_LINK_CRT
    NII_LIB_DIRECTORY
)

###################################################################
# 配置基于构建选项的全局构建选项
###################################################################
include(ConfigureBuild)

##################################################################
# 现在设立目标
##################################################################

# 安装资源文件
include(InstallResources)

# 设立NIIEngine工程
add_subdirectory(NIIEngine)

# 设立RenderSystems
#add_subdirectory(RenderSystems)

# 设立Plugins
#add_subdirectory(PlugIns)

# 设立Components
#add_subdirectory(Components)

# 设立tests(before samples so that PlayPen is included in browser)
#if (NII_BUILD_TESTS)
#    # enable CTest
#    ENABLE_TESTING()
#    INCLUDE(CTest)
#    add_subdirectory(Tests)
#endif ()

# 设立samples
#add_subdirectory(Samples) 

# 设立命令行工具
#if (NII_BUILD_TOOLS)
#  add_subdirectory(Tools)
#endif ()

# Setup XSIExporter
#if (NII_BUILD_XSIEXPORTER)
#  add_subdirectory(Tools/XSIExport)
#endif ()

# 安装文档
#add_subdirectory(doc)

# 安装媒体文件
#if (NII_INSTALL_SAMPLES OR NII_INSTALL_SAMPLES_SOURCE)
#  add_subdirectory(Samples/Media)
#endif ()

# 安装CMake模块
#add_subdirectory(CMake)

# 加入界面编辑器
#add_subdirectory(Designer)

# 提供CPack包装目标
include(Packaging)

# 显示功能摘要
include(FeatureSummary)