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
#include "NiiCurvedSurface.h"
#include "NiiMeshManager.h"
#include "NiiException.h"
#include "NiiGBufferManager.h"
#include "NiiGpuBuffer.h"

#define LEVEL_WIDTH(lvl) ((1 << (lvl+1)) + 1)

namespace NII
{
    //-----------------------------------------------------------------------
    NCount findSubdivide(Vector3f & a, Vector3f & b, Vector3f & c)
    {
        // Derived from work by Bart Sekura in rogl
        // Apart from I think I fixed a bug - see below
        // I also commented the code, the only thing wrong with rogl is almost no comments!!

        const NCount max_levels = 5;
        const NIIf subdiv = 10;
        NCount level;

        NIIf test = subdiv * subdiv;
        Vector3f s,t,d;
        for(level = 0; level < max_levels - 1; ++level)
        {
            // Subdivide the 2 lines
            s = a.middle(b);
            t = b.middle(c);
            // Find the midpoint between the 2 midpoints
            c = s.middle(t);
            // Get the vector between this subdivided midpoint and the middle point of the original line
            d = c - b;
            // Find the squared length, and break when small enough
            if(d.dotProduct(d) < test)
            {
                break;
            }
            b = a;
        }
        return level;
    }
    // TODO: make this deal with specular colours and more than 2 texture coords
    //-----------------------------------------------------------------------
    CurvedSurface::CurvedSurface():
        mDataRaw(0)
    {
        mType = T_Bezier;
    }
    //-----------------------------------------------------------------------
    CurvedSurface::~CurvedSurface()
    {
    }
    //-----------------------------------------------------------------------
    void CurvedSurface::define(VertexData * vd, NCount width, NCount height, 
        Type pType, NCount uMaxSubdivisionLevel, NCount vMaxSubdivisionLevel,
            FaceSideType visibleSide)
    {
        if(height == 0 || width == 0)
            return; // Do nothing - garbage

        mType = pType;
        mCtlWidth = width;
        mCtlHeight = height;
        mCtlCount = width * height;
        mData = vd;
        // Copy positions into Vector3 vector
        mCtlList.clear();
        const VertexUnit * elem = mData->get(VT_Pos);
        NCount vertSize = mData->getUnitSize(0);
        const Nui8 * vdata = (Nui8 *)mData->getAttach(0)->lock(Buffer::MM_READ);
        NIIf * pFloat;
        for(NCount i = 0; i < mCtlCount; ++i)
        {
            elem->getData(vdata, pFloat);
            mCtlList.push_back(Vector3f(pFloat[0], pFloat[1], pFloat[2]));
            vdata += vertSize;
        }
        mData->getAttach(0)->unlock();
        mVSide = visibleSide;

        // Determine max level
        // Initialise to 100% detail
        mFactor = 1.0f;
        if(uMaxSubdivisionLevel == (NCount)-1)
        {
            mULevel = mMaxULevel = getAutoULevel();
        }
        else
        {
            mULevel = mMaxULevel = uMaxSubdivisionLevel;
        }

        if (vMaxSubdivisionLevel == (NCount)-1)
        {
            mVLevel = mMaxVLevel = getAutoVLevel();
        }
        else
        {
            mVLevel = mMaxVLevel = vMaxSubdivisionLevel;
        }

        // Derive mesh width / height
        mMeshWidth  = (LEVEL_WIDTH(mMaxULevel) - 1) * ((mCtlWidth - 1) / 2) + 1;
        mMeshHeight = (LEVEL_WIDTH(mMaxVLevel) - 1) * ((mCtlHeight - 1) / 2) + 1;


        // Calculate number of required vertices / indexes at max resolution
        mVertexCount = mMeshWidth * mMeshHeight;
        //NIIi iterations = (mVSide == FST_Both)? 2 : 1;
        mIndexCount = (mMeshWidth - 1) * (mMeshHeight - 1) * 6 * ( (mVSide == FST_Both)? 2 : 1 );

        // Calculate bounds based on control points
        vector<Vector3f >::type::const_iterator ctli;
        Vector3f min = Vector3f::ZERO, max = Vector3f::UNIT;
        NIIf maxSqRadius = 0;
        bool first = true;
        for(ctli = mCtlList.begin(); ctli != mCtlList.end(); ++ctli)
        {
            if(first)
            {
                min = max = *ctli;
                maxSqRadius = ctli->lengthSquared();
                first = false;
            }
            else
            {
                min = min.floor(*ctli);
                max = max.ceil(*ctli);
                maxSqRadius = std::max(ctli->lengthSquared(), maxSqRadius);
            }
        }
        mAABB.setMin(min);
        mAABB.setMax(max);
        mBounding = Math::sqrt(maxSqRadius);
    }
    //-----------------------------------------------------------------------
    void CurvedSurface::build(VertexBuffer * destVertexBuffer, NCount vertexStart,
        IndexBuffer * destIndexBuffer, NCount indexStart)
    {
        if (mCtlList.empty())
            return;

        mVertexBuffer = destVertexBuffer;
        mVertexOffset = vertexStart;
        mIndexBuffer = destIndexBuffer;
        mIndexOffset = indexStart;

        // Lock just the region we are interested in
        mDataRaw = mVertexBuffer->lock(mVertexOffset * mData->getUnitSize(0), mVertexCount * mData->getUnitSize(0), Buffer::MM_WRITE);

        genControlPoint();

        // Subdivide the curve to the MAX :)
        // Do u direction first, so need to step over v levels not done yet
        NCount vStep = 1 << mMaxVLevel;
        NCount uStep = 1 << mMaxULevel;

        NCount v, u;
        for (v = 0; v < mMeshHeight; v += vStep)
        {
            // subdivide this row in u
            subdivideCurve(v*mMeshWidth, uStep, mMeshWidth / uStep, mULevel);
        }

        // Now subdivide in v direction, this time all the u direction points are there so no step
        for (u = 0; u < mMeshWidth; ++u)
        {
            subdivideCurve(u, vStep*mMeshWidth, mMeshHeight / vStep, mVLevel);
        }


        mVertexBuffer->unlock();

        // Make triangles from mesh at this current level of detail
        makeTriangles();
    }
    //-----------------------------------------------------------------------
    NCount CurvedSurface::getAutoULevel(bool forMax)
    {
        // determine levels
        // Derived from work by Bart Sekura in Rogl
        Vector3f a,b,c;
        NCount u,v;
        bool found = false;
        // Find u level
        for(v = 0; v < mCtlHeight; v++)
        {
            for(u = 0; u < mCtlWidth-1; u += 2)
            {
                a = mCtlList[v * mCtlWidth + u];
                b = mCtlList[v * mCtlWidth + u+1];
                c = mCtlList[v * mCtlWidth + u+2];
                if(a != c)
                {
                    found=true;
                    break;
                }
            }
            if(found) break;
        }
        if(!found)
        {
            N_EXCEPT(Internal, _I18n("Can't find suitable control points for determining U subdivision level"));
        }

        return findSubdivide(a,b,c);
    }
    //-----------------------------------------------------------------------
    NCount CurvedSurface::getAutoVLevel(bool forMax)
    {
        Vector3f a,b,c;
        NCount u,v;
        bool found=false;
        for(u = 0; u < mCtlWidth; ++u)
        {
            for(v = 0; v < mCtlHeight-1; v += 2)
            {
                a = mCtlList[v * mCtlWidth + u];
                b = mCtlList[(v+1) * mCtlWidth + u];
                c = mCtlList[(v+2) * mCtlWidth + u];
                if(a != c)
                {
                    found=true;
                    break;
                }
            }
            if(found)
                break;
        }
        if(!found)
        {
            N_EXCEPT(Internal,
                _I18n("Can't find suitable control points for determining V subdivision level"));
        }

        return findSubdivide(a,b,c);
    }
    //-----------------------------------------------------------------------
    void CurvedSurface::setSubdivisionFactor(NIIf factor)
    {
        N_assert(factor >= 0.0f && factor <= 1.0f, "error");

        mFactor = factor;
        mULevel = static_cast<NCount>(factor * mMaxULevel);
        mVLevel = static_cast<NCount>(factor * mMaxVLevel);

        makeTriangles();
    }
    /*
    //-----------------------------------------------------------------------
    void CurvedSurface::allocateMemory(void)
    {
        if(mMemoryAllocated)
            deallocateMemory();

        // Allocate to the size of max level

        // Create mesh
        mMesh = N_Only(Mesh).createManual(mMeshName);
        N_Only(GBuffer).create(mMesh->getVertexData());
        // Copy all vertex parameters
        mMesh->getVertexData()->mOffset = 0;
        // Vertex count will be set on build() because it depends on current level
        // NB clone the vertexdata because Mesh's VertexData will destroy it
        mMesh->getVertexData()->copy(mData);
        // Create buffer (only a single buffer)
        // Allocate enough buffer memory for maximum subdivision, not current subdivision
        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, mData->getUnitSize(0),
            mMaxMeshHeight * mMaxMeshWidth, // maximum size
                Buffer::M_DEVWRITE ); // dynamic for changing level

        // Set binding
        mMesh->getVertexData()->attach(0, vbuf);

        SubMesh * sm = mMesh->createSub();
        // Allocate enough index data for max subdivision
        sm->getIndexData(0)->mOffset = 0;
        // Index count will be set on build()
        Nui16 iterations = (mVSide == MeshManager::FST_Both ? 2 : 1);
        N_Only(GBuffer).create(sm->getIndexData(0)->mBuffer, 16,
            (mMaxMeshWidth-1) * (mMaxMeshHeight-1) * 6 * iterations, Buffer::M_WRITE);

        mMesh->load();

        // Derive bounds from control points, cannot stray outside that
        Vector3 min, max;
        NIIf maxSquaredRadius;
        bool first = true;
        vector<Vector3>::type::iterator i, iend = mCtlList.end();
        for (i = mCtlList.begin(); i != iend; ++i)
        {
            if (first)
            {
                min = max = *i;
                maxSquaredRadius = i->lengthSquared();
            }
            else
            {
                min = min.floor(*i);
                max = max.ceil(*i);
                maxSquaredRadius = std::max(maxSquaredRadius, i->lengthSquared());
            }

        }
        mMesh->setAABB(AABox(min, max));
        mMesh->setRangeFactor(0.01);
        mMesh->setCenterRange(Math::sqrt(maxSquaredRadius));
    }
    */
    //-----------------------------------------------------------------------
    void CurvedSurface::genControlPoint()
    {
        // Insert original control points into expanded mesh
        NCount uStep = 1 << mULevel;
        NCount vStep = 1 << mVLevel;

        NCount vertexSize = mData->getUnitSize(0);
        NIIf *pSrcReal, *pDestReal;
        RGBA *pSrcRGBA, *pDestRGBA;
        Nui8 * pDest;
        const VertexUnit * elemPos = mData->get(VT_Pos);
        const VertexUnit * elemNorm = mData->get(VT_Normals);
        const VertexUnit * elemTex0 = mData->get(VT_Tex_Coord, 0);
        const VertexUnit * elemTex1 = mData->get(VT_Tex_Coord, 1);
        const VertexUnit * elemDiffuse = mData->get(VT_Diffuse);
        Nui8 * pSrc = (Nui8 *)mData->getAttach(0)->lock(Buffer::MM_READ | Buffer::MM_WHOLE);
        for(NCount v = 0; v < mMeshHeight; v += vStep)
        {
            // set dest by v from base
            pDest = static_cast<Nui8 *>(mDataRaw) + (vertexSize * mMeshWidth * v);
            for(NCount u = 0; u < mMeshWidth; u += uStep)
            {
                // Copy Position
                elemPos->getData(pSrc, pSrcReal);
                elemPos->getData(pDest, pDestReal);
                *pDestReal++ = *pSrcReal++;
                *pDestReal++ = *pSrcReal++;
                *pDestReal++ = *pSrcReal++;

                // Copy Normals
                if(elemNorm)
                {
                    elemNorm->getData(pSrc, pSrcReal);
                    elemNorm->getData(pDest, pDestReal);
                    *pDestReal++ = *pSrcReal++;
                    *pDestReal++ = *pSrcReal++;
                    *pDestReal++ = *pSrcReal++;
                }

                // Copy Diffuse
                if(elemDiffuse)
                {
                    elemDiffuse->getData(pSrc, pSrcRGBA);
                    elemDiffuse->getData(pDest, pDestRGBA);
                    *pDestRGBA++ = *pSrcRGBA++;
                }

                // Copy texture coords
                if(elemTex0)
                {
                    elemTex0->getData(pSrc, pSrcReal);
                    elemTex0->getData(pDest, pDestReal);
                    for(NCount dim = 0; dim < DataTypeUtil::getCount(elemTex0->mType); ++dim)
                        *pDestReal++ = *pSrcReal++;
                }
                if(elemTex1)
                {
                    elemTex1->getData(pSrc, pSrcReal);
                    elemTex1->getData(pDest, pDestReal);
                    for(NCount dim = 0; dim < DataTypeUtil::getCount(elemTex1->mType); ++dim)
                        *pDestReal++ = *pSrcReal++;
                }

                // Increment source by one vertex
                pSrc = static_cast<Nui8 *>(pSrc) + vertexSize;
                // Increment dest by 1 vertex * uStep
                pDest = static_cast<Nui8 *>(pDest) + (vertexSize * uStep);
            } // u
        } // v
        mData->getAttach(0)->unlock();
    }
    //-----------------------------------------------------------------------
    void CurvedSurface::subdivideCurve(NCount startIdx, NCount stepSize, NCount numSteps, NCount iterations)
    {
        // Subdivides a curve within a sparsely populated buffer (gaps are already there to be interpolated into)
        NCount leftIdx, rightIdx, destIdx, halfStep, maxIdx;
        bool firstSegment;

        maxIdx = startIdx + (numSteps * stepSize);
        NCount step = stepSize;

        while(iterations--)
        {
            halfStep = step / 2;
            leftIdx = startIdx;
            destIdx = leftIdx + halfStep;
            rightIdx = leftIdx + step;
            firstSegment = true;
            while (leftIdx < maxIdx)
            {
                // Interpolate
                interpolateVertex(leftIdx, rightIdx, destIdx);

                // If 2nd or more segment, interpolate current left between current and last mid points
                if (!firstSegment)
                {
                    interpolateVertex(leftIdx - halfStep, leftIdx + halfStep, leftIdx);
                }
                // Next segment
                leftIdx = rightIdx;
                destIdx = leftIdx + halfStep;
                rightIdx = leftIdx + step;
                firstSegment = false;
            }

            step = halfStep;
        }
    }
    //-----------------------------------------------------------------------
    void CurvedSurface::makeTriangles(void)
    {
        // Our vertex buffer is subdivided to the highest level, we need to generate tris
        // which step over the vertices we don't need for this level of detail.

        // Calculate steps
        NIIi vStep = 1 << (mMaxVLevel - mVLevel);
        NIIi uStep = 1 << (mMaxULevel - mULevel);
        NCount currWidth = (LEVEL_WIDTH(mULevel)-1) * ((mCtlWidth-1)/2) + 1;
        NCount currHeight = (LEVEL_WIDTH(mVLevel)-1) * ((mCtlHeight-1)/2) + 1;

        bool use32bitindexes = (mIndexBuffer->getUnitSize() == 32);

        // The mesh is built, just make a list of indexes to spit out the triangles
        NIIi vInc, uInc;

        NCount vCount, uCount, v, u, iterations;

        if (mVSide == FST_Both)
        {
            iterations = 2;
            vInc = vStep;
            v = 0; // Start with front
        }
        else
        {
            iterations = 1;
            if (mVSide == FST_Front)
            {
                vInc = vStep;
                v = 0;
            }
            else
            {
                vInc = -vStep;
                v = mMeshHeight - 1;
            }
        }

        mFVertexCount = currWidth * currHeight;
        // Calc num indexes
        mFIndexCount = (currWidth - 1) * (currHeight - 1) * 6 * iterations;

        NCount v1, v2, v3;
        // Lock just the section of the buffer we need
        Nui16 * p16 = 0;
        Nui32 * p32 = 0;
        if (use32bitindexes)
        {
            p32 = static_cast<Nui32 *>(mIndexBuffer->lock(mIndexOffset * sizeof(Nui32), mIndexCount * sizeof(Nui32), Buffer::MM_WRITE));
        }
        else
        {
            p16 = static_cast<Nui16 *>(mIndexBuffer->lock(mIndexOffset * sizeof(Nui16), mIndexCount * sizeof(Nui16), Buffer::MM_WRITE));
        }

        while(iterations--)
        {
            // Make tris in a zigzag pattern (compatible with strips)
            u = 0;
            uInc = uStep; // Start with moving +u

            vCount = currHeight - 1;
            while (vCount--)
            {
                uCount = currWidth - 1;
                while (uCount--)
                {
                    // First Tri in cell
                    // -----------------
                    v1 = ((v + vInc) * mMeshWidth) + u;
                    v2 = (v * mMeshWidth) + u;
                    v3 = ((v + vInc) * mMeshWidth) + (u + uInc);
                    // Output indexes
                    if (use32bitindexes)
                    {
                        *p32++ = static_cast<Nui32>(v1);
                        *p32++ = static_cast<Nui32>(v2);
                        *p32++ = static_cast<Nui32>(v3);
                    }
                    else
                    {
                        *p16++ = static_cast<Nui16>(v1);
                        *p16++ = static_cast<Nui16>(v2);
                        *p16++ = static_cast<Nui16>(v3);
                    }
                    // Second Tri in cell
                    // ------------------
                    v1 = ((v + vInc) * mMeshWidth) + (u + uInc);
                    v2 = (v * mMeshWidth) + u;
                    v3 = (v * mMeshWidth) + (u + uInc);
                    // Output indexes
                    if (use32bitindexes)
                    {
                        *p32++ = static_cast<Nui32>(v1);
                        *p32++ = static_cast<Nui32>(v2);
                        *p32++ = static_cast<Nui32>(v3);
                    }
                    else
                    {
                        *p16++ = static_cast<Nui16>(v1);
                        *p16++ = static_cast<Nui16>(v2);
                        *p16++ = static_cast<Nui16>(v3);
                    }

                    // Next column
                    u += uInc;
                }
                // Next row
                v += vInc;
                u = 0;
            }
            // Reverse vInc for NIId sided
            v = mMeshHeight - 1;
            vInc = -vInc;
        }
        mIndexBuffer->unlock();
    }
    //-----------------------------------------------------------------------
    void CurvedSurface::interpolateVertex(NCount leftIdx, NCount rightIdx, NCount destIdx)
    {
        NCount vertexSize = mData->getUnitSize(0);
        const VertexUnit * elemPos = mData->get(VT_Pos);
        const VertexUnit * elemNorm = mData->get(VT_Normals);
        const VertexUnit * elemDiffuse = mData->get(VT_Diffuse);
        const VertexUnit * elemTex0 = mData->get(VT_Tex_Coord, 0);
        const VertexUnit * elemTex1 = mData->get(VT_Tex_Coord, 1);

        NIIf *pDestReal, *pLeftReal, *pRightReal;
        Nui8 *pDestChar, *pLeftChar, *pRightChar;
        Nui8 *pDest, *pLeft, *pRight;

        // Set up pointers & interpolate
        pDest = static_cast<Nui8*>(mDataRaw) + (vertexSize * destIdx);
        pLeft = static_cast<Nui8*>(mDataRaw) + (vertexSize * leftIdx);
        pRight = static_cast<Nui8*>(mDataRaw) + (vertexSize * rightIdx);

        // Position
        elemPos->getData(pDest, pDestReal);
        elemPos->getData(pLeft, pLeftReal);
        elemPos->getData(pRight, pRightReal);

        *pDestReal++ = (*pLeftReal++ + *pRightReal++) * 0.5f;
        *pDestReal++ = (*pLeftReal++ + *pRightReal++) * 0.5f;
        *pDestReal++ = (*pLeftReal++ + *pRightReal++) * 0.5f;

        if (elemNorm)
        {
            elemNorm->getData(pDest, pDestReal);
            elemNorm->getData(pLeft, pLeftReal);
            elemNorm->getData(pRight, pRightReal);
            Vector3f norm;
            norm.x = (*pLeftReal++ + *pRightReal++) * 0.5f;
            norm.y = (*pLeftReal++ + *pRightReal++) * 0.5f;
            norm.z = (*pLeftReal++ + *pRightReal++) * 0.5f;
            norm.normalise();

            *pDestReal++ = norm.x;
            *pDestReal++ = norm.y;
            *pDestReal++ = norm.z;
        }
        if (elemDiffuse)
        {
            // Blend each byte individually
            elemDiffuse->getData(pDest, pDestChar);
            elemDiffuse->getData(pLeft, pLeftChar);
            elemDiffuse->getData(pRight, pRightChar);
            // 4 bytes to RGBA
            *pDestChar++ = static_cast<Nui8>(((*pLeftChar++) + (*pRightChar++)) * 0.5);
            *pDestChar++ = static_cast<Nui8>(((*pLeftChar++) + (*pRightChar++)) * 0.5);
            *pDestChar++ = static_cast<Nui8>(((*pLeftChar++) + (*pRightChar++)) * 0.5);
            *pDestChar++ = static_cast<Nui8>(((*pLeftChar++) + (*pRightChar++)) * 0.5);
        }
        if (elemTex0)
        {
            elemTex0->getData(pDest, pDestReal);
            elemTex0->getData(pLeft, pLeftReal);
            elemTex0->getData(pRight, pRightReal);

            for (NCount dim = 0; dim < DataTypeUtil::getCount(elemTex0->mType); ++dim)
                *pDestReal++ = ((*pLeftReal++) + (*pRightReal++)) * 0.5f;
        }
        if (elemTex1)
        {
            elemTex1->getData(pDest, pDestReal);
            elemTex1->getData(pLeft, pLeftReal);
            elemTex1->getData(pRight, pRightReal);

            for (NCount dim = 0; dim < DataTypeUtil::getCount(elemTex1->mType); ++dim)
                *pDestReal++ = ((*pLeftReal++) + (*pRightReal++)) * 0.5f;
        }
    }
    //----------------------------------------------------------------------------------------------
}