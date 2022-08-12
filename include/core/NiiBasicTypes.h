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

#ifndef _NII_BASIC_TYPES_
#define _NII_BASIC_TYPES_

#include <stdint.h>
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
    typedef unsigned char       NCount8, NCountb;                     
    typedef unsigned short      NCount16, NCounts;   
    typedef size_t              NCount;
    typedef unsigned int        Nui;
    typedef unsigned long       Nul;                                         
    /////////////////////////////////
    typedef uint64_t            Nui64;            
    typedef unsigned int        Nui32;              
    typedef unsigned short      Nui16;           
    typedef unsigned char       Nui8;            
    typedef int64_t             Ni64;             
    typedef int                 Ni32;     
    typedef short               Ni16;       
    typedef char                Ni8;   
    /////////////////////////////////
    typedef int                 Nid;
    typedef size_t              Nidx;
    typedef unsigned short      Nidx16;
    typedef uint64_t            Nmark;
    typedef uint64_t            Nmark64;
    /////////////////////////////////
    typedef char                Nutf8;
    typedef void *              Nvoid;
    typedef const void *        Ncvoid;
    typedef Nui32               CodePoint;
    typedef Nui32               CodecResult;
    typedef unsigned int        TimeDurS;     //时间级[能区分136.1年]
    typedef Nui64               TimeTotalS;   //时段级
    typedef Nui32               TimeDurMS;    //时段级[能区分24.8天]
    typedef Nui64               TimeTotalMS;  //时段级
    typedef Nui64               TimeDurUS;    //处理级
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
    typedef Nid BufferID;
    typedef Nid PoseID;
    typedef Nid BoneID;
    typedef Nid MeshID;
    typedef Nid FontID;
    typedef Nid AnimationObjID;
    typedef Nid AnimationID;
    typedef Nid KeyFrameTrackID;
    typedef Nid SkeletonID;
    typedef Nid GpuProgramID;
    typedef Nid ApuProgramID;
    typedef Nid EventID;
    typedef Nid ResourceID;
    typedef Nid PatternID;
    typedef Nid GroupID;
    typedef Nid GpuGroupID;
    typedef Nid DataID;
    typedef Nid SchemeID;
    typedef Nid ObjID;
    typedef Nid TypeID;
    typedef Nid PropertyID;
    typedef Nid StateID;
    typedef Nid GameObjID;
    typedef Nid SpaceID;
    typedef Nid FactoryID;
    typedef Nid StyleViewID;
    typedef Ni64 StyleViewComID;
    typedef Nid WarSceneID;
    typedef Nid LangID;
    typedef Nid ScriptTypeID;
    typedef Nid PluginID;
    typedef Nid ScriptModuleID;
    typedef Nid InstanceMeshID;
    typedef Nid PixelBufferID;
    typedef Ni64 PixelBufferComID;
    typedef Nui64 FrameID;
    typedef Nid OverlayID;
    typedef Nid OverlayGeoID;
    typedef Nid FrameObjID;
    typedef Nui32 OverlayGeoType;
    typedef Nid VersionID;
    typedef Nid ViewportID;
    typedef Nid EffectID;
    typedef Nid DrawCallGroupID;

    typedef Nui16 SerialVer;
    typedef Nui16 SerialFormat;

    typedef Nui8 PacketCH;
    typedef Nui8 SoundCH;
    #define NIIEngine using namespace NII
}

#endif