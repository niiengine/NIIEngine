/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-1

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/
///@warnning ��Ҫ�ֶ��޸Ĵ��ļ�
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