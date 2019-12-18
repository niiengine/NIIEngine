/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_MATERIAL_PROPERTY_H_
#define _NII_MATERIAL_PROPERTY_H_

#include "NiiPreInclude.h"

namespace NII
{
    class _EngineAPI MaterialProperty
    {
    public:
        static const PropertyID ShadowCastMaterialProperty;
        static const PropertyID ShadowReceiveMaterialProperty;
        static const PropertyID ShaderFusionProperty;
        static const PropertyID ShaderFusionSchemeProperty;
        static const PropertyID ShaderChProperty;
        static const PropertyID ShaderChTextureUnitProperty;
        static const PropertyID ShaderChTextureSrcProperty;
        static const PropertyID VertexProgramProperty;
        static const PropertyID FragmentProgramProperty;
        static const PropertyID GeoProgramProperty;
        static const PropertyID ShadowCastVertexProgramProperty;;
        static const PropertyID ShadowReceiveVertexProgramProperty;;
        static const PropertyID ShadowCastFragmentProgramProperty;
        static const PropertyID ShadowReceiveFragmentProgramProperty;
        static const PropertyID ReceiveShadowProperty;
        static const PropertyID AlphaCastShadowProperty;
        static const PropertyID GpuVendorProperty;

        static const PropertyID LodSchemeValueProperty;
        static const PropertyID LodSchemeCameraProperty;
        static const PropertyID LodSchemeIndexProperty;;
        static const PropertyID LodSchemeProperty;

        static const PropertyID ColourWriteProperty;
        static const PropertyID AmbientColourProperty;
        static const PropertyID DiffuseColourProperty;
        static const PropertyID SpecularColourProperty;
        static const PropertyID EmissiveColourProperty;
        static const PropertyID VertexColourProperty;

        static const PropertyID FrameBlendProperty;
        static const PropertyID FrameBlend2Property;
        static const PropertyID FrameBlendOpProperty;
        static const PropertyID FrameBlend2OpProperty;
        static const PropertyID FrameColourBlendProperty;
        static const PropertyID ColourFactorOneProperty;
        static const PropertyID ColourFactorZeroProperty;
        static const PropertyID ColourFactorDestProperty;
        static const PropertyID ColourFactorSrcProperty;
        static const PropertyID ColourFactorOneMinusDestProperty;
        static const PropertyID ColourFactorOneMinusSrcProperty;
        static const PropertyID ColourFactorDestAlphaProperty;
        static const PropertyID ColourFactorSrcAlphaProperty;
        static const PropertyID ColourFactorOneMinusDestAlphaProperty;
        static const PropertyID ColourFactorOneMinusSrcAlphaProperty;
        static const PropertyID FrameOpRevSubtractProperty;
        static const PropertyID FrameOpMinProperty;
        static const PropertyID FrameOpMaxProperty;
        static const PropertyID DepthCheckProperty;
        static const PropertyID DepthWriteProperty;
        static const PropertyID DepthFuncProperty;
        static const PropertyID DepthBiasProperty;
        static const PropertyID DepthMultiBiasProperty;

        static const PropertyID CmpModeAlwaysFailProperty;
        static const PropertyID CmpModeAlwaysPassProperty;
        static const PropertyID CmpModeLessEqualProperty;
        static const PropertyID CmpModeLessProperty;
        static const PropertyID CmpModeEqualProperty;
        static const PropertyID CmpModeNotEqualProperty;
        static const PropertyID CmpModeGreaterEqualProperty;
        static const PropertyID CmpModeGreaterProperty;

        static const PropertyID AlphaRejectProperty;
        static const PropertyID AlphaCoverageProperty;
        static const PropertyID LightClipProperty;
        static const PropertyID LightClipPlaneProperty;
        static const PropertyID ForceDepthSortProperty;
        static const PropertyID ShaderStepProperty;
        static const PropertyID ShaderStepDecalProperty;
        static const PropertyID CullingModeProperty;
        static const PropertyID CullingModeClockwiseProperty;
        static const PropertyID CullingModeAntClockwiseProperty;
        static const PropertyID SysCullingModeProperty;
        static const PropertyID SysCullingBackProperty;
        static const PropertyID SysCullingFrontProperty;
        static const PropertyID UnitNormalsProperty;
        static const PropertyID LightingProperty;
        static const PropertyID ShadeModeProperty;
        static const PropertyID ShadeModeFlatProperty;
        static const PropertyID ShadeModeGouraudProperty;
        static const PropertyID ShadeModePhongProperty;
        static const PropertyID PolygonModeProperty;
        static const PropertyID PolygonModeSolidProperty;
        static const PropertyID PolygonModeWireFrameProperty;
        static const PropertyID PolygonModePointsProperty;
        static const PropertyID AutoReduceProperty;
        static const PropertyID FogProperty;
        static const PropertyID NoneProperty;
        static const PropertyID LinearMathProperty;
        static const PropertyID PointMathProperty;
        static const PropertyID AnisoMathProperty;
        static const PropertyID FogExpProperty;
        static const PropertyID FogExp2Property;
        static const PropertyID MaxLightsProperty;
        static const PropertyID StartLightProperty;
        static const PropertyID LightMaskProperty;
        static const PropertyID MultiChProperty;
        static const PropertyID NoEachLightProperty;
        static const PropertyID EachLightProperty;
        static const PropertyID EachLightLimitCountProperty;
        static const PropertyID ShaderStepGlobalProperty;
        static const PropertyID ShaderStepDetailLightProperty;
        static const PropertyID PointLightProperty;
        static const PropertyID SpotLightProperty;
        static const PropertyID DirLightProperty;
        static const PropertyID PointSizeProperty;
        static const PropertyID PointSpritesProperty;
        static const PropertyID PointSizeAttenuationProperty;
        static const PropertyID PointSizeMinProperty;
        static const PropertyID PointSizeMaxProperty;

        static const PropertyID AddMathProperty;
        static const PropertyID SubtractMathProperty;
        static const PropertyID ReplaceMathProperty;
        static const PropertyID ModulateMathProperty;
        static const PropertyID AlphaMathProperty;
        static const PropertyID TextureProperty;
        static const PropertyID Texture1DProperty;
        static const PropertyID Texture2DProperty;
        static const PropertyID Texture3DProperty;
        static const PropertyID TextureCubicProperty;
        static const PropertyID TextureAutoMipMapProperty;
        static const PropertyID TextureAlphaProperty;
        static const PropertyID TextureGammaProperty;
        static const PropertyID TextureAnimProperty;
        static const PropertyID TextureCubic6Property;
        static const PropertyID TextureUVWProperty;
        static const PropertyID TextureCoordSetProperty;
        static const PropertyID TextureAddressModeProperty;
        static const PropertyID TextureAddressWrapProperty;
        static const PropertyID TextureAddressClampProperty;
        static const PropertyID TextureAddressBorderProperty;
        static const PropertyID TextureAddressMirrorProperty;
        static const PropertyID TextureBorderColourProperty;
        static const PropertyID TextureFilteringProperty;
        static const PropertyID TextureBilinearProperty;
        static const PropertyID TextureTrilinearProperty;
        static const PropertyID TextureAnisotropyProperty;
        static const PropertyID TextureMipmapBiasProperty;
        static const PropertyID TextureColourBlendProperty;
        static const PropertyID TextureColourBlend2Property;
        static const PropertyID TextureAlphaBlendProperty;
        static const PropertyID TextureBlendSource1Property;
        static const PropertyID TextureBlendSource2Property;
        static const PropertyID TextureBlendModulateX2Property;
        static const PropertyID TextureBlendModulateX4Property;
        static const PropertyID TextureBlendSignedProperty;
        static const PropertyID TextureBlendSmoothProperty;
        static const PropertyID TextureSpecularBlendProperty;
        static const PropertyID TextureDiffuseBlendProperty;
        static const PropertyID TextureCurrentTextureBlendProperty;
        static const PropertyID TexturePreTextureBlendProperty;
        static const PropertyID TextureBlendConstantFactorProperty;
        static const PropertyID TextureBlendDotProductProperty;
        static const PropertyID TextureBlendPreTextureProperty;
        static const PropertyID TextureBlendCurrentTextureProperty;
        static const PropertyID TextureBlendCurrentDiffuseProperty;
        static const PropertyID TextureBlendCurrentSpecularProperty;
        static const PropertyID TextureBlendConstantProperty;
        static const PropertyID TextureEnvMapProperty;
        static const PropertyID TextureSphericalProperty;
        static const PropertyID TexturePlanarProperty;
        static const PropertyID TextureReflectionProperty;
        static const PropertyID TextureNormalMapProperty;
        static const PropertyID TextureScrollProperty;
        static const PropertyID TextureScrollAnimProperty;
        static const PropertyID TextureRotateProperty;
        static const PropertyID TextureRotateAnimProperty;
        static const PropertyID TextureScaleProperty;
        static const PropertyID TextureWaveXFormProperty;
        static const PropertyID TextureScrollXProperty;
        static const PropertyID TextureScrollYProperty;
        static const PropertyID TextureScaleXProperty;
        static const PropertyID TextureScaleYProperty;
        static const PropertyID TextureWaveSineProperty;
        static const PropertyID TextureWaveTriangleProperty;
        static const PropertyID TextureWaveSquareProperty;
        static const PropertyID TextureWaveSawtoothProperty;
        static const PropertyID TextureWaveInvSawtoothProperty;
        static const PropertyID TextureMatrixProperty;
        static const PropertyID TextureFetchTypeProperty;
        static const PropertyID TextureFetchVertexProperty;
        static const PropertyID TextureFetchFragmentProperty;
        static const PropertyID TextureContentTypeProperty;
        static const PropertyID TextureNormalContentTypeProperty;
        static const PropertyID TextureShadowContentTypeProperty;
        static const PropertyID TextureFusionContentTypeProperty;
        static const PropertyID PropertyCount;
    };
}

#endif