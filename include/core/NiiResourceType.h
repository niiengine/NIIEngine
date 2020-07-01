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

#ifndef _NII_RESOURCE_TYPE_H_
#define _NII_RESOURCE_TYPE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ��Դ����
    @remark ���ձ�
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
        RT_NII_Game_obj,        ///< ��������
        RT_NII_Game_war,        ///< ��������(��������Լ��)
        RT_NII_Game_ai,         ///< ��������/�����������ɢ�¼�
        RT_NII_Game_script,     ///< �߼�/����/��������Χע��
        RT_NII_Game_rule        ///< ��Ϊ����/����
    };
}
#endif