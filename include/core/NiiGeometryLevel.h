/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-7

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

#ifndef _NII_GEOMETRYLEVEL_H_
#define _NII_GEOMETRYLEVEL_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"

namespace NII
{
    /** ����ͼ��
    @version NIIEngine 3.0.0
    */
    enum GeometryLevel
    {
        GL_Base = 0,        ///< NIIEngine ��Ⱦ����
        GL_Core = 1,        ///< NIIEngine ��Ⱦ���ƻ�����Ʒ
        GL_Overlay = 2,     ///< NIIEngine UI����
        GL_Tip = 3,         ///< NIIEngine UI������չ
        GL_Ext = 4,         ///< NIIEngine ��չ
        GL_Unknow =5
    };

    /** ����ͼ���¼�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryLevelEventArgs : public EventArgs
    {
    public:
        GeometryLevelEventArgs(GeometryLevel gl);

        GeometryLevel mGL;
    };
}
#endif