#核心部分
PROJECT(NIIEngine)

#包含基本路径
set(SOURCE_NIIENGINE_DIR ${NII_BUILDALL_DIR}/NIIEngineSource)
include_directories("${SOURCE_NIIENGINE_DIR}/include")
include_directories("${SOURCE_NIIENGINE_DIR}/other")
##########################################设置平台参数##########################################
if(NII_BUILD_PLATFORM_NACL)
    # google native client 平台
    set(NIIENGINE_BUILD_PLATFORM "NII_NACL")
elseif (WIN32)
    #window平台
    set(NIIENGINE_BUILD_PLATFORM "NII_WIN32")
elseif (APPLE)
    if (NII_BUILD_PLATFORM_IPHONE)
        set(NIIENGINE_BUILD_PLATFORM "NII_IOS")
    else ()
        #苹果普通系统
        set(NIIENGINE_BUILD_PLATFORM "NII_OSX")
    endif ()
elseif (UNIX)
    set(NIIENGINE_BUILD_PLATFORM "NII_LINUX")
elseif (SYMBIAN)
    set(NIIENGINE_BUILD_PLATFORM "NII_SYMBIAN")
endif(NII_BUILD_PLATFORM_NACL)

set(HEADER_FILES "")
set(SOURCE_FILES "")
set(RESOURCE_FILES "")
get_nii_include_and_source(NII.cmd ${NIIEngine_SOURCE_DIR} ${SOURCE_NIIENGINE_DIR} ${NIIENGINE_BUILD_PLATFORM} HEADER_FILES SOURCE_FILES RESOURCE_FILES)
#############################################################################################
# 设置外围参数
if(NII_BUILD_PLATFORM_NACL)
    set(PLATFORM_LIBS nosys)
    set(PLATFORM_HEADER_INSTALL "NaCl")    
elseif (WIN32)
    #window平台
    source_group(Resources FILES ${RESOURCE_FILES})
    set(PLATFORM_HEADER_INSTALL "WIN32")
    if (MINGW)
        get_filename_component(NII_MINGW_PATH ${CMAKE_CXX_COMPILER} PATH)
        file(GLOB NII_MINGW_RC_NAMES "${NII_MINGW_PATH}/*windres*")
        find_program(NII_MINGW_RC NAMES ${NII_MINGW_RC_NAMES} HINTS ${NII_MINGW_PATH} DOC "MinGW资源编译器路径")
        if (NOT NII_MINGW_RC)
            message(SEND_ERROR "不能找到MinGW资源编译器.请指定NII_MINGW_RC路径.")
        endif ()
        mark_as_advanced(NII_MINGW_RC)
        add_custom_command(OUTPUT ${NII_BUILDALL_DIR}/NIIEngineSource/other/NiiWin32Resources.rc.obj
        COMMAND ${NII_MINGW_RC} -I${NII_BUILDALL_DIR}/NIIEngineSource/other/
	                           -i${NII_BUILDALL_DIR}/NIIEngineSource/other/NiiWin32Resources.rc
							   -o${NII_BUILDALL_DIR}/NIIEngineSource/other/NiiWin32Resources.rc.obj
        DEPENDS ${RESOURCE_FILES})
        list(APPEND SOURCE_FILES ${NII_BUILDALL_DIR}/NIIEngineSource/other/NiiWin32Resources.rc.obj)
    else()
        list(APPEND SOURCE_FILES ${NII_BUILDALL_DIR}/NIIEngineSource/other/NiiWin32Resources.rc)
    endif ()
elseif (APPLE)
    if (NII_BUILD_PLATFORM_IPHONE)
        #苹果手机
        set(PLATFORM_LIBS "")
        set(PLATFORM_HEADER_INSTALL "iPhone")
    else ()
        #苹果普通系统
        source_group(Resources FILES ${RESOURCE_FILES})
        set(PLATFORM_LIBS ${Carbon_LIBRARY_FWK} ${Cocoa_LIBRARY_FWK})
        set(PLATFORM_HEADER_INSTALL "OSX")
    endif ()
elseif (UNIX)
    set(PLATFORM_LIBS ${X11_LIBRARIES} ${X11_Xt_LIB} ${XAW_LIBRARY} pthread)
    set(PLATFORM_HEADER_INSTALL "GLX")    
    # 一些平台需要明确去连接libdl
    find_library(DL_LIBRARY NAMES dl dlsym)
    mark_as_advanced(DL_LIBRARY)
    if (DL_LIBRARY)
        list(APPEND PLATFORM_LIBS dl)
    endif ()
elseif (SYMBIAN)
    set(PLATFORM_HEADER_INSTALL "SYMBIAN")
endif()

######################################## 配置线程文件##############################################
if (N_THREAD EQUAL 0)
    set(NIIENGINE_BUILD_THREAD "NII_NONE_THREAD")    
elseif (N_THREAD EQUAL 1)
    set(NIIENGINE_BUILD_THREAD "NII_BOOST_THREAD")
elseif (N_THREAD EQUAL 2)
    set(NIIENGINE_BUILD_THREAD "NII_POCO_THREAD")
elseif (N_THREAD EQUAL 3)
    set(NIIENGINE_BUILD_THREAD "NII_TBB_THREAD")
endif ()

set(THREAD_HEADER_FILES  "")
set(THREAD_SOURCE_FILES "")
get_nii_spec_include_and_source(NII.cmd ${NIIEngine_SOURCE_DIR} ${SOURCE_NIIENGINE_DIR} ${NIIENGINE_BUILD_THREAD} THREAD_HEADER_FILES THREAD_SOURCE_FILES)
##################################################################################################
#添加生成动态库符号输出
add_definitions(-DNII_NONCLIENT_BUILD -DFREEIMAGE_LIB -D_MT -D_USRDLL)
##################################################################################################
get_nii_spec_include_and_source(NII.cmd ${NIIEngine_SOURCE_DIR} ${SOURCE_NIIENGINE_DIR} NII_NEDMALLOC SOME1_HEADER_FILES SOME1_SOURCE_FILES)
#################################################################################################
set(LIBRARIES ${FREETYPE_LIBRARIES} ${PLATFORM_LIBS})

if (NII_CONFIG_THREADS)
    list(APPEND LIBRARIES ${NII_THREAD_LIBRARIES})
endif ()

if (NII_CONFIG_ENABLE_FREEIMAGE)
    get_nii_spec_include_and_source(NII.cmd ${NIIEngine_SOURCE_DIR} ${SOURCE_NIIENGINE_DIR} NII_FreeImage SOME1_HEADER_FILES SOME1_SOURCE_FILES)
    list(APPEND LIBRARIES "${FreeImage_LIBRARIES}")
    list(APPEND HEADER_FILES ${SOME1_HEADER_FILES})
    list(APPEND SOURCE_FILES ${SOME1_SOURCE_FILES})
endif()

if (NII_CONFIG_ENABLE_DDS)
    get_nii_spec_include_and_source(NII.cmd ${NIIEngine_SOURCE_DIR} ${SOURCE_NIIENGINE_DIR} NII_DDS SOME1_HEADER_FILES SOME1_SOURCE_FILES)
    list(APPEND HEADER_FILES ${SOME1_HEADER_FILES})
    list(APPEND SOURCE_FILES ${SOME1_SOURCE_FILES})
endif ()

if (NII_CONFIG_ENABLE_PVRTC)
    get_nii_spec_include_and_source(NII.cmd ${NIIEngine_SOURCE_DIR} ${SOURCE_NIIENGINE_DIR} NII_PVRTC SOME1_HEADER_FILES SOME1_SOURCE_FILES)
    list(APPEND HEADER_FILES ${SOME1_HEADER_FILES})
    list(APPEND SOURCE_FILES ${SOME1_SOURCE_FILES})
endif ()

if (NII_CONFIG_ENABLE_ZIP)
    set(SOME1_HEADER_FILES "")
    set(SOME1_SOURCE_FILES "")
    get_nii_spec_include_and_source(NII.cmd ${NIIEngine_SOURCE_DIR} ${SOURCE_NIIENGINE_DIR} NII_ZIP SOME1_HEADER_FILES SOME1_SOURCE_FILES)
    list(APPEND HEADER_FILES ${SOME1_HEADER_FILES})
    list(APPEND SOURCE_FILES ${SOME1_SOURCE_FILES})
    list(APPEND LIBRARIES "${ZZip_LIBRARIES}")
    list(APPEND LIBRARIES "${ZLIB_LIBRARIES}")
endif ()

if (NII_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER)
    list(APPEND LIBRARIES "${GLSL_Optimizer_LIBRARIES}")
endif ()

if (NII_CONFIG_ENABLE_GLES2_CG_SUPPORT)
    list(APPEND LIBRARIES "${HLSL2GLSL_LIBRARIES}")
endif ()

set (TARGET_LINK_FLAGS "")

# 建立 NIIEngine 目标
nii_add_library(NIIEngine ${NII_LIB_TYPE} ${PREC_HEADER} ${HEADER_FILES} ${SOURCE_FILES} ${THREAD_HEADER_FILES} ${THREAD_SOURCE_FILES})
# 在VS2010 - 64 位会有一个错误: "LINK : fatal error LNK1210: exceeded internal ILK size limit; link with /INCREMENTAL:NO"
if(WIN32 AND MSVC10 AND CMAKE_CL_64)
    set_target_properties(NIIEngine PROPERTIES VERSION ${NII_SOVERSION} LINK_FLAGS "/INCREMENTAL:NO")
else()
    set_target_properties(NIIEngine PROPERTIES VERSION ${NII_SOVERSION})
endif()

if (APPLE)
    if (NOT NII_BUILD_PLATFORM_IPHONE)
        set_target_properties(NIIEngine PROPERTIES INSTALL_NAME_DIR "NII")
    else ()    
        set_target_properties(NIIEngine PROPERTIES FRAMEWORK TRUE)
        # 设置INSTALL_PATH,以至于框架定位
        set_target_properties(NIIEngine PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Frameworks")

        set_target_properties(NIIEngine PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
        set_target_properties(NIIEngine PROPERTIES RESOURCE "${RESOURCE_FILES}")
        set_source_files_properties("${RESOURCE_FILES}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        set_target_properties(NIIEngine PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
        set_target_properties(NIIEngine PROPERTIES XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${NII_SOURCE_DIR}/NIIEngine/PreInclude/NiiPreProcess.h")
        set_target_properties(NIIEngine PROPERTIES XCODE_ATTRIBUTE_GCC_UNROLL_LOOPS "YES")
        add_custom_command(TARGET NIIEngine POST_BUILD
            COMMAND mkdir ARGS -p ${NII_BINARY_DIR}/lib/$(CONFIGURATION)/Nii.framework/Headers/ COMMENT "创建头文件"
            COMMAND ditto ${SOURCE_NIIENGINE_DIR}/include/*.h ${NII_BINARY_DIR}/lib/$(CONFIGURATION)/Nii.framework/Headers/ COMMENT "关联头文件符号")
    endif ()

    # 框架名字为'Nii'
    set_target_properties(NIIEngine PROPERTIES OUTPUT_NAME NIIEngine)
endif ()

#设置NIIEngine需要连接的第三方库
target_link_libraries(NIIEngine ${LIBRARIES})
# mingw 中需要去连接winsotck
if (MINGW)
  # FreeImage需要winsock htons字节序转换函数
  target_link_libraries(NIIEngine ws2_32)
endif ()

# 指定使用的预编译头
use_precompiled_header(NIIEngine
  "${SOURCE_NIIENGINE_DIR}/include/NiiPreProcess.h"
  "${SOURCE_NIIENGINE_DIR}/src/NiiCPH.cpp"
)
# 安装 NIIEngine
nii_config_lib(NIIEngine TRUE)
install(FILES ${HEADER_FILES} DESTINATION NIIEngine/include/NII)
install(FILES ${THREAD_HEADER_FILES} DESTINATION NIIEngine/include/NII/Threading)