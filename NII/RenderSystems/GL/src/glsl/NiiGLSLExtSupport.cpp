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

#include "NiiGLSLExtSupport.h"
#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"

namespace NII
{
namespace GLSL
{
    //-----------------------------------------------------------------------------
    void reportGLSLError(GLenum glErr, const String & method, const String & errorTextPrefix, 
        GLuint obj, bool forceInfoLog, bool forceException)
    {
        bool errorsFound = false;
        String msg = errorTextPrefix;

        // get all the GL errors
        while (glErr != GL_NO_ERROR)
        {
            msg += glErrorToString(glErr);
            glErr = glGetError();
            errorsFound = true;
        }

        // if errors were found then put them in the Log and raise and exception
        if (errorsFound || forceInfoLog)
        {
            // if shader or program object then get the log message and send to the log manager
            msg += logObjectInfo( msg, obj );

            if (forceException) 
            {
                N_EXCEPT(Internal, msg + method);
            }
        }
    }
    //-----------------------------------------------------------------------------
    String logObjectInfo(const String& msg, GLuint obj)
    {
        String logMessage = getObjectInfo(obj);

        if (logMessage.empty())
            return msg;

        logMessage = msg + _T("\n") + logMessage;

        N_Only(Log).log(LL_Fatal, logMessage);

        return logMessage;
    }
    //-----------------------------------------------------------------------------
    String getObjectInfo(GLuint obj)
    {
        GLint infologLength = 0;

        GLboolean isShader = glIsShader(obj);
        GLboolean isProgramPipeline = N_Engine().getRender()->getFeature()->isSupport(GF_GLSL_SeparateShaderObject) && glIsProgramPipeline(obj);
        GLboolean isProgram = glIsProgram(obj);

        if (isShader)
        {
            GLCheck(glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength));
        }
        else if (isProgramPipeline)
        {
            //FIXME Crashes on NVIDIA? See GL3+ GSoC forum
            // posts around 2013-11-25.
            GLCheck(glGetProgramPipelineiv(obj, GL_INFO_LOG_LENGTH, &infologLength));
        }
        else if (isProgram)
        {
            GLCheck(glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength));
        }

        // No info log available.
        if (infologLength < 1)
        {
            return String("");
        }

        GLint charsWritten  = 0;

        GLchar * infoLog = new GLchar [infologLength];
        infoLog[0] = 0;

        if (isShader)
        {
            GLCheck(glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog));
        }
        else if (isProgramPipeline)
        {
            GLCheck(glGetProgramPipelineInfoLog(obj, infologLength, &charsWritten, infoLog));
        }
        else if (isProgram)
        {
            GLCheck(glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog));
        }

        StrConv::conv(infoLog, logMessage);
        delete [] infoLog;

        StringUtil::trim(logMessage, false, true);

        return logMessage;
    }
    //-----------------------------------------------------------------------------
}
}