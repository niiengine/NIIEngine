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

#include "NiiSDLWindow.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiImageCodec.h"
#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiStringConverter.h"

#if NII_PLATFORM == NII_PLATFORM_WIN32
#   include <windows.h>
#   include <wingdi.h>
#   include <GL/gl.h>
#   define GL_GLEXT_PROTOTYPES
#   include "glprocs.h"
#   include <GL/glu.h>
#elif NII_PLATFORM == NII_PLATFORM_LINUX
#   include <GL/gl.h>
#   include <GL/glu.h>
#elif NII_PLATFORM == NII_PLATFORM_APPLE
#   include <OpenGL/gl.h>
#   define GL_EXT_texture_env_combine 1
#   include <OpenGL/glext.h>
#   include <OpenGL/glu.h>
#endif

namespace NII {
    //-----------------------------------------------------------------------
    SDLWindow::SDLWindow() : mScreen(NULL), mActive(false), mClosed(false) {}
    //-----------------------------------------------------------------------
    SDLWindow::~SDLWindow()
    {
        // according to http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSetVideoMode
        // never free the surface returned from SDL_SetVideoMode
        /*if (mScreen != NULL)
            SDL_FreeSurface(mScreen);*/
    }
    //-----------------------------------------------------------------------
	void SDLWindow::setup(const String & name, NCount width, NCount height,
        bool fullScreen, const PropertyData * miscParams)
    {
		int colourDepth = 32;
		String title = name;
		if(miscParams)
		{
			// Parse miscellenous parameters
			PropertyData::const_iterator opt;
			// Bit depth
			opt = miscParams->find("colourDepth");
			if(opt != miscParams->end()) //check for FSAA parameter, if not ignore it...
				colourDepth = StringConverter::parseUnsignedInt(opt->second);
			// Full screen antialiasing
			opt = miscParams->find("FSAA");
			if(opt != miscParams->end()) //check for FSAA parameter, if not ignore it...
			{
				size_t fsaa_x_samples = StringConverter::parseUnsignedInt(opt->second);
				if(fsaa_x_samples>1) {
					// If FSAA is enabled in the parameters, enable the MULTISAMPLEBUFFERS
					// and set the number of samples before the render window is created.
					SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
					SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,fsaa_x_samples);
				}
			}
			// Window title
			opt = miscParams->find("title");
			if(opt != miscParams->end()) //check for FSAA parameter, if not ignore it...
				title = opt->second;
		}

        LogManager::getOnly().logMessage(_T("SDLWindow::setup"), LML_TRIVIAL);
        SDL_Surface* screen;
        int flags = SDL_OPENGL | SDL_HWPALETTE | SDL_RESIZABLE;

        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        // request good stencil size if 32-bit colour
        if (colourDepth == 32)
        {
            SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8);
        }

        if (fullScreen)
            flags |= SDL_FULLSCREEN;

        LogManager::getOnly().logMessage(_T("Create window"), LML_TRIVIAL);
        screen = SDL_SetVideoMode(width, height, colourDepth, flags);
        if (!screen)
        {
            LogManager::getOnly().logMessage(LML_CRITICAL,
                String(_T("Could not make screen: ")) + SDL_GetError());
            exit(1);
        }
        LogManager::getOnly().logMessage(_T("screen is valid"), LML_TRIVIAL);
        mScreen = screen;

        mName = name;

        mWidth = width;
        mHeight = height;

        mActive = true;

        if (!fullScreen)
            SDL_WM_SetCaption(title.c_str(), 0);

        glXGetVideoSyncSGI = (int (*)(unsigned int *))SDL_GL_GetProcAddress("glXGetVideoSyncSGI");
        glXWaitVideoSyncSGI = (int (*)(int, int, unsigned int *))SDL_GL_GetProcAddress("glXWaitVideoSyncSGI");
    }
    //-----------------------------------------------------------------------
    void SDLWindow::destroy(void)
    {
        // according to http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSetVideoMode
        // never free the surface returned from SDL_SetVideoMode
        //SDL_FreeSurface(mScreen);
        mScreen = NULL;
        mActive = false;

        Engine::getOnly().getRender()->remove(this);
    }
    //-----------------------------------------------------------------------
    bool SDLWindow::isActive() const
    {
        return mActive;
    }
    //-----------------------------------------------------------------------
    bool SDLWindow::isClosed() const
    {
        return mClosed;
    }
    //-----------------------------------------------------------------------
    void SDLWindow::pos(int left, int top)
    {
        // XXX FIXME
    }
    //-----------------------------------------------------------------------
    void SDLWindow::resize(unsigned int width, unsigned int height)
    {
        SDL_Surface* screen;
        int flags = SDL_OPENGL | SDL_HWPALETTE | SDL_RESIZABLE;

        LogManager::getOnly().logMessage(_T("Updating window"), LML_TRIVIAL);
        screen = SDL_SetVideoMode(width, height, mScreen->format->BitsPerPixel, flags);
        if (!screen)
        {
            LogManager::getOnly().logMessage(LML_CRITICAL,
                String(_T("Could not make screen: ")) + SDL_GetError());
            exit(1);
        }
        LogManager::getOnly().logMessage(_T("screen is valid"), LML_TRIVIAL);
        mScreen = screen;

        mWidth = width;
        mHeight = height;

        for(Views::iterator it = mViews.begin(); it != mViews.end(); ++it)
        {
            (*it).second->sync();
        }
    }
    //-----------------------------------------------------------------------
    void SDLWindow::swap(bool waitForVSync)
    {
        if ( waitForVSync && glXGetVideoSyncSGI && glXWaitVideoSyncSGI )
        {
            unsigned int retraceCount;
            glXGetVideoSyncSGI( &retraceCount );
            glXWaitVideoSyncSGI( 2, ( retraceCount + 1 ) & 1, &retraceCount);
        }

        SDL_GL_SwapBuffers();
        // XXX More?
    }
    //-----------------------------------------------------------------------
	void SDLWindow::fill(FrameObj::FaceType buffer, const Box & src, PixelBlock & dst)
	{
		if((dst.left < 0) || (dst.right > mWidth) ||
			(dst.top < 0) || (dst.bottom > mHeight) ||
                (dst.front != 0) || (dst.back != 1))
		{
			NII_EXCEPT(Exception::ERR_INVALIDPARAMS, _T("Invalid box."));
		}

		if(buffer == FT_Unknow)
		{
			buffer = mFullMode? FT_Front : FT_Back;
		}

		GLenum format = NII::GLPixelUtil::getGLOriginFormat(dst.format);
		GLenum type = NII::GLPixelUtil::getGLOriginDataType(dst.format);

		if ((format == GL_NONE) || (type == 0))
		{
			NII_EXCEPT(Exception::ERR_INVALIDPARAMS, _T("Unsupported format."));
		}

        // Switch context if different from current one
        RenderSys * rsys = Engine::getOnly().getRender();
        rsys->_setViewport(this->getViewport(0));

        // Must change the packing to ensure no overruns!
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

		glReadBuffer((buffer == FT_Front)? GL_FRONT : GL_BACK);
		glReadPixels((GLint)dst.left, (GLint)dst.top,
            (GLsizei)dst.getWidth(), (GLsizei)dst.getHeight(), format, type, dst.data);

        // restore default alignment
        glPixelStorei(GL_PACK_ALIGNMENT, 4);

		//vertical flip
		{
			size_t rowSpan = dst.getWidth() * PixelUtil::getNumElemBytes(dst.format);
			size_t height = dst.getHeight();
			uchar * tmpData = new uchar[rowSpan * height];
			uchar * srcRow = (uchar *)dst.data, *tmpRow = tmpData + (height - 1) * rowSpan;

			while(tmpRow >= tmpData)
			{
				memcpy(tmpRow, srcRow, rowSpan);
				srcRow += rowSpan;
				tmpRow -= rowSpan;
			}
			memcpy(dst.data, tmpData, rowSpan * height);

			delete [] tmpData;
		}
	}
    //-----------------------------------------------------------------------
}