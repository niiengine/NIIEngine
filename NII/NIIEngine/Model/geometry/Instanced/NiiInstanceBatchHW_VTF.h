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

#ifndef _NII_InstanceBatchHW_VTF_H__
#define _NII_InstanceBatchHW_VTF_H__

#include "NiiInstanceBatchVTF.h"
#include "NiiTexture.h"

namespace NII
{
    /** Instancing implementation using vertex texture through Vertex Texture Fetch (VTF) and
        hardware instancing.
        @see BaseInstanceBatchVTF and @see InstanceBatchHW

        The advantage over TextureVTF technique, is that this implements a basic culling algorithm
        to avoid useless processing in vertex shader and uses a lot less VRAM and memory bandwidth

        Basically it has the benefits of both TextureVTF (skeleton animations) and HWInstancingBasic
        (lower memory consumption and basic culling) techniques

        @remarks
            Design discussion webpage: http://www.ogre3d.org/forums/viewtopic.php?f=4&t=59902
        @author
            Matias N. Goldberg ("dark_sylinc")
        @version
            1.2
     */
    class _EngineAPI InstanceBatchHW_VTF : public BaseInstanceBatchVTF
    {
    protected:
        bool mKeepStatic;

        //Pointer to the buffer containing the per instance vertex data
        VertexBuffer * mInstanceVertexBuffer;

        void setupVertices(const SubMesh * baseSubMesh);
        void setupIndices(const SubMesh * baseSubMesh);

        /** Creates 2 TEXCOORD semantics that will be used to sample the vertex texture */
        void createVertexSemantics(VertexData * thisVertexData, VertexData * baseVertexData,
            const HWBoneIdxVec & hwBoneIdx, const HWBoneWgtVec & hwBoneWgt);

        /** updates the vertex buffer containing the per instance data
        @param[in] isFirstTime Tells if this is the first time the buffer is being updated
        @param[in] currentCamera The camera being used for render (valid when using bone matrix lookup)
        @return The number of instances to be rendered
        */
        virtual NCount updateInstanceDataBuffer(bool isFirstTime, Camera * currentCamera);


        virtual bool checkSubMeshCompatibility(const SubMesh * baseSubMesh);

        /** Keeps filling the VTF with world matrix data. Overloaded to avoid culled objects
            and update visible instances' animation
        */
        NCount updateVertexTexture(Camera * currentCamera);

        virtual bool matricesToghetherPerRow() const { return true; }
    public:
        InstanceBatchHW_VTF(InstanceMesh * creator, Mesh * mesh, Material * material, 
            NCount count, const BoneIndexList * bonemap, Nid bid);
        virtual ~InstanceBatchHW_VTF();
        /** @see InstanceBatch::calculateMaxNumInstances */
        NCount calculateMaxNumInstances( const SubMesh *baseSubMesh, Nui16 flags ) const;

        /** @copydoc InstanceBatchHW::_boundsDirty */
        void _boundsDirty(void);

        /** @copydoc InstanceBatchHW::setStaticAndUpdate */
        void setStaticAndUpdate( bool bStatic );

        bool isStatic() const { return mKeepStatic; }

        /** Overloaded to visibility on a per unit basis and finally updated the vertex texture */
        virtual void queue(RenderQueue * queue );
    };

}

#endif
