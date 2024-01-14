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

#include "SSAOLogic.h"
#include <Ogre.h>

using namespace Ogre;

class ssaoListener: public Ogre::CompositorInstance::Listener
{
public:
	ssaoListener(Ogre::CompositorInstance * instance) : mInstance(instance) {}

	// this callback we will use to modify SSAO parameters
    void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
    {
        if (pass_id != 42) // not SSAO, return
            return;

        // this is the camera you're using
		Ogre::Camera * cam = mInstance->getChain()->getViewport()->getCamera();

        // calculate the far-top-right corner in view-space
        Ogre::Vector3 farCorner = cam->getViewMatrix(false) * cam->getClipPlaneIntersect()[4];

        // get the pass
        NII::ShaderCh * pass = mat->getLOD()->get(0);

        // get the vertex shader parameters
        Ogre::GpuProgramParam * params;
        pass->getProgram().get(0, ST_VS, NULL, params);
        // set the camera's far-top-right corner
        if(params->getDefine() != 0)
        {
            if(params->getDefine()->get("farCorner"))
                params->set("farCorner", farCorner);
        }
        // get the fragment shader parameters
        pass->getProgram().get(0, ST_FS, NULL, params);
        // set the projection matrix we need
        static const Ogre::Matrix4 CLIP_SPACE_TO_IMAGE_SPACE(
            0.5,    0,    0,  0.5,
            0,   -0.5,    0,  0.5,
            0,      0,    1,    0,
            0,      0,    0,    1);
        if(params->getDefine() != 0)
        {
            if(params->getDefine()->get("ptMat"))
            {
                Matrix4f temp;
                RenderSys * rsys;
                N_Engine().get(rsys);
                rsys->getSysProj(cam->getProjMatrix(), temp, true);
                params->set("ptMat", CLIP_SPACE_TO_IMAGE_SPACE * temp);
            }
            if(params->getDefine()->get("far"))
            {
                params->set("far", cam->getFarClipDistance());
            }
        }
    }
private:
	Ogre::CompositorInstance* mInstance;
};

Ogre::CompositorInstance::Listener* SSAOLogic::createListener(Ogre::CompositorInstance* instance)
{
	return new ssaoListener(instance);
}
