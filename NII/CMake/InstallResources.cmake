
##################################################################
# Generate and install the config files needed for the samples
##################################################################

message(STATUS "正在执行InstallResources.cmake脚本")

if (WIN32)
    set(NII_MEDIA_PATH "media")
    set(NII_MEDIA_DIR_REL "../../${NII_MEDIA_PATH}")
    set(NII_MEDIA_DIR_DBG "../../${NII_MEDIA_PATH}")
    set(NII_PLUGIN_DIR_REL ".")
    set(NII_PLUGIN_DIR_DBG ".")
    set(NII_SAMPLES_DIR_REL ".")
    set(NII_SAMPLES_DIR_DBG ".")
    set(NII_CFG_INSTALL_PATH "bin")
elseif (APPLE)
    set(NII_MEDIA_PATH "Media")
    if(NII_BUILD_PLATFORM_IPHONE)
        set(NII_MEDIA_DIR_REL "${NII_MEDIA_PATH}")
        set(NII_MEDIA_DIR_DBG "${NII_MEDIA_PATH}")
    else()
        if(NII_INSTALL_SAMPLES_SOURCE)
            set(NII_MEDIA_PATH "Media")
            set(NII_MEDIA_DIR_REL "../../../${NII_MEDIA_PATH}")
            set(NII_MEDIA_DIR_DBG "../../../${NII_MEDIA_PATH}")
        else()
            set(NII_MEDIA_PATH "Samples/Media")
            set(NII_MEDIA_DIR_REL "../../../../${NII_MEDIA_PATH}")
            set(NII_MEDIA_DIR_DBG "../../../../${NII_MEDIA_PATH}")
        endif()
    endif()
    set(NII_PLUGIN_DIR_REL "")
    set(NII_PLUGIN_DIR_DBG "")
    set(NII_SAMPLES_DIR_REL "")
    set(NII_SAMPLES_DIR_DBG "")
    set(NII_CFG_INSTALL_PATH "bin")
elseif (UNIX)
    set(NII_MEDIA_PATH "share/NII/media")
    set(NII_MEDIA_DIR_REL "${CMAKE_INSTALL_PREFIX}/${NII_MEDIA_PATH}")
    set(NII_MEDIA_DIR_DBG "${CMAKE_INSTALL_PREFIX}/${NII_MEDIA_PATH}")
    set(NII_PLUGIN_DIR_REL "${CMAKE_INSTALL_PREFIX}/${NII_LIB_DIRECTORY}/NII")
    set(NII_PLUGIN_DIR_DBG "${CMAKE_INSTALL_PREFIX}/${NII_LIB_DIRECTORY}/NII")
    set(NII_SAMPLES_DIR_REL "${CMAKE_INSTALL_PREFIX}/${NII_LIB_DIRECTORY}/NII/Samples")
    set(NII_SAMPLES_DIR_DBG "${CMAKE_INSTALL_PREFIX}/${NII_LIB_DIRECTORY}/NII/Samples")
    set(NII_CFG_INSTALL_PATH "share/NII")
endif ()

# 配置plugins.cfg
if (NOT NII_BUILD_RENDERSYSTEM_D3D9)
    set(NII_COMMENT_RENDERSYSTEM_D3D9 "#")
endif ()

if (NOT NII_BUILD_RENDERSYSTEM_D3D11)
    set(NII_COMMENT_RENDERSYSTEM_D3D11 "#")
endif ()
if (CMAKE_SYSTEM_VERSION VERSION_LESS "6.0")
    set(NII_COMMENT_RENDERSYSTEM_D3D11 "#")
endif ()
if (NOT NII_BUILD_RENDERSYSTEM_GL)
    set(NII_COMMENT_RENDERSYSTEM_GL "#")
endif ()
if (NOT NII_BUILD_RENDERSYSTEM_GLES)
    set(NII_COMMENT_RENDERSYSTEM_GLES "#")
endif ()
if (NOT NII_BUILD_RENDERSYSTEM_GLES2)
    set(NII_COMMENT_RENDERSYSTEM_GLES2 "#")
endif ()
if (NOT NII_BUILD_PLUGIN_BSP)
    set(NII_COMMENT_PLUGIN_BSP "#")
endif ()
if (NOT NII_BUILD_PLUGIN_OCTREE)
    set(NII_COMMENT_PLUGIN_OCTREE "#")
endif ()
if (NOT NII_BUILD_PLUGIN_PCZ)
    set(NII_COMMENT_PLUGIN_PCZ "#")
endif ()
if (NOT NII_BUILD_PLUGIN_PFX)
    set(NII_COMMENT_PLUGIN_PARTICLEFX "#")
endif ()
if (NOT NII_BUILD_PLUGIN_CG)
    set(NII_COMMENT_PLUGIN_CG "#")
endif ()


# CREATE CONFIG FILES - INSTALL VERSIONS
# 创建 resources.cfg
configure_file(${NII_TEMPLATES_DIR}/resources_d.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/debug/resources_d.cfg)
configure_file(${NII_TEMPLATES_DIR}/resources.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/release/resources.cfg)
# 创建 plugins.cfg
configure_file(${NII_TEMPLATES_DIR}/plugins_d.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/debug/plugins_d.cfg)
configure_file(${NII_TEMPLATES_DIR}/plugins.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/release/plugins.cfg)
# 创建 quakemap.cfg
configure_file(${NII_TEMPLATES_DIR}/quakemap_d.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/debug/quakemap_d.cfg)
configure_file(${NII_TEMPLATES_DIR}/quakemap.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/release/quakemap.cfg)
# 创建 samples.cfg
configure_file(${NII_TEMPLATES_DIR}/samples_d.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/debug/samples_d.cfg)
configure_file(${NII_TEMPLATES_DIR}/samples.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/release/samples.cfg)
# create samples.cfg
configure_file(${NII_TEMPLATES_DIR}/tests.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/release/tests.cfg)
configure_file(${NII_TEMPLATES_DIR}/tests_d.cfg.in ${NII_INTERMEDIATE_DIR}/inst/bin/debug/tests_d.cfg)

# 安装资源文件
if (NII_INSTALL_SAMPLES OR NII_INSTALL_SAMPLES_SOURCE)
    install(FILES 
        ${NII_INTERMEDIATE_DIR}/inst/bin/debug/resources_d.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/debug/plugins_d.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/debug/samples_d.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/debug/tests_d.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/debug/quakemap_d.cfg
        DESTINATION "NIIEngine/${NII_CFG_INSTALL_PATH}${NII_DEBUG_PATH}" CONFIGURATIONS Debug)
    install(FILES 
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/resources.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/plugins.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/samples.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/tests.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/quakemap.cfg
        DESTINATION "NIIEngine/${NII_CFG_INSTALL_PATH}${NII_RELEASE_PATH}" CONFIGURATIONS Release None "")
    install(FILES 
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/resources.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/plugins.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/samples.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/tests.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/quakemap.cfg
        DESTINATION "NIIEngine/${NII_CFG_INSTALL_PATH}${NII_RELWDBG_PATH}" CONFIGURATIONS RelWithDebInfo)
    install(FILES 
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/resources.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/plugins.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/samples.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/tests.cfg
        ${NII_INTERMEDIATE_DIR}/inst/bin/release/quakemap.cfg
        DESTINATION "NIIEngine/${NII_CFG_INSTALL_PATH}${NII_MINSIZE_PATH}" CONFIGURATIONS MinSizeRel)
        # Need a special case here for the iOS SDK, configuration is not being matched, could be a CMake bug.
    if (NII_BUILD_PLATFORM_IPHONE)
        install(FILES 
            ${NII_INTERMEDIATE_DIR}/inst/bin/release/resources.cfg
            ${NII_INTERMEDIATE_DIR}/inst/bin/release/plugins.cfg
            ${NII_INTERMEDIATE_DIR}/inst/bin/release/samples.cfg
            ${NII_INTERMEDIATE_DIR}/inst/bin/release/tests.cfg
            ${NII_INTERMEDIATE_DIR}/inst/bin/release/quakemap.cfg
            DESTINATION "NIIEngine/${NII_CFG_INSTALL_PATH}${NII_RELEASE_PATH}")
    endif()
endif (NII_INSTALL_SAMPLES OR NII_INSTALL_SAMPLES_SOURCE)


# CREATE CONFIG FILES - BUILD DIR VERSIONS
if (NOT NII_BUILD_PLATFORM_IPHONE)
    set(NII_MEDIA_DIR_REL "${NII_SOURCE_DIR}/Samples/Media")
    set(NII_MEDIA_DIR_DBG "${NII_SOURCE_DIR}/Samples/Media")
else ()
    # iOS needs to use relative paths in the config files
    set(NII_MEDIA_DIR_REL "${NII_MEDIA_PATH}")
    set(NII_MEDIA_DIR_DBG "${NII_MEDIA_PATH}")
endif ()

if (WIN32)
    set(NII_PLUGIN_DIR_REL ".")
    set(NII_PLUGIN_DIR_DBG ".")
    set(NII_SAMPLES_DIR_REL ".")
    set(NII_SAMPLES_DIR_DBG ".")
elseif (APPLE)
    # not used on OS X, uses Resources
    set(NII_PLUGIN_DIR_REL "")
    set(NII_PLUGIN_DIR_DBG "")
    set(NII_SAMPLES_DIR_REL "")
    set(NII_SAMPLES_DIR_DBG "")
elseif (UNIX)
    set(NII_PLUGIN_DIR_REL "${NII_BUILDALL_DIR}/lib")
    set(NII_PLUGIN_DIR_DBG "${NII_BUILDALL_DIR}/lib")
    set(NII_SAMPLES_DIR_REL "${NII_BUILDALL_DIR}/lib")
    set(NII_SAMPLES_DIR_DBG "${NII_BUILDALL_DIR}/lib")
endif ()

if (MSVC AND NOT NMAKE)
    # 创建 resources.cfg
    configure_file(${NII_TEMPLATES_DIR}/resources_d.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/debug/resources_d.cfg)
    configure_file(${NII_TEMPLATES_DIR}/resources.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/release/resources.cfg)
    configure_file(${NII_TEMPLATES_DIR}/resources.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/relwithdebinfo/resources.cfg)
    configure_file(${NII_TEMPLATES_DIR}/resources.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/minsizerel/resources.cfg)
    # 创建 plugins.cfg
    configure_file(${NII_TEMPLATES_DIR}/plugins_d.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/debug/plugins_d.cfg)
    configure_file(${NII_TEMPLATES_DIR}/plugins.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/release/plugins.cfg)
    configure_file(${NII_TEMPLATES_DIR}/plugins.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/relwithdebinfo/plugins.cfg)
    configure_file(${NII_TEMPLATES_DIR}/plugins.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/minsizerel/plugins.cfg)
    # 创建 quakemap.cfg
    configure_file(${NII_TEMPLATES_DIR}/quakemap_d.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/debug/quakemap_d.cfg)
    configure_file(${NII_TEMPLATES_DIR}/quakemap.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/release/quakemap.cfg)
    configure_file(${NII_TEMPLATES_DIR}/quakemap.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/relwithdebinfo/quakemap.cfg)
    configure_file(${NII_TEMPLATES_DIR}/quakemap.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/minsizerel/quakemap.cfg)
    # 创建 samples.cfg
    configure_file(${NII_TEMPLATES_DIR}/samples_d.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/debug/samples_d.cfg)
    configure_file(${NII_TEMPLATES_DIR}/samples.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/release/samples.cfg)
    configure_file(${NII_TEMPLATES_DIR}/samples.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/relwithdebinfo/samples.cfg)
    configure_file(${NII_TEMPLATES_DIR}/samples.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/minsizerel/samples.cfg)
    # create tests.cfg
    configure_file(${NII_TEMPLATES_DIR}/tests_d.cfg.in ${NII_BUILDALL_DIR}/bin/debug/tests_d.cfg)
    configure_file(${NII_TEMPLATES_DIR}/tests.cfg.in ${NII_BUILDALL_DIR}/bin/release/tests.cfg)
    configure_file(${NII_TEMPLATES_DIR}/tests.cfg.in ${NII_BUILDALL_DIR}/bin/relwithdebinfo/tests.cfg)
    configure_file(${NII_TEMPLATES_DIR}/tests.cfg.in ${NII_BUILDALL_DIR}/bin/minsizerel/tests.cfg)
else() #其他操作只需要一个配置文件
    string(TOLOWER "${CMAKE_BUILD_TYPE}" NII_BUILD_TYPE)
    if (NII_BUILD_TYPE STREQUAL "debug" AND NOT APPLE)
        set(NII_CFG_SUFFIX "_d")
    endif ()
    # 创建 resources.cfg
    configure_file(${NII_TEMPLATES_DIR}/resources${NII_CFG_SUFFIX}.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/resources${NII_CFG_SUFFIX}.cfg)
    # 创建 plugins.cfg
    configure_file(${NII_TEMPLATES_DIR}/plugins${NII_CFG_SUFFIX}.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/plugins${NII_CFG_SUFFIX}.cfg)
    # 创建 quakemap.cfg
    configure_file(${NII_TEMPLATES_DIR}/quakemap${NII_CFG_SUFFIX}.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/quakemap${NII_CFG_SUFFIX}.cfg)
    # 创建 samples.cfg
    configure_file(${NII_TEMPLATES_DIR}/samples${NII_CFG_SUFFIX}.cfg.in ${NII_BUILDALL_DIR}/NIIEngine/bin/samples${NII_CFG_SUFFIX}.cfg)
    # create tests.cfg
    configure_file(${NII_TEMPLATES_DIR}/tests${NII_CFG_SUFFIX}.cfg.in ${NII_BUILDALL_DIR}/bin/tests${NII_CFG_SUFFIX}.cfg)
endif ()

