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

#ifndef _NII_MeshFileFormat_H__
#define _NII_MeshFileFormat_H__

#include "NiiPreInclude.h"

namespace NII
{
    /** 定义NII .mesh文件格式 .mesh 文件是二进制文件(在运行时读取效率高),还有安排
        成块数据,非常像3D Studio的格式一个块总包含:

            Nui16 CHUNK_ID : 下面的块IDList,识别块
            Nul LENGTH     : 块以字节为单位的长度,包含这个头
            void * DATA    : 数据,可能包含其他 子-块 (各种数据类型)

        一个.mesh 文件可以同时包含网格自己的定义和可选的材质使用定义(虽然这些可以省
        略,如果网格假定在运行时材质名字在网格中被提及到,从另一个资源中 加载/创建)

        一个.mesh文件只包含一个单一网格,其自身也可以有多个子网格
    */

    enum MeshChunkID
    {
        M_HEADER              = 0x1000,
            // NIIb*          version           : Version number check
        M_MESH                = 0x3000,
            // bool skeletallyAnimated   // important flag which affects h/w buffer policies
            // Optional M_GEOMETRY chunk
            M_SUBMESH             = 0x4000,
                // NIIb* materialName
                // bool mSelfVertex
                // Nui32 indexCount
                // bool indexes32Bit
                // Nui32* faceVertexIndices (indexCount)
                // OR
                // Nui16* faceVertexIndices (indexCount)
                // M_GEOMETRY chunk (Optional: present only if mSelfVertex = true)
                M_SUBMESH_OPERATION = 0x4010, // optional, trilist assumed if missing
                    // Nui16 mType
                M_SUBMESH_BONE_ASSIGNMENT = 0x4100,
                    // Optional bone weights (repeating section)
                    // Nui32 vertexIndex;
                    // Nui16 boneIndex;
                    // NIIf weight;
                // Optional chunk that matches a texture name to an alias
                // a texture alias is sent to the submesh material to use this texture name
                // instead of the one in the texture unit with a matching alias name
                M_SUBMESH_TEXTURE_ALIAS = 0x4200, // Repeating section
                    // NIIb* aliasName;
                    // NIIb* textureName;

            M_GEOMETRY          = 0x5000, // NB this chunk is embedded within M_MESH and M_SUBMESH
                // Nui32 vertexCount
                M_GEOMETRY_VERTEX_DECLARATION = 0x5100,
                    M_GEOMETRY_VERTEX_ELEMENT = 0x5110, // Repeating section
                        // Nui16 source;      // buffer bind source
                        // Nui16 type;        // DataType
                        // Nui16 semantic; // VertexType
                        // Nui16 offset;    // start offset in buffer in bytes
                        // Nui16 index;    // index of the semantic (for colours and texture coords)
                M_GEOMETRY_VERTEX_BUFFER = 0x5200, // Repeating section
                    // Nui16 bindIndex;    // Index to bind this buffer to
                    // Nui16 vertexSize;    // Per-vertex size, must agree with declaration at this index
                    M_GEOMETRY_VERTEX_BUFFER_DATA = 0x5210,
                        // raw buffer data
            M_MESH_SKELETON_LINK = 0x6000,
                // Optional link to skeleton
                // NIIb* skeletonName           : name of .skeleton to use
            M_MESH_BONE_ASSIGNMENT = 0x7000,
                // Optional bone weights (repeating section)
                // Nui32 vertexIndex;
                // Nui16 boneIndex;
                // NIIf weight;
            M_MESH_LOD = 0x8000,
                // Optional LOD information
                // string strategyName;
                // Nui16 numLevels;
                // bool manual;  (true for manual alternate meshes, false for generated)
                M_MESH_LOD_USAGE = 0x8100,
                // Repeating section, ordered in increasing depth
                // NB LOD 0 (full detail from 0 depth) is omitted
                // LOD value - this is a distance, a pixel count etc, based on strategy
                // NIIf lodValue;
                    M_MESH_LOD_MANUAL = 0x8110,
                    // Required if M_MESH_LOD section manual = true
                    // Resource mRid;
                    M_MESH_LOD_GENERATED = 0x8120,
                    // Required if M_MESH_LOD section manual = false
                    // Repeating section (1 per submesh)
                    // Nui32 indexCount;
                    // bool indexes32Bit
                    // Nui16* faceIndexes;  (indexCount)
                    // OR
                    // Nui32* faceIndexes;  (indexCount)
            M_MESH_BOUNDS = 0x9000,
                // NIIf minx, miny, minz
                // NIIf maxx, maxy, maxz
                // NIIf radius

            // Optional chunk which stores precomputed edge data
            M_EDGE_LISTS = 0xB000,
                // Each LOD has a separate edge list
                M_EDGE_LIST_LOD = 0xB100,
                    // Nui16 lodIndex
                    // bool isManual            // If manual, no edge data here, loaded from manual mesh
                        // bool mCloseUp
                        // Nul numTriangles
                        // Nul numEdgeGroups
                        // Triangle* triangleList
                            // Nul mIndexDataIndex
                            // Nul vertexSet
                            // Nul mVertIndex[3]
                            // Nul mLocalVertIndex[3]
                            // NIIf normal[4]

                        M_EDGE_GROUP = 0xB110,
                            // Nul mVertexDataIndex
                            // Nul mOffset
                            // Nul mCount
                            // Nul numEdges
                            // Edge* edgeList
                                // Nul  mTriIndex[2]
                                // Nul  mVertIndex[2]
                                // Nul  mLocalVertIndex[2]
                                // bool mAlone

            // Optional poses section, referred to by pose keyframes
            M_POSES = 0xC000,
                M_POSE = 0xC100,
                    // NIIb* name (may be blank)
                    // Nui16 target    // 0 for shared geometry,
                                                // 1+ for submesh index + 1
                    // bool includesNormals [1.8+]
                    M_POSE_VERTEX = 0xC111,
                        // Nul vertexIndex
                        // NIIf xoffset, yoffset, zoffset
                        // NIIf xnormal, ynormal, znormal (optional, 1.8+)
            // Optional vertex animation chunk
            M_ANIMATIONS = 0xD000,
                M_ANIMATION = 0xD100,
                // NIIb* name
                // NIIf length
                M_ANIMATION_BASEINFO = 0xD105,
                // [Optional] base keyframe information (pose animation only)
                // NIIb* baseAnimationName (blank for self)
                // NIIf baseKeyFrameTime
                M_ANIMATION_TRACK = 0xD110,
                    // Nui16 type            // 1 == morph, 2 == pose
                    // Nui16 target        // 0 for shared geometry,
                                                    // 1+ for submesh index + 1
                    M_ANIMATION_MORPH_KEYFRAME = 0xD111,
                        // NIIf time
                        // bool includesNormals [1.8+]
                        // NIIf x,y,z            // repeat by number of vertices in original geometry
                    M_ANIMATION_POSE_KEYFRAME = 0xD112,
                        // NIIf time
                        M_ANIMATION_POSE_REF = 0xD113, // repeat for number of referenced poses
                            // Nui16 mTargetIndex
                            // NIIf mWeight

            // Optional submesh extreme vertex list chink
            M_TABLE_EXTREMES = 0xE000,
            // Nui16 submesh_index;
            // NIIf extremes [n_extremes][3];

    /* Version 1.2 of the .mesh format (deprecated)
    enum MeshChunkID {
        M_HEADER                = 0x1000,
            // NIIb*          version           : Version number check
        M_MESH                = 0x3000,
            // bool skeletallyAnimated   // important flag which affects h/w buffer policies
            // Optional M_GEOMETRY chunk
            M_SUBMESH             = 0x4000,
                // NIIb* materialName
                // bool mSelfVertex
                // Nui32 indexCount
                // bool indexes32Bit
                // Nui32* faceVertexIndices (indexCount)
                // OR
                // Nui16* faceVertexIndices (indexCount)
                // M_GEOMETRY chunk (Optional: present only if mSelfVertex = true)
                M_SUBMESH_OPERATION = 0x4010, // optional, trilist assumed if missing
                    // Nui16 mType
                M_SUBMESH_BONE_ASSIGNMENT = 0x4100,
                    // Optional bone weights (repeating section)
                    // Nui32 vertexIndex;
                    // Nui16 boneIndex;
                    // NIIf weight;
            M_GEOMETRY          = 0x5000, // NB this chunk is embedded within M_MESH and M_SUBMESH
            */
                // Nui32 vertexCount
                // NIIf* pVertices (x, y, z order x numVertices)
                M_GEOMETRY_NORMALS = 0x5100,    //(Optional)
                    // NIIf* pNormals (x, y, z order x numVertices)
                M_GEOMETRY_COLOURS = 0x5200,    //(Optional)
                    // Nul* pColours (RGBA 8888 format x numVertices)
                M_GEOMETRY_TEXCOORDS = 0x5300,    //(Optional, REPEATABLE, each one adds an extra set)
                    // Nui16 dimensions    (1 for 1D, 2 for 2D, 3 for 3D)
                    // NIIf* pTexCoords  (u [v] [w] order, dimensions x numVertices)
            /*
            M_MESH_SKELETON_LINK = 0x6000,
                // Optional link to skeleton
                // NIIb* skeletonName           : name of .skeleton to use
            M_MESH_BONE_ASSIGNMENT = 0x7000,
                // Optional bone weights (repeating section)
                // Nui32 vertexIndex;
                // Nui16 boneIndex;
                // NIIf weight;
            M_MESH_LOD = 0x8000,
                // Optional LOD information
                // Nui16 numLevels;
                // bool manual;  (true for manual alternate meshes, false for generated)
                M_MESH_LOD_USAGE = 0x8100,
                // Repeating section, ordered in increasing depth
                // NB LOD 0 (full detail from 0 depth) is omitted
                // NIIf fromSquaredDepth;
                    M_MESH_LOD_MANUAL = 0x8110,
                    // Required if M_MESH_LOD section manual = true
                    // ResourceID mRid;
                    M_MESH_LOD_GENERATED = 0x8120,
                    // Required if M_MESH_LOD section manual = false
                    // Repeating section (1 per submesh)
                    // Nui32 indexCount;
                    // bool indexes32Bit
                    // Nui16* faceIndexes;  (indexCount)
                    // OR
                    // Nui32* faceIndexes;  (indexCount)
            M_MESH_BOUNDS = 0x9000
                // NIIf minx, miny, minz
                // NIIf maxx, maxy, maxz
                // NIIf radius
    */
    };
}
#endif