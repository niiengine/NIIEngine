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

#ifndef _NII_InstanceBatchShader_H__
#define _NII_InstanceBatchShader_H__

#include "NiiPreInclude.h"
#include "NiiInstanceBatch.h"

namespace NII
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Scene
    *  @{
    */

    /** This is the same technique the old "InstanceSpace" implementation used (with improvements).
        Basically it creates a large vertex buffer with many repeating entities, and sends per instance
        data through shader constants. Because SM 2.0 & 3.0 have up to 256 shader constant registers,
        this means there can be approx up to 84 instances per batch, assuming they're not skinned
        But using shader constants for other stuff (i.e. lighting) also affects negatively this number
        A mesh with skeletally animated 2 bones reduces the number 84 to 42 instances per batch.
        @par
        The main advantage of this technique is that it's supported on a high variety of hardware
        (SM 2.0 cards are required) and the same shader can be used for both skeletally animated
        normal entities and instanced entities without a single change required.
        @par
        Unlike the old InstanceSpace implementation, the developer doesn't need to worry about
        reaching the 84 instances limit, the InstanceMesh automatically takes care of splitting
        and creating new batches. But beware internally, this means less performance improvement.
        Another improvement is that vertex buffers are shared between batches, which significantly
        reduces GPU VRAM usage.

        @remarks
            Design discussion webpage: http://www.ogre3d.org/forums/viewtopic.php?f=4&t=59902
        @author
            Matias N. Goldberg ("dark_sylinc")
        @version
            1.0
     */
    class _EngineAPI InstanceBatchShader : public InstanceBatch
    {
        Nui16 mNumWorldMatrices;

        void setupVertices(const SubMesh * baseSubMesh);
        void setupIndices(const SubMesh * baseSubMesh);

        /** When the mesh is (hardware) skinned, a different code path is called so that
            we reuse the index buffers and modify them in place. For example Instance #2
            with reference to bone #5 would have BlendIndex = 2 + 5 = 7
            Everything is copied identically except the VT_Matrix_Index semantic
        */
        void setupHardwareSkinned(const SubMesh * baseSubMesh, VertexData * thisVertexData,
            VertexData * baseVertexData);
    public:
        InstanceBatchShader(InstanceMesh * creator, Mesh * mesh, Material * material, 
            NCount count, const BoneIndexList * bonemap, Nid bid);

        virtual ~InstanceBatchShader();

        /** @see InstanceBatch::calculateMaxNumInstances */
        NCount calculateMaxNumInstances(const SubMesh * baseSubMesh, Nui16 flags) const;

        /** @see InstanceBatch::buildFrom */
        void buildFrom(const SubMesh * baseSubMesh, const GeometryRaw & op);

        ///@copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;
        
        ///@copydetails GeometryObj::getMatrixCount
        NCount getMatrixCount() const;
    };
}

#endif
