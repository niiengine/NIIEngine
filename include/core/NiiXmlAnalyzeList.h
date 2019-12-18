/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-1

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
///@warnning 不要手动修改此文件
///@warnning Do not modify this file manually
#ifndef _NII_XML_TAG_H_
#define _NII_XML_TAG_H_

#include "NiiPreInclude.h"
#include "NiiScriptHelperManager.h"

// XmlUnitID
#define N_XML_TAG_Material				ScriptHelperManager::UnitPropertyStart
#define N_XML_TAG_GpuProgramVP			(ScriptHelperManager::UnitPropertyStart + 1)
#define N_XML_TAG_GpuProgramFP			(ScriptHelperManager::UnitPropertyStart + 2)
#define N_XML_TAG_GpuProgramGP			(ScriptHelperManager::UnitPropertyStart + 3)
#define N_XML_TAG_GpuProgramParam		(ScriptHelperManager::UnitPropertyStart + 4)
#define N_XML_TAG_ParticleSpace			(ScriptHelperManager::UnitPropertyStart + 5)
#define N_XML_TAG_SpreadParticle		(ScriptHelperManager::UnitPropertyStart + 6)
#define N_XML_TAG_ParticleEffect		(ScriptHelperManager::UnitPropertyStart + 7)
#define N_XML_TAG_Compositor			(ScriptHelperManager::UnitPropertyStart + 8)

#define N_XML_TAG_Widget				(ScriptHelperManager::UnitPropertyStart + 9)	//"Widget"
#define N_XML_TAG_ViewPixel				(ScriptHelperManager::UnitPropertyStart + 10)	//"ViewPixel"
#define N_XML_TAG_AnimationCollector	(ScriptHelperManager::UnitPropertyStart + 11)	//"AnimationCollector"
#define N_XML_TAG_AnimationUnit			(ScriptHelperManager::UnitPropertyStart + 12)	//"Animation"
#define N_XML_TAG_FontUnit				(ScriptHelperManager::UnitPropertyStart + 13)	//"Font"
#define N_XML_TAG_UIConfig				(ScriptHelperManager::UnitPropertyStart + 14)	//"UIConfig"
#define N_XML_TAG_UILayout				(ScriptHelperManager::UnitPropertyStart + 15)	//"UILayout"
#define N_XML_TAG_UIScheme				(ScriptHelperManager::UnitPropertyStart + 16)	//"UIScheme"
#define N_XML_TAG_UIStyle				(ScriptHelperManager::UnitPropertyStart + 17)	//"UIStyle"

#endif