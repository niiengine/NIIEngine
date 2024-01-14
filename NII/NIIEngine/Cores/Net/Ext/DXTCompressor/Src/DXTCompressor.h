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
#ifndef __DXT_COMPRESSOR_H__
#define __DXT_COMPRESSOR_H__

/* ------------------------------------------------------------------------------------------------------------------------------------ */
// Includes
#include <GL/glew.h>
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

/* ------------------------------------------------------------------------------------------------------------------------------------ */
// Forward decls.
class FramebufferObject;
class Renderbuffer;

/* ------------------------------------------------------------------------------------------------------------------------------------ */

enum CompressionType
{
	DXT1,
	DXT5_YCOCG,
};

/* ------------------------------------------------------------------------------------------------------------------------------------ */

class DXTCompressor
{
public:

	DXTCompressor();

public:

	// These need to be called before using the compressor, and shutdown after using the compressor
	static bool					Initialize();
	static void					Shutdown();

	// Compresses input data into the desired compressed format.
	// The input data is assumed to be 32-bit RGBA (1 byte per channel).
	// outputData should be allocated to at least GetBufferSize()
	// This will return true if the compression is successful.
	static bool					CompressImageData( CompressionType compressionType, const void *inputRGBA, int inputWidth, int inputHeight, void *outputData, bool bDisplayResults, bool sourceFormatIsBGRA );
	static int					GetBufferSize(CompressionType compressionType, int inputWidth, int inputHeight );

	// Saves the compressed data into a Microsoft DDS file.
	static int					GetDDSHeaderSize(void);
	static void					WriteDDSHeader( CompressionType compressionType, int width, int height, int compresedDataLength, void *outputData );
	static void					WriteDDSMemoryFile( CompressionType compressionType, int width, int height, const void* pCompressedData, int compresedDataLength, void **outputData, int *outputLength );

	// Prints out the accumulated performance of the compressor
	static void					PrintPerformanceLog();

private:

	void						SetShaderConstants();
	void						CompressInternal( bool sourceFormatIsBGRA);
	void						DoCompression( void* ppOutputData, bool sourceFormatIsBGRA );

	static bool					InitOpenGL();
	static bool					InitCG();
	static void					cgErrorCallback();
	static bool					IsInitialized();
	static void					DisplayTexture( GLuint textureID, int texW, int texH );
	static FramebufferObject*	RequestFBO( CompressionType compressionType, int width, int height );
	static void					DeallocateBuffers();

private:

	// Image related data
	int							m_imageWidth;
	int							m_imageHeight;
	CompressionType				m_compressionType;
	GLuint						m_imageTexId;

	FramebufferObject*			m_pCompressFbo;
	GLuint						m_compressFboTex;

	// CG related data
	static int					m_initRefCount;
	static CGcontext			m_cgContext; 
	static CGprofile			m_cgVProfile, m_cgFProfile;
	static CGprogram			m_compressVProg, m_compressDXT1RGBAFProg, m_compressDXT5RGBAFProg, m_compressDXT1BGRAFProg, m_compressDXT5BGRAFProg;

	// Used to collect stats
	static int					m_numIterations;
	static int					m_currentImageWidth, m_currentImageHeight;
	static float				m_lastCompressionTime;
	static float				m_accumulatedTime;
	static float				m_timeRunningCompressionShader;
	static float				m_timeCopyingPixelDataToCPU;
};

#endif // __DXT_COMPRESSOR_H__