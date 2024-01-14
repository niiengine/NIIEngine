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

#include "OgreGLSLESProgramFactory.h"
#include "OgreGLSLESProgramManager.h"
#include "OgreGLSLESProgram.h"
#include "OgreRoot.h"

namespace Ogre {
    GLSLESProgramManager* GLSLESProgramFactory::mProgramManager = NULL;
    //-----------------------------------------------------------------------
    String GLSLESProgramFactory::sLanguageName = "glsles";
    //-----------------------------------------------------------------------
    GLSLESProgramFactory::GLSLESProgramFactory(void)
    {
        if (mProgramManager == NULL)
        {
            mProgramManager = new GLSLESProgramManager();
        }
    }
    //-----------------------------------------------------------------------
    GLSLESProgramFactory::~GLSLESProgramFactory(void)
    {
        if (mProgramManager)
        {
            delete mProgramManager;
            mProgramManager = NULL;
        }
    }
    //-----------------------------------------------------------------------
    const String& GLSLESProgramFactory::getLanguage(void) const
    {
        return sLanguageName;
    }
    //-----------------------------------------------------------------------
    HighLevelGpuProgram* GLSLESProgramFactory::create(ResourceManager* creator, 
        const String& name, ResourceHandle handle,
        const String& group, bool isManual, ManualResourceLoader* loader)
    {
        return N_new GLSLESProgram(creator, name, handle, group, isManual, loader);
    }
    //-----------------------------------------------------------------------
    void GLSLESProgramFactory::destroy(HighLevelGpuProgram* prog)
    {
        N_delete prog;
    }
}
