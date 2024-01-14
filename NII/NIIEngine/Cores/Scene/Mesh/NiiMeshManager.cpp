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
#include "NiiMeshManager.h"
#include "NiiPlane.h"
#include "NiiGBufferManager.h"
#include "NiiException.h"

namespace NII
{
    /** 保存由这个类用于(re)创建一个手动网格的参数
    @version NIIEngine 3.0.0
    */
    struct MeshPatchDefine
    {
        enum MeshBuildType
        {
            MBT_PLANE,
            MBT_CURVED_ILLUSION_PLANE,
            MBT_CURVED_PLANE
        };

        MeshBuildType type;
        Plane plane;
        NIIf width;
        NIIf height;
        NIIf curvature;
        NIIi xsegments;
        NIIi ysegments;
        Nui16 numTexCoordSets;
        NIIf xTile;
        NIIf yTile;
        Vector3f upVector;
        Quaternion orientation;
        Nmark mVertexBufMode;
        Nmark mIndexBufMode;
        bool normals;
        NIIi ySegmentsToKeep;
    };
    //-----------------------------------------------------------------------
    void tesselate2DMesh(SubMesh * sm, Nui16 meshWidth, Nui16 meshHeight,
        bool doubleSided = false, Nmark ibusage = Buffer::M_NORMAL )
    {
        // The mesh is built, just make a list of indexes to spit out the triangles
        Nui16 vInc, uInc, v, u, iterations;
        Nui16 vCount, uCount;

        if(doubleSided)
        {
            iterations = 2;
            vInc = 1;
            v = 0; // Start with front
        }
        else
        {
            iterations = 1;
            vInc = 1;
            v = 0;
        }

        // Allocate memory for faces
        // Num faces, width*height*2 (2 tris per square), index count is * 3 on top
        sm->getIndexData(0)->mCount = (meshWidth-1) * (meshHeight-1) * 2 * iterations * 3;
        N_Only(GBuffer).create(sm->getIndexData(0)->mBuffer, 16, sm->getIndexData(0)->mCount, ibusage);

        Nui16 v1, v2, v3;
        //bool firstTri = true;
        IndexBuffer * ibuf = sm->getIndexData(0)->mBuffer;
        // Lock the whole buffer
        Nui16 * pIndexes = static_cast<Nui16 *>(ibuf->lock(Buffer::MM_WHOLE));

        while(iterations--)
        {
            // Make tris in a zigzag pattern (compatible with strips)
            u = 0;
            uInc = 1; // Start with moving +u

            vCount = meshHeight - 1;
            while(vCount--)
            {
                uCount = meshWidth - 1;
                while (uCount--)
                {
                    // First Tri in cell
                    // -----------------
                    v1 = ((v + vInc) * meshWidth) + u;
                    v2 = (v * meshWidth) + u;
                    v3 = ((v + vInc) * meshWidth) + (u + uInc);
                    // Output indexes
                    *pIndexes++ = v1;
                    *pIndexes++ = v2;
                    *pIndexes++ = v3;
                    // Second Tri in cell
                    // ------------------
                    v1 = ((v + vInc) * meshWidth) + (u + uInc);
                    v2 = (v * meshWidth) + u;
                    v3 = (v * meshWidth) + (u + uInc);
                    // Output indexes
                    *pIndexes++ = v1;
                    *pIndexes++ = v2;
                    *pIndexes++ = v3;

                    // Next column
                    u += uInc;
                }
                // Next row
                v += vInc;
                u = 0;
            }
            // Reverse vInc for NIId sided
            v = meshHeight - 1;
            vInc = -vInc;
        }
        // Unlock
        ibuf->unlock();
    }
    //-----------------------------------------------------------------------
    void _MESH_manualplane(Mesh * pMesh, MeshPatchDefine * params)
    {
        if((params->xsegments + 1) * (params->ysegments + 1) > 65536)
            N_EXCEPT(InvalidParam, _I18n("Plane tesselation is too high, must generate max 65536 vertices"));
        SubMesh * pSub = pMesh->createSub();

        // Set up vertex data
        // Use a single shared buffer
        VertexData * vertexData;
        N_Only(GBuffer).create(vertexData);
        pMesh->setVertexData(vertexData);

        NCount currOffset = 0;
        // We always need positions
        vertexData->add(DT_Float3x, VT_Pos, 0, currOffset);
        currOffset += DataTypeUtil::getSize(DT_Float3x);
        // Optional normals
        if(params->normals)
        {
            vertexData->add(DT_Float3x, VT_Normals, 0, currOffset);
            currOffset += DataTypeUtil::getSize(DT_Float3x);
        }

        for(Nui16 i = 0; i < params->numTexCoordSets; ++i)
        {
            // Assumes 2D texture coords
            vertexData->add(DT_Float2x, VT_Tex_Coord, 0, currOffset, i);
            currOffset += DataTypeUtil::getSize(DT_Float2x);
        }

        vertexData->mCount = (params->xsegments + 1) * (params->ysegments + 1);

        // Allocate vertex buffer
        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, vertexData->getUnitSize(0), vertexData->mCount, params->mVertexBufMode);

        vertexData->attach(0, vbuf);

        // Work out the transform required
        // Default orientation of plane is normal along +z, distance 0
        Matrix4f xlate, xform, rot;
        xlate = rot = Matrix4f::IDENTITY;
        // Determine axes
        Vector3f zAxis, yAxis, xAxis;
        zAxis = params->plane.mNormal;
        zAxis.normalise();
        yAxis = params->upVector;
        yAxis.normalise();
        xAxis = yAxis.crossProduct(zAxis);
        if(xAxis.length() == 0)
        {
            //upVector must be wrong
            N_EXCEPT(InvalidParam, _I18n("The upVector you supplied is parallel to the plane normal, so is not valid."));
        }

        Matrix3f rot3(xAxis, yAxis, zAxis);
        rot = rot3;

        // Set up standard xform from origin
        xlate.setTrans(params->plane.mNormal * -params->plane.mD);

        // concatenate
        xform = xlate * rot;

        // Generate vertex data
        // Lock the whole buffer
        NIIf * pReal = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));
        NIIf xSpace = params->width / params->xsegments;
        NIIf ySpace = params->height / params->ysegments;
        NIIf halfWidth = params->width / 2;
        NIIf halfHeight = params->height / 2;
        NIIf xTex = (1.0f * params->xTile) / params->xsegments;
        NIIf yTex = (1.0f * params->yTile) / params->ysegments;
        Vector3f vec;
        Vector3f min = Vector3f::ZERO, max = Vector3f::UNIT;
        NIIf maxSquaredLength = 0;
        bool firstTime = true;

        for(NIIi y = 0; y < params->ysegments + 1; ++y)
        {
            for(NIIi x = 0; x < params->xsegments + 1; ++x)
            {
                // Work out centered on origin
                vec.x = (x * xSpace) - halfWidth;
                vec.y = (y * ySpace) - halfHeight;
                vec.z = 0.0f;
                // Transform by orientation and distance
                vec = xform.affine(vec);
                // Assign to geometry
                *pReal++ = vec.x;
                *pReal++ = vec.y;
                *pReal++ = vec.z;

                // Build bounds as we go
                if (firstTime)
                {
                    min = vec;
                    max = vec;
                    maxSquaredLength = vec.lengthSquared();
                    firstTime = false;
                }
                else
                {
                    min = min.floor(vec);
                    max = max.ceil(vec);
                    maxSquaredLength = std::max(maxSquaredLength, vec.lengthSquared());
                }

                if(params->normals)
                {
                    // Default normal is along unit Z
                    vec = Vector3f::Z;
                    // Rotate
                    vec = rot.affine(vec);

                    *pReal++ = vec.x;
                    *pReal++ = vec.y;
                    *pReal++ = vec.z;
                }

                for (NCount i = 0; i < params->numTexCoordSets; ++i)
                {
                    *pReal++ = x * xTex;
                    *pReal++ = 1 - (y * yTex);
                }


            } // x
        } // y

        // Unlock
        vbuf->unlock();
        // Generate face list
        pSub->setSelfVertex(false);
        tesselate2DMesh(pSub, params->xsegments + 1, params->ysegments + 1, false,
            params->mIndexBufMode);

        pMesh->setAABB(AABox(min, max));
        pMesh->setRangeFactor(0.01f);
        pMesh->setCenterRange(Math::sqrt(maxSquaredLength));
    }
    //-----------------------------------------------------------------------
    void _MESH_manualcurvedllusionplane(Mesh * pMesh, MeshPatchDefine * params)
    {
        if(params->ySegmentsToKeep == 0)
            params->ySegmentsToKeep = params->ysegments;

        if((params->xsegments + 1) * (params->ySegmentsToKeep + 1) > 65536)
            N_EXCEPT(InvalidParam, _I18n("Plane tesselation is too high, must generate max 65536 vertices"));
        SubMesh * pSub = pMesh->createSub();
        // Set up vertex data
        // Use a single shared buffer
        VertexData * vertexData;
        N_Only(GBuffer).create(vertexData);
        pMesh->setVertexData(vertexData);
        NCount currOffset = 0;
        // We always need positions
        vertexData->add(DT_Float3x, VT_Pos, 0, currOffset);
        currOffset += DataTypeUtil::getSize(DT_Float3x);
        // Optional normals
        if(params->normals)
        {
            vertexData->add(DT_Float3x, VT_Normals, 0, currOffset);
            currOffset += DataTypeUtil::getSize(DT_Float3x);
        }

        for(Nui16 i = 0; i < params->numTexCoordSets; ++i)
        {
            // Assumes 2D texture coords
            vertexData->add(DT_Float2x, VT_Tex_Coord, 0, currOffset, i);
            currOffset += DataTypeUtil::getSize(DT_Float2x);
        }

        vertexData->mCount = (params->xsegments + 1) * (params->ySegmentsToKeep + 1);

        // Allocate vertex buffer
        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, vertexData->getUnitSize(0), vertexData->mCount, params->mVertexBufMode);

        vertexData->attach(0, vbuf);

        // Work out the transform required
        // Default orientation of plane is normal along +z, distance 0
        Matrix4f xlate, xform, rot;
        xlate = rot = Matrix4f::IDENTITY;
        // Determine axes
        Vector3f zAxis, yAxis, xAxis;
        zAxis = params->plane.mNormal;
        zAxis.normalise();
        yAxis = params->upVector;
        yAxis.normalise();
        xAxis = yAxis.crossProduct(zAxis);
        if (xAxis.length() == 0)
        {
            //upVector must be wrong
            N_EXCEPT(InvalidParam,
                _I18n("The upVector you supplied is parallel to the plane normal, so is not valid."));
        }

        Matrix3f rot3(xAxis, yAxis, zAxis);
        rot = rot3;

        // Set up standard xform from origin
        xlate.setTrans(params->plane.mNormal * -params->plane.mD);

        // concatenate
        xform = xlate * rot;

        // Generate vertex data
        // Imagine a large sphere with the camera located near the top
        // The lower the curvature, the larger the sphere
        // Use the angle from viewer to the points on the plane
        // Credit to Aftershock for the general approach
        NIIf camPos;      // Camera position relative to sphere center

        // Derive sphere radius
        Vector3f vertPos;  // position relative to camera
        NIIf sphDist;      // Distance from camera to sphere along box vertex vector
        // Vector3 camToSph; // camera position to sphere
        NIIf sphereRadius;// Sphere radius
        // Actual values irrelevant, it's the relation between sphere radius and camera position that's important
        const NIIf SPHERE_RAD = 100.0;
        const NIIf CAM_DIST = 5.0;

        sphereRadius = SPHERE_RAD - params->curvature;
        camPos = sphereRadius - CAM_DIST;

        // Lock the whole buffer
        NIIf * pFloat = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));
        NIIf xSpace = params->width / params->xsegments;
        NIIf ySpace = params->height / params->ysegments;
        NIIf halfWidth = params->width / 2;
        NIIf halfHeight = params->height / 2;
        Vector3f vec, norm;
        Vector3f min = Vector3f::ZERO, max = Vector3f::UNIT;
        NIIf maxSquaredLength = 0;
        bool firstTime = true;

        for (NIIi y = params->ysegments - params->ySegmentsToKeep; y < params->ysegments + 1; ++y)
        {
            for (NIIi x = 0; x < params->xsegments + 1; ++x)
            {
                // Work out centered on origin
                vec.x = (x * xSpace) - halfWidth;
                vec.y = (y * ySpace) - halfHeight;
                vec.z = 0.0f;
                // Transform by orientation and distance
                vec = xform.affine(vec);
                // Assign to geometry
                *pFloat++ = vec.x;
                *pFloat++ = vec.y;
                *pFloat++ = vec.z;

                // Build bounds as we go
                if (firstTime)
                {
                    min = vec;
                    max = vec;
                    maxSquaredLength = vec.lengthSquared();
                    firstTime = false;
                }
                else
                {
                    min = min.floor(vec);
                    max = max.ceil(vec);
                    maxSquaredLength = std::max(maxSquaredLength, vec.lengthSquared());
                }

                if (params->normals)
                {
                    // Default normal is along unit Z
                    norm = Vector3f::Z;
                    // Rotate
                    norm = params->orientation * norm;

                    *pFloat++ = norm.x;
                    *pFloat++ = norm.y;
                    *pFloat++ = norm.z;
                }

                // Generate texture coords
                // Normalise position
                // modify by orientation to return +y up
                vec = params->orientation.inverse() * vec;
                vec.normalise();
                // Find distance to sphere
                sphDist = Math::sqrt(camPos*camPos * (vec.y*vec.y-1.0f) + sphereRadius*sphereRadius) - camPos*vec.y;

                vec.x *= sphDist;
                vec.z *= sphDist;

                // Use x and y on sphere as texture coordinates, tiled
                NIIf s = vec.x * (0.01f * params->xTile);
                NIIf t = 1.0f - (vec.z * (0.01f * params->yTile));
                for (NCount i = 0; i < params->numTexCoordSets; ++i)
                {
                    *pFloat++ = s;
                    *pFloat++ = t;
                }
            } // x
        } // y

        // Unlock
        vbuf->unlock();
        // Generate face list
        pSub->setSelfVertex(false);
        tesselate2DMesh(pSub, params->xsegments + 1, params->ySegmentsToKeep + 1, false,
            params->mIndexBufMode);

        pMesh->setAABB(AABox(min, max));
        pMesh->setRangeFactor(0.01f);
        pMesh->setCenterRange(Math::sqrt(maxSquaredLength));
    }
    //-----------------------------------------------------------------------
    void _MESH_manualcurvedplane(Mesh * pMesh, MeshPatchDefine * params)
    {
        if((params->xsegments + 1) * (params->ysegments + 1) > 65536)
            N_EXCEPT(InvalidParam, _I18n("Plane tesselation is too high, must generate max 65536 vertices"));
        SubMesh * pSub = pMesh->createSub();

        // Set options
        VertexData * vd;
        N_Only(GBuffer).create(vd);
        pMesh->setVertexData(vd);

        vd->mOffset = 0;
        vd->mCount = (params->xsegments + 1) * (params->ysegments + 1);

        NCount offset = 0;
        vd->add(DT_Float3x, VT_Pos, 0, offset);
        offset += DataTypeUtil::getSize(DT_Float3x);
        if(params->normals)
        {
            vd->add(DT_Float3x, VT_Normals, 0, 0);
            offset += DataTypeUtil::getSize(DT_Float3x);
        }

        for(Nui16 i = 0; i < params->numTexCoordSets; ++i)
        {
            vd->add(DT_Float2x, VT_Tex_Coord, 0, offset, i);
            offset += DataTypeUtil::getSize(DT_Float2x);
        }

        // Allocate memory
        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, offset, vd->mCount, params->mVertexBufMode);
        vd->attach(0, vbuf);

        // Work out the transform required
        // Default orientation of plane is normal along +z, distance 0
        Matrix4f xlate, xform, rot;
        xlate = rot = Matrix4f::IDENTITY;
        // Determine axes
        Vector3f zAxis, yAxis, xAxis;
        zAxis = params->plane.mNormal;
        zAxis.normalise();
        yAxis = params->upVector;
        yAxis.normalise();
        xAxis = yAxis.crossProduct(zAxis);
        if (xAxis.length() == 0)
        {
            //upVector must be wrong
            N_EXCEPT(InvalidParam,
                _I18n("The upVector you supplied is parallel to the plane normal, so is not valid."));
        }

        Matrix3f rot3(xAxis, yAxis, zAxis);
        rot = rot3;

        // Set up standard xform from origin
        xlate.setTrans(params->plane.mNormal * -params->plane.mD);

        // concatenate
        xform = xlate * rot;

        // Generate vertex data
        NIIf * pFloat = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));
        NIIf xSpace = params->width / params->xsegments;
        NIIf ySpace = params->height / params->ysegments;
        NIIf halfWidth = params->width / 2;
        NIIf halfHeight = params->height / 2;
        NIIf xTex = (1.0f * params->xTile) / params->xsegments;
        NIIf yTex = (1.0f * params->yTile) / params->ysegments;
        Vector3f vec;

        Vector3f min = Vector3f::ZERO, max = Vector3f::UNIT;
        NIIf maxSqLen = 0;
        bool first = true;

        NIIf diff_x, diff_y, dist;

        for (NIIi y = 0; y < params->ysegments + 1; ++y)
        {
            for (NIIi x = 0; x < params->xsegments + 1; ++x)
            {
                // Work out centered on origin
                vec.x = (x * xSpace) - halfWidth;
                vec.y = (y * ySpace) - halfHeight;

                // Here's where curved plane is different from standard plane.  Amazing, I know.
                diff_x = (x - ((params->xsegments) / 2)) / static_cast<NIIf>((params->xsegments));
                diff_y = (y - ((params->ysegments) / 2)) / static_cast<NIIf>((params->ysegments));
                dist = sqrt(diff_x*diff_x + diff_y * diff_y );
                vec.z = (-sin((1-dist) * (Math::PI/2)) * params->curvature) + params->curvature;

                // Transform by orientation and distance
                Vector3f pos = xform.affine(vec);
                // Assign to geometry
                *pFloat++ = pos.x;
                *pFloat++ = pos.y;
                *pFloat++ = pos.z;

                // Record bounds
                if (first)
                {
                    min = max = vec;
                    maxSqLen = vec.lengthSquared();
                    first = false;
                }
                else
                {
                    min = min.floor(vec);
                    max = max.ceil(vec);
                    maxSqLen = std::max(maxSqLen, vec.lengthSquared());
                }

                if (params->normals)
                {
                    // This part is kinda 'wrong' for curved planes... but curved planes are
                    //   very valuable outside sky planes, which don't typically need normals
                    //   so I'm not going to mess with it for now.

                    // Default normal is along unit Z
                    //vec = Vector3::Z;
                    // Rotate
                    vec = rot.affine(vec);
                    vec.normalise();

                    *pFloat++ = vec.x;
                    *pFloat++ = vec.y;
                    *pFloat++ = vec.z;
                }

                for (NCount i = 0; i < params->numTexCoordSets; ++i)
                {
                    *pFloat++ = x * xTex;
                    *pFloat++ = 1 - (y * yTex);
                }

            } // x
        } // y
        vbuf->unlock();

        // Generate face list
        tesselate2DMesh(pSub, params->xsegments + 1, params->ysegments + 1,
            false, params->mIndexBufMode);

        pMesh->setAABB(AABox(min, max));
        pMesh->setRangeFactor(0.01f);
        pMesh->setCenterRange(Math::sqrt(maxSqLen));
    }
    //-----------------------------------------------------------------------
    void _MESH_prefabplane(Mesh * mesh)
    {
        mesh->setAutoLod(false);
        SubMesh * sub = mesh->createSub();
        NIIf vertices[32] =
        {
            -100, -100, 0,  // pos
            0,0,1,          // normal
            0,1,            // texcoord

            100, -100, 0,
            0,0,1,
            1,1,

            100,  100, 0,
            0,0,1,
            1,0,

            -100,  100, 0 ,
            0,0,1,
            0,0
        };
        VertexData * decl;
        N_Only(GBuffer).create(decl);
        mesh->setVertexData(decl);

        decl->mCount = 4;
        NCount offset = 0;
        decl->add(DT_Float3x, VT_Pos, 0, offset);
        offset += DataTypeUtil::getSize(DT_Float3x);
        decl->add(DT_Float3x, VT_Normals, 0, offset);
        offset += DataTypeUtil::getSize(DT_Float3x);
        decl->add(DT_Float2x, VT_Tex_Coord, 0, offset, 0);
        offset += DataTypeUtil::getSize(DT_Float2x);

        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, offset, 4, Buffer::M_NORMAL );
        decl->attach(0, vbuf);

        vbuf->write(vertices, 0, vbuf->getSize());

        sub->setSelfVertex(false);
        IndexBuffer * ibuf;
        N_Only(GBuffer).create(ibuf, 16, 6, Buffer::M_NORMAL );

        Nui16 faces[6] = { 0, 1, 2, 0, 2, 3 };
        sub->getIndexData(0)->mBuffer = ibuf;
        sub->getIndexData(0)->mCount = 6;
        sub->getIndexData(0)->mOffset =0;
        ibuf->write(faces, 0, ibuf->getSize());

        mesh->setAABB(AABox(-100,-100,0,100,100,0));
        mesh->setRangeFactor(0.01f);
        mesh->setCenterRange(Math::sqrt(100 * 100 + 100 * 100));
    }
    //---------------------------------------------------------------------
    void _MESH_prefabcube(Mesh * mesh)
    {
        SubMesh* sub = mesh->createSub();

        const NIIi NUM_VERTICES = 4 * 6; // 4 vertices per side * 6 sides
        const NIIi NUM_ENTRIES_PER_VERTEX = 8;
        const NIIi NUM_VERTEX_ENTRIES = NUM_VERTICES * NUM_ENTRIES_PER_VERTEX;
        const NIIi NUM_INDICES = 3 * 2 * 6; // 3 indices per face * 2 faces per side * 6 sides

        const NIIf CUBE_SIZE = 100.0f;
        const NIIf CUBE_HALF_SIZE = CUBE_SIZE / 2.0f;

        // Create 4 vertices per side instead of 6 that are shared for the whole cube.
        // The reason for this is with only 6 vertices the normals will look bad
        // since each vertex can "point" in a different direction depending on the face it is included in.
        NIIf vertices[NUM_VERTEX_ENTRIES] = {
            // front side
            -CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE,    // pos
            0,0,1,    // normal
            0,1,    // texcoord
            CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            0,0,1,
            1,1,
            CUBE_HALF_SIZE,  CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            0,0,1,
            1,0,
            -CUBE_HALF_SIZE,  CUBE_HALF_SIZE, CUBE_HALF_SIZE ,
            0,0,1,
            0,0,

            // back side
            CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            0,0,-1,
            0,1,
            -CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            0,0,-1,
            1,1,
            -CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            0,0,-1,
            1,0,
            CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            0,0,-1,
            0,0,

            // left side
            -CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            -1,0,0,
            0,1,
            -CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            -1,0,0,
            1,1,
            -CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            -1,0,0,
            1,0,
            -CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            -1,0,0,
            0,0,

            // right side
            CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            1,0,0,
            0,1,
            CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            1,0,0,
            1,1,
            CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            1,0,0,
            1,0,
            CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            1,0,0,
            0,0,

            // up side
            -CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            0,1,0,
            0,1,
            CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            0,1,0,
            1,1,
            CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            0,1,0,
            1,0,
            -CUBE_HALF_SIZE, CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            0,1,0,
            0,0,

            // down side
            -CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            0,-1,0,
            0,1,
            CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            0,-1,0,
            1,1,
            CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            0,-1,0,
            1,0,
            -CUBE_HALF_SIZE, -CUBE_HALF_SIZE, CUBE_HALF_SIZE,
            0,-1,0,
            0,0
        };
        VertexData * vdmesh;
        N_Only(GBuffer).create(vdmesh);
        mesh->setVertexData(vdmesh);
        vdmesh->mCount = NUM_VERTICES;
        VertexData * decl = vdmesh;

        NCount offset = 0;
        decl->add(DT_Float3x, VT_Pos, 0, offset);
        offset += DataTypeUtil::getSize(DT_Float3x);
        decl->add(DT_Float3x, VT_Normals, 0, offset);
        offset += DataTypeUtil::getSize(DT_Float3x);
        decl->add(DT_Float2x, VT_Tex_Coord, 0, offset, 0);
        offset += DataTypeUtil::getSize(DT_Float2x);

        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, offset, NUM_VERTICES, Buffer::M_NORMAL );
        decl->attach(0, vbuf);

        vbuf->write(vertices, 0, vbuf->getSize());

        sub->setSelfVertex(false);
        IndexBuffer * ibuf;
        N_Only(GBuffer).create(ibuf, 16, NUM_INDICES, Buffer::M_NORMAL );

        Nui16 faces[NUM_INDICES] =
        {
            // front
            0,1,2,
            0,2,3,

            // back
            4,5,6,
            4,6,7,

            // left
            8,9,10,
            8,10,11,

            // right
            12,13,14,
            12,14,15,

            // up
            16,17,18,
            16,18,19,

            // down
            20,21,22,
            20,22,23
        };

        sub->getIndexData(0)->mBuffer = ibuf;
        sub->getIndexData(0)->mCount = NUM_INDICES;
        sub->getIndexData(0)->mOffset = 0;
        ibuf->write(faces, 0, ibuf->getSize());

        mesh->setAABB(AABox(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE,
            CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE));
        mesh->setRangeFactor(0.01f);

        mesh->setCenterRange(CUBE_HALF_SIZE);
    }
    //-----------------------------------------------------------------------
    void _MESH_prefabsphere(Mesh * mesh)
    {
        // sphere creation code taken from the DeferredShading sample, originally from the wiki
        SubMesh * pSphereVertex = mesh->createSub();

        const NIIi NUM_SEGMENTS = 16;
        const NIIi NUM_RINGS = 16;
        const NIIf SPHERE_RADIUS = 50.0;

        VertexData * vertexData;
        N_Only(GBuffer).create(vertexData);
        mesh->setVertexData(vertexData);

        // define the vertex format
        NCount currOffset = 0;
        // positions
        vertexData->add(DT_Float3x, VT_Pos, 0, currOffset);
        currOffset += DataTypeUtil::getSize(DT_Float3x);
        // normals
        vertexData->add(DT_Float3x, VT_Normals, 0, currOffset);
        currOffset += DataTypeUtil::getSize(DT_Float3x);
        // two dimensional texture coordinates
        vertexData->add(DT_Float2x, VT_Tex_Coord, 0, currOffset, 0);

        // allocate the vertex buffer
        vertexData->mCount = (NUM_RINGS + 1) * (NUM_SEGMENTS + 1);
        VertexBuffer * vBuf;
        N_Only(GBuffer).create(vBuf, vertexData->getUnitSize(0), vertexData->mCount, Buffer::M_NORMAL );

        vertexData->attach(0, vBuf);
        NIIf * pVertex = static_cast<NIIf *>(vBuf->lock(Buffer::MM_WHOLE));

        // allocate index buffer
        pSphereVertex->getIndexData(0)->mCount = 6 * NUM_RINGS * (NUM_SEGMENTS + 1);
        N_Only(GBuffer).create(pSphereVertex->getIndexData(0)->mBuffer,
            16, pSphereVertex->getIndexData(0)->mCount, Buffer::M_NORMAL );
        IndexBuffer * iBuf = pSphereVertex->getIndexData(0)->mBuffer;
        Nui16 * pIndices = static_cast<Nui16*>(iBuf->lock(Buffer::MM_WHOLE));

        NIIf fDeltaRingAngle = (Math::PI / NUM_RINGS);
        NIIf fDeltaSegAngle = (2 * Math::PI / NUM_SEGMENTS);
        Nui16 wVerticeIndex = 0 ;

        // Generate the group of rings for the sphere
        for(NIIi ring = 0; ring <= NUM_RINGS; ++ring)
        {
            NIIf r0 = SPHERE_RADIUS * sinf (ring * fDeltaRingAngle);
            NIIf y0 = SPHERE_RADIUS * cosf (ring * fDeltaRingAngle);

            // Generate the group of segments for the current ring
            for(NIIi seg = 0; seg <= NUM_SEGMENTS; ++seg)
            {
                NIIf x0 = r0 * sinf(seg * fDeltaSegAngle);
                NIIf z0 = r0 * cosf(seg * fDeltaSegAngle);

                // Add one vertex to the strip which makes up the sphere
                *pVertex++ = x0;
                *pVertex++ = y0;
                *pVertex++ = z0;

                Vector3f vNormal;
                Vector3f(x0, y0, z0).normalise(vNormal);
                *pVertex++ = vNormal.x;
                *pVertex++ = vNormal.y;
                *pVertex++ = vNormal.z;

                *pVertex++ = (NIIf) seg / (NIIf) NUM_SEGMENTS;
                *pVertex++ = (NIIf) ring / (NIIf) NUM_RINGS;

                if(ring != NUM_RINGS)
                {
                    // each vertex (except the last) has six indicies pointing to it
                    *pIndices++ = wVerticeIndex + NUM_SEGMENTS + 1;
                    *pIndices++ = wVerticeIndex;
                    *pIndices++ = wVerticeIndex + NUM_SEGMENTS;
                    *pIndices++ = wVerticeIndex + NUM_SEGMENTS + 1;
                    *pIndices++ = wVerticeIndex + 1;
                    *pIndices++ = wVerticeIndex;
                    wVerticeIndex ++;
                }
            }; // end for seg
        } // end for ring

        // Unlock
        vBuf->unlock();
        iBuf->unlock();
        // Generate face list
        pSphereVertex->setSelfVertex(false);

        // the original code was missing this line:
        mesh->setAABB(AABox(Vector3f(-SPHERE_RADIUS, -SPHERE_RADIUS, -SPHERE_RADIUS),
            Vector3f(SPHERE_RADIUS, SPHERE_RADIUS, SPHERE_RADIUS)));

        mesh->setCenterRange(SPHERE_RADIUS);
    }
    //-----------------------------------------------------------------------
    template<> MeshManager * Singleton<MeshManager>::mOnly = 0;
    /*MeshManager * MeshManager::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    MeshManager & MeshManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //-----------------------------------------------------------------------
    MeshManager::MeshManager()
    {
        mType = RT_Mesh;
        mScriptLevel = 350;
        N_Only(ResourceScheme).add(mType, this);
    }
    //-----------------------------------------------------------------------
    MeshManager::~MeshManager()
    {
        PatchDefineList::iterator i, iend = mPatchDefineList.end();
        for(i = mPatchDefineList.begin(); i != iend; ++i)
        {
            N_free(i->second);
        }
        mPatchDefineList.clear();
        N_Only(ResourceScheme).remove(mType);
    }
    //-----------------------------------------------------------------------
    void MeshManager::init()
    {
/*         // PrefabPlane
        MeshPtr msh = create("Prefab_Plane", GroupInner, this);
        // Planes can never be manifold
        msh->setAutoLod(false);
        // to preserve previous behaviour, load immediately
        msh->load();
        // PrefabCube
        MeshPtr msh = create("Prefab_Cube", GroupInner, this);
        // to preserve previous behaviour, load immediately
        msh->load();
        // PrefabSphere
        MeshPtr msh = create("Prefab_Sphere", GroupInner, this);
        // to preserve previous behaviour, load immediately
        msh->load(); */
    }
    //-----------------------------------------------------------------------
    MeshManager::TouchResult MeshManager::ref(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params,
            GeometryPrefab type, Nmark vbufmode, Nmark ibufmode)
    {
        TouchResult res = ResourceManager::ref(rid, gid, ls, rs, params);
        Mesh * pMesh = static_cast<Mesh *>(res.first);

        if(res.second)
        {
            pMesh->setVertexMode(vbufmode);
            pMesh->setIndexMode(ibufmode);
            if(type != GSP_None)
            {
                pMesh->setLoadScheme(this);
                pMesh->setPrefabType(type);
            }
        }
        return res;
    }
    //-----------------------------------------------------------------------
    Mesh * MeshManager::plan(ResourceID rid, GroupID gid, GeometryPrefab type,
        Nmark vbufmode, Nmark ibufmode)
    {
        Mesh * pMesh = static_cast<Mesh *>(ref(rid, gid, 0, 0, 0, type, vbufmode, ibufmode).first);
        pMesh->plan();
        return pMesh;
    }
    //-----------------------------------------------------------------------
    Mesh * MeshManager::load(ResourceID rid, GroupID gid, GeometryPrefab type,
        Nmark vbufmode, Nmark ibufmode)
    {
        Mesh * pMesh = static_cast<Mesh *>(ref(rid, gid, 0, 0, 0, type, vbufmode, ibufmode).first);
        pMesh->load();
        return pMesh;
    }
    //-----------------------------------------------------------------------
    Mesh * MeshManager::createManual(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs)
    {
        return static_cast<Mesh *>(create(rid, gid, ls, rs));
    }
    //-----------------------------------------------------------------------
    Mesh * MeshManager::createPlane(ResourceID rid, GroupID gid, const Plane & plane,
        NIIf width, NIIf height, NIIi xdetails, NIIi ydetails, bool normals,
        Nui16 texcoordCount, NIIf xTile, NIIf yTile, const Vector3f & upVector,
                Nmark vbusage, Nmark ibusage)
    {
        // Create manual mesh which calls back self to load
        Mesh * pMesh = createManual(rid, gid, this);
        // Planes can never be manifold
        pMesh->setAutoLod(false);
        // store parameters
        MeshPatchDefine * params = N_alloc_t(MeshPatchDefine, 1);
        params->type = MeshPatchDefine::MBT_PLANE;
        params->plane = plane;
        params->width = width;
        params->height = height;
        params->xsegments = xdetails;
        params->ysegments = ydetails;
        params->normals = normals;
        params->numTexCoordSets = texcoordCount;
        params->xTile = xTile;
        params->yTile = yTile;
        params->upVector = upVector;
        params->mVertexBufMode = vbusage;
        params->mIndexBufMode = ibusage;
        mPatchDefineList[pMesh] = params;

        // to preserve previous behaviour, load immediately
        pMesh->load();

        return pMesh;
    }
    //-------------------------------------------------------------------------
    Mesh * MeshManager::createCurved(ResourceID rid, GroupID gid, const Plane & plane, 
        NIIf width, NIIf height, NIIf curvature, NIIi xdetails, NIIi ydetails, bool normals, 
            Nui16 texcoordCount, NIIf xTile, NIIf yTile, const Vector3f & upVector,
                Nmark vbusage, Nmark ibusage)
    {
        // Create manual mesh which calls back self to load
        Mesh * pMesh = createManual(rid, gid, this);
        // Planes can never be manifold
        pMesh->setAutoLod(false);
        // store parameters
        MeshPatchDefine * params = N_alloc_t(MeshPatchDefine, 1);
        params->type = MeshPatchDefine::MBT_CURVED_PLANE;
        params->plane = plane;
        params->width = width;
        params->height = height;
        params->curvature = curvature;
        params->xsegments = xdetails;
        params->ysegments = ydetails;
        params->normals = normals;
        params->numTexCoordSets = texcoordCount;
        params->xTile = xTile;
        params->yTile = yTile;
        params->upVector = upVector;
        params->mVertexBufMode = vbusage;
        params->mIndexBufMode = ibusage;
        mPatchDefineList[pMesh] = params;

        // to preserve previous behaviour, load immediately
        pMesh->load();

        return pMesh;
    }
    //-----------------------------------------------------------------------
    Mesh * MeshManager::createCurvedIllusion(ResourceID rid, GroupID gid,
        const Plane & plane, NIIf width, NIIf height, NIIf curvature,
        NIIi xdetails, NIIi ydetails, NIIi yKeep, bool normals,
        Nui16 texcoordCount, NIIf uTile, NIIf vTile, const Vector3f & upVector,
        const Quaternion & orientation, Nmark vbusage, Nmark ibusage)
    {
        // Create manual mesh which calls back self to load
        Mesh * pMesh = createManual(rid, gid, this);
        // Planes can never be manifold
        pMesh->setAutoLod(false);
        // store parameters
        MeshPatchDefine * params = N_alloc_t(MeshPatchDefine, 1);
        params->type = MeshPatchDefine::MBT_CURVED_ILLUSION_PLANE;
        params->plane = plane;
        params->width = width;
        params->height = height;
        params->curvature = curvature;
        params->xsegments = xdetails;
        params->ysegments = ydetails;
        params->normals = normals;
        params->numTexCoordSets = texcoordCount;
        params->xTile = uTile;
        params->yTile = vTile;
        params->upVector = upVector;
        params->orientation = orientation;
        params->mVertexBufMode = vbusage;
        params->mIndexBufMode = ibusage;
        params->ySegmentsToKeep = yKeep;
        mPatchDefineList[pMesh] = params;

        // to preserve previous behaviour, load immediately
        pMesh->load();

        return pMesh;
    }
    //-------------------------------------------------------------------------
    bool MeshManager::buildPrefab(Mesh * mesh, GeometryPrefab type)
    {
        if(type == GSP_Plane)
        {
            _MESH_prefabplane(mesh);
            return true;
        }
        else if(type == GSP_Cube)
        {
            _MESH_prefabcube(mesh);
            return true;
        }
        else if(type == GSP_Sphere)
        {
            _MESH_prefabsphere(mesh);
            return true;
        }
        else if(type == GSP_Teapot)
        {
            return false;
        }
        else if(type == GSP_Founder)
        {
            return false;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    void MeshManager::load(Resource * res)
    {
        Mesh * msh = static_cast<Mesh *>(res);

        bool ok = buildPrefab(msh, msh->getPrefabType());

        if(!ok)
        {
            // Find build parameters
            PatchDefineList::iterator ibld = mPatchDefineList.find(res);
            if(ibld == mPatchDefineList.end())
            {
                N_EXCEPT(NotExist, _I18n("Cannot find build parameters for ") + res->getOriginID());
            }
            MeshPatchDefine * params = ibld->second;

            switch(params->type)
            {
            case MeshPatchDefine::MBT_PLANE:
                _MESH_manualplane(msh, params);
                break;
            case MeshPatchDefine::MBT_CURVED_ILLUSION_PLANE:
                _MESH_manualcurvedllusionplane(msh, params);
                break;
            case MeshPatchDefine::MBT_CURVED_PLANE:
                _MESH_manualcurvedplane(msh, params);
                break;
            default:
                N_EXCEPT(NotExist, _I18n("创建类型不明资源ID: ") + res->getOriginID());
            }
        }
    }
    //-----------------------------------------------------------------------
    Mesh * MeshManager::createBezierPatch(ResourceID rid, GroupID gid, 
        NCount width, NCount height, VertexData * vd,  NCount uMaxSubLevel,
            NCount vMaxSubLevel, FaceSideType type, Nmark vbmode, Nmark ibmode)
    {
        if(width < 3 || height < 3)
        {
            N_EXCEPT(InvalidParam, _I18n("Bezier patch require at least 3x3 control points"));
        }

        Mesh * test = static_cast<Mesh *>(get(rid));
        if(test)
            return 0;

        PatchMesh * pm = N_new PatchMesh(rid, gid, vd, vbmode, ibmode);
        pm->define(width, height, uMaxSubLevel, vMaxSubLevel, type);
        pm->load();
        addImpl(pm);

        return pm;
    }
    //-----------------------------------------------------------------------
    Resource * MeshManager::createImpl(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
        // no use for createParams here
        return N_new Mesh(rid, gid, ls, rs);
    }
    //-----------------------------------------------------------------------
}