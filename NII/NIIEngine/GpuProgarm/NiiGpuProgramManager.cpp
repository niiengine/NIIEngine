/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#include "NiiPreProcess.h"
#include "NiiGpuProgramManager.h"
#include "NiiException.h"
#include "NiiDefaultStream.h"
#include "NiiEngine.h"

#define _GPM_SAVE       0x01
#define _GPM_SAVEFile   0x02
#define _GPM_DIRTY      0x04
namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GpuProgramManager
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    /// default implementation for RenderSystems without assembly shader support
    class UnsupportedGpuProgram : public GpuProgram
    {
    public:
        UnsupportedGpuProgram(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0, ResResultScheme * rs = 0,
            ScriptTypeID stid = N_CmdObj_GpuProgram, LangID lid = N_PrimaryLang) : 
            GpuProgram(rid, gid, ls, rs, stid, lid) { }

        bool isValid() const{ return false; }
    protected:
        void loadImpl()     { N_EXCEPT(RenderingAPI, _T("no support gpu program, shader name:") + mSrc + _T("\n")); }
        void loadCodeImpl(const VString & code) { N_EXCEPT(RenderingAPI, _T("no support gpu program, shader name:") + mSrc + _T("\n")); }
        void unloadImpl()   {}
    };
    //------------------------------------------------------------------------
    template<> GpuProgramManager * Singleton<GpuProgramManager>::mOnly = 0;
    const GpuParamValue GpuProgramManager::GPVNone(_T(""));
    //-----------------------------------------------------------------------
    Resource * GpuProgramManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
        String paramType;
        Nui32 paramSyntax;
        if (!params || (!params->get(N_PropertyGpuProgram_Syntax, paramSyntax)) ||
            ((paramType = params->get(N_PropertyGpuProgram_Type)) == N_StrBlank))
        {
            N_EXCEPT(InvalidParam, _I18n("You must supply 'syntax' and 'type' parameters"));
        }

        GpuProgram::ShaderType gpt;
        if (paramType == _T("vp"))
        {
            gpt = GpuProgram::ST_VS;
        }
        else if (paramType == _T("tp"))
        {
            gpt = GpuProgram::ST_TS;
        }
        else if (paramType == _T("dp"))
        {
            gpt = GpuProgram::ST_DS;
        }
        else if (paramType == _T("gp"))
        {
            gpt = GpuProgram::ST_GS;
        }
        else if (paramType == _T("fp"))
        {
            gpt = GpuProgram::ST_FS;
        }
        else if (paramType == _T("cp"))
        {
            gpt = GpuProgram::ST_CS;
        }
        else
        {
            gpt = GpuProgram::ST_FS;
        }
        return createImpl(rid, gid, ls, rs, gpt, (ShaderLanguage)paramSyntax);
    }
    //------------------------------------------------------------------------
    Resource * GpuProgramManager::createImpl(ResourceID rid, GroupID gid, 
        ResLoadScheme * ls, ResResultScheme * rs, GpuProgram::ShaderType type, ShaderLanguage sl)
    {
        return N_new UnsupportedGpuProgram(rid, gid, ls, rs);
    }
    //------------------------------------------------------------------------
    GpuProgramManager::GpuProgramManager():
        ResourceManager(RT_GpuProgram),
        mCacheMark(0),
        mDebug(false)
    {
        mScriptLevel = 50;
        mSyncParamList.reserve(GSP_Frame_CostTime + 1);
        mSyncParamList.emplace(_T("NULL"),                          GpuParamUnit(GSP_Null,                              0, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("projection_matrix"),             GpuParamUnit(GSP_Matrix_Proj,                        16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("view_matrix"),                   GpuParamUnit(GSP_Matrix_View,                        16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("world_matrix"),                  GpuParamUnit(GSP_Matrix_Space,                       16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("viewproj_matrix"),               GpuParamUnit(GSP_Matrix_Proj_X_View,                 16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("worldview_matrix"),              GpuParamUnit(GSP_Matrix_View_X_Space,                16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("worldviewproj_matrix"),          GpuParamUnit(GSP_Matrix_Proj_X_View_X_Space,         16, SPI_None, GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("inverse_projection_matrix"),     GpuParamUnit(GSP_Matrix_Proj_Inverse,                16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("inverse_view_matrix"),           GpuParamUnit(GSP_Matrix_View_Inverse,                16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("inverse_world_matrix"),          GpuParamUnit(GSP_Matrix_Space_Inverse,               16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("inverse_viewproj_matrix"),       GpuParamUnit(GSP_Matrix_Proj_X_View_Inverse,         16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("inverse_worldview_matrix"),      GpuParamUnit(GSP_Matrix_View_X_Space_Inverse,        16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("inverse_worldviewproj_matrix"),  GpuParamUnit(GSP_Matrix_Proj_X_View_X_Space_Inverse, 16, SPI_None, GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("transpose_projection_matrix"),   GpuParamUnit(GSP_Matrix_Proj_Transpose,              16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("transpose_view_matrix"),         GpuParamUnit(GSP_Matrix_View_Transpose,              16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("transpose_world_matrix"),        GpuParamUnit(GSP_Matrix_Space_Transpose,             16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("transpose_viewproj_matrix"),     GpuParamUnit(GSP_Matrix_Proj_X_View_Transpose,       16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("transpose_worldview_matrix"),    GpuParamUnit(GSP_Matrix_View_X_Space_Transpose,      16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("transpose_worldviewproj_matrix"),GpuParamUnit(GSP_Matrix_Proj_X_View_X_Space_Transpose, 16, SPI_None, GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("inverse_transpose_projection_matrix"),GpuParamUnit(GSP_Matrix_Proj_InverseTranspose, 16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("inverse_transpose_view_matrix"),GpuParamUnit(GSP_Matrix_View_InverseTranspose,       16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("inverse_transpose_world_matrix"),GpuParamUnit(GSP_Matrix_Space_InverseTranspose,     16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("inverse_transpose_viewproj_matrix"),GpuParamUnit(GSP_Matrix_Proj_X_View_InverseTranspose,16,SPI_None,       GDT_Float));
        mSyncParamList.emplace(_T("inverse_transpose_worldview_matrix"),GpuParamUnit(GSP_Matrix_View_X_Space_InverseTranspose,16, SPI_None,    GDT_Float));
        mSyncParamList.emplace(_T("inverse_transpose_worldviewproj_matrix"),GpuParamUnit(GSP_Matrix_Proj_X_View_X_Space_InverseTranspose, 16, SPI_None,GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("world_matrix_array_3x4"),        GpuParamUnit(GSP_Matrix3x4_Space_Array,             12, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("world_matrix_array"),            GpuParamUnit(GSP_Matrix4x4_Space_Array,             16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("world_matrix_translate_array"),  GpuParamUnit(GSP_Matrix3x4_Space_Translate,         16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("world_matrix_scale_array"),      GpuParamUnit(GSP_Matrix3x4_Space_Scale,             16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("world_matrix_rotate_array"),     GpuParamUnit(GSP_Matrix3x4_Space_Rotate,            16, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("world_scale_shear_matrix_array_3x4"),GpuParamUnit(GSP_Matrix3x4_Space_Scale_Shear,   9, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("world_dualquaternion_array_2x4"),GpuParamUnit(GSP_DualQuaternion_Space,              8, SPI_None, GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("viewport_width"),                GpuParamUnit(GSP_View_PixelWidth,                   1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("viewport_height"),               GpuParamUnit(GSP_View_PixelHeight,                  1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("viewport_size"),                 GpuParamUnit(GSP_View_PixelSize,                    4, SPI_None, GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("camera_position"),               GpuParamUnit(GSP_Camera_Pos,                        3, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("camera_position_object_space"),  GpuParamUnit(GSP_Camera_Pos_ChSpace,                3, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("view_direction"),                GpuParamUnit(GSP_Camera_Dir,                        3, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("view_side_vector"),              GpuParamUnit(GSP_Camera_Right,                      3, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("view_up_vector"),                GpuParamUnit(GSP_Camera_Up,                         3, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("fov"),                           GpuParamUnit(GSP_Camera_FOV,                        1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("near_clip_distance"),            GpuParamUnit(GSP_Camera_NearClip,                   1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("far_clip_distance"),             GpuParamUnit(GSP_Camera_FarClip,                    1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("lod_camera_position"),           GpuParamUnit(GSP_Camera_LOD_Pos,                    3, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("lod_camera_position_object_space"),GpuParamUnit(GSP_Camera_LOD_Pos_ChSpace,          3, SPI_None, GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("ambient_light_colour"),          GpuParamUnit(GSP_Ambient,                           4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("fps"),                           GpuParamUnit(GSP_FPS,                               1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("vertex_winding"),                GpuParamUnit(GSP_Vertex_Winding,                    1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("shadow_extrusion_distance"),     GpuParamUnit(GSP_Shadow_VolExtent,                  1, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("shadow_scene_depth_range"),      GpuParamUnit(GSP_Shadow_ChDepthRange_ParamIndex,    4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("shadow_colour"),                 GpuParamUnit(GSP_Shadow_Colour,                     4, SPI_None, GDT_Float));
        //mSyncParamList.emplace(_T("fog_colour"),                  GpuParamUnit(GSP_Fog_Colour,                        4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("fog_params"),                    GpuParamUnit(GSP_Fog_Params,                        8, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("point_params"),                  GpuParamUnit(GSP_Ponit_Params,                      4, SPI_None, GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("surface_ambient_colour"),        GpuParamUnit(GSP_Ch_Ambient,                        4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("surface_diffuse_colour"),        GpuParamUnit(GSP_Ch_Diffuse,                        4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("surface_specular_colour"),       GpuParamUnit(GSP_Ch_Specular,                       4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("surface_emissive_colour"),       GpuParamUnit(GSP_Ch_Emissive,                       4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("surface_shininess"),             GpuParamUnit(GSP_Ch_Shininess,                      1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("derived_scene_colour"),          GpuParamUnit(GSP_Ch_Emissive_Evn,                   4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("derived_ambient_light_colour"),  GpuParamUnit(GSP_Ch_Ambient_X_Env,                  4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("scene_depth_range"),             GpuParamUnit(GSP_Ch_Depth_Range,                    4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("pass_number"),                   GpuParamUnit(GSP_Ch_Index,                          1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("pass_iteration_number"),         GpuParamUnit(GSP_Ch_RenderCount,                    1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("custom"),                        GpuParamUnit(GSP_Ch_Custom,                         4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("animation_parametric"),          GpuParamUnit(GSP_Ch_AniParam,                       4, SPI_Int,  GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("light_number"),                  GpuParamUnit(GSP_Light_Index_ParamIndex,            1, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_diffuse_colour"),          GpuParamUnit(GSP_Light_Diffuse_ParamIndex,          4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_specular_colour"),         GpuParamUnit(GSP_Light_Specular_ParamIndex,         4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_attenuation"),             GpuParamUnit(GSP_Light_Attenuation_ParamIndex,      4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_position"),                GpuParamUnit(GSP_Light_Pos_ParamIndex,              4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_direction"),               GpuParamUnit(GSP_Light_Dir_ParamIndex,              4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_distance_object_space"),   GpuParamUnit(GSP_Light_Dis_ChSpace_ParamIndex,      1, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_position_object_space"),   GpuParamUnit(GSP_Light_Pos_ChSpace_ParamIndex,      4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_position_view_space"),     GpuParamUnit(GSP_Light_Pos_ChView_ParamIndex,       4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_direction_object_space"),  GpuParamUnit(GSP_Light_Dir_ChSpace_ParamIndex,      4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_direction_view_space"),    GpuParamUnit(GSP_Light_Dir_ChView_ParamIndex,       4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("spotlight_params"),              GpuParamUnit(GSP_Light_Spot_Params_ParamIndex,      4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_casts_shadows"),           GpuParamUnit(GSP_Light_CastShadow_ParamIndex,       1, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("light_power"),                   GpuParamUnit(GSP_Light_BrightFactor_ParamIndex,     1, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("derived_light_diffuse_colour"),  GpuParamUnit(GSP_Light_Diffuse_Ch_ParamIndex,       4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("derived_light_specular_colour"), GpuParamUnit(GSP_Light_Specular_Ch_ParamIndex,      4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_attenuation_array"),       GpuParamUnit(GSP_Light_Attenuation_ParamCount,      4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_diffuse_colour_array"),    GpuParamUnit(GSP_Light_Diffuse_ParamCount,          4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_specular_colour_array"),   GpuParamUnit(GSP_Light_Specular_ParamCount,         4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_position_array"),          GpuParamUnit(GSP_Light_Pos_ParamCount,              4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_position_object_space_array"),GpuParamUnit(GSP_Light_Pos_ChSpace_ParamCount,   4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_position_view_space_array"), GpuParamUnit(GSP_Light_Pos_ChView_ParamCount,     4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_direction_array"),         GpuParamUnit(GSP_Light_Dir_ParamCount,              4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_direction_object_space_array"),GpuParamUnit(GSP_Light_Dir_ChSpace_ParamCount,  4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_direction_view_space_array"),GpuParamUnit(GSP_Light_Dir_ChView_ParamCount,     4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_distance_object_space_array"),GpuParamUnit(GSP_Light_Dis_ChSpace_ParamCount,   1, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_power_array"),             GpuParamUnit(GSP_Light_BrightFactor_ParamCount,     1, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("spotlight_params_array"),        GpuParamUnit(GSP_Light_Spot_Params_ParamCount,      4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("derived_light_diffuse_colour_array"),GpuParamUnit(GSP_Light_Diffuse_Ch_ParamCount,   4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("derived_light_specular_colour_array"),GpuParamUnit(GSP_Light_Specular_Ch_ParamCount, 4, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("spotlight_viewproj_matrix"),     GpuParamUnit(GSP_Light_Spot_Matrix_Proj_X_View_ParamIndex,  16,SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("spotlight_viewproj_matrix_array"),GpuParamUnit(GSP_Light_Spot_Matrix_Proj_X_View_ParamCount, 16,SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("spotlight_worldviewproj_matrix"),GpuParamUnit(GSP_Light_Spot_Matrix_Proj_X_View_X_Space,     16,SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("light_count"),                   GpuParamUnit(GSP_Light_ActiveCount,                 1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("light_custom"),                  GpuParamUnit(GSP_Light_Custom,                      4, SPI_Int,  GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mSyncParamList.emplace(_T("render_target_flipping"),       GpuParamUnit(GSP_Texture_Flipping,                   1, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("texture_size"),                 GpuParamUnit(GSP_Texture_Dim_ParamIndex,             4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("packed_texture_size"),          GpuParamUnit(GSP_Texture_DimUnit_ParamIndex,         4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("inverse_texture_size"),         GpuParamUnit(GSP_Texture_InverseDim_ParamIndex,      4, SPI_Int,  GDT_Float));
        mSyncParamList.emplace(_T("texture_matrix"),               GpuParamUnit(GSP_Texture_Matrix_Transform_ParamIndex,16, SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("texture_viewproj_matrix"),      GpuParamUnit(GSP_Texture_Matrix_Proj_X_View_ParamIndex,  16,SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("texture_worldviewproj_matrix"), GpuParamUnit(GSP_Texture_Matrix_Proj_X_View_X_Space_ParamIndex,  16,SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("texture_viewproj_matrix_array"),GpuParamUnit(GSP_Texture_Matrix_Proj_X_View_ParamCount,  16,SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("texture_worldviewproj_matrix_array"),GpuParamUnit(GSP_Texture_Matrix_Proj_X_View_X_Space_ParamCount, 16,SPI_Int, GDT_Float));
        mSyncParamList.emplace(_T("texel_offsets"),                GpuParamUnit(GSP_Texel_Oft,                          4, SPI_None, GDT_Float));
        mSyncParamList.emplace(_T("time"),                         GpuParamUnit(GSP_Sys_CostTime,                       1, SPI_Float,GDT_Float));
        mSyncParamList.emplace(_T("time_0_x"),                     GpuParamUnit(GSP_Sys_CostTime_Fmod_ParamData,        4, SPI_Float,GDT_Float));
        mSyncParamList.emplace(_T("frame_time"),                   GpuParamUnit(GSP_Frame_CostTime,                     1, SPI_Float,GDT_Float));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    }
    //------------------------------------------------------------------------
    GpuProgramManager::~GpuProgramManager()
    {
        ShareCustomList::iterator i, iend = mShareCustomList.end();
        for(i = mShareCustomList.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mShareCustomList.clear();

        CacheList::iterator j, jend = mCacheList.end();
        for(j = mCacheList.begin(); j != jend; ++j)
        {
            N_delete j->second;
        }
        mCacheList.clear();
    }
    //------------------------------------------------------------------------
    bool GpuProgramManager::isSupport(ShaderLanguage lang) const
    {
        return N_Engine().getRender()->getFeature()->isSupport(lang);
    }
    //------------------------------------------------------------------------
    ShaderLanguageMark GpuProgramManager::getSupportMark() const
    {
        return N_Engine().getRender()->getFeature()->getShaderMark();
    }
    //------------------------------------------------------------------------
    GpuProgram * GpuProgramManager::load(ResourceID rid, GroupID gid,
        const String & file, GpuProgram::ShaderType gptype, ShaderLanguage lang)
    {
        GpuProgram * prg;
        {
            N_mutex1_lock
            prg = static_cast<GpuProgram *>(get(rid));

            if(prg == 0)
            {
                prg = create(rid, gid, file, gptype, lang);
            }
        }

        prg->load();
        return prg;
    }
    //------------------------------------------------------------------------
    GpuProgram * GpuProgramManager::load(ResourceID rid, GroupID gid,
        GpuProgram::ShaderType type, ShaderLanguage lang, const VString & src)
    {
        GpuProgram * prg;
        {
            N_mutex1_lock
            prg = static_cast<GpuProgram *>(get(rid));

            if(prg == 0)
            {
                prg = create(rid, gid, type, lang, src);
            }

        }
        prg->load();
        return prg;
    }
    //------------------------------------------------------------------------
    GpuProgram * GpuProgramManager::create(ResourceID rid, GroupID gid,
        GpuProgram::ShaderType type, ShaderLanguage lang, ResLoadScheme * ls,
            ResResultScheme * rs)
    {
        GpuProgram * re = static_cast<GpuProgram *>(createImpl(rid, gid, ls, rs, type, lang));
        addImpl(re);

        N_Only(ResourceScheme).onCreate(re);
        return re;
    }
    //------------------------------------------------------------------------
    GpuProgram * GpuProgramManager::create(ResourceID rid, GroupID gid,
        const String & file, GpuProgram::ShaderType type, ShaderLanguage lang)
    {
        GpuProgram * prg = create(rid, gid, type, lang);
        prg->setType(type);
        prg->setSyntaxType(lang);
        prg->setProgramSrc(file);
        return prg;
    }
    //------------------------------------------------------------------------
    GpuProgram * GpuProgramManager::create(ResourceID rid, GroupID gid,
        GpuProgram::ShaderType gptype, ShaderLanguage lang, const VString & src)
    {
        GpuProgram * prg = create(rid, gid, gptype, lang);
        prg->setType(gptype);
        prg->setSyntaxType(lang);
        prg->setProgramCode(src);
        return prg;
    }
    //------------------------------------------------------------------------
    Resource * GpuProgramManager::get(ResourceID rid, GroupID gid)
    {
        Resource * re = N_Only(HighLevelGpuProgram).get(rid, gid);
        if(re != 0)
            return re;

        return ResourceManager::get(rid, gid);
    }
    //------------------------------------------------------------------------
    GpuParamValue * GpuProgramManager::createShare(const String & name)
    {
        ShareCustomList::iterator i = mShareCustomList.find(name);
        if(i != mShareCustomList.end())
        {
            return i->second;
        }
        GpuParamValue * re = N_new GpuParamValue(name);
        mShareCustomList[name] = re;
        return re;
    }
    //------------------------------------------------------------------------
    void GpuProgramManager::destroyShare(const String & name)
    {
        ShareCustomList::iterator i = mShareCustomList.find(name);
        if(i != mShareCustomList.end())
        {
            N_delete i->second;
            mShareCustomList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    GpuParamValue * GpuProgramManager::getShare(const String & name) const
    {
        ShareCustomList::const_iterator i = mShareCustomList.find(name);
        if(i != mShareCustomList.end())
        {
            return i->second;
        }
        return &GPVNone;
    }
    //------------------------------------------------------------------------
    void GpuProgramManager::setCacheBinary(bool val)
    {
        if(val)
        {
            if(N_Engine().getRender()->getFeature()->isSupport(GF_Shader_Binary_Get))
                mCacheMark |= _GPM_SAVE;
            else
                mCacheMark &= ~_GPM_SAVE;
        }
        else
            mCacheMark &= ~_GPM_SAVE;
    }
    //------------------------------------------------------------------------
    bool GpuProgramManager::isCacheBinary() const
    {
        return mCacheMark & _GPM_SAVE;
    }
    //------------------------------------------------------------------------
    void GpuProgramManager::setCacheBinaryFile(bool val)
    {
        if (val)
        {
            if(N_Engine().getRender()->getFeature()->isSupport(GF_Shader_Binary_Get))
                mCacheMark |= _GPM_SAVEFile;
            else
                mCacheMark &= ~_GPM_SAVEFile;
        }
        else
            mCacheMark &= ~_GPM_SAVEFile;
    }
    //------------------------------------------------------------------------
    bool GpuProgramManager::isCacheBinaryFile() const
    {
        return mCacheMark & _GPM_SAVEFile;
    }
    //------------------------------------------------------------------------
    MemStream * GpuProgramManager::getCache(Nui32 cid) const
    {
        CacheList::iterator foundIter = mCacheList.find(cid);
        if(foundIter != mCacheList.end())
            return foundIter->second;
        return 0;
    }
    //------------------------------------------------------------------------
    MemStream * GpuProgramManager::createCache(Nui32 cid, NCount size)
    {
        MemStream * re = N_new MemStream(size);

        CacheList::iterator foundIter = mCacheList.find(cid);
        if(foundIter == mCacheList.end())
        {
            mCacheList.insert(Npair(cid, re));
            mCacheMark |= _GPM_DIRTY;
        }
        else
        {
            foundIter->second = re;
        }

        return re;
    }
    //------------------------------------------------------------------------
    void GpuProgramManager::destroyCache(Nui32 cid)
    {
        CacheList::iterator i = mCacheList.find(cid);
        if(i != mCacheList.end())
        {
            N_delete i->second;
            mCacheList.erase(i);
            mCacheMark |= _GPM_DIRTY;
        }
    }
    //------------------------------------------------------------------------
    void GpuProgramManager::exportCache(DataStream * stream) const
    {
        if (mCacheMark & _GPM_DIRTY)
        {
            if(!stream->isWriteable())
            {
                N_EXCEPT(IO, _I18n("Unable to write to stream ") + stream->getName());
            }

            // write the size of the array
            NCount sizeOfArray = mCacheList.size();
            stream->write(&sizeOfArray, sizeof(NCount));

            // loop the array and save it
            CacheList::const_iterator iter = mCacheList.begin();
            CacheList::const_iterator iterE = mCacheList.end();
            for( ; iter != iterE ; iter++)
            {
                // saves the name of the shader
                {
                    stream->write(&iter->first, sizeof(Nui32));
                }
                // saves the microcode
                {
                    const MemStream * microcodeOfShader = iter->second;
                    NCount size = microcodeOfShader->size();
                    stream->write(&size, sizeof(NCount));
                    stream->write(microcodeOfShader->getData(), size);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void GpuProgramManager::inportCache(const DataStream * stream)
    {
        mCacheList.clear();

        // write the size of the array
        NCount sizeOfArray = 0;
        stream->read(&sizeOfArray, sizeof(NCount));

        // loop the array and load it
        for(NCount i = 0 ; i < sizeOfArray ; i++)
        {
            Nui32 cid;
            stream->read(&cid, sizeof(Nui32));

            // loads the microcode
            NCount size = 0;
            stream->read(&size, sizeof(NCount));

            MemStream * microcodeOfShader(N_new MemStream(size, N_conv(cid)));
            microcodeOfShader->seek(0);
            stream->read(microcodeOfShader->getData(), size);

            mCacheList.insert(Npair(cid, microcodeOfShader));
        }
        mCacheMark &= ~_GPM_DIRTY;
    }
    //------------------------------------------------------------------------
    const GpuParamUnit * GpuProgramManager::getSyncParam(const String & name)
    {
        mSyncParamList::iterator i = mSyncParamList.find(name);
        if(i != mSyncParamList.end())
        {
            return i->second;
        }

        return 0;
    }
    //------------------------------------------------------------------------
    const GpuParamUnit * GpuProgramManager::getSyncParam(Nidx idx, String * outName)
    {
        mSyncParamList::iterator i = mSyncParamList.begin();
        std::advance(i, idx);

        if(outName)
            *outName = i->first;
        return i->second;
    }
    //------------------------------------------------------------------------
    const char * GpuProgramManager::getToken(PixelFormat pf) const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    const char * GpuProgramManager::getToken(PixelFormat pf, Texture::Type type, bool msaa, Buffer::OpType access) const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    static const char * gpm_cmpStrings[CPM_ALWAYS_PASS + 1] =
    {
        "==",   //CPM_NONE (dummy)
        "==",   //CPM_ALWAYS_FAIL (dummy)
        "<",    //CPM_LESS
        "<=",   //CPM_LESS_EQUAL
        "==",   //CPM_EQUAL
        "!=",   //CPM_NOT_EQUAL
        ">",    //CPM_GREATER
        ">=",   //CPM_GREATER_EQUAL
        "=="    //CPM_ALWAYS_PASS (dummy)
    };
    //-----------------------------------------------------------------------------------
    const char * GpuProgramManager::getCmpString(CmpMode mode)
    {
        return gpm_cmpStrings[mode];
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // NullProgram
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class NullProgram : public HighLevelGpuProgram
    {
    public:
        NullProgram(ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, LangID lid = N_PrimaryLang) :
                HighLevelGpuProgram(rid, gid, ls, rs, N_CmdObj_HighLevelGpuProgram, lid){}

        ~NullProgram(){}

        /// @copydetails GpuProgram::isValid
        bool isValid() const
        {
            return false;
        }

        /// @copydetails GpuProgram::getLanguage
        ShaderLanguage getLanguage() const
        {
            return SL_Unknow;
        }

        /// @copydetails PropertyCmdObj::set
        bool set(const String &, const String &)
        {
            return true;
        }
    protected:
        /** Internal load implementation, must be implemented by subclasses.
        */
        void loadCodeImpl(const VString & code)
        {
        }
        /// @copydetails HighLevelGpuProgram::genAssemblerImpl
        void genAssemblerImpl()
        {
        }
        /// Internal unload implementation, must be implemented by subclasses
        void unloadSysCodeImpl()
        {
        }
        /// @copydetails HighLevelGpuProgram::configParamImpl
        void configParamImpl(GpuProgramParam * params)
        {
            // Skip the normal implementation
            // Ensure we don't complain about missing parameter names
            params->setAllowMissing(true);

        }
        void genParamDefImpl()
        {
            // do nothing
        }
    };
    //------------------------------------------------------------------------
    class NullProgramFactory : public HighLevelGpuProgramFactory
    {
    public:
        NullProgramFactory(HighLevelGpuProgramManager * mag):
            HighLevelGpuProgramFactory(mag){}
        ~NullProgramFactory(){}

        FactoryID getID() const
        {
            return N_FACTORY_DummyProgram;
        }

        /// @copydetails HighLevelGpuProgramFactory::getLanguage
        ShaderLanguage getLanguage() const
        {
            return SL_Unknow;
        }

        HighLevelGpuProgram * create(ResourceID rid, GroupID gid)
        {
            return N_new NullProgram(rid, gid);
        }

        void destroy(void * prog)
        {
            NullProgram * temp = static_cast<NullProgram *>(prog);
            N_delete temp;
        }
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // HighLevelGpuProgramFactory
    //-----------------------------------------------------------------------
    //---------------------------------------------------------------------------
    HighLevelGpuProgramFactory::HighLevelGpuProgramFactory(HighLevelGpuProgramManager * mag):
        mManager(mag)
    {
    }
    //---------------------------------------------------------------------------
    HighLevelGpuProgramFactory::~HighLevelGpuProgramFactory() 
    {
    }
    //---------------------------------------------------------------------------
    Nmark HighLevelGpuProgramFactory::getVFSType() const
    {
        return VFST_Local;
    }
    //---------------------------------------------------------------------------
    //-----------------------------------------------------------------------
    /// HighLevelGpuProgramManager
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    template<> HighLevelGpuProgramManager * Singleton<HighLevelGpuProgramManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    HighLevelGpuProgramManager::HighLevelGpuProgramManager() :
        ResourceManager(RT_HighLevelGpuProgram)
    {
        // Loading order
        mScriptLevel = 50;
        N_Only(ResourceScheme).add(mType, this);

        add(N_new NullProgramFactory(this));
        add(N_new ProxyGpuProgramFactory(this));
    }
    //---------------------------------------------------------------------------
    HighLevelGpuProgramManager::~HighLevelGpuProgramManager()
    {
        FactoryMap::iterator i, iend = mFactList.end();

        i = mFactList.find(SL_Unknow);
        if(i != iend)
        {
            N_delete i->second;
            mFactList.erase(i);
        }

        i = mFactList.find(SL_ALTER);
        if(i != iend)
        {
            N_delete i->second;
            mFactList.erase(i);
        }

        N_Only(ResourceScheme).remove(mType);
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgramManager::add(HighLevelGpuProgramFactory * factory)
    {
        // deliberately allow later plugins to override earlier ones
        mFactList[factory->getLanguage()] = factory;
        factory->mManager = this;
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgramManager::remove(HighLevelGpuProgramFactory * factory)
    {
        // Remove only if equal to registered one, since it might overridden
        // by other plugins
        FactoryMap::iterator it = mFactList.find(factory->getLanguage());
        if (it != mFactList.end() && it->second == factory)
        {
            mFactList.erase(it);
        }
    }
    //---------------------------------------------------------------------------
    void HighLevelGpuProgramManager::init() 
    {
    }
    //---------------------------------------------------------------------------
    HighLevelGpuProgramFactory * HighLevelGpuProgramManager::getFactory(ShaderLanguage lang)
    {
        FactoryMap::iterator i = mFactList.find(lang);
        if(i == mFactList.end())
        {
            // use the null factory to create programs that will never be supported
            i = mFactList.find(SL_Unknow);
        }
        return i->second;
    }
    //---------------------------------------------------------------------
    bool HighLevelGpuProgramManager::isSupport(ShaderLanguage lang)
    {
        FactoryMap::iterator i = mFactList.find(lang);
        if(i == mFactList.end())
        {
            return N_Only(GpuProgram).isSupport(lang);
        }
        return true;
    }
    //---------------------------------------------------------------------------
    HighLevelGpuProgram * HighLevelGpuProgramManager::create(ResourceID rid,
        GroupID gid, ShaderLanguage lang, GpuProgram::ShaderType gptype)
    {
        if (rid == 0)
            rid = genValidID();
        HighLevelGpuProgram * prg = getFactory(lang)->create(rid, gid);
        prg->setType(gptype);
        prg->setSyntaxType(lang);

        addImpl(prg);
        // Tell resource group manager
        N_Only(ResourceScheme).onCreate(prg);
        return prg;
    }
    //---------------------------------------------------------------------------
    Resource * HighLevelGpuProgramManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
        String lang;

        if(!params || (lang = params->get(N_PropertyGpuProgram_Language)) == N_StrBlank)
        {
            N_EXCEPT(InvalidParam, _I18n("You must supply a 'language' parameter"));
        }
        ShaderLanguage shaderlang;
        StrConv::conv(lang, shaderlang);
        Resource * re = getFactory(shaderlang)->create(rid, gid);
        if(re)
        {
            re->setLoadScheme(ls);
            re->setResultScheme(rs);
        }
        return re;
    }
    //-----------------------------------------------------------------------
}