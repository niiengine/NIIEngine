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

#include "StrHelper.h"
#include "ImageEditorView.h"
#include "ImageEditorFrame.h"
#include "ImageGLCanvas.h"
#include "ImagePropertiesPanel.h"
#include "app.h"

#include "wx/wx.h" 
#include "wx/docview.h"

namespace gsgs
{
    //-----------------------------------------------------------------------
    IMPLEMENT_DYNAMIC_CLASS( ImageEditorView, wxView )
    //-----------------------------------------------------------------------
    ImageEditorView::ImageEditorView() :
        m_frame( 0 )
    {
    }
    //-----------------------------------------------------------------------
    bool ImageEditorView::OnCreate( wxDocument* document, long )
    {
        // Single-window mode
        m_frame = wxGetApp().GetImageEditorFrame();
        m_glcanvasImageset = m_frame->getCanvas();
        m_glcanvasImageset->setView ( this );
        m_glcanvasImageset->Reset();
        m_glcanvasImageset->Enable();
        m_propsPanel = m_frame->getPropsPanel();
        m_propsPanel->Enable();
        m_propsPanel->setView ( this );

        // Associate the appropriate frame with this view.
        SetFrame( m_frame );

        // Tell the frame about the document
        m_frame->SetDocument( static_cast<ImageDocument*>( document ) );

        // Make sure the document manager knows that this is the
        // current view.
        Activate( true );

        return true;
    }
    //-----------------------------------------------------------------------
    bool ImageEditorView::OnClose( bool WXUNUSED( deleteWindow ) )
    {
        if ( !GetDocument()->Close() )
            return false;

        m_glcanvasImageset->ClearBackground();
        m_glcanvasImageset->setView ( 0 );
        m_glcanvasImageset->Disable();
        m_glcanvasImageset = 0;

        m_propsPanel->reset();
        m_propsPanel->Disable();

        SetFrame( 0 );

        // Tell the frame about the document
        m_frame->SetDocument( 0 );
        m_frame = 0;

        Activate( false );

        return true;
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::OnUpdate( wxView* WXUNUSED( sender ), wxObject* WXUNUSED( hint ) )
    {
        if ( m_frame && m_frame->IsShown() )
        {
            //wxLogDebug(wxT("Needs repaint."));
            m_glcanvasImageset->Refresh( false );
            m_glcanvasImageset->Render();
        }
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::OnDraw( wxDC* dc ){}
    //-----------------------------------------------------------------------
    void ImageEditorView::onChangeFilename()
    {
        updateTitle();
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::onChangeImageFile( const wxString& newfilename )
    {
        m_propsPanel->setImageFilename( newfilename );
        m_glcanvasImageset->setImage( newfilename );
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::onChangeImagesetName( const wxString& newname )
    {
        m_propsPanel->setImagesetName( newname );
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::onChangeNativeResolution( const wxPoint& newnativeres )
    {
        m_propsPanel->setNativeResolution( newnativeres );
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::onChangeAutoScaled( const bool newAutoScaled )
    {
        m_propsPanel->setAutoScaled( newAutoScaled );
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::onAddRegion( const wxString& name, const wxRect& dim )
    {
        m_propsPanel->addRegion( name, dim );
        m_glcanvasImageset->addRegion( name, dim );
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::onDelRegion( const wxString& name )
    {
        m_propsPanel->deleteRegion( name );
        m_glcanvasImageset->deleteRegion( name );
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::onDelAllRegion()
    {
        m_propsPanel->deleteAllRegions();
        m_glcanvasImageset->deleteAllRegions();
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::onSetRegionArea( const wxString& name, const wxRect& dim, const bool evt_src )
    {
        m_propsPanel->setRegionArea( name, dim );

        if (!evt_src)
            m_glcanvasImageset->setRegionArea( name, dim );
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::onRenameRegion( const wxString& currentname, const wxString& newname )
    {
        m_glcanvasImageset->renameRegion( currentname, newname );
        m_propsPanel->renameRegion( currentname, newname );
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::updateTitle()
    {
        wxString title ( wxTheApp->GetAppName() );

        if ( GetDocument() )
        {
            title << wxT( "- [" ) << GetDocument()->GetFilename() << wxT( "]" );
        }
        if ( m_frame )
        {
            m_frame->SetTitle( title );
        }
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::updateMouseStatusBar( const float posx, const float posy )
    {
        if ( !m_frame )
            return;

        wxChar statusInfo [ 100 ];
        wxSnprintf(
            statusInfo,
            sizeof( statusInfo ) / sizeof( wxChar ),
            wxT( "pos[%d, %d]" ),
            static_cast<int>( posx ),
            static_cast<int>( posy ) ) ;

        m_frame->GetStatusBar()->SetStatusText( statusInfo, 0 );
    }
    //-----------------------------------------------------------------------
    void ImageEditorView::updateDocStatusBar( const float zoom, const float width, const float height )
    {
        if ( !m_frame )
            return;

        wxChar statusInfo [ 100 ];

        wxSnprintf(
            statusInfo,
            sizeof( statusInfo ) / sizeof( wxChar ),
            wxT( "size[%d, %d] zoom[%d%%]" ),
            static_cast<int>( width ),
            static_cast<int>( height ),
            static_cast<int>( zoom * 100.0f ) );

        m_frame->GetStatusBar()->SetStatusText( statusInfo, 1 );
    }
    //-----------------------------------------------------------------------
}