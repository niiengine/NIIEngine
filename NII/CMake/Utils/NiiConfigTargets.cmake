

# Configure settings and install targets

# Default build output paths
if (NOT NII_ARCHIVE_OUTPUT)
    set(NII_ARCHIVE_OUTPUT ${NII_INTERMEDIATE_DIR}/NIIEngine/lib)
endif ()
if (NOT NII_LIBRARY_OUTPUT)
    set(NII_LIBRARY_OUTPUT ${NII_INTERMEDIATE_DIR}/NIIEngine/lib)
endif ()
if (NOT NII_RUNTIME_OUTPUT)
    set(NII_RUNTIME_OUTPUT ${NII_INTERMEDIATE_DIR}/NIIEngine/bin) 
endif ()

if (WIN32)
    set(NII_RELEASE_PATH "/Release")
    set(NII_RELWDBG_PATH "/RelWithDebInfo")
    set(NII_MINSIZE_PATH "/MinSizeRel")
    set(NII_DEBUG_PATH "/Debug")
    set(NII_LIB_RELEASE_PATH "/Release")
    set(NII_LIB_RELWDBG_PATH "/RelWithDebInfo")
    set(NII_LIB_MINSIZE_PATH "/MinSizeRel")
    set(NII_LIB_DEBUG_PATH "/Debug")
    set(NII_PLUGIN_PATH "/opt")
    set(NII_SAMPLE_PATH "/opt/samples")
elseif (UNIX)
    set(NII_RELEASE_PATH "")
    set(NII_RELWDBG_PATH "")
    set(NII_MINSIZE_PATH "")
    set(NII_DEBUG_PATH "/debug")
    if (NOT APPLE)
        set(NII_DEBUG_PATH "")
    endif ()
    set(NII_LIB_RELEASE_PATH "")
    set(NII_LIB_RELWDBG_PATH "")
    set(NII_LIB_MINSIZE_PATH "")
    set(NII_LIB_DEBUG_PATH "")
    if(APPLE AND NII_BUILD_PLATFORM_IPHONE)
        set(NII_LIB_RELEASE_PATH "/Release")
    endif(APPLE AND NII_BUILD_PLATFORM_IPHONE)
    if (APPLE)
        set(NII_PLUGIN_PATH "/")
    else()
        set(NII_PLUGIN_PATH "/NII")
    endif(APPLE)
        set(NII_SAMPLE_PATH "/NII/Samples")
elseif (SYMBIAN)
    set(NII_RELEASE_PATH ".")
    set(NII_RELWDBG_PATH ".")
    set(NII_MINSIZE_PATH ".")
    set(NII_DEBUG_PATH ".")
    set(NII_LIB_RELEASE_PATH ".")
    set(NII_LIB_RELWDBG_PATH ".")
    set(NII_LIB_MINSIZE_PATH ".")
    set(NII_LIB_DEBUG_PATH ".")
    set(NII_PLUGIN_PATH ".")
    set(NII_SAMPLE_PATH ".")  
endif ()

# create vcproj.user file for Visual Studio to set debug working directory
function(nii_create_vcproj_userfile TARGETNAME)
  if (MSVC)
    configure_file(
	  ${NII_TEMPLATES_DIR}/VisualStudioUserFile.vcproj.user.in
	  ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcproj.user
	  @ONLY
	)
    configure_file(
	  ${NII_TEMPLATES_DIR}/VisualStudioUserFile.vcxproj.user.in
	  ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcxproj.user
	  @ONLY
	)
  endif ()
endfunction(nii_create_vcproj_userfile)

# install targets according to current build type
function(nii_install_target TARGETNAME SUFFIX EXPORT)
	# Skip all install targets in SDK
	if (NII_SDK_BUILD)
		return()
	endif()

	if(EXPORT)
	  install(TARGETS ${TARGETNAME} #EXPORT Nii-exports
		BUNDLE DESTINATION "${TARGETNAME}/bin${NII_RELEASE_PATH}" CONFIGURATIONS Release None ""
		RUNTIME DESTINATION "${TARGETNAME}/bin${NII_RELEASE_PATH}" CONFIGURATIONS Release None ""
		LIBRARY DESTINATION "$${TARGETNAME}/{NII_LIB_DIRECTORY}${NII_LIB_RELEASE_PATH}${SUFFIX}" CONFIGURATIONS Release None ""
		ARCHIVE DESTINATION "$${TARGETNAME}/{NII_LIB_DIRECTORY}${NII_LIB_RELEASE_PATH}${SUFFIX}" CONFIGURATIONS Release None ""
		FRAMEWORK DESTINATION "$${TARGETNAME}/{NII_LIB_DIRECTORY}${NII_RELEASE_PATH}/Release" CONFIGURATIONS Release None ""
      )
	  install(TARGETS ${TARGETNAME} #EXPORT Nii-exports
		BUNDLE DESTINATION "${TARGETNAME}/bin${NII_RELWDBG_PATH}" CONFIGURATIONS RelWithDebInfo
		RUNTIME DESTINATION "${TARGETNAME}/bin${NII_RELWDBG_PATH}" CONFIGURATIONS RelWithDebInfo
		LIBRARY DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_RELWDBG_PATH}${SUFFIX}" CONFIGURATIONS RelWithDebInfo
		ARCHIVE DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_RELWDBG_PATH}${SUFFIX}" CONFIGURATIONS RelWithDebInfo
		FRAMEWORK DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_RELWDBG_PATH}/RelWithDebInfo" CONFIGURATIONS RelWithDebInfo
      )
	  install(TARGETS ${TARGETNAME} #EXPORT Nii-exports
		BUNDLE DESTINATION "${TARGETNAME}/bin${NII_MINSIZE_PATH}" CONFIGURATIONS MinSizeRel
		RUNTIME DESTINATION "${TARGETNAME}/bin${NII_MINSIZE_PATH}" CONFIGURATIONS MinSizeRel
		LIBRARY DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_MINSIZE_PATH}${SUFFIX}" CONFIGURATIONS MinSizeRel
		ARCHIVE DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_MINSIZE_PATH}${SUFFIX}" CONFIGURATIONS MinSizeRel
		FRAMEWORK DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_MINSIZE_PATH}/MinSizeRel" CONFIGURATIONS MinSizeRel
      )
	  install(TARGETS ${TARGETNAME} #EXPORT Nii-exports
		BUNDLE DESTINATION "${TARGETNAME}/bin${NII_DEBUG_PATH}" CONFIGURATIONS Debug
		RUNTIME DESTINATION "${TARGETNAME}/bin${NII_DEBUG_PATH}" CONFIGURATIONS Debug
		LIBRARY DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_DEBUG_PATH}${SUFFIX}" CONFIGURATIONS Debug
		ARCHIVE DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_DEBUG_PATH}${SUFFIX}" CONFIGURATIONS Debug
		FRAMEWORK DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_DEBUG_PATH}/Debug" CONFIGURATIONS Debug
	  )
	  #install(EXPORT Nii-exports DESTINATION ${NII_LIB_DIRECTORY})
	else()
	  install(TARGETS ${TARGETNAME}
		BUNDLE DESTINATION "${TARGETNAME}/bin${NII_RELEASE_PATH}" CONFIGURATIONS Release None ""
		RUNTIME DESTINATION "${TARGETNAME}/bin${NII_RELEASE_PATH}" CONFIGURATIONS Release None ""
		LIBRARY DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_RELEASE_PATH}${SUFFIX}" CONFIGURATIONS Release None ""
		ARCHIVE DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_RELEASE_PATH}${SUFFIX}" CONFIGURATIONS Release None ""
		FRAMEWORK DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_RELEASE_PATH}/Release" CONFIGURATIONS Release None ""
      )
	  install(TARGETS ${TARGETNAME}
		BUNDLE DESTINATION "${TARGETNAME}/bin${NII_RELWDBG_PATH}" CONFIGURATIONS RelWithDebInfo
		RUNTIME DESTINATION "${TARGETNAME}/bin${NII_RELWDBG_PATH}" CONFIGURATIONS RelWithDebInfo
		LIBRARY DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_RELWDBG_PATH}${SUFFIX}" CONFIGURATIONS RelWithDebInfo
		ARCHIVE DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_RELWDBG_PATH}${SUFFIX}" CONFIGURATIONS RelWithDebInfo
		FRAMEWORK DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_RELWDBG_PATH}/RelWithDebInfo" CONFIGURATIONS RelWithDebInfo
      )
	  install(TARGETS ${TARGETNAME}
		BUNDLE DESTINATION "${TARGETNAME}/bin${NII_MINSIZE_PATH}" CONFIGURATIONS MinSizeRel
		RUNTIME DESTINATION "${TARGETNAME}/bin${NII_MINSIZE_PATH}" CONFIGURATIONS MinSizeRel
		LIBRARY DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_MINSIZE_PATH}${SUFFIX}" CONFIGURATIONS MinSizeRel
		ARCHIVE DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_MINSIZE_PATH}${SUFFIX}" CONFIGURATIONS MinSizeRel
		FRAMEWORK DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_MINSIZE_PATH}/MinSizeRel" CONFIGURATIONS MinSizeRel
      )
	  install(TARGETS ${TARGETNAME}
		BUNDLE DESTINATION "${TARGETNAME}/bin${NII_DEBUG_PATH}" CONFIGURATIONS Debug
		RUNTIME DESTINATION "${TARGETNAME}/bin${NII_DEBUG_PATH}" CONFIGURATIONS Debug
		LIBRARY DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_DEBUG_PATH}${SUFFIX}" CONFIGURATIONS Debug
		ARCHIVE DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_LIB_DEBUG_PATH}${SUFFIX}" CONFIGURATIONS Debug
		FRAMEWORK DESTINATION "${TARGETNAME}/${NII_LIB_DIRECTORY}${NII_DEBUG_PATH}/Debug" CONFIGURATIONS Debug
	  )
	endif()

endfunction(nii_install_target)

# setup common target settings
function(nii_config_common TARGETNAME)
    set_target_properties(${TARGETNAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${NII_ARCHIVE_OUTPUT}
        LIBRARY_OUTPUT_DIRECTORY ${NII_LIBRARY_OUTPUT}
        RUNTIME_OUTPUT_DIRECTORY ${NII_RUNTIME_OUTPUT})
    if(NII_BUILD_PLATFORM_IPHONE)
        set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
        set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_UNROLL_LOOPS "YES")
        set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
        set_target_properties(${TARGETNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
    endif(NII_BUILD_PLATFORM_IPHONE)

    nii_create_vcproj_userfile(${TARGETNAME})
endfunction(nii_config_common)

# setup library build
function(nii_config_lib LIBNAME EXPORT)
    nii_config_common(${LIBNAME})
    if (NII_STATIC)
        # add static prefix, if compiling static version
        set_target_properties(${LIBNAME} PROPERTIES OUTPUT_NAME ${LIBNAME}Static)
    else (NII_STATIC)
        if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
            # add GCC visibility flags to shared library build
            set_target_properties(${LIBNAME} PROPERTIES COMPILE_FLAGS "${NII_GCC_VISIBILITY_FLAGS}")
            set_target_properties(${LIBNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "${XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN}")
            set_target_properties(${LIBNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "${XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN}")
        endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
        if (MINGW)
            # remove lib prefix from DLL outputs
            set_target_properties(${LIBNAME} PROPERTIES PREFIX "")
        endif ()
    endif (NII_STATIC)
    nii_install_target(${LIBNAME} "" ${EXPORT})
  
    if (NII_INSTALL_PDB)
        # install debug pdb files
        if (NII_STATIC)
            install(FILES ${NII_BINARY_DIR}/lib${NII_LIB_DEBUG_PATH}/${LIBNAME}Static_d.pdb
                DESTINATION ${LIBNAME}/${NII_LIB_DIRECTORY}${NII_LIB_DEBUG_PATH}
                CONFIGURATIONS Debug)
            install(FILES ${NII_BINARY_DIR}/lib${NII_LIB_RELWDBG_PATH}/${LIBNAME}Static.pdb
                DESTINATION ${LIBNAME}/${NII_LIB_DIRECTORY}${NII_LIB_RELWDBG_PATH}
                CONFIGURATIONS RelWithDebInfo)
        else ()
            install(FILES ${NII_BINARY_DIR}/bin${NII_DEBUG_PATH}/${LIBNAME}_d.pdb
                DESTINATION ${LIBNAME}/bin${NII_DEBUG_PATH} CONFIGURATIONS Debug)
            install(FILES ${NII_BINARY_DIR}/bin${NII_RELWDBG_PATH}/${LIBNAME}.pdb
                DESTINATION ${LIBNAME}/bin${NII_RELWDBG_PATH} CONFIGURATIONS RelWithDebInfo)
        endif ()
    endif ()
endfunction(nii_config_lib)

function(nii_config_component LIBNAME)
    nii_config_lib(${LIBNAME} FALSE)
endfunction(nii_config_component)


# setup plugin build
function(nii_config_plugin PLUGINNAME)
    nii_config_common(${PLUGINNAME})
    set_target_properties(${PLUGINNAME} PROPERTIES VERSION ${NII_SOVERSION})
    if (NII_STATIC)
        # add static prefix, if compiling static version
        set_target_properties(${PLUGINNAME} PROPERTIES OUTPUT_NAME ${PLUGINNAME}Static)

        if(NII_BUILD_PLATFORM_IPHONE)
            set_target_properties(${PLUGINNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
            set_target_properties(${PLUGINNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_UNROLL_LOOPS "YES")
            set_target_properties(${PLUGINNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
        endif(NII_BUILD_PLATFORM_IPHONE)
    else (NII_STATIC)
        if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
            # add GCC visibility flags to shared library build
            set_target_properties(${PLUGINNAME} PROPERTIES COMPILE_FLAGS "${NII_GCC_VISIBILITY_FLAGS}")
            set_target_properties(${PLUGINNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "${XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN}")
            set_target_properties(${PLUGINNAME} PROPERTIES XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "${XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN}")
            # disable "lib" prefix on Unix
        set_target_properties(${PLUGINNAME} PROPERTIES PREFIX "")
        endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)	
    endif (NII_STATIC)
    # export only if static
    nii_install_target(${PLUGINNAME} ${NII_PLUGIN_PATH} ${NII_STATIC})

    if (NII_INSTALL_PDB)
        # install debug pdb files
        if (NII_STATIC)
            install(FILES ${NII_BINARY_DIR}/lib${NII_LIB_DEBUG_PATH}/${PLUGINNAME}Static_d.pdb
                DESTINATION ${PLUGINNAME}/${NII_LIB_DIRECTORY}${NII_LIB_DEBUG_PATH}/opt
                CONFIGURATIONS Debug)
            install(FILES ${NII_BINARY_DIR}/lib${NII_LIB_RELWDBG_PATH}/${PLUGINNAME}Static.pdb
                DESTINATION ${PLUGINNAME}/${NII_LIB_DIRECTORY}${NII_LIB_RELWDBG_PATH}/opt
                CONFIGURATIONS RelWithDebInfo)
        else ()
            install(FILES ${NII_BINARY_DIR}/bin${NII_DEBUG_PATH}/${PLUGINNAME}_d.pdb
                DESTINATION ${PLUGINNAME}/bin${NII_DEBUG_PATH}
                CONFIGURATIONS Debug)
            install(FILES ${NII_BINARY_DIR}/bin${NII_RELWDBG_PATH}/${PLUGINNAME}.pdb
                DESTINATION ${PLUGINNAME}/bin${NII_RELWDBG_PATH}
                CONFIGURATIONS RelWithDebInfo)
        endif ()
    endif ()
endfunction(nii_config_plugin)

# setup NII sample build
function(nii_config_sample_common SAMPLENAME)
    nii_config_common(${SAMPLENAME})
    # set install RPATH for Unix systems
    if (UNIX AND NII_FULL_RPATH)
        set_property(TARGET ${SAMPLENAME} APPEND PROPERTY
            INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${NII_LIB_DIRECTORY})
        set_property(TARGET ${SAMPLENAME} PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
    endif ()
  
    if (APPLE)
        # On OS X, create .app bundle
        set_property(TARGET ${SAMPLENAME} PROPERTY MACOSX_BUNDLE TRUE)
        if (NOT NII_BUILD_PLATFORM_IPHONE)
            # Add the path where the Nii framework was found
            if(${NII_FRAMEWORK_PATH})
                set_target_properties(${SAMPLENAME} PROPERTIES
                    COMPILE_FLAGS "-F${NII_FRAMEWORK_PATH}"
                    LINK_FLAGS "-F${NII_FRAMEWORK_PATH}")
            endif()
        endif(NOT NII_BUILD_PLATFORM_IPHONE)
    endif (APPLE)
        if(NOT NII_STATIC)
        if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
            # add GCC visibility flags to shared library build
            set_target_properties(${SAMPLENAME} PROPERTIES COMPILE_FLAGS "${NII_GCC_VISIBILITY_FLAGS}")
            set_target_properties(${SAMPLENAME} PROPERTIES XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "${XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN}")
            set_target_properties(${SAMPLENAME} PROPERTIES XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN "${XCODE_ATTRIBUTE_GCC_INLINES_ARE_PRIVATE_EXTERN}")
            # disable "lib" prefix on Unix
            set_target_properties(${SAMPLENAME} PROPERTIES PREFIX "")
        endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)	
    endif()
    if (NII_INSTALL_SAMPLES)
        nii_install_target(${SAMPLENAME} ${NII_SAMPLE_PATH} FALSE)
    endif()
endfunction(nii_config_sample_common)

function(nii_config_sample_exe SAMPLENAME)
    nii_config_sample_common(${SAMPLENAME})
    if (NII_INSTALL_PDB AND NII_INSTALL_SAMPLES)
        # install debug pdb files - no _d on exe
        install(FILES ${NII_BINARY_DIR}/bin${NII_DEBUG_PATH}/${SAMPLENAME}.pdb
            DESTINATION ${SAMPLENAME}/bin${NII_DEBUG_PATH} CONFIGURATIONS Debug)
        install(FILES ${NII_BINARY_DIR}/bin${NII_RELWDBG_PATH}/${SAMPLENAME}.pdb
            DESTINATION ${SAMPLENAME}/bin${NII_RELWDBG_PATH} CONFIGURATIONS RelWithDebInfo)
    endif ()

    if (APPLE AND NOT NII_BUILD_PLATFORM_IPHONE AND NII_SDK_BUILD)
        # Add the path where the Nii framework was found
        if(NOT ${NII_FRAMEWORK_PATH} STREQUAL "")
            set_target_properties(${SAMPLENAME} PROPERTIES
            COMPILE_FLAGS "-F${NII_FRAMEWORK_PATH}"
            LINK_FLAGS "-F${NII_FRAMEWORK_PATH}")
        endif()
    endif(APPLE AND NOT NII_BUILD_PLATFORM_IPHONE AND NII_SDK_BUILD)
endfunction(nii_config_sample_exe)

function(nii_config_sample_lib SAMPLENAME)
    nii_config_sample_common(${SAMPLENAME})
    if (NII_INSTALL_PDB AND NII_INSTALL_SAMPLES)
        # install debug pdb files - with a _d on lib
        install(FILES ${NII_BINARY_DIR}/bin${NII_DEBUG_PATH}/${SAMPLENAME}_d.pdb
            DESTINATION ${SAMPLENAME}/bin${NII_DEBUG_PATH}CONFIGURATIONS Debug)
        install(FILES ${NII_BINARY_DIR}/bin${NII_RELWDBG_PATH}/${SAMPLENAME}.pdb
            DESTINATION ${SAMPLENAME}/bin${NII_RELWDBG_PATH}
            CONFIGURATIONS RelWithDebInfo)
    endif ()

    if (APPLE AND NOT NII_BUILD_PLATFORM_IPHONE AND NII_SDK_BUILD)
        # Add the path where the Nii framework was found
        if(NOT ${NII_FRAMEWORK_PATH} STREQUAL "")
        set_target_properties(${SAMPLENAME} PROPERTIES
            COMPILE_FLAGS "-F${NII_FRAMEWORK_PATH}" LINK_FLAGS "-F${NII_FRAMEWORK_PATH}")
        endif()
    endif(APPLE AND NOT NII_BUILD_PLATFORM_IPHONE AND NII_SDK_BUILD)

    # Add sample to the list of link targets
    # Global property so that we can build this up across entire sample tree
    # since vars are local to containing scope of directories / functions
    get_property(NII_SAMPLES_LIST GLOBAL PROPERTY "NII_SAMPLES_LIST")
    set_property (GLOBAL PROPERTY "NII_SAMPLES_LIST" ${NII_SAMPLES_LIST} ${SAMPLENAME})
endfunction(nii_config_sample_lib)


# setup Nii tool build
function(nii_config_tool TOOLNAME)
    nii_config_common(${TOOLNAME})

    #set _d debug postfix
    if (NOT APPLE)
        set_property(TARGET ${TOOLNAME} APPEND PROPERTY DEBUG_POSTFIX "_d")
    endif ()

    # set install RPATH for Unix systems
    if (UNIX AND NII_FULL_RPATH)
        set_property(TARGET ${TOOLNAME} APPEND PROPERTY
            INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${NII_LIB_DIRECTORY})
        set_property(TARGET ${TOOLNAME} PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
    endif ()

    if (NII_INSTALL_TOOLS)
        nii_install_target(${TOOLNAME} "" FALSE)
        if (NII_INSTALL_PDB)
            # install debug pdb files
            install(FILES ${NII_BINARY_DIR}/bin${NII_DEBUG_PATH}/${TOOLNAME}_d.pdb
                DESTINATION ${TOOLNAME}/bin${NII_DEBUG_PATH} CONFIGURATIONS Debug)
            install(FILES ${NII_BINARY_DIR}/bin${NII_RELWDBG_PATH}/${TOOLNAME}.pdb
                DESTINATION ${TOOLNAME}/bin${NII_RELWDBG_PATH} CONFIGURATIONS RelWithDebInfo)
        endif ()
    endif ()	

endfunction(nii_config_tool)

# Get component include dir (different when referencing SDK)
function(nii_add_component_include_dir COMPONENTNAME)
	if (NII_SDK_BUILD)
		include_directories("${NII_INCLUDE_DIR}/${COMPONENTNAME}")
	else()
		include_directories("${NII_SOURCE_DIR}/Components/${COMPONENTNAME}/include")	
	endif()
endfunction(nii_add_component_include_dir)
