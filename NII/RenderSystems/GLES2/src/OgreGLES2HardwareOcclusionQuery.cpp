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

#include "OgreGLES2HardwareOcclusionQuery.h"
#include "OgreLogManager.h"
#include "OgreException.h"
#include "OgreRoot.h"
#include "OgreGLES2RenderSystem.h"
#include "OgreGLUtil.h"

namespace Ogre {

//------------------------------------------------------------------
/**
  * Default object constructor
  * 
  */
GLES2HardwareOcclusionQuery::GLES2HardwareOcclusionQuery() 
{ 
    createQuery();
}
//------------------------------------------------------------------
/**
  * Object destructor
  */
GLES2HardwareOcclusionQuery::~GLES2HardwareOcclusionQuery() 
{ 
    destroyQuery();
}
//------------------------------------------------------------------
void GLES2HardwareOcclusionQuery::createQuery()
{
    GLCheck(glGenQueriesEXT(1, &mQueryID));
}
//------------------------------------------------------------------
void GLES2HardwareOcclusionQuery::destroyQuery()
{
    GLCheck(glDeleteQueriesEXT(1, &mQueryID));
}
//------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
void GLES2HardwareOcclusionQuery::notifyOnContextLost()
{
    destroyQuery();
}
//------------------------------------------------------------------
void GLES2HardwareOcclusionQuery::notifyOnContextReset()
{
    createQuery();
}
#endif
//------------------------------------------------------------------
void GLES2HardwareOcclusionQuery::beginOcclusionQuery() 
{
    GLCheck(glBeginQueryEXT(GL_ANY_SAMPLES_PASSED_EXT, mQueryID));
}
//------------------------------------------------------------------
void GLES2HardwareOcclusionQuery::endOcclusionQuery() 
{
    GLCheck(glEndQueryEXT(GL_ANY_SAMPLES_PASSED_EXT));
}
//------------------------------------------------------------------
bool GLES2HardwareOcclusionQuery::pullOcclusionQuery( unsigned int* NumOfFragments ) 
{
    GLCheck(glGetQueryObjectuivEXT(mQueryID, GL_QUERY_RESULT_EXT, (GLuint*)NumOfFragments));
    mPixelCount = *NumOfFragments;
    return true;
}
//------------------------------------------------------------------
bool GLES2HardwareOcclusionQuery::isStillOutstanding(void)
{    
    GLuint available = GL_FALSE;

    GLCheck(glGetQueryObjectuivEXT(mQueryID, GL_QUERY_RESULT_AVAILABLE_EXT, &available));

    // GL_TRUE means a wait would occur
    return !(available == GL_TRUE);
} 

}
