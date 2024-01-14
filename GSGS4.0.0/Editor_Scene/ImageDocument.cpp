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

#include "ImageDocument.h"
#include "ImageEditorView.h"
#include "StrHelper.h"
#include "NiiLogManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiPixelBufferManager.h"
#include "NiiDefaultStream.h"
#include "NiiEngine.h"
#include "NiiHash.h"
#include "NiiPropertyObj.h"
#include "NiiXmlAnalyzeList.h"
#include "NiiCommon.h"

#include "wx/wx.h"
#include "wx/filename.h"
#include "wx/docview.h"

using namespace NII;

namespace gsgs
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ImagesetHandler
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    N_PROPERTY(NII, ImagesetHandler, ImagesetElement, 0); // "Imageset"
    N_PROPERTY(NII, ImagesetHandler, ImageElement, 1); // _T("Image") 
    N_PROPERTY(NII, ImagesetHandler, ImagesetImageFileAttribute, 2); // "Imagefile";
    N_PROPERTY(NII, ImagesetHandler, ImagesetResourceGroupAttribute, 3); // "ResourceGroup"
    N_PROPERTY(NII, ImagesetHandler, ImagesetNameAttribute, 4); // "Name"
    N_PROPERTY(NII, ImagesetHandler, ImagesetNativeHorzResAttribute, 5); // "NativeHorzRes"
    N_PROPERTY(NII, ImagesetHandler, ImagesetNativeVertResAttribute, 6); // "NativeVertRes"
    N_PROPERTY(NII, ImagesetHandler, ImagesetAutoScaledAttribute, 7); // "AutoScaled"
    N_PROPERTY(NII, ImagesetHandler, ImageNameAttribute, 8); // "Name"
    N_PROPERTY(NII, ImagesetHandler, ImageXPosAttribute, 9); // "XPos"
    N_PROPERTY(NII, ImagesetHandler, ImageYPosAttribute, 10); // "YPos"
    N_PROPERTY(NII, ImagesetHandler, ImageWidthAttribute, 11); // "Width"
    N_PROPERTY(NII, ImagesetHandler, ImageHeightAttribute, 12); // "Height"
    N_PROPERTY(NII, ImagesetHandler, ImageXOffsetAttribute, 13); // "XOffset"
    N_PROPERTY(NII, ImagesetHandler, ImageYOffsetAttribute, 14); // "YOffset"
    //-----------------------------------------------------------------------
    void ImagesetHandler::onBegin(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc)
    {
        if (pid == ImageElement )
        {
            wxString name;
            String tempstr;
            if (pdc->isExist(ImageNameAttribute))
            {
                pdc->get(ImageNameAttribute, tempstr);
                name = StrHelper::ToWXString(tempstr);
            }
            wxRect  rect;
            int tempi;
            if (pdc->isExist(ImageXPosAttribute))
            {
                pdc->get(ImageXPosAttribute, tempi);
                rect.SetX(tempi);
            }
            if (pdc->isExist(ImageYPosAttribute))
            {
                pdc->get(ImageYPosAttribute, tempi);
                rect.SetY(tempi);
            }
            if (pdc->isExist(ImageWidthAttribute))
            {
                pdc->get(ImageWidthAttribute, tempi);
                rect.SetWidth(tempi);
            }
            if (pdc->isExist(ImageHeightAttribute))
            {
                pdc->get(ImageHeightAttribute, tempi);
                rect.SetHeight(tempi);
            }

            wxPoint offset;

            if (pdc->isExist(ImageXOffsetAttribute))
            {
                pdc->get(ImageXOffsetAttribute, offset.x, 0);
            }
            if (pdc->isExist(ImageYOffsetAttribute))
            {
                pdc->get(ImageYOffsetAttribute, offset.y, 0);
            }

            m_document->addRegion( name, rect );
        }
        else if (pid == ImagesetElement )
        {
            String tempstr;
            if ( pdc->isExist ( ImagesetNameAttribute ) )
            {
                pdc->get(ImagesetNameAttribute, tempstr);
                wxString    name( StrHelper::ToWXString( tempstr ) );
                m_document->setImagesetName( name );
            }

            if ( pdc->isExist ( ImagesetImageFileAttribute ) )
            {
                pdc->get(ImagesetImageFileAttribute, tempstr);
                wxString    filename( StrHelper::ToWXString( tempstr ) );
                m_document->setImageFilename( filename );
            }

            //
            // load auto-scaling configuration
            //
            int hres, vres;

            if ( pdc->isExist ( ImagesetNativeHorzResAttribute ) )
            {
                // get native horizontal resolution
                pdc->get( ImagesetNativeHorzResAttribute, hres, 640 );
            }
            if ( pdc->isExist ( ImagesetNativeVertResAttribute ) )
            {
                // get native vertical resolution
                pdc->get( ImagesetNativeVertResAttribute, vres, 480 );
            }
            m_document->setNativeResolution( wxPoint( hres, vres ) );


            bool autoscal = false;
            if ( pdc->isExist ( ImagesetAutoScaledAttribute ) )
            {
                // enable / disable auto-scaling for this Imageset according to the setting
                pdc->get( ImagesetAutoScaledAttribute, autoscal, false );
            }
            m_document->setAutoScaled( autoscal );
        }
    }
    //-----------------------------------------------------------------------
    void ImagesetHandler::onEnd(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc)
    {
        if (pid == ImagesetElement )
        {
            //Logger::getSingleton().logEvent("Finished creation of Imageset '" + d_imageset->d_name + "' via XML file.", Informative);
            m_document->Modify( true );
        }
    }
    //-----------------------------------------------------------------------
    void ImagesetHandler::PrcTagUnitCB(const PropertyData * pdc)
    {

    }
    //-----------------------------------------------------------------------
    void ImagesetHandler::PrcTagUnitECB(const PropertyData * pdc)
    {

    }
    //-----------------------------------------------------------------------
    XmlUnitID ImagesetHandler::getUnit() const
    {
        return N_XML_TAG_Custom + 1;
    }
    //-----------------------------------------------------------------------
    GroupID ImagesetHandler::getGroup() const
    {
        return N_Only(Widget).getGroup();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ImageDocument
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    IMPLEMENT_DYNAMIC_CLASS( ImageDocument, wxDocument )
    //-----------------------------------------------------------------------
    // Declared in Imageset::xmlHandler
    const wxPoint ImageDocument::c_defaultNativeResolution( wxPoint( 640, 480 ) );
    //-----------------------------------------------------------------------
    ImageDocument::ImageDocument()
    {
        reset();
    }
    //-----------------------------------------------------------------------
    bool ImageDocument::OnOpenDocument( const wxString& filename )
    {
        if ( filename.empty() || !wxDocument::OnOpenDocument( filename ) )
            return false;

        wxString relFilename( getDefaultResourceGroupRelativePath( filename ) );

        // create handler object
        ImagesetHandler handler( this );

        // This could take some time, enter 'busy' state
        wxWindowDisabler wd; wxBusyCursor bc;

        // do parse (which uses handler to create actual data)
        try
        {
            FileDataStream * file = N_new FileDataStream(StrHelper::ToNIIString(relFilename));
            N_Engine().getXmlPattern()->parse(&handler, file);
            N_delete file;
        }
        catch ( ... )
        {
            N_log( _T("ImageDocument::onOpenDocument - loading of Imageset from file '") +
                StrHelper::ToNIIString( filename ) + _T("' failed."));
            throw;
        }

        // Since we have overwritten this method, we must notify the views ourselves!
        UpdateAllViews();

        SetFilename( filename, true );
        SetTitle( filename );

        Modify( false );

        return true;
    }
    //-----------------------------------------------------------------------
    bool ImageDocument::OnSaveDocument( const wxString& filename )
    {
        try
        {
            PixelBufferGroup * newImageset = generateRealImageset();

            ImagesetHandler handler(this);

            Nfstream fileSave;

            fileSave.open ( static_cast<const char*>( wxConvLibc.cWX2MB( filename ) ), std::ios::out ) ;
            if ( !fileSave.is_open() )
            {
                wxLogError ( wxT( "Error: could not open file for saving. Is it read-only?" ) ) ;
                return false;
            }

            N_Engine().getXmlPattern()->read(&handler, StrHelper::ToNIIString(m_imagesetName), &fileSave);
            fileSave.close() ;
            
            // cleanup imageset
            N_Only(PixelBuffer).destroyGroup(newImageset->getGroupID() );

            // Tell the document it's "clean" again
            Modify(false);
        }
        catch (...)
        {
            N_log(_T( "ImageDocument::OnSaveDocument - Error") );
            return false;
        }

        return true;
    }
    //-----------------------------------------------------------------------
    bool ImageDocument::OnCloseDocument()
    {
        reset();
        return true;
    }
    //-----------------------------------------------------------------------
    bool ImageDocument::OnNewDocument()
    {
        wxDocument::OnNewDocument();

        reset();

        // Since we have overwritten this method, we must notify the views ourselves!
        UpdateAllViews();

        return true;
    }
    //-----------------------------------------------------------------------
    void ImageDocument::reset()
    {
        deleteAllRegions();
        setImagesetName( wxT( "" ) );
        setImageFilename( wxT( "" ) );
        setNativeResolution( c_defaultNativeResolution );
        setAutoScaled( false );
        Modify( false );
    }
    //-----------------------------------------------------------------------
    void ImageDocument::setImageFilename( const wxString& filename )
    {
        if ( m_imageFilename != filename)
        {
            m_imageFilename = filename;

            if ( isViewValid() )
                getView()->onChangeImageFile(filename);

            Modify( true );
        }
    }
    //-----------------------------------------------------------------------
    void ImageDocument::setImagesetName( const wxString& newname )
    {
        if ( m_imagesetName != newname )
        {
            m_imagesetName = newname;

            if ( isViewValid() )
                getView()->onChangeImagesetName( newname );

            Modify( true );
        }
    }
    //-----------------------------------------------------------------------
    void ImageDocument::setNativeResolution( const wxPoint& newnativeres )
    {
        if ( m_nativeResolution != newnativeres )
        {
            m_nativeResolution = newnativeres;

            if ( isViewValid() )
                getView()->onChangeNativeResolution( newnativeres );

            Modify( true );
        }
    }
    //-----------------------------------------------------------------------
    void ImageDocument::setAutoScaled( const bool newAutoScaled )
    {
        if ( m_autoScaled != newAutoScaled )
        {
            m_autoScaled = newAutoScaled;

            if ( isViewValid() )
                getView()->onChangeAutoScaled( newAutoScaled );

            Modify( true );
        }
    }
    //-----------------------------------------------------------------------
    void ImageDocument::addRegion( const wxString& name, const wxRect& dim )
    {
        m_mapSetRectangles[ name ] = dim;

        if ( isViewValid() )
            getView()->onAddRegion( name, dim );

        Modify( true );
    }
    //-----------------------------------------------------------------------
    void ImageDocument::deleteRegion( const wxString& name )
    {
        m_mapSetRectangles.erase( name );

        if ( isViewValid() )
            getView()->onDelRegion( name );

        Modify( true );
    }
    //-----------------------------------------------------------------------
    void ImageDocument::deleteAllRegions()
    {
        m_mapSetRectangles.clear();

        if ( isViewValid() )
            getView()->onDelAllRegion();

        Modify( true );
    }
    //-----------------------------------------------------------------------
    void ImageDocument::setRegionArea( const wxString& name, const wxRect& dim, const bool evt_src )
    {
        // if area is the same, do nothing (saves infinite recursion possibility)
        if (getRegionArea(name) == dim)
            return;

        m_mapSetRectangles[ name ] = dim;

        if ( isViewValid() )
            getView()->onSetRegionArea(name, dim, evt_src);

        Modify( true );
    }
    //-----------------------------------------------------------------------
    wxRect ImageDocument::getRegionArea( const wxString& name ) const
    {
        wxRect area;

        mapNamedRegion::const_iterator iter = m_mapSetRectangles.find( name );

        if ( iter != m_mapSetRectangles.end() )
            area = iter->second;

        return area;
    }
    //-----------------------------------------------------------------------
    void ImageDocument::renameRegion( const wxString& currentname, const wxString& newname )
    {
        // check the new name does not exist in GUI
        if (N_Only(Widget).isExist(StrHelper::ToNIIString( newname ) ) )
        {
            wxLogError ( wxT( "ImageDocument::renameRegion - Error: A region with that name already exists." ) ) ;
        }
        // new name is fine, go ahead with the rename
        else
        {
            wxRect dim = m_mapSetRectangles[ currentname ];
            m_mapSetRectangles.erase( currentname );
            m_mapSetRectangles[ newname ] = dim;

            if ( isViewValid() )
                getView()->onRenameRegion( currentname, newname );

            Modify( true );
        }
    }
    //-----------------------------------------------------------------------
    void ImageDocument::Modify( bool modify )
    {
        wxDocument::Modify( modify );
        UpdateAllViews();
    }
    //-----------------------------------------------------------------------
    PixelBufferGroup * ImageDocument::generateRealImageset()
    {
        const String imagesetName ( StrHelper::ToNIIString( m_imagesetName ) );
        const String imageFilename ( StrHelper::ToNIIString( m_imageFilename ) );

        PixelBufferManager * isMgr = N_OnlyPtr(PixelBuffer);
        Nui32 pgid = NII::FastHash(imagesetName);
        // TODO: Maybe ask user whether to continue here?
        isMgr->destroyGroup(pgid);

        // create the imageset
        PixelBufferGroup * imageset = isMgr->create(pgid, imageFilename );

        // auto-scale options
        //imageset->setMode( m_autoScaled );
        imageset->setNativeRes(PlaneSizef( m_nativeResolution.x, m_nativeResolution.y ) );

        // iterate over all the elements in the class
        mapNamedRegion::iterator it;
        for( it = m_mapSetRectangles.begin(); it != m_mapSetRectangles.end(); ++it )
        {
            PixelBufferID pid = NII::FastHash(StrHelper::ToNIIString( wxString( it->first ) ));

            if ( !imageset->isExist(pid))
            {
                const wxRect r( it->second );
                const Rectf newDim( ( ( r.GetLeft() > 0 ) ? r.GetLeft() : 0 ),
                                   ( ( r.GetTop() > 0 ) ? r.GetTop() : 0 ),
                                   ( ( r.GetRight() + 1 > 0 ) ? r.GetRight() + 1 : 0 ),
                                   ( ( r.GetBottom() + 1 > 0 ) ? r.GetBottom() + 1 : 0 ) );
                PixelBuffer * bf = N_new PixelBuffer(pid, imageset, newDim);
                imageset->add(bf);
            }
        }

        imageset->remove(0 );

        return imageset;
    }
    //-----------------------------------------------------------------------
    bool ImageDocument::isViewValid() const
    {
        return GetFirstView() != 0;
    }
    //-----------------------------------------------------------------------
    ImageEditorView* ImageDocument::getView() const
    {
        return static_cast<ImageEditorView*>( GetFirstView() );
    }
    //-----------------------------------------------------------------------
    wxString ImageDocument::getDefaultResourceGroupRelativePath( const wxString& filename ) const
    {
        wxString resourcePath( wxT( "./" ) );
        wxFileName filespec( filename );

        if ( !N_Only(Widget).getGroup() )
            resourcePath = StrHelper::ToWXString(N_Only(ResourceScheme).getRelPath(N_Only(Widget).getGroup()) );

        filespec.MakeRelativeTo( resourcePath );
        return filespec.GetFullPath();
    }
    //-----------------------------------------------------------------------
}
