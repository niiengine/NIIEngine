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
#include "NiiMaterialProperty.h"
#include "NiiScriptProperty.h"

namespace NII
{
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadowCastMaterialProperty,            0);  // FontDefine
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadowReceiveMaterialProperty,         1);  
    N_PROPERTY(ScriptProperty, MaterialProperty, ShaderFusionProperty,                  2);  // 
    N_PROPERTY(ScriptProperty, MaterialProperty, ShaderFusionSchemeProperty,            3);  // 
    N_PROPERTY(ScriptProperty, MaterialProperty, ShaderChProperty,                      4);  //
    N_PROPERTY(ScriptProperty, MaterialProperty, ShaderChTextureUnitProperty,           5);  // 
    N_PROPERTY(ScriptProperty, MaterialProperty, ShaderChTextureSrcProperty,            6);  // 
    N_PROPERTY(ScriptProperty, MaterialProperty, VertexProgramProperty,                 7);  // 
    N_PROPERTY(ScriptProperty, MaterialProperty, FragmentProgramProperty,               8);  // 
    N_PROPERTY(ScriptProperty, MaterialProperty, GeoProgramProperty,                    9);  //
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadowCastVertexProgramProperty,       10);  //
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadowReceiveVertexProgramProperty,    11);  //
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadowCastFragmentProgramProperty,     12);  //
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadowReceiveFragmentProgramProperty,  13);  //
    N_PROPERTY(ScriptProperty, MaterialProperty, LodSchemeValueProperty,                14);
    N_PROPERTY(ScriptProperty, MaterialProperty, LodSchemeCameraProperty,               15);
    N_PROPERTY(ScriptProperty, MaterialProperty, LodSchemeIndexProperty,                16);
    N_PROPERTY(ScriptProperty, MaterialProperty, LodSchemeProperty,                     17);

    N_PROPERTY(ScriptProperty, MaterialProperty, ReceiveShadowProperty,                 18);
    N_PROPERTY(ScriptProperty, MaterialProperty, AlphaCastShadowProperty,               19);
    N_PROPERTY(ScriptProperty, MaterialProperty, GpuVendorProperty,                     20);

    N_PROPERTY(ScriptProperty, MaterialProperty, ColourWriteProperty,                   21);
    N_PROPERTY(ScriptProperty, MaterialProperty, AmbientColourProperty,                 22);
    N_PROPERTY(ScriptProperty, MaterialProperty, DiffuseColourProperty,                 23);
    N_PROPERTY(ScriptProperty, MaterialProperty, SpecularColourProperty,                24);
    N_PROPERTY(ScriptProperty, MaterialProperty, EmissiveColourProperty,                25);
    N_PROPERTY(ScriptProperty, MaterialProperty, VertexColourProperty,                  26);

    N_PROPERTY(ScriptProperty, MaterialProperty, FrameBlendProperty,                    27);
    N_PROPERTY(ScriptProperty, MaterialProperty, FrameBlend2Property,                   28);
    N_PROPERTY(ScriptProperty, MaterialProperty, FrameBlendOpProperty,                  29);
    N_PROPERTY(ScriptProperty, MaterialProperty, FrameBlend2OpProperty,                 30);
    N_PROPERTY(ScriptProperty, MaterialProperty, FrameColourBlendProperty,              31);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorOneProperty,               32);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorZeroProperty,              33);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorDestProperty,              34);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorSrcProperty,               35);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorOneMinusDestProperty,      36);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorOneMinusSrcProperty,       37);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorDestAlphaProperty,         38);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorSrcAlphaProperty,          39);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorOneMinusDestAlphaProperty, 40);
    N_PROPERTY(ScriptProperty, MaterialProperty, ColourFactorOneMinusSrcAlphaProperty,  41);
    N_PROPERTY(ScriptProperty, MaterialProperty, FrameOpRevSubtractProperty,            42);
    N_PROPERTY(ScriptProperty, MaterialProperty, FrameOpMinProperty,                    43);
    N_PROPERTY(ScriptProperty, MaterialProperty, FrameOpMaxProperty,                    44);
    N_PROPERTY(ScriptProperty, MaterialProperty, DepthCheckProperty,                    45);
    N_PROPERTY(ScriptProperty, MaterialProperty, DepthWriteProperty,                    46);
    N_PROPERTY(ScriptProperty, MaterialProperty, DepthFuncProperty,                     47);
    N_PROPERTY(ScriptProperty, MaterialProperty, DepthBiasProperty,                     48);
    N_PROPERTY(ScriptProperty, MaterialProperty, DepthMultiBiasProperty,                49);

    N_PROPERTY(ScriptProperty, MaterialProperty, CmpModeAlwaysFailProperty,             50);
    N_PROPERTY(ScriptProperty, MaterialProperty, CmpModeAlwaysPassProperty,             51);
    N_PROPERTY(ScriptProperty, MaterialProperty, CmpModeLessEqualProperty,              52);
    N_PROPERTY(ScriptProperty, MaterialProperty, CmpModeLessProperty,                   53);
    N_PROPERTY(ScriptProperty, MaterialProperty, CmpModeEqualProperty,                  54);
    N_PROPERTY(ScriptProperty, MaterialProperty, CmpModeNotEqualProperty,               55);
    N_PROPERTY(ScriptProperty, MaterialProperty, CmpModeGreaterEqualProperty,           56);
    N_PROPERTY(ScriptProperty, MaterialProperty, CmpModeGreaterProperty,                57);

    N_PROPERTY(ScriptProperty, MaterialProperty, AlphaRejectProperty,                   58);
    N_PROPERTY(ScriptProperty, MaterialProperty, AlphaCoverageProperty,                 59);
    N_PROPERTY(ScriptProperty, MaterialProperty, LightClipProperty,                     60);
    N_PROPERTY(ScriptProperty, MaterialProperty, LightClipPlaneProperty,                61);
    N_PROPERTY(ScriptProperty, MaterialProperty, ForceDepthSortProperty,                62);
    N_PROPERTY(ScriptProperty, MaterialProperty, ShaderStepProperty,                    63);
    N_PROPERTY(ScriptProperty, MaterialProperty, ShaderStepDecalProperty,               64);
    N_PROPERTY(ScriptProperty, MaterialProperty, CullingModeProperty,                   65);
    N_PROPERTY(ScriptProperty, MaterialProperty, CullingModeClockwiseProperty,          66);
    N_PROPERTY(ScriptProperty, MaterialProperty, CullingModeAntClockwiseProperty,       67);
    N_PROPERTY(ScriptProperty, MaterialProperty, SysCullingModeProperty,                68);
    N_PROPERTY(ScriptProperty, MaterialProperty, SysCullingBackProperty,                69);
    N_PROPERTY(ScriptProperty, MaterialProperty, SysCullingFrontProperty,               70);
    N_PROPERTY(ScriptProperty, MaterialProperty, UnitNormalsProperty,                   71);
    N_PROPERTY(ScriptProperty, MaterialProperty, LightingProperty,                      72);
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadeModeProperty,                     73);
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadeModeFlatProperty,                 74);
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadeModeGouraudProperty,              75);
    N_PROPERTY(ScriptProperty, MaterialProperty, ShadeModePhongProperty,                76);
    N_PROPERTY(ScriptProperty, MaterialProperty, PolygonModeProperty,                   77);
    N_PROPERTY(ScriptProperty, MaterialProperty, PolygonModeSolidProperty,              78);
    N_PROPERTY(ScriptProperty, MaterialProperty, PolygonModeWireFrameProperty,          79);
    N_PROPERTY(ScriptProperty, MaterialProperty, PolygonModePointsProperty,             80);
    N_PROPERTY(ScriptProperty, MaterialProperty, AutoReduceProperty,                    81);
    N_PROPERTY(ScriptProperty, MaterialProperty, FogProperty,                           82);
    N_PROPERTY(ScriptProperty, MaterialProperty, NoneProperty,                          83);
    N_PROPERTY(ScriptProperty, MaterialProperty, LinearMathProperty,                    84);
    N_PROPERTY(ScriptProperty, MaterialProperty, PointMathProperty,                     85);
    N_PROPERTY(ScriptProperty, MaterialProperty, AnisoMathProperty,                     86);
    N_PROPERTY(ScriptProperty, MaterialProperty, FogExpProperty,                        87);
    N_PROPERTY(ScriptProperty, MaterialProperty, FogExp2Property,                       88);
    N_PROPERTY(ScriptProperty, MaterialProperty, MaxLightsProperty,                     89);
    N_PROPERTY(ScriptProperty, MaterialProperty, StartLightProperty,                    90);
    N_PROPERTY(ScriptProperty, MaterialProperty, LightMaskProperty,                     91);
    N_PROPERTY(ScriptProperty, MaterialProperty, MultiChProperty,                       92);
    N_PROPERTY(ScriptProperty, MaterialProperty, NoEachLightProperty,                   93);
    N_PROPERTY(ScriptProperty, MaterialProperty, EachLightProperty,                     94);
    N_PROPERTY(ScriptProperty, MaterialProperty, EachLightLimitCountProperty,           95);
    N_PROPERTY(ScriptProperty, MaterialProperty, ShaderStepGlobalProperty,              96);
    N_PROPERTY(ScriptProperty, MaterialProperty, ShaderStepDetailLightProperty,         97);
    N_PROPERTY(ScriptProperty, MaterialProperty, PointLightProperty,                    98);
    N_PROPERTY(ScriptProperty, MaterialProperty, SpotLightProperty,                     99);
    N_PROPERTY(ScriptProperty, MaterialProperty, DirLightProperty,                      100);
    N_PROPERTY(ScriptProperty, MaterialProperty, PointSizeProperty,                     101);
    N_PROPERTY(ScriptProperty, MaterialProperty, PointSpritesProperty,                  102);
    N_PROPERTY(ScriptProperty, MaterialProperty, PointSizeAttenuationProperty,          103);
    N_PROPERTY(ScriptProperty, MaterialProperty, PointSizeMinProperty,                  104);
    N_PROPERTY(ScriptProperty, MaterialProperty, PointSizeMaxProperty,                  105);
    
    N_PROPERTY(ScriptProperty, MaterialProperty, AddMathProperty,                       106);
    N_PROPERTY(ScriptProperty, MaterialProperty, SubtractMathProperty,                  107);
    N_PROPERTY(ScriptProperty, MaterialProperty, ReplaceMathProperty,                   108);
    N_PROPERTY(ScriptProperty, MaterialProperty, ModulateMathProperty,                  109);
    N_PROPERTY(ScriptProperty, MaterialProperty, AlphaMathProperty,                     110);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureProperty,                       111);
    N_PROPERTY(ScriptProperty, MaterialProperty, Texture1DProperty,                     112);
    N_PROPERTY(ScriptProperty, MaterialProperty, Texture2DProperty,                     113);
    N_PROPERTY(ScriptProperty, MaterialProperty, Texture3DProperty,                     114);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureCubicProperty,                  115);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAutoMipMapProperty,             116);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAlphaProperty,                  117);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureGammaProperty,                  118);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAnimProperty,                   119);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureCubic6Property,                 120);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureUVWProperty,                    121);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureCoordSetProperty,               122);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAddressModeProperty,            123);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAddressWrapProperty,            124);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAddressClampProperty,           125);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAddressBorderProperty,          126);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAddressMirrorProperty,          127);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBorderColourProperty,           128);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureFilteringProperty,              129);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBilinearProperty,               130);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureTrilinearProperty,              131);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAnisotropyProperty,             132);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureMipmapBiasProperty,             133);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureColourBlendProperty,            134);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureColourBlend2Property,           135);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureAlphaBlendProperty,             136);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendSource1Property,           137);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendSource2Property,           138);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendModulateX2Property,        139);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendModulateX4Property,        140);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendSignedProperty,            141);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendSmoothProperty,            142);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureSpecularBlendProperty,          143);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureDiffuseBlendProperty,           144);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureCurrentTextureBlendProperty,    145);
    N_PROPERTY(ScriptProperty, MaterialProperty, TexturePreTextureBlendProperty,        146);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendConstantFactorProperty,    147);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendDotProductProperty,        148);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendPreTextureProperty,        149);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendCurrentTextureProperty,    150);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendCurrentDiffuseProperty,    151);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendCurrentSpecularProperty,   152);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureBlendConstantProperty,          153);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureEnvMapProperty,                 154);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureSphericalProperty,              155);
    N_PROPERTY(ScriptProperty, MaterialProperty, TexturePlanarProperty,                 156);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureReflectionProperty,             157);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureNormalMapProperty,              158);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureScrollProperty,                 159);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureScrollAnimProperty,             160);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureRotateProperty,                 161);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureRotateAnimProperty,             162);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureScaleProperty,                  163);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureWaveXFormProperty,              164);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureScrollXProperty,                165);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureScrollYProperty,                166);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureScaleXProperty,                 167);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureScaleYProperty,                 168);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureWaveSineProperty,               169);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureWaveTriangleProperty,           170);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureWaveSquareProperty,             171);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureWaveSawtoothProperty,           172);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureWaveInvSawtoothProperty,        173);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureMatrixProperty,                 174);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureFetchTypeProperty,              175);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureFetchVertexProperty,            176);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureFetchFragmentProperty,          177);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureContentTypeProperty,            178);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureNormalContentTypeProperty,      179);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureShadowContentTypeProperty,      180);
    N_PROPERTY(ScriptProperty, MaterialProperty, TextureFusionContentTypeProperty,      181);
    N_PROPERTY(ScriptProperty, MaterialProperty, PropertyCount,                         182);
}