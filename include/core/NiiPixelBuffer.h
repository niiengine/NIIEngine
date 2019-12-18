/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-6-22

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
#ifndef _NII_PIXELBUFFER_H_
#define _NII_PIXELBUFFER_H_

#include "NiiPreInclude.h"
#include "NiiPlaneSize.h"
#include "NiiRect.h"

namespace NII
{
    /** ���ػ���
    @remark
        ���ظ������/Ƭ���ӵ�.���ص�ʵ����ȾЧ�����������ŷ�ʽ,ʵ����Ⱦ��С�Ȳ���
        ȷ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBuffer : public ResourceAlloc
    {
        friend class GeometryPixel;
    public:
        PixelBuffer(PixelBufferID pxid, ResourceID gid);
		PixelBuffer(PixelBufferID pxid, Texture * obj, const Rectf & area, const Vector2f & oft);
        PixelBuffer(PixelBufferID pxid, Texture * obj, const Rectf & area,
            const Vector2f & oft, ScaleMode mode, const PlaneSizef & srcRes);

        ~PixelBuffer();

        /** ��ȡID
        @version NIIEngine 3.0.0
        */
        PixelBufferID getID() const;

        /** ��ȡȺ��
        @version NIIEngine 3.0.0
        */
        ResourceID getGroupID() const;

        /** ����������Դ
        @param[in] obj ����
        @version NIIEngine 3.0.0
        */
        void setTexture(Texture * obj);

        /** ������������
        @param[in] area ������(���ؼ�)
        @version NIIEngine 3.0.0
        */
        void setArea(const Rectf & area);

        /** ��������ģʽ
        @param[in] mode ģʽ
        @version NIIEngine 3.0.0
        */
        void setMode(ScaleMode mode);

        /** ��ȡ��Դԭ�ֱ���
        @note (���ؼ�)
        @version NIIEngine 3.0.0
        */
        void setNativeRes(const PlaneSizef & area);

        /** ������Ⱦƫ��
        @param[in] oft ����λ��ƫ��(���ؼ�)
        @version NIIEngine 3.0.0
        */
        void setPixelOffset(const Vector2f & oft);

        /** ��ȡʵ�ʳߴ�
        @return ʵ����Ⱦ�ߴ�(��Ļ��)
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getSize() const;

        /** ��ȡʵ��ƫ��
        @return ʵ����Ⱦƫ��(��Ļ��)
        @version NIIEngine 3.0.0
        */
        const Vector2f & getOffset() const;

        /** ȷ����ͼ��ʵ�ʴ�С
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizef & size);

        /** �������ű���
        @param[in] out ���ű���
        @version NIIEngine 3.0.0
        */
        static void calc(ScaleMode mode, const PlaneSizef & dest, const PlaneSizef & src, Vector2f & out);
    protected:
        PixelBufferID mID;
        Texture * mTexture;     // �ϲ� ͳһ
        ScaleMode mMode;        // �ϲ� ͳһ
        Rectf mArea;
        Vector2f mOffset;
        Vector2f mPixelOffset;  // ��ʱ
        PlaneSizef mNativeRes;  // �ϲ� ��ʱ
        PlaneSizef mSize;		// ��ʱ
    };
}
#endif