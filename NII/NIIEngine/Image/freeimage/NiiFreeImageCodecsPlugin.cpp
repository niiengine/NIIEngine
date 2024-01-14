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

#include "NiiFreeImageCodecPlugin.h"
#include "NiiFreeImageCodec.h"
#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include <FreeImage.h>

/**
	FIF_UNKNOWN	
Unknown format (returned value only, never use it as input value)
FIF_BMP	
Windows or OS/2 Bitmap File (*.BMP)
FIF_ICO	
Windows Icon (*.ICO)
FIF_JPEG	
Independent JPEG Group (*.JPG, *.JIF, *.JPEG, *.JPE)
FIF_JNG	
JPEG Network Graphics (*.JNG)
FIF_KOALA	
Commodore 64 Koala format (*.KOA)
FIF_LBM	
Amiga IFF (*.IFF, *.LBM)
FIF_IFF	
Amiga IFF (*.IFF, *.LBM)
FIF_MNG	
Multiple Network Graphics (*.MNG)
FIF_PBM	
Portable Bitmap (ASCII) (*.PBM)
FIF_PBMRAW	
Portable Bitmap (BINARY) (*.PBM)
FIF_PCD	
Kodak PhotoCD (*.PCD)
FIF_PCX	
Zsoft Paintbrush PCX bitmap format (*.PCX)
FIF_PGM	
Portable Graymap (ASCII) (*.PGM)
FIF_PGMRAW	
Portable Graymap (BINARY) (*.PGM)
FIF_PNG	
Portable Network Graphics (*.PNG)
FIF_PPM	
Portable Pixelmap (ASCII) (*.PPM)
FIF_PPMRAW	
Portable Pixelmap (BINARY) (*.PPM)
FIF_RAS	
Sun Rasterfile (*.RAS)
FIF_TARGA	
truevision Targa files (*.TGA, *.TARGA)
FIF_TIFF	
Tagged Image File Format (*.TIF, *.TIFF)
FIF_WBMP	
Wireless Bitmap (*.WBMP)
FIF_PSD	
Adobe Photoshop (*.PSD)
FIF_CUT	
Dr. Halo (*.CUT)
FIF_XBM	
X11 Bitmap Format (*.XBM)
FIF_XPM	
X11 Pixmap Format (*.XPM)
FIF_DDS	
DirectDraw Surface (*.DDS)
FIF_GIF	
Graphics Interchange Format (*.GIF)
FIF_HDR	
High Dynamic Range (*.HDR)
FIF_FAXG3	
Raw Fax format CCITT G3 (*.G3)
FIF_SGI	
Silicon Graphics SGI image format (*.SGI)
FIF_EXR	
OpenEXR format (*.EXR)
FIF_J2K	
JPEG-2000 format (*.J2K, *.J2C)
FIF_JP2	
JPEG-2000 format (*.JP2)
FIF_PFM	
Portable FloatMap (*.PFM)
FIF_PICT	
Macintosh PICT (*.PICT)
FIF_RAW	
RAW camera image (*.*)
*/
namespace NII
{
    //-----------------------------------------------------------------------
    FreeImageCodecPlugin::RegisteredCodecList FreeImageCodecPlugin::mCodecList;
    //-----------------------------------------------------------------------
    void FreeImageLoadErrorHandler(FREE_IMAGE_FORMAT fif, const NIIb * message)
    {
        // Callback method as required by FreeImage to report problems
        const NIIb * tt = FreeImage_GetFormatFromFIF(fif);
        if (tt)
        {
            N_Only(Log).stream() << _T("FreeImage error: '") << message << 
                _T("' when loading format ") << tt;
        }
        else
        {
            N_Only(Log).stream() << _T("FreeImage error: '") << message << "'";
        }
    }
    //-----------------------------------------------------------------------
    FreeImageCodecPlugin::FreeImageCodecPlugin()
    {
    }
    //----------------------------------------------------------------------
    const String & FreeImageCodecPlugin::getVerbose()const
    {
        static const String des = "support jpeg png tiff etc.. image format";
        return des;
    }
    //----------------------------------------------------------------------
    const String & FreeImageCodecPlugin::getName() const
    {
        static const String name = "NII-Plugin-FreeImage";
        return name;
    }
    //----------------------------------------------------------------------
    void FreeImageCodecPlugin::install()
    {
        FreeImage_Initialise(false);

        // ×¢²á±àÂëÆ÷
        StringStream strExt;
        strExt << _T("Supported formats: ");
        bool first = true;
        for (NIIi i = 0; i < FreeImage_GetFIFCount(); ++i)
        {
            // Skip DDS codec since FreeImage does not have the option
            // to keep DXT data compressed, we'll use our own codec
            if ((FREE_IMAGE_FORMAT)i == FIF_DDS)
                continue;

            String exts(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
            if (!first)
            {
                strExt << ",";
            }
            first = false;
            strExt << exts;

            // Pull off individual formats (separated by comma by FI)
            StringList extsVector;
            StrUtil::split(exts, extsVector, ",");
            for (StringList::iterator v = extsVector.begin(); v != extsVector.end(); ++v)
            {
                // FreeImage 3.13 lists many formats twice: once under their own codec and
                // once under the "RAW" codec, which is listed last. Avoid letting the RAW override
                // the dedicated codec!
                if (!Codec::isExist(*v))
                {
                    ImageCodec * codec = N_new FreeImageCodec(0, *v, i);
                    mCodecList.push_back(codec);
                    Codec::add(codec);
                }
            }
        }
        N_Only(Log).log(LML_NORMAL,strExt.str());

        // Set error handler
        FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);
    }
    //----------------------------------------------------------------------
    void FreeImageCodecPlugin::init()
    {
        // nothing
    }
    //----------------------------------------------------------------------
    void FreeImageCodecPlugin::shutdown()
    {
        // nothing
    }
    //----------------------------------------------------------------------
    void FreeImageCodecPlugin::uninstall()
    {
		FreeImage_DeInitialise();
        RegisteredCodecList::iterator i, iend = mCodecList.end();
		for (i = mCodecList.begin(); i != iend; ++i)
		{
			Codec::remove(*i);
			N_delete *i;
		}
		mCodecList.clear();        
    }
    //----------------------------------------------------------------------
    NIIb FreeImageCodecPlugin::getComplex()const
    {
        return 9;
    }
    //----------------------------------------------------------------------
    NIIb FreeImageCodecPlugin::getExtention()const
    {
        return 8;
    }
    //----------------------------------------------------------------------
    const String & FreeImageCodecPlugin::getOwnVersion() const
    {
        static const String version = FreeImage_GetVersion();
        return version;
    }
    //----------------------------------------------------------------------
    const String & FreeImageCodecPlugin::getLisence() const
    {
        static const String lis = FreeImage_GetCopyrightMessage();
        return lis;
    }
    //----------------------------------------------------------------------
}