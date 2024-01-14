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

#include "NiiPreProcess.h"
#include "NiiGeometryPixel.h"
#include "NiiRenderSys.h"
#include "NiiGBufferManager.h"
#include "NiiUIManager.h"
#include "NiiQuaternion.h"
#include "NiiColour.h"
#include "NiiPixelBuffer.h"
#include "NiiRenderEffect.h"

#define _GP_Valid_Matrix 0x01
#define _GP_Valid_Buffer 0x02
#define _GP_mClip        0x04

namespace NII
{
    /// type to track info for per-texture sub batches of geometry
    struct GeometryPixelBatch
    {
        Texture * texture;
        Nui32 vertexCount;
        bool clip;
    };
    //------------------------------------------------------------------------
    static const TextureSampleUnit S_textureSample =
    {
        TFOP_LINEAR, TFOP_LINEAR, TFOP_POINT,
        TAM_CLAMP, TAM_CLAMP, TAM_CLAMP
    };
    //------------------------------------------------------------------------
    static const TextureBlend S_colourBlendMode =
    {
        TCT_RGB,
        TBO_MODULATE,
        TBS_TEXTURE,
        TBS_DIFFUSE
    };
    //------------------------------------------------------------------------
    static const TextureBlend S_alphaBlendMode =
    {
        TCT_AAA,
        TBO_MODULATE,
        TBS_TEXTURE,
        TBS_DIFFUSE
    };
    //------------------------------------------------------------------------
    static const TextureAddressing S_textureAddressMode =
    {
        TAM_CLAMP,
        TAM_CLAMP,
        TAM_CLAMP
    };
    //------------------------------------------------------------------------
    // Helper to allocate a vertex buffer and initialse a GeometryRaw
    static void _n_gdata_init(GeometryRaw & gr, VertexBuffer *& vb, NCount count)
    {
        // basic initialisation of render op
        N_Only(GBuffer).create(gr.mVertex);
        gr.mType = GeometryRaw::OT_TRIANGLE_LIST;
        gr.remove(GRT_Index);

        VertexData * vd = gr.mVertex;
        NCount vd_offset = 0;
        vd->add(DT_Float3x, VT_Pos, 0, vd_offset);
        vd_offset += DataTypeUtil::getSize(DT_Float3x);
        vd->add(DT_Colour, VT_Diffuse, 0, vd_offset);
        vd_offset += DataTypeUtil::getSize(DT_Colour);
        vd->add(DT_Float2x, VT_Tex_Coord, 0, vd_offset);

        N_Only(GBuffer).create(vb, vd->getUnitSize(0), count, Buffer::M_WRITE | Buffer::M_WHOLE);
        vb->setAutoDestroy(false);
        // bind vertex buffer
        gr.mVertex->attach(0, vb);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GeometryQueue
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    void GeometryQueue::flush() const
    {
        Buffers::const_iterator i, iend = mObjs.end();
        for (i = mObjs.begin(); i != iend; ++i)
        {
            (*i)->flush();
        }
    }
    //------------------------------------------------------------------------
    void GeometryQueue::add(GeometryPixel * obj)
    {
        mObjs.push_back(obj);
    }
    //------------------------------------------------------------------------
    void GeometryQueue::remove(GeometryPixel * obj)
    {
        Buffers::iterator i = std::find(mObjs.begin(), mObjs.end(), obj);
        if (i != mObjs.end())
        {
            mObjs.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void GeometryQueue::clear()
    {
        mObjs.clear();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GeometryLevelEventArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    GeometryLevelEventArgs::GeometryLevelEventArgs(GeometryLevel gl) :
        mGL(gl)
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GeometryPixel
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    GeometryPixel::GeometryPixel(RenderSys * rs) :
        mSys(rs),
        mTexture(0),
        mClipArea(0, 0, 0, 0),
        mOffset(0, 0, 0),
        mRot(Quaternion::IDENTITY),
        mOrigin(0, 0, 0),
        mEffect(0),
        mSkinOffset(rs->getTexelXOffset(), rs->getTexelYOffset())
    {
        _n_gdata_init(mRenderOp, mBuffer, 64);
        mMark |= _GP_mClip;
    }
    //------------------------------------------------------------------------
    GeometryPixel::GeometryPixel() :
        mMode(PBM_Normal)
    {
    }
    //------------------------------------------------------------------------
    GeometryPixel::~GeometryPixel()
    {
        N_Only(GBuffer).destroy(mRenderOp.mVertex);
        mRenderOp.mVertex = 0;
        N_Only(GBuffer).destroy(mBuffer);
        mBuffer = 0;
    }
    //------------------------------------------------------------------------
    void GeometryPixel::add(const Vertex * in, NCount count)
    {
        if(mBatchList.empty() || mBatchList.back()->texture != mTexture ||
            mBatchList.back()->clip != ((mMark & _GP_mClip) != 0))
        {
            GeometryPixelBatch * batch = N_alloc_t(GeometryPixelBatch, 1);
            batch->texture = mTexture;
            batch->vertexCount = 0;
            batch->clip = ((mMark & _GP_mClip) != 0);
            mBatchList.push_back(batch);
        }

        // update size of current batch
        mBatchList.back()->vertexCount += count;

        // buffer these vertices
        VertexData v;
        for(NCount i = 0; i < count; ++i)
        {
            const Vertex & vs = in[i];
            const Colour & col = vs.mColour;

            v.mPosX = vs.mPos.x + mSkinOffset.x;
            v.mPosY = vs.mPos.y + mSkinOffset.y;
            v.mPosZ = vs.mPos.z;

            Colour ocv(col.getRed(), col.getGreen(), col.getBlue(), col.getAlpha());
            mSys->convertType(ocv, &v.mDiffuse);
            v.mCoordU = vs.mCoord.x;
            v.mCoordV = vs.mCoord.y;

            mVertexList.push_back(v);
        }
        mMark &= ~_GP_Valid_Buffer;
    }
    //------------------------------------------------------------------------
    void GeometryPixel::add(Texture * tex, const Rectf & texarea, const Rectf & posarea, 
        const Rectf * posclip, const ColourArea & ca)
    {
        //QuadSplitMode quad_split_mode = TopLeftToBottomRight;

        // get the rect area that we will actually draw to (i.e. perform clipping)
        Rectf farea = posclip ? posarea.intersects(*posclip) : posarea;

        // Obtain correct scale values from the texture
        Vector2f scale = Vector2f(tex->getWidth(), tex->getHeight());
        Vector2f tex_per_pix(texarea.width() / posarea.width(), texarea.height() / posarea.height());

        // calculate final, clipped, texture co-ordinates
        const Rectf tarea((texarea.min() + ((farea.min() - posarea.min()) * tex_per_pix)) / scale,
            (texarea.max() + ((farea.max() - posarea.max()) * tex_per_pix)) / scale);

        // URGENT FIXME: Shouldn't this be in the hands of the user?
        farea.mLeft = N_ToPixel(farea.mLeft);
        farea.mTop = N_ToPixel(farea.mTop);
        farea.mRight = N_ToPixel(farea.mRight);
        farea.mBottom = N_ToPixel(farea.mBottom);

        if ((farea.width() == 0) || (farea.height() == 0))
            return;

        Vertex v[6];

        // vertex 0
        v[0].mPos = Vector3f(farea.left(), farea.top(), 0.0f);
        v[0].mColour = ca.mTL;
        v[0].mCoord = Vector2f(tarea.left(), tarea.top());

        // vertex 1
        v[1].mPos = Vector3f(farea.left(), farea.bottom(), 0.0f);
        v[1].mColour = ca.mBL;
        v[1].mCoord = Vector2f(tarea.left(), tarea.bottom());

        // vertex 2
        v[2].mPos.x = farea.right();
        v[2].mPos.z = 0.0f;
        v[2].mColour = ca.mBR;
        v[2].mCoord.x = tarea.right();

        // top-left to bottom-right diagonal
        //if(quad_split_mode == TopLeftToBottomRight)
        //{
        v[2].mPos.y = farea.bottom();
        v[2].mCoord.y = tarea.bottom();
        //}
        // bottom-left to top-right diagonal
        //else
        //{
        //    v[2].mPos.y = farea.top();
        //    v[2].mCoord.y = tarea.top();
        //}

        // vertex 3
        v[3].mPos = Vector3f(farea.right(), farea.top(), 0.0f);
        v[3].mColour = ca.mTR;
        v[3].mCoord = Vector2f(tarea.right(), tarea.top());

        // vertex 4
        v[4].mPos.x = farea.left();
        v[4].mPos.z = 0.0f;
        v[4].mColour = ca.mTL;
        v[4].mCoord.x = tarea.left();

        // top-left to bottom-right diagonal
        //if(quad_split_mode == TopLeftToBottomRight)
        //{
        v[4].mPos.y = farea.top();
        v[4].mCoord.y = tarea.top();
        //}
        // bottom-left to top-right diagonal
        //else
        //{
        //    v[4].mPos.y = farea.bottom();
        //    v[4].mCoord.y = tarea.bottom();
        //}

        // vertex 5
        v[5].mPos = Vector3f(farea.right(), farea.bottom(), 0.0f);
        v[5].mColour = ca.mBR;
        v[5].mCoord = Vector2f(tarea.right(), tarea.bottom());

        setTexture(tex);
        add(v, 6);
    }
    //------------------------------------------------------------------------
    void GeometryPixel::add(const PixelBuffer & in, const Rectf & posarea, 
        const Rectf * posclip, const ColourArea & ca)
    {
        add(in.getTexture(), in.mArea, posarea + in.getOffset(), posclip, ca);
    }
    //------------------------------------------------------------------------
    void GeometryPixel::flush()
    {
        if((mMark & _GP_Valid_Buffer) == 0)
        {
            // Reallocate h/w buffer as requied
            NCount size = mBuffer->getUnitCount();
            NCount required_size = mVertexList.size();
            if(size < required_size)
            {
                // calculate new size to use
                while(size < required_size)
                    size *= 2;

                N_Only(GBuffer).destroy(mRenderOp.mVertex);
                N_Only(GBuffer).destroy(mBuffer);
                mRenderOp.mVertex = 0;
                mBuffer = 0;

                _n_gdata_init(mRenderOp, mBuffer, size);
            }

            // copy vertex data into hw buffer
            if(required_size > 0)
            {
                std::memcpy(mBuffer->lock(NII::VertexBuffer::MM_WHOLE),
                    &mVertexList[0], sizeof(VertexData) * mVertexList.size());

                mBuffer->unlock();
            }

            mMark |= _GP_Valid_Buffer;
        }
        UIManager * mag = N_OnlyPtr(UI);
        mag->setModelMatrix(getMatrix());
        mag->apply(mMode);
        mag->syn();

        NCount pass, pass_count = mEffect ? mEffect->getPassCount() : 1;
        for(pass = 0; pass < pass_count; ++pass)
        {
            // set up RenderEffect
            if(mEffect)
                mEffect->onRenderBegin(pass);

            // draw the batches
            NCount pos = 0;
            BatchList::const_iterator i, iend = mBatchList.end();
            for(i = mBatchList.begin(); i != iend; ++i)
            {
                GeometryPixelBatch * batch = *i;
                // ²Ã¼ôÇøÓò
                mSys->setScissorTest(batch->clip, mClipArea.left(), mClipArea.top(),
                    mClipArea.right(), mClipArea.bottom());

                mRenderOp.mVertex->mOffset = pos;
                mRenderOp.mVertex->mCount = batch->vertexCount;

                mSys->_bindTexture(0, batch->texture, true);
                mSys->_setTextureCoordPrc(0, TCP_NONE);
                mSys->_setTextureCoordSet(0, 0);
                //mSys->_setTextureFilter(0, TFOP_LINEAR, TFOP_LINEAR, TFOP_POINT);
                //mSys->_setTextureAddressing(0, S_textureAddressMode);
                mSys->_setTextureSample(0, S_textureSample);
                mSys->_setTextureMatrix(0, Matrix4f::IDENTITY);
                mSys->setColour(ShaderChColour(CPM_ALWAYS_PASS, 0, false), SB_Alpha);
                mSys->_setTextureBlend(0, S_colourBlendMode);
                mSys->_setTextureBlend(0, S_alphaBlendMode);
                mSys->_enableTexture(1);
                mSys->_render(mRenderOp);

                pos += batch->vertexCount;
            }
        }

        // clean up RenderEffect
        if(mEffect)
            mEffect->onRenderEnd();
    }
    //------------------------------------------------------------------------
    void GeometryPixel::clear()
    {
        BatchList::iterator i, iend = mBatchList.end();
        for(i = mBatchList.begin(); i != iend; ++i)
        {
            N_free(*i);
        }
        mBatchList.clear();
        mVertexList.clear();
        mTexture = 0;
        mMark &= ~_GP_Valid_Buffer;
    }
    //------------------------------------------------------------------------
    void GeometryPixel::setClip(bool b)
    {
        if(b)
            mMark |= _GP_mClip;
        else
            mMark &= ~_GP_mClip;
    }
    //------------------------------------------------------------------------
    bool GeometryPixel::isClip() const
    {
        return mMark & _GP_mClip;
    }
    //------------------------------------------------------------------------
    void GeometryPixel::setClip(const Recti & area)
    {
        mClipArea.setTop(N_MAX(Nui32, 0, area.top()));
        mClipArea.setBottom(N_MAX(Nui32, 0, area.bottom()));
        mClipArea.setLeft(N_MAX(Nui32, 0, area.left()));
        mClipArea.setRight(N_MAX(Nui32, 0, area.right()));
    }
    //------------------------------------------------------------------------
    void GeometryPixel::setOffset(const Vector3f & v)
    {
        mOffset = v;
        mMark &= ~_GP_Valid_Matrix;
    }
    //------------------------------------------------------------------------
    void GeometryPixel::setRotation(const Quaternion & r)
    {
        mRot = r;
        mMark &= ~_GP_Valid_Matrix;
    }
    //------------------------------------------------------------------------
    void GeometryPixel::setOrigin(const Vector3f & p)
    {
        mOrigin = p;
        mMark &= ~_GP_Valid_Matrix;
    }
    //------------------------------------------------------------------------
    const Matrix4f & GeometryPixel::getMatrix()
    {
        if((mMark & _GP_Valid_Matrix) == 0)
        {
            // translation to position geometry and offset to pivot point
            TranslationMatrix4 trans(mOffset.x + mOrigin.x, mOffset.y + mOrigin.y, mOffset.z + mOrigin.z);

            // rotation
            Matrix4f rot(mRot);

            // translation to remove rotation pivot offset
            TranslationMatrix4 inv_pivot_trans(-mOrigin.x, -mOrigin.y, -mOrigin.z);

            // calculate final matrix
            mModelMatrix = trans * rot * inv_pivot_trans;

            mMark |= _GP_Valid_Matrix;
        }
        return mModelMatrix;
    }
    //------------------------------------------------------------------------
}