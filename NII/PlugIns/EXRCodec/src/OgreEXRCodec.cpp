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
#include "OgreStableHeaders.h"

#include "OgreRoot.h"
#include "OgreLogManager.h"
#include "OgreImage.h"
#include "OgreException.h"

#include "OgreEXRCodec.h"

#include "O_IStream.h"

#include <cmath>
#include <ImfOutputFile.h>
#include <ImfInputFile.h>
#include <ImfChannelList.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

using namespace Imath;
using namespace Imf;

namespace Ogre {

void writeEXRHalf(OStream *ost, const float *pixels,
	      int width, int height, int components) 
{
	//assert(components==3 || components==4);
	// TODO: throw std::exception if invalid number of components

	Header header (width, height);
	header.channels().insert ("R", Channel (HALF));
	header.channels().insert ("G", Channel (HALF));
	header.channels().insert ("B", Channel (HALF));
	if(components==4)
		header.channels().insert ("A", Channel (HALF));

	// Convert data to half
	half *data = new half [width*height*components];
	
	std::copy(pixels, pixels+(width*height*components), data);
	
	// And save it
	OutputFile file (*ost, header);
	FrameBuffer frameBuffer;

	frameBuffer.insert("R",				// name
			    Slice (HALF,		// type
				   ((char *) data)+0,	// base
				   2 * components,		// xStride
				   2 * components * width));	// yStride
	frameBuffer.insert("G",				// name
			    Slice (HALF,		// type
				   ((char *) data)+2,	// base
				   2 * components,		// xStride
				   2 * components * width));	// yStride
	frameBuffer.insert("B",				// name
			    Slice (HALF,		// type
				   ((char *) data)+4,	// base
				   2 * components,		// xStride
				   2 * components * width));	// yStride
	if(components==4) {
		frameBuffer.insert("A",					// name
				    Slice (HALF,			// type
					   ((char *) data)+6,		// base
					   2 * components,		// xStride
					   2 * components * width));	// yStride
	}

	file.setFrameBuffer(frameBuffer);
	file.writePixels(height);
	delete data;
}


EXRCodec::EXRCodec() 
{
    LogManager::getSingleton().logMessage("EXRCodec initialised");
}
EXRCodec::~EXRCodec() 
{
    LogManager::getSingleton().logMessage("EXRCodec deinitialised");
}

DataStream * EXRCodec::code(MemDataStream * input, CodecDataPtr& pData) const
{

}

Codec::DecodeResult EXRCodec::decode(DataStream * input) const
{
    ImageData * imgData = new ImageData;
    MemDataStream * output;

    try {
        // Make a mutable clone of input to be able to change file pointer
        MemDataStream myIn(input);
    
        // Now we can simulate an OpenEXR file with that
        O_IStream str(myIn, "SomeChunk.exr");
        InputFile file(str);
    
        Box2i dw = file.header().dataWindow();
        int width  = dw.max.x - dw.min.x + 1;
        int height = dw.max.y - dw.min.y + 1;
        int components = 3;
    
        // Alpha channel present?
        const ChannelList &channels = file.header().channels();
        if(channels.findChannel("A"))
            components = 4;
        
        // Allocate memory
        output = N_new MemDataStream(width * height * components * 4);
    
        // Construct frame buffer
        uchar *pixels = output->getData();
        FrameBuffer frameBuffer;
        frameBuffer.insert("R",             // name
                    Slice (FLOAT,       // type
                       ((char *) pixels)+0, // base
                       4 * components,      // xStride
                    4 * components * width));    // yStride
        frameBuffer.insert("G",             // name
                    Slice (FLOAT,       // type
                       ((char *) pixels)+4, // base
                       4 * components,      // xStride
                    4 * components * width));    // yStride
        frameBuffer.insert("B",             // name
                    Slice (FLOAT,       // type
                       ((char *) pixels)+8, // base
                       4 * components,      // xStride
                    4 * components * width));    // yStride
        if(components==4) {
            frameBuffer.insert("A",                 // name
                        Slice (FLOAT,           // type
                           ((char *) pixels)+12,        // base
                           4 * components,      // xStride
                        4 * components * width));    // yStride
        }
      
        file.setFrameBuffer (frameBuffer);
        file.readPixels (dw.min.y, dw.max.y);
    
        imgData->format = components==3 ? PF_R32G32B32 : PF_R32G32B32A32;
        imgData->width = width;
        imgData->height = height;
        imgData->depth = 1;
        imgData->size = width*height*components*4;
        imgData->num_mipmaps = 0;
        imgData->flags = 0;
    } 
    catch (const std::exception &exc) 
    {
        delete imgData;
        throw(Exception(Exception::ERR_INTERNAL_ERROR,
            "OpenEXR Error",
            exc.what()));
    }
    
    DecodeResult ret;
    ret.first = output; 
    ret.second = CodecDataPtr(imgData);
    return ret;
}

void EXRCodec::codeToFile(MemDataStream * input, const String& outFileName, CodecDataPtr& pData) const
{

}


String EXRCodec::getType() const 
{
    return "exr";
}


String EXRCodec::magicNumberToFileExt(const char* magicNumberPtr, size_t maxbytes) const
{
  // look for OpenEXR magic number
  if (maxbytes >= 4 
      && magicNumberPtr[0] == 0x76
      && magicNumberPtr[1] == 0x2f
      && magicNumberPtr[2] == 0x31
      && magicNumberPtr[3] == 0x01)
    return "exr";
    
  return "";
}

}
