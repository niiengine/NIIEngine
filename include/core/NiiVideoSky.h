/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-5

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

#ifndef _NII_VIDEO_SKY_H_
#define _NII_VIDEO_SKY_H_

#include "NiiPreInclude.h"
#include "NiiSky.h"

namespace NII
{
    /** һ��ģ�⼫�����
    @remark һ����������ϵͳ�޷����,��Ч�ʺܲ������ϵͳ�Ĵ��淽ʽ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VideoSky : public Sky
    {
    public:
        VideoSky();
        VideoSky(const SpaceManager * gm);
        virtual ~VideoSky();

        /** ������Ƶ��Դ
        @version NIIEngine 3.0.0
        */
        void setVideo(ResourceID rid);

		/** ��ȡ��Ƶ��Դ
		@version NIIEngine 3.0.0
		*/
		ResourceID getVideo() const;

		/** ��ȡƽ��Ķ���
		@version NIIEngine 3.0.0
		*/
		const Plane & getPlane() const;

        /** ����ʱ������
        @version NIIEngine 3.0.0
        */
        void setTimeFactor(NIIf f);

        /** ��ȡʱ������
        @version NIIEngine 3.0.0
        */
        NIIf getTimeFactor() const;

		/** ��������
		@version NIIEngine 3.0.0
		*/
		void buildGeo(const Plane & plane, NIIf scale = 1000, NIIf width = 1000, NIIf height = 1000);
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
		Plane mPlane;
        NIIf mTimeFactor;
        Texture * mGeo;
    };
}
#endif