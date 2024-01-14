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

#include "RenderSystemCapabilitiesTests.h"
#include "OgreRenderSystemCapabilities.h"
#include "OgreRenderSystemCapabilitiesManager.h"
#include "OgreStringConverter.h"
#include "OgreRenderSystemCapabilitiesSerializer.h"
#include "OgreArchiveManager.h"

#include <fstream>
#include <algorithm>


// Regsiter the suite
CPPUNIT_TEST_SUITE_REGISTRATION( RenderSystemCapabilitiesTests );

void RenderSystemCapabilitiesTests::setUp()
{
    using namespace Ogre;

    // we need to be able to create FileSystem archives to load .gpufeature
    mFileSystemArchiveFactory = N_new FileSystemArchiveFactory();

    mArchiveManager = N_new ArchiveManager();
    ArchiveManager::getSingleton().addArchiveFactory( mFileSystemArchiveFactory );

    mRenderSystemCapabilitiesManager = N_new RenderSystemCapabilitiesManager();
    // actual parsing happens here. test methods confirm parse results only
    mRenderSystemCapabilitiesManager->parseCapabilitiesFromArchive("../../../Media/CustomCapabilities", "FileSystem", true);
}

void RenderSystemCapabilitiesTests::tearDown()
{
    N_delete mRenderSystemCapabilitiesManager;
    N_delete mArchiveManager;
    N_delete mFileSystemArchiveFactory;

}

void RenderSystemCapabilitiesTests::testIsShaderProfileSupported(void)
{
    // create a new RSC
    Ogre::RenderSysFeature rsc;

    // check that no shader profile is supported
    CPPUNIT_ASSERT(!rsc.isShaderSupport("vs_1"));
    CPPUNIT_ASSERT(!rsc.isShaderSupport("ps_1_1"));
    CPPUNIT_ASSERT(!rsc.isShaderSupport("fp1"));

    rsc.addShaderProfile("vs_1");
    rsc.addShaderProfile("fp1");

    // check that the added shader profiles are supported
    CPPUNIT_ASSERT(rsc.isShaderSupport("vs_1"));
    CPPUNIT_ASSERT(rsc.isShaderSupport("fp1"));


    // check that non added profile is not supported
    CPPUNIT_ASSERT(!rsc.isShaderSupport("ps_1_1"));


    // check that empty string is not supported
    CPPUNIT_ASSERT(!rsc.isShaderSupport(""));
}

void RenderSystemCapabilitiesTests::testHasCapability()
{
    using namespace Ogre;

    RenderSysFeature rsc;

    // check that no caps (from 2 categories) are supported
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Tex_AutoMipmap));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Tex_Blend));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Program_Fragment));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Stencil_BothSide));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Tex_MipMapBias));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Tex_Compress));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Tex_Fromat_VTC));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_GL_FrameBuffer_ATI));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_GL_PBuffer));

    // add support for few caps from each category
    rsc.setCapability(GF_Tex_AutoMipmap);
    rsc.setCapability(GF_Program_Fragment);
    rsc.setCapability(GF_Tex_Compress);
    rsc.setCapability(GF_GL_FrameBuffer_ATI);

    // check that the newly set caps are supported
    CPPUNIT_ASSERT(rsc.hasCapability(GF_Tex_AutoMipmap));
    CPPUNIT_ASSERT(rsc.hasCapability(GF_Program_Fragment));
    CPPUNIT_ASSERT(rsc.hasCapability(GF_Tex_Compress));
    CPPUNIT_ASSERT(rsc.hasCapability(GF_GL_FrameBuffer_ATI));
    // check that the non-set caps are NOT supported
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Tex_Blend));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Stencil_BothSide));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Tex_MipMapBias));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_Tex_Fromat_VTC));
    CPPUNIT_ASSERT(!rsc.hasCapability(GF_GL_PBuffer));
}

void RenderSystemCapabilitiesTests::testSerializeBlank()
{
    using namespace Ogre;

    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();

    RenderSysFeature* rsc = rscManager->loadParsedCapabilities("TestCaps Blank");

    // if we have a non-NULL it's good enough
    CPPUNIT_ASSERT(rsc != 0);
}

void RenderSystemCapabilitiesTests::testSerializeEnumCapability()
{
    using namespace Ogre;

    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();

    RenderSysFeature* rsc = rscManager->loadParsedCapabilities("TestCaps enum GpuFeature");
    // confirm that RSC was loaded
    CPPUNIT_ASSERT(rsc != 0);

    // confirm that the contents are rhe same as in .gpufeature file
    CPPUNIT_ASSERT(rsc->hasCapability(GF_Tex_AutoMipmap));
    CPPUNIT_ASSERT(rsc->hasCapability(GF_GL_FrameBuffer_Arb));
}


void RenderSystemCapabilitiesTests::testSerializeStringCapability()
{
    using namespace Ogre;

    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();

    RenderSysFeature* rsc = rscManager->loadParsedCapabilities("TestCaps set String");
    // confirm that RSC was loaded
    CPPUNIT_ASSERT(rsc != 0);

    CPPUNIT_ASSERT(rsc->isShaderSupport("vs99"));
}

void RenderSystemCapabilitiesTests::testSerializeBoolCapability()
{
    using namespace Ogre;

    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();

    RenderSysFeature* rscTrue = rscManager->loadParsedCapabilities("TestCaps set bool (true)");
    RenderSysFeature* rscFalse = rscManager->loadParsedCapabilities("TestCaps set bool (false)");
    // confirm that RSC was loaded
    CPPUNIT_ASSERT(rscTrue != 0);
    CPPUNIT_ASSERT(rscFalse != 0);

}

void RenderSystemCapabilitiesTests::testSerializeIntCapability()
{
    using namespace Ogre;

    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();

    RenderSysFeature* rsc = rscManager->loadParsedCapabilities("TestCaps set int");
    // confirm that RSC was loaded
    CPPUNIT_ASSERT(rsc != 0);

    // TODO: why no get?
    CPPUNIT_ASSERT(rsc->getMaxMultiFrameObj() == 99);
}

void RenderSystemCapabilitiesTests::testSerializeRealCapability()
{
    using namespace Ogre;

    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();

    RenderSysFeature* rsc = rscManager->loadParsedCapabilities("TestCaps set Real");
    // confirm that RSC was loaded
    CPPUNIT_ASSERT(rsc != 0);

    CPPUNIT_ASSERT(rsc->getMaxPointSize() == 99.5);
}

void RenderSystemCapabilitiesTests::testSerializeShaderCapability()
{
    using namespace Ogre;

    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();

    RenderSysFeature* rsc = rscManager->loadParsedCapabilities("TestCaps addShaderProfile");
    // confirm that RSC was loaded
    CPPUNIT_ASSERT(rsc != 0);

    CPPUNIT_ASSERT(rsc->isShaderSupport("vp1"));
    CPPUNIT_ASSERT(rsc->isShaderSupport("vs_1_1"));
    CPPUNIT_ASSERT(rsc->isShaderSupport("ps_99"));
}

void RenderSystemCapabilitiesTests::testWriteSimpleCapabilities()
{
    using namespace Ogre;
	using namespace std;

    String name = "simple caps";
    String filename = "simpleCapsTest.gpufeature";

    // set up caps of every type
    RenderSystemCapabilitiesSerializer serializer;
    RenderSysFeature caps;
    caps.setCapability(GF_Tex_AutoMipmap);
    caps.setMaxPointSize(10.5);
    caps.addShaderProfile("vs999");
    caps.addShaderProfile("sp999");
    caps.setMaxSpace(777);

    // write them to file
    serializer.writeScript(&caps, name, filename);

    // read them back
    ifstream capsfile(filename.c_str());
    char buff[255];

    capsfile.getline(buff, 255);
    CPPUNIT_ASSERT_EQUAL(String("render_system_capabilities \"") + name + "\"", String(buff));

    capsfile.getline(buff, 255);
    CPPUNIT_ASSERT_EQUAL(String("{"), String(buff));
    // scan every line and find the set capabilities it them
	std::vector <String> lines;
    while(capsfile.good())
    {
        capsfile.getline(buff, 255);
        lines.push_back(String(buff));
    }

    // check that the file is closed nicely
    String closeBracket = *(lines.end() - 2);
    CPPUNIT_ASSERT_EQUAL(String("}"), closeBracket);
    CPPUNIT_ASSERT_EQUAL(String(""), lines.back());

    // check that all the set caps are there
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "automipmap true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "max_point_size 10.5") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "max_vertex_program_version vs999") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "shader_profile sp999") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vertex_texture_units_shared true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "num_world_matrices 777") != lines.end());
}

void RenderSystemCapabilitiesTests::testWriteAllFalseCapabilities()
{
    using namespace Ogre;
    using namespace std;

    String name = "all false caps";
    String filename = "allFalseCapsTest.gpufeature";

    // set up caps of every type
    RenderSystemCapabilitiesSerializer serializer;
    RenderSysFeature caps;

    // write them to file
    serializer.writeScript(&caps, name, filename);

    // read them back
    ifstream capsfile(filename.c_str());
    char buff[255];

    capsfile.getline(buff, 255);
    CPPUNIT_ASSERT_EQUAL(String("render_system_capabilities \"") + name + "\"", String(buff));

    capsfile.getline(buff, 255);
    CPPUNIT_ASSERT_EQUAL(String("{"), String(buff));
    // scan every line and find the set capabilities it them
    std::vector <String> lines;
    while(capsfile.good())
    {
        capsfile.getline(buff, 255);
        lines.push_back(String(buff));
    }

      // check that the file is closed nicely
    String closeBracket = *(lines.end() - 2);
    CPPUNIT_ASSERT_EQUAL(String("}"), closeBracket);
    CPPUNIT_ASSERT_EQUAL(String(""), lines.back());

    // confirm every caps
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "automipmap false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "blending false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "anisotropy false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "dot3 false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "cubemapping false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "hwstencil false") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vbo false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vertex_program false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "fragment_program false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "scissor_test false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "two_sided_stencil false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "stencil_wrap false") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "hwocclusion false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "user_clip_planes false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vertex_format_ubyte4 false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "infinite_far_plane false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "hwrender_to_texture false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_float false") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "non_power_of_2_textures false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_3d false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "point_sprites false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "point_extended_parameters false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vertex_texture_fetch false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "mipmap_lod_bias false") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_compression false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_compression_dxt false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_compression_vtc false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "fbo false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "fbo_arb false") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "fbo_ati false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "pbuffer false") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "perstageconstant false") != lines.end());

    // bool caps
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vertex_texture_units_shared false") != lines.end());

}

void RenderSystemCapabilitiesTests::testWriteAllTrueCapabilities()
{
    using namespace Ogre;
    using namespace std;

    String name = "all false caps";
    String filename = "allFalseCapsTest.gpufeature";

    // set up caps of every type
    RenderSystemCapabilitiesSerializer serializer;
    RenderSysFeature caps;

    caps.setCapability(GF_Tex_AutoMipmap);
    caps.setCapability(GF_Tex_Blend);
    caps.setCapability(GF_Tex_Anisotropy);
    caps.setCapability(GF_Tex_Dot3);
    caps.setCapability(GF_Tex_Cube);
    caps.setCapability(GF_HW_Stencil);

    caps.setCapability(GF_HW_Buffer);
    caps.setCapability(GF_Program_Vertex);
    caps.setCapability(GF_Program_Fragment);
    caps.setCapability(GF_ClipRect);
    caps.setCapability(GF_Stencil_BothSide);
    caps.setCapability(GF_Stencil_Wrap);

    caps.setCapability(GF_HW_PixelQuery);
    caps.setCapability(GF_ClipPlanes);
    caps.setCapability(GF_Vertex_UByte4);
    caps.setCapability(GF_Infinite_FarPlane);
    caps.setCapability(GF_Render_To_Texture);
    caps.setCapability(GF_Tex_Float);

    caps.setCapability(GF_Tex_NonPow2);
    caps.setCapability(GF_Tex_3D);
    caps.setCapability(GF_Point_Sprites);
    caps.setCapability(GF_Point_Param);
    caps.setCapability(GF_Program_VertexTexFetch);
    caps.setCapability(GF_Tex_MipMapBias);

    caps.setCapability(GF_Tex_Compress);
    caps.setCapability(GF_Tex_Fromat_DXT);
    caps.setCapability(GF_Tex_Fromat_VTC);
    caps.setCapability(GF_GL_FrameBuffer);
    caps.setCapability(GF_GL_FrameBuffer_Arb);

    caps.setCapability(GF_GL_FrameBuffer_ATI);
    caps.setCapability(GF_GL_PBuffer);
    caps.setCapability(GF_D3D_PerStageConstant);


    // write them to file
    serializer.writeScript(&caps, name, filename);

    // read them back
    ifstream capsfile(filename.c_str());
    char buff[255];

    capsfile.getline(buff, 255);
    CPPUNIT_ASSERT_EQUAL(String("render_system_capabilities \"") + name + "\"", String(buff));

    capsfile.getline(buff, 255);
    CPPUNIT_ASSERT_EQUAL(String("{"), String(buff));
    // scan every line and find the set capabilities it them
    std::vector <String> lines;
    while(capsfile.good())
    {
        capsfile.getline(buff, 255);
        lines.push_back(String(buff));
    }

      // check that the file is closed nicely
    String closeBracket = *(lines.end() - 2);
    CPPUNIT_ASSERT_EQUAL(String("}"), closeBracket);
    CPPUNIT_ASSERT_EQUAL(String(""), lines.back());

    // confirm all caps
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "automipmap true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "blending true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "anisotropy true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "dot3 true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "cubemapping true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "hwstencil true") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vbo true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vertex_program true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "fragment_program true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "scissor_test true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "two_sided_stencil true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "stencil_wrap true") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "hwocclusion true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "user_clip_planes true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vertex_format_ubyte4 true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "infinite_far_plane true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "hwrender_to_texture true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_float true") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "non_power_of_2_textures true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_3d true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "point_sprites true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "point_extended_parameters true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vertex_texture_fetch true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "mipmap_lod_bias true") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_compression true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_compression_dxt true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "texture_compression_vtc true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "fbo true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "fbo_arb true") != lines.end());

    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "fbo_ati true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "pbuffer true") != lines.end());
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "perstageconstant true") != lines.end());

    // bool caps
    CPPUNIT_ASSERT(find(lines.begin(), lines.end(), "vertex_texture_units_shared true") != lines.end());

}

void RenderSystemCapabilitiesTests::testWriteAndReadComplexCapabilities()
{
    using namespace Ogre;
    using namespace std;

    String name = "complex caps";
    String filename = "complexCapsTest.gpufeature";

    // set up caps of every type
    RenderSysFeatureSerializer serializer;
    RenderSysFeature caps;

    caps.setCapability(GF_Tex_AutoMipmap);
    caps.setCapability(GF_Tex_Dot3);
    caps.setCapability(GF_Tex_Cube);
    caps.setCapability(GF_HW_Stencil);
    caps.setCapability(GF_HW_Buffer);
    caps.setCapability(GF_Program_Fragment);
    caps.setCapability(GF_ClipRect);
    caps.setCapability(GF_Stencil_BothSide);
    caps.setCapability(GF_HW_PixelQuery);
    caps.setCapability(GF_Vertex_UByte4);
    caps.setCapability(GF_Render_To_Texture);
    caps.setCapability(GF_Tex_Float);
    caps.setCapability(GF_Tex_NonPow2);
    caps.setCapability(GF_Tex_3D);
    caps.setCapability(GF_Point_Param);
    caps.setCapability(GF_Tex_MipMapBias);
    caps.setCapability(GF_Tex_Compress);
    caps.setCapability(GF_Tex_Fromat_DXT);
    caps.setCapability(GF_Tex_Fromat_VTC);
    caps.setCapability(GF_D3D_PerStageConstant);

    caps.setMaxSpace(11);
    caps.setMaxTexUnit(22);
    caps.setStencilBit(20001);

    caps.setMaxMultiFrameObj(23);

    caps.addShaderProfile("99foo100");
    // try out stranger names
    caps.addShaderProfile("..f(_)specialsymbolextravaganza!@#$%^&*_but_no_spaces");

    caps.setMaxVertexProgramParam(1111, 2222, 3333);

    caps.setMaxFragmentProgramParam(4444, 5555, 64000);

    caps.setMaxPointSize(123.75);
    caps.setOnlyPow2Texture(true);

	DriverVersion driverversion;
	driverversion.major = 11;
	driverversion.minor = 13;
	driverversion.release = 17;
	driverversion.build = 0;

	caps.getGpuInfo()->setVersion(driverversion);


     // write them to file
    serializer.writeScript(&caps, name, filename);

    FileStreamDataStream* fdatastream = N_new FileStreamDataStream(filename,
            OGRE_NEW_T(ifstream, MEMCATEGORY_GENERAL)(filename.c_str()));

    // parsing does not return a raw RSC, but adds it to the Manager
    serializer.read(fdatastream);
    N_delete fdatastream;
    
    RenderSystemCapabilitiesManager* rscManager = RenderSystemCapabilitiesManager::getSingletonPtr();

    RenderSysFeature* rsc = rscManager->loadParsedCapabilities(name);
    // confirm that RSC was loaded
    CPPUNIT_ASSERT(rsc != 0);

    // create a reference, so that were're working with two refs
    RenderSysFeature& caps2 = *rsc;

    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_AutoMipmap), caps2.hasCapability(GF_Tex_AutoMipmap));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_Blend), caps2.hasCapability(GF_Tex_Blend));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_Anisotropy), caps2.hasCapability(GF_Tex_Anisotropy));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_Dot3), caps2.hasCapability(GF_Tex_Dot3));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_Cube), caps2.hasCapability(GF_Tex_Cube));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_HW_Stencil), caps2.hasCapability(GF_HW_Stencil));

    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_HW_Buffer), caps2.hasCapability(GF_HW_Buffer));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Program_Vertex), caps2.hasCapability(GF_Program_Vertex));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Program_Fragment), caps2.hasCapability(GF_Program_Fragment));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_ClipRect), caps2.hasCapability(GF_ClipRect));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Stencil_BothSide), caps2.hasCapability(GF_Stencil_BothSide));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Stencil_Wrap), caps2.hasCapability(GF_Stencil_Wrap));

    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_HW_PixelQuery), caps2.hasCapability(GF_HW_PixelQuery));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_ClipPlanes), caps2.hasCapability(GF_ClipPlanes));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Vertex_UByte4), caps2.hasCapability(GF_Vertex_UByte4));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Infinite_FarPlane), caps2.hasCapability(GF_Infinite_FarPlane));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Render_To_Texture), caps2.hasCapability(GF_Render_To_Texture));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_Float), caps2.hasCapability(GF_Tex_Float));

    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_NonPow2), caps2.hasCapability(GF_Tex_NonPow2));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_3D), caps2.hasCapability(GF_Tex_3D));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Point_Sprites), caps2.hasCapability(GF_Point_Sprites));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Point_Param), caps2.hasCapability(GF_Point_Param));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Program_VertexTexFetch), caps2.hasCapability(GF_Program_VertexTexFetch));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_MipMapBias), caps2.hasCapability(GF_Tex_MipMapBias));

    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_Compress), caps2.hasCapability(GF_Tex_Compress));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_Fromat_DXT), caps2.hasCapability(GF_Tex_Fromat_DXT));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_Tex_Fromat_VTC), caps2.hasCapability(GF_Tex_Fromat_VTC));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_GL_FrameBuffer), caps2.hasCapability(GF_GL_FrameBuffer));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_GL_FrameBuffer_Arb), caps2.hasCapability(GF_GL_FrameBuffer_Arb));

    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_GL_FrameBuffer_ATI), caps2.hasCapability(GF_GL_FrameBuffer_ATI));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_GL_PBuffer), caps2.hasCapability(GF_GL_PBuffer));
    CPPUNIT_ASSERT_EQUAL(caps.hasCapability(GF_D3D_PerStageConstant), caps2.hasCapability(GF_D3D_PerStageConstant));

    CPPUNIT_ASSERT_EQUAL(caps.getMaxSpace(), caps2.getMaxSpace());
    CPPUNIT_ASSERT_EQUAL(caps.getMaxTexUnit(), caps2.getMaxTexUnit());
    CPPUNIT_ASSERT_EQUAL(caps.getStencilBit(), caps2.getStencilBit());
    CPPUNIT_ASSERT_EQUAL(caps.getMaxMultiFrameObj(), caps2.getMaxMultiFrameObj());

    CPPUNIT_ASSERT_EQUAL(caps.getMaxVPFloatParamCount(), caps2.getMaxVPFloatParamCount());
    CPPUNIT_ASSERT_EQUAL(caps.getMaxVPIntParamCount(), caps2.getMaxVPIntParamCount());
    CPPUNIT_ASSERT_EQUAL(caps.getMaxVPBoolParamCount(), caps2.getMaxVPBoolParamCount());

    CPPUNIT_ASSERT_EQUAL(caps.getMaxFPFloatCount(), caps2.getMaxFPFloatCount());
    CPPUNIT_ASSERT_EQUAL(caps.getMaxFPIntCount(), caps2.getMaxFPIntCount());
    CPPUNIT_ASSERT_EQUAL(caps.getMaxFPBoolCount(), caps2.getMaxFPBoolCount());

    CPPUNIT_ASSERT_EQUAL(caps.getMaxPointSize(), caps2.getMaxPointSize());
    CPPUNIT_ASSERT_EQUAL(caps.isOnlyPow2Texture(), caps2.isOnlyPow2Texture());
	
	// test versions
	CPPUNIT_ASSERT_EQUAL(caps.getGpuInfo()->getVersion().major, caps2.getGpuInfo()->getVersion().major);
	CPPUNIT_ASSERT_EQUAL(caps.getGpuInfo()->getVersion().minor, caps2.getGpuInfo()->getVersion().minor);
	CPPUNIT_ASSERT_EQUAL(caps.getGpuInfo()->getVersion().release, caps2.getGpuInfo()->getVersion().release);
	CPPUNIT_ASSERT_EQUAL(0, caps2.getGpuInfo()->getVersion().build);
}


