
#############################################################
# Install useful CMake modules.
# These are necessary to compile the samples from the install
# directory, but can also be used for custom projects.
#############################################################

if(WIN32 OR APPLE)
	set(NII_CMAKE_DIR "CMake")
else()
	set(NII_CMAKE_DIR "${NII_LIB_DIRECTORY}/NII/cmake")
endif()

set(INST_FILES
    Packages/FindOIS.cmake
    Packages/FindNII.cmake
    Utils/FindPkgMacros.cmake
    Utils/MacroLogFeature.cmake
    Utils/PreprocessorUtils.cmake
    Utils/PrecompiledHeader.cmake
    Utils/NiiAddTargets.cmake  
    Utils/NiiConfigTargets.cmake
    Utils/NiiGetVersion.cmake
    # Utils/NiiFindFrameworks.cmake
) 
set(STATIC_INST_FILES
    Packages/FindCg.cmake
    Packages/FindDirectX.cmake
    Packages/FindFreeImage.cmake
    Packages/FindFreetype.cmake
    Packages/FindOpenGLES.cmake
    Packages/FindOpenGLES2.cmake
    Packages/FindPOCO.cmake
    Packages/FindTBB.cmake
    Packages/FindZLIB.cmake
    Packages/FindZZip.cmake
    Packages/FindSoftimage.cmake
)
if (WIN32)
    set(INST_FILES ${INST_FILES}
        Templates/VisualStudioUserFile.vcproj.user.in
        Templates/VisualStudioUserFile.vcxproj.user.in
    )
endif ()

if (APPLE)
    set(INST_FILES ${INST_FILES}
        Packages/FindIOKit.cmake
        Packages/FindCarbon.cmake
        Packages/FindCocoa.cmake
        Packages/FindCoreVideo.cmake
    )
    set(STATIC_INST_FILES ${STATIC_INST_FILES}
        Packages/FindiOSSDK.cmake
        Packages/FindGLSLOptimizer.cmake
        Packages/FindHLSL2GLSL.cmake
    )
endif ()

install(FILES ${INST_FILES} DESTINATION ${NII_CMAKE_DIR})
if (NII_STATIC)
    install(FILES ${STATIC_INST_FILES} DESTINATION ${NII_CMAKE_DIR})
endif ()
