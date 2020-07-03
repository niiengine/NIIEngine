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
#ifndef _NII_GeometryPixel_H_
#define _NII_GeometryPixel_H_

#include "NiiPreInclude.h"
#include "NiiRect.h"
#include "NiiTexture.h"
#include "NiiMatrix4.h"
#include "NiiGeometryRaw.h"
#include "NiiVertex.h"
#include "NiiColourArea.h"
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
        GL_Unknow = 5
    };

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
        inline void setBlendMode(PixelBlendMode set) { mMode = set; }

        /** ��ȡ���ػ��ģʽ
        @version NIIEngine 3.0.0
        */
        inline PixelBlendMode getBlendMode() const { return mMode;  }

        /** �����Ƿ�ü�
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setClip(bool b);

        /** ���òü�����
        @param[in] area ���βü�
        @version NIIEngine 3.0.0
        */
        void setClip(const Recti & area);

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
        inline NCount getVertexCount() const { return mVertexList.size();  }

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline NCount getBatchCount() const { return mBatchList.size();  }

        /** ���õ�ǰ����
        @param obj ����
        @note �����������ڴ�
        @version NIIEngine 3.0.0
        */
        inline void setTexture(Texture * obj) { mTexture = obj;  }

        /** ��ȡ��ǰ����
        @note �����������ڴ�
        @version NIIEngine 3.0.0
        */
        inline Texture * getTexture() const { return mTexture;  }

        /** ������ȾЧ��
        @version NIIEngine 3.0.0
        */
        inline void setEffect(RenderEffect * obj) { mEffect = obj;  }

        /** ��ȡ��ȾЧ��
        @version NIIEngine 3.0.0
        */
        inline RenderEffect * getEffect() const { return mEffect;  }
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
        RenderSys * mSys;
        Texture * mTexture;
        VertexBuffer * mBuffer;
        RenderEffect * mEffect;
        GeometryRaw mRenderOp;
        PixelBlendMode mMode;
        BatchList mBatchList;
        VertexList mVertexList;

        Recti mClipArea;
        Vector3f mOffset;
        Vector3f mOrigin;       ///< ԭ��
        Vector2f mSkinOffset;
        Quaternion mRot;

        Matrix4f mModelMatrix;   ///< ģ�;���
        Nmark mMark;
    };

    /** �������ض���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryQueue : public UIAlloc
    {
    public:
        /** �Ѽ������س�ˢ����Ⱦ����
        @version NIIEngine 3.0.0
        */
        void flush() const;

        /** ���
        @note ���಻�������һ���ڴ�
        @param[in] obj ��������
        @version NIIEngine 3.0.0
        */
        void add(GeometryPixel * obj);

        /** ��ȥ
        @note ���಻�������һ���ڴ�
        @param[in] obj ��������
        @version NIIEngine 3.0.0
        */
        void remove(GeometryPixel * obj);

        /** ��ȥ����
        @note ���ܼ������ض����ڴ�
        @version NIIEngine 3.0.0
        */
        void clear();
    private:
        typedef vector<GeometryPixel *>::type Buffers;
    private:
        Buffers mObjs;
    };
}
#endif