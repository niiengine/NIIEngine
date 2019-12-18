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
#ifndef _NII_GeometryPixel_H_
#define _NII_GeometryPixel_H_

#include "NiiPreInclude.h"
#include "NiiRect.h"
#include "NiiTexture.h"
#include "NiiMatrix4.h"
#include "NiiGeometryRaw.h"
#include "NiiVertex.h"
#include "NiiColourArea.h"

namespace NII
{
    /** ���ģʽ
    @version NIIEngine 3.0.0
    */
    enum PixelBlendMode
    {
        PBM_Normal,
        PBM_Average,
        PBM_Unknow,
    };

    class RenderEffect;
    struct GeometryPixelBatch;

    /** һ�����ϵĸ��ϻ���
    @remark
        ��������е� VertexBuffer / IndexBuffer / FrameBuffer �����͸�����Щ,ͨ
        �����־Ϳ��Է�����������ж��ֻ������,�������ֳ��˺����������.������Ӧ����
        �� GeometrySpace, GeometrySpace ��������Ӱ,��������,��������򲻰���.
    @note
        ������ʺ���ɢ��������/�ı���,�����ʴ��ģ�й��ɵ�������/�ı���,����ʹ�����
        ����������Ŀ���
    @par NIIEngine 3.0.0 ��3���汾��Geometry
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryPixel : public BufferAlloc
    {
    public:
        GeometryPixel(RenderSys * rs);

        ~GeometryPixel();

        /** ���
        @param in
        @param count
        @version NIIEngine 3.0.0
        */
        void add(const Vertex * in, NCount count = 1);

		/** ���
		@param[in] tex
		@param[in] texarea
		@param[in] posarea
		@param[in] posclip
		@param[in] ca
		@version NIIEngine 3.0.0
		*/
		void add(Texture * tex, const Rectf & texarea, const Rectf & posarea,
			const Rectf * posclip, const ColourArea & ca);

        /** ���
        @param[in] in
        @param[in] area
        @param[in] clip
        @param[in] ca
        @version NIIEngine 3.0.0
        */
        void add(const PixelBuffer & in, const Rectf & area, const Rectf * clip,
            const ColourArea & ca);

        /** ִ����Ⱦ
        @remark �� Opengl ��� glFlush ����������
        @version NIIEngine 3.0.0
        */
        void flush();

        /** ������Ⱦ
        @remark �� Opengl ��� glClear ����������
        @version NIIEngine 3.0.0
        */
        void clear();

        /** �������ػ��ģʽ
        @param m ���ģʽ
        @version NIIEngine 3.0.0
        */
        void setBlendMode(PixelBlendMode m);

        /** ��ȡ���ػ��ģʽ
        @version NIIEngine 3.0.0
        */
        PixelBlendMode getBlendMode() const;

        /** �����Ƿ�ü�
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setClip(bool b);

        /** ���òü�����
        @param[in] area ���βü�
        @version NIIEngine 3.0.0
        */
        void setClip(const Rectf & area);

        /** �Ƿ�ü�
        @return
        @version NIIEngine 3.0.0
        */
        bool isClip() const;

        /** λ��ƫ��
        @param v
        */
        void setOffset(const Vector3f & oft);

        /** λ����ת
        @param r
        */
        void setRotation(const Quaternion & r);

        /** ԭ��
        @remark ��ת����
        @param p ԭ��
        @version NIIEngine 3.0.0
        */
        void setOrigin(const Vector3f & pos);

        /** ��ȡ���վ���
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getMatrix();

        /** ��������
        @version NIIEngine 3.0.0
        */
        NCount getVertexCount() const;

        /** ��������
        @version NIIEngine 3.0.0
        */
        NCount getBatchCount() const;

        /** ���õ�ǰ����
        @param obj ����
        @note �����������ڴ�
        @version NIIEngine 3.0.0
        */
        void apply(Texture * obj);

        /** ��ȡ��ǰ����
        @note �����������ڴ�
        @version NIIEngine 3.0.0
        */
		Texture * getTexture() const;

        /** ������ȾЧ��
        @version NIIEngine 3.0.0
        */
        void apply(RenderEffect * obj);

        /** ��ȡ��ȾЧ��
        @version NIIEngine 3.0.0
        */
		RenderEffect * getEffect() const;
    protected:
        GeometryPixel();
    protected:
        struct VertexData
        {
            NIIf mPosX, mPosY, mPosZ;
            RGBA mDiffuse;          ///< opengl
            NIIf mCoordU, mCoordV;
        };
        typedef vector<VertexData>::type VertexList;
        typedef vector<GeometryPixelBatch *>::type BatchList;
    private:
        RenderSys * mSys;       ///< ��Ⱦϵͳ
        Texture * mTexture;     ///<
        VertexBuffer * mBuffer; ///<
        RenderEffect * mEffect; ///<
        GeometryRaw mRenderOp;  ///<
        PixelBlendMode mMode;   ///<
        BatchList mBatchList;   ///<
        VertexList mVertexList; ///<

        Rectf mClipArea;        ///<
        Vector3f mOffset;       ///<
        Vector3f mOrigin;       ///< ԭ��
        Vector2f mSkinOffset;   ///<
        Quaternion mRot;        ///<

        Matrix4f mModelMatrix;   ///< ģ�;���
        Nmark mMark;
    };
}
#endif