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

#include "NiiPreProcess.h"
#include "NiiViewWindow.h"
#include "NiiEngine.h"
#include "NiiViewport.h"
#include "NiiTimer.h"

namespace NII
{
    //-----------------------------------------------------------------------------------
    ViewWindow::ViewWindow(Nui32 widthPt, Nui32 heightPt, bool full, const String & name) :
        mName( name ),
        mBuffer( 0 ),
        mDepth( 0 ),
        mStencil( 0 ),
        mDisplayFrequency( 0 ),
        mDisplayDenominator( 0 ),
        mViewWidth( widthPt ),
        mViewHeight( heightPt ),
        mFullMode( false ),
        mNextFullMode( full ),
        mBorderless( false ),
        mAutoDeactive(true),
        mActive( true ),
        mFirstDev( false ),
        mVSync( false ),
        mVSyncInterval( 1u ),
        mFSAA(0),
        mLeft( 0 ),
        mTop( 0 )
    {
    }
    //-----------------------------------------------------------------------------------
    ViewWindow::~ViewWindow()
    {
        assert( !mBuffer && "Derived class didn't properly free resources" );
        assert( !mDepth && "Derived class didn't properly free resources" );
        assert( !mStencil && "Derived class didn't properly free resources" );
    }
    //-----------------------------------------------------------------------------------
    void ViewWindow::setFullMode( bool full, Nui32 viewWidth, Nui32 viewHeight, Nui32 frequency, Nui32 denominator, 
        bool borderless, Nui32 monitorIdx)
    {
        mNextFullMode = full;
        mViewWidth = viewWidth;
        mViewHeight = viewHeight;
        mDisplayFrequency = frequency;
        mDisplayDenominator = denominator;
        mBorderless = borderless;
    }
    //-----------------------------------------------------------------------------------
    void ViewWindow::setName( const String & title )
    {
        mName = title;
    }
    //-----------------------------------------------------------------------
    void ViewWindow::notify()
    {
    }
    //-----------------------------------------------------------------------
    bool ViewWindow::isVisible() const
    {
        return true;
    }
    //-----------------------------------------------------------------------
    void ViewWindow::setVisible(bool set)
    {
        (void)set;
    }
    //-----------------------------------------------------------------------
    bool ViewWindow::isHidden() const
    {
        return false;
    }
    //-----------------------------------------------------------------------
    void ViewWindow::setHidden(bool set)
    {
        (void)set;
    }
    //-----------------------------------------------------------------------
    void ViewWindow::setVSync(bool set)
    {
        mVSync = set;
    }
    //-----------------------------------------------------------------------
    bool ViewWindow::isVSync() const
    {
        return mVSync;
    }
    //-----------------------------------------------------------------------
    void ViewWindow::setSwapInterval(NCount count)
    {
        mVSyncInterval = interval;
    }
    //-----------------------------------------------------------------------
    NCount ViewWindow::getSwapInterval() const
    {
        return mVSyncInterval;
    }
    //-----------------------------------------------------------------------
    bool ViewWindow::isActive() const
    {
        return mActive && isVisible();
    }
    //-----------------------------------------------------------------------
    void ViewWindow::getArea(NIIi & left, NIIi& top, Nui32 & w, Nui32 & h) const
    {
        left = mLeft;
        top = mTop;
        if (mBuffer)
        {
            w = mBuffer->getWidth();
            h = mBuffer->getHeight();
        }
        else
        {
            float scale = getViewRatio();
            w = (Nui32)floorf(mViewWidth * scale + 0.5f);
            h = (Nui32)floorf(mViewHeight * scale + 0.5f);
        }
    }
    //-----------------------------------------------------------------------
    bool ViewWindow::isFullMode() const
    {
        return mFullMode;
    }
    //-----------------------------------------------------------------------
    bool ViewWindow::isFirstWindow() const
    {
        return mFirstDev;
    }
    //-----------------------------------------------------------------------
    void ViewWindow::_notifyCreate(RenderSys *)
    {

    }
    //-----------------------------------------------------------------------
    void ViewWindow::_notifyInit(RenderSys *)
    {

    }
    //-----------------------------------------------------------------------
    float ViewWindow::getViewRatio() const              
    { 
        return 1.0f; 
    }
    //-----------------------------------------------------------------------
    void ViewWindow::setFsaa(const String & fsaa)       
    { 
        mFSAAExt = fsaa; 
    }
    //-----------------------------------------------------------------------
    Nui32 ViewWindow::getFSAA() const
    { 
        return mFSAA; 
    }
    //-----------------------------------------------------------------------
    void ViewWindow::screenshots(const String & filename)
    {
        PixelFormat pf = getFormat();

        Nui8 * data = N_alloc_t(Nui8, mWidth * mHeight * PixelUtil::getUnitSize(pf));
        PixelBlock pb(mWidth, mHeight, 1, pf, data);

        fill(FT_Unknow, pb, pb);

        Image().load(data, mWidth, mHeight, 1, pf, false, 1, 0).save(filename);

        N_free(data);
    }
    //-----------------------------------------------------------------------
    String ViewWindow::screenshots(const String & filenamePrefix, const String & filenameSuffix)
    {
        struct tm *pTime;
        time_t ctTime; time(&ctTime);
        pTime = localtime(&ctTime);
        StringStream oss;
        oss << std::setw(2) << std::setfill(_T('0')) << (pTime->tm_mon + 1)
            << std::setw(2) << std::setfill(_T('0')) << pTime->tm_mday
            << std::setw(2) << std::setfill(_T('0')) << (pTime->tm_year + 1900)
            << _T("_") << std::setw(2) << std::setfill(_T('0')) << pTime->tm_hour
            << std::setw(2) << std::setfill(_T('0')) << pTime->tm_min
            << std::setw(2) << std::setfill(_T('0')) << pTime->tm_sec
            << std::setw(3) << std::setfill(_T('0')) << (N_Engine().getTimer()->getMS() % 1000);
        String filename = filenamePrefix + oss.str() + filenameSuffix;
        screenshots(filename);
        return filename;
    }
    //-----------------------------------------------------------------------------------
    void ViewWindow::setSize(Nui32 width, Nui32 height)
    {
        if( mBuffer )
            mBuffer->setResolution(width, height, 1u);
        if( mDepth )
            mDepth->setResolution(width, height, 1u);
        if( mStencil )
            mStencil->setResolution(width, height, 1u);
    }
    //-----------------------------------------------------------------------------------
    void ViewWindow::resize(Nui32 w, Nui32 h)
    {
        mViewWidth = w;
        mViewHeight = h;
    }
    //-----------------------------------------------------------------------------------
    void ViewWindow::setBorderless(bool set)
    {
        mBorderless = set;
    }
    //-----------------------------------------------------------------------------------
    float ViewWindow::getViewRatio() const
    { 
        return 1.0f; 
    }
    //-----------------------------------------------------------------------------------
    void ViewWindow::setActive(bool set)
    {
        mActive = set;
    }
    //-----------------------------------------------------------------------------------
}