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

#ifndef _NII_SHADOWVOLUMEEXTRUDEPROGRAM_H_
#define _NII_SHADOWVOLUMEEXTRUDEPROGRAM_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 一个为挤压体积阴影静态包含顶点着色程序的类
    @remark
        因为不存在,所以我们依赖于外部媒体文件.这里使用汇编所以我们就不必依赖特定的插
        件.这个汇编的文件内容由以下Cg代码生成:
    @code
        // 点光源阴影体积挤出
        void shadowVolumeExtrudePointLight_vp (
            float4 position         : POSITION,
            NIIf  wcoord            : TEXCOORD0,

            out float4 oPosition    : POSITION,

            uniform float4x4 worldViewProjMatrix,
            uniform float4   lightPos // homogeneous, object space
            )
        {
            // extrusion in object space
            // vertex unmodified if w==1, extruded if w==0
            float4 newpos =
                (wcoord.xxxx * lightPos) +
                float4(position.xyz - lightPos.xyz, 0);

            oPosition = mul(worldViewProjMatrix, newpos);
        }

        // 定向光挤压
        void shadowVolumeExtrudeDirLight_vp (
            float4 position            : POSITION,
            NIIf  wcoord            : TEXCOORD0,

            out float4 oPosition    : POSITION,

            uniform float4x4 worldViewProjMatrix,
            uniform float4   lightPos // homogenous, object space
            )
        {
            // extrusion in object space
            // vertex unmodified if w==1, extruded if w==0
            float4 newpos =
                (wcoord.xxxx * (position + lightPos)) - lightPos;

            oPosition = mul(worldViewProjMatrix, newpos);
        }

        // 点光源阴影体积挤压 - 有限
        void shadowVolumeExtrudePointLightFinite_vp (
            float4 position            : POSITION,
            NIIf  wcoord            : TEXCOORD0,

            out float4 oPosition    : POSITION,

            uniform float4x4 worldViewProjMatrix,
            uniform float4   lightPos, // homogeneous, object space
            uniform NIIf    extrusionDistance // how far to extrude
            )
        {
            // extrusion in object space
            // vertex unmodified if w==1, extruded if w==0
            float3 extrusionDir = position.xyz - lightPos.xyz;
            extrusionDir = normalize(extrusionDir);

            float4 newpos = float4(position.xyz +
                ((1 - wcoord.x) * extrusionDistance * extrusionDir), 1);

            oPosition = mul(worldViewProjMatrix, newpos);
        }

        // 定向光挤压 - 有限
        void shadowVolumeExtrudeDirLightFinite_vp (
            float4 position            : POSITION,
            NIIf  wcoord            : TEXCOORD0,

            out float4 oPosition    : POSITION,

            uniform float4x4 worldViewProjMatrix,
            uniform float4   lightPos, // homogeneous, object space
            uniform NIIf    extrusionDistance // how far to extrude
            )
        {
            // extrusion in object space
            // vertex unmodified if w==1, extruded if w==0
            // -ve lightPos is direction
            float4 newpos = float4(position.xyz -
                (wcoord.x * extrusionDistance * lightPos.xyz), 1);

            oPosition = mul(worldViewProjMatrix, newpos);
        }
    @endcode
    */
    class _EngineInner ShadowVolumeExtrudeProgram : public ShadowAlloc
    {
        #define EXTRUDER_PROGRAM_COUNT 8    
    public:
        enum Programs
        {
            POINT_LIGHT = 0,                    ///< 点光源挤出机,无限距离
            POINT_LIGHT_DEBUG = 1,              ///< 点光源挤出机,无限距离,调试模式
            DIRECTIONAL_LIGHT = 2,              ///< 定向光挤出机,无限距离
            DIRECTIONAL_LIGHT_DEBUG = 3,        ///< 定向光挤出机,无限距离,调试模式
            POINT_LIGHT_FINITE = 4,             ///< 点光源挤出机,有限的距离
            POINT_LIGHT_FINITE_DEBUG = 5,       ///< 点光源挤出机,有限的距离,调试模式
            DIRECTIONAL_LIGHT_FINITE = 6,       ///< 定向光挤出机,有限的距离
            DIRECTIONAL_LIGHT_FINITE_DEBUG = 7  ///< 定向光挤出机,有限的距离,调试模式
        };
        static ResourceID programNames[EXTRUDER_PROGRAM_COUNT];
        static ResourceID frgProgramName;

        /// 初始化它们顶点着色程序的创建
        static void init();
        /// 关闭&删除顶点着色程序
        static void shutdown();
        /// 获取为点光源的挤压着色程序 源,与arbvp1兼容
        static inline const VString & getPointLightExtruderArbvp1(void) { return mPointArbvp1; }
        /// 获取为点光源的挤压着色程序 源,与vs_1_1兼容
        static inline const VString & getPointLightExtruderVs_1_1(void) { return mPointVs_1_1; }
        /// 获取为点光源的挤压着色程序 源,与vs_4_0兼容
        static inline const VString & getPointLightExtruderVs_4_0(void) { return mPointVs_4_0; }
        /// Get extruder program source for point lights, compatible with glsles
        static inline const VString & getPointLightExtruderVs_glsles(void) { return mPointVs_glsles; }
        ///获取方向光源的挤压着色程序,与arbvp1兼容
        static inline const VString & getDirectionalLightExtruderArbvp1(void) { return mDirArbvp1; }
        /// 获取方向光源的挤压着色程序,与vs_1_1兼容
        static inline const VString & getDirectionalLightExtruderVs_1_1(void) { return mDirVs_1_1; }
        /// 获取方向光源的挤压着色程序,与vs_4_0兼容
        static inline const VString & getDirectionalLightExtruderVs_4_0(void) { return mDirVs_4_0; }
        /// Get extruder program source for directional lights, compatible with glsles
        static inline const VString & getDirectionalLightExtruderVs_glsles(void) { return mDirVs_glsles; }

        /// 获取为调整点光源的挤压着色程序,与arbvp1兼容
        static inline const VString & getPointLightExtruderArbvp1Debug(void) { return mPointArbvp1Debug; }
        /// 获取为调整点光源的挤压着色程序, 与vs_1_1兼容
        static inline const VString & getPointLightExtruderVs_1_1Debug(void) { return mPointVs_1_1Debug; }
        /// 获取为调整点光源的挤压着色程序, 与vs_4_0兼容
        static inline const VString & getPointLightExtruderVs_4_0Debug(void) { return mPointVs_4_0Debug; }
        /// Get extruder program source for debug point lights, compatible with glsles
        static inline const VString & getPointLightExtruderVs_glslesDebug(void) { return mPointVs_glslesDebug; }
        /// 获取为调整点光源的挤压着色程序,与arbvp1兼容
        static inline const VString & getDirectionalLightExtruderArbvp1Debug(void) { return mDirArbvp1Debug; }
        /// 获取为调整点光源的挤压着色程序,与vs_1_1兼容
        static inline const VString & getDirectionalLightExtruderVs_1_1Debug(void) { return mDirVs_1_1Debug; }
        /// 获取为调整点光源的挤压着色程序,与vs_4_0兼容
        static inline const VString & getDirectionalLightExtruderVs_4_0Debug(void) { return mDirVs_4_0Debug; }
        /// Get extruder program source for debug directional lights, compatible with glsles
        static inline const VString & getDirectionalLightExtruderVs_glslesDebug(void) { return mDirVs_glslesDebug; }
        /// 通用方法,获取任何着色程序的源
        static inline const VString & getProgramSource(LightType type, ShaderLanguage syntax, bool finite, bool debug);

        static ResourceID getProgramName(LightType lightType, bool finite, bool debug);

        /// 获取为点光源的FINITE(有限)挤压着色程序, 与arbvp1兼容
        static inline const VString & getPointLightExtruderArbvp1Finite(void) { return mPointArbvp1Finite; }
        /// 获取为点光源的FINITE(有限)挤压着色程序,与vs_1_1兼容
        static inline const VString & getPointLightExtruderVs_1_1Finite(void) { return mPointVs_1_1Finite; }
        /// 获取为点光源的FINITE(有限)挤压着色程序,与vs_4_0兼容
        static inline const VString & getPointLightExtruderVs_4_0Finite(void) { return mPointVs_4_0Finite; }
        /// Get FINITE extruder program source for point lights, compatible with glsles
        static inline const VString & getPointLightExtruderVs_glslesFinite(void) { return mPointVs_glslesFinite; }        
        /// 获取为点光源的FINITE(有限)挤压着色程序, 与arbvp1兼容
        static inline const VString & getDirectionalLightExtruderArbvp1Finite(void) { return mDirArbvp1Finite; }
        /// 获取为点光源的FINITE(有限)挤压着色程序, 与vs_1_1兼容
        static inline const VString & getDirectionalLightExtruderVs_1_1Finite(void) { return mDirVs_1_1Finite; }
        /// 获取为点光源的FINITE(有限)挤压着色程序,与vs_4_0兼容
        static inline const VString & getDirectionalLightExtruderVs_4_0Finite(void) { return mDirVs_4_0Finite; }
        /// Get FINITE extruder program source for directional lights, compatible with glsles
        static inline const VString & getDirectionalLightExtruderVs_glslesFinite(void) { return mDirVs_glslesFinite; }
        /// 获取为调整点光源的FINITE(有限)挤压着色程序, 与arbvp1兼容
        static inline const VString & getPointLightExtruderArbvp1FiniteDebug(void) { return mPointArbvp1FiniteDebug; }
        /// 获取为调整点光源的FINITE(有限)挤压着色程序,与vs_1_1兼容
        static inline const VString & getPointLightExtruderVs_1_1FiniteDebug(void) { return mPointVs_1_1FiniteDebug; }
        /// 获取为调整点光源的FINITE(有限)挤压着色程序, 与vs_4_0兼容
        static inline const VString & getPointLightExtruderVs_4_0FiniteDebug(void) { return mPointVs_4_0FiniteDebug; }
        /// Get extruder program source for debug point lights, compatible with glsles
        static inline const VString & getPointLightExtruderVs_glslesFiniteDebug(void) { return mPointVs_glslesFiniteDebug; }        
        /// 获取为调整点光源的FINITE(有限)挤压着色程序, 与arbvp1兼容
        static inline const VString & getDirectionalLightExtruderArbvp1FiniteDebug(void) { return mDirArbvp1FiniteDebug; }
        /// 获取为调整点光源的FINITE(有限)挤压着色程序, 与vs_1_1兼容
        static inline const VString & getDirectionalLightExtruderVs_1_1FiniteDebug(void) { return mDirVs_1_1FiniteDebug; }
        /// 获取为调整点光源的FINITE(有限)挤压着色程序,与vs_4_0兼容
        static inline const VString & getDirectionalLightExtruderVs_4_0FiniteDebug(void) { return mDirVs_4_0FiniteDebug; }
        /// Get FINITE extruder program source for debug directional lights, compatible with glsles
        static inline const VString & getDirectionalLightExtruderVs_glslesFiniteDebug(void) { return mDirVs_glslesFiniteDebug; }    
    private:
        static VString mPointArbvp1;
        static VString mPointVs_1_1;
        static VString mPointVs_4_0;
        static VString mPointVs_glsles;
        static VString mDirArbvp1;
        static VString mDirVs_1_1;
        static VString mDirVs_4_0;
        static VString mDirVs_glsles;
        // 像上面那样,除了颜色设置为1,去启用调试被看见体积
        static VString mPointArbvp1Debug;
        static VString mPointVs_1_1Debug;
        static VString mPointVs_4_0Debug;
        static VString mPointVs_glslesDebug;
        static VString mDirArbvp1Debug;
        static VString mDirVs_1_1Debug;
        static VString mDirVs_4_0Debug;
        static VString mDirVs_glslesDebug;

        static VString mPointArbvp1Finite;
        static VString mPointVs_1_1Finite;
        static VString mPointVs_4_0Finite;
        static VString mPointVs_glslesFinite;
        static VString mDirArbvp1Finite;
        static VString mDirVs_1_1Finite;
        static VString mDirVs_4_0Finite;
        static VString mDirVs_glslesFinite;
        // 像上面那样,除了颜色设置为1,去启用调试被看见体积
        static VString mPointArbvp1FiniteDebug;
        static VString mPointVs_1_1FiniteDebug;
        static VString mPointVs_4_0FiniteDebug;
        static VString mPointVs_glslesFiniteDebug;
        static VString mDirArbvp1FiniteDebug;
        static VString mDirVs_1_1FiniteDebug;
        static VString mDirVs_4_0FiniteDebug;
        static VString mDirVs_glslesFiniteDebug;

        static VString mGeneralFs_4_0;
        static VString mGeneralFs_glsles;
        /// 已经向GpuProgramManager注册它们了吗?
        static bool mInitialised;    
    };
}
#endif