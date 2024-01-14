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

#ifndef _NII_InstanceBatchHW_H__
#define _NII_InstanceBatchHW_H__

#include "NiiPreInclude.h"
#include "NiiInstanceBatch.h"

namespace NII
{
    /** This is technique requires true instancing hardware support.
        Basically it creates a cloned vertex buffer from the original, with an extra buffer containing
        3 additional TEXCOORDS (12 bytes) repeated as much as the instance count.
        That will be used for each instance data.
        @par
        The main advantage of this technique is that it's <u>VERY</u> fast; but it doesn't support
        skeletal animation at all. Very reduced memory consumption and bandwidth. Great for particles,
        debris, bricks, trees, sprites.
        This batch is one of the few (if not the only) techniques that allows culling on an individual
        basis. This means we can save vertex shader performance for instances that aren't in scene or
        just not focused by the camera.

        @remarks
            Design discussion webpage: http://www.ogre3d.org/forums/viewtopic.php?f=4&t=59902
        @author
            Matias N. Goldberg ("dark_sylinc")
        @version
            1.1
     */
    class _EngineAPI InstanceBatchHW : public InstanceBatch
    {
    public:
        InstanceBatchHW(InstanceMesh * creator, Mesh * mesh, Material * material, 
            NCount instancesPerBatch, const BoneIndexList * indexToBoneMap, Nid bid);

        virtual ~InstanceBatchHW();

        /** @see InstanceBatch::calculateMaxNumInstances */
        NCount calculateMaxNumInstances(const SubMesh * baseSubMesh, Nui16 flags) const;

        /** @see InstanceBatch::buildFrom */
        void buildFrom(const SubMesh * baseSubMesh, const GeometryRaw & op);

        /** Overloaded so that we don't perform needless updates when in static mode. Also doing that
            could cause glitches with shadow mapping (since Ogre thinks we're small/bigger than we
            really are when displaying, or that we're somewhere else)
        */
        void _boundsDirty(void);

        /** @see InstanceBatch::setStaticAndUpdate. While this flag is true, no individual per-entity
            cull check is made. This means if the camera is looking at only one instance, all instances
            are sent to the vertex shader (unlike when this flag is false). This saves a lot of CPU
            power and a bit of bus bandwidth.
        */
        void setStaticAndUpdate(bool bStatic);

        bool isStatic() const { return mKeepStatic; }

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails GeometryObj::getMatrixCount
        NCount getMatrixCount() const;

        /** Overloaded to avoid updating skeletons (which we don't support), check visibility on a
            per unit basis and finally updated the vertex buffer */
        virtual void queue(RenderQueue * queue);
    private:
        bool mKeepStatic;

        void setupVertices(const SubMesh * baseSubMesh);
        void setupIndices(const SubMesh * baseSubMesh);

        void removeBlendData();
        virtual bool checkSubMeshCompatibility(const SubMesh * baseSubMesh);

        NCount updateVertexBuffer(Camera *currentCamera);
    };
}
#endif