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

#ifndef _NII_GLPIXELFORMAT_H_
#define _NII_GLPIXELFORMAT_H_

#include "NiiGLPreInclude.h"
#include "NiiPixelFormat.h"

namespace NII 
{
	/**
	* Class to do pixel format mapping between GL and NII
	*/
	class _EngineGLAPI GLPixelUtil
	{
	public:
		/** Takes the NII pixel format and returns the appropriate GL one
			@returns a GLenum describing the format, or 0 if there is no exactly matching 
			one (and conversion is needed)
		*/
		static GLenum getGLOriginFormat(PixelFormat pf);
	
		/** Takes the NII pixel format and returns type that must be provided
			to GL as data type for reading it into the GPU
			@returns a GLenum describing the data type, or 0 if there is no exactly matching 
			one (and conversion is needed)
		*/
		static GLenum getGLOriginDataType(PixelFormat pf);
        
        /**	Takes the NII pixel format and returns the type that must be provided
			to GL as internal format. GL_NONE if no match exists.
		@param mFormat The pixel format
		@param hwGamma Whether a hardware gamma-corrected version is requested
		*/
		static GLenum getGLInternalFormat(PixelFormat pf, bool hwGamma = false);
		
		/**	Function to get the closest matching NII format to an internal GL format. To be
			precise, the format will be chosen that is most efficient to transfer to the card 
			without losing precision.
			@remarks It is valid for this function to always return PF_A8R8G8B8.
		*/
		static PixelFormat getClosestNIIFormat(GLenum fmt);
        
        /** Returns next power-of-two size if required by render system, in case
            GF_Tex_NonPow2 is supported it returns value as-is.
        */
        static NCount optionalPO2(Nui32 value);
        
        /** Takes the OGRE pixel format and returns the type that must be provided
            to GL as internal format for image load/store. GL_NONE if no match exists.
            @param format The pixel format
        */
        static GLenum getGLImageInternalFormat(PixelFormat format);

        /** Takes the OGRE pixel format and returns the type that must be provided
            to GL as internal format for image load/store. If no match exists, 
            returns the closest match.
            @param format The pixel format
        */
        static GLenum getClosestGLImageInternalFormat(PixelFormat format);
	};
}
#endif
