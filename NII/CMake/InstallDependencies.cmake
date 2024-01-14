message(STATUS "正在执行InstallDependencies.cmake脚本")

if (NOT APPLE AND NOT WIN32)
    return()
endif()

# TODO - most of this file assumes a common dependencies root folder
# This is not robust, we should instead source dependencies from their individual locations
get_filename_component(NII_DEP_DIR ${OIS_INCLUDE_DIR}/../../ ABSOLUTE)

option(NII_INSTALL_DEPENDENCIES "安装例子演示需要的依赖库" TRUE)
option(NII_COPY_DEPENDENCIES "复制依赖库到构建路径中" TRUE)

macro(install_debug INPUT DIR)
    if (EXISTS ${NII_DEP_DIR}/bin/debug/${INPUT})
        if (IS_DIRECTORY ${NII_DEP_DIR}/bin/debug/${INPUT})
            install(DIRECTORY ${NII_DEP_DIR}/bin/debug/${INPUT} DESTINATION ${DIR}/bin/debug CONFIGURATIONS Debug)
        else ()
            install(FILES ${NII_DEP_DIR}/bin/debug/${INPUT} DESTINATION ${DIR}/bin/debug CONFIGURATIONS Debug)
        endif ()
    else()
        message(send_error "${NII_DEP_DIR}/bin/debug/${INPUT}不存在,不能安装!")
    endif ()
endmacro(install_debug)

macro(install_release INPUT DIR)
    if (EXISTS ${NII_DEP_DIR}/bin/release/${INPUT})
        if (IS_DIRECTORY ${NII_DEP_DIR}/bin/release/${INPUT})
            install(DIRECTORY ${NII_DEP_DIR}/bin/release/${INPUT} DESTINATION ${DIR}/bin/release CONFIGURATIONS Release None "")
            install(DIRECTORY ${NII_DEP_DIR}/bin/release/${INPUT} DESTINATION ${DIR}/bin/relwithdebinfo CONFIGURATIONS RelWithDebInfo)
            install(DIRECTORY ${NII_DEP_DIR}/bin/release/${INPUT} DESTINATION ${DIR}/bin/minsizerel CONFIGURATIONS MinSizeRel)
        else ()
            install(FILES ${NII_DEP_DIR}/bin/release/${INPUT} DESTINATION ${DIR}/bin/release CONFIGURATIONS Release None "")
            install(FILES ${NII_DEP_DIR}/bin/release/${INPUT} DESTINATION ${DIR}/bin/relwithdebinfo CONFIGURATIONS RelWithDebInfo)
            install(FILES ${NII_DEP_DIR}/bin/release/${INPUT} DESTINATION ${DIR}/bin/minsizerel CONFIGURATIONS MinSizeRel)
        endif ()    
    else()
        message(send_error "${NII_DEP_DIR}/bin/release/${INPUT}不存在,不能安装!")
    endif ()
endmacro(install_release)

macro(copy_debug INPUT DIR)
    if (EXISTS ${NII_DEP_DIR}/lib/debug/${INPUT})
        if (MINGW OR NMAKE)
            configure_file(${NII_DEP_DIR}/lib/debug/${INPUT} ${DIR}/lib/${INPUT} COPYONLY)
        else ()
            if (IS_DIRECTORY ${NII_DEP_DIR}/lib/debug/${INPUT})
                install(DIRECTORY ${NII_DEP_DIR}/lib/debug/${INPUT} DESTINATION ${DIR}/lib/debug)
            else ()
                configure_file(${NII_DEP_DIR}/lib/debug/${INPUT} ${DIR}/lib/debug/${INPUT} COPYONLY)
            endif ()            
        endif ()
    endif ()
endmacro(copy_debug)

macro(copy_release INPUT DIR)
    if (EXISTS ${NII_DEP_DIR}/lib/release/${INPUT})
        if (MINGW OR NMAKE)
            configure_file(${NII_DEP_DIR}/lib/release/${INPUT} ${DIR}/lib/${INPUT} COPYONLY)
        else ()
            if (IS_DIRECTORY ${NII_DEP_DIR}/lib/release/${INPUT})
                install(DIRECTORY ${NII_DEP_DIR}/lib/release/${INPUT} DESTINATION ${DIR}/lib/release CONFIGURATIONS Release None "")
                install(DIRECTORY ${NII_DEP_DIR}/lib/release/${INPUT} DESTINATION ${DIR}/lib/relwithdebinfo CONFIGURATIONS RelWithDebInfo)
                install(DIRECTORY ${NII_DEP_DIR}/lib/release/${INPUT} DESTINATION ${DIR}/lib/minsizerel CONFIGURATIONS MinSizeRel)
            else ()
                configure_file(${NII_DEP_DIR}/lib/release/${INPUT} ${DIR}/lib/release/${INPUT} COPYONLY)
                configure_file(${NII_DEP_DIR}/lib/release/${INPUT} ${DIR}/lib/relwithdebinfo/${INPUT} COPYONLY)
                configure_file(${NII_DEP_DIR}/lib/release/${INPUT} ${DIR}/lib/minsizerel/${INPUT} COPYONLY)
            endif ()
        endif ()
    endif ()
endmacro(copy_release)

if (NII_INSTALL_DEPENDENCIES)
    if (NII_STATIC)
        # 在静态编译中，工程依赖于需要的第三方库,因此复制完整的include和lib路径
        if (EXISTS ${NII_DEP_DIR}/include/)
            install(DIRECTORY ${NII_DEP_DIR}/include/ DESTINATION NIIEngine/include)
        endif ()
        if (EXISTS ${NII_DEP_DIR}/lib/)
            install(DIRECTORY ${NII_DEP_DIR}/lib/ DESTINATION NIIEngine/lib)
        endif ()
    else ()
	    # 在非静态编译中，我们例子演示仅仅需要OIS
        if (EXISTS ${NII_DEP_DIR}/include/OIS/)
            install(DIRECTORY ${NII_DEP_DIR}/include/OIS DESTINATION NIIEngine/include)
        endif ()
        if(WIN32)
            if (EXISTS ${NII_DEP_DIR}/lib/debug/OIS_d.lib)
                install(FILES
                ${NII_DEP_DIR}/lib/debug/OIS_d.lib
                DESTINATION NIIEngine/lib/debug CONFIGURATIONS Debug)
            endif ()
            if (EXISTS ${NII_DEP_DIR}/lib/release/OIS.lib)
                install(FILES
                ${NII_DEP_DIR}/lib/release/OIS.lib
                DESTINATION NIIEngine/lib/release CONFIGURATIONS Release RelWithDebInfo MinSizeRel None "")
            endif ()
            if (MINGW)
                install(FILES ${OIS_LIBRARY_DBG} DESTINATION NIIEngine/lib/debug CONFIGURATIONS Debug)
                install(FILES ${OIS_LIBRARY_REL} DESTINATION NIIEngine/lib/relwithdebinfo CONFIGURATIONS RelWithDebInfo)
                install(FILES ${OIS_LIBRARY_REL} DESTINATION NIIEngine/lib/release CONFIGURATIONS Release)
                install(FILES ${OIS_LIBRARY_REL} DESTINATION NIIEngine/lib/minsizerel CONFIGURATIONS MinSizeRel)
            endif ()
        elseif(APPLE)
            if (EXISTS ${NII_DEP_DIR}/lib/debug/libOIS.a)
                install(FILES
                    ${NII_DEP_DIR}/lib/debug/libOIS.a
                    DESTINATION NIIEngine/lib/debug CONFIGURATIONS Debug)
            endif ()
            if (EXISTS ${NII_DEP_DIR}/lib/release/libOIS.a)
                install(FILES
                  ${NII_DEP_DIR}/lib/release/libOIS.a
                  DESTINATION NIIEngine/lib/release CONFIGURATIONS Release RelWithDebInfo MinSizeRel None "")
            endif()
        endif()
    endif(NII_STATIC)

    if(WIN32)
        # copy the dependency DLLs to the right places
        install_debug(OIS_d.dll NIIEngine)
        install_release(OIS.dll NIIEngine)
        if (NII_BUILD_PLUGIN_CG)
            # if MinGW or NMake, the release/debug cg.dll's would conflict, so just pick one
            if (MINGW OR (CMAKE_GENERATOR STREQUAL "NMake Makefiles"))
                if (CMAKE_BUILD_TYPE STREQUAL "Debug")
                    install_debug(cg.dll NIIEngine)
                else ()
                    install_release(cg.dll NIIEngine)
                endif ()
            else ()
                install_debug(cg.dll NIIEngine)
                install_release(cg.dll NIIEngine)
            endif ()
        endif ()

        # install GLES dlls
        if (NII_BUILD_RENDERSYSTEM_GLES)
            install_debug(libgles_cm.dll NIIEngine)
            install_release(libgles_cm.dll NIIEngine)
        endif ()
        
        # install GLES2 dlls
        if (NII_BUILD_RENDERSYSTEM_GLES2)
            install_debug(libGLESv2.dll NIIEngine)
            install_release(libEGL.dll NIIEngine)
        endif ()        
    elseif(APPLE)
        # copy the dependency libs to the right places
        install_debug(libOIS.a NIIEngine)
        install_release(libOIS.a NIIEngine)

        if (NII_BUILD_PLUGIN_CG)
            install_debug(Cg.framework NIIEngine)
            install_release(Cg.framework NIIEngine)
        endif()
    endif()#Win32orApple

    # If we're installing the sample source for an SDK, also install Boost headers & libraries
    if (NII_INSTALL_SAMPLES_SOURCE AND NOT NII_BUILD_PLATFORM_IPHONE)
        if (Boost_FOUND)
            # headers (try to exclude things we don't need)
            install(DIRECTORY "${Boost_INCLUDE_DIR}/boost" DESTINATION "boost"
                PATTERN "accumulators" EXCLUDE
                PATTERN "archive" EXCLUDE
                PATTERN "asio" EXCLUDE
                PATTERN "assign" EXCLUDE
                PATTERN "bimap" EXCLUDE
                PATTERN "chrono" EXCLUDE
                PATTERN "circular_buffer" EXCLUDE
                PATTERN "compatibility" EXCLUDE
                PATTERN "concept_check" EXCLUDE
                PATTERN "container" EXCLUDE
                PATTERN "dynamic_bitset" EXCLUDE
                PATTERN "filesystem" EXCLUDE
                PATTERN "flyweight" EXCLUDE
                PATTERN "format" EXCLUDE
                PATTERN "functional" EXCLUDE
                PATTERN "fusion" EXCLUDE
                PATTERN "geometry" EXCLUDE
                PATTERN "gil" EXCLUDE
                PATTERN "graph" EXCLUDE
                PATTERN "interprocess" EXCLUDE
                PATTERN "intrusive" EXCLUDE
                PATTERN "iostreams" EXCLUDE
                PATTERN "lambda" EXCLUDE
                PATTERN "logic" EXCLUDE
                PATTERN "mpi" EXCLUDE
                PATTERN "multi_array" EXCLUDE
                PATTERN "multi_index" EXCLUDE
                PATTERN "numeric" EXCLUDE
                PATTERN "parameter" EXCLUDE
                PATTERN "pending" EXCLUDE
                PATTERN "phoenix" EXCLUDE
                PATTERN "pool" EXCLUDE
                PATTERN "program_options" EXCLUDE
                PATTERN "property_map" EXCLUDE
                PATTERN "property_tree" EXCLUDE
                PATTERN "proto" EXCLUDE
                PATTERN "ptr_container" EXCLUDE
                PATTERN "python" EXCLUDE
                PATTERN "random" EXCLUDE
                PATTERN "ratio" EXCLUDE
                PATTERN "regex" EXCLUDE
                PATTERN "serialization" EXCLUDE
                PATTERN "signals" EXCLUDE
                PATTERN "signals2" EXCLUDE
                PATTERN "spirit" EXCLUDE
                PATTERN "statechart" EXCLUDE
                PATTERN "system" EXCLUDE
                PATTERN "test" EXCLUDE
                PATTERN "timer" EXCLUDE
                PATTERN "tr1" EXCLUDE
                PATTERN "typeof" EXCLUDE
                PATTERN "units" EXCLUDE
                PATTERN "unordered" EXCLUDE
                PATTERN "uuid" EXCLUDE
                PATTERN "variant" EXCLUDE
                PATTERN "wave" EXCLUDE
                PATTERN "xpressive" EXCLUDE )
            # 协议
            if (EXISTS "${Boost_INCLUDE_DIR}/boost/LICENSE_1_0.txt")
                install(FILES "${Boost_INCLUDE_DIR}/boost/LICENSE_1_0.txt" DESTINATION "NIIEngine/boost")
            elseif (EXISTS "${Boost_INCLUDE_DIR}/LICENSE_1_0.txt")
                install(FILES "${Boost_INCLUDE_DIR}/LICENSE_1_0.txt" DESTINATION "NIIEngine/boost")
            endif ()
            # 库
            if (Boost_THREAD_FOUND)
                install(FILES ${Boost_THREAD_LIBRARY_DEBUG} DESTINATION "NIIEngine/boost/lib" CONFIGURATIONS Debug)
                install(FILES ${Boost_THREAD_LIBRARY_RELEASE} DESTINATION "NIIEngine/boost/lib" CONFIGURATIONS Release)
            endif()
            if (Boost_DATE_TIME_FOUND)
                install(FILES ${Boost_DATE_TIME_LIBRARY_DEBUG} DESTINATION "NIIEngine/boost/lib" CONFIGURATIONS Debug)
                install(FILES ${Boost_DATE_TIME_LIBRARY_RELEASE} DESTINATION "NIIEngine/boost/lib" CONFIGURATIONS Release)
            endif()
        endif()#Boost_FOUND
    endif()#NII_INSTALL_SAMPLES_SOURCE
endif ()#NII_INSTALL_DEPENDENCIES

if (NII_COPY_DEPENDENCIES)
    if (WIN32)
        # copy the required DLLs to the build directory (configure_file is the only copy-like op I found in CMake)
        copy_debug(OIS_d.dll NIIEngine)
        copy_release(OIS.dll NIIEngine)
        if (NII_BUILD_PLUGIN_CG)
            # if MinGW or NMake, the release/debug cg.dll's would conflict, so just pick one
            if (MINGW OR (CMAKE_GENERATOR STREQUAL "NMake Makefiles"))
                if (CMAKE_BUILD_TYPE STREQUAL "Debug")
                    copy_debug(cg.dll NIIEngine)
                else()
                    copy_release(cg.dll NIIEngine)
                endif()
            else ()
                copy_debug(cg.dll NIIEngine)
                copy_release(cg.dll NIIEngine)
            endif ()
        endif ()
        if (NII_BUILD_RENDERSYSTEM_GLES)
            copy_debug(libgles_cm.dll NIIEngine)
            copy_release(libgles_cm.dll NIIEngine)
        endif ()
        
        if (NII_BUILD_RENDERSYSTEM_GLES2)	
            copy_debug(libEGL.dll NIIEngine)
            copy_debug(libGLESv2.dll NIIEngine)
            copy_release(libEGL.dll NIIEngine)
            copy_release(libGLESv2.dll NIIEngine)
        endif ()        
    elseif(APPLE)
        # copy the required libs and frameworks to the build directory (configure_file is the only copy-like op I found in CMake)
        copy_debug(libOIS.a NIIEngine)
        copy_release(libOIS.a NIIEngine)

        if (NII_BUILD_PLUGIN_CG)
            copy_debug(Cg.framework NIIEngine)
            copy_release(Cg.framework NIIEngine)
        endif ()
    endif ()#WIN32
endif ()#NII_COPY_DEPENDENCIES
