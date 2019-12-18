/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_BASIC_TYPES_
#define _NII_BASIC_TYPES_
namespace NII
{
    //////////////////////////////////
    typedef wchar_t             Nwchar;
    typedef char                Nchar;
    typedef char                NIIb;                             
    typedef short               NIIs;                             
    typedef int                 NIIi;                            
    typedef float               NIIf;                            
    typedef double              NIId;                           
    typedef long                NIIl;                          
    typedef unsigned char       NCountb;                     
    typedef unsigned short      NCounts;                     
    typedef unsigned int        NCounti;                    
    typedef unsigned long       Nul;                       
    typedef size_t              NCount;                   
    typedef unsigned int        NBitCount;               
    /////////////////////////////////
    typedef uint64              Nui64;            
    typedef unsigned int        Nui32;              
    typedef unsigned short      Nui16;           
    typedef unsigned char       Nui8;            
    typedef int64               Ni64;             
    typedef int                 Ni32;     
    typedef short               Ni16;       
    typedef char                Ni8;  
    /////////////////////////////////
    typedef long long           NBigi;       
    typedef unsigned long long  NBigui;    
    /////////////////////////////////
    typedef Ni32                Nid;
    typedef size_t              Nidx;
    typedef Nui32               Nmark;
    /////////////////////////////////
    typedef long long           Nll;  
    typedef unsigned long long  Null;
    /////////////////////////////////
    typedef char                Nutf8;
    typedef void *              NIIp;
    typedef const void *        NIIcp;
    typedef Nui32               CodePoint;
    typedef Nui32               CodecResult;
    typedef unsigned int        TimeDurS;     //时间级[能区分136.1年]
    typedef Nui64               TimeTotalS;   //时段级
    typedef Nui32               TimeDurMS;    //时段级[能区分24.8天]
    typedef Nui64               TimeTotalMS;  //时段级
    typedef Nui32               TimeDurUS;    //处理级[能区分35.7分钟]
    typedef Nui64               TimeTotalUS;  //时段级

    //以下的ID概念范围(0, N_Max_Valid_ID), 0被NIIEngine定义为无效的ID或是形式NULL对象
    typedef Nid ClassID;
    typedef Nid CodecID;
    typedef Nid XmlUnitID;
    typedef Nid SerializerID;
    typedef Nid ScriptUnitID;
    typedef Nid RequestID;
    typedef Nui32 PrcID;
    typedef Nid QueueID;
    typedef Nid ParentID;
    typedef Nid ChildID;
    typedef Nid PoseID;
    typedef Nid BoneID;
    typedef Nid MeshID;
    typedef Nid FontID;
    typedef Nid AnimationObjID;
    typedef Nid AnimationID;
    typedef Nid KeyFrameTrackID;
    typedef Nid SkeletonID;
    typedef Nid GpuProgramID;
    typedef Nid EventID;
    typedef Nid ResourceID;
    typedef Nid PatternID;
    typedef Nid GroupID;
    typedef Nid DataID;
    typedef Nid SchemeID;
    typedef Nid ObjID;
    typedef Nid TypeID;
    typedef Nid PropertyID;
    typedef Nid StateID;
    typedef Nid GameObjID;
    typedef Nid SpaceID;
    typedef Nid FactoryID;
    typedef Nid WarSceneID;
    typedef Nid LangID;
    typedef Nid ScriptTypeID;
    typedef Nid PluginID;
    typedef Nid ScriptModuleID;
    typedef Nid InstanceMeshID;
    typedef Nid PixelBufferID;
    typedef Nid OverlayID;
    typedef Nid OverlayGeoID;
    typedef Nid FrameObjID;
    typedef Nui32 OverlayGeoType;
    typedef Nid VersionID;
    typedef Nid ViewportID;
    typedef Nid EffectID;

    typedef Nui16 SerialVer;
    typedef Nui16 SerialFormat;

    #define NIIEngine using namespace NII
}

#endif