
message(STATUS "正在执行FeatureSummary.cmake脚本")

set(_features "\n")
set(_features "${_features}----------------------------------------------------------------------------\n")
set(_features "${_features}  功能摘要\n")
set(_features "${_features}----------------------------------------------------------------------------\n\n")

#summarise components
if (NII_BUILD_COMPONENT_PAGING)
	set(_components "${_components}  + Paging插件\n")
endif ()
if (NII_BUILD_COMPONENT_PROPERTY)
	set(_components "${_components}  + Property插件\n")
endif ()
if (NII_BUILD_COMPONENT_TERRAIN)
	set(_components "${_components}  + Terrain插件\n")
endif ()
if (NII_BUILD_COMPONENT_RTSHADERSYSTEM)
	set(_components "${_components}  + RTShader着色系统\n")
	
	if (NII_BUILD_RTSHADERSYSTEM_CORE_SHADERS)
		set(_components "${_components}  + RTShader着色系统核心着色器\n")
	endif ()
	
	if (NII_BUILD_RTSHADERSYSTEM_EXT_SHADERS)
		set(_components "${_components}  + RTShader着色系统扩展着色器\n")
	endif ()
	
endif ()

if (DEFINED _components)
	set(_features "${_features}构建成分:\n${_components}")
endif ()

# summarise plugins
if (NII_BUILD_PLUGIN_BSP)
	set(_plugins "${_plugins}  + BSP场景管理器\n")
endif ()
if (NII_BUILD_PLUGIN_CG)
	set(_plugins "${_plugins}  + Cg着色程序管理器\n")
endif ()
if (NII_BUILD_PLUGIN_OCTREE)
	set(_plugins "${_plugins}  + Octree场景管理器\n")
endif ()
if (NII_BUILD_PLUGIN_PCZ)
	set(_plugins "${_plugins}  + Portal connected zone场景管理器\n")
endif ()
if (NII_BUILD_PLUGIN_PFX)
	set(_plugins "${_plugins}  + 粒子FX插件\n")
endif ()

if (DEFINED _plugins)
	set(_features "${_features}构建插件:\n${_plugins}")
endif ()

# summarise rendersystems
if (NII_BUILD_RENDERSYSTEM_D3D9)
	set(_rendersystems "${_rendersystems}  + Direct3D 9渲染系统\n")
endif ()
if (NII_BUILD_RENDERSYSTEM_D3D11)
	set(_rendersystems "${_rendersystems}  + Direct3D 11渲染系统\n")
endif ()
if (NII_BUILD_RENDERSYSTEM_GL)
	set(_rendersystems "${_rendersystems}  + OpenGL渲染系统\n")
endif ()
if (NII_BUILD_RENDERSYSTEM_GLES)
	set(_rendersystems "${_rendersystems}  + OpenGLES 1.x渲染系统\n")
endif ()
if (NII_BUILD_RENDERSYSTEM_GLES2)
    set(_rendersystems "${_rendersystems}  + OpenGL ES 2.x\n")
endif ()
if (DEFINED _rendersystems)
	set(_features "${_features}构建渲染系统:\n${_rendersystems}")
endif ()

# summarise programs
if (NII_BUILD_SAMPLES)
	set(_programs "${_programs}  + 事例\n")
endif ()
if (NII_BUILD_TESTS)
	set(_programs "${_programs}  + 测试\n")
endif ()
if (NII_BUILD_TOOLS)
	set(_programs "${_programs}  + 工具\n")
endif ()

if (DEFINED _programs)
	set(_features "${_features}构建的可执行程序:\n${_programs}")
endif ()

# summarise core features
if (NII_CONFIG_ENABLE_DDS)
	set(_core "${_core}  + DDS 图片编码\n")
endif ()
if (NII_CONFIG_ENABLE_PVRTC)
	set(_core "${_core}  + PVRTC 图片编码\n")
endif ()
if (NII_CONFIG_ENABLE_FREEIMAGE)
	set(_core "${_core}  + FreeImage 图片编码\n")
endif ()
if (NII_CONFIG_ENABLE_ZIP)
	set(_core "${_core}  + ZIP 档包解码\n")
endif ()
if (NII_CONFIG_ENABLE_VIEWPORT_ORIENTATIONMODE)
	set(_core "${_core}  + 视口正交 模式支持\n")
endif ()

if (NII_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER)
    set(_core "${_core}  + GLSL Optimiser for OpenGL ES 2.0\n")
endif ()
if (NII_CONFIG_ENABLE_GLES2_CG_SUPPORT)
    set(_core "${_core}  + Cg for OpenGL ES 2.0\n")
endif ()

if (DEFINED _core)
	set(_features "${_features}构建核心功能:\n${_core}")
endif ()


set(_features "${_features}\n")


# miscellaneous
macro(var_to_string VAR STR)
	if (${VAR})
		set(${STR} "启用")
	else ()
		set(${STR} "禁用")
	endif ()
endmacro ()

# allocator settings
if (NII_CONFIG_ALLOCATOR EQUAL 1)
	set(_allocator "standard")
elseif (NII_CONFIG_ALLOCATOR EQUAL 2)
	set(_allocator "nedmalloc")
elseif (NII_CONFIG_ALLOCATOR EQUAL 3)
	set(_allocator "user")
else ()
    set(_allocator "nedmalloc(pooling)")
endif()
# various true/false settings
var_to_string(NII_CONFIG_CONTAINERS_USE_CUSTOM_ALLOCATOR _containers)
var_to_string(NII_CONFIG_DOUBLE _double)
var_to_string(NII_CONFIG_MEMTRACK_DEBUG _memtrack_debug)
var_to_string(NII_CONFIG_MEMTRACK_RELEASE _memtrack_release)
var_to_string(NII_CONFIG_NEW_COMPILERS _compilers)
var_to_string(NII_CONFIG_STRING_USE_CUSTOM_ALLOCATOR _string)
var_to_string(NII_USE_BOOST _boost)
# threading settings
if (NII_CONFIG_THREADS EQUAL 0)
	set(_threads "无")
elseif (NII_CONFIG_THREADS EQUAL 1)
	set(_threads "全线 (${NII_CONFIG_THREAD_PROVIDER})")
else ()
	set(_threads "后台 (${NII_CONFIG_THREAD_PROVIDER})")
endif ()
# build type
if (NII_STATIC)
	set(_buildtype "静态连接")
else ()
	set(_buildtype "动态连接")
endif ()

set(_features "${_features}构建类型:                      ${_buildtype}\n")
set(_features "${_features}线程支持:                      ${_threads}\n")
set(_features "${_features}使用双精:                      ${_double}\n")
set(_features "${_features}分配策略类型:                  ${_allocator}\n")
set(_features "${_features}STL容器使用的分配策略:          ${_containers}\n")
set(_features "${_features}字符串使用的分配策略:           ${_string}\n")
set(_features "${_features}内存跟踪(调试版本):             ${_memtrack_debug}\n")
set(_features "${_features}Memory tracker (发布版本):     ${_memtrack_release}\n")
set(_features "${_features}使用新的脚本编译器:             ${_compilers}\n")
set(_features "${_features}使用Boost标准c++库:            ${_boost}\n")


set(_features "${_features}\n----------------------------------------------------------------------------\n")
message(STATUS ${_features})
