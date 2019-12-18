/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-2-24

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

#ifndef _NII_RESOURCE_TYPE_H_
#define _NII_RESOURCE_TYPE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 资源类型
    @remark 对照表
    @version NIIEngine 3.0.0
    */
    enum ResourceType
    {
        RT_Unknow = 0,
        RT_Font = 1,
        RT_Mesh = 2,
        RT_Skeleton = 3,
        RT_Material = 4,        
        RT_Image = 5,
        RT_Texture = 6,
        RT_Voice = 7,
        RT_Sound = 8,
        RT_Script = 9,
        RT_GameScript = 10,
        RT_Widget = 11,
        RT_GpuProgram = 12,
        RT_HighLevelGpuProgram = 13,
        RT_FrameFilter = 14,
        RT_Mesh_Ext_3ds,
        RT_Mesh_Ext_ac,
        RT_Mesh_Ext_ase,
        RT_Mesh_Ext_md2,
        RT_Mesh_Ext_md3,
        RT_Mesh_Ext_md5,
        RT_Mesh_Ext_b3d,
        RT_Mesh_Ext_bsd,
        RT_Mesh_Ext_bvh,
        RT_Mesh_Ext_cob,
        RT_Mesh_Ext_collada,
        RT_Mesh_Ext_20,
        RT_Mesh_Ext_19,
        RT_Mesh_Ext_18,
        RT_Mesh_Ext_17,
        RT_Mesh_Ext_16,
        RT_Mesh_Ext_15,
        RT_Mesh_Ext_14,
        RT_Mesh_Ext_13,
        RT_Mesh_Ext_12,
        RT_Mesh_Ext_11,
        RT_Mesh_Ext_10,
        RT_Mesh_Ext_9,
        RT_Mesh_Ext_8,
        RT_Mesh_Ext_7,
        RT_Mesh_Ext_6,
        RT_Mesh_Ext_5,
        RT_Mesh_Ext_4,
        RT_Mesh_Ext_3,
        RT_Mesh_Ext_2,
        RT_Mesh_Ext_1,
        RT_NII_Game_obj,        ///< 基础对象
        RT_NII_Game_war,        ///< 场景对象(包含物理约束)
        RT_NII_Game_ai,         ///< 基础对象/场景对象的扩散事件
        RT_NII_Game_script,     ///< 逻辑/设置/创建的外围注入
        RT_NII_Game_rule        ///< 行为规律/剧情
    };
}
#endif