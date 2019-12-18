/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-8

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

#ifndef _NII_TOUCH_CONTROL_ARGS_H_
#define _NII_TOUCH_CONTROL_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /** �����¼�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TouchControlArgs : public EventArgs
    {
    public:
        TouchControlArgs(const Vector3<NIIi> & p, const Vector3<NIIi> & r) :
            pos(p),
            rel(r) {}
        
        const Vector3<NIIi> & pos;  ///< ��ǰλ��
        const Vector3<NIIi> & rel;  ///< ��ǰλ�ú��ϴ�λ�õĲ�ֵ
    };
}
}

#endif