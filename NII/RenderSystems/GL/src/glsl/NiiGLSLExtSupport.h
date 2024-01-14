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

#ifndef __GLSLExtSupport_H__
#define __GLSLExtSupport_H__

#include "NiiGLPreInclude.h"

namespace NII
{
namespace GLSL 
{
    // forward declarations
    class GLSLShader;
    class GLSLProgram;
    class GLSLProgramManager;
    class GLSLShaderFactory;

    /** Check for GL errors and report them in the Ogre Log.
    */
    void reportGLSLError(GLenum glErr, const String & ogreMethod, const String & errorTextPrefix, GLuint obj,
        bool forceInfoLog = false, bool forceException = false);

    /** if there is a message in GL info log then post it in the Ogre Log
    @param msg the info log message string is appended to this string
    @param obj the GL object that is used to retrieve the info log
    */
    String logObjectInfo(const String & msg, GLuint obj);

    /// just return the info without logging it
    String getObjectInfo(GLuint obj);

}
}
#endif