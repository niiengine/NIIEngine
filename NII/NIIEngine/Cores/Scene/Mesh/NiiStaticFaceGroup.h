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

#ifndef _NII_StaticFaceGroup_H__
#define _NII_StaticFaceGroup_H__

#include "NiiPreInclude.h"

#include "NiiPlane.h"
#include "NiiCurvedSurface.h"

namespace NII
{
    /** 一个面群组类型,即过程的中面列表 等 
    */
    enum FaceGroupType 
    {
        FGT_FACE_LIST,
        FGT_PATCH,
        FGT_UNKNOWN
    };

    /** 一组静态集,即在一起的不能移动的面,有共同的属性如它们使用的材质,位于
        同一平面
    @remark 
        Whilst for discrete geometry (i.e. movable objects) groups of faces are
        held in the SubMesh class, for immovable objects like scenery there
        needs to ba little more flexibility in the grouping since the group is
        likely to be a small part of a huge set of geometry. In addition, because
        the faces are unmoving certain optimisations can be performed, e.g.
        precalculating a world-coordinate bounding box and normal.
    @par
        Exactly how this class is used depends on the format of the large
        static geometry used in the level. An example would be the use of this
        class in the BspNode class for indoor levels.
        For flexibility and efficiency, it is not assumed that this class holds
        details of the vertices itself, or in fact that it holds the vertex indices
        itself. Everything is manipulated via pointers so if you want this
        class to point into a block of geometry data it can.
    */
    struct StaticFaceGroup 
    {
        // 面群组类型
        FaceGroupType fType;

        /// 这是天空面吗?
        bool isSky;

        /** Index into a buffer containing vertex definitions. Because we're
            dealing with subsets of large levels this is likely to be part-way
            through a huge vertex buffer. 
        */
        NIIi vertexStart;

        /** The range of vertices in the buffer this facegroup references.
            This is really for copying purposes only, so that we know which
            subset of vertices to copy from our large-level buffer into the rendering buffer.
        */
        NIIi numVertices;

        /** Index into a buffer containing vertex indices. This buffer
            may be individual to this group or shared for memory allocation
            efficiency.The vertex indexes are relative the the mVertexStart pointer,
            not to the start of the large-level buffer, allowing simple reindexing
            when copying data into rendering buffers.
            This is only applicable to FGT_FACE_LIST face group types.
        */
        NIIi elementStart;

        /** The number of vertex indices.
            This is only applicable to FGT_FACE_LIST face group types.
        */
        NIIi numElements;

        /** Handle to material used by this group.
            Note the use of the material handle rather than the material
            name - this is for efficiency since there will be many of these.
        */
        NIIi materialHandle;

        Plane plane;

        /// 补丁面 (只适用于当 fType = FGT_PATCH时)
        CurvedSurface * patchSurf;

        _EngineAPI friend Nostream & operator<<(Nostream & o, const StaticFaceGroup& s)
        {
            o << "StaticFaceGroup(";
            if (s.fType == FGT_FACE_LIST)
            {
                o << "faceList, numVertices=" << s.numVertices << ", vertexStart=" << s.vertexStart;
                o << ", numElements=" << s.numElements << ", elementStart=" << s.elementStart;
                o << ", normal=" << s.plane.normal;
            }
            else if (s.fType == FGT_PATCH)
            {
                o << "bezierPatch, numVertices=" << s.numVertices << ", vertexStart=" << s.vertexStart;
                // TODO
            }

            o << ", materialHandle=" << s.materialHandle;
            o << ")";

            return o;
        }
    };
}
#endif