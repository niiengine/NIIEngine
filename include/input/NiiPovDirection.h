/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

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

#ifndef _NII_POV_DIRECTION_H_
#define _NII_POV_DIRECTION_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_MEDIA
{
    enum PovType
    {
        PT_Center    = 0,
        PT_North     = 0x01,
        PT_South     = 0x02,
        PT_East      = 0x04,
        PT_NorthEast = 0x05,
        PT_SouthEast = 0x06,
        PT_West      = 0x08,
        PT_NorthWest = 0x09,
        PT_SouthWest = 0x0A
    };
}
}
#endif
