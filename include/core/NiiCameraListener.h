/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-11-7

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

#ifndef _NII_CAMERALISTENER_H_
#define _NII_CAMERALISTENER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CameraListener
    {
    public:
        CameraListener();
        virtual ~CameraListener();

        /** �ڴ���ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCreate(Camera * obj);

        /** ��ɾ��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(Camera * obj);

        /** ��Ⱦǰ����
        @version NIIEngine 3.0.0
        */
        virtual void onPreRender(Camera * obj);

        /** ��Ⱦ�����
        @version NIIEngine 3.0.0
        */
        virtual void onEndRender(Camera * obj);
    };
    typedef vector<CameraListener *>::type CameraListenerList;
}

#endif