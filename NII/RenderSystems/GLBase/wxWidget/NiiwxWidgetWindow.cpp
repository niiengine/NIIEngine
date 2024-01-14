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
#include "NiiwxWidgetWindow.h"
#include "NiiwxWidgetGLSupport.h"
#include "NiiwxWidgetContext.h"
#include "NiiGLPixelFormat.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"
#include "linesVS.h"
#include "linesPS.h"
#include "wx/dcgraph.h"
#ifdef _DEBUG
#include <N_assert.h>
#define b3Assert N_assert
#else
#define b3Assert(x)
#endif

#define MAX_POINTS_IN_BATCH 1024
#define MAX_LINES_IN_BATCH 1024
namespace NII
{
    static void b3CreateFrustum(
        float left,
        float right,
        float bottom,
        float top,
        float nearVal,
        float farVal,
        float frustum[16])
    {
        frustum[0 * 4 + 0] = (float(2) * nearVal) / (right - left);
        frustum[0 * 4 + 1] = float(0);
        frustum[0 * 4 + 2] = float(0);
        frustum[0 * 4 + 3] = float(0);

        frustum[1 * 4 + 0] = float(0);
        frustum[1 * 4 + 1] = (float(2) * nearVal) / (top - bottom);
        frustum[1 * 4 + 2] = float(0);
        frustum[1 * 4 + 3] = float(0);

        frustum[2 * 4 + 0] = (right + left) / (right - left);
        frustum[2 * 4 + 1] = (top + bottom) / (top - bottom);
        frustum[2 * 4 + 2] = -(farVal + nearVal) / (farVal - nearVal);
        frustum[2 * 4 + 3] = float(-1);

        frustum[3 * 4 + 0] = float(0);
        frustum[3 * 4 + 1] = float(0);
        frustum[3 * 4 + 2] = -(float(2) * farVal * nearVal) / (farVal - nearVal);
        frustum[3 * 4 + 3] = float(0);
    }

    static void b3CreateLookAt(const Vector3f & eye, const Vector3f& center, const Vector3f& up, float result[16])
    {
        Vector3f f,u, s;
        (center - eye).normaliseFast(f);
        up.normaliseFast(u);
        (f.crossProduct(u)).normaliseFast(s);
        u = s.crossProduct(f);

        result[0 * 4 + 0] = s.x;
        result[1 * 4 + 0] = s.y;
        result[2 * 4 + 0] = s.z;

        result[0 * 4 + 1] = u.x;
        result[1 * 4 + 1] = u.y;
        result[2 * 4 + 1] = u.z;

        result[0 * 4 + 2] = -f.x;
        result[1 * 4 + 2] = -f.y;
        result[2 * 4 + 2] = -f.z;

        result[0 * 4 + 3] = 0.f;
        result[1 * 4 + 3] = 0.f;
        result[2 * 4 + 3] = 0.f;

        result[3 * 4 + 0] = -s.dotProduct(eye);
        result[3 * 4 + 1] = -u.dotProduct(eye);
        result[3 * 4 + 2] = f.dotProduct(eye);
        result[3 * 4 + 3] = 1.f;
    }

    // Load the shader from the source text
    void gltLoadShaderSrc(const char *szShaderSrc, GLuint shader)
    {
        GLchar *fsStringPtr[1];

        fsStringPtr[0] = (GLchar *)szShaderSrc;
        glShaderSource(shader, 1, (const GLchar **)fsStringPtr, NULL);
    }

    GLuint gltLoadShaderPair(const char *szVertexProg, const char *szFragmentProg)
    {
        N_assert(glGetError() == GL_NO_ERROR);

        // Temporary Shader objects
        GLuint hVertexShader;
        GLuint hFragmentShader;
        GLuint hReturn = 0;
        GLint testVal;

        // Create shader objects
        hVertexShader = glCreateShader(GL_VERTEX_SHADER);
        hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        gltLoadShaderSrc(szVertexProg, hVertexShader);
        gltLoadShaderSrc(szFragmentProg, hFragmentShader);

        // Compile them
        glCompileShader(hVertexShader);
        N_assert(glGetError() == GL_NO_ERROR);

        glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
        if (testVal == GL_FALSE)
        {
            char temp[256] = "";
            glGetShaderInfoLog(hVertexShader, 256, NULL, temp);
            fprintf(stderr, "Compile failed:\n%s\n", temp);
            N_assert(0);
            return 0;
            glDeleteShader(hVertexShader);
            glDeleteShader(hFragmentShader);
            return (GLuint)0;
        }

        N_assert(glGetError() == GL_NO_ERROR);

        glCompileShader(hFragmentShader);
        N_assert(glGetError() == GL_NO_ERROR);

        glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
        if (testVal == GL_FALSE)
        {
            char temp[256] = "";
            glGetShaderInfoLog(hFragmentShader, 256, NULL, temp);
            fprintf(stderr, "Compile failed:\n%s\n", temp);
            N_assert(0);
            exit(EXIT_FAILURE);
            glDeleteShader(hVertexShader);
            glDeleteShader(hFragmentShader);
            return (GLuint)0;
        }

        N_assert(glGetError() == GL_NO_ERROR);

        // Check for errors

        // Link them - assuming it works...
        hReturn = glCreateProgram();
        glAttachShader(hReturn, hVertexShader);
        glAttachShader(hReturn, hFragmentShader);

        glLinkProgram(hReturn);

        // These are no longer needed
        glDeleteShader(hVertexShader);
        glDeleteShader(hFragmentShader);

        // Make sure link worked too
        glGetProgramiv(hReturn, GL_LINK_STATUS, &testVal);
        if (testVal == GL_FALSE)
        {
            GLsizei maxLen = 4096;
            GLchar infoLog[4096];
            GLsizei actualLen;

            glGetProgramInfoLog(hReturn,
                maxLen,
                &actualLen,
                infoLog);

            printf("Warning/Error in GLSL shader:\n");
            printf("%s\n", infoLog);
            glDeleteProgram(hReturn);
            return (GLuint)0;
        }

        return hReturn;
    }

    struct DrawGridData
    {
        int gridSize;
        float upOffset;
        int upAxis;
        float gridColor[4];

        DrawGridData(int upAx = 1)
            : gridSize(500),
            upOffset(0.001f),
            upAxis(upAx)
        {
            gridColor[0] = 0.647f;
            gridColor[1] = 0.647f;
            gridColor[2] = 0.647f;
            gridColor[3] = 1.f;
        }
    };

    //-----------------------------------------------------------------------
    static void CheckGLError()
    {
        GLenum errLast = GL_NO_ERROR;

        for (;; )
        {
            GLenum err = glGetError();
            if (err == GL_NO_ERROR)
                return;

            // normally the error is reset by the call to glGetError() but if
            // glGetError() itself returns an error, we risk looping forever here
            // so check that we get a different error than the last time
            if (err == errLast)
            {
                wxLogError(wxT("OpenGL error state couldn't be reset."));
                return;
            }

            errLast = err;

            wxLogError(wxT("OpenGL error %d"), err);
        }
    }
    //-----------------------------------------------------------------------
    RenderWidget::RenderWidget(wxWindow * win, const wxSize & size, const int *attribList) :
        wxGLCanvas(win, wxID_ANY, attribList, wxDefaultPosition, size, wxFULL_REPAINT_ON_RESIZE | wxSIMPLE_BORDER),
        mContext(0),
        mWin(win)
    {
        Bind(wxEVT_PAINT, &RenderWidget::OnPaint, this);
        mWin->GetParent()->Bind(wxEVT_SCROLL_CHANGED, &RenderWidget::OnScroll, this);
    }
    //-----------------------------------------------------------------------
    RenderWidget::~RenderWidget()
    {
        Unbind(wxEVT_PAINT, &RenderWidget::OnPaint, this);
        mWin->GetParent()->Unbind(wxEVT_SCROLL_CHANGED, &RenderWidget::OnScroll, this);
    }
    //-----------------------------------------------------------------------
    void RenderWidget::OnScroll(wxScrollEvent& e)
    {
        if (mContext)
        {
            RenderSys * sys = N_EnginePtr()->getRender();
            sys->getConfigData()->get(N_PropertyWindow_wxWidgetMoveY, movey);
            sys->getConfigData()->get(N_PropertyWindow_wxWidgetMoveX, movex);
            Move(-movex, -movey);
        }
    }
    //-----------------------------------------------------------------------
    void RenderWidget::setContext(wxWidgetContext * c)
    { 
        mContext = c; 
        mContext->setCurrent();

        linesShader = gltLoadShaderPair(linesVertexShader, linesFragmentShader);
        lines_ModelViewMatrix = glGetUniformLocation(linesShader, "ModelViewMatrix");
        lines_ProjectionMatrix = glGetUniformLocation(linesShader, "ProjectionMatrix");
        lines_colour = glGetUniformLocation(linesShader, "colour");
        lines_position = glGetAttribLocation(linesShader, "position");
        glLinkProgram(linesShader);

        linesShader1 = gltLoadShaderPair(linesVertexShader1, linesFragmentShader);
        lines_ModelViewMatrix1 = glGetUniformLocation(linesShader1, "ModelViewMatrix");
        lines_ProjectionMatrix1 = glGetUniformLocation(linesShader1, "ProjectionMatrix");
        lines_colour1 = glGetUniformLocation(linesShader1, "colour");
        lines_other1 = glGetUniformLocation(linesShader1, "other");
        lines_position1 = glGetAttribLocation(linesShader1, "position");
        glLinkProgram(linesShader1);

        {
            glGenVertexArrays(1, &linesVertexArrayObject);
            glBindVertexArray(linesVertexArrayObject);

            glGenBuffers(1, &linesVertexBufferObject);
            glGenBuffers(1, &linesIndexVbo);

            int sz = MAX_LINES_IN_BATCH * sizeof(float) * 4;

            glBindBuffer(GL_ARRAY_BUFFER, linesVertexBufferObject);
            float positions[16] = {
                0.f , 0.f , 0.f ,1.0f,
                0.f , 0.f , 0.f ,2.0f,
                0.f , 0.f , 0.f ,3.0f,
                0.f , 0.f , 0.f ,4.0f };

            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, 0, GL_DYNAMIC_DRAW);

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, positions);
            b3Assert(glGetError() == GL_NO_ERROR);
            glEnableVertexAttribArray(lines_position1);
            b3Assert(glGetError() == GL_NO_ERROR);
            glVertexAttribPointer(lines_position1, 4, GL_FLOAT, GL_FALSE, 0, 0);
            b3Assert(glGetError() == GL_NO_ERROR);

            glBindVertexArray(0);
        }
        {
            glGenVertexArrays(1, &lineVertexArrayObject);
            glBindVertexArray(lineVertexArrayObject);

            glGenBuffers(1, &lineVertexBufferObject);
            glGenBuffers(1, &lineIndexVbo);

            int sz = MAX_POINTS_IN_BATCH * sizeof(float) * 4;

            glBindBuffer(GL_ARRAY_BUFFER, lineVertexBufferObject);
            glBufferData(GL_ARRAY_BUFFER, sz, 0, GL_DYNAMIC_DRAW);

            glBindVertexArray(0);
        }

        float r, g, b, a;
        r = 0.0f;//红
        g = 1.0f;//绿
        b = 0.0f;//蓝
        a = 0.0f;//alpha，透明度用于背景混合，一般设0
        glClearColor(r, g, b, a);
        // set up the parameters we want to use
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_TEXTURE_2D);

        // add slightly more light, the default lighting is rather dark
        GLfloat ambient[] = { 0.5, 0.5, 0.5, 0.5 };
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

        // set viewing projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        int xss, yss;
        GetSize(&xss, &yss);
        float aspect = (float)xss / yss;
        #define m_frustumZNear  0.01
        #define m_frustumZFar   100
        //glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
        b3CreateFrustum(-aspect * m_frustumZNear, aspect* m_frustumZNear, -m_frustumZNear, m_frustumZNear, m_frustumZNear, m_frustumZFar, m_projectionMatrix);
        b3CreateLookAt(Vector3f(5, 5, 5), Vector3f(0,0,0),Vector3f(0, 1, 0), m_viewMatrix);
    }
    //-----------------------------------------------------------------------
    void RenderWidget::OnPaint(wxPaintEvent& e)
    {
        if (mContext == 0)
            return;
        mContext->setCurrent();
        //wxPaintDC dc(this);
        /*RenderSys * sys = N_EnginePtr()->getRender();
        int movex;
        int movey;
        sys->getConfigData()->get(N_PropertyWindow_wxWidgetMoveY, movey);
        sys->getConfigData()->get(N_PropertyWindow_wxWidgetMoveX, movex);
        Move(-movex, -movey);*/
        //dc.SetDeviceOrigin(500, 500);
        // This is required even though dc is not used otherwise.
        // Set the OpenGL viewport according to the client size of this canvas.
        // This is done here rather than in a wxSizeEvent handler because our
        // OpenGL rendering context (and thus viewport setting) is used with
        // multiple canvases: If we updated the viewport in the wxSizeEvent
        // handler, changing the size of one canvas causes a viewport setting that
        // is wrong when next another canvas is repainted.
        glClearColor(0.347f, 0.347f, 0.347f, 0.0f);
        // set up the parameters we want to use
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_TEXTURE_2D);

        // add slightly more light, the default lighting is rather dark
        GLfloat ambient[] = { 1.0, 1.0, 0.0, 1.0 };
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

        // set viewing projection
        //glMatrixMode(GL_PROJECTION);
        //glLoadIdentity();
        //glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
        const wxSize ClientSize = GetClientSize();
        glViewport(0, 0, ClientSize.x, ClientSize.y);

        // Render the graphics and swap the buffers.
        GLboolean quadStereoSupported;
        glGetBooleanv(GL_STEREO, &quadStereoSupported);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //glTranslatef(0.0f, 0.0f, -2.0f);
        //glRotatef(1, 1.0f, 0.0f, 0.0f);
        //glRotatef(1, 0.0f, 1.0f, 0.0f);

        DrawGridData gdata;

        drawGrid(gdata);
        drawLine((const NIIf *)Vector4f(0, 0, 0), (const NIIf *)Vector4f(1, 0, 0), (const NIIf *)Vector4f(1, 0, 0), 3);
        drawLine((const NIIf *)Vector4f(0, 0, 0), (const NIIf *)Vector4f(0, 1, 0), (const NIIf *)Vector4f(0, 1, 0), 3);
        drawLine((const NIIf *)Vector4f(0, 0, 0), (const NIIf *)Vector4f(0, 0, 1), (const NIIf *)Vector4f(0, 0, 1), 3);

        drawPoint((const NIIf *)Vector4f(1, 0, 0), (const NIIf *)Vector4f(1, 0, 0), 6);
        drawPoint((const NIIf *)Vector4f(0, 1, 0), (const NIIf *)Vector4f(0, 1, 0), 6);
        drawPoint((const NIIf *)Vector4f(0, 0, 1), (const NIIf *)Vector4f(0, 0, 1), 6);

        glFlush();
        CheckGLError();
        SwapBuffers();
    }
    //-----------------------------------------------------------------------
    void RenderWidget::drawGrid(const DrawGridData & data)
    {
        float lineWidth = 1;
        //b3Clamp(lineWidth, (float)lineWidthRange[0], (float)lineWidthRange[1]);
        glLineWidth(lineWidth);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        b3Assert(glGetError() == GL_NO_ERROR);
        glUseProgram(linesShader1);
        glUniformMatrix4fv(lines_ProjectionMatrix1, 1, false, &m_projectionMatrix[0]);
        glUniformMatrix4fv(lines_ModelViewMatrix1, 1, false, &m_viewMatrix[0]);
        glUniform4f(lines_colour1, data.gridColor[0], data.gridColor[1], data.gridColor[2], data.gridColor[3]);
        glUniform4f(lines_other1, data.gridSize, data.upOffset, data.upAxis, 0);

        glBindVertexArray(linesVertexArrayObject);

        b3Assert(glGetError() == GL_NO_ERROR);
        {

            glDrawArraysInstanced(GL_LINES, 0, 16, (data.gridSize + 1) * 2);
        }

        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        b3Assert(glGetError() == GL_NO_ERROR);
        glBindVertexArray(0);
        b3Assert(glGetError() == GL_NO_ERROR);
        glPointSize(1);
        b3Assert(glGetError() == GL_NO_ERROR);
        glUseProgram(0);
    }
    //------------------------------------------------------------------------
    void RenderWidget::drawLine(const float from[4], const float to[4], const float color[4], float lineWidth)
    {
        b3Assert(glGetError() == GL_NO_ERROR);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        b3Assert(glGetError() == GL_NO_ERROR);

        glUseProgram(linesShader);

        b3Assert(glGetError() == GL_NO_ERROR);

        glUniformMatrix4fv(lines_ProjectionMatrix, 1, false, &m_projectionMatrix[0]);
        glUniformMatrix4fv(lines_ModelViewMatrix, 1, false, &m_viewMatrix[0]);
        glUniform4f(lines_colour, color[0], color[1], color[2], color[3]);

        b3Assert(glGetError() == GL_NO_ERROR);


        b3Assert(glGetError() == GL_NO_ERROR);

        glLineWidth(lineWidth);

        b3Assert(glGetError() == GL_NO_ERROR);

        glBindVertexArray(lineVertexArrayObject);
        b3Assert(glGetError() == GL_NO_ERROR);

        glBindBuffer(GL_ARRAY_BUFFER, lineVertexBufferObject);
        b3Assert(glGetError() == GL_NO_ERROR);

        const float vertexPositions[] = {
            from[0], from[1], from[2], 1,
            to[0], to[1], to[2], 1 };
        int sz = sizeof(vertexPositions);

        glBufferSubData(GL_ARRAY_BUFFER, 0, sz, vertexPositions);
        b3Assert(glGetError() == GL_NO_ERROR);

        glEnableVertexAttribArray(lines_position);
        glVertexAttribPointer(lines_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
        b3Assert(glGetError() == GL_NO_ERROR);

        glDrawArrays(GL_LINES, 0, 2);
        b3Assert(glGetError() == GL_NO_ERROR);

        glBindVertexArray(0);
        glLineWidth(1);

        b3Assert(glGetError() == GL_NO_ERROR);
        glUseProgram(0);
    }
    //------------------------------------------------------------------------
    void RenderWidget::drawPoint(const float* positions, const float color[4], float pointDrawSize)
    {
        drawPoints(positions, color, 1, 3 * sizeof(float), pointDrawSize);
    }
    //------------------------------------------------------------------------
    void RenderWidget::drawPoints(const float* positions, const float color[4], int numPoints, int pointStrideInBytes, float pointDrawSize)
    {
        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        b3Assert(glGetError() == GL_NO_ERROR);
        glUseProgram(linesShader);
        glUniformMatrix4fv(lines_ProjectionMatrix, 1, false, &m_projectionMatrix[0]);
        glUniformMatrix4fv(lines_ModelViewMatrix, 1, false, &m_viewMatrix[0]);
        glUniform4f(lines_colour, color[0], color[1], color[2], color[3]);

        glPointSize(pointDrawSize);
        glBindVertexArray(lineVertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, lineVertexBufferObject);

        int maxPointsInBatch = MAX_POINTS_IN_BATCH;
        int remainingPoints = numPoints;
        int offsetNumPoints = 0;
        while (1)
        {
            int curPointsInBatch = N_MIN(int, maxPointsInBatch, remainingPoints);
            if (curPointsInBatch)
            {
                glBufferSubData(GL_ARRAY_BUFFER, 0, curPointsInBatch * pointStrideInBytes, positions + offsetNumPoints * (pointStrideInBytes / sizeof(float)));
                glEnableVertexAttribArray(lines_position);
                int numFloats = 3;  // pointStrideInBytes / sizeof(float);
                glVertexAttribPointer(lines_position, numFloats, GL_FLOAT, GL_FALSE, pointStrideInBytes, 0);
                glDrawArrays(GL_POINTS, 0, curPointsInBatch);
                remainingPoints -= curPointsInBatch;
                offsetNumPoints += curPointsInBatch;
            }
            else
            {
                break;
            }
        }

        glBindVertexArray(0);
        glPointSize(1);
        glUseProgram(0);
    }
    //-----------------------------------------------------------------------
    // RenderWidget TODO:
    // - resize events et al
    // - Change aspect ratio
    wxWidgetWindow::wxWidgetWindow() : 
        ViewWindow(0),
        mWindow(0),
        mContext(0),
        mRenderWidget(0)
    {
        mWidth = 0;
        mHeight = 0;
    }
    //-----------------------------------------------------------------------
    wxWidgetWindow::~wxWidgetWindow() 
    {
        if (mRenderWidget)
            delete mRenderWidget;
        if (mContext)
            delete mContext;
    }
    //-----------------------------------------------------------------------
    RenderWidget * wxWidgetWindow::getRenderWidget()
    {
        return mRenderWidget;
    }
    //-----------------------------------------------------------------------
    void wxWidgetWindow::setup(const String & name, NCount width, NCount height, bool full, const PropertyData * params)
    {
        mName = name;

        int left;
        int top;
        int depthBuffer;
        String title;

        PropertyData::PropertyList::const_iterator opt, optend = params->getList().end();
        for (opt = params->getList().begin(); opt != optend; ++opt)
        {
            switch (opt->first)
            {
            case N_PropertyWindow_Title:
                title = opt->second;
                break;
            case N_PropertyWindow_Left:
                StrConv::conv(opt->second, left);
                break;
            case N_PropertyWindow_Width:
                if (width == 0)
                    StrConv::conv(opt->second, width);
                break;
            case N_PropertyWindow_Height:
                if (height == 0)
                    StrConv::conv(opt->second, height);
                break;
            case N_PropertyWindow_FullWindow:
                StrConv::conv(opt->second, full);
                break;
            case N_PropertyWindow_Top:
                StrConv::conv(opt->second, top);
                break;
            case N_PropertyWindow_DepthBuffer:
                StrConv::conv(opt->second, depthBuffer);
                break;
            }
        }
        if (params)
        {
            void * tempptr;
            params->getData(N_PropertyWindow_wxWidgetPanel, tempptr);
            if (tempptr)
            {
                mWindow = static_cast<wxWindow *>(tempptr);
                mFullMode = false;

                //int stereoAttribList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
                int stereoAttribList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_STEREO, 0 };
                mRenderWidget = new RenderWidget(mWindow, wxSize(width, height), stereoAttribList);
                mContext = new wxWidgetContext(mRenderWidget);
                //mWindow->SetMinSize(wxSize(width , height));
            }
        }
        mWidth = width;
        mHeight = height;
    }
    //-----------------------------------------------------------------------
    void wxWidgetWindow::destroy()
    {
        Engine::getOnly().getRender()->remove(this);
    }
    //-----------------------------------------------------------------------
    void wxWidgetWindow::setFullMode(bool fullScreen, NCount width, NCount height)
    {
        /*if ((fullScreen) && (!mFullMode))
        {
            mFullMode = true;
            mWindow->fullscreen();
            mRenderWidget->set_size_request(width, height);
        }
        else if ((!fullScreen) && (mFullMode))*/
        {
            mFullMode = false;
            //mWindow->unfullscreen();
            mRenderWidget->SetSize(width, height);
        }
    }
    //-----------------------------------------------------------------------
    bool wxWidgetWindow::isActive() const
    {
        return mWindow->IsShown();
    }
    //-----------------------------------------------------------------------
    bool wxWidgetWindow::isClosed() const
    {
        return !mRenderWidget->IsShown();
    }
    //-----------------------------------------------------------------------
    void wxWidgetWindow::pos(NIIi left, NIIi top)
    {
        if (mWindow)
        {
            mWindow->SetPosition(wxPoint(left, top));
            mWindow->Refresh();
        }
    }
    //-----------------------------------------------------------------------
    void wxWidgetWindow::resize(NCount width, NCount height)
    {
        if (mWindow)
        {
            mWindow->SetSize(width, height);
            mWindow->Refresh();
        }
    }
    //-----------------------------------------------------------------------
    void wxWidgetWindow::swap(bool waitForVSync)
    {
        mRenderWidget->SwapBuffers();
    }
    //-----------------------------------------------------------------------
    void wxWidgetWindow::fill(FaceType buffer, const Box & src, PixelBlock & dst)
    {
        if (src.mRight > mWidth || src.mBottom > mHeight || src.mFront != 0 || src.mBack != 1 ||
            dst.getWidth() != src.getWidth() || dst.getHeight() != src.getHeight() || dst.getDepth() != 1)
        {
            N_EXCEPT(InvalidParam, _T("Invalid box."));
        }

        if (buffer == FT_Unknow)
        {
            buffer = mFullMode ? FT_Front : FT_Back;
        }

        static_cast<GLRenderSystemBase *>(N_Engine().getRender())->_copyContentsToMemory(getViewport(0), src, dst, buffer);
    }
    //-----------------------------------------------------------------------
    bool wxWidgetWindow::getPlatformData(const String & name, void * data) const
    {
        if (name == _T("GLCONTEXT"))
        {
            *static_cast<GLContext**>(data) = mContext;
            return true;
        }
        else if (name == _T("WXWIDGETWINDOW"))
        {
            wxWindow ** win = static_cast<wxWindow **>(data);
            *win = mWindow;
            return true;
        }
        else if (name == _T("WXWIDGETWIDGET"))
        {
            wxGLCanvas ** widget = static_cast<wxGLCanvas **>(data);
            *widget = mRenderWidget;
            return true;
        }
        else if (name == _T("isTexture"))
        {
            bool *b = reinterpret_cast<bool *>(data);
            *b = false;
            return true;
        }
        return ViewWindow::getPlatformData(name, data);
    }
    //-----------------------------------------------------------------------
    GLContext* wxWidgetWindow::getContext() const { return mContext; }
    //-----------------------------------------------------------------------
    void wxWidgetWindow::_notifyInit(RenderSys * sys)
    {
        mRenderWidget->setContext(mContext);
    }
    //-----------------------------------------------------------------------
}