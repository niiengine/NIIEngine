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

#include "OgreShaderParameter.h"
#include "OgreStringConverter.h"
#include "OgreMatrix4.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreVector4.h"
#include "OgreShaderGenerator.h"

namespace Ogre {
namespace RTShader {

	//-----------------------------------------------------------------------
	// Define some ConstParameterTypes
	//-----------------------------------------------------------------------

	/** ConstParameterVec2 represents a Vector2 constant.
	*/
	class ConstParameterVec2 : public ConstParameter<Vector2>
	{
	public:
		ConstParameterVec2(	Vector2 val,
			GpuDataType type,
			const Semantic& semantic,
			const Content& content)
			: ConstParameter<Vector2>(val, type, semantic, content)
		{
		}

		~ConstParameterVec2() {}

		/**
		@see Parameter::toString.
		*/
		virtual String toString () const
		{
			const String& lang = ShaderGenerator::getSingleton().getTargetLanguage();
			return
				((lang.empty() == false) && (lang[0] == 'g') ? String("vec2(") : String("float2(")) +
				Ogre::StrConv::conv(mValue.x) + "," +
				Ogre::StrConv::conv(mValue.y) + ")";
		}
	};

	/** ConstParameterVec3 represents a Vector3 constant.
	*/
	class ConstParameterVec3 : public ConstParameter<Vector3>
	{
	public:
		ConstParameterVec3(	Vector3 val,
			GpuDataType type,
			const Semantic& semantic,
			const Content& content)
			: ConstParameter<Vector3>(val, type, semantic, content)
		{
		}
		~ConstParameterVec3() {}

		/**
		@see Parameter::toString.
		*/
		virtual String toString () const
		{
			const String& lang = ShaderGenerator::getSingleton().getTargetLanguage();
			return
				((lang.empty() == false) && (lang[0] == 'g') ? String("vec3(") : String("float3(")) +
				Ogre::StrConv::conv(mValue.x) + "," +
				Ogre::StrConv::conv(mValue.y) + "," +
				Ogre::StrConv::conv(mValue.z) + ")";
		}
	};

	/** ConstParameterVec4 represents a Vector2 Vector4.
	*/
	class ConstParameterVec4 : public ConstParameter<Vector4>
	{
	public:
		ConstParameterVec4(	Vector4 val,
			GpuDataType type,
			const Semantic& semantic,
			const Content& content)
			: ConstParameter<Vector4>(val, type, semantic, content)
		{
		}
		~ConstParameterVec4() {}

		/**
		@see Parameter::toString.
		*/
		virtual String toString () const
		{
			const String& lang = ShaderGenerator::getSingleton().getTargetLanguage();
			return
				((lang.empty() == false) && (lang[0] == 'g') ? String("vec4(") : String("float4(")) +
				Ogre::StrConv::conv(mValue.x) + "," +
				Ogre::StrConv::conv(mValue.y) + "," +
				Ogre::StrConv::conv(mValue.z) + "," +
				Ogre::StrConv::conv(mValue.w) + ")";
		}
	};

	/** ConstParameterFloat represents a float constant.
	*/
	class ConstParameterFloat : public ConstParameter<float>
	{
	public:
		ConstParameterFloat(float val,
			GpuDataType type,
			const Semantic& semantic,
			const Content& content)
			: ConstParameter<float>(val, type, semantic, content)
		{
		}

		~ConstParameterFloat() {}

		/**
		@see Parameter::toString.
		*/
		virtual String toString () const
		{
			String val = Ogre::StrConv::conv(mValue);

			// Make sure that float params have always this representation #.#
			if(val.find(".") == String::npos)
			{
				val += ".0";
			}

			return val;
		}
	};
	/** ConstParameterInt represents an int constant.
	*/
	class ConstParameterInt : public ConstParameter<int>
	{
	public:
		ConstParameterInt(int val,
			GpuDataType type,
			const Semantic& semantic,
			const Content& content)
			: ConstParameter<int>(val, type, semantic, content)
		{
		}

		~ConstParameterInt() {}

		/**
		@see Parameter::toString.
		*/
		virtual String toString () const
		{
			return Ogre::StrConv::conv(mValue);
		}
	};

	//-----------------------------------------------------------------------

	struct AutoShaderParameter
	{
		GpuSyncParam autoType;
		String name;
		GpuDataType type;

		AutoShaderParameter(GpuSyncParam _autoType, const String& _name, GpuDataType _type)
		{
			autoType = _autoType;
			name	 = _name;
			type	 = _type;
		}
	};

	AutoShaderParameter g_AutoParameters[] = 
    {
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Space,                  "world_matrix",							GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Space_Inverse,          "inverse_world_matrix",					GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Space_Transpose,        "transpose_world_matrix",           	GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Space_InverseTranspose, "inverse_transpose_world_matrix",		GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix3x4_Space_Array,		    "world_matrix_array_3x4",				GDT_Matrix3X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix4x4_Space_Array,		    "world_matrix_array",					GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_DualQuaternion_Space,          "world_dualquaternion_array_2x4",		GDT_Matrix2X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix3x4_Space_Scale_Shear,	"world_scale_shear_matrix_array_3x4",	GDT_Matrix3X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_View,				    "view_matrix",				            GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_View_Inverse,		    "inverse_view_matrix",		            GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_View_Transpose,         "transpose_view_matrix",	            GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_View_InverseTranspose,  "inverse_transpose_view_matrix",		GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj,              		"projection_matrix",					GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_Inverse,      		"inverse_projection_matrix",			GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_Transpose,    		"transpose_projection_matrix",			GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_InverseTranspose,	"inverse_transpose_projection_matrix",	GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_X_View,		    "viewproj_matrix",		                GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_X_View_Inverse,	"inverse_viewproj_matrix",				GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_X_View_Transpose,				"transpose_viewproj_matrix",			GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_X_View_InverseTranspose,		"inverse_transpose_viewproj_matrix",	GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_View_X_Space,              			"worldview_matrix",						GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_View_X_Space_Inverse,      			"inverse_worldview_matrix",				GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_View_X_Space_Transpose,				"transpose_worldview_matrix",			GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_View_X_Space_InverseTranspose,		"inverse_transpose_worldview_matrix",	GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_X_View_X_Space,			    "worldviewproj_matrix",					GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_X_View_X_Space_Inverse,	    "inverse_worldviewproj_matrix",			GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_X_View_X_Space_Transpose,	    "transpose_worldviewproj_matrix",		GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Matrix_Proj_X_View_X_Space_InverseTranspose, "inverse_transpose_worldviewproj_matrix", GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Texture_Flipping,			            "render_target_flipping",		    GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Vertex_Winding,					    "vertex_winding",					GDT_Float),
		//AutoShaderParameter(GpuProgramParam::GSP_Fog_Colour,						    "fog_colour",				        GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Fog_Params,						    "fog_params",				        GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Ambient,         		            "surface_ambient_colour",		    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Diffuse,         		            "surface_diffuse_colour",		    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Specular,        		            "surface_specular_colour",		    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Emissive,        		            "surface_emissive_colour",		    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Shininess,              		    "surface_shininess",			    GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Light_ActiveCount,                     "light_count",                      GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Ambient,          			            "ambient_light_colour",             GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Diffuse_ParamIndex,          	"light_diffuse_colour",         	GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Specular_ParamIndex,         	"light_specular_colour",            GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Attenuation_ParamIndex,          "light_attenuation",            	GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Spot_Params_ParamIndex,          "spotlight_params",                 GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Pos_ParamIndex,                  "light_position",                   GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Pos_ChSpace_ParamIndex,   		"light_position_object_space",      GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Pos_ChView_ParamIndex,		    "light_position_view_space",        GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Dir_ParamIndex,			        "light_direction",                  GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Dir_ChSpace_ParamIndex,			"light_direction_object_space",     GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Dir_ChView_ParamIndex,		    "light_direction_view_space",       GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Dis_ChSpace_ParamIndex,			"light_distance_object_space",      GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Light_BrightFactor_ParamIndex,   		"light_power",					    GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Diffuse_ParamCount,		        "light_diffuse_colour_array",       GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Specular_ParamCount,			    "light_specular_colour_array",	    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Attenuation_ParamCount,			"light_attenuation_array",          GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Pos_ParamCount,					"light_position_array",             GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Pos_ChSpace_ParamCount,		    "light_position_object_space_array",GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Pos_ChView_ParamCount,           "light_position_view_space_array",  GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Dir_ParamCount,					"light_direction_array",            GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Dir_ChSpace_ParamCount,		    "light_direction_object_space_array", 		GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Dir_ChView_ParamCount,           "light_direction_view_space_array",   		GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Dis_ChSpace_ParamCount,		    "light_distance_object_space_array",		GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Light_BrightFactor_ParamCount,   		"light_power_array",						GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Spot_Params_ParamCount,			"spotlight_params_array",					GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Ambient_X_Env,    		    "derived_ambient_light_colour",     		GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Emissive_Evn,            		    "derived_scene_colour",             		GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Diffuse_Ch_ParamIndex,    		"derived_light_diffuse_colour",     		GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Specular_Ch_ParamIndex,   		"derived_light_specular_colour",    		GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Diffuse_Ch_ParamCount,           "derived_light_diffuse_colour_array",   	GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Specular_Ch_ParamCount,	        "derived_light_specular_colour_array",  	GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Index_ParamIndex,                "light_number",                             GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Light_CastShadow_ParamIndex, 			"light_casts_shadows",						GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Shadow_VolExtent,     	                "shadow_extrusion_distance",				GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_Pos,               			    "camera_position",							GDT_Float3X),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_Pos_ChSpace,  	                "camera_position_object_space",				GDT_Float3X),
		AutoShaderParameter(GpuProgramParam::GSP_Texture_Matrix_Proj_X_View_ParamIndex, "texture_viewproj_matrix",					GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Texture_Matrix_Proj_X_View_ParamCount, "texture_viewproj_matrix_array",			GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Texture_Matrix_Proj_X_View_X_Space_ParamIndex, "texture_worldviewproj_matrix",     GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Texture_Matrix_Proj_X_View_X_Space_ParamCount, "texture_worldviewproj_matrix_array",		GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Spot_Matrix_Proj_X_View_ParamIndex,	"spotlight_viewproj_matrix",		    GDT_Matrix4X4),
        AutoShaderParameter(GpuProgramParam::GSP_Light_Spot_Matrix_Proj_X_View_ParamCount,  "spotlight_viewproj_matrix_array",      GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Light_Spot_Matrix_Proj_X_View_X_Space,	    "spotlight_worldviewproj_matrix",       GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Custom,							    "custom",                                   GDT_Float4X),  // *** needs to be tested
		AutoShaderParameter(GpuProgramParam::GSP_Sys_CostTime,						    "time",					        GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Sys_CostTime_Fmod_ParamData,           "time_0_x",                     GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Frame_CostTime,                        "frame_time",                   GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_FPS,                           	    "fps",                          GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_View_PixelWidth,                	    "viewport_width",               GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_View_PixelHeight,               	    "viewport_height",              GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_View_PixelSize,                 	    "viewport_size",                GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_Dir,                			"view_direction",               GDT_Float3X),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_Right,              			"view_side_vector",             GDT_Float3X),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_Up,                			    "view_up_vector",               GDT_Float3X),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_FOV,                            "fov",                          GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_NearClip,						"near_clip_distance",           GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_FarClip,						"far_clip_distance",            GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Index,							    "pass_number",                  GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_RenderCount,					    "pass_iteration_number",        GDT_Float),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_AniParam,					        "animation_parametric",         GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Texel_Oft,               			    "texel_offsets",			    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Ch_Depth_Range,           			    "scene_depth_range",		    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Shadow_ChDepthRange_ParamIndex,    	"shadow_scene_depth_range",	    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Shadow_Colour,							"shadow_colour",			    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Texture_Dim_ParamIndex,                "texture_size",					GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Texture_InverseDim_ParamIndex,       	"inverse_texture_size",			GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Texture_DimUnit_ParamIndex,            "packed_texture_size",		    GDT_Float4X),
		AutoShaderParameter(GpuProgramParam::GSP_Texture_Matrix_Transform_ParamIndex,	"texture_matrix",			    GDT_Matrix4X4),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_LOD_Pos,					    "lod_camera_position",          GDT_Float3X),
		AutoShaderParameter(GpuProgramParam::GSP_Camera_LOD_Pos_ChSpace,		        "lod_camera_position_object_space", GDT_Float3X),
        AutoShaderParameter(GpuProgramParam::GSP_Light_Custom,                          "light_custom",                 GDT_Float)
	};

//-----------------------------------------------------------------------
Parameter::Parameter(GpuDataType type, const String & name,
    const Semantic & semantic, int index, const Content & content, size_t size)
{
	mName					= name;
	mType					= type;
	mSemantic				= semantic;
	mIndex					= index;
	mContent				= content;
	mSize					= size;
}

//-----------------------------------------------------------------------
bool UniformParameter::isFloat() const
{
	switch(getType())
	{
	case GDT_Int:
	case GDT_Int2X:
	case GDT_Int3X:
	case GDT_Int4X:
	case GDT_Sampler1D:
	case GDT_Sampler2D:
    case GDT_SamplerArray2D:
	case GDT_Sampler3D:
	case GDT_SamplerCube:
	case GDT_SamplerShadow1D:
	case GDT_SamplerShadow2D:
		return false;
	default:
		return true;
	};
}

//-----------------------------------------------------------------------
bool UniformParameter::isSampler() const
{
	switch(getType())
	{
	case GDT_Sampler1D:
	case GDT_Sampler2D:
    case GDT_SamplerArray2D:
	case GDT_Sampler3D:
	case GDT_SamplerCube:
	case GDT_SamplerShadow1D:
	case GDT_SamplerShadow2D:
		return true;
	default:
		return false;
	};
}
//-----------------------------------------------------------------------
UniformParameter::UniformParameter(GpuDataType type, const String & name,
    const Semantic & semantic, int index, const Content & content,
        Nmark typemark, size_t size) :
            Parameter(type, name, semantic, index, content, size)
{
	mIsAutoConstantReal	 = false;
	mIsAutoConstantInt	 = false;
	mAutoConstantIntData = 0;
	mVariability		 = typemark;
	mParamsPtr			 = NULL;
	mPhysicalIndex		 = -1;
}
//-----------------------------------------------------------------------
UniformParameter::UniformParameter(GpuSyncParam autoType,
    NIIf fAutoConstantData, size_t size)
{
	AutoShaderParameter* parameterDef = &g_AutoParameters[autoType];

	mName				= parameterDef->name;
	if (fAutoConstantData != 0.0)
	{
		mName += StrConv::conv(fAutoConstantData);
		//replace possible illegal point character in name
		std::replace(mName.begin(), mName.end(), '.', '_');
	}
	mType				= parameterDef->type;
	mSemantic			= SPS_UNKNOWN;
	mIndex				= -1;
	mContent			= SPC_UNKNOWN;
	mIsAutoConstantReal	= true;
	mIsAutoConstantInt	= false;
	mAutoConstantType	= autoType;
	mAutoConstantRealData = fAutoConstantData;
	mVariability		= GPT_Render;
	mParamsPtr			 = NULL;
	mPhysicalIndex		 = -1;
	mSize				 = size;
}

//-----------------------------------------------------------------------
UniformParameter::UniformParameter(GpuSyncParam autoType,
    NIIf fAutoConstantData, size_t size, GpuDataType type)
{
	AutoShaderParameter* parameterDef = &g_AutoParameters[autoType];

	mName				= parameterDef->name;
	if (fAutoConstantData != 0.0)
	{
		mName += StrConv::conv(fAutoConstantData);
		//replace possible illegal point character in name
		std::replace(mName.begin(), mName.end(), '.', '_');
	}
	mType				= type;
	mSemantic			= SPS_UNKNOWN;
	mIndex				= -1;
	mContent			= SPC_UNKNOWN;
	mIsAutoConstantReal	= true;
	mIsAutoConstantInt	= false;
	mAutoConstantType	= autoType;
	mAutoConstantRealData = fAutoConstantData;
	mVariability		= GPT_Render;
	mParamsPtr			 = NULL;
	mPhysicalIndex		 = -1;
	mSize				 = size;
}

//-----------------------------------------------------------------------
UniformParameter::UniformParameter(GpuSyncParam autoType,
    size_t nAutoConstantData, size_t size)
{
	AutoShaderParameter * parameterDef = &g_AutoParameters[autoType];

	mName = parameterDef->name;
	if(nAutoConstantData != 0)
		mName += StrConv::conv(nAutoConstantData);
	mType = parameterDef->type;
	mSemantic = SPS_UNKNOWN;
	mIndex = -1;
	mContent = SPC_UNKNOWN;
	mIsAutoConstantReal	= false;
	mIsAutoConstantInt = true;
	mAutoConstantType = autoType;
	mAutoConstantIntData = nAutoConstantData;
	mVariability = GPT_Render;
	mParamsPtr = NULL;
	mPhysicalIndex = -1;
	mSize = size;
}

//-----------------------------------------------------------------------
UniformParameter::UniformParameter(GpuSyncParam autoType,
    size_t nAutoConstantData, size_t size, GpuDataType type)
{
	AutoShaderParameter * parameterDef = &g_AutoParameters[autoType];

	mName				= parameterDef->name;
	if (nAutoConstantData != 0)
		mName += StrConv::conv(nAutoConstantData);
	mType				= type;
	mSemantic			= SPS_UNKNOWN;
	mIndex				= -1;
	mContent			= SPC_UNKNOWN;
	mIsAutoConstantReal	= false;
	mIsAutoConstantInt	= true;
	mAutoConstantType	= autoType;
	mAutoConstantIntData = nAutoConstantData;
	mVariability		= GPT_Render;
	mParamsPtr			 = NULL;
	mPhysicalIndex		 = -1;
	mSize				 = size;
}

//-----------------------------------------------------------------------
void UniformParameter::bind(GpuProgramParam * paramsPtr)
{
	if(paramsPtr != 0)
	{
        if(paramsPtr->getDefine() != 0)
        {
            const GpuParamUnit * def = paramsPtr->getDefine()->get(mName);

            if(def != NULL)
            {
                mParamsPtr = paramsPtr;
                mPhysicalIndex = def->mMemIndex;
            }
        }
	}
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInPosition(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float4X, "iPos_" + StrConv::conv(index),
		Parameter::SPS_POSITION, index, Parameter::SPC_POSITION_OBJECT_SPACE));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutPosition(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float4X, "oPos_" + StrConv::conv(index),
		Parameter::SPS_POSITION, index, Parameter::SPC_POSITION_PROJECTIVE_SPACE));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInNormal(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float3X, "iNormal_" + StrConv::conv(index),
		Parameter::SPS_NORMAL, index, Parameter::SPC_NORMAL_OBJECT_SPACE));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInWeights(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float4X, "iBlendWeights_" + StrConv::conv(index),
		Parameter::SPS_BLEND_WEIGHTS, index, Parameter::SPC_BLEND_WEIGHTS));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInIndices(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float4X, "iBlendIndices_" + StrConv::conv(index),
		Parameter::SPS_BLEND_INDICES, index, Parameter::SPC_BLEND_INDICES));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInBiNormal(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float3X, "iBiNormal_" + StrConv::conv(index),
		Parameter::SPS_BINORMAL, index, Parameter::SPC_BINORMAL_OBJECT_SPACE));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInTangent(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float3X, "iTangent_" + StrConv::conv(index),
		Parameter::SPS_TANGENT, index,
		Parameter::SPC_TANGENT_OBJECT_SPACE));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutNormal(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float3X, "oNormal_" + StrConv::conv(index),
		Parameter::SPS_NORMAL, index, Parameter::SPC_NORMAL_OBJECT_SPACE));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutBiNormal(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float3X, "oBiNormal_" + StrConv::conv(index),
		Parameter::SPS_BINORMAL, index, Parameter::SPC_BINORMAL_OBJECT_SPACE));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutTangent(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float3X, "oTangent_" + StrConv::conv(index),
		Parameter::SPS_TANGENT, index, Parameter::SPC_TANGENT_OBJECT_SPACE));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInColor(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float4X, "iColor_" + StrConv::conv(index),
		Parameter::SPS_COLOR, index,
		index == 0 ? Parameter::SPC_COLOR_DIFFUSE : Parameter::SPC_COLOR_SPECULAR));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutColor(int index)
{
	return ParameterPtr(N_new Parameter(GDT_Float4X, "oColor_" + StrConv::conv(index),
		Parameter::SPS_COLOR, index,
		index == 0 ? Parameter::SPC_COLOR_DIFFUSE : Parameter::SPC_COLOR_SPECULAR));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInTexcoord(GpuDataType type, int index, Parameter::Content content)
{
	switch (type)
	{
	case GDT_Float:
		return createInTexcoord1(index, content);

	case GDT_Float2X:
		return createInTexcoord2(index, content);

	case GDT_Float3X:
		return createInTexcoord3(index, content);

	case GDT_Float4X:
		return createInTexcoord4(index, content);
    default:
	case GDT_Sampler1D:
    case GDT_Sampler2D:
    case GDT_SamplerArray2D:
    case GDT_Sampler3D:
    case GDT_SamplerCube:
    case GDT_SamplerShadow1D:
    case GDT_SamplerShadow2D:
    case GDT_Matrix2X2:
    case GDT_Matrix2X3:
    case GDT_Matrix2X4:
    case GDT_Matrix3X2:
    case GDT_Matrix3X3:
    case GDT_Matrix3X4:
    case GDT_Matrix4X2:
    case GDT_Matrix4X3:
    case GDT_Matrix4X4:
    case GDT_Int:
    case GDT_Int2X:
    case GDT_Int3X:
    case GDT_Int4X:
    case GDT_Unknow:
        break;
	}

	return ParameterPtr();
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutTexcoord(GpuDataType type, int index, Parameter::Content content)
{
	switch (type)
	{
	case GDT_Float:
		return createOutTexcoord1(index, content);

	case GDT_Float2X:
		return createOutTexcoord2(index, content);

	case GDT_Float3X:
		return createOutTexcoord3(index, content);

	case GDT_Float4X:
		return createOutTexcoord4(index, content);

    default:
	case GDT_Sampler1D:
    case GDT_Sampler2D:
    case GDT_SamplerArray2D:
    case GDT_Sampler3D:
    case GDT_SamplerCube:
    case GDT_SamplerShadow1D:
    case GDT_SamplerShadow2D:
    case GDT_Matrix2X2:
    case GDT_Matrix2X3:
    case GDT_Matrix2X4:
    case GDT_Matrix3X2:
    case GDT_Matrix3X3:
    case GDT_Matrix3X4:
    case GDT_Matrix4X2:
    case GDT_Matrix4X3:
    case GDT_Matrix4X4:
    case GDT_Int:
    case GDT_Int2X:
    case GDT_Int3X:
    case GDT_Int4X:
    case GDT_Unknow:
        break;
	}

	return ParameterPtr();
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInTexcoord1(int index, Parameter::Content content)
{
	return ParameterPtr(N_new Parameter(GDT_Float, "iTexcoord1_" + StrConv::conv(index),
		Parameter::SPS_TEXTURE_COORDINATES, index, content));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutTexcoord1(int index, Parameter::Content content)
{
	return ParameterPtr(N_new Parameter(GDT_Float, "oTexcoord1_" + StrConv::conv(index),
		Parameter::SPS_TEXTURE_COORDINATES, index, content));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInTexcoord2(int index, Parameter::Content content)
{
	return ParameterPtr(N_new Parameter(GDT_Float2X, "iTexcoord2_" + StrConv::conv(index),
		Parameter::SPS_TEXTURE_COORDINATES, index,
		content));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutTexcoord2(int index, Parameter::Content content)
{
	return ParameterPtr(N_new Parameter(GDT_Float2X, "oTexcoord2_" + StrConv::conv(index),
		Parameter::SPS_TEXTURE_COORDINATES, index,
		content));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInTexcoord3(int index, Parameter::Content content)
{
	return ParameterPtr(N_new Parameter(GDT_Float3X, "iTexcoord3_" + StrConv::conv(index),
		Parameter::SPS_TEXTURE_COORDINATES, index,
		content));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutTexcoord3(int index, Parameter::Content content)
{
	return ParameterPtr(N_new Parameter(GDT_Float3X, "oTexcoord3_" + StrConv::conv(index),
		Parameter::SPS_TEXTURE_COORDINATES, index,
		content));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createInTexcoord4(int index, Parameter::Content content)
{
	return ParameterPtr(N_new Parameter(GDT_Float4X, "iTexcoord4_" + StrConv::conv(index),
		Parameter::SPS_TEXTURE_COORDINATES, index,
		content));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createOutTexcoord4(int index, Parameter::Content content)
{
	return ParameterPtr(N_new Parameter(GDT_Float4X, "oTexcoord4_" + StrConv::conv(index),
		Parameter::SPS_TEXTURE_COORDINATES, index,
		content));
}

//-----------------------------------------------------------------------
UniformParameterPtr ParameterFactory::createSampler(GpuDataType type, int index)
{
	switch (type)
	{
	case GDT_Sampler1D:
		return createSampler1D(index);

	case GDT_Sampler2D:
		return createSampler2D(index);

    case GDT_SamplerArray2D:
        return createSampler2DArray(index);

	case GDT_Sampler3D:
		return createSampler3D(index);

	case GDT_SamplerCube:
		return createSamplerCUBE(index);

    default:
    case GDT_SamplerShadow1D:
    case GDT_SamplerShadow2D:
    case GDT_Matrix2X2:
    case GDT_Matrix2X3:
    case GDT_Matrix2X4:
    case GDT_Matrix3X2:
    case GDT_Matrix3X3:
    case GDT_Matrix3X4:
    case GDT_Matrix4X2:
    case GDT_Matrix4X3:
    case GDT_Matrix4X4:
    case GDT_Int:
    case GDT_Int2X:
    case GDT_Int3X:
    case GDT_Int4X:
    case GDT_Unknow:
        break;
	}

	return UniformParameterPtr();

}

//-----------------------------------------------------------------------
UniformParameterPtr ParameterFactory::createSampler1D(int index)
{
	return UniformParameterPtr(N_new UniformParameter(GDT_Sampler1D, "gSampler1D_" + StrConv::conv(index),
		Parameter::SPS_UNKNOWN, index,
		Parameter::SPC_UNKNOWN, GPT_Render, 1));
}

//-----------------------------------------------------------------------
UniformParameterPtr ParameterFactory::createSampler2D(int index)
{
	return UniformParameterPtr(N_new UniformParameter(GDT_Sampler2D, "gSampler2D_" + StrConv::conv(index),
		Parameter::SPS_UNKNOWN, index,
		Parameter::SPC_UNKNOWN,
		GPT_Render, 1));
}

//-----------------------------------------------------------------------
UniformParameterPtr ParameterFactory::createSampler2DArray(int index)
{
    return UniformParameterPtr(N_new UniformParameter(GDT_SamplerArray2D, "gSampler2DArray_" + StrConv::conv(index),
                                                         Parameter::SPS_UNKNOWN, index,
                                                         Parameter::SPC_UNKNOWN,
                                                         GPT_Render, 1));
}

//-----------------------------------------------------------------------
UniformParameterPtr ParameterFactory::createSampler3D(int index)
{
	return UniformParameterPtr(N_new UniformParameter(GDT_Sampler3D, "gSampler3D_" + StrConv::conv(index),
		Parameter::SPS_UNKNOWN, index,
		Parameter::SPC_UNKNOWN,
		GPT_Render, 1));
}

//-----------------------------------------------------------------------
UniformParameterPtr ParameterFactory::createSamplerCUBE(int index)
{
	return UniformParameterPtr(N_new UniformParameter(GDT_SamplerCube, "gSamplerCUBE_" + StrConv::conv(index),
		Parameter::SPS_UNKNOWN, index,
		Parameter::SPC_UNKNOWN,
		GPT_Render, 1));
}
//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createConstParamVector2(Vector2 val)
{
	return ParameterPtr((Parameter*)N_new ConstParameterVec2(val,
													GDT_Float2X,
													Parameter::SPS_UNKNOWN,
													Parameter::SPC_UNKNOWN));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createConstParamVector3(Vector3 val)
{
	return ParameterPtr((Parameter*)N_new ConstParameterVec3(val,
													GDT_Float3X,
													Parameter::SPS_UNKNOWN,
													Parameter::SPC_UNKNOWN));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createConstParamVector4(Vector4 val)
{
	return ParameterPtr((Parameter*)N_new ConstParameterVec4(val,
													GDT_Float4X,
													Parameter::SPS_UNKNOWN,
													Parameter::SPC_UNKNOWN));
}

//-----------------------------------------------------------------------
ParameterPtr ParameterFactory::createConstParamFloat(float val)
{
	return ParameterPtr((Parameter*)N_new ConstParameterFloat(val,
												  GDT_Float,
												  Parameter::SPS_UNKNOWN,
												  Parameter::SPC_UNKNOWN));
}

//-----------------------------------------------------------------------
UniformParameterPtr ParameterFactory::createUniform(GpuDataType type,
    int index, Nmark typemark, const String & suggestedName, size_t size)
{
	UniformParameterPtr param;

	param = UniformParameterPtr(N_new UniformParameter(type, suggestedName + StrConv::conv(index),
		Parameter::SPS_UNKNOWN, index, Parameter::SPC_UNKNOWN, typemark, size));

	return param;
}

}
}