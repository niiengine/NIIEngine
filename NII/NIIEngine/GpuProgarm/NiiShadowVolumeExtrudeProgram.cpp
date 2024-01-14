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
#include "NiiShadowVolumeExtrudeProgram.h"
#include "NiiGpuProgramManager.h"
#include "NiiLight.h"
#include "NiiException.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ShadowVolumeExtrudeProgram
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // c4 is the light position/direction in these
    VString ShadowVolumeExtrudeProgram::mPointArbvp1 =
        "!!ARBvp1.0\n"
        "PARAM c5 = { 0, 0, 0, 0 };\n"
        "TEMP R0;\n"
        "ATTRIB v24 = vertex.texcoord[0];\n"
        "ATTRIB v16 = vertex.position;\n"
        "PARAM c0[4] = { program.local[0..3] };\n"
        "PARAM c4 = program.local[4];\n"
        "ADD R0.xyz, v16.xyzx, -c4.xyzx;\n"
        "MOV R0.w, c5.x;\n"
        "MAD R0, v24.x, c4, R0;\n"
        "DP4 result.position.x, c0[0], R0;\n"
        "DP4 result.position.y, c0[1], R0;\n"
        "DP4 result.position.z, c0[2], R0;\n"
        "DP4 result.position.w, c0[3], R0;\n"
        "END\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_1_1 =
        "vs_1_1\n"
        "def c5, 0, 0, 0, 0\n"
        "dcl_texcoord0 v7\n"
        "dcl_position v0\n"
        "add r0.xyz, v0.xyz, -c4.xyz\n"
        "mov r0.w, c5.x\n"
        "mad r0, v7.x, c4, r0\n"
        "dp4 oPos.x, c0, r0\n"
        "dp4 oPos.y, c1, r0\n"
        "dp4 oPos.z, c2, r0\n"
        "dp4 oPos.w, c3, r0\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_4_0 =
        "// Point light shadow volume extrude\n"
        "struct VS_OUTPUT\n"
        "{\n"
        "\tfloat4 Pos : SV_POSITION;\n"
        "};\n"
        "VS_OUTPUT vs_main (\n"
        "    float4 position            : POSITION,\n"
        "    NIIf  wcoord            : TEXCOORD0,\n"
        "    uniform float4x4 worldviewproj_matrix,\n"
        "    uniform float4   light_position_object_space // homogeneous, object space\n"
        "    )\n"
        "{\n"
        "    // extrusion in object space\n"
        "    // vertex unmodified if w==1, extruded if w==0\n"
        "    float4 newpos = \n"
        "        (wcoord.xxxx * light_position_object_space) + \n"
        "        float4(position.xyz - light_position_object_space.xyz, 0);\n"
        "\n"
        "    VS_OUTPUT output = (VS_OUTPUT)0;\n"
        "    output.Pos = mul(worldviewproj_matrix, newpos);\n"
        "    return output;\n"
        "}\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_glsles =
        "#version 100\n"
        "precision highp NIIf;\n"
        "precision highp NIIi;\n"
        "precision lowp sampler2D;\n"
        "precision lowp samplerCube;\n\n"
        "// Point light shadow volume extrude\n"
        "attribute vec4 uv0;\n"
        "attribute vec4 position;\n\n"
        "uniform mat4 worldviewproj_matrix;\n"
        "uniform vec4 light_position_object_space; // homogenous, object space\n\n"
        "void main()\n"
        "{\n"
        "    // Extrusion in object space\n"
        "    // Vertex unmodified if w==1, extruded if w==0\n"
        "    vec4 newpos = \n"
        "        (uv0.xxxx * light_position_object_space) + \n"
        "        vec4(position.xyz - light_position_object_space.xyz, 0.0);\n"
        "\n"
        "    gl_Position = worldviewproj_matrix * newpos;\n"
        "}\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirArbvp1 =
        "!!ARBvp1.0\n"
        "TEMP R0;\n"
        "ATTRIB v24 = vertex.texcoord[0];\n"
        "ATTRIB v16 = vertex.position;\n"
        "PARAM c0[4] = { program.local[0..3] };\n"
        "PARAM c4 = program.local[4];\n"
        "ADD R0, v16, c4;\n"
        "MAD R0, v24.x, R0, -c4;\n"
        "DP4 result.position.x, c0[0], R0;\n"
        "DP4 result.position.y, c0[1], R0;\n"
        "DP4 result.position.z, c0[2], R0;\n"
        "DP4 result.position.w, c0[3], R0;\n"
        "END\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_1_1 =
        "vs_1_1\n"
        "dcl_texcoord0 v7\n"
        "dcl_position v0\n"
        "add r0, v0, c4\n"
        "mad r0, v7.x, r0, -c4\n"
        "dp4 oPos.x, c0, r0\n"
        "dp4 oPos.y, c1, r0\n"
        "dp4 oPos.z, c2, r0\n"
        "dp4 oPos.w, c3, r0\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_4_0 =
        "// Directional light extrude\n"
        "struct VS_OUTPUT\n"
        "{\n"
        "\tfloat4 Pos : SV_POSITION;\n"
        "};\n"
        "VS_OUTPUT vs_main (\n"
        "    float4 position            : POSITION,\n"
        "    NIIf  wcoord            : TEXCOORD0,\n"
        "\n"
        "    uniform float4x4 worldviewproj_matrix,\n"
        "    uniform float4   light_position_object_space // homogenous, object space\n"
        "    )\n"
        "{\n"
        "    // extrusion in object space\n"
        "    // vertex unmodified if w==1, extruded if w==0\n"
        "    float4 newpos = \n"
        "        (wcoord.xxxx * (position + light_position_object_space)) - light_position_object_space;\n"
        "\n"
        "    VS_OUTPUT output = (VS_OUTPUT)0;\n"
        "    output.Pos = mul(worldviewproj_matrix, newpos);\n"
        "    return output;\n"
        "}\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_glsles =
        "#version 100\n"
        "precision highp NIIf;\n"
        "precision highp NIIi;\n"
        "precision lowp sampler2D;\n"
        "precision lowp samplerCube;\n\n"
        "// Directional light extrude\n"
        "attribute vec4 uv0;\n"
        "attribute vec4 position;\n\n"
        "uniform mat4 worldviewproj_matrix;\n"
        "uniform vec4 light_position_object_space; // homogenous, object space\n\n"
        "void main()\n"
        "{\n"
        "    // Extrusion in object space\n"
        "    // Vertex unmodified if w==1, extruded if w==0\n"
        "    vec4 newpos = \n"
        "        (uv0.xxxx * (position + light_position_object_space)) - light_position_object_space;\n"
        "\n"
        "    gl_Position = worldviewproj_matrix * newpos;\n"
        "}\n";

    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointArbvp1Debug =
        "!!ARBvp1.0\n"
        "PARAM c5 = { 0, 0, 0, 0 };\n"
        "PARAM c6 = { 1, 1, 1, 1 };\n"
        "TEMP R0;\n"
        "ATTRIB v24 = vertex.texcoord[0];\n"
        "ATTRIB v16 = vertex.position;\n"
        "PARAM c0[4] = { program.local[0..3] };\n"
        "PARAM c4 = program.local[4];\n"
        "ADD R0.xyz, v16.xyzx, -c4.xyzx;\n"
        "MOV R0.w, c5.x;\n"
        "MAD R0, v24.x, c4, R0;\n"
        "DP4 result.position.x, c0[0], R0;\n"
        "DP4 result.position.y, c0[1], R0;\n"
        "DP4 result.position.z, c0[2], R0;\n"
        "DP4 result.position.w, c0[3], R0;\n"
        "MOV result.color.front.primary, c6.x;\n"
        "END\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_1_1Debug =
        "vs_1_1\n"
        "def c5, 0, 0, 0, 0\n"
        "def c6, 1, 1, 1, 1\n"
        "dcl_texcoord0 v7\n"
        "dcl_position v0\n"
        "add r0.xyz, v0.xyz, -c4.xyz\n"
        "mov r0.w, c5.x\n"
        "mad r0, v7.x, c4, r0\n"
        "dp4 oPos.x, c0, r0\n"
        "dp4 oPos.y, c1, r0\n"
        "dp4 oPos.z, c2, r0\n"
        "dp4 oPos.w, c3, r0\n"
        "mov oD0, c6.x\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_4_0Debug = mPointVs_4_0;
    VString ShadowVolumeExtrudeProgram::mPointVs_glslesDebug = mPointVs_glsles;
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirArbvp1Debug =
        "!!ARBvp1.0\n"
        "PARAM c5 = { 1, 1, 1, 1};\n"
        "TEMP R0;\n"
        "ATTRIB v24 = vertex.texcoord[0];\n"
        "ATTRIB v16 = vertex.position;\n"
        "PARAM c0[4] = { program.local[0..3] };\n"
        "PARAM c4 = program.local[4];\n"
        "ADD R0, v16, c4;\n"
        "MAD R0, v24.x, R0, -c4;\n"
        "DP4 result.position.x, c0[0], R0;\n"
        "DP4 result.position.y, c0[1], R0;\n"
        "DP4 result.position.z, c0[2], R0;\n"
        "DP4 result.position.w, c0[3], R0;\n"
        "MOV result.color.front.primary, c5.x;"
        "END\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_1_1Debug =
        "vs_1_1\n"
        "def c5, 1, 1, 1, 1\n"
        "dcl_texcoord0 v7\n"
        "dcl_position v0\n"
        "add r0, v0, c4\n"
        "mad r0, v7.x, r0, -c4\n"
        "dp4 oPos.x, c0, r0\n"
        "dp4 oPos.y, c1, r0\n"
        "dp4 oPos.z, c2, r0\n"
        "dp4 oPos.w, c3, r0\n"
        "mov oD0, c5.x\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_4_0Debug = mDirVs_4_0;
    VString ShadowVolumeExtrudeProgram::mDirVs_glslesDebug = mDirVs_glsles;
    //--------------------------------------------------------------------

    // c4 is the light position/direction in these
    // c5 is extrusion distance
    VString ShadowVolumeExtrudeProgram::mPointArbvp1Finite =
        "!!ARBvp1.0\n"
        "PARAM c6 = { 1, 0, 0, 0 };\n"
        "TEMP R0, R1;\n"
        "ATTRIB v24 = vertex.texcoord[0];\n"
        "ATTRIB v16 = vertex.position;\n"
        "PARAM c0[4] = { program.local[0..3] };\n"
        "PARAM c5 = program.local[5];\n"
        "PARAM c4 = program.local[4];\n"
        "ADD R0.x, c6.x, -v24.x;\n"
        "MUL R0.w, R0.x, c5.x;\n"
        "ADD R0.xyz, v16.xyzx, -c4.xyzx;\n"
        "DP3 R1.w, R0.xyzx, R0.xyzx;\n"     // R1.w = Vector3(vertex - lightpos).sqrLength()
        "RSQ R1.w, R1.w;\n"                 // R1.w = 1 / Vector3(vertex - lightpos).length()
        "MUL R0.xyz, R1.w, R0.xyzx;\n"      // Vector3(vertex - lightpos).normalise(R0.xyz)
        "MAD R0.xyz, R0.w, R0.xyzx, v16.xyzx;\n"
        "DPH result.position.x, R0.xyzz, c0[0];\n"
        "DPH result.position.y, R0.xyzz, c0[1];\n"
        "DPH result.position.z, R0.xyzz, c0[2];\n"
        "DPH result.position.w, R0.xyzz, c0[3];\n"
        "END\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_1_1Finite =
        "vs_1_1\n"
        "def c6, 1, 0, 0, 0\n"
        "dcl_texcoord0 v7\n"
        "dcl_position v0\n"
        "add r0.x, c6.x, -v7.x\n"
        "mul r1.x, r0.x, c5.x\n"
        "add r0.yzw, v0.xxyz, -c4.xxyz\n"
        "dp3 r0.x, r0.yzw, r0.yzw\n"
        "rsq r0.x, r0.x\n"
        "mul r0.xyz, r0.x, r0.yzw\n"
        "mad r0.xyz, r1.x, r0.xyz, v0.xyz\n"
        "mov r0.w, c6.x\n"
        "dp4 oPos.x, c0, r0\n"
        "dp4 oPos.y, c1, r0\n"
        "dp4 oPos.z, c2, r0\n"
        "dp4 oPos.w, c3, r0\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_4_0Finite =
        "// Point light shadow volume extrude - FINITE\n"
        "struct VS_OUTPUT\n"
        "{\n"
        "\tfloat4 Pos : SV_POSITION;\n"
        "};\n"
        "VS_OUTPUT vs_main (\n"
        "    float4 position            : POSITION,\n"
        "    NIIf  wcoord            : TEXCOORD0,\n"
        "\n"
        "    uniform float4x4 worldviewproj_matrix,\n"
        "    uniform float4   light_position_object_space, // homogeneous, object space\n"
        "    uniform NIIf    shadow_extrusion_distance // how far to extrude\n"
        "    )\n"
        "{\n"
        "    // extrusion in object space\n"
        "    // vertex unmodified if w==1, extruded if w==0\n"
        "    float3 extrusionDir = position.xyz - light_position_object_space.xyz;\n"
        "    extrusionDir = normalize(extrusionDir);\n"
        "    \n"
        "    float4 newpos = float4(position.xyz +  \n"
        "        ((1 - wcoord.x) * shadow_extrusion_distance * extrusionDir), 1);\n"
        "\n"
        "    VS_OUTPUT output = (VS_OUTPUT)0;\n"
        "    output.Pos = mul(worldviewproj_matrix, newpos);\n"
        "    return output;\n"

        "\n"
        "}\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_glslesFinite =
        "#version 100\n"
        "precision highp NIIf;\n"
        "precision highp NIIi;\n"
        "precision lowp sampler2D;\n"
        "precision lowp samplerCube;\n\n"
        "// Point light shadow volume extrude - FINITE\n"
        "attribute vec4 uv0;\n"
        "attribute vec4 position;\n\n"
        "uniform mat4 worldviewproj_matrix;\n"
        "uniform vec4 light_position_object_space; // homogenous, object space\n"
        "uniform NIIf shadow_extrusion_distance; // how far to extrude\n\n"
        "void main()\n"
        "{\n"
        "    // Extrusion in object space\n"
        "    // Vertex unmodified if w==1, extruded if w==0\n"
        "    vec3 extrusionDir = position.xyz - light_position_object_space.xyz;\n"
        "    extrusionDir = normalize(extrusionDir);\n"
        "    \n"
        "    vec4 newpos = vec4(position.xyz +  \n"
        "        ((1.0 - uv0.x) * shadow_extrusion_distance * extrusionDir), 1.0);\n"
        "\n"
        "    gl_Position = worldviewproj_matrix * newpos;\n"
        "}\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirArbvp1Finite =
        "!!ARBvp1.0\n"
        "PARAM c6 = { 1, 0, 0, 0 };\n"
        "TEMP R0;\n"
        "ATTRIB v24 = vertex.texcoord[0];\n"
        "ATTRIB v16 = vertex.position;\n"
        "PARAM c0[4] = { program.local[0..3] };\n"
        "PARAM c4 = program.local[4];\n"
        "PARAM c5 = program.local[5];\n"
        "ADD R0.x, c6.x, -v24.x;\n"
        "MUL R0.x, R0.x, c5.x;\n"
        "MAD R0.xyz, -R0.x, c4.xyzx, v16.xyzx;\n"
        "DPH result.position.x, R0.xyzz, c0[0];\n"
        "DPH result.position.y, R0.xyzz, c0[1];\n"
        "DPH result.position.z, R0.xyzz, c0[2];\n"
        "DPH result.position.w, R0.xyzz, c0[3];\n"
        "END\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_1_1Finite =
        "vs_1_1\n"
        "def c6, 1, 0, 0, 0\n"
        "dcl_texcoord0 v7\n"
        "dcl_position v0\n"
        "add r0.x, c6.x, -v7.x\n"
        "mul r0.x, r0.x, c5.x\n"
        "mad r0.xyz, -r0.x, c4.xyz, v0.xyz\n"
        "mov r0.w, c6.x\n"
        "dp4 oPos.x, c0, r0\n"
        "dp4 oPos.y, c1, r0\n"
        "dp4 oPos.z, c2, r0\n"
        "dp4 oPos.w, c3, r0\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_4_0Finite =
        "// Directional light extrude - FINITE\n"
        "struct VS_OUTPUT\n"
        "{\n"
        "\tfloat4 Pos : SV_POSITION;\n"
        "};\n"
        "VS_OUTPUT vs_main (\n"
        "    float4 position            : POSITION,\n"
        "    NIIf  wcoord            : TEXCOORD0,\n"
        "\n"
        "    uniform float4x4 worldviewproj_matrix,\n"
        "    uniform float4   light_position_object_space, // homogeneous, object space\n"
        "    uniform NIIf    shadow_extrusion_distance // how far to extrude\n"
        "    )\n"
        "{\n"
        "    // extrusion in object space\n"
        "    // vertex unmodified if w==1, extruded if w==0\n"
        "    // -ve light_position_object_space is direction\n"
        "    float4 newpos = float4(position.xyz - \n"
        "        (wcoord.x * shadow_extrusion_distance * light_position_object_space.xyz), 1);\n"
        "\n"
        "    VS_OUTPUT output = (VS_OUTPUT)0;\n"
        "    output.Pos = mul(worldviewproj_matrix, newpos);\n"
        "    return output;\n"
        "\n"
        "}\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_glslesFinite =
        "#version 100\n"
        "precision highp NIIf;\n"
        "precision highp NIIi;\n"
        "precision lowp sampler2D;\n"
        "precision lowp samplerCube;\n\n"
        "// Directional light extrude - FINITE\n"
        "attribute vec4 uv0;\n"
        "attribute vec4 position;\n\n"
        "uniform mat4 worldviewproj_matrix;\n"
        "uniform vec4 light_position_object_space; // homogenous, object space\n"
        "uniform NIIf shadow_extrusion_distance;  // how far to extrude\n\n"
        "void main()\n"
        "{\n"
        "    // Extrusion in object space\n"
        "    // Vertex unmodified if w==1, extruded if w==0\n"
        "     // -ve light_position_object_space is direction\n"
        "    vec4 newpos = vec4(position.xyz - \n"
        "        (uv0.x * shadow_extrusion_distance * light_position_object_space.xyz), 1.0);\n"
        "\n"
        "    gl_Position = worldviewproj_matrix * newpos;\n"
        "\n"
        "}\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointArbvp1FiniteDebug =
        "!!ARBvp1.0\n"
        "PARAM c6 = { 1, 0, 0, 0 };\n"
        "TEMP R0, R1;\n"
        "ATTRIB v24 = vertex.texcoord[0];\n"
        "ATTRIB v16 = vertex.position;\n"
        "PARAM c0[4] = { program.local[0..3] };\n"
        "PARAM c5 = program.local[5];\n"
        "PARAM c4 = program.local[4];\n"
        "MOV result.color.front.primary, c6.x;\n"
        "ADD R0.x, c6.x, -v24.x;\n"
        "MUL R1.x, R0.x, c5.x;\n"
        "ADD R0.yzw, v16.xxyz, -c4.xxyz;\n"
        "DP3 R0.x, R0.yzwy, R0.yzwy;\n"
        "RSQ R0.x, R0.x;\n"
        "MUL R0.xyz, R0.x, R0.yzwy;\n"
        "MAD R0.xyz, R1.x, R0.xyzx, v16.xyzx;\n"
        "DPH result.position.x, R0.xyzz, c0[0];\n"
        "DPH result.position.y, R0.xyzz, c0[1];\n"
        "DPH result.position.z, R0.xyzz, c0[2];\n"
        "DPH result.position.w, R0.xyzz, c0[3];\n"
        "END\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_1_1FiniteDebug =
        "vs_1_1\n"
        "def c6, 1, 0, 0, 0\n"
        "dcl_texcoord0 v7\n"
        "dcl_position v0\n"
        "mov oD0, c6.x\n"
        "add r0.x, c6.x, -v7.x\n"
        "mul r1.x, r0.x, c5.x\n"
        "add r0.yzw, v0.xxyz, -c4.xxyz\n"
        "dp3 r0.x, r0.yzw, r0.yzw\n"
        "rsq r0.x, r0.x\n"
        "mul r0.xyz, r0.x, r0.yzw\n"
        "mad r0.xyz, r1.x, r0.xyz, v0.xyz\n"
        "mov r0.w, c6.x\n"
        "dp4 oPos.x, c0, r0\n"
        "dp4 oPos.y, c1, r0\n"
        "dp4 oPos.z, c2, r0\n"
        "dp4 oPos.w, c3, r0\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mPointVs_4_0FiniteDebug = mPointVs_4_0Finite;
    VString ShadowVolumeExtrudeProgram::mPointVs_glslesFiniteDebug = mPointVs_glslesFinite;
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirArbvp1FiniteDebug =
        "!!ARBvp1.0\n"
        "PARAM c6 = { 1, 0, 0, 0 };\n"
        "TEMP R0;\n"
        "ATTRIB v24 = vertex.texcoord[0];\n"
        "ATTRIB v16 = vertex.position;\n"
        "PARAM c0[4] = { program.local[0..3] };\n"
        "PARAM c4 = program.local[4];\n"
        "PARAM c5 = program.local[5];\n"
        "MOV result.color.front.primary, c6.x;\n"
        "ADD R0.x, c6.x, -v24.x;\n"
        "MUL R0.x, R0.x, c5.x;\n"
        "MAD R0.xyz, -R0.x, c4.xyzx, v16.xyzx;\n"
        "DPH result.position.x, R0.xyzz, c0[0];\n"
        "DPH result.position.y, R0.xyzz, c0[1];\n"
        "DPH result.position.z, R0.xyzz, c0[2];\n"
        "DPH result.position.w, R0.xyzz, c0[3];\n"
        "END\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_1_1FiniteDebug =
        "vs_1_1\n"
        "def c6, 1, 0, 0, 0\n"
        "dcl_texcoord0 v7\n"
        "dcl_position v0\n"
        "mov oD0, c6.x\n"
        "add r0.x, c6.x, -v7.x\n"
        "mul r0.x, r0.x, c5.x\n"
        "mad r0.xyz, -r0.x, c4.xyz, v0.xyz\n"
        "mov r0.w, c6.x\n"
        "dp4 oPos.x, c0, r0\n"
        "dp4 oPos.y, c1, r0\n"
        "dp4 oPos.z, c2, r0\n"
        "dp4 oPos.w, c3, r0\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mDirVs_4_0FiniteDebug = mDirVs_4_0Finite;
    VString ShadowVolumeExtrudeProgram::mDirVs_glslesFiniteDebug = mDirVs_glslesFinite;
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mGeneralFs_4_0 =
        "struct VS_OUTPUT\n"
        "{\n"
        "\tfloat4 Pos : SV_POSITION;\n"
        "};\n"
        "float4 fs_main (VS_OUTPUT input): SV_Target\n"
        "{\n"
        "    float4 finalColor = float4(1,1,1,1);\n"
        "    return finalColor;\n"
        "}\n";
    //--------------------------------------------------------------------
    VString ShadowVolumeExtrudeProgram::mGeneralFs_glsles =
        "#version 100\n"
        "precision highp NIIf;\n"
        "precision highp NIIi;\n"
        "precision lowp sampler2D;\n"
        "precision lowp samplerCube;\n\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}\n";
    //--------------------------------------------------------------------
    ResourceID ShadowVolumeExtrudeProgram::programNames[EXTRUDER_PROGRAM_COUNT] =
    {
        0,//"Ogre/ShadowExtrudePointLight",
        0,//"Ogre/ShadowExtrudePointLightDebug",
        0,//"Ogre/ShadowExtrudeDirLight",
        0,//"Ogre/ShadowExtrudeDirLightDebug",
        0,//"Ogre/ShadowExtrudePointLightFinite",
        0,//"Ogre/ShadowExtrudePointLightFiniteDebug",
        0,//"Ogre/ShadowExtrudeDirLightFinite",
        0,//"Ogre/ShadowExtrudeDirLightFiniteDebug"
    };
    //--------------------------------------------------------------------
    ResourceID ShadowVolumeExtrudeProgram::frgProgramName = 0;
    //--------------------------------------------------------------------
    bool ShadowVolumeExtrudeProgram::mInitialised = false;
    //---------------------------------------------------------------------
    void ShadowVolumeExtrudeProgram::init()
    {
        if(!mInitialised)
        {
            String syntax;
            ShaderLanguage syntaxl;
            bool vertexProgramFinite[EXTRUDER_PROGRAM_COUNT] =
            {
                false, false, false, false, true, true, true, true
            };
            bool vertexProgramDebug[EXTRUDER_PROGRAM_COUNT] =
            {
                false, true, false, true, false, true, false, true
            };
            LightType vertexProgramLightTypes[EXTRUDER_PROGRAM_COUNT] =
            {
                LT_POINT, LT_POINT, LT_DIRECTIONAL, LT_DIRECTIONAL,
                LT_POINT, LT_POINT, LT_DIRECTIONAL, LT_DIRECTIONAL
            };

            // load hardware extrusion programs for point & dir lights
            if(N_Only(GpuProgram).isShaderSupport(SL_ARBVP1))
            {
                // ARBvp1
                syntax = _T("arbvp1");
                syntaxl = SL_ARBVP1;
            }
            else if(N_Only(GpuProgram).isShaderSupport(SL_VS_1_1))
            {
                syntax = _T("vs_1_1");
                syntaxl = SL_VS_1_1;
            }
            else if(N_Only(GpuProgram).isShaderSupport(SL_VS_4_0))
            {
                syntax = _T("vs_4_0");
                syntaxl = SL_VS_4_0;
            }
            else if(N_Only(GpuProgram).isShaderSupport(SL_GLSLES))
            {
                syntax = _T("glsles");
                syntaxl = SL_GLSLES;
            }
            else
            {
                N_EXCEPT(Internal, _I18n("Vertex programs are supposedly supported, but neither ")
                    _I18n("arbvp1, glsles, vs_1_1 nor vs_4_0 syntaxes are present."));
            }
            // Create all programs
            for(NCount v = 0; v < EXTRUDER_PROGRAM_COUNT; ++v)
            {
                // Create debug extruders
                if(programNames[v] == 0 || !N_Only(GpuProgram).get(programNames[v]))
                {
                    if (syntax == _T("vs_4_0"))
                    {
                        HighLevelGpuProgram * vp = N_Only(HighLevelGpuProgram).create(
                            0, GroupInner, SL_HLSL, GpuProgram::ST_VS);

                        programNames[v] = vp->getOriginID();
                        vp->setProgramCode(ShadowVolumeExtrudeProgram::getProgramSource(
                            vertexProgramLightTypes[v], syntaxl, vertexProgramFinite[v], 
                            vertexProgramDebug[v]));

                        vp->set(N_PropertyGpuProgram_Language, syntax);
                        vp->set(N_PropertyGpuProgram_Main, "vs_main");
                        vp->load();

                        if(frgProgramName == 0)
                        {
                            //frgProgramName = "Ogre/ShadowFrgProgram";
                            HighLevelGpuProgram * fp = N_Only(HighLevelGpuProgram).create(
                                frgProgramName, GroupInner, SL_HLSL, GpuProgram::ST_FS);

                            fp->setProgramCode(mGeneralFs_4_0);
                            fp->set(N_PropertyGpuProgram_Language, _T("ps_4_0"));
                            fp->set(N_PropertyGpuProgram_Main, "fs_main");
                            fp->load();
                        }
                    }
                    else if (syntax == _T("glsles"))
                    {
                        HighLevelGpuProgram * vp = N_Only(HighLevelGpuProgram).create(
                            0, GroupInner, SL_GLSLES, GpuProgram::ST_VS);
                        vp->setProgramCode(ShadowVolumeExtrudeProgram::getProgramSource(
                            vertexProgramLightTypes[v], syntaxl,
                            vertexProgramFinite[v], vertexProgramDebug[v]));
                        vp->set(N_PropertyGpuProgram_Language, syntax);
                        vp->load();

                        if (frgProgramName == 0)
                        {
                            //frgProgramName = "Ogre/ShadowFrgProgram";
                            HighLevelGpuProgram * fp = N_Only(HighLevelGpuProgram).create(
                                frgProgramName, GroupInner, SL_GLSLES, GpuProgram::ST_FS);

                            fp->setProgramCode(mGeneralFs_glsles);
                            fp->set(N_PropertyGpuProgram_Language, _T("glsles"));
                            fp->load();
                        }
                    }
                    else
                    {
                        GpuProgram * vp = N_Only(GpuProgram).create(
                            0, GroupInner, GpuProgram::ST_VS, syntaxl,
                            ShadowVolumeExtrudeProgram::getProgramSource(
                            vertexProgramLightTypes[v], syntaxl,
                            vertexProgramFinite[v], vertexProgramDebug[v]));
                        vp->load();
                    }

                }
            }
            mInitialised = true;
        }
    }
    //---------------------------------------------------------------------
    void ShadowVolumeExtrudeProgram::shutdown(void)
    {
        if (mInitialised)
        {
            for (NCount v = 0; v < EXTRUDER_PROGRAM_COUNT; ++v)
            {
                // Destroy debug extruders
                N_Only(GpuProgram).remove(programNames[v]);
            }
            mInitialised = false;
        }
    }
    //---------------------------------------------------------------------
    const VString & ShadowVolumeExtrudeProgram::getProgramSource(
        LightType lightType, ShaderLanguage lang, bool finite, bool debug)
    {
        if(lightType == LT_DIRECTIONAL)
        {
            if (lang == SL_ARBVP1)
            {
                if (finite)
                {
                    if (debug)
                    {
                        return getDirectionalLightExtruderArbvp1FiniteDebug();
                    }
                    else
                    {
                        return getDirectionalLightExtruderArbvp1Finite();
                    }
                }
                else
                {
                    if (debug)
                    {
                        return getDirectionalLightExtruderArbvp1Debug();
                    }
                    else
                    {
                        return getDirectionalLightExtruderArbvp1();
                    }
                }
            }
            else if (lang == SL_VS_1_1)
            {
                if (finite)
                {
                    if (debug)
                    {
                        return getDirectionalLightExtruderVs_1_1FiniteDebug();
                    }
                    else
                    {
                        return getDirectionalLightExtruderVs_1_1Finite();
                    }
                }
                else
                {
                    if (debug)
                    {
                        return getDirectionalLightExtruderVs_1_1Debug();
                    }
                    else
                    {
                        return getDirectionalLightExtruderVs_1_1();
                    }
                }
            }
            else if (lang == SL_VS_4_0)
            {
                if (finite)
                {
                    if (debug)
                    {
                        return getDirectionalLightExtruderVs_4_0FiniteDebug();
                    }
                    else
                    {
                        return getDirectionalLightExtruderVs_4_0Finite();
                    }
                }
                else
                {
                    if (debug)
                    {
                        return getDirectionalLightExtruderVs_4_0Debug();
                    }
                    else
                    {
                        return getDirectionalLightExtruderVs_4_0();
                    }
                }
            }
            else if (lang == SL_GLSLES)
            {
                if (finite)
                {
                    if (debug)
                    {
                        return getDirectionalLightExtruderVs_glslesFiniteDebug();
                    }
                    else
                    {
                        return getDirectionalLightExtruderVs_glslesFinite();
                    }
                }
                else
                {
                    if (debug)
                    {
                        return getDirectionalLightExtruderVs_glslesDebug();
                    }
                    else
                    {
                        return getDirectionalLightExtruderVs_glsles();
                    }
                }
            }
            else
            {
                N_EXCEPT(Internal, _I18n("Vertex programs are supposedly supported, but neither ")
                    _I18n("arbvp1, glsles, vs_1_1 nor vs_4_0 syntaxes are present."));
            }

        }
        else
        {
            if (lang == SL_ARBVP1)
            {
                if (finite)
                {
                    if (debug)
                    {
                        return getPointLightExtruderArbvp1FiniteDebug();
                    }
                    else
                    {
                        return getPointLightExtruderArbvp1Finite();
                    }
                }
                else
                {
                    if (debug)
                    {
                        return getPointLightExtruderArbvp1Debug();
                    }
                    else
                    {
                        return getPointLightExtruderArbvp1();
                    }
                }
            }
            else if (lang == SL_VS_1_1)
            {
                if (finite)
                {
                    if (debug)
                    {
                        return getPointLightExtruderVs_1_1FiniteDebug();
                    }
                    else
                    {
                        return getPointLightExtruderVs_1_1Finite();
                    }
                }
                else
                {
                    if (debug)
                    {
                        return getPointLightExtruderVs_1_1Debug();
                    }
                    else
                    {
                        return getPointLightExtruderVs_1_1();
                    }
                }
            }
            else if (lang == SL_VS_4_0)
            {
                if (finite)
                {
                    if (debug)
                    {
                        return getPointLightExtruderVs_4_0FiniteDebug();
                    }
                    else
                    {
                        return getPointLightExtruderVs_4_0Finite();
                    }
                }
                else
                {
                    if (debug)
                    {
                        return getPointLightExtruderVs_4_0Debug();
                    }
                    else
                    {
                        return getPointLightExtruderVs_4_0();
                    }
                }
            }
            else if (lang == SL_GLSLES)
            {
                if (finite)
                {
                    if (debug)
                    {
                        return getPointLightExtruderVs_glslesFiniteDebug();
                    }
                    else
                    {
                        return getPointLightExtruderVs_glslesFinite();
                    }
                }
                else
                {
                    if (debug)
                    {
                        return getPointLightExtruderVs_glslesDebug();
                    }
                    else
                    {
                        return getPointLightExtruderVs_glsles();
                    }
                }
            }
            else
            {
                N_EXCEPT(Internal, _I18n("Vertex programs are supposedly supported, but neither ")
                    _I18n("arbvp1, glsles, vs_1_1 nor vs_4_0 syntaxes are present."));
            }

        }
    }
    //---------------------------------------------------------------------
    ResourceID ShadowVolumeExtrudeProgram::getProgramName(LightType lightType,
        bool finite, bool debug)
    {
        if (lightType == LT_DIRECTIONAL)
        {
            if (finite)
            {
                if (debug)
                {
                    return programNames[DIRECTIONAL_LIGHT_FINITE_DEBUG];
                }
                else
                {
                    return programNames[DIRECTIONAL_LIGHT_FINITE];
                }
            }
            else
            {
                if (debug)
                {
                    return programNames[DIRECTIONAL_LIGHT_DEBUG];
                }
                else
                {
                    return programNames[DIRECTIONAL_LIGHT];
                }
            }
        }
        else
        {
            if(finite)
            {
                if(debug)
                {
                    return programNames[POINT_LIGHT_FINITE_DEBUG];
                }
                else
                {
                    return programNames[POINT_LIGHT_FINITE];
                }
            }
            else
            {
                if(debug)
                {
                    return programNames[POINT_LIGHT_DEBUG];
                }
                else
                {
                    return programNames[POINT_LIGHT];
                }
            }
        }
    }
    //------------------------------------------------------------------
}