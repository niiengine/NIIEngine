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

#ifndef _NII_WXWIDGETWINDOW_H_
#define _NII_WXWIDGETWINDOW_H_

#include "NiiGLPreInclude.h"
#include "NiiGLRenderTarget.h"
#include "NiiViewWindow.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/dcbuffer.h>
#include <wx/glcanvas.h>

namespace NII
{
    struct DrawGridData;
    class wxWidgetContext;
    class RenderWidget : public wxGLCanvas
    {
    public:
        RenderWidget(wxWindow * win, const wxSize & size, const int *attribList);
        ~RenderWidget();

        void setContext(wxWidgetContext * c);
        void OnScroll(wxScrollEvent& e);
        void OnPaint(wxPaintEvent& e);

        void drawGrid(const DrawGridData & data);
        void drawLine(const float from[4], const float to[4], const float color[4], float lineWidth = 1);
        void drawPoint(const float* positions, const float color[4], float pointDrawSize = 1);
        void drawPoints(const float* positions, const float color[4], int numPoints, int pointStrideInBytes, float pointDrawSize = 1);
    protected:
        wxWidgetContext * mContext;
        wxWindow * mWin;

        GLfloat m_projectionMatrix[16];
        GLfloat m_viewMatrix[16];

        GLuint linesShader;
        GLuint linesShader1;
        GLint lines_ModelViewMatrix = 0;
        GLint lines_ProjectionMatrix = 0;
        GLint lines_position = 0;
        GLint lines_colour = 0;
        GLint lines_ModelViewMatrix1 = 0;
        GLint lines_ProjectionMatrix1 = 0;
        GLint lines_position1 = 0;
        GLint lines_colour1 = 0;
        GLint lines_other1 = 0;
        GLuint lineVertexBufferObject = 0;
        GLuint lineVertexArrayObject = 0;
        GLuint lineIndexVbo = 0;

        GLuint linesVertexBufferObject = 0;
        GLuint linesVertexArrayObject = 0;
        GLuint linesIndexVbo = 0;
        int movex = 0;
        int movey = 0;
    };

    class wxWidgetWindow : public ViewWindow, public GLRenderTarget
    {
    public:
        wxWidgetWindow();
        ~wxWidgetWindow();

        /**
        * Get the actual widget that is housing OGRE's GL view.
        */
        RenderWidget * getRenderWidget();

        /// @copydetails ViewWindow::setup
        void setup(const String & name, NCount wpixel, NCount hpixel, bool full, const PropertyData * params);

        /// @copydetails ViewWindow::setFullMode
        void setFullMode(bool fullScreen, NCount width, NCount height);

        /// @copydetails ViewWindow::destroy
        void destroy(void);

        /// @copydetails ViewWindow::isActive
        bool isActive() const;

        /// @copydetails ViewWindow::isClosed
        bool isClosed() const;

        /// @copydetails ViewWindow::pos
        void pos(NIIi left, NIIi top);

        /// @copydetails ViewWindow::resize
        void resize(NCount width, NCount height);

        /// @copydetails FrameObj::swap
        void swap(bool vsync);

        /// @copydetails FrameObj::fill
        void fill(FaceType buffer, const Box & src, PixelBlock & dst);

        /** @copydetails FrameObj::getPlatformData
         * WXWIDGETWINDOW   The wxPanel instance
         * WXWIDGETWIDGET	The wxGLCanvas instance
         */
        bool getPlatformData(const String & name, void * data) const;

        /// @copydetails GLRenderTarget::getContext
        GLContext* getContext() const;

        /// @copydetails ViewWindow::_notifyInit
        void _notifyInit(RenderSys * sys);
    private:
        wxWindow * mWindow;
        wxWidgetContext * mContext;
        RenderWidget * mRenderWidget;
    };
};
#endif