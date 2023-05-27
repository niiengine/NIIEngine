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

#ifndef _NII_PRE_INCLUDE_H_
#define _NII_PRE_INCLUDE_H_

#include "NiiPlat.h"
#include "NiiVersion.h"
#include "NiiBasicTypes.h"
#include "NiiThreadInclude.h"
#include "NiiStdInclude.h"
#include "NiiStringBase.h"
#include "NiiHash.h"
#include "NiiI18NLib.h"
#include "NiiDefine.h"

namespace NII
{
    /** 大小模式
    @version NIIEngine 3.0.0
    */
    enum SizeMode
    {
        SM_Unit,        ///< 单位级
        SM_Abs,         ///< 绝对
        SM_Rel          ///< 相对
    };

    /** 垂直对齐模式
    @version NIIEngine 3.0.0
    */
    enum VAlign
    {
        VA_TOP,         ///< 上对齐
        VA_CENTRE,      ///< 垂直中对齐
        VA_BOTTOM       ///< 下对齐
    };

    /** 水平对齐模式
    @version NIIEngine 3.0.0
    */
    enum HAlign
    {
        HA_LEFT,        ///< 左对齐
        HA_CENTRE,      ///< 水平中对齐
        HA_RIGHT        ///< 右对齐
    };

    class Engine;
    class BitSet;
    class ThreadEvent;
    class Job;
    class JobPrc;
    class JobResult;
    class JobQueue;
    class Buffer;
    class PropertyData;
    class RenderEffect;
    class SerializerFactoryObj;
    class GpuProgramParam;
    class GpuParamDefine;
    class GpuParamValue;
    class PropertyCmdSet;
    class PixelChain;
    class PixelBuffer;
    class PixelBufferGroup;
    class PixelBufferManager;
    class MemStream;
    class ScriptLanguage;
    class ScriptHelperManager;
    class HardwareAnimation;
    class ScriptFunctor;
    class ScriptModule;
    class Clipboard;
    class AnimationFusion;
    class NodeFusion;
    class VertexOffsetFusion;
    class UnitFusion;
    class AnimationFusionObj;
    class VertexKeyFrameTrack;
    class RectGeo;
    class PropertyKeyFrameTrack;
    class TrackTime;
    class KeyFrameTrack;
    class Animation;
    class AnimationObj;
    class AnimationManager;
    class PropertyFusion;
    class XmlAnalyze;
    class XmlPattern;
    class XmlSerializer;
    class ScriptModule;
    class VFS;
    class VFSFactory;
    class VFSManager;
    class SysSyncParam;
    class AABox;
    class AABSpaceQuery;
    class DrawCallGroup;
    class DrawCallManager;
    class Node;
    class Audio;
    class SyncParticle;
    class ParticleCtrl;
    class Bone;
    class Camera;
    class CameraListener;
    class Codec;
    class ConfigDialog;
    template <typename in, typename out> class DataEquation;
    template <typename in, typename out> class DataFunc;
    class EquationManager;
    template <typename t> class DataValue;
    class Angle;
    class ExtFrameBuffer;
    class DynamicModule;
    class DynamicModuleManager;
    class GeoEdget;
    class EdgeListBuilder;
    class ErrorDialog;
    class ClipboardData;
    class ExtTextureManager;
    class Factory;
    class FusionData;
    class FusionNum;
    class Particle;
    class ParticleEffect;
    class Font;
    class FontEventArgs;
    class FontGlyph;
    class FreeTypeFont;
    class PixmapFont;
    class FontManager;
    class BufferManager;
    class GBufferManager;
    class EventArgs;
    class RenderFrameListener;
    class Frustum;
    class RegexMatcher;
    class RegexManager;
    class Colour;
    class ColourArea;
    class GpuProgram;
    class GpuProgramManager;
    class ApuProgram;
    class ApuParamMap;
    class ApuProgramParam;
    class ControlItemManager;
    class IndexBuffer;
    class StructBuffer;
    class PixelCountQuery;
    class DataStream;
    class VertexBuffer;
    class GpuBuffer;
    class FrameBuffer;
    class TextureBuffer;
    class IndirectBuffer;
    class CounterBuffer;
    class StorageBuffer;
    class HighLevelGpuProgram;
    class HighLevelGpuProgramManager;
    class HighLevelGpuProgramFactory;
    class LoadScheme;
    class IndexData;
    class InstanceBatch;
    class InstanceMesh;
    class InstanceGeo;
    class Image;
    class KeyFrame;
    class Light;
    class Log;
    class LogManager;
    class LodScheme;
    class MaterialLodScheme;
    class MeshLodScheme;
    class LodSchemeManager;
    class ResultScheme;
    class SpaceTypeFactory;
    class ScriptAnalyze;
    class ScriptError;
    class ScriptNode;
    class ResLoadScheme;
    class ResResultScheme;
    class RenderFrameArgs;
    class CustomSpaceObj;
    class Material;
    class MaterialManager;
    class Math;
    class TextureFrame;
    template <typename T = NIIf>class Matrix3;
    template <typename T = NIIf>class Matrix4;
    template <typename T> class Size;
    template <typename = TimeDurMS, typename = NIIf>class DataEquation;
    class MemoryManager;
    class Mesh;
    class MeshManager;
    class SpaceObj;
    class SpaceObjFactory;
    class SpaceObjListener;
    class PlaneSpaceObj;
    class PosNode;
    class NodeKeyFrameTrack;
    class NodeKeyFrame;
    class UnitKeyFrameTrack;
    class UnitKeyFrame;
    class AudioChanneInfo;
    class Sky;
    class InstanceSpace;
    class PlaneSpaceQuery;
    class AreaSpaceQuery;
    class Overlay;
    class OverlayView;
    class OverlayGeo;
    class OverlayGeoFactory;
    class OverlayManager;
    class ParticleEffect;
    class SpreadParticle;
    class ParticleSpace;
    class ParticleSpaceManager;
    class ParticleGeo;
    class ParticleGeoFactory;
    class ParticleEffectFactory;
    class SpreadParticleFactory;
    class FileSerializerFactory;
    class ShaderCh;
    class PatchMesh;
    class PixelBlock;
    class Plane;
    class Plugin;
    class VertexOffset;
    class ProgressiveMesh;
    class Profile;
    class Profiler;
    class FileDataStream;
    class Quaternion;
    class Radian;
    class Ray;
    class RaySpaceQuery;
    class RenderLevelGroup;
    class RenderQueue;
    class RenderGroup;
    class RenderGroupFusion;
    class RenderQueueFusion;

    class RenderListener;
    class RenderSys;
    class RenderFeature;
    class RenderSysManager;
    class RenderFeatureSerializer;
    class FrameObj;
    class FrameObjListener;
    class PlaneSpace;
    class MultiTextureFrame;
    class ViewWindow;
    class Resource;
    class ResourceSchemeManager;
    class ResourceManager;
    class PixelOrbit;
    class GeometryObj;
    class GeometryRaw;
    class GeometryPixel;
    class GeometryQueue;
    class GeometryLevelEventArgs;
    class GeometryCom;
    class GeometrySpace;
    class SpaceManager;
    class GeometryObjListener;
    class SpaceTypeManager;
    class SpaceNode;
    class SpaceQuery;
    class SpaceQueryCmd;
    class ScriptLangMap;
    class ScriptProperty;
    class ScriptCompiler;
    class ScriptCompilerManager;
    class ScriptAnalyzeFactory;
    class PropertyScriptSys;
    class TextureAsync;
    class ThreadCondition;
    class ThreadMutex;
    class Thread;
    class ThreadMain;
    class ThreadEventGroup;
    class ThreadEventSignalGroup;
    class ProgramScriptLanguage;
    class Serializer;
    class FileSerializer;
    class MeshSerializer;
    class ShadowObj;
    class ShadowVolume;
    class ShadowTextureManager;
    class RenderObj;
    class LinearSpline;
    class Skeleton;
    class SkeletonFusion;
    class SkeletonManager;
    class VertexUnitKeyFrameTrack;
    class VertexOffsetKeyFrameTrack;
    class Sphere;
    class SphereSpaceQuery;
    class StaticSpace;
    class FileStreamSerializer;
    class StrConvert;
    class PropertyCmdObj;
    class SubMesh;
    class TagBone;
    class ShaderFusion;
    class ExtTexture;
    class PhysicalObj;
    class PhysicalScope;
    class PhysicalCom;
    class TextureSample;
    class TextureSampleUnit;
    class ShaderChTextureUnit;
    class ShaderChFog;
    class ShaderChColour;
    class ShaderChPoint;
    class ShaderChBlend;
    class ShaderChStencil;
    class ShaderChProgram;
    class Texture;
    class TextureManager;
    class NodeKeyFrame;
    class Timer;
    class ExtData;
    template <typename V2 = NIIf>class Vector2;
    template <typename V3 = NIIf>class Vector3;
    template <typename V4 = NIIf>class Vector4;
    template <typename T = NIIf>class TRect;
    class Viewport;
    class VertexData;
    class VertexUnitKeyFrame;
    class VertexOffsetKeyFrame;
    class WireAABGeo;
    class ThreadManager;
    class Fusion;
    class FusionManager;
    class FusionInstance;
    class FrameFusion;
    class FrameFusionListener;
    class ScriptModule;
    class GeometryQuery;
    class FusionShaderOp;
    class FusionShader;
    class FrameShader;
    class InputManager;
    class FusionRender;
    class RenderEffectManager;
    class IDManager;
    class Pattern;
    class RenderPattern;
    class ApuContext;
    class ApuProgram;
    class ApuProgramParam;
    class ApuParamMap;
    class SimdManager;

    class UIManager;
    class WidgetManager;

    namespace NII_COMMAND
    {
        class Command;
        class Condition;
        class EventObj;
        class CommandObj;
        class EventSignal;
        class Functor;
        class RefFunctor;
        class PtrFunctor;
        class MethodFunctor;
        class CopyFunctor;
        class CommandFunctor;
        class EventFunctor;
        class MemberFunctor;
        class EventObjectManager;
        class Event;
    }

    namespace NII_NET
    {

    }

    namespace NII_MEDIA
    {
        class ControlPattern;
        class ControlItem;
        class KeyboardControlPattern;
        class MouseControlPattern;
        class JoyPadControlPattern;
        class JoyStickControlPattern;
        class JoyHandleControlPattern;
        class JoyWheelControlPattern;
        class TouchControlPattern;
        class MouseControlItem;
        class KeyboardControlItem;
        class JoyPadControlItem;
        class JoyStickControlItem;
        class JoyHandleControlItem;
        class JoyWheelControlItem;
        class TouchControlItem;
        class MouseArgs;

        class SoundSys;
        class AudioLoader;
        class SoundReceive;
        class SoundSource;
        class AudioBuffer;
        class Sound;
        class BgSound;
        class MusicSound;
        class NormalSound;
        class FusionSound;
        class SoundUnit;
        class DefaultSoundUnit;
        class MusicSoundUnit;
        class SoundReceiveObj;
    }

    typedef vector<Font *>::type FontList;
    typedef vector<Material *>::type MaterialList;
    typedef vector<Texture *>::type TextureList;
    typedef vector<NII_MEDIA::Sound *>::type SoundList;
    typedef set<Texture *>::type TextureSet;
    typedef vector<GeometryObj *>::type GeometryObjList;
    typedef vector<ShadowVolume *>::type ShadowVolumeList;
    typedef vector<ShaderFusion *>::type ShaderFusionList;
    typedef vector<IndexData *>::type IndexDataList;
    typedef vector<ShaderCh *>::type ShaderChList;
	typedef vector<Thread *>::type ThreadList;
    typedef vector<PhysicalObj *>::type PhysicalObjList;
    typedef vector<PhysicalCom *>::type PhysicalComList;
	typedef set<Thread *>::type ThreadSet;
    typedef vector<ThreadEvent *>::type ThreadEventList;
    typedef vector<ThreadEventGroup *>::type ThreadEventGroupList;

    typedef HashedVector<Light *> LightList;

    typedef vector<Nui8>::type DataArray;
    typedef vector<Nui32>::type UIntArray;
    typedef vector<Ni32>::type IntArray;
    typedef vector<NIIf>::type FloatArray;
    typedef vector<NIId>::type DoubleArray;
    typedef vector<NIIf>::type BufferArray;
    typedef vector<Nid>::type IDList;
    typedef vector<Nidx>::type IdxList;
    typedef vector<Nui32>::type ChList;
    typedef vector<Nui16>::type SliceList
    typedef vector<ResourceID>::type ResourceIDList;
    typedef vector<GroupID>::type GroupIDList;
    typedef vector<ObjID>::type ObjIDList;
    typedef vector<GameObjID>::type GameObjIDList;
    typedef vector<EventID>::type EventIDList;
    typedef vector<PropertyID>::type PropertyArray;
    typedef vector<void *>::type PtrList;
	typedef vector<NIIf *>::type FloatPtrList;
    typedef vector<Nui16>::type BoneIndexList;
    typedef vector<SubMesh *>::type SubMeshList;
    typedef vector<Bone *>::type BoneList;
    typedef vector<VertexOffset *>::type VertexOffsetList;
    typedef vector<ShadowObj *>::type ShadowVolumeGenList;
    
    typedef vector<GpuBuffer *>::type GpuBufferList;
    typedef vector<IndexBuffer *>::type IndexBufferList;
    typedef vector<VertexBuffer *>::type VertexBufferList;
    typedef vector<CounterBuffer *>::type CounterBufferList;
    typedef vector<StructBuffer *>::type StructBufferList;
    typedef vector<IndirectBuffer *>::type IndirectBufferList;
    typedef vector<StorageBuffer *>::type StorageBufferList;
    typedef vector<PixelBuffer *>::type PixelBufferList;
    typedef vector<MappedBuffer *>::type MappedBufferList;
    typedef vector<FrameBuffer *>::type FrameBufferList;
    typedef vector<TextureBuffer *>::type TextureBufferList;
    typedef vector<TextureAsync *>::type TextureAsyncList;
    typedef vector<GpuProgram *>::type GpuProgramList;
    typedef unordered_set<GeometryRaw *>::type VAOList;

    typedef Nui32 RGBA;
    typedef Nui32 ARGB;
    typedef Nui32 ABGR;
    typedef Nui32 BGRA;
    
    typedef Nui64 HashType;

    #define multiCeil(num, alignment )  (((num + alignment - 1u) / alignment ) * alignment)
    #define multiFloor(num, alignment ) NCount((num / alignment) * alignment)

    #define N_MAGIC3(c0, c1, c2)        (c0 | (c1 << 8) | (c2 << 16))
    #define N_MAGIC4(c0, c1, c2, c3)    (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
    #define N_TYPE_MAX(type)            (std::numeric_limits<type>::max)()
    #define N_TYPE_MIN(type)            (std::numeric_limits<type>::min)()
    #define N_TYPE_EPSILON(type)        std::numeric_limits<type>::epsilon()
    #define N_TYPE_INFINITY(type)       std::numeric_limits<type>::infinity()
    #define N_MIN(type, a, b)           std::min<type>((a), (b))
    #define N_MAX(type, a, b)           std::max<type>((a), (b))
    #define N_StrBlank                  StrUtil::BLANK
    #define N_ToPixel(x)                NIIf((int)((x)+((x) > 0.0f ? 0.5f : -0.5f)))

    #define N_OBJ(p, c, f, v)           const StateID c::f = v + p::ObjCount
    #define N_EVENT(p, c, f, v)         const EventID c::f = v + p::EventCount
    #define N_PROPERTY(p, c, f, v)      const PropertyID c::f = v + p::PropertyCount
    #define N_STATE(p, c, f, v)         const StateID c::f = v + p::StateCount

    #define NII_STREAM_TEMP_SIZE    128

    #define N_PRIMARY_Viewport      1
    #define N_VR_Left_Viewport      2
    #define N_VR_RIGHT_Viewport     3
    #define N_Shadow_Viewport       4
    #define N_UI_Viewport           5

    #define N_ScriptModule_UI       9

    //一般本地使用大字节序时才与网络相同,但一般操作系统都是小字节序的
    //#define N_NET_Endian_N
    #define N_NET_PROTOCOL_VER 4
    #define N_Net_MaxMtuSize 1492
    #define N_Net_MinMtuSize 400

    // 音频缓存 2MByte
    #define N_MAX_AUDIO_BUFFER_SIZE 2097152

    // 视频缓存 400(宽或高)*800(宽或高)*3(RGB 3字节)*23(标准电影36帧每秒)
    #define N_MAX_VIDEO_BUFFER_SIZE 22080000

    // 输入阀值 单位毫秒
    #define N_INPUT_INTERVAL 250

    // 用于标记时间有效位 32位无符号
    #define N_TIME_MARK 0xFFFFFFFF
    #define NII_MAX_NUM_BONES 256

    #ifndef NII_NUM_RENDERTARGET_GROUPS
        #define NII_NUM_RENDERTARGET_GROUPS 10
        #define NII_DEFAULT_RT_GROUP 4
        #define NII_REND_TO_TEX_RT_GROUP 2
    #endif
    
    #define N_Mark(value, mark)         value |= (mark)
    #define N_NonMark(value, mark)      value &= ~(mark)
    #define N_MarkValue(value, mark)    (value) | (mark)
    #define N_NonMarkValue(value, mark) (value) & ~(mark)

    #define N_DMBFuncSign(api) extern "C" void api NiiDynModuleBegin(void) throw()
    #define N_DMEFuncSign(api) extern "C" void api NiiDynModuleEnd(void)

    #define GroupUnknow                 1
    #define GroupGlobal                 1
    #define GroupCustom                 2
    #define GroupDefault                3
    #define GroupInner                  4
    #define GroupSeparate               5
    #define GroupNii                    6

    #define Thread_Interrupt
    #define Thread_Signal
    #define Thread_Timeout

    #define N_UNICODE                   1
    #define N_ASCII                     2

    #define N_SpotShadowFade            555
    #ifndef N_Str_Code
        #define N_Str_Code              N_UNICODE
    #endif

    #define N_PrimaryLang               0
    #define N_Geo_Default_Level         100
    #define N_Max_Valid_ID              0x07000000
    #define N_Max_Inner_ID              0x07800000
    #define N_Max_Num_Value             0xFFFFFFFF

    #define N_Level_Bit_Escape          16				/// auto id = 前16位parent后16位child
    #define N_Com_Escape                32				/// auto id = 前16位parent后16位child
    #define N_Com_EscapeMark            0xFFFFFFFF                
    #define N_Max_Event_ID              0xffff
    #define N_Max_Property_ID           0xffff
    #define N_Inner_Property_ID         0x10000
    #define N_Custom_Property_ID        0xfffff

    #define N_Inner_State_ID            0x10000

    #define N_TEX_MIPMAP                10240
    #define N_Null_Obj                  0
    #define N_Parent_Obj                4294967295
	#define N_ChildID(pid, id)			(pid << N_Level_Bit_Escape) + id

    #define N_Only(mag)                 NII::mag##Manager::getOnly()
    #define N_OnlyPtr(mag)              NII::mag##Manager::getOnlyPtr()
    #define N_Engine()                  NII::Engine::getOnly()
    #define N_EnginePtr()               NII::Engine::getOnlyPtr()
    #define N_Timer()                   NII::Engine::getOnly().getTimer()

    #define N_MarkTrue(src, mark)                   ((src & mark) == mark)
    #define N_MarkOnlyTrue(src, flmark, mark)       ((src & flmark) == mark)
    #define N_MarkFalse(src, mark)                  ((src & mark) != mark)
    #define N_MarkOnlyFalse(src, flmark, mark)      ((src & flmark) != mark)

    #define N_SetMark(src, mark)                    src |= mark
    #define N_SetMarkOnly(src, flmark, mark)        src ~= flmark;src |= mark

    // param lastest only for 16bit
    #define N_64_16_0(data, num)        ((data & 0xFFFFFFFFFFFF0000 ) | (Nui64)num)
    #define N_64_16_1(data, num)        ((data & 0xFFFFFFFF0000FFFF ) | (Nui64)num << 16)
    #define N_64_16_2(data, num)        ((data & 0xFFFF0000FFFFFFFF ) | (Nui64)num << 32)
    #define N_64_16_3(data, num)        ((data & 0xFFFFFFFFFFFF ) | (Nui64)num << 48)
    #define N_64_16(data, idx, num)     ((data & N64_16NotMark[idx] ) | (Nui64)num << (idx << 4))
}

#endif