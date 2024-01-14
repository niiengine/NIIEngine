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

#include "NiiGLPixelCountQuery.h"
#include "NiiException.h"

namespace NII 
{
    /**
      * This is a class that is the base class of the query class for
      * hardware occlusion testing.
      *
      * @author Lee Sandberg email: lee@abcmedia.se
      *
      * Updated on 12/7/2004 by Chris McGuirk
      * - Implemented ARB_occlusion_query
      * Updated on 13/9/2005 by Tuan Kuranes email: tuan.kuranes@free.fr
      */
    //------------------------------------------------------------------
    GLPixelCountQuery::GLPixelCountQuery()
    {
        // Check for hardware occlusion support
        if(GLEW_VERSION_1_5 || GLEW_ARB_occlusion_query)
        {
            glGenQueries(1, &mQueryID );
        }
        else if (GLEW_NV_occlusion_query)
        {
            glGenOcclusionQueriesNV(1, &mQueryID);
        }
        else
        {
            N_EXCEPT( Internal,
                _I18n("Cannot allocate a Hardware query. This video card doesn't supports it, sorry."));
        }
    }
    //------------------------------------------------------------------
    GLPixelCountQuery::~GLPixelCountQuery()
    {
        if(GLEW_VERSION_1_5 || GLEW_ARB_occlusion_query)
        {
            glDeleteQueries(1, &mQueryID);
        }
        else if (GLEW_NV_occlusion_query)
        {
            glDeleteOcclusionQueriesNV(1, &mQueryID);
        }
    }
    //------------------------------------------------------------------
    void GLPixelCountQuery::beginRender()
    {
        if(GLEW_VERSION_1_5 || GLEW_ARB_occlusion_query)
        {
            glBeginQueryARB(GL_ANY_SAMPLES_PASSED, mQueryID);
        }
        else if (GLEW_NV_occlusion_query)
        {
            glBeginOcclusionQueryNV(mQueryID);
        }
    }
    //------------------------------------------------------------------
    void GLPixelCountQuery::endRender()
    {
        if(GLEW_VERSION_1_5 || GLEW_ARB_occlusion_query)
        {
            glEndQueryARB(GL_ANY_SAMPLES_PASSED);
        }
        else if (GLEW_NV_occlusion_query)
        {
            glEndOcclusionQueryNV();
        }
    }
    //------------------------------------------------------------------
    bool GLPixelCountQuery::query(NCount & count)
    {
        if(GLEW_VERSION_1_5 || GLEW_ARB_occlusion_query)
        {
            glGetQueryObjectuiv(mQueryID, GL_QUERY_RESULT, (GLuint*)&mLast);
            count = mLast;
            return true;
        }
        else if(GLEW_NV_occlusion_query)
        {
            glGetOcclusionQueryuivNV(mQueryID, GL_PIXEL_COUNT_NV, (GLuint*)&mLast);
            count = mLast;
            return true;
        }

        return false;
    }
    //------------------------------------------------------------------
    bool GLPixelCountQuery::isRun()
    {
        GLuint available = GL_FALSE;

        if(GLEW_VERSION_1_5 || GLEW_ARB_occlusion_query)
        {
            glGetQueryObjectuiv(mQueryID, GL_QUERY_RESULT_AVAILABLE, &available);
        }
        else if (GLEW_NV_occlusion_query)
        {
            glGetOcclusionQueryuivNV(mQueryID, GL_PIXEL_COUNT_AVAILABLE_NV, &available);
        }

        // GL_TRUE means a wait would occur
        return !(available == GL_TRUE);
    }
    //-----------------------------------------------------------------------
}