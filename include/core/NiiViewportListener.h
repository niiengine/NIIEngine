/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-7

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

#ifndef _NII_VIEWPORT_LISTENER_H_
#define _NII_VIEWPORT_LISTENER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �ӿڼ�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ViewportListener
    {
    public:
        virtual ~ViewportListener();

        /** �����ӿ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCreate(Viewport * obj);
        
        /** ɾ���ӿ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(Viewport * obj);

        /** ����������ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCameraChange(Viewport * obj, Camera * dst);

        /** �ӿ�����ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onAreaChange(Viewport * obj);
    };
    typedef vector<ViewportListener *>::type ViewportListenerList;
}
#endif