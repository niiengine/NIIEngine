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

#ifndef _ImageEDITOR_DOCUMENT_H_
#define _ImageEDITOR_DOCUMENT_H_

#include "wx/wx.h" 
#include "wx/docview.h"
#include "wx/hashmap.h"
#include "NiiPixelBuffer.h"
#include "NiiXmlAnalyzeManager.h"
#include "NiiXmlPattern.h"
#include "NiiDefine.h"

using namespace NII;

namespace gsgs
{
    class ImageDocument;

    /*!
    \brief
    Handler class used to parse the Imageset XML files using SAX2
    */
    class ImagesetHandler : public XmlAnalyze
    {
    public:
        ImagesetHandler( ImageDocument* document ) : XmlAnalyze(3000, N_Script_Custom, _T("Imageset.xsd")), m_document( document )
        {}
        virtual ~ImagesetHandler( void ) {}

        /*************************************************************************
        SAX2 Handler overrides
        *************************************************************************/
        /*!
        \brief
        document processing (only care about elements, schema validates format)
        */
        virtual void onBegin(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);
        virtual void onEnd(XmlAnalyze * parent, PropertyID pid, const PropertyData * pdc);

        void PrcTagUnitCB(const PropertyData * pdc);

        void PrcTagUnitECB(const PropertyData * pdc);

        virtual XmlUnitID getUnit() const;

        virtual GroupID getGroup() const;
    private:
        static const PropertyID ImagesetElement;             //!< Tag name for Imageset elements.
        static const PropertyID ImageElement;                    //!< Tag name for Image elements.
        static const PropertyID   ImagesetNameAttribute;        //!< Attribute name that stores the name of the Imageset
        static const PropertyID   ImagesetImageFileAttribute;   //!< Attribute name that stores the filename for the image file.
        static const PropertyID   ImagesetResourceGroupAttribute;   //!< Attribute name that stores the resource group identifier used when loading image file.
        static const PropertyID   ImagesetNativeHorzResAttribute;   //!< Optional attribute that stores 'native' horizontal resolution for the Imageset.
        static const PropertyID   ImagesetNativeVertResAttribute;   //!< Optional attribute that stores 'native' vertical resolution for the Imageset.
        static const PropertyID   ImagesetAutoScaledAttribute;  //!< Optional attribute that specifies whether the Imageset should be auto-scaled.
        static const PropertyID   ImageNameAttribute;           //!< Attribute name that stores the name of the new Image.
        static const PropertyID   ImageXPosAttribute;           //!< Attribute name that stores the x position of the new Image.
        static const PropertyID   ImageYPosAttribute;           //!< Attribute name that stores the y position of the new Image.
        static const PropertyID   ImageWidthAttribute;          //!< Attribute name that stores the width of the new Image.
        static const PropertyID   ImageHeightAttribute;         //!< Attribute name that stores the height of the new Image.
        static const PropertyID   ImageXOffsetAttribute;        //!< Attribute name that stores the x rendering offset of the new Image.
        static const PropertyID   ImageYOffsetAttribute;        //!< Attribute name that stores the y rendering offset of the new Image.

        /*************************************************************************
        Implementation Data
        *************************************************************************/
        ImageDocument* m_document;         //!< Holds a pointer to the Imageset that created the handler object
    };
    
    class ImageEditorView;

    /** The application's Document implementation.
    */
    class ImageDocument : public wxDocument
    {
        DECLARE_DYNAMIC_CLASS( ImageDocument )
    public:
        ImageDocument();

        /** Opens the given imageset file as editable.
        */
        bool OnOpenDocument( const wxString& filename );

        /** Starts a new imageset.
        */
        bool OnNewDocument() ;

        /** Saves the current imageset to its filepath.
        */
        bool OnSaveDocument( const wxString& filename );

        /** The current imageset has been closed. The system has prompted the user to save changes, if any.
        */
        bool OnCloseDocument();

        /** Sets the document to being 'dirty' and updates the attached views.*/
        void Modify ( bool modify );

        /** Resets the document for editing eg. after a resolution-change which re-starts Ogre.*/
        void reset() ;

        // possible actions on the document
        void setImageFilename( const wxString& newfilename );
        void setImagesetName( const wxString& newname );
        void setNativeResolution( const wxPoint& newnativeres );
        void setAutoScaled( const bool newAutoScaled );
        void addRegion( const wxString& name, const wxRect& dim );
        void deleteRegion( const wxString& name );
        void deleteAllRegions();
        void setRegionArea( const wxString& name, const wxRect& dim, const bool evt_src = false );
        void renameRegion( const wxString& currentname, const wxString& newname );

        //! Return current area for a region
        wxRect getRegionArea( const wxString& name ) const;
    private:
        //! returns whether we have a valid view assigned
        bool isViewValid() const;
        
        //! returns a pointer to our view (if any)
        ImageEditorView* getView() const;

        //! Helper that generates a real GUI imageset based upon the defined regions.*/
        PixelBufferGroup* generateRealImageset();

        //! Return the given path 'filename' relative to the default resource group directory
        wxString getDefaultResourceGroupRelativePath( const wxString& filename ) const;

        //! default value for the NativeResolution value.
        static const wxPoint c_defaultNativeResolution;

        //! Name of the Imageset
        wxString m_imagesetName;

        //! Filename of the source image.
        wxString m_imageFilename;

        // Document data
        wxPoint m_nativeResolution;

        // Document data
        bool m_autoScaled;

        // Document data
        WX_DECLARE_STRING_HASH_MAP( wxRect, mapNamedRegion );

        mapNamedRegion m_mapSetRectangles;
    };
}
#endif // _EDITOR_DOCUMENT_H_
