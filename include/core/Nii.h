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

#ifndef _NII_H_
#define _NII_H_

#include "NiiPreInclude.h"
#include "NiiPropertyList.h"
#include "NiiErrorList.h"
#include "NiiVFSManager.h"
#include "NiiGpuProgramManager.h"
#include "NiiGBufferManager.h"
#include "NiiEquationManager.h"
#include "NiiMaterialManager.h"
#include "NiiMeshManager.h"
#include "NiiLogManager.h"
#include "NiiOverlayManager.h"
#include "NiiAnimationManager.h"
#include "NiiHighLevelGpuProgramManager.h"
#include "NiiParticleSpaceManager.h"
#include "NiiResourceSchemeManager.h"
#include "NiiSkeletonManager.h"
#include "NiiTextureManager.h"
#include "NiiFusionManager.h"
#include "NiiAABox.h"
#include "NiiConfigFile.h"
#include "NiiDataStream.h"
#include "NiiGeometrySpace.h"
#include "NiiException.h"
#include "NiiFrustum.h"
#include "NiiSpreadParticle.h"
#include "NiiGpuBuffer.h"
#include "NiiPixelCountQuery.h"
#include "NiiInstanceBatch.h"
#include "NiiInstanceGeo.h"
#include "NiiInstanceMesh.h"
#include "NiiKeyFrame.h"
#include "NiiLight.h"
#include "NiiCustomSpaceObj.h"
#include "NiiMaterialSerializer.h"
#include "NiiMeshSerializer.h"
#include "NiiOverlayView.h"
#include "NiiParticleEffect.h"
#include "NiiProfiler.h"
#include "NiiCustomQueue.h"
#include "NiiRenderListener.h"
#include "NiiFrameObjListener.h"
#include "NiiTextureFrame.h"
#include "NiiViewWindow.h"
#include "NiiPixelChain.h"
#include "NiiParticleCtrl.h"
#include "NiiPixelOrbit.h"
#include "NiiEngine.h"
#include "NiiSpaceNode.h"
#include "NiiRenderObj.h"
#include "NiiSkeletonSerializer.h"
#include "NiiStaticSpace.h"
#include "NiiStrConv.h"
#include "NiiGeometryCom.h"
#include "NiiShaderChTexture.h"
#include "NiiShaderChProgram.h"
#include "NiiViewport.h"
#include "NiiFrameResult.h"
#include "NiiFrameShaderCh.h"
#include "NiiSysOption.h"
#include "NiiShadowGenCamera.h"
#include "NiiNodeFusion.h"
#include "NiiVertexOffsetFusion.h"
#include "NiiUnitFusion.h"
#include "NiiKeyFrameTrack.h"
#include "NiiAny.h"

#endif