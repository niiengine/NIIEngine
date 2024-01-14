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
#include "NiiRenderSys.h"
#include "NiiEngine.h"
#include "NiiMeshManager.h"
#include "NiiMaterialManager.h"
#include "NiiTextureManager.h"
#include "NiiRenderFrameListener.h"
#include "NiiMultiTextureFrame.h"
#include "NiiGpuBuffer.h"
#include "NiiViewWindow.h"
#include "NiiTimer.h"
#include "NiiPixelCountQuery.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiException.h"
#define NII_CAPS_BITSHIFT (48 - CC_Count)
#define CAPS_CATEGORY_MASK (((1 << CC_Count) - 1) << NII_CAPS_BITSHIFT)

    /** Creates a new rendering window.
    @remarks
        This method creates a new rendering window as specified
        by the paramteters. The rendering system could be
        responible for only a single window (e.g. in the case
        of a game), or could be in charge of multiple ones (in the
        case of a level editor). The option to create the window
        as a child of another is therefore given.
        This method will create an appropriate subclass of
        RenderWindow depending on the API and platform implementation.
    @par
        After creation, this window can be retrieved using getRenderTarget().
    @param
        name The name of the window. Used in other methods
        later like setRenderTarget and getRenderTarget.
        @param
        width The width of the new window.
        @param
        height The height of the new window.
        @param
        fullScreen Specify true to make the window full screen
        without borders, title bar or menu bar.
        @param
        miscParams A PropertyData describing the other parameters for the new rendering window. 
        Options are case sensitive. Unrecognised parameters will be ignored silently.
        These values might be platform dependent, but these are present for all platforms unless
        indicated otherwise:
        <table>
        <tr>
            <td><b>Key</b></td>
            <td><b>Type/Values</b></td>
            <td><b>Default</b></td>
            <td><b>Description</b></td>
            <td><b>Notes</b></td>
        </tr>
        <tr>
            <td>title</td>
            <td>Any string</td>
            <td>FrameObj name</td>
            <td>The title of the window that will appear in the title bar</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>colourDepth</td>
            <td>16, 32</td>
            <td>Desktop depth</td>
            <td>Colour depth of the resulting rendering window; only applies if fullScreen</td>
            <td>Win32 Specific</td>
        </tr>
        <tr>
            <td>left</td>
            <td>Positive integers</td>
            <td>Centred</td>
            <td>Screen x coordinate from left</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>top</td>
            <td>Positive integers</td>
            <td>Centred</td>
            <td>Screen y coordinate from left</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>depthBuffer</td>
            <td>true, false</td>
            <td>true</td>
            <td>Use depth buffer</td>
            <td>DirectX9 specific</td>
        </tr>
        <tr>
            <td>externalWindowHandle</td>
            <td>Win32: HWND as integer<br/>
                GLX: poslong:posint:poslong (display*:screen:windowHandle) or poslong:posint:poslong:poslong (display*:screen:windowHandle:XVisualInfo*)<br/>
                OS X Cocoa: OgreGLView address as an integer. You can pass NSView or NSWindow too, but should perform OgreGLView callbacks into the Ogre manually.
                iOS: UIWindow address as an integer
            </td>
            <td>0 (none)</td>
            <td>External window handle, for embedding the OGRE render in an existing window</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>externalGLControl</td>
            <td>true, false</td>
            <td>false</td>
            <td>Let the external window control OpenGL i.e. don't select a pixel format for the window,
            do not change v-sync and do not swap buffer. When set to true, the calling application
            is responsible of OpenGL initialization and buffer swapping. It should also create an
            OpenGL context for its own rendering, Ogre will create one for its use. Then the calling
            application must also enable Ogre OpenGL context before calling any Ogre function and
            restore its OpenGL context after these calls.</td>
            <td>OpenGL specific</td>
        </tr>
        <tr>
            <td>externalGLContext</td>
            <td>Context as Unsigned Long</td>
            <td>0 (create own context)</td>
            <td>Use an externally created GL context</td>
            <td>OpenGL Specific</td>
        </tr>
        <tr>
            <td>parentWindowHandle</td>
            <td>Win32: HWND as integer<br/>
                GLX: poslong:posint:poslong (display*:screen:windowHandle) or poslong:posint:poslong:poslong (display*:screen:windowHandle:XVisualInfo*)</td>
            <td>0 (none)</td>
            <td>Parent window handle, for embedding the OGRE in a child of an external window</td>
            <td>&nbsp;</td>
        </tr>
         <tr>
             <td>contentScalingFactor</td>
             <td>Positive Float greater than 1.0</td>
             <td>The default content scaling factor of the screen</td>
             <td>Specifies the CAEAGLLayer content scaling factor.  Only supported on iOS 4 or greater.
                 This can be useful to limit the resolution of the OpenGL ES backing store.  For example, the iPhone 4's
                 native resolution is 960 x 640.  Windows are always 320 x 480, if you would like to limit the display
                 to 720 x 480, specify 1.5 as the scaling factor.
             </td>
             <td>iOS Specific</td>
             <td>&nbsp;</td>
         </tr>
         <tr>
             <td>externalViewHandle</td>
             <td>UIView pointer as an integer</td>
             <td>0</td>
             <td>External view handle, for rendering OGRE render in an existing view</td>
             <td>iOS Specific</td>
             <td>&nbsp;</td>
         </tr>
         <tr>
             <td>externalViewControllerHandle</td>
             <td>UIViewController pointer as an integer</td>
             <td>0</td>
             <td>External view controller handle, for embedding OGRE in an existing view controller</td>
             <td>iOS Specific</td>
             <td>&nbsp;</td>
         </tr>
         <tr>
             <td>externalSharegroup</td>
             <td>EAGLSharegroup pointer as an integer</td>
             <td>0</td>
             <td>External sharegroup, used to shared GL resources between contexts</td>
             <td>iOS Specific</td>
             <td>&nbsp;</td>
         </tr>
         <tr>
             <td>Full Screen</td>
             <td>true, false</td>
             <td>false</td>
             <td>Specify whether to create the window in full screen mode</td>
             <td>OS X Specific</td>
             <td>&nbsp;</td>
         </tr>
         <tr>
            <td>MSAA</td>
            <td>Positive integer (usually 1, 2, 4, 8, 16)</td>
            <td>1</td>
            <td>Multisample antialiasing factor</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>MSAA_Quality</td>
            <td>Depends on RenderSys and hardware. Currently supports:<br/>
            0 ... infinite number (depends on HW)</td>
            <td>Blank</td>
            <td>Full screen antialiasing hint</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>displayFrequency</td>
            <td>Refresh rate in Hertz (e.g. 60, 75, 100)</td>
            <td>Desktop vsync rate</td>
            <td>Display frequency rate, for fullscreen mode</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>vsync</td>
            <td>true, false</td>
            <td>false</td>
            <td>Synchronize buffer swaps to monitor vsync, eliminating tearing at the expense of a fixed frame rate</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>vsyncInterval</td>
            <td>1, 2, 3, 4</td>
            <td>1</td>
            <td>If vsync is enabled, the minimum number of vertical blanks that should occur between renders. 
            For example if vsync is enabled, the refresh rate is 60 and this is set to 2, then the
            frame rate will be locked at 30.</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>border</td>
            <td>none, fixed, resize</td>
            <td>resize</td>
            <td>The type of window border (in windowed mode)</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>outerDimensions</td>
            <td>true, false</td>
            <td>false</td>
            <td>Whether the width/height is expressed as the size of the 
            outer window, rather than the content area</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>useNVPerfHUD</td>
            <td>true, false</td>
            <td>false</td>
            <td>Enable the use of nVidia NVPerfHUD</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>gamma</td>
            <td>true, false</td>
            <td>false</td>
            <td>Enable hardware conversion from linear colour space to gamma
            colour space on rendering to the window.</td>
            <td>&nbsp;</td>
        </tr>
        <tr>
            <td>enableDoubleClick</td>
            <td>true, false</td>
            <td>false</td>
            <td>Enable the window to keep track and transmit double click messages.</td>
            <td>Win32 Specific</td>
        </tr>
        <tr>
            <td>MSAA</td>
            <td>Positive integer (usually 0, 2, 4, 8, 16)</td>
            <td>0</td>
            <td>Full screen antialiasing factor</td>	  
            <td>Android Specific</td>
        </tr>  
        <tr>
            <td>CSAA</td>
            <td>Positive integer (usually 0, 2, 4, 8, 16)</td>
            <td>0</td>
            <td>Coverage sampling factor (https://www.khronos.org/registry/egl/extensions/NV/EGL_NV_coverage_sample.txt)</td>	  
            <td>Android Specific</td>
        </tr>	
        <tr>
            <td>maxColourBufferSize</td>
            <td>Positive integer (usually 16, 32)</td>
            <td>32</td>
            <td>Max EGL_BUFFER_SIZE</td>	  
            <td>Android Specific</td>
        </tr>	 
        <tr>
            <td>minColourBufferSize</td>
            <td>Positive integer (usually 16, 32)</td>
            <td>16</td>
            <td>Min EGL_BUFFER_SIZE</td>	  
            <td>Android Specific</td>
        </tr>  
        <tr>
            <td>maxStencilBufferSize</td>
            <td>Positive integer (usually 0, 8)</td>
            <td>0</td>
            <td>EGL_STENCIL_SIZE</td>	  
            <td>Android Specific</td>
        </tr>	
        <tr>
            <td>maxDepthBufferSize</td>
            <td>Positive integer (usually 0, 16, 24)</td>
            <td>16</td>
            <td>EGL_DEPTH_SIZE</td>	  
            <td>Android Specific</td>
        </tr>
    */

    void loadMaterials(const String & filename, GroupID gid, const String & additionalTextureExtension);

    void saveMaterials(RenderPatternType type, const String & filename, const String & additionalTextureExtension);

    void saveMaterial(const ShaderChMaterial * mat, const String & filename, const String & additionalTextureExtension);

    Nidx getFreeBasicBlock(uint8 type, ShaderChBase * scb)
    {
        if(mFreeBlockIds[type].empty())
        {
            N_EXCEPT(Internal, "Can't have more than " + StrConv::conv( mActiveBlocks[type].size() ) +
                " active blocks! You have too many materials with different rasterizer state, "
                "blending state, or sampler state parameters.");
        }

        const Nidx idx = mFreeBlockIds[type].back();
        mFreeBlockIds[type].pop_back();

        mActiveBlocks[type].push_back(idx);
        if(scb)
            mBlocks[type][idx] = scb;

        return idx;
    }

    void destroyBasicBlock(ShaderChBase * scb)
    {
        scb->mRsData = 0;

        IdxList::iterator itor = std::find(mActiveBlocks[scb->mBlockType].begin(), mActiveBlocks[scb->mBlockType].end(), scb->mId);

        assert(itor != mActiveBlocks[scb->mBlockType].end());
        mActiveBlocks[scb->mBlockType].erase(itor);

        mFreeBlockIds[scb->mBlockType].push_back(scb->mId);

        scb->mId = std::numeric_limits<uint16>::max();
    }

    /////////////////////////////////////////////////////////////////////////////////////
    IdxList mActiveBlocks[HBT_Count];
    IdxList mFreeBlockIds[HBT_Count];
    ShaderChBase * mBlocks[HBT_Count][64];
    //-----------------------------------------------------------------------------------
    template <typename T> const T * RenderSys::getDescriptorSet(typename set<T>::type & container, const T & baseParams, void (*renderSysFunc)(T *))
    {
        typename set<T>::type::iterator itor = container.find(baseParams);

        if(itor == container.end())
        {
            T newDescSet = baseParams;
            newDescSet.mRefCount = 0;
            (*renderSysFunc)(&newDescSet);
            std::pair<typename set<T>::type::iterator, bool> entry = container.insert(newDescSet);
            itor = entry.first;
        }

        T * re = const_cast<T *>(&(*itor));
        ++re->mRefCount;
        return re;
    }
    //-----------------------------------------------------------------------------------
    template <typename T> void RenderSys::destroyDescriptorSet(typename set<T>::type & container, const T * descSet, void (*renderSysFunc)(T *))
    {
        typename set<T>::type::iterator itor = container.find(*descSet);
        if(itor == container.end() || &(*itor) != descSet)
        {
            N_EXCEPT(Internal, "The DescriptorSet wasn't created with this manager!");
        }

        //We have to const_cast because std::set protects programmers from altering the
        //order. However we will only be touching mRefCount & mRsData elements, which
        //are not used by our sorting operators.
        T * descSetPtr = const_cast<T *>( &(*itor) );
        --descSetPtr->mRefCount;
        if(!descSetPtr->mRefCount)
        {
            (*renderSysFunc)(descSetPtr);
            container.erase(itor);
        }
    }
    //-----------------------------------------------------------------------------------
    template <typename T, HlmsBlockType type, size_t maxLimit>
        T * RenderSys::getBasicBlock(typename vector<T>::type & container, const T & baseParams)
    {
        assert( baseParams.mBlockType == type &&
                "baseParams.mBlockType should always be HBT_DepthStencil or HBT_Blend! "
                "You can ignore this assert,  but it usually indicates memory corruption"
                "(or you created the block without its default constructor)." );

        typename vector<T>::type::iterator itor = std::find(container.begin(), container.end(), baseParams);

        if( itor == container.end() )
        {
            OGRE_ASSERT_LOW( container.size() <= maxLimit &&
                "Exceeded the max number of blocks that can be created during the lifetime of an application!!!");
            container.push_back( baseParams );
            container.back().mRefCount   = 0;
            container.back().mId         = std::numeric_limits<uint16>::max();
            container.back().mLifetimeId = static_cast<uint16>(container.size() - 1u);
            container.back().mBlockType  = type;
            itor = container.end() - 1u;
        }

        if( !itor->mRefCount )
        {
            OGRE_ASSERT_LOW(itor->mId == std::numeric_limits<uint16>::max());
            itor->mId = getFreeBasicBlock(type, &*itor);
        }

        return &(*itor);
    }
namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RenderFeature
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    RenderFeature::RenderFeature() :
        mMaxSpace(0),
        mMaxTextureUnit(0),
        mStencilBit(0),
        mMaxMultiFrameObj(1),
        mMaxGPOutput(0),
        mOnlyPow2Texture(false)
    {
        mSLMark = 0;
        for (NIIi i = 0; i < CC_Count; i++)
        {
            mFeature[i] = 0;
        }
    }
    //-----------------------------------------------------------------------
    RenderFeature::~RenderFeature()
    {
    }
    //-----------------------------------------------------------------------
    void RenderFeature::add(GpuFeature c)
    {
        NIIi index = (CAPS_CATEGORY_MASK & c) >> NII_CAPS_BITSHIFT;
        // zero out the index from the stored capability
        mFeature[index] |= (c & ~CAPS_CATEGORY_MASK);
    }
    //-----------------------------------------------------------------------
    void RenderFeature::remove(GpuFeature c)
    {
        NIIi index = (CAPS_CATEGORY_MASK & c) >> NII_CAPS_BITSHIFT;
        // zero out the index from the stored capability
        mFeature[index] &= (~c | CAPS_CATEGORY_MASK);
    }
    //-----------------------------------------------------------------------
    bool RenderFeature::isSupport(GpuFeature c) const
    {
        NIIi index = (CAPS_CATEGORY_MASK & c) >> NII_CAPS_BITSHIFT;
        // test against
        if (mFeature[index] & (c & ~CAPS_CATEGORY_MASK))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    //-----------------------------------------------------------------------
    bool RenderFeature::isSpecFeature(GpuFeature c) const
    {
        NIIi cat = c >> NII_CAPS_BITSHIFT;
        if (cat == CC_GL || cat == CC_D3D)
            return true;
        return false;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxTexSize(NCount t2d, NCount t3d, NCount tCube)
    {
        mMaxTexSize[0] = t2d;
        mMaxTexSize[1] = t3d;
        mMaxTexSize[3] = tCube;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxVertexProgramParam(Nui16 c)
    {
        mMaxVPParam[0] = c;
        mMaxVPParam[1] = c;
        mMaxVPParam[2] = c;
        mMaxVPParam[3] = c;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxVertexProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b)
    {
        mMaxVPParam[0] = f;
        mMaxVPParam[1] = d;
        mMaxVPParam[2] = i;
        mMaxVPParam[3] = b;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxFragmentProgramParam(Nui16 c)
    {
        mMaxFPParam[0] = c;
        mMaxFPParam[1] = c;
        mMaxFPParam[2] = c;
        mMaxFPParam[3] = c;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxFragmentProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b)
    {
        mMaxFPParam[0] = f;
        mMaxFPParam[1] = d;
        mMaxFPParam[2] = i;
        mMaxFPParam[3] = b;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxGeometryProgramParam(Nui16 c)
    {
        mMaxGPParam[0] = c;
        mMaxGPParam[1] = c;
        mMaxGPParam[2] = c;
        mMaxGPParam[3] = c;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxGeometryProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b)
    {
        mMaxGPParam[0] = f;
        mMaxGPParam[1] = d;
        mMaxGPParam[2] = i;
        mMaxGPParam[3] = b;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxTessControlProgramParam(Nui16 c)
    {
        mMaxTPParam[0] = c;
        mMaxTPParam[1] = c;
        mMaxTPParam[2] = c;
        mMaxTPParam[3] = c;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxTessControlProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b)
    {
        mMaxTPParam[0] = f;
        mMaxTPParam[1] = d;
        mMaxTPParam[2] = i;
        mMaxTPParam[3] = b;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxDomainProgramParam(Nui16 c)
    {
        mMaxDPParam[0] = c;
        mMaxDPParam[1] = c;
        mMaxDPParam[2] = c;
        mMaxDPParam[3] = c;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxDomainProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b)
    {
        mMaxDPParam[0] = f;
        mMaxDPParam[1] = d;
        mMaxDPParam[2] = i;
        mMaxDPParam[3] = b;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxComputeProgramParam(Nui16 c)
    {
        mMaxCPParam[0] = c;
        mMaxCPParam[1] = c;
        mMaxCPParam[2] = c;
        mMaxCPParam[3] = c;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::setMaxComputeProgramParam(Nui16 f, Nui16 d, Nui16 i, Nui16 b)
    {
        mMaxCPParam[0] = f;
        mMaxCPParam[1] = d;
        mMaxCPParam[2] = i;
        mMaxCPParam[3] = b;
    }
    //-----------------------------------------------------------------------
    void RenderFeature::log(Log * l)
    {
        l->log(_I18n("渲染系统特性"));
        l->log(_T("-------------------------"));
        l->log(_I18n("渲染系统名称: ") + getName());
        l->log(_I18n("显卡核心供应商: ") + GpuVendorDetail::toStr(getGpuInfo()->getVendor()));
        l->log(_I18n("显卡设备名称: ") + getGpuInfo()->getName());
        l->log(_I18n("显卡驱动版本: ") + getGpuInfo()->getVersion().toString());
        l->log(_I18n(" * 显卡固定管线: ") + N_conv(isSupport(GF_Fixed_Pipeline)));
        l->log(_I18n(" * Anisotropic texture filtering: ") + N_conv(isSupport(GF_Tex_Anisotropy)));
        l->log(_I18n(" * Dot product texture operation: ") + N_conv(isSupport(GF_Tex_Dot3)));
        l->log(_I18n(" * Hardware stencil buffer: ") + N_conv(isSupport(GF_HW_Stencil)));
        if (isSupport(GF_HW_Stencil))
        {
            l->log(_I18n("   - Stencil depth: ") + N_conv(getStencilBit()));
            l->log(_I18n("   - Two sided stencil support: ") + N_conv(isSupport(GF_Stencil_BothSide)));
            l->log(_I18n("   - Wrap stencil values: ") + N_conv(isSupport(GF_Stencil_Wrap)));
        }
        l->log(_I18n(" * Hardware colour blend: ") + N_conv(isSupport(GF_Colour_Blend)));
        l->log(_I18n(" * Hardware debug: ") + N_conv(isSupport(GF_Debug)));
        l->log(_I18n(" * Hardware persistent mapping: ") + N_conv(isSupport(CF_PersistentMapping)));
        l->log(_I18n(" * Hardware instance base: ") + N_conv(isSupport(CF_BaseInstance)));
        l->log(_I18n(" * Hardware indirect: ") + N_conv(isSupport(CF_IndirectBuffer)));
        
        l->log(_I18n(" * Hardware vertex / index buffers: ") + N_conv(isSupport(GF_HW_Buffer)));
        l->log(_I18n(" * Vertex programs: ") + N_conv(isSupport(GF_Program_Vertex)));
        l->log(_I18n(" * Number of floating-point constants for vertex programs: ") + N_conv(mMaxVPParam[0]));
        l->log(_I18n(" * Number of integer constants for vertex programs: ") + N_conv(mMaxVPParam[1]));
        l->log(_I18n(" * Number of boolean constants for vertex programs: ") + N_conv(mMaxVPParam[2]));
        l->log(_I18n(" * Fragment programs: ") + N_conv(isSupport(GF_Program_Fragment)));
        l->log(_I18n(" * Number of floating-point constants for fragment programs: ") + N_conv(mMaxFPParam[0]));
        l->log(_I18n(" * Number of integer constants for fragment programs: ") + N_conv(mMaxFPParam[1]));
        l->log(_I18n(" * Number of boolean constants for fragment programs: ") + N_conv(mMaxFPParam[2]));
        l->log(_I18n(" * Geometry programs: ") + N_conv(isSupport(GF_Program_Geometry)));
        l->log(_I18n(" * Number of floating-point constants for geometry programs: ") + N_conv(mMaxGPParam[0]));
        l->log(_I18n(" * Number of integer constants for geometry programs: ") + N_conv(mMaxGPParam[1]));
        l->log(_I18n(" * Number of boolean constants for geometry programs: ") + N_conv(mMaxGPParam[2]));
        l->log(_I18n(" * Hull-shader programs: ") + N_conv(isSupport(GF_Program_Tess)));
        l->log(_I18n(" * Number of floating-point constants for hull-shader programs: ") + N_conv(mMaxTPParam[0]));
        l->log(_I18n(" * Number of integer constants for hull-shader programs: ") + N_conv(mMaxTPParam[1]));
        l->log(_I18n(" * Number of boolean constants for hull-shader programs: ") + N_conv(mMaxTPParam[2]));
        l->log(_I18n(" * Domain-shader programs: ") + N_conv(isSupport(GF_Program_Domain)));
        l->log(_I18n(" * Number of floating-point constants for domain-shader programs: ") + N_conv(mMaxDPParam[0]));
        l->log(_I18n(" * Number of integer constants for domain-shader programs: ") + N_conv(mMaxDPParam[1]));
        l->log(_I18n(" * Number of boolean constants for domain-shader programs: ") + N_conv(mMaxDPParam[2]));
        l->log(_I18n(" * Compute-shader programs: ") + N_conv(isSupport(GF_Program_Compute)));
        l->log(_I18n(" * Number of floating-point constants for compute-shader programs: ") + N_conv(mMaxCPParam[0]));
        l->log(_I18n(" * Number of integer constants for compute-shader programs: ") + N_conv(mMaxCPParam[1]));
        l->log(_I18n(" * Number of boolean constants for compute-shader programs: ") + N_conv(mMaxCPParam[2]));

        l->log(_I18n(" * Supported Shader Profiles:") + N_conv(mSLMark));

        l->log(_I18n(" * Texture Compression: ") + N_conv(isSupport(GF_Tex_Compress)));
        if (isSupport(GF_Tex_Compress))
        {
            l->log(_I18n("   - DXT: ") + N_conv(isSupport(GF_Tex_Fromat_DXT)));
            l->log(_I18n("   - VTC: ") + N_conv(isSupport(GF_Tex_Fromat_VTC)));
            l->log(_I18n("   - PVRTC: ") + N_conv(isSupport(GF_Tex_Fromat_PVRTC)));
            l->log(_I18n("   - ETC2: ") + N_conv(isSupport(GF_Tex_Fromat_ETC2)));
            l->log(_I18n("   - BC4_BC5: ") + N_conv(isSupport(GF_Tex_Fromat_BC4_5)));
            l->log(_I18n("   - BC6H_BC7: ") + N_conv(isSupport(GF_Tex_Fromat_BC6H_7)));
            l->log(_I18n("   - ASTC: ") + N_conv(isSupport(GF_Tex_Fromat_ASTC)));
        }
        l->log(_I18n(" * Shader Binary Get: ") + N_conv(isSupport(GF_Shader_Binary_Get)));
        l->log(_I18n(" * Scissor Rectangle: ") + N_conv(isSupport(GF_ClipRect)));
        l->log(_I18n(" * Hardware Occlusion Query: ") + N_conv(isSupport(GF_HW_PixelQuery)));
        l->log(_I18n(" * User clip planes: ") + N_conv(isSupport(GF_ClipPlanes)));
        l->log(_I18n(" * DT_UByte vertex element type: ") + N_conv(isSupport(GF_Vertex_UByte4)));
        l->log(_I18n(" * Infinite far plane projection: ") + N_conv(isSupport(GF_Infinite_FarPlane)));
        l->log(_I18n(" * Hardware render-to-texture: ") + N_conv(isSupport(GF_Render_To_Texture)));
        l->log(_I18n(" * Floating point textures: ") + N_conv(isSupport(GF_Tex_Float)));
        l->log(_I18n(" * Non-power-of-two textures: ") + N_conv(isSupport(GF_Tex_NonPow2)) + (mOnlyPow2Texture ? _I18n(" (limited)") : _T("")));
        l->log(_I18n(" * Volume textures: ") + N_conv(isSupport(GF_Tex_3D)));
        l->log(_I18n(" * Multiple Render Targets: ") + N_conv(mMaxMultiFrameObj));
        l->log(_I18n("   - With different bit depths: ") + N_conv(isSupport(GF_MultiRenderTarget_EachDepthBit)));
        l->log(_I18n(" * Point Sprites: ") + N_conv(isSupport(GF_Point_Sprites)));
        l->log(_I18n(" * Extended point parameters: ") + N_conv(isSupport(GF_Point_Param)));
        if (isSupport(GF_Point_Sprites))
        {
            l->log(_I18n(" * Max Point Size: ") + N_conv(mMaxPointSize));
        }
        l->log(_I18n(" * Vertex texture fetch: ") + N_conv(isSupport(GF_Program_VertexTexFetch)));
        l->log(_I18n(" * Number of world matrices: ") + N_conv(mMaxSpace));
        l->log(_I18n(" * Number of texture units: ") + N_conv(mMaxTextureUnit));
        l->log(_I18n(" * Stencil buffer depth: ") + N_conv(mStencilBit));

        if (isSupport(GF_Program_VertexTexFetch))
        {
            l->log(_I18n("   - Max vertex textures: ") + N_conv(mMaxVertexTexUnit));
        }
        l->log(_I18n(" * Render to Vertex Buffer : ") + N_conv(isSupport(GF_Render_To_Vertex)));

        if (isSupport(GF_GL) || isSupport(GF_GL_ES))
        {
            l->log(_I18n(" * PBuffer support: ") + N_conv(isSupport(GF_GL_PBuffer)));
            l->log(_I18n(" * Separate shader objects: ") + N_conv(isSupport(GF_GLSL_SeparateShaderObject)));
        }

        if (isSupport(GF_D3D))
        {
            l->log(_I18n(" * DirectX per stage constants: ") + N_conv(isSupport(GF_D3D_PerStageConstant)));
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RenderSys
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    const Matrix4f RenderSys::ProjClipSpace2DToImageSpace = Matrix4f(
        0.5,    0,      0,      0.5,
        0,      -0.5,   0,      0.5,
        0,      0,      1,      0,
        0,      0,      0,      1);
    //---------------------------------------------------------------------
    RenderSys::Listener* RenderSys::mListener = 0;
    //-----------------------------------------------------------------------
    RenderSys::RenderSys() :
        mTextureManager(0),
        // 使用顺时针顶点拣选?即在图形正面是逆时针方向,它和 OpenGL和其他右手坐标系系统相同
        mCullingMode(CM_Clockwise),
        mVSyncInterval(1),
        mEnableTexCount(0),
        mRenderCount(0),
        mCurrentRender(0),
        mDepthBiasBase(0.0f),
        mDepthBiasFactor(0.0f),
        mDepthBiasSlopeScale(0.0f),
        mInstanceData(0),
        mInstanceCount(1),
        mTextureProjRel(Vector3f::ZERO),
        mVendorFeature(0),
        mFeature(0),
        mActiveVGPP(0),
        mActiveGGPP(0),
        mActiveFGPP(0),
        mActiveTGPP(0),
        mActiveDGPP(0),
        mActiveCGPP(0),
        mSysFeatureValid(true),
        mTexProjRelative(false),
        mClipValid(true),
        mVertexTextureShared(false),
        mBackFaceInCast(true),
        mAutoDepthBias(false),
        mVPMode(false),
        mGPMode(false),
        mFPMode(false),
        mTPMode(false),
        mDPMode(false),
        mCPMode(false),
        mVSync(true),
        mWBuffer(false),
        mInvertVertexWinding(false),
        mDefaultRenderType(RPT_Normal),
        mDefaultComputeType(CPT_Normal),
        mScriptLevel(100)
        mActiveFrame(0), 
        mViewportCount(16u), 
        mBufferManager(0), 
        mGpuBufValid(true), 
        mUavStartingSlot( 1 ), 
        mGpuParamBuffer( 0 ),  
        mShaderVersion(0), 
        mReverseDepth(true)
    {
        mEventNames.push_back("RenderSystemCapabilitiesCreated");

        mFixedPipelineMode = true;
        
        mScriptPattern.push_back( "*.material.json" );
        N_Only(ResourceScheme).add(this);
        
        /*
        memset( mBlocks, 0, sizeof( mBlocks ) );

        mStencilList.reserve(4096);
        mBlendList.reserve(4096);

        mActiveBlocks[HBT_DepthStencil].reserve(32);
        mFreeBlockIds[HBT_DepthStencil].reserve(32);
        mActiveBlocks[HBT_Blend].reserve(32);
        mFreeBlockIds[HBT_Blend].reserve(32);
        mActiveBlocks[HBT_Sampler].reserve(64);
        mFreeBlockIds[HBT_Sampler].reserve(64);
        
        for( uint8 i = 0; i < 32; ++i )
            mFreeBlockIds[HBT_DepthStencil].push_back( (32 - 1) - i );

        for( uint8 i = 0; i < 32; ++i )
            mFreeBlockIds[HBT_Blend].push_back( (32 - 1) - i );

        for( uint8 i = 0; i < 64; ++i )
        {
            mTexSampleList[i].mId = i;
            mTexSampleList[i].mLifetimeId = i;
            mBlocks[HBT_Sampler][i] = &mTexSampleList[i];
            mFreeBlockIds[HBT_Sampler].push_back( (64 - 1) - i );
        }*/
    }
    //-----------------------------------------------------------------------
    RenderSys::~RenderSys()
    {
        shutdown();

        if (mVendorFeature)
        {
            N_delete mVendorFeature;
            mVendorFeature = 0;
        }
        if (!mSysFeatureValid && mFeature)
        {
            N_delete mFeature;
            mFeature = 0;
        }
        
        N_Only(ResourceScheme).remove(this);

        RenderPatternArrayList::iterator i, iend = mRenderPatternList.end();
        for(i = mRenderPatternList.begin(); i != iend; ++i )
        {
            RenderPatternList::iterator j, jend = i->second.end();
            for(j = i->second.begin(); j != jend; ++j )
            {
                RenderPattern * rp = *j;
                if(rp && rp->isAutoDestroy())
                {
                    N_delete rp;
                }
            }
        }
        mRenderPatternList.clear();
        
        ComputePatternList::iterator j, jend = mComputePatternList.end();
        for(j = mComputePatternList.begin(); j != jend; ++j )
        {
            ComputePattern * cp = j->second;
            if(cp && cp->isAutoDestroy())
            {
                N_delete cp;
            }
        }
        mComputePatternList.clear();
    }
    //-----------------------------------------------------------------------
    /*void RenderSys::_initAllFrame()
    {
        FrameList::iterator i, iend = mFrameObjList.end();
        for(i = mFrameObjList.begin(); i != iend; ++i)
        {
            i->second->resetFPS();
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::_updateAllFrame(bool swap)
    {
        // Update all in order of priority
        // This ensures render-to-texture targets get updated before render windows
        FrameLevelList::iterator i, iend = mFrameLevelList.end();
        for(i = mFrameLevelList.begin(); i != iend; ++i)
        {
            if(i->second->isActive() && i->second->isAutoFlush())
                i->second->flush(swap);
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::_swapAllFrame(bool vsync)
    {
        // Update all in order of priority
        // This ensures render-to-texture targets get updated before render windows
        FrameLevelList::iterator i, iend = mFrameLevelList.end();
        for(i = mFrameLevelList.begin(); i != iend; ++i)
        {
            if(i->second->isActive() && i->second->isAutoFlush())
                i->second->swap(vsync);
        }
    }*/
    //-----------------------------------------------------------------------
    void RenderSys::init()
    {
        mVPMode = false;
        mGPMode = false;
        mFPMode = false;
        mTPMode = false;
        mDPMode = false;
        mCPMode = false;
    }
    //-----------------------------------------------------------------------
    void RenderSys::setupConfig()
    {
        SysOption optFullScreen;
        optFullScreen.mID = N_PropertyWindow_FullWindow;
        optFullScreen.mName = _T("Full Screen");
        optFullScreen.mReserve.push_back(_T("No"));
        optFullScreen.mReserve.push_back(_T("Yes"));
        optFullScreen.mValue = _T("No");
        optFullScreen.mReserveEnable = true;
        mOptions[optFullScreen.mID] = optFullScreen;
        mOptionData.add(optFullScreen.mID, optFullScreen.mValue);

        SysOption optVSync;
        optVSync.mID = N_PropertyWindow_Vsync;
        optVSync.mName = _T("VSync");
        optVSync.mReserveEnable = true;
        optVSync.mReserve.push_back(_T("No"));
        optVSync.mReserve.push_back(_T("Yes"));
        optVSync.mValue = _T("Yes");
        mOptions[optVSync.mID] = optVSync;
        mOptionData.add(optVSync.mID, optVSync.mValue);

        SysOption optSRGB;
        optSRGB.mID = N_PropertyWindow_sRGB;
        optSRGB.mName = _T("sRGB Gamma Conversion");
        optSRGB.mReserveEnable = true;
        optSRGB.mReserve.push_back(_T("No"));
        optSRGB.mReserve.push_back(_T("Yes"));
        optSRGB.mValue = _T("No");
        mOptions[optSRGB.mID] = optSRGB;
        mOptionData.add(optSRGB.mID, optSRGB.mValue);

        SysOption optStereoMode;
        optStereoMode.mID = N_PropertyWindow_Stereo;
        optStereoMode.mName = _T("Stereo Mode");
        optStereoMode.mReserve.push_back(N_conv(SMT_NONE));
        optStereoMode.mReserve.push_back(N_conv(SMT_Stereo));
        optStereoMode.mValue = optStereoMode.mReserve[0];
        optStereoMode.mReserveEnable = true;
        mOptions[optStereoMode.mID] = optStereoMode;
        mOptionData.add(optStereoMode.mID, optStereoMode.mValue);
    }
    //-----------------------------------------------------------------------
    bool RenderSys::setConfig(const String & name, const String & value)
    {
        SysOptionList::iterator o, oend = mOptions.end();
        for (o = mOptions.begin(); o != oend; ++o)
        {
            if (o->second.mName == name)
            {
                o->second.mValue = value;
                mOptionData.add(o->second.mID, value);
                break;
            }
        }
        if (o == oend)
            return false;
        return true;
    }
    //-----------------------------------------------------------------------
    void RenderSys::addConfig(PropertyID pid, const String & name, const String & value)
    {
        SysOptionList::iterator o = mOptions.find(pid);
        if(o != mOptions.end())
        {
            o->second.mName == name;
            o->second.mValue = value;
        }
        else
        {
            SysOption optStereoMode;
            optStereoMode.mID = pid;
            optStereoMode.mName = name;
            optStereoMode.mValue = value;
            optStereoMode.mReserveEnable = true;
            mOptions.insert(Npair(pid, optStereoMode));
        }
        mOptionData.add(pid, value);
    }
    //-----------------------------------------------------------------------
    void RenderSys::setCustomFeature(RenderFeature * rsf)
    {
        if(mVendorFeature != 0)
        {
            N_EXCEPT(Internal, _I18n("初始化前调用"));
        }
        if (!mSysFeatureValid && mFeature)
        {
            N_delete mFeature;
        }
        mFeature = rsf;
        mSysFeatureValid = false;
    }
    //-----------------------------------------------------------------------
    void RenderSys::setBackFaceInCast(bool b)
    {
        if(mBackFaceInCast != b)
        {
            mBackFaceInCast = b;

            RenderPatternArrayList::iterator i, iend = mRenderPatternList.end();
            for(i = mRenderPatternList.begin(); i != iend; ++i )
            {
                RenderPatternList::iterator j, jend = i->second.end();
                for(j = i->second.begin(); j != jend; ++j )
                {
                    RenderPattern * rp = *j;
                    if(rp)
                    {
                        MaterialList::const_iterator itor = rp->mMaterialList.begin();
                        MaterialList::const_iterator end  = rp->mMaterialList.end();

                        while( itor != end )
                        {
                            ShaderChMaterial * mat = itor->second.mMaterial;
                            mat->setStencil(mat->getStencil(ShadowType::ST_Normal), ShadowType::ST_Normal);

                            ++itor;
                        }
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::setupFFPParam()
    {
        if (mFFGPP)
            return;

        mFFGPP = N_new GpuProgramParam();
        mFFGPP->setDefine(new GpuParamDefine());
        mFFGPP->set(0, GSP_Matrix_Space);
        mFFGPP->set(1, GSP_Matrix_View);
        mFFGPP->set(2, GSP_Matrix_Proj);
        mFFGPP->set(3, GSP_Ch_Ambient);
        mFFGPP->set(4, GSP_Ch_Diffuse);
        mFFGPP->set(5, GSP_Ch_Specular);
        mFFGPP->set(6, GSP_Ch_Emissive);
        mFFGPP->set(7, GSP_Ch_Shininess);
        mFFGPP->set(8, GSP_Ponit_Params);
        mFFGPP->set(9, GSP_Fog_Params); // ACT_FOG_PARAMS
        //mFFGPP->set(19, Vector4f::ZERO); // ACT_FOG_COLOUR
        mFFGPP->set(10, GSP_Ambient);

        // allocate per light parameters. slots 21..69
        for (int i = 0; i < NII_MAX_LIGHT; i++)
        {
            size_t light_offset = 11 + i * 6;
            mFFGPP->set(light_offset + 0, 0, Vector4f::ZERO); // position
            mFFGPP->set(light_offset + 1, 0, Vector4f::ZERO); // direction
            mFFGPP->set(light_offset + 2, 0, Vector4f::ZERO); // diffuse
            mFFGPP->set(light_offset + 3, 0, Vector4f::ZERO); // specular
            mFFGPP->set(light_offset + 4, 0, Vector4f::ZERO); // attenuation
            mFFGPP->set(light_offset + 5, 0, Vector4f::ZERO); // spotlight
        }
    }
    //-----------------------------------------------------------------------
    bool RenderSys::createWindow(const WindowDefineList & deflist, WindowList & winlist)
    {
        Nui32 fullscreenWindowsCount = 0;

        for(NCount cur = 0; cur < deflist.size(); ++cur)
        {
            const WindowDefine * curDef = &deflist[cur];

            if(curDef->mFull)
                fullscreenWindowsCount++;

            bool renderWindowFound = false;

            for(NCount nSecWindow = cur + 1 ; nSecWindow < deflist.size(); ++nSecWindow)
            {
                if(curDef->mName == deflist[nSecWindow].mName)
                {
                    renderWindowFound = true;
                    break;
                }
            }

            // 确保我们不存在一个相同名字的渲染目标
            if(renderWindowFound)
            {
                String msg;

                msg = _I18n("A render target of the same name '") + curDef->mName +
                    _I18n("' already exists.  You cannot create a new window with this name.");
                N_EXCEPT(Internal, msg);
            }
        }

        if(fullscreenWindowsCount > 0)
        {
            // 不能混搅全屏和窗口渲染窗体
            if(fullscreenWindowsCount != deflist.size())
            {
                N_EXCEPT(InvalidParam, _I18n("Can not create mix of full screen and windowed rendering windows"));
            }
        }
        return true;
    }
    //---------------------------------------------------------------------------------------------
    void RenderSys::destroyWindow(ViewWindow * win)
    {
        WindowSet::iterator itor = mWindows.find(win);
        if(itor == mWindows.end())
        {
            N_EXCEPT(NotExist, "Window does not belong to us or is already deleted!");
        }
        mWindows.erase(win);
        N_delete win;
    }
    //---------------------------------------------------------------------------------------------
    /*void RenderSys::destroyFrame(const FrameObj * fo)
    {
        FrameObj * de = remove(fo);
        N_delete de;
    }
    //---------------------------------------------------------------------------------------------
    void RenderSys::add(FrameObj * obj)
    {
        N_assert(obj->getFlushLevel() < NII_NUM_RENDERTARGET_GROUPS, "error");

        mFrameObjList.insert(Npair(obj->getName(), obj));
        mFrameLevelList.insert(Npair(obj->getFlushLevel(), obj));
    }
    //---------------------------------------------------------------------------------------------
    FrameObj * RenderSys::getFrame(const String & name)
    {
        FrameList::iterator it = mFrameObjList.find(name);
        if(it != mFrameObjList.end())
        {
            return it->second;
        }
        return 0;
    }
    //---------------------------------------------------------------------------------------------
    FrameObj * RenderSys::remove(const FrameObj * fo)
    {
        FrameObj * ret = NULL;
        FrameList::iterator it = mFrameObjList.find(fo->getName());
        if(it != mFrameObjList.end())
        {
            ret = it->second;
            FrameLevelList::iterator itarg, itargend = mFrameLevelList.end();
            for(itarg = mFrameLevelList.begin(); itarg != itargend; ++itarg)
            {
                if(itarg->second == ret)
                {
                    mFrameLevelList.erase(itarg);
                    break;
                }
            }

            mFrameObjList.erase(it);
        }

        return ret;
    }*/
    //-----------------------------------------------------------------------
    void RenderSys::_enableTexture(Nidx unit, const ShaderChTextureUnit * tl)
    {
        const Texture * tex = tl->getTexture();
        
        if(tl->getFetchType() == TDF_VS)
        {
            if(mFeature->isSupport(GF_Program_VertexTexFetch) && !mVertexTextureShared)
            {
                _bindVertexTexture(unit, tex);
                //_bindTexture(unit, 0, true);
            }
            else
            {
                //_bindVertexTexture(unit, 0);
                _bindTexture(unit, tex, true);
            }
        }
        else if (tl->getFetchType() == TDF_GS)
        {
            if(mFeature->isSupport(GF_Program_Geometry))
            {
                _bindGeometryTexture(unit, tex);
                //_bindTexture(unit, 0, true);
            }
            else
            {
                //_bindGeometryTexture(unit, 0);
                _bindTexture(unit, tex, true);
            }
        }
        else if (tl->getFetchType() == TDF_DS)
        {
            if(mFeature->isSupport(GF_Program_Domain))
            {
                _bindDomainTexture(unit, tex);
                //_bindTexture(unit, 0, true);
            }
            else
            {
                //_bindDomainTexture(unit, 0);
                _bindTexture(unit, tex, true);
            }
        }

        else if (tl->getFetchType() == TDF_TS)
        {
            if(mFeature->isSupport(GF_Program_Tess))
            {
                _bindTessControlTexture(unit, tex);
                //_bindTexture(unit, 0, true);
            }
            else
            {
                //_bindTessControlTexture(unit, 0);
                _bindTexture(unit, tex, true);
            }
        }
        else
        {
            _bindTexture(unit, tex, true);
        }

        _setTextureCoordSet(unit, tl->getCoordSet());

        _setTextureSample(unit, *tl->getSample());

        _setTextureBlend(unit, *tl->getColourBlend());
        _setTextureBlend(unit, *tl->getAlphaBlend());

        bool anyCalcs = false;
        if(tl->isEnvMap())
        {
            ShaderChTextureUnit::EnvMapMode temp = tl->getEnvMapType();
            if(temp == ShaderChTextureUnit::ENM_Sphere)
            {
                _setTextureCoordPrc(unit, TCP_ENV_SPHERE);
                anyCalcs = true;
            }
            else if(temp == ShaderChTextureUnit::ENM_Planar)
            {
                _setTextureCoordPrc(unit, TCP_ENV_REFLECTION);
                anyCalcs = true;
            }
            else if(temp == ShaderChTextureUnit::ENM_Reflection)
            {
                _setTextureCoordPrc(unit, TCP_ENV_REFLECTION_INV);
                anyCalcs = true;
            }
            else if(temp == ShaderChTextureUnit::ENM_Normal)
            {
                _setTextureCoordPrc(unit, TCP_ENV_NORMAL);
                anyCalcs = true;
            }
        }
        else if(tl->isProjTexture())
        {
            _setTextureCoordPrc(unit, TCP_ENV_PROJECTIVE, tl->getProjFrustum());
            anyCalcs = true;
        }

        if(!anyCalcs)
        {
            _setTextureCoordPrc(unit, TCP_NONE);
        }

        _setTextureMatrix(unit, tl->getMatrix());
    }
    //---------------------------------------------------------------------
    void RenderSys::_enableTexture(NCount count)
    {
        NCount limit = NII_MAX_TextureLayer;
        if(limit > mEnableTexCount)
            limit = mEnableTexCount;
        mEnableTexCount = count;
        for(NCount i = count; i < limit; ++i)
        {
            _disableTexture(i);
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::_disableTexture(Nidx unit)
    {
        _bindTexture(unit, 0, false);
    }
    //-----------------------------------------------------------------------
    void RenderSys::_bindVertexTexture(Nidx unit, const Texture * tex)
    {
        N_EXCEPT(UnImpl, _I18n("error"));
    }
    //-----------------------------------------------------------------------
    void RenderSys::_bindGeometryTexture(Nidx unit, const Texture * tex)
    {
        N_EXCEPT(UnImpl, _I18n("error"));
    }
    //-----------------------------------------------------------------------
    void RenderSys::_bindTessControlTexture(Nidx unit, const Texture * tex)
    {
        N_EXCEPT(UnImpl, _I18n("error"));
    }
    //-----------------------------------------------------------------------
    void RenderSys::_bindDomainTexture(Nidx unit, const Texture * tex)
    {
        N_EXCEPT(UnImpl, _I18n("error"));
    }
    //---------------------------------------------------------------------
    /*void RenderSys::destroyAllExtBuffer(bool bCleanManualBuffers)
    {
        ExtFrameList::iterator itMap = mExtFrameList.begin();
        ExtFrameList::iterator enMap = mExtFrameList.end();

        while(itMap != enMap)
        {
            ExtFrameBufferList::const_iterator itor = itMap->second.begin();
            ExtFrameBufferList::const_iterator end  = itMap->second.end();

            while(itor != end)
            {
                if(bCleanManualBuffers || !(*itor)->isManual())
                    N_delete *itor;
                ++itor;
            }

            itMap->second.clear();

            ++itMap;
        }

        mExtFrameList.clear();
    }*/
    //-----------------------------------------------------------------------
    void RenderSys::_setDepthBias(bool derive, NIIf base, NIIf basefactor, NIIf slopeScale)
    {
        mAutoDepthBias = derive;
        mDepthBiasBase = base;
        mDepthBiasFactor = basefactor;
        mDepthBiasSlopeScale = slopeScale;
        _setDepthBias(base, slopeScale);
    }
    //-----------------------------------------------------------------------
    NIIf RenderSys::getDepthRange() const
    { 
        return 1.0f; 
    }
    //-----------------------------------------------------------------------
    CullingMode RenderSys::_getCullingMode() const
    {
        return mCullingMode;
    }
    //-----------------------------------------------------------------------
    /*void RenderSys::createExtBuffer(FrameObj * fo)
    {
        GroupID grp = fo->getRasterGroup();
        if(grp == GroupUnknow)
            return; //FrameObj explicitly requested no depth buffer

        //Find a depth buffer in the pool
        ExtFrameBufferList::const_iterator itor = mExtFrameList[grp].begin();
        ExtFrameBufferList::const_iterator end  = mExtFrameList[grp].end();

        bool bAttached = false;
        while(itor != end && !bAttached)
            bAttached = fo->attachExt(*itor++);

        if(!bAttached)
        {
            ExtFrameBuffer * newext = createExtFrameImpl(fo);

            if(newext)
            {
                newext->setGroup(grp);
                mExtFrameList[grp].push_back(newext);

                bAttached = fo->attachExt(newext);

                N_assert(bAttached, "A new ExtFrameBuffer for a FrameObj was created, but after creation it says it's incompatible with that RT" );
            }
            else
            {
                N_Only(Log).log(_I18n("WARNING: Couldn't create a suited ExtFrameBuffer for RT: ") + fo->getName());
            }
        }
    }*/
    //-----------------------------------------------------------------------
    void RenderSys::shutdown()
    {
        PixelCountQueryList::iterator i, iend = mPixelQueryList.end();
        for (i = mPixelQueryList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mPixelQueryList.clear();
        
        //destroyAllExtBuffer();
        
        FrameBufferObjectList::const_iterator itor = mFBOList.begin();
        FrameBufferObjectList::const_iterator end  = mFBOList.end();

        while( itor != end )
            N_delete *itor++;

        mFBOList.clear();

        N_delete mTextureManager;
        mTextureManager = 0;
        N_delete mBufferManager;
        mBufferManager = 0;

        {
            // Remove all windows.
            // (destroy primary window last since others may depend on it)
            ViewWindow *primary = 0;
            WindowSet::const_iterator itor = mWindows.begin();
            WindowSet::const_iterator end  = mWindows.end();

            while( itor != end )
            {
                //Set mTextureManager to 0 as it is no longer valid on shutdown
                if( (*itor)->getBuffer() )
                    (*itor)->getBuffer()->mTextureManager = 0;
                if( (*itor)->getDepth() )
                    (*itor)->getDepth()->mTextureManager = 0;
                if( (*itor)->getStencil() )
                    (*itor)->getStencil()->mTextureManager = 0;

                if( !primary && (*itor)->isFirstWindow() )
                    primary = *itor;
                else
                    N_delete *itor;

                ++itor;
            }

            N_delete primary;
            mWindows.clear();
        }

        // 移去所有的渲染目标.(最后删除主要的目标,因为其他对象可能依赖它)
        /*FrameObj * primary = 0;
        FrameList::iterator it, itend = mFrameObjList.end();
        for(it = mFrameObjList.begin(); it != itend; ++it)
        {
            if (!primary && it->second->isFirstWindow())
                primary = it->second;
            else
                N_delete it->second;
        }
        if(primary)
            N_delete primary;
        mFrameObjList.clear();
        // 清空优先级组列表
        mFrameLevelList.clear();*/
        
        /*
            for( size_t i=0; i < RPT_Count; ++i )
            {
                if( mRenderPatternList[i] )
                    mRenderPatternList[i]->clearCache();
            }

            {
                IdxList::const_iterator itor = mActiveBlocks[HBT_DepthStencil].begin();
                IdxList::const_iterator end  = mActiveBlocks[HBT_DepthStencil].end();
                while( itor != end )
                {
                    ShaderChStencil * block = static_cast<ShaderChStencil*>( mBlocks[HBT_DepthStencil][*itor] );
                    destroyStencil( block );
                    ++itor;
                }

                itor = mActiveBlocks[HBT_Blend].begin();
                end  = mActiveBlocks[HBT_Blend].end();
                while( itor != end )
                {
                    ShaderChBlend * block = static_cast<ShaderChBlend*>( mBlocks[HBT_Blend][*itor] );
                    destroyBlend( block );
                    ++itor;
                }

                itor = mActiveBlocks[HBT_Sampler].begin();
                end  = mActiveBlocks[HBT_Sampler].end();
                while( itor != end )
                destroyTexSample( &mTexSampleList[*itor++] );
            }

            {
                DescriptorSetTextureSet::iterator itor = mTextureList.begin();
                DescriptorSetTextureSet::iterator end  = mTextureList.end();
                while( itor != end )
                {
                    //const_cast see RenderSys::destroyDescriptorSetTexture comments
                    DescriptorSetTexture *descSetPtr = const_cast<DescriptorSetTexture*>( &(*itor) );
                    _descriptorSetTextureDestroyed( descSetPtr );
                {
                    SCTextureList::iterator itor = mTextureList.begin();
                    SCTextureList::iterator end  = mTextureList.end();
                    while( itor != end )
                    {
                        //const_cast see RenderSys::destroyDescriptorSetTexture comments
                        ShaderChTexture *descSetPtr = const_cast<ShaderChTexture*>( &(*itor) );
                        destroyTexture(descSetPtr);
                        ++itor;
                    }
                }
            {
                TextureSampleList::iterator itor = mSampleList.begin();
                TextureSampleList::iterator end  = mSampleList.end();
                while( itor != end )
                {
                    //const_cast see RenderSys::destroyDescriptorSetTexture comments
                    TextureSample *descSetPtr = const_cast<TextureSample*>( &(*itor) );
                    destroySampler( descSetPtr );
                    ++itor;
                }
            }
        */
    }
    //-----------------------------------------------------------------------
    void RenderSys::convertType(const Colour & src, Nui32 * dest)
    {
        *dest = Engine::getColourValue(src, getColourType());
    }
    //-----------------------------------------------------------------------
    void RenderSys::_setSpace(const Matrix4f * m, NCount count)
    {
        //这里不处理矩阵,派生类应该注意好设置硬件矩阵为无
        _setModel(Matrix4f::IDENTITY);
    }
    //-----------------------------------------------------------------------
    void RenderSys::setGeometry(const GeometryRaw * geo)
    {
        mGeometry = geo;
        
        if(mGeometry->mMark & GRT_Index)
        {
            if(mGeometry->mCount == 0)
                const_cast<GeometryRaw &>(mGeometry)->mCount = mGeometry->mIndex->mCount;
        }
        else
        {
            if(mGeometry->mCount == 0)
                const_cast<GeometryRaw &>(mGeometry)->mCount = mGeometry->mVertex->mCount;
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::_render()
    {
        // 更新状态
        NIIi instcnt = std::max<NCount>(mGeometry->mInstanceCount, 1);
        NCount val = mGeometry->mCount * instcnt * mRenderCount;

        mCurrentRender = 0;

        switch(mGeometry->mType)
        {
        case GeometryRaw::OT_TRIANGLE_LIST:
            mMetrics.mTriangleCount += (val / 3);
            break;
        case GeometryRaw::OT_TRIANGLE_STRIP:
        case GeometryRaw::OT_TRIANGLE_FAN:
            mMetrics.mTriangleCount += (val - 2);
            break;
        case GeometryRaw::OT_POINT_LIST:
        case GeometryRaw::OT_LINE_LIST:
        case GeometryRaw::OT_LINE_STRIP:
            break;
        }

        mMetrics.mVertexCount += mGeometry->mVertex->mCount * instcnt;
        mMetrics.mBatchCount += mRenderCount;

        // 整理修建面, 在这里处理,以防矩阵问题
        if(!mClipValid)
        {
            setClipPlanesImpl(mClipPlanes);
            mClipValid = true;
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::setInvertVertexWinding(bool b)
    {
        mInvertVertexWinding = b;
    }
    //-----------------------------------------------------------------------
    bool RenderSys::isInvertVertexWinding()
    {
        return mInvertVertexWinding;
    }
    //---------------------------------------------------------------------
    void RenderSys::addClipPlane(const Plane & p)
    {
        mClipPlanes.push_back(p);
        mClipValid = false;
    }
    //---------------------------------------------------------------------
    void RenderSys::setClipPlanes(const PlaneList & planes)
    {
        if (planes != mClipPlanes)
        {
            mClipPlanes = planes;
            mClipValid = false;
        }
    }
    //---------------------------------------------------------------------
    void RenderSys::resetClipPlanes()
    {
        if(!mClipPlanes.empty())
        {
            mClipPlanes.clear();
            mClipValid = false;
        }
    }
    //-----------------------------------------------------------------------
    /*void RenderSys::_notifyDestroy(const Camera * cam)
    {
        FrameList::iterator i, iend = mFrameObjList.end();
        for(i = mFrameObjList.begin(); i != iend; ++i)
        {
            FrameObj * target = i->second;
            target->_notifyDestroy(cam);
        }
    }*/
    //-----------------------------------------------------------------------
    void RenderSys::loadMaterials(const String & filename, GroupID gid, const String & additionalTextureExtension)
    {
        DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource( filename, gid );

        vector<char>::type fileData;
        fileData.resize( stream->size() + 1 );
        if( !fileData.empty() )
        {
            stream->read( &fileData[0], stream->size() );

            //Add null terminator just in case (to prevent bad input)
            fileData.back() = '\0';
            HlmsJson hlmsJson( this);
            hlmsJson.loadMaterials( stream->getName(), gid, &fileData[0], additionalTextureExtension );
        }
    }
    //-----------------------------------------------------------------------------------
    void RenderSys::saveMaterials(RenderPatternType type, const String & filename, const String & additionalTextureExtension)
    {
        assert( type != RPT_Count );
        assert( type != RPT_Single );

        String jsonString;
        HlmsJson hlmsJson( this);
        hlmsJson.saveMaterials( mRenderPatternList[type], jsonString, additionalTextureExtension );

        std::ofstream file( filename.c_str(), std::ios::binary | std::ios::out );
        if( file.is_open() )
            file.write( jsonString.c_str(), jsonString.size() );
        file.close();
    }
    //-----------------------------------------------------------------------------------
    void RenderSys::saveMaterial( const ShaderChMaterial * mat, const String & filename, const String & additionalTextureExtension )
    {
        String jsonString;
        HlmsJson hlmsJson( this);
        hlmsJson.saveMaterial( mat, jsonString, additionalTextureExtension );

        std::ofstream file( filename.c_str(), std::ios::binary | std::ios::out );
        if( file.is_open() )
            file.write( jsonString.c_str(), jsonString.size() );
        file.close();
    }
    //-----------------------------------------------------------------------------------
    void RenderSys::parse(DataStream * stream, GroupID gid, PtrList & out)
    {
        vector<char>::type fileData;
        fileData.resize( stream->size() + 1 );
        if( !fileData.empty() )
        {
            stream->read( &fileData[0], stream->size() );

            String addtexExt;
            /*StringMap::const_iterator itExt = mAdditionalTextureExtensionsPerGroup.find( gid );
            if( itExt != mAdditionalTextureExtensionsPerGroup.end() )
                addtexExt = itExt->second;*/

            //Add null terminator just in case (to prevent bad input)
            fileData.back() = '\0';
            HlmsJson hlmsJson(this);
            hlmsJson.loadMaterials(stream->getName(), gid, &fileData[0], addtexExt);
        }
    }
    //-----------------------------------------------------------------------
    bool RenderSys::updateParam()
    {
        if(mRenderCount <= 1)
            return false;

        --mRenderCount;
        ++mCurrentRender;
        return true;
    }
    //-----------------------------------------------------------------------
    void RenderSys::setRenderCount(NCount count)
    {
        mRenderCount = count;
    }
    //-----------------------------------------------------------------------
    void RenderSys::destroy(PixelCountQuery * q)
    {
        PixelCountQueryList::iterator i = std::find(mPixelQueryList.begin(), mPixelQueryList.end(), q);
        if (i != mPixelQueryList.end())
        {
            N_delete q;
            mPixelQueryList.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::on(GpuProgram * prg)
    {
        switch(prg->getType())
        {
        case GpuProgram::ST_VS:
            // 标记修剪面改变(可编程改变了空间)
            if(!mVPMode && !mClipPlanes.empty())
                mClipValid = false;
            mVPMode = true;
            break;
        case GpuProgram::ST_GS:
            mGPMode = true;
            break;
        case GpuProgram::ST_FS:
            mFPMode = true;
            break;
        case GpuProgram::ST_TS:
            mTPMode = true;
            break;
        case GpuProgram::ST_DS:
            mDPMode = true;
            break;
        case GpuProgram::ST_CS:
            mCPMode = true;
            break;
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::off(GpuProgram::ShaderType type)
    {
        switch(type)
        {
        case GpuProgram::ST_VS:
            // 标记修剪面改变(可编程改变了空间)
            if(mVPMode && !mClipPlanes.empty())
                mClipValid = false;
            mVPMode = false;
            break;
        case GpuProgram::ST_GS:
            mGPMode = false;
            break;
        case GpuProgram::ST_FS:
            mFPMode = false;
            break;
        case GpuProgram::ST_TS:
            mTPMode = false;
            break;
        case GpuProgram::ST_DS:
            mDPMode = false;
            break;
        case GpuProgram::ST_CS:
            mCPMode = false;
            break;
        }
    }
    //-----------------------------------------------------------------------
    bool RenderSys::isOn(GpuProgram::ShaderType type)
    {
        switch(type)
        {
        case GpuProgram::ST_VS:
            return mVPMode;
        case GpuProgram::ST_GS:
            return mGPMode;
        case GpuProgram::ST_FS:
            return mFPMode;
        case GpuProgram::ST_TS:
            return mTPMode;
        case GpuProgram::ST_DS:
            return mDPMode;
        case GpuProgram::ST_CS:
            return mCPMode;
        }

        return false;
    }
    //---------------------------------------------------------------------
    void RenderSys::_setTextureProjRel(bool b, const Vector3f & pos)
    {
        mTexProjRelative = b;
        mTextureProjRel = pos;
    }
    //---------------------------------------------------------------------
    /*void RenderSys::_setFrame(FrameObj * fo) = 0;
    //---------------------------------------------------------------------
    FrameObj * RenderSys::_pauseFrame()
    {
        _endFrame();
        return N_new RenderSys::FrameObj;
    }
    //---------------------------------------------------------------------
    void RenderSys::_resumeFrame(FrameObj * fo)
    {
        _beginFrame();
        N_delete fo;
    }*/
    //---------------------------------------------------------------------
    SchemeID RenderSys::getMaterial() const
    {
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
        if( !(mFeature->isSupport(GF_Fixed_Pipeline)))
        {
            // I am returning the exact value for now - I don't want to add dependency for the RTSS just for one string
            return 1000;
        }
        else
#endif
        {
            return MaterialManager::Default;
        }
    }
    //-----------------------------------------------------------------------------------
    ShaderChMaterial * RenderSys::getMaterial(Nid name , RenderPatternType type) const
    {
        ShaderChMaterial * re = 0;

        MaterialList::const_iterator itor = mMaterialList.find(name);
        if(itor != mMaterialList.end())
            re = itor->second;

        if(!re)
        {
            N_error( "Can't find HLMS mat material '" + name.getFriendlyText() + "'. It may not be visible to this manager, try "
                "finding it by retrieving getRenderPattern()->getMaterial()");

            re = getMaterial(type);
        }

        return re;
    }
    //-----------------------------------------------------------------------------------
    ShaderChMaterial * RenderSys::getMaterial(RenderPatternType type) const
    {
        return mRenderPatternList[type]->getMaterial();
    }
    //---------------------------------------------------------------------
    bool RenderSys::getBufferType(ViewportType ) 
    { 
        return false; 
    }
    //---------------------------------------------------------------------
    void RenderSys::ref(const ShaderChBase * scb)
    {
        ShaderChBase * re = mBlocks[scb->mBlockType][scb->mId];
        if( re != scb )
        {
            N_EXCEPT(NotExist, "The ShaderChBase wasn't created with this manager!");
        }

        ++re->mRefCount;
    }
    //---------------------------------------------------------------------
    void RenderSys::sync(ShaderChBase * scb)
    {
        
    }
    //---------------------------------------------------------------------
    ShaderChColour * RenderSys::createColour(const String & name, bool bindToSys) const
    {
        return N_new ShaderChColour();
    }
    //---------------------------------------------------------------------
    ShaderChColour * RenderSys::getColour(const String & name) const
    {
        ColourList::const_iterator i = mColourList.find(name);
        if (i != mColourList.end())
        {
            return i->second;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void RenderSys::destroyColour(ShaderChColour * scc)
    {
        N_delete scc;
    }
    //---------------------------------------------------------------------
    ShaderChPoint * RenderSys::createPoint(const String & name, bool bindToSys) const
    {
        return N_new ShaderChPoint();
    }
    //---------------------------------------------------------------------
    ShaderChPoint * RenderSys::getPoint(const String & name) const
    {
        PointList::const_iterator i = mPointList.find(name);
        if (i != mPointList.end())
        {
            return i->second;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void RenderSys::destroyPoint(ShaderChPoint * scp)
    {
        N_delete scp;
    }
    //---------------------------------------------------------------------
    ShaderChBlend * RenderSys::createBlend(const String & name, bool bindToSys) const
    {
        return N_new ShaderChBlend();
    }
    //---------------------------------------------------------------------
    ShaderChBlend * RenderSys::getBlend(const String & name) const
    {
        BlendList::const_iterator i = mBlendList.find(name);
        if (i != mBlendList.end())
        {
            return i->second;
        }
        return 0;
        
        /**
            ShaderChBlend * re = getBasicBlock<ShaderChBlend, HBT_Blend, 4096>(mBlendList, blend);

            if(!re->mRefCount)
            {
                if(!(blend.mDestBlendFactor == SBF_ZERO &&
                       blend.mSourceBlendFactor != SBF_DEST_COLOUR &&
                       blend.mSourceBlendFactor != SBF_ONE_MINUS_DEST_COLOUR &&
                       blend.mSourceBlendFactor != SBF_DEST_ALPHA &&
                       blend.mSourceBlendFactor != SBF_ONE_MINUS_DEST_ALPHA))
                {
                    re->mIsTransparent |= 1u;
                }
                createBlend(re);
            }

            OGRE_ASSERT_LOW(re->mRefCount < 0xFFFF && "Reference count overflow!");
            ++re->mRefCount;

            return re;
        */
    }
    //-----------------------------------------------------------------------------------
    void RenderSys::destroyBlend(const ShaderChBlend * scb)
    {
        if(&mBlendList[scb->mLifetimeId] != scb)
        {
            N_EXCEPT(Internal, "The Blendblock wasn't created with this manager!");
        }

        --mBlendList[scb->mLifetimeId].mRefCount;

        if(!mBlendList[scb->mLifetimeId].mRefCount)
        {
            destroyBlend(&mBlendList[scb->mLifetimeId]);
            destroyBasicBlock(&mBlendList[scb->mLifetimeId]);
        }
    }
    //---------------------------------------------------------------------
    ShaderChFog * RenderSys::createFog(const String & name, bool bindToSys) const
    {
        return N_new ShaderChFog();
    }
    //---------------------------------------------------------------------
    ShaderChFog * RenderSys::getFog(const String & name) const
    {
        FogList::const_iterator i = mFogList.find(name);
        if (i != mFogList.end())
        {
            return i->second;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void RenderSys::destroyFog(ShaderChFog * scf)
    {
        N_delete scf;
    }
    //---------------------------------------------------------------------
    ShaderChStencil * RenderSys::createStencil(const String & name, bool bindToSys) const
    {
        return N_new ShaderChStencil();
    }
    //---------------------------------------------------------------------
    ShaderChStencil * RenderSys::getStencil(const String & name) const
    {
        StencilList::const_iterator i = mStencilList.find(name);
        if (i != mStencilList.end())
        {
            return i->second;
        }
        return 0;
        
        /**
        ShaderChStencil * re = getBasicBlock<ShaderChStencil, HBT_DepthStencil, 4096>( mStencilList, stencil );

        if( !re->mRefCount )
            createStencil( re );

        OGRE_ASSERT_LOW( re->mRefCount < 0xFFFF && "Reference count overflow!" );
        ++re->mRefCount;

        return re;
        */
    }
    //---------------------------------------------------------------------
    void RenderSys::destroyStencil(ShaderChStencil * scds) 
    {
        if(&mStencilList[scds->mLifetimeId] != scds)
        {
            N_EXCEPT(Internal, "The stencil wasn't created with this manager!");
        }
        if(!mStencilList[scds->mLifetimeId].mRefCount)
        {
            N_EXCEPT(Runtime, "This stencil has already been destroyed!");
        }

        --mStencilList[scds->mLifetimeId].mRefCount;

        if(!mStencilList[scds->mLifetimeId].mRefCount)
        {
            destroyStencil(&mStencilList[scds->mLifetimeId]);
            destroyBasicBlock(&mStencilList[scds->mLifetimeId]);
        }
    }
    //---------------------------------------------------------------------
    TextureBlend * RenderSys::createTexBlend(const String & name, bool bindToSys) const
    {
        return 0;
    }
    //---------------------------------------------------------------------
    TextureBlend * RenderSys::getTexBlend(const String & name) const
    {
        TexBlendList::const_iterator i = mTexBlendList.find(name);
        if (i != mTexBlendList.end())
        {
            return i->second;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void RenderSys::destroyTexBlend(TextureBlend * tb)
    {
        
    }
    //---------------------------------------------------------------------
    TextureSampleUnit * RenderSys::createTexSample(const String & name, bool bindToSys) const
    {
        return 0;
    }
    //---------------------------------------------------------------------
    TextureSampleUnit * RenderSys::getTexSample(const String & name) const
    {
        TexSampleList::const_iterator i = mTexSampleList.find(name);
        if(i != mTexSampleList.end())
        {
            return i->second;
        }
        return 0;
        /*
            assert( baseParams.mBlockType == HBT_Sampler &&
                    "baseParams.mBlockType should always be HBT_Sampler! You can ignore this assert,"
                    " but it usually indicates memory corruption (or you created the block without "
                    "its default constructor)." );

            bool errorsFixed = false;

            if( baseParams.mMaxAnisotropy < 1.0f )
            {
                baseParams.mMaxAnisotropy = 1.0f;
                N_log( "WARNING: Max anisotropy can't be lower than 1" );
            }

            if( baseParams.mMinFilter != FO_ANISOTROPIC && baseParams.mMagFilter != FO_ANISOTROPIC &&
                baseParams.mMipFilter != FO_ANISOTROPIC && baseParams.mMaxAnisotropy > 1.0f )
            {
                baseParams.mMaxAnisotropy = 1.0f;
                N_log( "WARNING: Max anisotropy must be 1 if no anisotropic filter is used." );
            }

            if( errorsFixed )
            {
                N_log( "WARNING: Invalid sampler block parameters detected. They've been corrected." );
            }

            IdxList::iterator itor = mActiveBlocks[HBT_Sampler].begin();
            IdxList::iterator end  = mActiveBlocks[HBT_Sampler].end();

            while( itor != end && mTexSampleList[*itor] != baseParams )
                ++itor;

            TextureSampleUnit * re = 0;
            if( itor != end )
            {
                //Already exists
                re = &mTexSampleList[*itor];
            }
            else
            {
                size_t idx = getFreeBasicBlock( HBT_Sampler, 0 );

                mTexSampleList[idx] = baseParams;
                //Restore the values which has just been overwritten and we need properly set.
                mTexSampleList[idx].mRefCount = 0;
                mTexSampleList[idx].mId = idx;
                mTexSampleList[idx].mLifetimeId = idx;
                mTexSampleList[idx].mBlockType = HBT_Sampler;
                createTexSample( &mTexSampleList[idx] );

                re = &mTexSampleList[idx];
            }

            assert( re->mRefCount < 0xFFFF && "Reference count overflow!" );
            ++re->mRefCount;

            return re;
        */
    }
    //---------------------------------------------------------------------
    void RenderSys::destroyTexSample(const TextureSampleUnit * tsu)
    {
        if(&mTexSampleList[tsu->mId] != tsu)
        {
            N_EXCEPT(Internal, "The Samplerblock wasn't created with this manager!");
        }

        --mTexSampleList[tsu->mId].mRefCount;

        if(!mTexSampleList[tsu->mId].mRefCount)
        {
            destroyTexSample(&mTexSampleList[tsu->mId]);
            destroyBasicBlock(&mTexSampleList[tsu->mId]);
        }
    }
    //---------------------------------------------------------------------
    const ShaderChTexture * RenderSys::createTexture(const String & name, bool BindtoRsys)
    {
        sct.checkValidity();

        assert(sct.mTextures.size() < NII_MAX_TextureLayer &&
                "Recompile Ogre w/ a different NII_MAX_TextureLayer value if you "
                "want to bind more textures (API/HW restrictions may also apply)");

        const ShaderChTexture * re = getDescriptorSet( mTextureList, sct, createTexture );
        return re;
    }
    //----------------------------------------------------------------------
    ShaderChTexture * RenderSys::getTexture(const String & name) const
    {
        
    }
    //----------------------------------------------------------------------
    void RenderSys::destroyTexture(const ShaderChTexture * sct)
    {
        destroyDescriptorSet(mTextureList, sct, destroyTexture);
    }
    //---------------------------------------------------------------------
    void RenderSys::syncTexture(ShaderChTexture * sct)
    {
        
    }
    //---------------------------------------------------------------------
    const TextureSample * RenderSys::createSampler(const TextureSample & ts)
    {
        ts.checkValidity();

        const TextureSample * re = getDescriptorSet(mSampleList, ts, createSampler);
        return re;
    }
    //---------------------------------------------------------------------
    TextureSample * RenderSys::getSampler(const String & name) const
    {
        
    }
    //---------------------------------------------------------------------
    void RenderSys::destroySampler(const TextureSample * ts)
    {
        destroyDescriptorSet(mSampleList, ts, destroySampler);
    }
    //---------------------------------------------------------------------
    void RenderSys::setColour(const String & name, Nmark applymark)
    {
        ColourList::iterator i = mColourList.find(name);
        if(i != mColourList.end())
        {
            setColour(*i->second, applymark);
        }
    }
    //---------------------------------------------------------------------
    void RenderSys::setPoint(const String & name, Nmark applymark)
    {
        PointList::iterator i = mPointList.find(name);
        if(i != mPointList.end())
        {
            setPoint(*i->second, applymark);
        }
    }
    //---------------------------------------------------------------------
    void RenderSys::setBlend(const String & name, Nmark applymark)
    {
        BlendList::iterator i = mBlendList.find(name);
        if(i != mBlendList.end())
        {
            setBlend(*i->second, applymark);
        }
    }
    //---------------------------------------------------------------------
    void RenderSys::setFog(const String & name, Nmark applymark)
    {
        FogList::iterator i = mFogList.find(name);
        if(i != mFogList.end())
        {
            setFog(*i->second, applymark);
        }
    }
    //---------------------------------------------------------------------
    void RenderSys::setStencil(const ShaderChStencil & d, Nmark applymark)
    {
        mStencilList[0] = d;

        // NB: We should always treat CCW as front face for consistent with default
        // culling mode.
        const bool mustFlip = ((mInvertVertexWinding && !mActiveFrame->isFlipping()) ||
            (!mInvertVertexWinding && mActiveFrame->isFlipping()));

        if( mustFlip )
        {
            mStencilList.flipStencil();
        }
    }
    //---------------------------------------------------------------------
    void RenderSys::setStencil(const String & name, Nmark applymark)
    {
        StencilList::iterator i = mStencilList.find(name);
        if (i != mStencilList.end())
        {
            setStencil(*i->second, applymark);
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::setRSO(const RenderStateObject * rso)
    {
        N_assert( (!rso) && "The PipelineStateObject must have been created via ");

        mActiveVGPP = 0;
        mActiveGGPP = 0;
        mActiveTGPP = 0;
        mActiveDGPP = 0;
        mActiveFGPP = 0;
        mActiveCGPP = 0;
        
        mVPMode = false;
        mGPMode = false;
        mFPMode = false;
        mTPMode = false;
        mDPMode = false;
        mCPMode = false;

        if( mVPMode && !mClipPlanes.empty() )
            mClipValid = true;

        //Derived class must set new state
    }
    //-----------------------------------------------------------------------
    void RenderSys::destroyRSO(RenderStateObject * rso)
    {
        N_delete rso;
    }
    //-----------------------------------------------------------------------
    void RenderSys::syncRSO(RenderStateObject *)                   
    {
    }
    //-----------------------------------------------------------------------
    void RenderSys::createCSO(ComputeStateObject *& cso)
    {
        
    }
    //-----------------------------------------------------------------------
    void RenderSys::destroyCSO(ComputeStateObject * cso)
    {
        N_delete cso;
    }
    //-----------------------------------------------------------------------
    void RenderSys::syncCSO(ComputeStateObject * cso)                  
    {
        
    }
    //-----------------------------------------------------------------------
    void RenderSys::destroy(FrameBufferObject * fbo)
    {
        FrameBufferObjectList::iterator itor = mFBOList.find(fbo);
        N_assert( itor != mFBOList.end(), "Already destroyed?" );
        if( itor != mFBOList.end() )
            mFBOList.erase( itor );
        N_delete fbo;
    }
    //---------------------------------------------------------------------
    void RenderSys::beginFBO(FrameBufferObject * fbo, const ViewportList & vplist)
    {
        mActiveFrame = fbo;
        mViewportCount = vplist.size();
        mActiveViewports = vplist;
    }
    //---------------------------------------------------------------------
    void RenderSys::executeFBO()
    {
    }
    //---------------------------------------------------------------------
    void RenderSys::endFBO()
    {
        mActiveFrame = 0;
        const size_t maxBoundViewports = mViewportCount;
        for( size_t i = 0; i < maxBoundViewports; ++i )
            mActiveViewports[i].setArea(0, Vector4::ZERO, Vector4::ZERO, 0u);
        mViewportCount = 1u;

        //Where graphics ends, compute may start, or a new frame.
        //Very likely we'll have to flush the UAVs again, so assume we need.
        mGpuBufValid = false;
    }
    //-----------------------------------------------------------------------
    SampleObject RenderSys::getSampleObject(Nmark extmark)
    {
        const FrameBufferObject * fbo = getFBO();

        SampleObject so;

        //Needed so that memcmp in SampleObject::operator == works correctly
        memset( &so, 0, sizeof(SampleObject) );

        so.mStencil = getStencil();

        for(int i = 0; i < NII_MAX_RenderTarget; ++i)
        {
            if( fbo->mColour[i].mTexture )
            {
                so.mColour[i] = fbo->mColour[i].mTexture->getFormat();
                so.mSampleType = fbo->mColour[i].mTexture->getOriginSample();
            }
            else
                so.mColour[i] = PFG_NULL;
        }

        so.mDepth = PFG_NULL;
        if( fbo->mDepth.mTexture )
        {
            so.mDepth = fbo->mDepth.mTexture->getFormat();
            so.mSampleType = fbo->mDepth.mTexture->getOriginSample();
        }
        else
        {
            so.mMark |= SB_DepthCheckDisable;
        }

        so.mDevID = 1; //TODO: Ask RenderSys current adapter ID.

        //if( smag->getCurrentPrePassMode() == PrePassUse )
            so.mMark |= extmark;

        const bool invertVertexWinding = getInvertVertexWinding();

        if( (fbo->isFlipping() && !invertVertexWinding) ||
            (!fbo->isFlipping() && invertVertexWinding) )
        {
            so.mMark |= SB_InvertVertexWinding;
        }

        return so;
    }
    //-----------------------------------------------------------------------
    const GpuParamBuffer * RenderSys::createBuffer(GpuParamBuffer *& gpb)
    {
        gpb.checkValidity();

        const GpuParamBuffer * re = getDescriptorSet(mGpuParamBufferList, gpb, createBuffer);
        return re;
    }
    //-----------------------------------------------------------------------
    void RenderSys::destroyBuffer(const GpuParamBuffer * gpb)
    {
        destroyDescriptorSet(mGpuParamBufferList, gpb, destroyBuffer);
    }
    //-----------------------------------------------------------------------
    void RenderSys::syncBuffer(GpuParamBuffer * gpb)   
    {
        
    }
    //-----------------------------------------------------------------------
    void RenderSys::createRSO(RenderStateObject *&)
    {
        
    }
    //---------------------------------------------------------------------
    Texture * RenderSys::createDepthImpl(Texture * colour, bool framebuf, PixelFormat pf)
    {
        uint32 textureFlags = Texture::MM_FRAME;

        //if(framebuf )
        //    textureFlags |= Texture::MM_FRAME;

        //char tmpBuffer[64];
        //LwString depthBufferName( LwString::FromEmptyPointer( tmpBuffer, sizeof(tmpBuffer) ) );
        //depthBufferName.a( "DepthBuffer_", Id::generateNewId<Texture>() );

        /*Texture * re = mTextureManager->createTexture( depthBufferName.c_str(),
            Texture::RT_Disk, textureFlags, Texture::Type2D );*/
            
        Texture * re = mTextureManager->createTexture(0, Texture::RT_Disk, textureFlags, Texture::Type2D);

        re->setResolution(colour->getWidth(), colour->getHeight());
        re->setFormat(pf);
        re->setSample(colour->getSample());

        re->load(StorageType::ST_Gpu, (Nui8*)0);

        return re;
    }
    //---------------------------------------------------------------------
    Texture * RenderSys::createDepthBuffer(Texture * colour, GroupID poolId, bool texture, PixelFormat pf)
    {
        if(poolId == GroupUnknow || pf == PFG_NULL)
            return 0; //FrameObj explicitly requested no depth buffer

        if(colour->isWindow())
        {
            ViewWindow * window;
            colour->getPlatformData("Window", &window);
            return window->getDepth();
        }

        if(poolId == GroupSeparate)
        {
            Texture * re = createDepthImpl(colour, texture, pf);
            return re;
        }

        //Find a depth buffer in the pool
        TextureList::const_iterator itor = mDepthBufferList[poolId].begin();
        TextureList::const_iterator end  = mDepthBufferList[poolId].end();

        Texture *re = 0;

        while( itor != end && !re )
        {
            if( texture == (*itor)->isTexture() &&
                (pf == PF_UNKNOWN || pf == (*itor)->getFormat()) &&
                (*itor)->isDepthStencilSupport( colour ) )
            {
                re = *itor;
            }
            else
            {
                re = 0;
            }
            ++itor;
        }

        //Not found yet? Create a new one!
        if( !re )
        {
            re = createDepthImpl(colour, texture, pf);
            mDepthBufferList[poolId].push_back( re );

            if( !re )
            {
                N_fatal( "WARNING: Couldn't create a suited ExtFrameBuffer for RTT: " + colour->getName());
            }
        }

        return re;
    }
    //---------------------------------------------------------------------
    void RenderSys::_setBuffer(Nidx slotStart, const GpuParamBuffer * gpb)
    {
        if(mGpuParamBuffer != gpb)
        {
            mGpuParamBuffer = gpb;
            mGpuBufValid = false;
        }
        if(slotStart != std::numeric_limits<Nui8>::max())
        {
            mUavStartingSlot = slotStart;
            mGpuBufValid = false;
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::createStorageTask(StorageTask *& st)
    {
        
    }
    //-----------------------------------------------------------------------
    void RenderSys::destroyStorageTask(StorageTask * st)
    {
        
    }
    //-----------------------------------------------------------------------
    void RenderSys::syncStorageTask(StorageTask * st)
    {
        
    }
    //-----------------------------------------------------------------------
    void RenderSys::_execute(StorageTask * st)
    {
        
    }
    //-----------------------------------------------------------------------
    void RenderSys::_beginFrameOnce()
    {
        mBufferManager->_beginFrame();
    }
    //-----------------------------------------------------------------------
    void RenderSys::_endFrameOnce()
    {
        _setBuffer(0, 0);
    }
    //-----------------------------------------------------------------------
    SampleType RenderSys::checkSample(const SampleType & st, PixelFormat pf)
    {
        SampleType re(st.getMaxSampling(), st.getType());
        return re;
    }
    //-----------------------------------------------------------------------
    void RenderSys::_addRecord(const DrawRecord & a)
    {
        if(mMetrics.mEnable)
        {
            mMetrics.mBatchCount += a.mBatchCount;
            mMetrics.mTriangleCount += a.mTriangleCount;
            mMetrics.mVertexCount += a.mVertexCount;
            mMetrics.mDrawCount += a.mDrawCount;
            mMetrics.mInstanceCount += a.mInstanceCount;
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::getSysProj(const Matrix4 & in, Matrix4 & out, bool)
    {
        out = in;

        if(mReverseDepth)
        {
            // Convert depth range from [-1,+1] to [1,0]
            out[2][0] = (-out[2][0] + out[3][0]) / 2;
            out[2][1] = (-out[2][1] + out[3][1]) / 2;
            out[2][2] = (-out[2][2] + out[3][2]) / 2;
            out[2][3] = (-out[2][3] + out[3][3]) / 2;
        }
        else
        {
            // Convert depth range from [-1,+1] to [0,1]
            out[2][0] = (out[2][0] + out[3][0]) / 2;
            out[2][1] = (out[2][1] + out[3][1]) / 2;
            out[2][2] = (out[2][2] + out[3][2]) / 2;
            out[2][3] = (out[2][3] + out[3][3]) / 2;
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::getSysProj(const Matrix4f & matrix, Matrix4f & dest, NIIf nearPlane, NIIf farPlane, Frustum::Type projectionType )
    {
        dest = matrix;

        NIIf inv_d = 1 / (farPlane - nearPlane);
        NIIf q, qn;

        if( mReverseDepth )
        {
            if( projectionType == PT_PERSPECTIVE )
            {
                if( farPlane == 0 )
                {
                    // Infinite far plane
                    //  q   = limit( near / (far - near), far, inf );
                    //  qn  = limit( (far * near) / (far - near), far, inf );
                    q   = 0;
                    qn  = nearPlane;
                }
                else
                {
                    //Standard Z for range [-1; 1]
                    //  q = - (far + near) / (far - near)
                    //  qn = - 2 * (far * near) / (far - near)
                    //
                    //Standard Z for range [0; 1]
                    //  q = - far / (far - near)
                    //  qn = - (far * near) / (far - near)
                    //
                    //Reverse Z for range [1; 0]:
                    // [ 1   0    0  0  ]   [ A   0   C   0  ]
                    // [ 0   1    0  0  ] X [ 0   B   D   0  ]
                    // [ 0   0   -1  1  ]   [ 0   0   q   qn ]
                    // [ 0   0    0  1  ]   [ 0   0   -1  0  ]
                    //
                    // [ A   0   C      0  ]
                    // [ 0   B   D      0  ]
                    // [ 0   0   -q-1  -qn ]
                    // [ 0   0   -1     0  ]
                    //
                    //  q' = -q - 1
                    //     =  far / (far - near) - 1
                    //     = ( far - (far - near) ) / (far - near)
                    //  q' = near / (far - near)
                    //  qn'= -qn
                    q   = nearPlane * inv_d;
                    qn  = (farPlane * nearPlane) * inv_d;
                }
            }
            else
            {
                if( farPlane == 0 )
                {
                    // Can not do infinite far plane here, avoid divided zero only
                    q = Frustum::INFINITE_FAR_PLANE_ADJUST / nearPlane;
                    qn = Frustum::INFINITE_FAR_PLANE_ADJUST + 1;
                }
                else
                {
                    //Standard Z for range [-1; 1]
                    //  q = - 2 / (far - near)
                    //  qn = -(far + near) / (far - near)
                    //
                    //Standard Z for range [0; 1]
                    //  q = - 1 / (far - near)
                    //  qn = - near / (far - near)
                    //
                    //Reverse Z for range [1; 0]:
                    //  q' = 1 / (far - near)
                    //  qn'= far / (far - near)
                    q   = inv_d;
                    qn  = farPlane * inv_d;
                }
            }
        }
        else
        {
            if( projectionType == PT_PERSPECTIVE )
            {
                if( farPlane == 0 )
                {
                    // Infinite far plane
                    q   = Frustum::INFINITE_FAR_PLANE_ADJUST - 1;
                    qn  = nearPlane * (Frustum::INFINITE_FAR_PLANE_ADJUST - 1);
                }
                else
                {
                    q   = -farPlane * inv_d;
                    qn  = -(farPlane * nearPlane) * inv_d;
                }
            }
            else
            {
                if( farPlane == 0 )
                {
                    // Can not do infinite far plane here, avoid divided zero only
                    q   = -Frustum::INFINITE_FAR_PLANE_ADJUST / nearPlane;
                    qn  = -Frustum::INFINITE_FAR_PLANE_ADJUST;
                }
                else
                {
                    q   = -inv_d;
                    qn  = -nearPlane * inv_d;
                }
            }
        }

        dest[2][2] = q;
        dest[2][3] = qn;
    }
    //-----------------------------------------------------------------------
    void RenderSys::getOpenVrProj(const Matrix4 & in, Matrix4 & out)
    {
        out = in;

        if(mReverseDepth)
        {
            // Convert depth range from [0,1] to [1,0]
            out[2][0] = (-out[2][0] + out[3][0]);
            out[2][1] = (-out[2][1] + out[3][1]);
            out[2][2] = (-out[2][2] + out[3][2]);
            out[2][3] = (-out[2][3] + out[3][3]);
        }
    }
    //-----------------------------------------------------------------------
    void RenderSys::addListener(Listener * l)
    {
        mEventListenerList.push_back(l);
    }
    //-----------------------------------------------------------------------
    void RenderSys::removeListener(Listener * l)
    {
        mEventListenerList.remove(l);
    }
    //-----------------------------------------------------------------------
    void RenderSys::signal(EventID eid, const PropertyData * params)
    {
        ListenerList::iterator i, iend = mEventListenerList.end()
        for(i = mEventListenerList.begin(); i != iend; ++i)
        {
            (*i)->signal(eid, params);
        }

        if(mListener)
            mListener->signal(eid, params);
    }
    //----------------------------------------------------------------------
    void RenderSys::addRenderPattern(RenderPattern * rp)
    {
        RenderPatternType type = rp->getType();

        RenderPatternArrayList::iterator i = mRenderPatternList.find(type);
        if(i != mRenderPatternList.end())
        {
            RenderPatternList & ins = i->second;
            RenderPatternList::iterator j, jend = ins.end();
            for(j = ins.begin(); j != jend; ++j)
            {
                RenderPattern * temp = *j;
                if(temp->getSpace() == rp->getSpace())
                {
                    if(temp->isAutoDestroy())
                    {
                        N_delete temp;
                    }
                    *j = rp;
                    rp->setRender(this);
                    return;
                }
            }
            rp->setRender(this);
            ins.push(rp);
        }
        else
        {
            rp->setRender(this);
            mRenderPatternList.insert(Npair(type, rp)).first->second.push(rp);
        }
    }
    //----------------------------------------------------------------------
    void RenderSys::removeRenderPattern(SpaceManager * smag, RenderPatternType type)
    {
        RenderPatternArrayList::iterator i = mRenderPatternList.find(type);
        if(i != mRenderPatternList.end())
        {
            RenderPatternList::iterator j, jend = i->second.end();
            for(j = i->second.begin(); j != jend; ++j)
            {
                RenderPattern * rp = *j;
                if(rp->getSpace() == smag)
                {
                    if(rp->isAutoDestroy())
                    {
                        N_delete rp;
                    }
                    i->second.erase(i);
                }
            }
        }
    }
    //---------------------------------------------------------------------
    RenderPattern * RenderSys::getRenderPattern(SpaceManager * smag, RenderPatternType type)
    {
        RenderPatternArrayList::iterator i = mRenderPatternList.find(type);
        if(i != mRenderPatternList.end())
        {
            RenderPatternList::iterator j, jend = i->second.end();
            for(j = i->second.begin(); j != jend; ++j)
            {
                RenderPattern * rp = *j;
                if(rp->getSpace() == smag)
                    return rp;
            }
        }

        return 0;
    }
    //---------------------------------------------------------------------
    RenderPattern * RenderSys::getRenderPattern(Nid id)
    {
        RenderPatternArrayList::iterator i, iend = mRenderPatternList.end();
        for(i = mRenderPatternList.begin(); i != iend; ++i)
        {
            RenderPatternList::iterator j, jend = i->second.end();
            for(j = i->second.begin(); j != jend; ++j)
            {
                RenderPattern * rp = *j;
                if(rp->getID() == id)
                {
                    return rp;
                }
            }
        }

        return 0;
    }
    //----------------------------------------------------------------------
    void RenderSys::addComputePattern(ComputePattern * cp)
    {
        ComputePatternType type = cp->getType();

        ComputePatternList::iterator i = mComputePatternList.find(type);
        if(i != mComputePatternList.end())
        {
            ComputePattern * temp = i->second;
            if(temp->isAutoDestroy())
                N_delete temp;
            
            i->second = cp;
            cp->setRender(this);
        }
        else
        {
            cp->setRender(this);
            mComputePatternList.insert(Npair(type, cp));
        }
    }
    //----------------------------------------------------------------------
    void RenderSys::removeComputePattern()
    {
        ComputePatternList::iterator i = mComputePatternList.find(type);
        if(i != mComputePatternList.end())
        {
            if(cp->isAutoDestroy())
                N_delete cp;
            mComputePatternList.erase(i);
        }
    }
    //----------------------------------------------------------------------
    ComputePattern * RenderSys::getComputePattern(ComputePatternType type) const
    {
        ComputePatternList::iterator i = mComputePatternList.find(type);
        if(i != mComputePatternList.end())
        {
            return i->second;
        }

        return 0;
    }
    //----------------------------------------------------------------------
    ComputePattern * RenderSys::getComputePattern(Nid id) const
    {
        ComputePatternList::iterator i, iend = mComputePatternList.end();
        for(i = mComputePatternList.begin(); i != iend; ++i)
        {
            ComputePattern * cp = mComputePatternList[i];
            if(cp->getID() == id)
            {
                return cp;
            }
        }

        return 0;
    }
    //----------------------------------------------------------------------
    void RenderSys::addMaterial(ShaderChMaterial * mat)
    {
        Nid id = mat->getID();
        MaterialList::iterator i = mMaterialList.find(id);
        if(i != mMaterialList.end())
        {
            i->second = mat;
        }
        else
        {
            mMaterialList.insert(Npair(id, mat));
        }
    }
    //----------------------------------------------------------------------
    void RenderSys::removeMaterial(Nid name)
    {
        MaterialList::iterator itor = mMaterialList.find(name);
        assert(itor != mMaterialList.end());

        if(itor != mMaterialList.end())
        {
            mMaterialList.erase(itor);
        }
    }
    //---------------------------------------------------------------------
    void RenderSys::_update()
    {
        //OgreProfile( "RenderSys::_update" );
        mTextureManager->_update(false);
        mBufferManager->_update();
    }
    //---------------------------------------------------------------------
    void RenderSys::_clear()
    {
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // Listener
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    RenderSys::Listener::~Listener() {}
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // DrawRecord
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    RenderSys::DrawRecord::DrawRecord() :
        mEnable(false),
        mBatchCount(0),
        mTriangleCount(0),
        mVertexCount(0),
        mDrawCount(0),
        mInstanceCount(0)
    {
    }
    //---------------------------------------------------------------------
}