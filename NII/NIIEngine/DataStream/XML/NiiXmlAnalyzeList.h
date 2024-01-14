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
///@warnning 不要手动修改此文件
///@warnning Do not modify this file manually
#ifndef _NII_XML_TAG_H_
#define _NII_XML_TAG_H_

#include "NiiPreInclude.h"
#include "NiiScriptHelperManager.h"

// XmlUnitID
#define N_XML_TAG_Material                ScriptHelperManager::UnitPropertyStart
#define N_XML_TAG_GpuProgramVP            (ScriptHelperManager::UnitPropertyStart + 1)
#define N_XML_TAG_GpuProgramFP            (ScriptHelperManager::UnitPropertyStart + 2)
#define N_XML_TAG_GpuProgramGP            (ScriptHelperManager::UnitPropertyStart + 3)
#define N_XML_TAG_GpuProgramParam        (ScriptHelperManager::UnitPropertyStart + 4)
#define N_XML_TAG_ParticleSpace            (ScriptHelperManager::UnitPropertyStart + 5)
#define N_XML_TAG_SpreadParticle        (ScriptHelperManager::UnitPropertyStart + 6)
#define N_XML_TAG_ParticleEffect        (ScriptHelperManager::UnitPropertyStart + 7)
#define N_XML_TAG_Compositor            (ScriptHelperManager::UnitPropertyStart + 8)

#define N_XML_TAG_Widget                (ScriptHelperManager::UnitPropertyStart + 9)    //"Widget"
#define N_XML_TAG_ViewPixel                (ScriptHelperManager::UnitPropertyStart + 10)    //"ViewPixel"
#define N_XML_TAG_AnimationCollector    (ScriptHelperManager::UnitPropertyStart + 11)    //"AnimationCollector"
#define N_XML_TAG_AnimationUnit            (ScriptHelperManager::UnitPropertyStart + 12)    //"Animation"
#define N_XML_TAG_FontUnit                (ScriptHelperManager::UnitPropertyStart + 13)    //"Font"
#define N_XML_TAG_UIConfig                (ScriptHelperManager::UnitPropertyStart + 14)    //"UIConfig"
#define N_XML_TAG_UILayout                (ScriptHelperManager::UnitPropertyStart + 15)    //"UILayout"
#define N_XML_TAG_UIScheme                (ScriptHelperManager::UnitPropertyStart + 16)    //"UIScheme"
#define N_XML_TAG_UIStyle                (ScriptHelperManager::UnitPropertyStart + 17)    //"UIStyle"

#define N_XML_TAG_Custom                (ScriptHelperManager::UnitPropertyStart + 100000)
#define N_XML_TAG_CustomMax             (ScriptHelperManager::UnitPropertyStart + 900000)

#endif