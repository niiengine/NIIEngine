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
#include "OgreD3D11RenderSystem.h"
#include "OgreD3D11Prerequisites.h"
#include "OgreD3D11DriverList.h"
#include "OgreD3D11Driver.h"
#include "OgreD3D11VideoModeList.h"
#include "OgreD3D11VideoMode.h"
#include "OgreD3D11RenderWindow.h"
#include "OgreD3D11TextureManager.h"
#include "OgreD3D11Texture.h"
#include "OgreViewport.h"
#include "OgreLogManager.h"
#include "OgreMeshManager.h"
#include "OgreSceneManagerEnumerator.h"
#include "OgreD3D11HardwareBufferManager.h"
#include "OgreD3D11HardwareIndexBuffer.h"
#include "OgreD3D11HardwareVertexBuffer.h"
#include "OgreD3D11VertexDeclaration.h"
#include "OgreGpuProgramManager.h"
#include "OgreD3D11HLSLProgramFactory.h"

#include "OgreD3D11HardwareOcclusionQuery.h"
#include "OgreFrustum.h"
#include "OgreD3D11MultiRenderTarget.h"
#include "OgreD3D11HLSLProgram.h"

#include "OgreD3D11DepthBuffer.h"
#include "OgreD3D11HardwarePixelBuffer.h"
#include "OgreD3D11RenderTarget.h"
#include "OgreException.h"

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
#include "OgreD3D11StereoDriverBridge.h"
#endif


#ifndef D3D_FL9_3_SIMULTANEOUS_RENDER_TARGET_COUNT
#   define D3D_FL9_3_SIMULTANEOUS_RENDER_TARGET_COUNT 4
#endif

#ifndef D3D_FL9_1_SIMULTANEOUS_RENDER_TARGET_COUNT
#   define D3D_FL9_1_SIMULTANEOUS_RENDER_TARGET_COUNT 1
#endif
//---------------------------------------------------------------------
#include <d3d10.h>
#include "OgreNsightChecker.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT &&  defined(_WIN32_WINNT_WINBLUE) && _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
#include <dxgi1_3.h> // for IDXGIDevice3::Trim
#endif

namespace Ogre 
{
    HRESULT WINAPI D3D11CreateDeviceN(
        _In_opt_ IDXGIAdapter* pAdapter,
        D3D_DRIVER_TYPE DriverType,
        HMODULE Software,
        UINT Flags,
        const D3D_FEATURE_LEVEL* pFeatureLevels,
        UINT FeatureLevels,
        UINT SDKVersion,
        _Out_ ID3D11DeviceN** ppDevice,
        _Out_ D3D_FEATURE_LEVEL* pFeatureLevel,
        _Out_ ID3D11DeviceContextN** ppImmediateContext )
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        return D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);

#elif OGRE_PLATFORM == OGRE_PLATFORM_WINRT
        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;
        ComPtr<ID3D11DeviceN> deviceN;
        ComPtr<ID3D11DeviceContextN> contextN;
        D3D_FEATURE_LEVEL featureLevel;
        HRESULT mainHr, hr;

        mainHr = hr = D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion,
                                        (ppDevice ? device.GetAddressOf() : NULL), &featureLevel, (ppImmediateContext ? context.GetAddressOf() : NULL));
        if(FAILED(hr)) return hr;

        hr = device ? device.As(&deviceN) : S_OK;
        if(FAILED(hr)) return hr;

        hr = context ? context.As(&contextN) : S_OK;
        if(FAILED(hr)) return hr;

        if(ppDevice)            *ppDevice = deviceN.Detach();
        if(pFeatureLevel)       *pFeatureLevel = featureLevel;
        if(ppImmediateContext)  *ppImmediateContext = contextN.Detach();

        return mainHr;
#endif
    }

    //---------------------------------------------------------------------
    D3D11RenderSystem::D3D11RenderSystem()
		: mDevice()
#if OGRE_NO_QUAD_BUFFER_STEREO == 0
		, mStereoDriver(NULL)
#endif	
#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
		, suspendingToken()
		, surfaceContentLostToken()
#endif
    {
        LogManager::getSingleton().logMessage( "D3D11: " + getName() + " created." );

        mRenderSystemWasInited = false;
        mSwitchingFullscreenCounter = 0;
        mDriverType = D3D_DRIVER_TYPE_HARDWARE;

        initRenderSystem();

        // set config options defaults
        initConfigOptions();

        // Clear class instance storage
        memset(mClassInstances, 0, sizeof(mClassInstances));
        memset(mNumClassInstances, 0, sizeof(mNumClassInstances));

        mEventNames.push_back("DeviceLost");
        mEventNames.push_back("DeviceRestored");

#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
#if defined(_WIN32_WINNT_WINBLUE) && _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
		suspendingToken = (Windows::ApplicationModel::Core::CoreApplication::Suspending +=
			ref new Windows::Foundation::EventHandler<Windows::ApplicationModel::SuspendingEventArgs^>([this](Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e)
		{
			// Hints to the driver that the app is entering an idle state and that its memory can be used temporarily for other apps.
			ComPtr<IDXGIDevice3> pDXGIDevice;
			if(mDevice.get() && SUCCEEDED(mDevice->QueryInterface(pDXGIDevice.GetAddressOf())))
				pDXGIDevice->Trim();
		}));

		surfaceContentLostToken = (Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated +=
			ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation^, Platform::Object^>(
				[this](Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ arg)
		{
			LogManager::getSingleton().logMessage("D3D11: DisplayContentsInvalidated.");
			checkDevice(true);
		}));
#else // Win 8.0
		surfaceContentLostToken = (Windows::Graphics::Display::DisplayProperties::DisplayContentsInvalidated +=
			ref new Windows::Graphics::Display::DisplayPropertiesEventHandler([this](Platform::Object ^sender)
		{
			LogManager::getSingleton().logMessage("D3D11: DisplayContentsInvalidated.");
			checkDevice(true);
		}));
#endif
#endif
    }
    //---------------------------------------------------------------------
    D3D11RenderSystem::~D3D11RenderSystem()
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
#if defined(_WIN32_WINNT_WINBLUE) && _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
		Windows::ApplicationModel::Core::CoreApplication::Suspending -= suspendingToken;
		Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated -= surfaceContentLostToken;
#else // Win 8.0
		Windows::Graphics::Display::DisplayProperties::DisplayContentsInvalidated -= surfaceContentLostToken;
#endif
#endif

        shutdown();

        // Deleting the HLSL program factory
        if (mHLSLProgramFactory)
        {
            // Remove from manager safely
            if (HighLevelGpuProgramManager::getSingletonPtr())
                HighLevelGpuProgramManager::getSingleton().removeFactory(mHLSLProgramFactory);
            delete mHLSLProgramFactory;
            mHLSLProgramFactory = 0;
        }

        LogManager::getSingleton().logMessage( "D3D11: " + getName() + " destroyed." );
    }
    //---------------------------------------------------------------------
    const String& D3D11RenderSystem::getName() const
    {
        static String strName( "Direct3D11 Rendering Subsystem");
        return strName;
    }

	//---------------------------------------------------------------------
    D3D11DriverList* D3D11RenderSystem::getDirect3DDrivers(bool refreshList /* = false*/)
    {
        if(!mDriverList)
            mDriverList = new D3D11DriverList();

        if(refreshList || mDriverList->count() == 0)
            mDriverList->refresh();

        return mDriverList;
    }
    //---------------------------------------------------------------------
	ID3D11DeviceN* D3D11RenderSystem::createD3D11Device(D3D11Driver* d3dDriver, D3D_DRIVER_TYPE driverType,
		D3D_FEATURE_LEVEL minFL, D3D_FEATURE_LEVEL maxFL, D3D_FEATURE_LEVEL* pFeatureLevel)
	{
		IDXGIAdapterN* pAdapter = (d3dDriver && driverType == D3D_DRIVER_TYPE_HARDWARE) ? d3dDriver->getDeviceAdapter() : NULL;

		N_assert(driverType == D3D_DRIVER_TYPE_HARDWARE || driverType == D3D_DRIVER_TYPE_SOFTWARE || driverType == D3D_DRIVER_TYPE_WARP);
		if(d3dDriver != NULL)
		{
			if(0 == wcscmp(d3dDriver->getAdapterIdentifier().Description, L"NVIDIA PerfHUD"))
				driverType = D3D_DRIVER_TYPE_REFERENCE;
			else
				driverType = D3D_DRIVER_TYPE_UNKNOWN;
		}

		// determine deviceFlags
		UINT deviceFlags = 0;
#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
		// This flag is required in order to enable compatibility with Direct2D.
		deviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif
		if(OGRE_DEBUG_MODE && !IsWorkingUnderNsight() && D3D11Device::D3D_NO_EXCEPTION != D3D11Device::getExceptionsErrorLevel())
		{
			deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}
		if(!OGRE_THREAD_SUPPORT)
		{
			deviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
		}

		// determine feature levels
		D3D_FEATURE_LEVEL requestedLevels[] = {
#if !__OGRE_WINRT_PHONE // Windows Phone support only FL 9.3, but simulator can create much more capable device, so restrict it artificially here
#if defined(_WIN32_WINNT_WIN8) && _WIN32_WINNT >= _WIN32_WINNT_WIN8
			D3D_FEATURE_LEVEL_11_1,
#endif
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
#endif // !__OGRE_WINRT_PHONE
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		D3D_FEATURE_LEVEL *pFirstFL = requestedLevels, *pLastFL = pFirstFL + ARRAYSIZE(requestedLevels) - 1;
		for(unsigned int i = 0; i < ARRAYSIZE(requestedLevels); i++)
		{
			if(minFL == requestedLevels[i])
				pLastFL = &requestedLevels[i];
			if(maxFL == requestedLevels[i])
				pFirstFL = &requestedLevels[i];
		}
		if(pLastFL < pFirstFL)
		{
			N_EXCEPT(Exception::ERR_INTERNAL_ERROR,
				"Requested min level feature is bigger the requested max level feature.",
				"D3D11RenderSystem::initialise");
		}

		// create device
		ID3D11DeviceN* device = NULL;
		HRESULT hr = D3D11CreateDeviceN(pAdapter, driverType, NULL, deviceFlags, pFirstFL, pLastFL - pFirstFL + 1, D3D11_SDK_VERSION, &device, pFeatureLevel, 0);

		if(FAILED(hr) && 0 != (deviceFlags & D3D11_CREATE_DEVICE_DEBUG))
		{
			StringStream error;
			error << "Failed to create Direct3D11 device with debug layer (" << hr << ")\nRetrying without debug layer.";
			Ogre::LogManager::getSingleton().logMessage(error.str());

			// create device - second attempt, without debug layer
			deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
			hr = D3D11CreateDeviceN(pAdapter, driverType, NULL, deviceFlags, pFirstFL, pLastFL - pFirstFL + 1, D3D11_SDK_VERSION, &device, pFeatureLevel, 0);
		}
		if(FAILED(hr))
		{
			N_EXCEPT_EX(RenderingAPI, hr, "Failed to create Direct3D11 device", "D3D11RenderSystem::D3D11RenderSystem");
		}
		return device;
	}
    //---------------------------------------------------------------------
    void D3D11RenderSystem::initConfigOptions()
    {
        RenderSystem::initConfigOptions();

        ConfigOption optDevice;
        ConfigOption optVideoMode;
        ConfigOption optVSyncInterval;
		ConfigOption optBackBufferCount;
        ConfigOption optAA;
        ConfigOption optFPUMode;
        ConfigOption optNVPerfHUD;
        ConfigOption optMinFeatureLevels;
        ConfigOption optMaxFeatureLevels;
        ConfigOption optExceptionsErrorLevel;
        ConfigOption optDriverType;

        optDevice.name = "Rendering Device";
        optDevice.currentValue = "(default)";
        optDevice.possibleValues.push_back("(default)");
        D3D11DriverList* driverList = getDirect3DDrivers();
        for( unsigned j=0; j < driverList->count(); j++ )
        {
            D3D11Driver* driver = driverList->item(j);
            optDevice.possibleValues.push_back( driver->DriverDescription() );
        }
        optDevice.immutable = false;

        optVideoMode.name = "Video Mode";
        optVideoMode.currentValue = "800 x 600 @ 32-bit colour";
        optVideoMode.immutable = false;

        optVSyncInterval.name = "VSync Interval";
        optVSyncInterval.immutable = false;
        optVSyncInterval.possibleValues.push_back( "1" );
        optVSyncInterval.possibleValues.push_back( "2" );
        optVSyncInterval.possibleValues.push_back( "3" );
        optVSyncInterval.possibleValues.push_back( "4" );
        optVSyncInterval.currentValue = "1";

		optBackBufferCount.name = "Backbuffer Count";
		optBackBufferCount.immutable = false;
		optBackBufferCount.possibleValues.push_back( "Auto" );
		optBackBufferCount.possibleValues.push_back( "1" );
		optBackBufferCount.possibleValues.push_back( "2" );
		optBackBufferCount.currentValue = "Auto";

        optAA.name = "FSAA";
        optAA.immutable = false;
        optAA.possibleValues.push_back( "None" );
        optAA.currentValue = "None";

        optFPUMode.name = "Floating-point mode";
#if OGRE_DOUBLE_PRECISION
        optFPUMode.currentValue = "Consistent";
#else
        optFPUMode.currentValue = "Fastest";
#endif
        optFPUMode.possibleValues.clear();
        optFPUMode.possibleValues.push_back("Fastest");
        optFPUMode.possibleValues.push_back("Consistent");
        optFPUMode.immutable = false;

        optNVPerfHUD.currentValue = "No";
        optNVPerfHUD.immutable = false;
        optNVPerfHUD.name = "Allow NVPerfHUD";
        optNVPerfHUD.possibleValues.push_back( "Yes" );
        optNVPerfHUD.possibleValues.push_back( "No" );

        // min feature level
        optMinFeatureLevels;
        optMinFeatureLevels.name = "Min Requested Feature Levels";
        optMinFeatureLevels.possibleValues.push_back("9.1");
        optMinFeatureLevels.possibleValues.push_back("9.3");
        optMinFeatureLevels.possibleValues.push_back("10.0");
        optMinFeatureLevels.possibleValues.push_back("10.1");
        optMinFeatureLevels.possibleValues.push_back("11.0");

        optMinFeatureLevels.currentValue = "9.1";
        optMinFeatureLevels.immutable = false;      


        // max feature level
        optMaxFeatureLevels;
        optMaxFeatureLevels.name = "Max Requested Feature Levels";
        optMaxFeatureLevels.possibleValues.push_back("9.1");

#if __OGRE_WINRT_PHONE_80
        optMaxFeatureLevels.possibleValues.push_back("9.2");
        optMaxFeatureLevels.possibleValues.push_back("9.3");
        optMaxFeatureLevels.currentValue = "9.3";
#elif __OGRE_WINRT_PHONE || __OGRE_WINRT_STORE
        optMaxFeatureLevels.possibleValues.push_back("9.3");
        optMaxFeatureLevels.possibleValues.push_back("10.0");
        optMaxFeatureLevels.possibleValues.push_back("10.1");
        optMaxFeatureLevels.possibleValues.push_back("11.0");
        optMaxFeatureLevels.possibleValues.push_back("11.1");
        optMaxFeatureLevels.currentValue = "11.1";
#else
        optMaxFeatureLevels.possibleValues.push_back("9.3");
        optMaxFeatureLevels.possibleValues.push_back("10.0");
        optMaxFeatureLevels.possibleValues.push_back("10.1");
        optMaxFeatureLevels.possibleValues.push_back("11.0");
        optMaxFeatureLevels.currentValue = "11.0";
#endif

        optMaxFeatureLevels.immutable = false;      

        // Exceptions Error Level
        optExceptionsErrorLevel.name = "Information Queue Exceptions Bottom Level";
        optExceptionsErrorLevel.possibleValues.push_back("No information queue exceptions");
        optExceptionsErrorLevel.possibleValues.push_back("Corruption");
        optExceptionsErrorLevel.possibleValues.push_back("Error");
        optExceptionsErrorLevel.possibleValues.push_back("Warning");
        optExceptionsErrorLevel.possibleValues.push_back("Info (exception on any message)");
#if OGRE_DEBUG_MODE
        optExceptionsErrorLevel.currentValue = "Info (exception on any message)";
#else
        optExceptionsErrorLevel.currentValue = "No information queue exceptions";
#endif
        optExceptionsErrorLevel.immutable = false;
        

        // Driver type
        optDriverType.name = "Driver type";
        optDriverType.possibleValues.push_back("Hardware");
        optDriverType.possibleValues.push_back("Software");
        optDriverType.possibleValues.push_back("Warp");
        optDriverType.currentValue = "Hardware";
        optDriverType.immutable = false;

        mOptions[optDevice.name] = optDevice;
        mOptions[optVideoMode.name] = optVideoMode;
        mOptions[optVSyncInterval.name] = optVSyncInterval;
        mOptions[optAA.name] = optAA;
        mOptions[optFPUMode.name] = optFPUMode;
        mOptions[optNVPerfHUD.name] = optNVPerfHUD;
        mOptions[optMinFeatureLevels.name] = optMinFeatureLevels;
        mOptions[optMaxFeatureLevels.name] = optMaxFeatureLevels;
        mOptions[optExceptionsErrorLevel.name] = optExceptionsErrorLevel;
        mOptions[optDriverType.name] = optDriverType;

		mOptions[optBackBufferCount.name] = optBackBufferCount;

        
        refreshD3DSettings();

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::refreshD3DSettings()
    {
        ConfigOption* optVideoMode;
        D3D11VideoMode* videoMode;

        ConfigOptionMap::iterator opt = mOptions.find( "Rendering Device" );
        if( opt != mOptions.end() )
        {
            D3D11Driver *driver = getDirect3DDrivers()->findByName(opt->second.currentValue);
            if (driver)
            {
                opt = mOptions.find( "Video Mode" );
                optVideoMode = &opt->second;
                optVideoMode->possibleValues.clear();
                // get vide modes for this device
                for( unsigned k=0; k < driver->getVideoModeList()->count(); k++ )
                {
                    videoMode = driver->getVideoModeList()->item( k );
                    optVideoMode->possibleValues.push_back( videoMode->getDescription() );
                }

                // Reset video mode to default if previous doesn't avail in new possible values
                StringList::const_iterator itValue =
                    std::find(optVideoMode->possibleValues.begin(),
                              optVideoMode->possibleValues.end(),
                              optVideoMode->currentValue);
                if (itValue == optVideoMode->possibleValues.end())
                {
                    optVideoMode->currentValue = "800 x 600 @ 32-bit colour";
                }

                // Also refresh FSAA options
                refreshFSAAOptions();
            }
        }

    }
    //---------------------------------------------------------------------
    bool D3D11RenderSystem::setConfig( const String &name, const String &value )
    {
        initRenderSystem();

        LogManager::getSingleton().stream()
            << "D3D11: RenderSystem Option: " << name << " = " << value;

        bool viewModeChanged = false;

        // Find option
        ConfigOptionMap::iterator it = mOptions.find( name );

        // Update
        if( it != mOptions.end() )
            it->second.currentValue = value;
        else
        {
            StringStream str;
            str << "Option named '" << name << "' does not exist.";
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, str.str(), "D3D11RenderSystem::setConfig" );
        }

        // Refresh other options if D3DDriver changed
        if( name == "Rendering Device" )
            refreshD3DSettings();

        if( name == "Full Screen" )
        {
            // Video mode is applicable
            it = mOptions.find( "Video Mode" );
            if (it->second.currentValue.empty())
            {
                it->second.currentValue = "800 x 600 @ 32-bit colour";
                viewModeChanged = true;
            }
        }

        if( name == "Min Requested Feature Levels" )
        {
            mMinRequestedFeatureLevel = D3D11Device::parseFeatureLevel(value, D3D_FEATURE_LEVEL_9_1);
        }

        if( name == "Max Requested Feature Levels" )
        {
#if defined(_WIN32_WINNT_WIN8) && _WIN32_WINNT >= _WIN32_WINNT_WIN8
            mMaxRequestedFeatureLevel = D3D11Device::parseFeatureLevel(value, D3D_FEATURE_LEVEL_11_1);
#else
            mMaxRequestedFeatureLevel = D3D11Device::parseFeatureLevel(value, D3D_FEATURE_LEVEL_11_0);
#endif
        }

        if( name == "Allow NVPerfHUD" )
        {
            if (value == "Yes")
                mUseNVPerfHUD = true;
            else
                mUseNVPerfHUD = false;
        }

        if (viewModeChanged || name == "Video Mode")
        {
            refreshFSAAOptions();
        }

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::refreshFSAAOptions(void)
    {

        ConfigOptionMap::iterator it = mOptions.find( "FSAA" );
        ConfigOption* optFSAA = &it->second;
        optFSAA->possibleValues.clear();

        it = mOptions.find("Rendering Device");
        D3D11Driver *driver = getDirect3DDrivers()->findByName(it->second.currentValue);
        if (driver)
        {
            it = mOptions.find("Video Mode");
            ComPtr<ID3D11DeviceN> device;
            device.Attach(createD3D11Device(driver, mDriverType, mMinRequestedFeatureLevel, mMaxRequestedFeatureLevel, NULL));
            D3D11VideoMode* videoMode = driver->getVideoModeList()->item(it->second.currentValue); // Could be NULL if working over RDP/Simulator
            DXGI_FORMAT format = videoMode ? videoMode->getFormat() : DXGI_FORMAT_R8G8B8A8_UNORM;
            UINT numLevels = 0;
            // set maskable levels supported
            for (unsigned int n = 1; n <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; n++)
            {
                HRESULT hr = device->CheckMultisampleQualityLevels(format, n, &numLevels);
                if (SUCCEEDED(hr) && numLevels > 0)
                {
                    optFSAA->possibleValues.push_back(StrConv::conv(n));

                    // 8x could mean 8xCSAA, and we need other designation for 8xMSAA
                    if(n == 8 && SUCCEEDED(device->CheckMultisampleQualityLevels(format, 4, &numLevels)) && numLevels > 8    // 8x CSAA
                    || n == 16 && SUCCEEDED(device->CheckMultisampleQualityLevels(format, 4, &numLevels)) && numLevels > 16  // 16x CSAA
                    || n == 16 && SUCCEEDED(device->CheckMultisampleQualityLevels(format, 8, &numLevels)) && numLevels > 16) // 16xQ CSAA
                    {
                        optFSAA->possibleValues.push_back(StrConv::conv(n) + " [Quality]");
                    }
                }
                else if(n == 16) // there could be case when 16xMSAA is not supported but 16xCSAA and may be 16xQ CSAA are supported
                {
                    bool csaa16x = SUCCEEDED(device->CheckMultisampleQualityLevels(format, 4, &numLevels)) && numLevels > 16;
                    bool csaa16xQ = SUCCEEDED(device->CheckMultisampleQualityLevels(format, 8, &numLevels)) && numLevels > 16;
                    if(csaa16x || csaa16xQ)
                        optFSAA->possibleValues.push_back("16");
                    if(csaa16x && csaa16xQ)
                        optFSAA->possibleValues.push_back("16 [Quality]");
                }
            }
        }

        if(optFSAA->possibleValues.empty())
        {
            optFSAA->possibleValues.push_back("1"); // D3D11 does not distinguish between noMSAA and 1xMSAA
        }

        // Reset FSAA to none if previous doesn't avail in new possible values
        StringList::const_iterator itValue =
            std::find(optFSAA->possibleValues.begin(),
                      optFSAA->possibleValues.end(),
                      optFSAA->currentValue);
        if (itValue == optFSAA->possibleValues.end())
        {
            optFSAA->currentValue = optFSAA->possibleValues[0];
        }

    }
    //---------------------------------------------------------------------
    String D3D11RenderSystem::checkConfig()
    {
        ConfigOptionMap::iterator it;
        
        // check if video mode is selected
        it = mOptions.find( "Video Mode" );
        if (it->second.currentValue.empty())
            return "A video mode must be selected.";

        it = mOptions.find( "Rendering Device" );
        String driverName = it->second.currentValue;
        if(driverName != "(default)" && getDirect3DDrivers()->findByName(driverName)->DriverDescription() != driverName)
        {
            // Just pick default driver
            setConfig("Rendering Device", "(default)");
            return "Requested rendering device could not be found, default would be used instead.";
        }

        return N_StrBlank;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::init()
    {
        // call superclass method
        RenderSystem::init();

        LogManager::getSingleton().logMessage( "D3D11: Subsystem Initialising" );

		if(IsWorkingUnderNsight())
			LogManager::getSingleton().logMessage( "D3D11: Nvidia Nsight found");

        // Init using current settings
        ConfigOptionMap::iterator opt = mOptions.find( "Rendering Device" );
        if( opt == mOptions.end() )
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Can`t find requested Direct3D driver name!", "D3D11RenderSystem::initialise" );
        mDriverName = opt->second.currentValue;

        // Driver type
        opt = mOptions.find( "Driver type" );
        if( opt == mOptions.end() )
            N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Can't find driver type!", "D3D11RenderSystem::initialise" );
        mDriverType = D3D11Device::parseDriverType(opt->second.currentValue);

        opt = mOptions.find( "Information Queue Exceptions Bottom Level" );
        if( opt == mOptions.end() )
            N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Can't find Information Queue Exceptions Bottom Level option!", "D3D11RenderSystem::initialise" );
        D3D11Device::setExceptionsErrorLevel(opt->second.currentValue);

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        // Stereo driver must be created before device is created
        StereoModeType stereoMode = StringConverter::parseStereoMode(mOptions["Stereo Mode"].currentValue);
        D3D11StereoDriverBridge* stereoBridge = N_new D3D11StereoDriverBridge(stereoMode);
#endif

        // create the device for the selected adapter
        createDevice();

        LogManager::getSingleton().logMessage("***************************************");
        LogManager::getSingleton().logMessage("*** D3D11: Subsystem Initialized OK ***");
        LogManager::getSingleton().logMessage("***************************************");

        this->fireDeviceEvent(&mDevice, "DeviceCreated");
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::reinit()
    {
        LogManager::getSingleton().logMessage( "D3D11: Reinitializing" );
        this->shutdown();
    //  this->initialise( true );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::shutdown()
    {
        RenderSystem::shutdown();

        mRenderSystemWasInited = false;

        mPrimaryWindow = NULL; // primary window deleted by base class.
        freeDevice();
        SAFE_DELETE( mDriverList );
        mActiveD3DDriver = D3D11Driver();
        mDevice.ReleaseAll();
        LogManager::getSingleton().logMessage("D3D11: Shutting down cleanly.");
        SAFE_DELETE( mTextureManager );
        SAFE_DELETE( mHardwareBufferManager );

        if(mGpuProgramManager)
        {
            ResourceGroupManager::getSingleton()._unregisterResourceManager(mGpuProgramManager->getResourceType());
            SAFE_DELETE( mGpuProgramManager );
        }
    }
    //---------------------------------------------------------------------
	RenderWindow* D3D11RenderSystem::createWindow(const String &name,
		unsigned int width, unsigned int height, bool fullScreen,
		const PropertyData *miscParams)
	{

		// Check we're not creating a secondary window when the primary
		// was fullscreen
		if (mPrimaryWindow && mPrimaryWindow->isFullScreen() && fullScreen == false)
		{
			N_EXCEPT(Exception::ERR_INVALID_STATE,
				"Cannot create secondary windows not in full screen when the primary is full screen",
				"D3D11RenderSystem::createWindow");
		}

		// Log a message
		StringStream ss;
		ss << "D3D11RenderSystem::createWindow \"" << name << "\", " <<
			width << "x" << height << " ";
		if (fullScreen)
			ss << "fullscreen ";
		else
			ss << "windowed ";
		if (miscParams)
		{
			ss << " miscParams: ";
			PropertyData::const_iterator it;
			for (it = miscParams->begin(); it != miscParams->end(); ++it)
			{
				ss << it->first << "=" << it->second << " ";
			}
			LogManager::getSingleton().logMessage(ss.str());
		}

		String msg;

		// Make sure we don't already have a render target of the 
		// sam name as the one supplied
		if (mRenderTargets.find(name) != mRenderTargets.end())
		{
			msg = "A render target of the same name '" + name + "' already "
				"exists.  You cannot create a new window with this name.";
			N_EXCEPT(Exception::ERR_INTERNAL_ERROR, msg, "D3D11RenderSystem::createWindow");
		}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		D3D11RenderWindowBase* win = new D3D11RenderWindowHwnd(mDevice);
#elif OGRE_PLATFORM == OGRE_PLATFORM_WINRT
		String windowType;
		if(miscParams)
		{
			// Get variable-length params
			PropertyData::const_iterator opt = miscParams->find("windowType");
			if(opt != miscParams->end())
				windowType = opt->second;
		}

		D3D11RenderWindowBase* win = NULL;
#if !__OGRE_WINRT_PHONE_80
		if(win == NULL && windowType == "SurfaceImageSource")
			win = new D3D11RenderWindowImageSource(mDevice);
		if(win == NULL && windowType == "SwapChainPanel")
			win = new D3D11RenderWindowSwapChainPanel(mDevice);
#endif // !__OGRE_WINRT_PHONE_80
		if(win == NULL)
			win = new D3D11RenderWindowCoreWindow(mDevice);
#endif
		win->create(name, width, height, fullScreen, miscParams);

		attachRenderTarget(*win);

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
		// Must be called after device has been linked to window
		D3D11StereoDriverBridge::getSingleton().addRenderWindow(win);
		win->_validateStereo();
#endif

		// If this is the first window, get the D3D device and create the texture manager
		if (!mPrimaryWindow)
		{
			mPrimaryWindow = win;
			win->getPlatformData("D3DDEVICE", &mDevice);

			// Create the texture manager for use by others
			mTextureManager = new D3D11TextureManager(mDevice);
			// Also create hardware buffer manager
			mHardwareBufferManager = new D3D11HardwareBufferManager(mDevice);

			// Create the GPU program manager
	        mGpuProgramManager = new GpuProgramManager();
	        ResourceGroupManager::getSingleton()._registerResourceManager(mGpuProgramManager->getResourceType(),
	                                                                      mGpuProgramManager);
			// create & register HLSL factory
			if (mHLSLProgramFactory == NULL)
				mHLSLProgramFactory = new D3D11HLSLProgramFactory(mDevice);
			mVendorFeature = setupFeature();

			// if we are using custom capabilities, then 
			// mFeature has already been loaded
			if (mSysFeatureValid)
				mFeature = mVendorFeature;

			fireEvent("RenderSystemCapabilitiesCreated");

			initImpl(mFeature, mPrimaryWindow);

		}
		else
		{
			mSecondaryWindows.push_back(win);
		}

		return win;
	}

    //---------------------------------------------------------------------
    void D3D11RenderSystem::fireDeviceEvent(D3D11Device* device, const String & name, D3D11RenderWindowBase* sendingWindow /* = NULL */)
    {
        PropertyData params;
        params["D3DDEVICE"] =  StrConv::conv((size_t)device->get());
        if(sendingWindow)
            params["RenderWindow"] = StrConv::conv((size_t)sendingWindow);
        fireEvent(name, &params);
    }
    //---------------------------------------------------------------------
    RenderFeature* D3D11RenderSystem::setupFeature() const
    {
        RenderFeature* rsc = new RenderFeature();
        rsc->setDriverVersion(mVersion);
        rsc->setDeviceName(mActiveD3DDriver.DriverDescription());
        rsc->setRenderSystemName(getName());

		rsc->setCapability(RSC_ADVANCED_BLEND_OPERATIONS);
		
        // Does NOT support fixed-function!
        //rsc->setCapability(RSC_FIXED_FUNCTION);

        rsc->setCapability(RSC_HWSTENCIL);
        rsc->setStencilBufferBitDepth(8);

        UINT formatSupport;
        if(mFeatureLevel >= D3D_FEATURE_LEVEL_9_2
        || SUCCEEDED(mDevice->CheckFormatSupport(DXGI_FORMAT_R32_UINT, &formatSupport)) && 0 != (formatSupport & D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER))
            rsc->setCapability(RSC_32BIT_INDEX);

        // Set number of texture units, cap at NII_MAX_TextureLayer
        rsc->setMaxTexUnit(NII_MAX_TextureLayer);
        rsc->setNumVertexAttributes(D3D11_STANDARD_VERTEX_ELEMENT_COUNT);
        rsc->setCapability(RSC_ANISOTROPY);
        rsc->setCapability(RSC_DOT3);
        // Cube map
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->setCapability(RSC_READ_BACK_AS_TEXTURE);
        }

        // We always support compression, D3DX will decompress if device does not support
        rsc->setCapability(RSC_TEXTURE_COMPRESSION);
        rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);
        rsc->setCapability(RSC_SCISSOR_TEST);

		if(mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
			rsc->setCapability(RSC_TWO_SIDED_STENCIL);

        rsc->setCapability(RSC_STENCIL_WRAP);
        rsc->setCapability(RSC_HWOCCLUSION);
        rsc->setCapability(RSC_HWOCCLUSION_ASYNCHRONOUS);

        convertVertexShaderCaps(rsc);
        convertPixelShaderCaps(rsc);
        convertGeometryShaderCaps(rsc);
        convertHullShaderCaps(rsc);
        convertDomainShaderCaps(rsc);
        convertComputeShaderCaps(rsc);
        rsc->addShaderProfile("hlsl");

        // Check support for dynamic linkage
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->setCapability(RSC_SHADER_SUBROUTINE);
        }

        rsc->setCapability(RSC_USER_CLIP_PLANES);
        rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);

        rsc->setCapability(RSC_RTT_MAIN_DEPTHBUFFER_ATTACHABLE);


        // Adapter details
        const DXGI_ADAPTER_DESC1& adapterID = mActiveD3DDriver.getAdapterIdentifier();

        switch(mDriverType) {
        case D3D_DRIVER_TYPE_HARDWARE:
            // determine vendor
            // Full list of vendors here: http://www.pcidatabase.com/vendors.php?sort=id
            switch(adapterID.VendorId)
            {
            case 0x10DE:
                rsc->setVendor(GPU_NVIDIA);
                break;
            case 0x1002:
                rsc->setVendor(GPU_AMD);
                break;
            case 0x163C:
            case 0x8086:
                rsc->setVendor(GPU_INTEL);
                break;
            default:
                rsc->setVendor(GPU_UNKNOWN);
                break;
            };
            break;
        case D3D_DRIVER_TYPE_SOFTWARE:
            rsc->setVendor(GPU_MS_SOFTWARE);
            break;
        case D3D_DRIVER_TYPE_WARP:
            rsc->setVendor(GPU_MS_WARP);
            break;
        default:
            rsc->setVendor(GPU_UNKNOWN);
            break;
        }

        rsc->setCapability(RSC_INFINITE_FAR_PLANE);

        rsc->setCapability(RSC_TEXTURE_3D);
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
            rsc->setCapability(RSC_HWRENDER_TO_TEXTURE_3D);
            rsc->setCapability(RSC_TEXTURE_1D);
            rsc->setCapability(RSC_TEXTURE_COMPRESSION_BC6H_BC7);
        }

        rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
        rsc->setCapability(RSC_TEXTURE_FLOAT);

#ifdef D3D_FEATURE_LEVEL_9_3
        int numMultiRenderTargets = (mFeatureLevel > D3D_FEATURE_LEVEL_9_3) ? D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT :      // 8
                                    (mFeatureLevel == D3D_FEATURE_LEVEL_9_3) ? 4/*D3D_FL9_3_SIMULTANEOUS_RENDER_TARGET_COUNT*/ :    // 4
                                    1/*D3D_FL9_1_SIMULTANEOUS_RENDER_TARGET_COUNT*/;                                                // 1
#else
        int numMultiRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;     // 8
#endif

        rsc->setNumMultiRenderTargets(std::min(numMultiRenderTargets, (int)NII_MAX_RenderTarget));
        rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);

        rsc->setCapability(RSC_POINT_SPRITES);
        rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS);
        rsc->setMaxPointSize(256); // TODO: guess!
    
        rsc->setCapability(GF_Program_VertexTexFetch);
        rsc->setNumVertexTextureUnits(4);
        rsc->setVertexTextureUnitsShared(true);

        rsc->setCapability(RSC_MIPMAP_LOD_BIAS);

        // actually irrelevant, but set
        rsc->setCapability(RSC_PERSTAGECONSTANT);

        rsc->setCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA);
        rsc->setCapability(RSC_CAN_GET_COMPILED_SHADER_BUFFER);

        rsc->setCapability(RSC_PRIMITIVE_RESTART);

        return rsc;

    }
    //-----------------------------------------------------------------------
    void D3D11RenderSystem::initImpl(
        RenderFeature* caps, RenderTarget* primary)
    {
        if(caps->getRenderSystemName() != getName())
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, 
                "Trying to initialize D3D11RenderSystem from RenderFeature that do not support Direct3D11",
                "D3D11RenderSystem::initImpl");
        }
        
        // add hlsl
        HighLevelGpuProgramManager::getSingleton().addFactory(mHLSLProgramFactory);

        Log* defaultLog = LogManager::getSingleton().getDefaultLog();
        if (defaultLog)
        {
            caps->log(defaultLog);
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertVertexShaderCaps(RenderFeature* rsc) const
    {
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_9_1)
        {
            rsc->addShaderProfile("vs_4_0_level_9_1");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("vs_2_0");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_9_3)
        {
            rsc->addShaderProfile("vs_4_0_level_9_3");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("vs_2_a");
            rsc->addShaderProfile("vs_2_x");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->addShaderProfile("vs_4_0");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("vs_3_0");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
        {
            rsc->addShaderProfile("vs_4_1");
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("vs_5_0");
        }

        rsc->setCapability(RSC_VERTEX_PROGRAM);

        // TODO: constant buffers have no limits but lower models do
        // 16 boolean params allowed
        rsc->setVertexProgramConstantBoolCount(16);
        // 16 integer params allowed, 4D
        rsc->setVertexProgramConstantIntCount(16);
        // float params, always 4D
        rsc->setVertexProgramConstantFloatCount(512);

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertPixelShaderCaps(RenderFeature* rsc) const
    {
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_9_1)
        {
            rsc->addShaderProfile("ps_4_0_level_9_1");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("ps_2_0");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_9_3)
        {
            rsc->addShaderProfile("ps_4_0_level_9_3");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("ps_2_a");
            rsc->addShaderProfile("ps_2_b");
            rsc->addShaderProfile("ps_2_x");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->addShaderProfile("ps_4_0");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("ps_3_0");
            rsc->addShaderProfile("ps_3_x");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
        {
            rsc->addShaderProfile("ps_4_1");
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("ps_5_0");
        }


        rsc->setCapability(RSC_FRAGMENT_PROGRAM);


        // TODO: constant buffers have no limits but lower models do
        // 16 boolean params allowed
        rsc->setFragmentProgramConstantBoolCount(16);
        // 16 integer params allowed, 4D
        rsc->setFragmentProgramConstantIntCount(16);
        // float params, always 4D
        rsc->setFragmentProgramConstantFloatCount(512);

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertHullShaderCaps(RenderFeature* rsc) const
    {
        // Only for shader model 5.0
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("hs_5_0");
            
            rsc->setCapability(RSC_TESSELLATION_HULL_PROGRAM);

            // TODO: constant buffers have no limits but lower models do
            // 16 boolean params allowed
            rsc->setTessellationHullProgramConstantBoolCount(16);
            // 16 integer params allowed, 4D
            rsc->setTessellationHullProgramConstantIntCount(16);
            // float params, always 4D
            rsc->setTessellationHullProgramConstantFloatCount(512);
        }

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertDomainShaderCaps(RenderFeature* rsc) const
    {
        // Only for shader model 5.0
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("ds_5_0");

            rsc->setCapability(RSC_TESSELLATION_DOMAIN_PROGRAM);


            // TODO: constant buffers have no limits but lower models do
            // 16 boolean params allowed
            rsc->setTessellationDomainProgramConstantBoolCount(16);
            // 16 integer params allowed, 4D
            rsc->setTessellationDomainProgramConstantIntCount(16);
            // float params, always 4D
            rsc->setTessellationDomainProgramConstantFloatCount(512);
        }

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertComputeShaderCaps(RenderFeature* rsc) const
    {

        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->addShaderProfile("cs_4_0");
            rsc->setCapability(RSC_COMPUTE_PROGRAM);
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
        {
            rsc->addShaderProfile("cs_4_1");
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("cs_5_0");
        }



        // TODO: constant buffers have no limits but lower models do
        // 16 boolean params allowed
        rsc->setComputeProgramConstantBoolCount(16);
        // 16 integer params allowed, 4D
        rsc->setComputeProgramConstantIntCount(16);
        // float params, always 4D
        rsc->setComputeProgramConstantFloatCount(512);

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertGeometryShaderCaps(RenderFeature* rsc) const
    {
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->addShaderProfile("gs_4_0");
            rsc->setCapability(RSC_GEOMETRY_PROGRAM);
            rsc->setCapability(RSC_HWRENDER_TO_VERTEX_BUFFER);
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
        {
            rsc->addShaderProfile("gs_4_1");
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("gs_5_0");
        }

        rsc->setGeometryProgramConstantFloatCount(512);
        rsc->setGeometryProgramConstantIntCount(16);
        rsc->setGeometryProgramConstantBoolCount(16);
        rsc->setGeometryProgramNumOutputVertices(1024);
    }
    //-----------------------------------------------------------------------
    bool D3D11RenderSystem::checkVertexTextureFormats(void)
    {
        return true;
    }
    //-----------------------------------------------------------------------
    MultiRenderTarget * D3D11RenderSystem::createMultiRenderTarget(const String & name)
    {
        MultiRenderTarget *retval;
        retval = new D3D11MultiRenderTarget(name);
        attachRenderTarget(*retval);

        return retval;
    }
    //-----------------------------------------------------------------------
    DepthBuffer* D3D11RenderSystem::_createDepthBufferFor( RenderTarget *renderTarget )
    {
        // Get surface data (mainly to get MSAA data)
        D3D11RenderTarget* d3d11RenderTarget = dynamic_cast<D3D11RenderTarget*>(renderTarget);
        ID3D11Texture2D* d3d11Texture = NULL;
        if (d3d11RenderTarget)
        {
            d3d11Texture = d3d11RenderTarget->getSurface();
        }

        if (!d3d11Texture)
        {
            N_EXCEPT(RenderingAPI, "Invalid render target",
                        "D3D11RenderSystem::_createDepthBufferFor");
        }

        D3D11_TEXTURE2D_DESC BBDesc;
        d3d11Texture->GetDesc(&BBDesc);

        // Create depth stencil texture
        ComPtr<ID3D11Texture2D> pDepthStencil;
        D3D11_TEXTURE2D_DESC descDepth;

        descDepth.Width                 = renderTarget->getWidth();
        descDepth.Height                = renderTarget->getHeight();
        descDepth.MipLevels             = 1;
        descDepth.ArraySize             = BBDesc.ArraySize;

        if ( mFeatureLevel < D3D_FEATURE_LEVEL_10_0)
            descDepth.Format            = DXGI_FORMAT_D24_UNORM_S8_UINT;
        else
            descDepth.Format            = DXGI_FORMAT_R24G8_TYPELESS;

        descDepth.SampleDesc.Count      = BBDesc.SampleDesc.Count;
        descDepth.SampleDesc.Quality    = BBDesc.SampleDesc.Quality;
        descDepth.Usage                 = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags             = D3D11_BIND_DEPTH_STENCIL;

        // If we tell we want to use it as a Shader Resource when in MSAA, we will fail
        // This is a recomandation from NVidia.
        if(!mReadBackAsTexture && mFeatureLevel >= D3D_FEATURE_LEVEL_10_0 && BBDesc.SampleDesc.Count == 1)
            descDepth.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

        descDepth.CPUAccessFlags        = 0;
        descDepth.MiscFlags             = 0;

        if (descDepth.ArraySize == 6)
        {
            descDepth.MiscFlags     |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        }


        HRESULT hr = mDevice->CreateTexture2D( &descDepth, NULL, pDepthStencil.ReleaseAndGetAddressOf() );
        if( FAILED(hr) || mDevice.isError())
        {
            String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr,
                "Unable to create depth texture\nError Description:" + errorDescription,
                "D3D11RenderSystem::_createDepthBufferFor");
        }

        //
        // Create the View of the texture
        // If MSAA is used, we cannot do this
        //
        if(!mReadBackAsTexture && mFeatureLevel >= D3D_FEATURE_LEVEL_10_0 && BBDesc.SampleDesc.Count == 1)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
            viewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            viewDesc.Texture2D.MostDetailedMip = 0;
            viewDesc.Texture2D.MipLevels = 1;
            HRESULT hr = mDevice->CreateShaderResourceView( pDepthStencil.Get(), &viewDesc, mDSTResView.ReleaseAndGetAddressOf());
            if( FAILED(hr) || mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString(hr);
                N_EXCEPT_EX(RenderingAPI, hr,
                    "Unable to create the view of the depth texture \nError Description:" + errorDescription,
                    "D3D11RenderSystem::_createDepthBufferFor");
            }
        }

        // Create the depth stencil view
        ID3D11DepthStencilView      *depthStencilView;
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        ZeroMemory( &descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC) );

        descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDSV.ViewDimension = (BBDesc.SampleDesc.Count > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Flags = 0 /* D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL */;    // TODO: Allows bind depth buffer as depth view AND texture simultaneously.
                                                                                            // TODO: Decide how to expose this feature
        descDSV.Texture2D.MipSlice = 0;
        hr = mDevice->CreateDepthStencilView( pDepthStencil.Get(), &descDSV, &depthStencilView );
        if( FAILED(hr) )
        {
			String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(RenderingAPI, hr,
                "Unable to create depth stencil view\nError Description:" + errorDescription,
                "D3D11RenderSystem::_createDepthBufferFor");
        }

        //Create the abstract container
        D3D11DepthBuffer *newDepthBuffer = new D3D11DepthBuffer( DepthBuffer::GroupInner, this, depthStencilView,
                                                descDepth.Width, descDepth.Height,
                                                descDepth.SampleDesc.Count, descDepth.SampleDesc.Quality,
                                                false );

        return newDepthBuffer;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_removeManualDepthBuffer(DepthBuffer *depthBuffer)
    {
        if(depthBuffer != NULL)
        {
            DepthBufferVec& pool = mDepthBufferPool[depthBuffer->getPoolId()];
            pool.erase(std::remove(pool.begin(), pool.end(), depthBuffer), pool.end());
        }
    }
    //---------------------------------------------------------------------
    DepthBuffer* D3D11RenderSystem::_addManualDepthBuffer( ID3D11DepthStencilView *depthSurface,
                                                            uint32 width, uint32 height,
                                                            uint32 fsaa, uint32 fsaaQuality )
    {
        //If this depth buffer was already added, return that one
        DepthBufferVec::const_iterator itor = mDepthBufferPool[DepthBuffer::GroupInner].begin();
        DepthBufferVec::const_iterator end  = mDepthBufferPool[DepthBuffer::GroupInner].end();

        while( itor != end )
        {
            if( static_cast<D3D11DepthBuffer*>(*itor)->getDepthStencilView() == depthSurface )
                return *itor;

            ++itor;
        }

        //Create a new container for it
        D3D11DepthBuffer *newDepthBuffer = new D3D11DepthBuffer( DepthBuffer::GroupInner, this, depthSurface,
                                                                    width, height, fsaa, fsaaQuality, true );

        //Add the 'main' depth buffer to the pool
        mDepthBufferPool[newDepthBuffer->getPoolId()].push_back( newDepthBuffer );

        return newDepthBuffer;
    }
    //---------------------------------------------------------------------
    RenderTarget* D3D11RenderSystem::detachRenderTarget(const String &name)
    {
        RenderTarget* target = RenderSystem::detachRenderTarget(name);
        detachRenderTargetImpl(name);
        return target;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::detachRenderTargetImpl(const String& name)
    {
        // Check in specialized lists
		if (mPrimaryWindow != NULL && mPrimaryWindow->getName() == name)
        {
            // We're destroying the primary window, so reset device and window
			mPrimaryWindow = NULL;
        }
        else
        {
            // Check secondary windows
            SecondaryWindowList::iterator sw;
            for (sw = mSecondaryWindows.begin(); sw != mSecondaryWindows.end(); ++sw)
            {
                if ((*sw)->getName() == name)
                {
                    mSecondaryWindows.erase(sw);
                    break;
                }
            }
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::destroyRenderTarget(const String& name)
    {
#if OGRE_NO_QUAD_BUFFER_STEREO == 0
		D3D11StereoDriverBridge::getSingleton().removeRenderWindow(name);
#endif

        detachRenderTargetImpl(name);

        // Do the real removal
        RenderSystem::destroyRenderTarget(name);

        // Did we destroy the primary?
        if (!mPrimaryWindow)
        {
            // device is no longer valid, so free it all up
            freeDevice();
        }

    }
    //-----------------------------------------------------------------------
    void D3D11RenderSystem::freeDevice(void)
    {
        if (!mDevice.isNull() && mFeature)
        {
            // Set all texture units to nothing to release texture surfaces
            _disableTextureUnitsFrom(0);
            // Unbind any vertex streams to avoid memory leaks
            /*for (unsigned int i = 0; i < mLastVertexSourceCount; ++i)
            {
                HRESULT hr = mDevice->SetStreamSource(i, NULL, 0, 0);
            }
            */
            // Clean up depth stencil surfaces
            mDevice.ReleaseAll();
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createDevice()
    {
        mDevice.ReleaseAll();

        D3D11Driver* d3dDriver = getDirect3DDrivers(true)->findByName(mDriverName);
        mActiveD3DDriver = *d3dDriver; // store copy of selected driver, so that it is not lost when drivers would be re-enumerated
        LogManager::getSingleton().stream() << "D3D11: Requested \"" << mDriverName << "\", selected \"" << d3dDriver->DriverDescription() << "\"";

        if(D3D11Driver* nvPerfHudDriver = (mDriverType == D3D_DRIVER_TYPE_HARDWARE && mUseNVPerfHUD) ? getDirect3DDrivers()->item("NVIDIA PerfHUD") : NULL)
        {
            d3dDriver = nvPerfHudDriver;
            LogManager::getSingleton().logMessage("D3D11: Actually \"NVIDIA PerfHUD\" is used");
        }

        ID3D11DeviceN * device = createD3D11Device(d3dDriver, mDriverType, mMinRequestedFeatureLevel, mMaxRequestedFeatureLevel, &mFeatureLevel);
        mDevice.TransferOwnership(device);

        LARGE_INTEGER driverVersion = mDevice.GetDriverVersion();
        mVersion.major = HIWORD(driverVersion.HighPart);
        mVersion.minor = LOWORD(driverVersion.HighPart);
        mVersion.release = HIWORD(driverVersion.LowPart);
        mVersion.build = LOWORD(driverVersion.LowPart);
    }
    //-----------------------------------------------------------------------
    void D3D11RenderSystem::handleDeviceLost()
    {
        LogManager::getSingleton().logMessage("D3D11: Device was lost, recreating.");

        // release device depended resources
        fireDeviceEvent(&mDevice, "DeviceLost");

        SceneManagerEnumerator::SceneManagerIterator scnIt = SceneManagerEnumerator::getSingleton().getSceneManagerIterator();
        while(scnIt.hasMoreElements())
            scnIt.getNext()->_releaseManualHardwareResources();

        notifyDeviceLost(&mDevice);

        // Release all automatic temporary buffers and free unused
        // temporary buffers, so we doesn't need to recreate them,
        // and they will reallocate on demand.
        GBufferManager::getSingleton()._releaseBufferCopies(true);

        // Cleanup depth stencils surfaces.
        _cleanupDepthBuffers();

        // recreate device
        createDevice();

        // recreate device depended resources
        notifyDeviceRestored(&mDevice);

        MeshManager::getSingleton().reloadAll(Resource::LF_PRESERVE_STATE);

        scnIt = SceneManagerEnumerator::getSingleton().getSceneManagerIterator();
        while(scnIt.hasMoreElements())
            scnIt.getNext()->_restoreManualHardwareResources();

        // Invalidate active view port.
        mActiveViewport = NULL;

        fireDeviceEvent(&mDevice, "DeviceRestored");

        LogManager::getSingleton().logMessage("D3D11: Device was restored.");
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::checkDevice(bool forceDeviceElection)
    {
        if(mDevice.isNull())
            return;

        // The D3D Device is no longer valid if the elected adapter changes or if
        // the device has been removed.

        bool anotherIsElected = false;
        if(forceDeviceElection)
        {
            // elect new device
            D3D11Driver* newDriver = getDirect3DDrivers(true)->findByName(mDriverName);

            // check by LUID
            LUID newLUID = newDriver->getAdapterIdentifier().AdapterLuid;
            LUID prevLUID = mActiveD3DDriver.getAdapterIdentifier().AdapterLuid;
            anotherIsElected = (newLUID.LowPart != prevLUID.LowPart) || (newLUID.HighPart != prevLUID.HighPart);
        }

        if(anotherIsElected || mDevice.IsDeviceLost())
        {
            handleDeviceLost();
        }
    }
    //-----------------------------------------------------------------------
    void D3D11RenderSystem::_updateAllRenderTargets(bool _swap)
    {
        try
        {
            RenderSystem::_updateAllRenderTargets(_swap);
        }
        catch(const D3D11RenderingAPIException& e)
        {
            if(e.getHResult() == DXGI_ERROR_DEVICE_REMOVED || e.getHResult() == DXGI_ERROR_DEVICE_RESET)
                LogManager::getSingleton().logMessage("D3D11: Device was lost while rendering.");
            else
                throw;
        }
    }
    //-----------------------------------------------------------------------
    void D3D11RenderSystem::_swapAllRenderTargetBuffers()
    {
        try
        {
            RenderSystem::_swapAllRenderTargetBuffers();
        }
        catch(const D3D11RenderingAPIException& e)
        {
            if(e.getHResult() == DXGI_ERROR_DEVICE_REMOVED || e.getHResult() == DXGI_ERROR_DEVICE_RESET)
                LogManager::getSingleton().logMessage("D3D11: Device was lost while rendering.");
            else
                throw;
        }
    }
    //---------------------------------------------------------------------
    DataType D3D11RenderSystem::getColourType(void) const
    {
        return DT_Colour_ABGR;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::getSysProj(const Matrix4& matrix, Matrix4& dest, bool forGpuProgram)
    {
        dest = matrix;

        // Convert depth range from [-1,+1] to [0,1]
        dest[2][0] = (dest[2][0] + dest[3][0]) / 2;
        dest[2][1] = (dest[2][1] + dest[3][1]) / 2;
        dest[2][2] = (dest[2][2] + dest[3][2]) / 2;
        dest[2][3] = (dest[2][3] + dest[3][3]) / 2;

        if (!forGpuProgram)
        {
            // Convert right-handed to left-handed
            dest[0][2] = -dest[0][2];
            dest[1][2] = -dest[1][2];
            dest[2][2] = -dest[2][2];
            dest[3][2] = -dest[3][2];
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_bindTexture( size_t stage, bool enabled, const TexturePtr & tex)
    {
        static D3D11TexturePtr dt;
        dt = static_pointer_cast<D3D11Texture>(tex);
        if (enabled && dt && dt->getSize() > 0)
        {
            // note used
            dt->ref();
            ID3D11ShaderResourceView * pTex = dt->getSrvView();
            mTexStageDesc[stage].pTex = pTex;
            mTexStageDesc[stage].used = true;
            mTexStageDesc[stage].type = dt->getTextureType();

            mLastTextureUnitState = stage+1;
        }
        else
        {
            mTexStageDesc[stage].used = false;
            // now we now what's the last texture unit set
			mLastTextureUnitState = std::min(mLastTextureUnitState,stage);
        }
        mSamplerStatesChanged = true;
    }
    void D3D11RenderSystem::_setSampler(size_t unit, TextureSampleUnit& sampler)
    {
        mSamplerStatesChanged = true;

        const TextureSampleUnit::UVWAddressingMode& uvw = sampler.getAddressingMode();
        mTexStageDesc[unit].samplerDesc.AddressU = D3D11Mappings::get(uvw.u);
        mTexStageDesc[unit].samplerDesc.AddressV = D3D11Mappings::get(uvw.v);
        mTexStageDesc[unit].samplerDesc.AddressW = D3D11Mappings::get(uvw.w);
        mTexStageDesc[unit].samplerDesc.MipLODBias = sampler.getMipmapBias();

        if (uvw.u == TAM_BORDER || uvw.v == TAM_BORDER || uvw.w == TAM_BORDER)
            D3D11Mappings::get(sampler.getBorderColour(), mTexStageDesc[unit].samplerDesc.BorderColor);

        mTexStageDesc[unit].samplerDesc.MaxAnisotropy = sampler.getAnisotropy();
        mTexStageDesc[unit].samplerDesc.ComparisonFunc = D3D11Mappings::get(sampler.getCompareFunction());

        FilterMinification[unit] = sampler.getFiltering(FT_MIN);
        FilterMagnification[unit] = sampler.getFiltering(FT_MAG);
        FilterMips[unit] = sampler.getFiltering(FT_MIP);

        mTexStageDesc[unit].samplerDesc.Filter =
            D3D11Mappings::get(FilterMinification[unit], FilterMagnification[unit], FilterMips[unit],
                               sampler.getCompareEnabled());
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTextureAddressingMode( size_t stage, const TextureSampleUnit::UVWAddressingMode & uvw )
    {
        // record the stage state
        mTexStageDesc[stage].samplerDesc.AddressU = D3D11Mappings::get(uvw.u);
        mTexStageDesc[stage].samplerDesc.AddressV = D3D11Mappings::get(uvw.v);
        mTexStageDesc[stage].samplerDesc.AddressW = D3D11Mappings::get(uvw.w);
        mSamplerStatesChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setSeparateSceneBlending( ColourFactor sourceFactor, ColourFactor destFactor, ColourFactor sourceFactorAlpha, ColourFactor destFactorAlpha, ColourBlendMode op /*= SBO_ADD*/, ColourBlendMode alphaOp /*= SBO_ADD*/ )
    {
        if( sourceFactor == SBF_ONE && destFactor == SBF_ZERO && 
            sourceFactorAlpha == SBF_ONE && destFactorAlpha == SBF_ZERO)
        {
            mBlendDesc.RenderTarget[0].BlendEnable = FALSE;
        }
        else
        {
            mBlendDesc.RenderTarget[0].BlendEnable = TRUE;
            mBlendDesc.RenderTarget[0].SrcBlend = D3D11Mappings::get(sourceFactor, false);
            mBlendDesc.RenderTarget[0].DestBlend = D3D11Mappings::get(destFactor, false);
            mBlendDesc.RenderTarget[0].BlendOp = D3D11Mappings::get(op) ;
            mBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11Mappings::get(sourceFactorAlpha, true);
            mBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11Mappings::get(destFactorAlpha, true);
            mBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11Mappings::get(alphaOp) ;
            mBlendDesc.AlphaToCoverageEnable = false;

            mBlendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
        }
        mBlendDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setAlphaRejectSettings( CmpMode func, unsigned char value, bool alphaToCoverage )
    {
        mSceneAlphaRejectFunc   = func;
        mSceneAlphaRejectValue  = value;
        mSceneAlphaToCoverage   = alphaToCoverage;
        mBlendDesc.AlphaToCoverageEnable = alphaToCoverage;
        mBlendDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setCullingMode( CullingMode mode )
    {
        mCullingMode = mode;

		bool flip = (mInvertVertexWinding && !mActiveRenderTarget->isFlipping() ||
					!mInvertVertexWinding && mActiveRenderTarget->isFlipping());

		mRasterizerDesc.CullMode = D3D11Mappings::get(mode, flip);
        mRasterizerDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setDepthBufferParams( bool depthTest, bool depthWrite, CmpMode depthFunction )
    {
        _setDepthBufferCheckEnabled( depthTest );
        _setDepthBufferWriteEnabled( depthWrite );
        _setDepthBufferFunction( depthFunction );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setDepthBufferCheckEnabled( bool enabled )
    {
        mDepthStencilDesc.DepthEnable = enabled;
        mDepthStencilDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setDepthBufferWriteEnabled( bool enabled )
    {
        if (enabled)
        {
            mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        }
        else
        {
            mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        }
        mDepthStencilDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setDepthBufferFunction( CmpMode func )
    {
        mDepthStencilDesc.DepthFunc = D3D11Mappings::get(func);
        mDepthStencilDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setDepthBias(float constantBias, float slopeScaleBias)
    {
		const float nearFarFactor = 10.0; 
		mRasterizerDesc.DepthBias = static_cast<int>(-constantBias * nearFarFactor);
		mRasterizerDesc.SlopeScaledDepthBias = -slopeScaleBias;
        mRasterizerDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setColourBufferWriteEnabled(bool red, bool green, 
        bool blue, bool alpha)
    {
        UINT8 val = 0;
        if (red) 
            val |= D3D11_COLOR_WRITE_ENABLE_RED;
        if (green)
            val |= D3D11_COLOR_WRITE_ENABLE_GREEN;
        if (blue)
            val |= D3D11_COLOR_WRITE_ENABLE_BLUE;
        if (alpha)
            val |= D3D11_COLOR_WRITE_ENABLE_ALPHA;

        mBlendDesc.RenderTarget[0].RenderTargetWriteMask = val; 
        mBlendDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setPolygonMode(PolygonMode level)
    {
        if(mPolygonMode != level)
        {
            mPolygonMode = level;
            mRasterizerDesc.FillMode = D3D11Mappings::get(mPolygonMode);
            mRasterizerDescChanged = true;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setStencilCheckEnabled(bool enabled)
    {
        mDepthStencilDesc.StencilEnable = enabled;
        mDepthStencilDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setStencil(CmpMode func, 
        uint32 refValue, uint32 compareMask, uint32 writeMask, StencilOpType stencilFailOp, 
        StencilOpType depthFailOp, StencilOpType passOp, 
        bool twoSidedOperation, bool readBackAsTexture)
    {
		// We honor user intent in case of one sided operation, and carefully tweak it in case of two sided operations.
		bool flipFront = twoSidedOperation &&
						(mInvertVertexWinding && !mActiveRenderTarget->isFlipping() ||
						!mInvertVertexWinding && mActiveRenderTarget->isFlipping());
		bool flipBack = twoSidedOperation && !flipFront;

        mStencilRef = refValue;
        mDepthStencilDesc.StencilReadMask = compareMask;
        mDepthStencilDesc.StencilWriteMask = writeMask;

		mDepthStencilDesc.FrontFace.StencilFailOp = D3D11Mappings::get(stencilFailOp, flipFront);
		mDepthStencilDesc.BackFace.StencilFailOp = D3D11Mappings::get(stencilFailOp, flipBack);
        
		mDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11Mappings::get(depthFailOp, flipFront);
		mDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11Mappings::get(depthFailOp, flipBack);
        
		mDepthStencilDesc.FrontFace.StencilPassOp = D3D11Mappings::get(passOp, flipFront);
		mDepthStencilDesc.BackFace.StencilPassOp = D3D11Mappings::get(passOp, flipBack);

		mDepthStencilDesc.FrontFace.StencilFunc = D3D11Mappings::get(func);
		mDepthStencilDesc.BackFace.StencilFunc = D3D11Mappings::get(func);
        mReadBackAsTexture = readBackAsTexture;
        mDepthStencilDescChanged = true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTextureUnitFiltering(size_t unit, FilterType ftype, 
        TextureFilterOP filter)
    {
        switch(ftype) {
        case FT_MIN:
            FilterMinification[unit] = filter;
            break;
        case FT_MAG:
            FilterMagnification[unit] = filter;
            break;
        case FT_MIP:
            FilterMips[unit] = filter;
            break;
        }

        mTexStageDesc[unit].samplerDesc.Filter = D3D11Mappings::get(FilterMinification[unit], FilterMagnification[unit], FilterMips[unit],CompareEnabled);
        mSamplerStatesChanged = true;
    }
    //---------------------------------------------------------------------
    DWORD D3D11RenderSystem::_getCurrentAnisotropy(size_t unit)
    {
        return mTexStageDesc[unit].samplerDesc.MaxAnisotropy;;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setRenderTarget(RenderTarget *target)
    {
        mActiveRenderTarget = target;
        if (mActiveRenderTarget)
        {
            // we need to clear the state 
            mDevice.GetImmediateContext()->ClearState();

            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot Clear State\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_setRenderTarget");
            }

            _setRenderTargetViews();
        }
    }

    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setRenderTargetViews()
    {
        RenderTarget* target = mActiveRenderTarget;
        D3D11RenderTarget* d3d11RenderTarget = dynamic_cast<D3D11RenderTarget*>(target);

        if (target && d3d11RenderTarget)
        {
            ID3D11RenderTargetView* pRTView[NII_MAX_RenderTarget];
            memset(pRTView, 0, sizeof(pRTView));

            uint numberOfViews = d3d11RenderTarget->getNumberOfViews();

            for (uint i = 0; i < NII_MAX_RenderTarget; i++)
            {
                pRTView[i] = d3d11RenderTarget->getRenderTargetView(i);
                if (!pRTView[i])
                {
                    break;
                }
            }

            //Retrieve depth buffer
            D3D11DepthBuffer *depthBuffer = static_cast<D3D11DepthBuffer*>(target->getDepthBuffer());

            if( target->getDepthBufferPool() != DepthBuffer::GroupUnknow && !depthBuffer )
            {
                //Depth is automatically managed and there is no depth buffer attached to this RT
                //or the Current D3D device doesn't match the one this Depth buffer was created
                setDepthBufferFor( target );
            }

            //Retrieve depth buffer again (it may have changed)
            depthBuffer = static_cast<D3D11DepthBuffer*>(target->getDepthBuffer());

            // now switch to the new render target
            mDevice.GetImmediateContext()->OMSetRenderTargets(
                numberOfViews,
                pRTView,
                depthBuffer ? depthBuffer->getDepthStencilView() : 0 );

            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set render target\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_setRenderTargetViews");
            }
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setViewport( Viewport *vp )
    {
        if (!vp)
        {
            mActiveViewport = NULL;
            _setRenderTarget(NULL);
        }
        else if( vp != mActiveViewport || !vp->isSyncRender() )
        {
            mActiveViewport = vp;

            // ok, it's different, time to set render target and viewport params
            D3D11_VIEWPORT d3dvp;

            // Set render target
            RenderTarget* target;
            target = vp->getTarget();

            _setRenderTarget(target);
            _setCullingMode( mCullingMode );

            // set viewport dimensions
            d3dvp.TopLeftX = static_cast<FLOAT>(vp->getPixelLeft());
            d3dvp.TopLeftY = static_cast<FLOAT>(vp->getPixelTop());
            d3dvp.Width = static_cast<FLOAT>(vp->getPixelWidth());
            d3dvp.Height = static_cast<FLOAT>(vp->getPixelHeight());
            if (target->isFlipping())
            {
                // Convert "top-left" to "bottom-left"
                d3dvp.TopLeftY = target->getHeight() - d3dvp.Height - d3dvp.TopLeftY;
            }

            // Z-values from 0.0 to 1.0 (TODO: standardise with OpenGL)
            d3dvp.MinDepth = 0.0f;
            d3dvp.MaxDepth = 1.0f;

            mDevice.GetImmediateContext()->RSSetViewports(1, &d3dvp);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set viewports\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_setViewport");
            }

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
			D3D11RenderWindowBase* d3d11Window = dynamic_cast<D3D11RenderWindowBase*>(target);
			if(d3d11Window)
				d3d11Window->_validateStereo();
#endif

            vp->syncRenderImpl();
        }
        else
        {
            // if swapchain was created with DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL we need to reestablish render target views
            D3D11RenderWindowBase* d3d11Window = dynamic_cast<D3D11RenderWindowBase*>(vp->getTarget());
            if(d3d11Window && d3d11Window->_shouldRebindBackBuffer())
                _setRenderTargetViews();
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_beginFrame()
    {
    
        if( !mActiveViewport )
            N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Cannot begin frame - no viewport selected.", "D3D11RenderSystem::_beginFrame" );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_endFrame()
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setVertexDeclaration(VertexDeclaration* decl)
    {
            N_EXCEPT( Exception::ERR_INTERNAL_ERROR, 
                    "Cannot directly call setVertexDeclaration in the d3d11 render system - cast then use 'setVertexDeclaration(VertexDeclaration* decl, VertexBufferBinding* binding)' .", 
                    "D3D11RenderSystem::setVertexDeclaration" );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setVertexDeclaration(VertexDeclaration* decl, VertexBufferBinding* binding)
    {
        D3D11VertexDeclaration* d3ddecl = 
            static_cast<D3D11VertexDeclaration*>(decl);

        d3ddecl->bindToShader(mBoundVertexProgram, binding);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setVertexBufferBinding(VertexBufferBinding* binding)
    {
        // TODO: attempt to detect duplicates
        const VertexBufferBinding::VertexBufferBindingMap& binds = binding->getBindings();
        VertexBufferBinding::VertexBufferBindingMap::const_iterator i, iend;
        iend = binds.end();
        for (i = binds.begin(); i != iend; ++i)
        {
            const D3D11VertexBuffer* d3d11buf = 
                static_cast<const D3D11VertexBuffer*>(i->second.get());

            UINT stride = static_cast<UINT>(d3d11buf->getVertexSize());
            UINT offset = 0; // no stream offset, this is handled in _render instead
            UINT slot = static_cast<UINT>(i->first);
            ID3D11Buffer * pVertexBuffers = d3d11buf->getD3DVertexBuffer();
            mDevice.GetImmediateContext()->IASetVertexBuffers(
                slot, // The first input slot for binding.
                1, // The number of vertex buffers in the array.
                &pVertexBuffers,
                &stride,
                &offset 
                );

            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set vertex buffers\nError Description:" + errorDescription,
                    "D3D11RenderSystem::setVertexBufferBinding");
            }
        }

        mLastVertexSourceCount = binds.size();      
    }

    //---------------------------------------------------------------------
    // TODO: Move this class to the right place.
    class D3D11RenderOperationState : public Renderable::RenderSystemData
    {
    public:
        ComPtr<ID3D11BlendState> mBlendState;
        ComPtr<ID3D11RasterizerState> mRasterizer;
        ComPtr<ID3D11DepthStencilState> mDepthStencilState;

        ComPtr<ID3D11SamplerState> mSamplerStates[NII_MAX_TextureLayer];
        size_t mSamplerStatesCount;

        ID3D11ShaderResourceView * mTextures[NII_MAX_TextureLayer]; // note - not owning
        size_t mTexturesCount;

        D3D11RenderOperationState() : mSamplerStatesCount(0), mTexturesCount(0) {}
        ~D3D11RenderOperationState() {}
    };

    //---------------------------------------------------------------------
    void D3D11RenderSystem::_dispatchCompute(const Vector3i& workgroupDim)
    {
        // Bound unordered access views
        mDevice.GetImmediateContext()->Dispatch(workgroupDim[0], workgroupDim[1], workgroupDim[2]);

        // unbind
        ID3D11UnorderedAccessView* views[] = { 0 };
        ID3D11ShaderResourceView* srvs[] = { 0 };
        mDevice.GetImmediateContext()->CSSetShaderResources( 0, 1, srvs );
        mDevice.GetImmediateContext()->CSSetUnorderedAccessViews( 0, 1, views, NULL );
        mDevice.GetImmediateContext()->CSSetShader( NULL, NULL, 0 );
    }

    void D3D11RenderSystem::_render(const GeometryRaw& op)
    {

        // Exit immediately if there is nothing to render
        if (op.vertexData==0 || op.vertexData->vertexCount == 0)
        {
            return;
        }

        HardwareVertexBufferSharedPtr globalInstanceVertexBuffer = getInstanceBuffer();
        VertexDeclaration* globalVertexDeclaration = getInstanceData();

        bool hasInstanceData = op.useGlobalInstancingVertexBufferIsAvailable &&
                    globalInstanceVertexBuffer && globalVertexDeclaration != NULL 
                || op.vertexData->vertexBufferBinding->hasInstanceData();

        size_t numberOfInstances = op.mInstanceCount;

        if (op.useGlobalInstancingVertexBufferIsAvailable)
        {
            numberOfInstances *= getInstanceCount();
        }

        // Call super class
        RenderSystem::_render(op);
        
        D3D11RenderOperationState stackOpState;
        D3D11RenderOperationState * opState = &stackOpState;

        if(mBlendDescChanged)
        {
            mBlendDescChanged = false;
            mBoundBlendState = 0;

            HRESULT hr = mDevice->CreateBlendState(&mBlendDesc, opState->mBlendState.ReleaseAndGetAddressOf()) ;
            if (FAILED(hr))
            {
				String errorDescription = mDevice.getErrorString(hr);
				N_EXCEPT_EX(RenderingAPI, hr,
                    "Failed to create blend state\nError Description:" + errorDescription, 
                    "D3D11RenderSystem::_render" );
            }
        }
        else
        {
            opState->mBlendState = mBoundBlendState;
        }

        if(mRasterizerDescChanged)
		{
			mRasterizerDescChanged=false;
			mBoundRasterizer = 0;

            HRESULT hr = mDevice->CreateRasterizerState(&mRasterizerDesc, opState->mRasterizer.ReleaseAndGetAddressOf()) ;
            if (FAILED(hr))
            {
				String errorDescription = mDevice.getErrorString(hr);
				N_EXCEPT_EX(RenderingAPI, hr,
                    "Failed to create rasterizer state\nError Description:" + errorDescription, 
                    "D3D11RenderSystem::_render" );
            }
        }
        else
        {
            opState->mRasterizer = mBoundRasterizer;
        }

        if(mDepthStencilDescChanged)
		{
			mBoundDepthStencilState = 0;
			mDepthStencilDescChanged=false;

            HRESULT hr = mDevice->CreateDepthStencilState(&mDepthStencilDesc, opState->mDepthStencilState.ReleaseAndGetAddressOf()) ;
            if (FAILED(hr))
            {
				String errorDescription = mDevice.getErrorString(hr);
				N_EXCEPT_EX(RenderingAPI, hr,
                    "Failed to create depth stencil state\nError Description:" + errorDescription, 
                    "D3D11RenderSystem::_render" );
            }
        }
        else
		{
			opState->mDepthStencilState = mBoundDepthStencilState;
		}

        if(mSamplerStatesChanged)
		{
            // samplers mapping
            size_t numberOfSamplers = std::min(mLastTextureUnitState,(size_t)(NII_MAX_TextureLayer + 1));
            
            opState->mSamplerStatesCount = numberOfSamplers;
            opState->mTexturesCount = numberOfSamplers;
                            
            for (size_t n = 0; n < numberOfSamplers; n++)
            {
                ComPtr<ID3D11SamplerState> samplerState;
                ID3D11ShaderResourceView *texture = NULL;
                sD3DTextureStageDesc & stage = mTexStageDesc[n];
                if(stage.used)
                {
                    texture = stage.pTex;

                    stage.samplerDesc.Filter = D3D11Mappings::get(FilterMinification[n], FilterMagnification[n], FilterMips[n], false);
                    stage.samplerDesc.ComparisonFunc = D3D11Mappings::get(mSceneAlphaRejectFunc);
                    stage.samplerDesc.MipLODBias = static_cast<float>(std::clamp(stage.samplerDesc.MipLODBias - 0.5, -16.00, 15.99));
                    stage.samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
                    stage.samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

                    HRESULT hr = mDevice->CreateSamplerState(&stage.samplerDesc, samplerState.ReleaseAndGetAddressOf());
                    if (FAILED(hr))
                    {
                        String errorDescription = mDevice.getErrorString(hr);
                        N_EXCEPT_EX(RenderingAPI, hr,
                            "Failed to create sampler state\nError Description:" + errorDescription,
                            "D3D11RenderSystem::_render" );
                    }
                
                }
                opState->mSamplerStates[n].Swap(samplerState);
                opState->mTextures[n]       = texture;
            }
            for (size_t n = opState->mTexturesCount; n < NII_MAX_TextureLayer; n++)
			{
				opState->mTextures[n] = NULL;
			}
        }

        if (opState->mBlendState != mBoundBlendState)
        {
            mBoundBlendState = opState->mBlendState ;
            mDevice.GetImmediateContext()->OMSetBlendState(opState->mBlendState.Get(), 0, 0xffffffff); // TODO - find out where to get the parameters
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set blend state\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }
        }

        if (opState->mRasterizer != mBoundRasterizer)
        {
            mBoundRasterizer = opState->mRasterizer ;

            mDevice.GetImmediateContext()->RSSetState(opState->mRasterizer.Get());
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set rasterizer state\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }
        }
        

        if (opState->mDepthStencilState != mBoundDepthStencilState)
        {
            mBoundDepthStencilState = opState->mDepthStencilState ;

            mDevice.GetImmediateContext()->OMSetDepthStencilState(opState->mDepthStencilState.Get(), mStencilRef);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set depth stencil state\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }
        }

        if (mSamplerStatesChanged && opState->mSamplerStatesCount > 0 ) //  if the NumSamplers is 0, the operation effectively does nothing.
        {
            mSamplerStatesChanged = false; // now it's time to set it to false
            /// Pixel Shader binding
            {
                {
                    mDevice.GetImmediateContext()->PSSetSamplers(static_cast<UINT>(0), static_cast<UINT>(opState->mSamplerStatesCount), opState->mSamplerStates[0].GetAddressOf());
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT(RenderingAPI, 
                            "D3D11 device cannot set pixel shader samplers\nError Description:" + errorDescription,
                            "D3D11RenderSystem::_render");
                    }
                }

                mDevice.GetImmediateContext()->PSSetShaderResources(static_cast<UINT>(0), static_cast<UINT>(opState->mTexturesCount), &opState->mTextures[0]);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set pixel shader resources\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }
            }
            
            /// Vertex Shader binding
            if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            {
                mDevice.GetImmediateContext()->VSSetSamplers(static_cast<UINT>(0), static_cast<UINT>(opState->mSamplerStatesCount), opState->mSamplerStates[0].GetAddressOf());
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set pixel shader samplers\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }
                mDevice.GetImmediateContext()->VSSetShaderResources(static_cast<UINT>(0), static_cast<UINT>(opState->mTexturesCount), &opState->mTextures[0]);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set pixel shader resources\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }
            }

            /// Geometry Shader binding
            if (mBoundGeometryProgram && mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            {
                mDevice.GetImmediateContext()->GSSetSamplers(0, opState->mSamplerStatesCount, opState->mSamplerStates[0].GetAddressOf());
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set geometry shader samplers\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }

                mDevice.GetImmediateContext()->GSSetShaderResources(0, opState->mTexturesCount, &opState->mTextures[0]);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set geometry shader resources\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }
            }

            /// Compute Shader binding
            if (mBoundComputeProgram && mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            {
                mDevice.GetImmediateContext()->CSSetSamplers(static_cast<UINT>(0), static_cast<UINT>(opState->mSamplerStatesCount), opState->mSamplerStates[0].GetAddressOf());
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set compute shader samplers\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }

                mDevice.GetImmediateContext()->CSSetShaderResources(static_cast<UINT>(0), static_cast<UINT>(opState->mTexturesCount), &opState->mTextures[0]);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set compute shader resources\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }
            }

            /// Hull Shader binding
            if (mBoundTessellationHullProgram && mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            {
                mDevice.GetImmediateContext()->HSSetSamplers(static_cast<UINT>(0), static_cast<UINT>(opState->mSamplerStatesCount), opState->mSamplerStates[0].GetAddressOf());
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set hull shader samplers\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }

                mDevice.GetImmediateContext()->HSSetShaderResources(static_cast<UINT>(0), static_cast<UINT>(opState->mTexturesCount), &opState->mTextures[0]);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set hull shader resources\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }
            }
            
            /// Domain Shader binding
            if (mBoundTessellationDomainProgram && mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            {
                mDevice.GetImmediateContext()->DSSetSamplers(static_cast<UINT>(0), static_cast<UINT>(opState->mSamplerStatesCount), opState->mSamplerStates[0].GetAddressOf());
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set domain shader samplers\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }

                mDevice.GetImmediateContext()->DSSetShaderResources(static_cast<UINT>(0), static_cast<UINT>(opState->mTexturesCount), &opState->mTextures[0]);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI,
                        "D3D11 device cannot set domain shader resources\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }
            }
        }

        ComPtr<ID3D11Buffer> pSOTarget;
        // Mustn't bind a emulated vertex, pixel shader (see below), if we are rendering to a stream out buffer
        mDevice.GetImmediateContext()->SOGetTargets(1, pSOTarget.GetAddressOf());

        //check consistency of vertex-fragment shaders
        if (!mBoundVertexProgram ||
             (!mBoundFragmentProgram && op.mType != GeometryRaw::OT_POINT_LIST && !pSOTarget ) 
           ) 
        {
            
            N_EXCEPT(RenderingAPI, 
                "Attempted to render to a D3D11 device without both vertex and fragment shaders there is no fixed pipeline in d3d11 - use the RTSS or write custom shaders.",
                "D3D11RenderSystem::_render");
        }

        // Check consistency of tessellation shaders
        if( (mBoundTessellationHullProgram && !mBoundTessellationDomainProgram) ||
            (!mBoundTessellationHullProgram && mBoundTessellationDomainProgram) )
        {
            if (mBoundTessellationHullProgram && !mBoundTessellationDomainProgram) {
            N_EXCEPT(RenderingAPI, 
                "Attempted to use tessellation, but domain shader is missing",
                "D3D11RenderSystem::_render");
            }
            else {
                N_EXCEPT(RenderingAPI, 
                "Attempted to use tessellation, but hull shader is missing",
                "D3D11RenderSystem::_render"); }
        }

        if (mDevice.isError())
        {
            // this will never happen but we want to be consistent with the error checks... 
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT(RenderingAPI, 
                "D3D11 device cannot set geometry shader to null\nError Description:" + errorDescription,
                "D3D11RenderSystem::_render");
        }

        // Defer program bind to here because we must bind shader class instances,
        // and this can only be made in SetShader calls.
        // Also, bind shader resources
        if (mBoundVertexProgram)
        {
            mDevice.GetImmediateContext()->VSSetShader(mBoundVertexProgram->getVertexShader(), 
                                                       mClassInstances[GPT_VERTEX_PROGRAM], 
                                                       mNumClassInstances[GPT_VERTEX_PROGRAM]);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set vertex shader\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }
        }
        if (mBoundFragmentProgram)
        {
            mDevice.GetImmediateContext()->PSSetShader(mBoundFragmentProgram->getPixelShader(),
                                                       mClassInstances[GPT_FRAGMENT_PROGRAM], 
                                                       mNumClassInstances[GPT_FRAGMENT_PROGRAM]);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set pixel shader\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }
        }
        if (mBoundGeometryProgram)
        {
            mDevice.GetImmediateContext()->GSSetShader(mBoundGeometryProgram->getGeometryShader(),
                                                       mClassInstances[GPT_GEOMETRY_PROGRAM], 
                                                       mNumClassInstances[GPT_GEOMETRY_PROGRAM]);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set geometry shader\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }
        }
        if (mBoundTessellationHullProgram)
        {
            mDevice.GetImmediateContext()->HSSetShader(mBoundTessellationHullProgram->getHullShader(),
                                                       mClassInstances[GPT_HULL_PROGRAM], 
                                                       mNumClassInstances[GPT_HULL_PROGRAM]);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set hull shader\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }
        }
        if (mBoundTessellationDomainProgram)
        {
            mDevice.GetImmediateContext()->DSSetShader(mBoundTessellationDomainProgram->getDomainShader(),
                                                       mClassInstances[GPT_DOMAIN_PROGRAM], 
                                                       mNumClassInstances[GPT_DOMAIN_PROGRAM]);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set domain shader\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }
        }
        if (mBoundComputeProgram)
        {
            mDevice.GetImmediateContext()->CSSetShader(mBoundComputeProgram->getComputeShader(),
                                                       mClassInstances[GPT_COMPUTE_PROGRAM], 
                                                       mNumClassInstances[GPT_COMPUTE_PROGRAM]);
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set compute shader\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }
        }


        setVertexDeclaration(op.vertexData->vertexDeclaration, op.vertexData->vertexBufferBinding);
        setVertexBufferBinding(op.vertexData->vertexBufferBinding);


        // Determine rendering operation
        D3D11_PRIMITIVE_TOPOLOGY primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        DWORD primCount = 0;

        if(mBoundTessellationHullProgram && mBoundTessellationDomainProgram)
        {
            // useful primitives for tessellation
            switch( op.mType )
            {
            case GeometryRaw::OT_LINE_LIST:
                primType = D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 2;
                break;

            case GeometryRaw::OT_LINE_STRIP:
                primType = D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 1;
                break;

            case GeometryRaw::OT_TRIANGLE_LIST:
                primType = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 3;
                break;

            case GeometryRaw::OT_TRIANGLE_STRIP:
                primType = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 2;
                break;
            }
        }
        else
        {
            //rendering without tessellation.   
            int mType = op.mType;
            if(mGPMode && mBoundGeometryProgram && mBoundGeometryProgram->isAdjacencyPrimitive())
                mType |= GeometryRaw::OT_DETAIL_ADJACENCY_BIT;

            switch( mType )
            {
            case GeometryRaw::OT_POINT_LIST:
                primType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount);
                break;

            case GeometryRaw::OT_LINE_LIST:
                primType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 2;
                break;

            case GeometryRaw::OT_LINE_LIST_ADJ:
                primType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 4;
                break;

            case GeometryRaw::OT_LINE_STRIP:
                primType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 1;
                break;

            case GeometryRaw::OT_LINE_STRIP_ADJ:
                primType = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 2;
                break;

            case GeometryRaw::OT_TRIANGLE_LIST:
                primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 3;
                break;

            case GeometryRaw::OT_TRIANGLE_LIST_ADJ:
                primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 6;
                break;

            case GeometryRaw::OT_TRIANGLE_STRIP:
                primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 2;
                break;

            case GeometryRaw::OT_TRIANGLE_STRIP_ADJ:
                primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 2 - 2;
                break;

            case GeometryRaw::OT_TRIANGLE_FAN:
                N_EXCEPT(RenderingAPI, "Error - DX11 render - no support for triangle fan (OT_TRIANGLE_FAN)", "D3D11RenderSystem::_render");
                primType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED; // todo - no TRIANGLE_FAN in DX 11
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 2;
                break;
            }
        }
        
        if (primCount)
        {
            // Issue the op
            //HRESULT hr;
            if( op.useIndexes  )
            {
                D3D11IndexBuffer* d3dIdxBuf = 
                    static_cast<D3D11IndexBuffer*>(op.indexData->indexBuffer.get());
                mDevice.GetImmediateContext()->IASetIndexBuffer( d3dIdxBuf->getD3DIndexBuffer(), D3D11Mappings::getFormat(d3dIdxBuf->getType()), 0 );
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set index buffer\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }
            }

            mDevice.GetImmediateContext()->IASetPrimitiveTopology( primType );
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(RenderingAPI, 
                    "D3D11 device cannot set primitive topology\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }

            do
            {
                if(op.useIndexes)
                {
                    if(hasInstanceData)
                    {
                        mDevice.GetImmediateContext()->DrawIndexedInstanced(
                            static_cast<UINT>(op.indexData->indexCount), 
                            static_cast<UINT>(numberOfInstances), 
                            static_cast<UINT>(op.indexData->indexStart), 
                            static_cast<INT>(op.vertexData->vertexStart),
                            0);
                    }
                    else
                    {
                        mDevice.GetImmediateContext()->DrawIndexed(
                            static_cast<UINT>(op.indexData->indexCount),
                            static_cast<UINT>(op.indexData->indexStart),
                            static_cast<INT>(op.vertexData->vertexStart));
                    }
                }
                else // non indexed
                {
                    if(op.vertexData->vertexCount == -1) // -1 is a sign to use DrawAuto
                    {
                        mDevice.GetImmediateContext()->DrawAuto();
                    }
                    else if(hasInstanceData)
                    {
                        mDevice.GetImmediateContext()->DrawInstanced(
                            static_cast<UINT>(op.vertexData->vertexCount),
                            static_cast<UINT>(numberOfInstances),
                            static_cast<UINT>(op.vertexData->vertexStart),
                            0);
                    }
                    else
                    {
                        mDevice.GetImmediateContext()->Draw(
                            static_cast<UINT>(op.vertexData->vertexCount),
                            static_cast<UINT>(op.vertexData->vertexStart));
                    }
                }

                if(mDevice.isError())
                {
                    String errorDescription = "D3D11 device cannot draw";
                    if(!op.useIndexes && op.vertexData->vertexCount == -1) // -1 is a sign to use DrawAuto
                        errorDescription.append(" auto");
                    else
                        errorDescription.append(op.useIndexes ? " indexed" : "").append(hasInstanceData ? " instanced" : "");
                    errorDescription.append("\nError Description:").append(mDevice.getErrorString());
                    errorDescription.append("\nActive OGRE shaders:")
                        .append(mBoundVertexProgram ? ("\nVS = " + mBoundVertexProgram->getName()).c_str() : "")
                        .append(mBoundTessellationHullProgram ? ("\nHS = " + mBoundTessellationHullProgram->getName()).c_str() : "")
                        .append(mBoundTessellationDomainProgram ? ("\nDS = " + mBoundTessellationDomainProgram->getName()).c_str() : "")
                        .append(mBoundGeometryProgram ? ("\nGS = " + mBoundGeometryProgram->getName()).c_str() : "")
                        .append(mBoundFragmentProgram ? ("\nFS = " + mBoundFragmentProgram->getName()).c_str() : "")
                        .append(mBoundComputeProgram ? ("\nCS = " + mBoundComputeProgram->getName()).c_str() : "");

                    N_EXCEPT(RenderingAPI, errorDescription, "D3D11RenderSystem::_render");
                }

            }while(updateParam());
        }


        // Crashy : commented this, 99% sure it's useless but really time consuming
        /*if (true) // for now - clear the render state
        {
            mDevice.GetImmediateContext()->OMSetBlendState(0, 0, 0xffffffff); 
            mDevice.GetImmediateContext()->RSSetState(0);
            mDevice.GetImmediateContext()->OMSetDepthStencilState(0, 0); 
//          mDevice->PSSetSamplers(static_cast<UINT>(0), static_cast<UINT>(0), 0);
            
            // Clear class instance storage
            memset(mClassInstances, 0, sizeof(mClassInstances));
            memset(mNumClassInstances, 0, sizeof(mNumClassInstances));      
        }*/

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_renderUsingReadBackAsTexture(unsigned int passNr, Ogre::String variableName, unsigned int StartSlot)
    {
        RenderTarget* target = mActiveRenderTarget;
        D3D11RenderTarget* d3d11RenderTarget = dynamic_cast<D3D11RenderTarget*>(target);
        switch (passNr)
        {
        case 1:
            if (target && d3d11RenderTarget)
            {
                ID3D11RenderTargetView* pRTView[NII_MAX_RenderTarget];
                memset(pRTView, 0, sizeof(pRTView));

                uint numberOfViews = d3d11RenderTarget->getNumberOfViews();

                for (uint i = 0; i < NII_MAX_RenderTarget; i++)
                {
                    pRTView[i] = d3d11RenderTarget->getRenderTargetView(i);
                    if (!pRTView[i])
                    {
                        break;
                    }
                }

                //Retrieve depth buffer
                D3D11DepthBuffer *depthBuffer = static_cast<D3D11DepthBuffer*>(target->getDepthBuffer());

                // now switch to the new render target
                mDevice.GetImmediateContext()->OMSetRenderTargets(
                    numberOfViews,
                    pRTView,
                    depthBuffer->getDepthStencilView());

                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set render target\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_renderUsingReadBackAsTexture");
                }
                
                mDevice.GetImmediateContext()->ClearDepthStencilView(depthBuffer->getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

                float ClearColor[4];
                // D3D11Mappings::get(colour, ClearColor);
                // Clear all views
                for (uint i = 0; i < numberOfViews; ++i)
                {
                    mDevice.GetImmediateContext()->ClearRenderTargetView(pRTView[i], ClearColor);
                }
            }
            break;
        case 2:
            if (target && d3d11RenderTarget)
            {
                //
                // We need to remove the the DST from the Render Targets if we want to use it as a texture :
                //
                ID3D11RenderTargetView* pRTView[NII_MAX_RenderTarget];
                memset(pRTView, 0, sizeof(pRTView));

                uint numberOfViews = d3d11RenderTarget->getNumberOfViews();

                for (uint i = 0; i < NII_MAX_RenderTarget; i++)
                {
                    pRTView[i] = d3d11RenderTarget->getRenderTargetView(i);
                    if (!pRTView[i])
                    {
                        break;
                    }
                }

                //Retrieve depth buffer
                D3D11DepthBuffer *depthBuffer = static_cast<D3D11DepthBuffer*>(target->getDepthBuffer());

                // now switch to the new render target
                mDevice.GetImmediateContext()->OMSetRenderTargets(
                    numberOfViews,
                    pRTView,
                    NULL);

                mDevice.GetImmediateContext()->PSSetShaderResources(static_cast<UINT>(StartSlot), 1, mDSTResView.GetAddressOf());
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set pixel shader resources\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_renderUsingReadBackAsTexture");
                }

            }
            break;
        case 3:
            //
            // We need to unbind mDSTResView from the given variable because this buffer
            // will be used later as the typical depth buffer, again
            // must call Apply(0) here : to flush SetResource(NULL)
            //
            
            if (target)
            {
                mDevice.GetImmediateContext()->PSSetShaderResources(static_cast<UINT>(StartSlot), 1, NULL);
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT(RenderingAPI, 
                            "D3D11 device cannot set pixel shader resources\nError Description:" + errorDescription,
                            "D3D11RenderSystem::_renderUsingReadBackAsTexture");
                    }           
            }

            break;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::bindGpuProgram(GpuProgram* prg)
    {
        if (!prg)
        {
            N_EXCEPT(RenderingAPI, 
                "Null program bound.",
                "D3D11RenderSystem::bindGpuProgram");
        }

        switch (prg->getType())
        {
        case GPT_VERTEX_PROGRAM:
            {
                // get the shader
                mBoundVertexProgram = static_cast<D3D11HLSLProgram*>(prg);
/*              ID3D11VertexShader * vsShaderToSet = mBoundVertexProgram->getVertexShader();

                // set the shader
                mDevice.GetImmediateContext()->VSSetShader(vsShaderToSet, NULL, 0);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set vertex shader\nError Description:" + errorDescription,
                        "D3D11RenderSystem::bindGpuProgram");
                }*/     
            }
            break;
        case GPT_FRAGMENT_PROGRAM:
            {
                mBoundFragmentProgram = static_cast<D3D11HLSLProgram*>(prg);
/*              ID3D11PixelShader* psShaderToSet = mBoundFragmentProgram->getPixelShader();

                mDevice.GetImmediateContext()->PSSetShader(psShaderToSet, NULL, 0);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set fragment shader\nError Description:" + errorDescription,
                        "D3D11RenderSystem::bindGpuProgram");
                }*/     
            }
            break;
        case GPT_GEOMETRY_PROGRAM:
            {
                mBoundGeometryProgram = static_cast<D3D11HLSLProgram*>(prg);
/*              ID3D11GeometryShader* gsShaderToSet = mBoundGeometryProgram->getGeometryShader();

                mDevice.GetImmediateContext()->GSSetShader(gsShaderToSet, NULL, 0);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set geometry shader\nError Description:" + errorDescription,
                        "D3D11RenderSystem::bindGpuProgram");
                }*/     

            }
            break;
        case GPT_HULL_PROGRAM:
            {
                mBoundTessellationHullProgram = static_cast<D3D11HLSLProgram*>(prg);
/*              ID3D11HullShader* gsShaderToSet = mBoundTessellationHullProgram->getHullShader();

                mDevice.GetImmediateContext()->HSSetShader(gsShaderToSet, NULL, 0);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set hull shader\nError Description:" + errorDescription,
                        "D3D11RenderSystem::bindGpuProgram");
                }       */

            }
            break;
        case GPT_DOMAIN_PROGRAM:
            {
                mBoundTessellationDomainProgram = static_cast<D3D11HLSLProgram*>(prg);
/*              ID3D11DomainShader* gsShaderToSet = mBoundTessellationDomainProgram->getDomainShader();

                mDevice.GetImmediateContext()->DSSetShader(gsShaderToSet, NULL, 0);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set domain shader\nError Description:" + errorDescription,
                        "D3D11RenderSystem::bindGpuProgram");
                }*/     

            }
            break;
        case GPT_COMPUTE_PROGRAM:
            {
                mBoundComputeProgram = static_cast<D3D11HLSLProgram*>(prg);
/*              ID3D11ComputeShader* gsShaderToSet = mBoundComputeProgram->getComputeShader();

                mDevice.GetImmediateContext()->CSSetShader(gsShaderToSet, NULL, 0);
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(RenderingAPI, 
                        "D3D11 device cannot set compute shader\nError Description:" + errorDescription,
                        "D3D11RenderSystem::bindGpuProgram");
                }*/     

            }
            break;
        };

        RenderSystem::bindGpuProgram(prg);
   }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::unbindGpuProgram(GpuProgramType gptype)
    {

        switch(gptype)
        {
        case GPT_VERTEX_PROGRAM:
            {
                mActiveVGPP.reset();
                mBoundVertexProgram = NULL;
                //mDevice->VSSetShader(NULL);
                mDevice.GetImmediateContext()->VSSetShader(NULL, NULL, 0);
            }
            break;
        case GPT_FRAGMENT_PROGRAM:
            {
                mActiveFGPP.reset();
                mBoundFragmentProgram = NULL;
                //mDevice->PSSetShader(NULL);
                mDevice.GetImmediateContext()->PSSetShader(NULL, NULL, 0);
            }

            break;
        case GPT_GEOMETRY_PROGRAM:
            {
                mActiveGGPP.reset();
                mBoundGeometryProgram = NULL;
                mDevice.GetImmediateContext()->GSSetShader( NULL, NULL, 0 );
            }
            break;
        case GPT_HULL_PROGRAM:
            {
                mActiveTGPP.reset();
                mBoundTessellationHullProgram = NULL;
                mDevice.GetImmediateContext()->HSSetShader( NULL, NULL, 0 );
            }
            break;
        case GPT_DOMAIN_PROGRAM:
            {
                mActiveDGPP.reset();
                mBoundTessellationDomainProgram = NULL;
                mDevice.GetImmediateContext()->DSSetShader( NULL, NULL, 0 );
            }
            break;
        case GPT_COMPUTE_PROGRAM:
            {
                mActiveCGPP.reset();
                mBoundComputeProgram = NULL;
                mDevice.GetImmediateContext()->CSSetShader( NULL, NULL, 0 );
            }
            break;
        default:
            N_assert(false && "Undefined Program Type!");
        };
        RenderSystem::unbindGpuProgram(gptype);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::on(GpuProgramType gptype, const GpuProgramParam *& params, uint16 mask)
    {
        if (mask & (uint16)GPV_GLOBAL)
        {
            // TODO: Dx11 supports shared constant buffers, so use them
            // check the match to constant buffers & use rendersystem data hooks to store
            // for now, just copy
            params->syncValue();
        }

        // Do everything here in Dx11, since deal with via buffers anyway so number of calls
        // is actually the same whether we categorise the updates or not
        ID3D11Buffer* pBuffers[1] ;
        switch(gptype)
        {
        case GPT_VERTEX_PROGRAM:
            {
                //  if (params->getAutoConstantCount() > 0)
                //{
                if (mBoundVertexProgram)
                {
                    pBuffers[0] = mBoundVertexProgram->getConstantBuffer(params, mask);
                    mDevice.GetImmediateContext()->VSSetConstantBuffers( 0, 1, pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT(RenderingAPI, 
                            "D3D11 device cannot set vertex shader constant buffers\nError Description:" + errorDescription,
                            "D3D11RenderSystem::on");
                    }       

                }
            }
            break;
        case GPT_FRAGMENT_PROGRAM:
            {
                //if (params->getAutoConstantCount() > 0)
                //{
                if (mBoundFragmentProgram)
                {
                    pBuffers[0] = mBoundFragmentProgram->getConstantBuffer(params, mask);
                    mDevice.GetImmediateContext()->PSSetConstantBuffers( 0, 1, pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT(RenderingAPI, 
                            "D3D11 device cannot set fragment shader constant buffers\nError Description:" + errorDescription,
                            "D3D11RenderSystem::on");
                    }       

                }
            }
            break;
        case GPT_GEOMETRY_PROGRAM:
            {
                if (mBoundGeometryProgram)
                {
                    pBuffers[0] = mBoundGeometryProgram->getConstantBuffer(params, mask);
                    mDevice.GetImmediateContext()->GSSetConstantBuffers( 0, 1, pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT(RenderingAPI, 
                            "D3D11 device cannot set Geometry shader constant buffers\nError Description:" + errorDescription,
                            "D3D11RenderSystem::on");
                    }       

                }
            }
            break;
        case GPT_HULL_PROGRAM:
            {
                if (mBoundTessellationHullProgram)
                {
                    pBuffers[0] = mBoundTessellationHullProgram->getConstantBuffer(params, mask);
                    mDevice.GetImmediateContext()->HSSetConstantBuffers( 0, 1, pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT(RenderingAPI, 
                            "D3D11 device cannot set Hull shader constant buffers\nError Description:" + errorDescription,
                            "D3D11RenderSystem::on");
                    }       

                }
            }
            break;
        case GPT_DOMAIN_PROGRAM:
            {
                if (mBoundTessellationDomainProgram)
                {
                    pBuffers[0] = mBoundTessellationDomainProgram->getConstantBuffer(params, mask);
                    mDevice.GetImmediateContext()->DSSetConstantBuffers( 0, 1, pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT(RenderingAPI, 
                            "D3D11 device cannot set Domain shader constant buffers\nError Description:" + errorDescription,
                            "D3D11RenderSystem::on");
                    }       

                }
            }
            break;
        case GPT_COMPUTE_PROGRAM:
            {
                if (mBoundComputeProgram)
                {
                    pBuffers[0] = mBoundComputeProgram->getConstantBuffer(params, mask);
                    mDevice.GetImmediateContext()->CSSetConstantBuffers( 0, 1, pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT(RenderingAPI, 
                            "D3D11 device cannot set Compute shader constant buffers\nError Description:" + errorDescription,
                            "D3D11RenderSystem::on");
                    }       

                }
            }
            break;
        };

        // Now, set class instances
        const GpuProgramParameters::SubroutineMap& subroutineMap = params->getSubroutineMap();
        if (subroutineMap.empty())
            return;

        GpuProgramParameters::SubroutineIterator it;
        GpuProgramParameters::SubroutineIterator end = subroutineMap.end();
        for(it = subroutineMap.begin(); it != end; ++it)
        {
            setSubroutine(gptype, it->first, it->second);
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::bindGpuProgramPassIterationParameters(GpuProgramType gptype)
    {

        switch(gptype)
        {
        case GPT_VERTEX_PROGRAM:
            on(gptype, mActiveVGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;

        case GPT_FRAGMENT_PROGRAM:
            on(gptype, mActiveFGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        case GPT_GEOMETRY_PROGRAM:
            on(gptype, mActiveGGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        case GPT_HULL_PROGRAM:
            on(gptype, mActiveTGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        case GPT_DOMAIN_PROGRAM:
            on(gptype, mActiveDGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        case GPT_COMPUTE_PROGRAM:
            on(gptype, mActiveCGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setSubroutine(GpuProgramType gptype, unsigned int slotIndex, const String& subroutineName)
    {
        ID3D11ClassInstance* instance = 0;
        
        ClassInstanceIterator it = mInstanceMap.find(subroutineName);
        if (it == mInstanceMap.end())
        {
            // try to get instance already created (must have at least one field)
            HRESULT hr = mDevice.GetClassLinkage()->GetClassInstance(subroutineName.c_str(), 0, &instance);
            if (FAILED(hr) || instance == 0)
            {
                // probably class don't have a field, try create a new
                hr = mDevice.GetClassLinkage()->CreateClassInstance(subroutineName.c_str(), 0, 0, 0, 0, &instance);
                if (FAILED(hr) || instance == 0)
                {
					N_EXCEPT_EX(RenderingAPI, hr,
						"Shader subroutine with name " + subroutineName + " doesn't exist.",
						"D3D11RenderSystem::setSubroutineName");
                }
            }

            // Store class instance
            mInstanceMap.insert(std::make_pair(subroutineName, instance));
        }
        else
        {
            instance = it->second;
        }
        
        // If already created, store class instance
        mClassInstances[gptype][slotIndex] = instance;
        mNumClassInstances[gptype] = mNumClassInstances[gptype] + 1;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setSubroutine(GpuProgramType gptype, const String& slotName, const String& subroutineName)
    {
        unsigned int slotIdx = 0;
        switch(gptype)
        {
        case GPT_VERTEX_PROGRAM:
            {
                if (mBoundVertexProgram)
                {
                    slotIdx = mBoundVertexProgram->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_FRAGMENT_PROGRAM:
            {
                if (mBoundFragmentProgram)
                {
                    slotIdx = mBoundFragmentProgram->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_GEOMETRY_PROGRAM:
            {
                if (mBoundGeometryProgram)
                {
                    slotIdx = mBoundGeometryProgram->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_HULL_PROGRAM:
            {
                if (mBoundTessellationHullProgram)
                {
                    slotIdx = mBoundTessellationHullProgram->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_DOMAIN_PROGRAM:
            {
                if (mBoundTessellationDomainProgram)
                {
                    slotIdx = mBoundTessellationDomainProgram->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_COMPUTE_PROGRAM:
            {
                if (mBoundComputeProgram)
                {
                    slotIdx = mBoundComputeProgram->getSubroutineSlot(slotName);
                }
            }
            break;
        };
        
        // Set subroutine for slot
        setSubroutine(gptype, slotIdx, subroutineName);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setScissorTest(bool enabled, size_t left, size_t top, size_t right,
        size_t bottom)
    {
        mRasterizerDesc.ScissorEnable = enabled;
        mScissorRect.left = static_cast<LONG>(left);
        mScissorRect.top = static_cast<LONG>(top);
        mScissorRect.right = static_cast<LONG>(right);
        mScissorRect.bottom =static_cast<LONG>( bottom);

        mDevice.GetImmediateContext()->RSSetScissorRects(1, &mScissorRect);
        if (mDevice.isError())
        {
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT(RenderingAPI, 
                "D3D11 device cannot set scissor rects\nError Description:" + errorDescription,
                "D3D11RenderSystem::setScissorTest");
        }   
        mRasterizerDescChanged=true;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::clearBuffer(unsigned int buffers, 
        const Colour& colour, Real depth, unsigned short stencil)
    {
        D3D11RenderTarget* d3d11RenderTarget = dynamic_cast<D3D11RenderTarget*>(mActiveRenderTarget);
        if (mActiveRenderTarget && d3d11RenderTarget)
        {
            ID3D11RenderTargetView* pRTView[NII_MAX_RenderTarget];
            memset(pRTView, 0, sizeof(pRTView));

            for (uint i = 0; i < NII_MAX_RenderTarget; i++)
            {
                pRTView[i] = d3d11RenderTarget->getRenderTargetView(i);
                if (!pRTView[i])
                {
                    break;
                }
            }

            if (buffers & FBT_COLOUR)
            {
                float ClearColor[4];
                D3D11Mappings::get(colour, ClearColor);

                // Clear all views
                uint numberOfViews = d3d11RenderTarget->getNumberOfViews();
                for (uint i = 0; i < numberOfViews; ++i)
                {
                    mDevice.GetImmediateContext()->ClearRenderTargetView(pRTView[i], ClearColor);
                }

            }
            UINT ClearFlags = 0;
            if (buffers & FBT_DEPTH)
            {
                ClearFlags |= D3D11_CLEAR_DEPTH;
            }
            if (buffers & FBT_STENCIL)
            {
                ClearFlags |= D3D11_CLEAR_STENCIL;
            }

            if (ClearFlags)
            {
                D3D11DepthBuffer *depthBuffer = static_cast<D3D11DepthBuffer*>(mActiveRenderTarget->
                                                                                        getDepthBuffer());
                if( depthBuffer )
                {
                    mDevice.GetImmediateContext()->ClearDepthStencilView(
                                                        depthBuffer->getDepthStencilView(),
                                                        ClearFlags, depth, static_cast<UINT8>(stencil) );
                }
            }
        }
    }
    //---------------------------------------------------------------------
    PixelCountQuery* D3D11RenderSystem::create(void)
    {
        D3D11HardwareOcclusionQuery* ret = new D3D11HardwareOcclusionQuery (mDevice); 
        mPixelQueryList.push_back(ret);
        return ret;
    }
    //---------------------------------------------------------------------
    Real D3D11RenderSystem::getTexelXOffset(void)
    {
        // D3D11 is now like GL
        return 0.0f;
    }
    //---------------------------------------------------------------------
    Real D3D11RenderSystem::getTexelYOffset(void)
    {
        // D3D11 is now like GL
        return 0.0f;
    }
    //---------------------------------------------------------------------
    Real D3D11RenderSystem::getMinDepth(void)
    {
        // Range [0.0f, 1.0f]
        return 0.0f;
    }
    //---------------------------------------------------------------------
    Real D3D11RenderSystem::getMaxDepth(void)
    {
        // Range [0.0f, 1.0f]
        // D3D inverts even identity view matrices, so maximum INPUT is -1.0
        return -1.0f;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::registerThread()
    {
        // nothing to do - D3D11 shares rendering context already
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::unregisterThread()
    {
        // nothing to do - D3D11 shares rendering context already
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::preExtraThreadsStarted()
    {
        // nothing to do - D3D11 shares rendering context already
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::postExtraThreadsStarted()
    {
        // nothing to do - D3D11 shares rendering context already
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::determineFSAASettings(uint fsaa, const String& fsaaHint, 
        DXGI_FORMAT format, DXGI_SAMPLE_DESC* outFSAASettings)
    {
        bool qualityHint = fsaa >= 8 && fsaaHint.find("Quality") != String::npos;
        
        // NVIDIA, AMD - prefer CSAA aka EQAA if available.
        // see http://developer.nvidia.com/object/coverage-sampled-aa.html
        // see http://developer.amd.com/wordpress/media/2012/10/EQAA%20Modes%20for%20AMD%20HD%206900%20Series%20Cards.pdf

        // Modes are sorted from high quality to low quality, CSAA aka EQAA are listed first
        // Note, that max(Count, Quality) == FSAA level and (Count >= 8 && Quality != 0) == quality hint
        DXGI_SAMPLE_DESC presets[] = {
                { 8, 16 }, // CSAA 16xQ, EQAA 8f16x
                { 4, 16 }, // CSAA 16x,  EQAA 4f16x
                { 16, 0 }, // MSAA 16x

                { 12, 0 }, // MSAA 12x

                { 8, 8 },  // CSAA 8xQ
                { 4, 8 },  // CSAA 8x,  EQAA 4f8x
                { 8, 0 },  // MSAA 8x

                { 6, 0 },  // MSAA 6x
                { 4, 0 },  // MSAA 4x
                { 2, 0 },  // MSAA 2x
                { 1, 0 },  // MSAA 1x
                { NULL },
        };

        // Skip too HQ modes
        DXGI_SAMPLE_DESC* mode = presets;
        for(; mode->Count != 0; ++mode)
        {
            unsigned modeFSAA = std::max(mode->Count, mode->Quality);
            bool modeQuality = mode->Count >= 8 && mode->Quality != 0;
            bool tooHQ = (modeFSAA > fsaa || modeFSAA == fsaa && modeQuality && !qualityHint);
            if(!tooHQ)
                break;
        }

        // Use first supported mode
        for(; mode->Count != 0; ++mode)
        {
            UINT outQuality;
            HRESULT hr = mDevice->CheckMultisampleQualityLevels(format, mode->Count, &outQuality);

            if(SUCCEEDED(hr) && outQuality > mode->Quality)
            {
                *outFSAASettings = *mode;
                return;
            }
        }

        outFSAASettings->Count = 1;
        outFSAASettings->Quality = 0;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::initRenderSystem()
    {
        if (mRenderSystemWasInited)
        {
            return;
        }

        mRenderSystemWasInited = true;
        // set pointers to NULL
        mDriverList = NULL;
        mTextureManager = NULL;
        mHardwareBufferManager = NULL;
        mGpuProgramManager = NULL;
        mPrimaryWindow = NULL;
        mMinRequestedFeatureLevel = D3D_FEATURE_LEVEL_9_1;
#if __OGRE_WINRT_PHONE // Windows Phone support only FL 9.3, but simulator can create much more capable device, so restrict it artificially here
        mMaxRequestedFeatureLevel = D3D_FEATURE_LEVEL_9_3;
#elif defined(_WIN32_WINNT_WIN8) && _WIN32_WINNT >= _WIN32_WINNT_WIN8
        mMaxRequestedFeatureLevel = D3D_FEATURE_LEVEL_11_1;
#else
        mMaxRequestedFeatureLevel = D3D_FEATURE_LEVEL_11_0;
#endif
        mUseNVPerfHUD = false;
        mHLSLProgramFactory = NULL;

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
		N_delete mStereoDriver;
		mStereoDriver = NULL;
#endif

        mBoundVertexProgram = NULL;
        mBoundFragmentProgram = NULL;
        mBoundGeometryProgram = NULL;
        mBoundTessellationHullProgram = NULL;
        mBoundTessellationDomainProgram = NULL;
        mBoundComputeProgram = NULL;

        ZeroMemory( &mBlendDesc, sizeof(mBlendDesc));

        ZeroMemory( &mRasterizerDesc, sizeof(mRasterizerDesc));
        mRasterizerDesc.FrontCounterClockwise = true;
		mRasterizerDesc.DepthClipEnable = true;
        mRasterizerDesc.MultisampleEnable = true;


        ZeroMemory( &mDepthStencilDesc, sizeof(mDepthStencilDesc));

        ZeroMemory( &mDepthStencilDesc, sizeof(mDepthStencilDesc));
        ZeroMemory( &mScissorRect, sizeof(mScissorRect));

        // set filters to defaults
        for (size_t n = 0; n < NII_MAX_TextureLayer; n++)
        {
            FilterMinification[n] = FO_NONE;
            FilterMagnification[n] = FO_NONE;
            FilterMips[n] = FO_NONE;
        }

        mPolygonMode = PM_SOLID;
        mRasterizerDesc.FillMode = D3D11Mappings::get(mPolygonMode);

        //sets the modification trackers to true
        mBlendDescChanged = true;
		mRasterizerDescChanged = true;
		mDepthStencilDescChanged = true;
		mSamplerStatesChanged = true;
		mLastTextureUnitState = 0;

        ZeroMemory(mTexStageDesc, NII_MAX_TextureLayer * sizeof(sD3DTextureStageDesc));

        mLastVertexSourceCount = 0;
        mReadBackAsTexture = false;

        ID3D11DeviceN * device = createD3D11Device(NULL, D3D_DRIVER_TYPE_HARDWARE, mMinRequestedFeatureLevel, mMaxRequestedFeatureLevel, 0);
        mDevice.TransferOwnership(device);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::getPlatformData(const String& name, void* pData)
    {
        if( name == "D3DDEVICE" )
        {
            *(ID3D11DeviceN**)pData = mDevice.get();
        }
        else
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Attribute not found: " + name, "RenderSystem::getPlatformData");
        }
    }
    //---------------------------------------------------------------------
    bool D3D11RenderSystem::_getDepthBufferCheckEnabled( void )
    {
        return mDepthStencilDesc.DepthEnable == TRUE;
    }
    //---------------------------------------------------------------------
    D3D11HLSLProgram* D3D11RenderSystem::_getBoundVertexProgram() const
    {
        return mBoundVertexProgram;
    }
    //---------------------------------------------------------------------
    D3D11HLSLProgram* D3D11RenderSystem::_getBoundFragmentProgram() const
    {
        return mBoundFragmentProgram;
    }
    //---------------------------------------------------------------------
    D3D11HLSLProgram* D3D11RenderSystem::_getBoundGeometryProgram() const
    {
        return mBoundGeometryProgram;
    }
    //---------------------------------------------------------------------
    D3D11HLSLProgram* D3D11RenderSystem::_getBoundTessellationHullProgram() const
    {
        return mBoundTessellationHullProgram;
    }
    //---------------------------------------------------------------------
    D3D11HLSLProgram* D3D11RenderSystem::_getBoundTessellationDomainProgram() const
    {
        return mBoundTessellationDomainProgram;
    }
    //---------------------------------------------------------------------
    D3D11HLSLProgram* D3D11RenderSystem::_getBoundComputeProgram() const
    {
        return mBoundComputeProgram;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::beginProfiler( const String &eventName )
    {
#if OGRE_D3D11_PROFILING
        if(mDevice.GetProfiler())
        {			
            wchar_t wideName[256]; // Let avoid heap memory allocation if we are in profiling code.
            bool wideNameOk = !eventName.empty() && 0 != MultiByteToWideChar(CP_ACP, 0, eventName.data(), eventName.length() + 1, wideName, ARRAYSIZE(wideName));
            mDevice.GetProfiler()->BeginEvent(wideNameOk ? wideName : L"<too long or empty event name>");
        }
#endif
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::endProfiler( void )
    {
#if OGRE_D3D11_PROFILING
        if(mDevice.GetProfiler())
            mDevice.GetProfiler()->EndEvent();
#endif
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::markProfiler( const String &eventName )
    {
#if OGRE_D3D11_PROFILING
        if(mDevice.GetProfiler())
        {
            wchar_t wideName[256]; // Let avoid heap memory allocation if we are in profiling code.
            bool wideNameOk = !eventName.empty() && 0 != MultiByteToWideChar(CP_ACP, 0, eventName.data(), eventName.length() + 1, wideName, ARRAYSIZE(wideName));
            mDevice.GetProfiler()->SetMarker(wideNameOk ? wideName : L"<too long or empty event name>");
        }
#endif
    }
    /////////////////////////////////////////////////////////////////////////
    //---------------------------------------------------------------------
    D3D11RenderSystem::D3D11RenderSystem()
        : mDevice(),
          mVendorExtension( 0 ),
          mBoundIndirectBuffer( 0 ),
          mIndirectPtr( 0 ),
          mPso( 0 ),
          mBoundComputeProgram( 0 ),
          mMaxBoundUavCS( 0 ),
          mCurrentVertexBuffer( 0 ),
          mCurrentIndexBuffer( 0 ),
          mNumberOfViews( 0 ),
          mDepthStencilView( 0 ),
          mMaxComputeShaderSrvCount( 0 )
#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        , mStereoDriver(NULL)
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
        , suspendingToken()
        , surfaceContentLostToken()
#endif
    {
        LogManager::getSingleton().logMessage( "D3D11: " + getName() + " created." );

        memset( mRenderTargetViews, 0, sizeof( mRenderTargetViews ) );
        memset( mNullViews, 0, sizeof(mNullViews) );
        memset( mMaxSrvCount, 0, sizeof(mMaxSrvCount) );

        mRenderSystemWasInited = false;
        mSwitchingFullscreenCounter = 0;
        mDriverType = D3D_DRIVER_TYPE_HARDWARE;

        initRenderSystem();

        // set config options defaults
        initConfigOptions();

        // Clear class instance storage
        memset(mClassInstances, 0, sizeof(mClassInstances));
        memset(mNumClassInstances, 0, sizeof(mNumClassInstances));

        mEventNames.push_back("DeviceLost");
        mEventNames.push_back("DeviceRestored");

#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
#if defined(_WIN32_WINNT_WINBLUE) && _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
        suspendingToken = (Windows::ApplicationModel::Core::CoreApplication::Suspending +=
            ref new Windows::Foundation::EventHandler<Windows::ApplicationModel::SuspendingEventArgs^>([this](Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e)
        {
            // Hints to the driver that the app is entering an idle state and that its memory can be used temporarily for other apps.
            ComPtr<IDXGIDevice3> pDXGIDevice;
            if(mDevice.get() && SUCCEEDED(mDevice->QueryInterface(pDXGIDevice.GetAddressOf())))
                pDXGIDevice->Trim();
        }));

        surfaceContentLostToken = (Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated +=
            ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation^, Platform::Object^>(
                [this](Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ arg)
        {
            LogManager::getSingleton().logMessage("D3D11: DisplayContentsInvalidated.");
            checkDevice(true);
        }));
#else // Win 8.0
        surfaceContentLostToken = (Windows::Graphics::Display::DisplayProperties::DisplayContentsInvalidated +=
            ref new Windows::Graphics::Display::DisplayPropertiesEventHandler([this](Platform::Object ^sender)
        {
            LogManager::getSingleton().logMessage("D3D11: DisplayContentsInvalidated.");
            checkDevice(true);
        }));
#endif
#endif
    }
    //---------------------------------------------------------------------
    D3D11RenderSystem::~D3D11RenderSystem()
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
#if defined(_WIN32_WINNT_WINBLUE) && _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
        Windows::ApplicationModel::Core::CoreApplication::Suspending -= suspendingToken;
        Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated -= surfaceContentLostToken;
#else // Win 8.0
        Windows::Graphics::Display::DisplayProperties::DisplayContentsInvalidated -= surfaceContentLostToken;
#endif
#endif

        shutdown();

        // Deleting the HLSL program factory
        if (mHLSLProgramFactory)
        {
            // Remove from manager safely
            if (HighLevelGpuProgramManager::getSingletonPtr())
                HighLevelGpuProgramManager::getSingleton().removeFactory(mHLSLProgramFactory);
            delete mHLSLProgramFactory;
            mHLSLProgramFactory = 0;
        }

        LogManager::getSingleton().logMessage( "D3D11: " + getName() + " destroyed." );
    }
    //---------------------------------------------------------------------
    const String& D3D11RenderSystem::getName() const
    {
        static String strName( "Direct3D11 Rendering Subsystem");
        return strName;
    }
    //---------------------------------------------------------------------
    const String& D3D11RenderSystem::getFriendlyName(void) const
    {
        static String strName("Direct3D 11");
        return strName;
    }
    //---------------------------------------------------------------------
    D3D11DriverList* D3D11RenderSystem::getDirect3DDrivers(bool refreshList /* = false*/)
    {
        if(!mDriverList)
            mDriverList = new D3D11DriverList();

        if(refreshList || mDriverList->count() == 0)
            mDriverList->refresh();

        return mDriverList;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createD3D11Device( D3D11VendorExtension *vendorExtension,
                                               const String &appName,
                                               D3D11Driver* d3dDriver,
                                               D3D_DRIVER_TYPE driverType,
                                               D3D_FEATURE_LEVEL minFL, D3D_FEATURE_LEVEL maxFL,
                                               D3D_FEATURE_LEVEL* pFeatureLevel,
                                               ID3D11Device **outDevice )
    {
        IDXGIAdapterN* pAdapter = (d3dDriver && driverType == D3D_DRIVER_TYPE_HARDWARE) ?
                                      d3dDriver->getDeviceAdapter() : NULL;

        assert( driverType == D3D_DRIVER_TYPE_HARDWARE || driverType == D3D_DRIVER_TYPE_SOFTWARE ||
                driverType == D3D_DRIVER_TYPE_WARP );
        if( d3dDriver != NULL )
        {
            if( 0 == wcscmp(d3dDriver->getAdapterIdentifier().Description, L"NVIDIA PerfHUD") )
                driverType = D3D_DRIVER_TYPE_REFERENCE;
            else
                driverType = D3D_DRIVER_TYPE_UNKNOWN;
        }

        // determine deviceFlags
        UINT deviceFlags = 0;
#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
        // This flag is required in order to enable compatibility with Direct2D.
        deviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif
        if( OGRE_DEBUG_MODE >= OGRE_DEBUG_HIGH && !IsWorkingUnderNsight() &&
            D3D11Device::D3D_NO_EXCEPTION != D3D11Device::getExceptionsErrorLevel() )
        {
            deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        }
        if( !OGRE_THREAD_SUPPORT )
            deviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;

        // determine feature levels
        D3D_FEATURE_LEVEL requestedLevels[] =
        {
            // Windows Phone support only FL 9.3, but simulator can create
            // much more capable device, so restrict it artificially here
#if !__OGRE_WINRT_PHONE
#if defined( _WIN32_WINNT_WIN8 )
            D3D_FEATURE_LEVEL_11_1,
#endif
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
#endif // !__OGRE_WINRT_PHONE
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        D3D_FEATURE_LEVEL *pFirstFL = requestedLevels;
        D3D_FEATURE_LEVEL *pLastFL  = pFirstFL + ARRAYSIZE(requestedLevels) - 1;
        for( size_t i = 0; i <ARRAYSIZE(requestedLevels); ++i )
        {
            if( minFL == requestedLevels[i] )
                pLastFL = &requestedLevels[i];
            if( maxFL == requestedLevels[i] )
                pFirstFL = &requestedLevels[i];
        }
        if( pLastFL < pFirstFL )
        {
            N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Requested min level feature is bigger the requested max level feature.");
        }

        vendorExtension->createDevice( appName, pAdapter, driverType, deviceFlags, pFirstFL,
                                       static_cast<UINT>( pLastFL - pFirstFL + 1u ),
                                       pFeatureLevel, outDevice );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::initConfigOptions()
    {
        ConfigOption optDevice;
        ConfigOption optVideoMode;
        ConfigOption optFullScreen;
        ConfigOption optVSync;
        ConfigOption optVSyncInterval;
        ConfigOption optBackBufferCount;
        ConfigOption optAA;
        ConfigOption optFPUMode;
        ConfigOption optNVPerfHUD;
        ConfigOption optSRGB;
        ConfigOption optMinFeatureLevels;
        ConfigOption optMaxFeatureLevels;
        ConfigOption optExceptionsErrorLevel;
        ConfigOption optDriverType;
        ConfigOption optVendorExt;
        ConfigOption optFastShaderBuildHack;
#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        ConfigOption optStereoMode;
#endif

        optDevice.name = "Rendering Device";
        optDevice.currentValue = "(default)";
        optDevice.possibleValues.push_back("(default)");
        D3D11DriverList* driverList = getDirect3DDrivers();
        for( unsigned j=0; j < driverList->count(); j++ )
        {
            D3D11Driver* driver = driverList->item(j);
            optDevice.possibleValues.push_back( driver->DriverDescription() );
        }
        optDevice.immutable = false;

        optVideoMode.name = "Video Mode";
        //optVideoMode.currentValue = "800 x 600 @ 32-bit colour";
        optVideoMode.immutable = false;

        optFullScreen.name = "Full Screen";
        optFullScreen.possibleValues.push_back( "Yes" );
        optFullScreen.possibleValues.push_back( "No" );
        optFullScreen.currentValue = "Yes";
        optFullScreen.immutable = false;

        optVSync.name = "VSync";
        optVSync.immutable = false;
        optVSync.possibleValues.push_back( "Yes" );
        optVSync.possibleValues.push_back( "No" );
        optVSync.currentValue = "No";

        optVSyncInterval.name = "VSync Interval";
        optVSyncInterval.immutable = false;
        optVSyncInterval.possibleValues.push_back( "1" );
        optVSyncInterval.possibleValues.push_back( "2" );
        optVSyncInterval.possibleValues.push_back( "3" );
        optVSyncInterval.possibleValues.push_back( "4" );
        optVSyncInterval.currentValue = "1";

        optBackBufferCount.name = "Backbuffer Count";
        optBackBufferCount.immutable = false;
        optBackBufferCount.possibleValues.push_back( "Auto" );
        optBackBufferCount.possibleValues.push_back( "1" );
        optBackBufferCount.possibleValues.push_back( "2" );
        optBackBufferCount.currentValue = "Auto";


        optAA.name = "FSAA";
        optAA.immutable = false;
        optAA.possibleValues.push_back( "None" );
        optAA.currentValue = "None";

        optFPUMode.name = "Floating-point mode";
#if OGRE_DOUBLE_PRECISION
        optFPUMode.currentValue = "Consistent";
#else
        optFPUMode.currentValue = "Fastest";
#endif
        optFPUMode.possibleValues.clear();
        optFPUMode.possibleValues.push_back("Fastest");
        optFPUMode.possibleValues.push_back("Consistent");
        optFPUMode.immutable = false;

        optNVPerfHUD.currentValue = "No";
        optNVPerfHUD.immutable = false;
        optNVPerfHUD.name = "Allow NVPerfHUD";
        optNVPerfHUD.possibleValues.push_back( "Yes" );
        optNVPerfHUD.possibleValues.push_back( "No" );

        // SRGB on auto window
        optSRGB.name = "sRGB Gamma Conversion";
        optSRGB.possibleValues.push_back("Yes");
        optSRGB.possibleValues.push_back("No");
        optSRGB.currentValue = "Yes";
        optSRGB.immutable = false;

        // min feature level
        optMinFeatureLevels;
        optMinFeatureLevels.name = "Min Requested Feature Levels";
        optMinFeatureLevels.possibleValues.push_back("9.1");
        optMinFeatureLevels.possibleValues.push_back("9.3");
        optMinFeatureLevels.possibleValues.push_back("10.0");
        optMinFeatureLevels.possibleValues.push_back("10.1");
        optMinFeatureLevels.possibleValues.push_back("11.0");

        optMinFeatureLevels.currentValue = "9.1";
        optMinFeatureLevels.immutable = false;


        // max feature level
        optMaxFeatureLevels;
        optMaxFeatureLevels.name = "Max Requested Feature Levels";
        optMaxFeatureLevels.possibleValues.push_back("9.1");

#if __OGRE_WINRT_PHONE_80
        optMaxFeatureLevels.possibleValues.push_back("9.2");
        optMaxFeatureLevels.possibleValues.push_back("9.3");
        optMaxFeatureLevels.currentValue = "9.3";
#else
        optMaxFeatureLevels.possibleValues.push_back("9.3");
        optMaxFeatureLevels.possibleValues.push_back("10.0");
        optMaxFeatureLevels.possibleValues.push_back("10.1");
        optMaxFeatureLevels.possibleValues.push_back("11.0");
#if defined(_WIN32_WINNT_WIN8)
        if (IsWindows8OrGreater())
        {
            optMaxFeatureLevels.possibleValues.push_back("11.1");
            optMaxFeatureLevels.currentValue = "11.1";
        }
        else
        {
            optMaxFeatureLevels.currentValue = "11.0";
        }
#else
        optMaxFeatureLevels.currentValue = "11.0";
#endif
#endif
        optMaxFeatureLevels.immutable = false;

        // Exceptions Error Level
        optExceptionsErrorLevel.name = "Information Queue Exceptions Bottom Level";
        optExceptionsErrorLevel.possibleValues.push_back("No information queue exceptions");
        optExceptionsErrorLevel.possibleValues.push_back("Corruption");
        optExceptionsErrorLevel.possibleValues.push_back("Error");
        optExceptionsErrorLevel.possibleValues.push_back("Warning");
        optExceptionsErrorLevel.possibleValues.push_back("Info (exception on any message)");
#if OGRE_DEBUG_MODE >= OGRE_DEBUG_HIGH
        optExceptionsErrorLevel.currentValue = "Info (exception on any message)";
#else
        optExceptionsErrorLevel.currentValue = "No information queue exceptions";
#endif
        optExceptionsErrorLevel.immutable = false;


        // Driver type
        optDriverType.name = "Driver type";
        optDriverType.possibleValues.push_back("Hardware");
        optDriverType.possibleValues.push_back("Software");
        optDriverType.possibleValues.push_back("Warp");
        optDriverType.currentValue = "Hardware";
        optDriverType.immutable = false;

        optVendorExt.name = "Vendor Extensions";
        optVendorExt.possibleValues.push_back("Auto");
        optVendorExt.possibleValues.push_back("NVIDIA");
        optVendorExt.possibleValues.push_back("AMD");
        optVendorExt.possibleValues.push_back("Disabled");
        optVendorExt.currentValue = "Auto";
        optVendorExt.immutable = false;

        //This option improves shader compilation times by massive amounts
        //(requires Hlms to be aware of it), making shader compile times comparable
        //to GL (which is measured in milliseconds per shader, instead of seconds).
        //There's two possible reasons to disable this hack:
        //  1. Easier debugging. Shader structs like "Material m[256];" get declared
        //     as "Material m[2];" which cause debuggers to show only 2 entires,
        //     instead of all of them. Some debuggers (like RenderDoc) allow changing
        //     the amount of elements displayed and workaround it; nonetheless
        //     disabling it makes your life easier.
        //  2. Troubleshooting an obscure GPU/driver combination. I tested this hack
        //     with a lot of hardware and it seems to work. However the possibility
        //     that it breaks with a specific GPU/driver combo always exists. In
        //     such case, the end user should be able to turn this off.
        optFastShaderBuildHack.name = "Fast Shader Build Hack";
        optFastShaderBuildHack.possibleValues.push_back( "Yes" );
        optFastShaderBuildHack.possibleValues.push_back( "No" );
        optFastShaderBuildHack.currentValue = "Yes";
        optFastShaderBuildHack.immutable = false;

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        optStereoMode.name = "Stereo Mode";
        optStereoMode.possibleValues.push_back(StringConverter::toString(SMT_NONE));
        optStereoMode.possibleValues.push_back(StringConverter::toString(SMT_FRAME_SEQUENTIAL));
        optStereoMode.currentValue = optStereoMode.possibleValues[0];
        optStereoMode.immutable = false;

        mOptions[optStereoMode.name] = optStereoMode;
#endif

        mOptions[optDevice.name] = optDevice;
        mOptions[optVideoMode.name] = optVideoMode;
        mOptions[optFullScreen.name] = optFullScreen;
        mOptions[optVSync.name] = optVSync;
        mOptions[optVSyncInterval.name] = optVSyncInterval;
        mOptions[optAA.name] = optAA;
        mOptions[optFPUMode.name] = optFPUMode;
        mOptions[optNVPerfHUD.name] = optNVPerfHUD;
        mOptions[optSRGB.name] = optSRGB;
        mOptions[optMinFeatureLevels.name] = optMinFeatureLevels;
        mOptions[optMaxFeatureLevels.name] = optMaxFeatureLevels;
        mOptions[optExceptionsErrorLevel.name] = optExceptionsErrorLevel;
        mOptions[optDriverType.name] = optDriverType;
        mOptions[optVendorExt.name] = optVendorExt;
        mOptions[optFastShaderBuildHack.name] = optFastShaderBuildHack;

        mOptions[optBackBufferCount.name] = optBackBufferCount;


        refreshD3DSettings();
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::refreshD3DSettings()
    {
        ConfigOption* optVideoMode;
        D3D11VideoMode* videoMode;

        ConfigOptionMap::iterator opt = mOptions.find( "Rendering Device" );
        if( opt != mOptions.end() )
        {
            D3D11Driver *driver = getDirect3DDrivers()->findByName(opt->second.currentValue);
            if (driver)
            {
                opt = mOptions.find( "Video Mode" );
                optVideoMode = &opt->second;
                optVideoMode->possibleValues.clear();
                // get vide modes for this device
                for( unsigned k=0; k < driver->getVideoModeList()->count(); k++ )
                {
                    videoMode = driver->getVideoModeList()->item( k );
                    optVideoMode->possibleValues.push_back( videoMode->getDescription() );
                }

                // Reset video mode to default if previous doesn't avail in new possible values
                StringList::const_iterator itValue =
                    std::find(optVideoMode->possibleValues.begin(),
                              optVideoMode->possibleValues.end(),
                              optVideoMode->currentValue);
                if (itValue == optVideoMode->possibleValues.end())
                {
                    if( optVideoMode->possibleValues.empty() )
                        optVideoMode->currentValue = "800 x 600 @ 32-bit colour";
                    else
                        optVideoMode->currentValue = optVideoMode->possibleValues.back();
                }

                // Also refresh FSAA options
                refreshFSAAOptions();
            }
        }

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setConfig( const String &name, const String &value )
    {
        initRenderSystem();

        LogManager::getSingleton().stream()
            << "D3D11: RenderSystem Option: " << name << " = " << value;

        bool viewModeChanged = false;

        // Find option
        ConfigOptionMap::iterator it = mOptions.find( name );

        // Update
        if( it != mOptions.end() )
            it->second.currentValue = value;
        else
        {
            StringStream str;
            str << "Option named '" << name << "' does not exist.";
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, str.str());
        }

        // Refresh other options if D3DDriver changed
        if( name == "Rendering Device" )
            refreshD3DSettings();

        if( name == "Full Screen" )
        {
            // Video mode is applicable
            it = mOptions.find( "Video Mode" );
            if (it->second.currentValue.empty())
            {
                it->second.currentValue = "800 x 600 @ 32-bit colour";
                viewModeChanged = true;
            }
        }

        if( name == "Min Requested Feature Levels" )
        {
            mMinRequestedFeatureLevel = D3D11Device::parseFeatureLevel(value, D3D_FEATURE_LEVEL_9_1);
        }

        if( name == "Max Requested Feature Levels" )
        {
#if defined(_WIN32_WINNT_WIN8) && _WIN32_WINNT >= _WIN32_WINNT_WIN8
        if( IsWindows8OrGreater() )
            mMaxRequestedFeatureLevel = D3D11Device::parseFeatureLevel(value, D3D_FEATURE_LEVEL_11_1);
        else
            mMaxRequestedFeatureLevel = D3D11Device::parseFeatureLevel(value, D3D_FEATURE_LEVEL_11_0);
#else
            mMaxRequestedFeatureLevel = D3D11Device::parseFeatureLevel(value, D3D_FEATURE_LEVEL_11_0);
#endif
        }

        if( name == "Allow NVPerfHUD" )
        {
            if (value == "Yes")
                mUseNVPerfHUD = true;
            else
                mUseNVPerfHUD = false;
        }

        if (viewModeChanged || name == "Video Mode")
        {
            refreshFSAAOptions();
        }

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::refreshFSAAOptions(void)
    {
        ConfigOptionMap::iterator it = mOptions.find( "FSAA" );
        ConfigOption* optFSAA = &it->second;
        optFSAA->possibleValues.clear();

        it = mOptions.find("Rendering Device");
        D3D11Driver *driver = getDirect3DDrivers()->findByName(it->second.currentValue);
        if (driver)
        {
            it = mOptions.find("Video Mode");
            ComPtr<ID3D11Device> device;
            createD3D11Device( mVendorExtension, "", driver, mDriverType,
                               mMinRequestedFeatureLevel, mMaxRequestedFeatureLevel,
                               NULL, device.GetAddressOf() );
            // 'videoMode' could be NULL if working over RDP/Simulator
            D3D11VideoMode* videoMode = driver->getVideoModeList()->item(it->second.currentValue);
            DXGI_FORMAT format = videoMode ? videoMode->getFormat() : DXGI_FORMAT_R8G8B8A8_UNORM;
            UINT numLevels = 0;
            // set maskable levels supported
            for( UINT n = 1; n <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; n++ )
            {
                // new style enumeration, with "8x CSAA", "8x MSAA" values
                if( n == 4 &&
                    SUCCEEDED( device->CheckMultisampleQualityLevels( format, 2, &numLevels ) ) &&
                    numLevels > 4 )  // 2f4x EQAA
                {
                    optFSAA->possibleValues.push_back( "2f4x EQAA" );
                }
                if( n == 8 &&
                    SUCCEEDED( device->CheckMultisampleQualityLevels( format, 4, &numLevels ) ) &&
                    numLevels > 8 )  // 8x CSAA
                {
                    optFSAA->possibleValues.push_back( "8x CSAA" );
                }
                if( n == 16 &&
                    SUCCEEDED( device->CheckMultisampleQualityLevels( format, 4, &numLevels ) ) &&
                    numLevels > 16 )  // 16x CSAA
                {
                    optFSAA->possibleValues.push_back( "16x CSAA" );
                }
                if( n == 16 &&
                    SUCCEEDED( device->CheckMultisampleQualityLevels( format, 8, &numLevels ) ) &&
                    numLevels > 16 )  // 16xQ CSAA
                {
                    optFSAA->possibleValues.push_back( "16xQ CSAA" );
                }
                if( SUCCEEDED( device->CheckMultisampleQualityLevels( format, n, &numLevels ) ) &&
                    numLevels > 0 )  // Nx MSAA
                {
                    optFSAA->possibleValues.push_back( StringConverter::toString( n ) + "x MSAA" );
                }
            }
        }

        if(optFSAA->possibleValues.empty())
        {
            optFSAA->possibleValues.push_back("1"); // D3D11 does not distinguish between noMSAA and 1xMSAA
        }

        // Reset FSAA to none if previous doesn't avail in new possible values
        StringList::const_iterator itValue =
            std::find(optFSAA->possibleValues.begin(),
                      optFSAA->possibleValues.end(),
                      optFSAA->currentValue);
        if (itValue == optFSAA->possibleValues.end())
        {
            optFSAA->currentValue = optFSAA->possibleValues[0];
        }
    }
    //---------------------------------------------------------------------
    String D3D11RenderSystem::checkConfig()
    {
        ConfigOptionMap::iterator it;

        // check if video mode is selected
        it = mOptions.find( "Video Mode" );
        if (it->second.currentValue.empty())
            return "A video mode must be selected.";

        it = mOptions.find( "Rendering Device" );
        String driverName = it->second.currentValue;
        if(driverName != "(default)" && getDirect3DDrivers()->findByName(driverName)->DriverDescription() != driverName)
        {
            // Just pick default driver
            setConfig("Rendering Device", "(default)");
            return "Requested rendering device could not be found, default would be used instead.";
        }

        return N_StrBlank;
    }
    //---------------------------------------------------------------------
    ConfigOptionMap& D3D11RenderSystem::getConfig()
    {
        // return a COPY of the current config options
        return mOptions;
    }
    //---------------------------------------------------------------------
    Window* D3D11RenderSystem::init( bool autoCreateWindow, const String& windowTitle )
    {
        Window* autoWindow = NULL;
        LogManager::getSingleton().logMessage( "D3D11: Subsystem Initialising" );

        if(IsWorkingUnderNsight())
            LogManager::getSingleton().logMessage( "D3D11: Nvidia Nsight found");

        // Init using current settings
        ConfigOptionMap::iterator opt = mOptions.find( "Rendering Device" );
        if( opt == mOptions.end() )
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "Can`t find requested Direct3D driver name!");
        mDriverName = opt->second.currentValue;

        // Driver type
        opt = mOptions.find( "Driver type" );
        if( opt == mOptions.end() )
            N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Can't find driver type!");
        mDriverType = D3D11Device::parseDriverType(opt->second.currentValue);

        opt = mOptions.find( "Information Queue Exceptions Bottom Level" );
        if( opt == mOptions.end() )
            N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Can't find Information Queue Exceptions Bottom Level option!");
        D3D11Device::setExceptionsErrorLevel(opt->second.currentValue);

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        // Stereo driver must be created before device is created
        StereoModeType stereoMode = StringConverter::parseStereoMode(mOptions["Stereo Mode"].currentValue);
        D3D11StereoDriverBridge* stereoBridge = OGRE_NEW D3D11StereoDriverBridge(stereoMode);
#endif

        // create the device for the selected adapter
        createDevice( windowTitle );

        if( autoCreateWindow )
        {
            bool fullScreen;
            opt = mOptions.find( "Full Screen" );
            if( opt == mOptions.end() )
                N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Can't find full screen option!");
            fullScreen = opt->second.currentValue == "Yes";

            D3D11VideoMode* videoMode = NULL;
            unsigned int width, height;
            String temp;

            opt = mOptions.find( "Video Mode" );
            if( opt == mOptions.end() )
                N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Can't find Video Mode option!");

            // The string we are manipulating looks like this :width x height @ colourDepth
            // Pull out the colour depth by getting what comes after the @ and a space
            String colourDepth = opt->second.currentValue.substr(opt->second.currentValue.rfind('@')+1);
            // Now we know that the width starts a 0, so if we can find the end we can parse that out
            String::size_type widthEnd = opt->second.currentValue.find(' ');
            // we know that the height starts 3 characters after the width and goes until the next space
            String::size_type heightEnd = opt->second.currentValue.find(' ', widthEnd+3);
            // Now we can parse out the values
            width = StringConverter::parseInt(opt->second.currentValue.substr(0, widthEnd));
            height = StringConverter::parseInt(opt->second.currentValue.substr(widthEnd+3, heightEnd));

            D3D11VideoModeList* videoModeList = mActiveD3DDriver.getVideoModeList();
            for( unsigned j=0; j < videoModeList->count(); j++ )
            {
                temp = videoModeList->item(j)->getDescription();

                // In full screen we only want to allow supported resolutions, so temp and
                // opt->second.currentValue need to match exactly, but in windowed mode we
                // can allow for arbitrary window sized, so we only need to match the
                // colour values
                if( (fullScreen && (temp == opt->second.currentValue)) ||
                    (!fullScreen && (temp.substr(temp.rfind('@')+1) == colourDepth)) )
                {
                    videoMode = videoModeList->item(j);
                    break;
                }
            }

            // sRGB window option
            bool hwGamma = false;
            opt = mOptions.find( "sRGB Gamma Conversion" );
            if( opt == mOptions.end() )
                N_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Can't find sRGB option!");
            hwGamma = opt->second.currentValue == "Yes";
            String fsaa;
            if( (opt = mOptions.find("FSAA")) != mOptions.end() )
                fsaa = opt->second.currentValue;

            if( !videoMode )
            {
                LogManager::getSingleton().logMessage(
                            "WARNING D3D11: Couldn't find requested video mode. Forcing 32bpp. "
                            "If you have two GPUs and you're rendering to the GPU that is not "
                            "plugged to the monitor you can then ignore this message.",
                            LML_CRITICAL );
            }

            PropertyData miscParams;
            miscParams["colourDepth"] = StringConverter::toString(videoMode ? videoMode->getColourDepth() : 32);
            miscParams["FSAA"] = fsaa;
            miscParams["useNVPerfHUD"] = StringConverter::toString(mUseNVPerfHUD);
            miscParams["gamma"] = StringConverter::toString(hwGamma);
            //miscParams["useFlipMode"] = StringConverter::toString(true);

            opt = mOptions.find("VSync");
            if (opt == mOptions.end())
                N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find VSync options!");
            bool vsync = (opt->second.currentValue == "Yes");
            miscParams["vsync"] = StringConverter::toString(vsync);

            opt = mOptions.find("VSync Interval");
            if (opt == mOptions.end())
                N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find VSync Interval options!");
            miscParams["vsyncInterval"] = opt->second.currentValue;

            autoWindow = this->createWindow( windowTitle, width, height,
                                                    fullScreen, &miscParams );

            // If we have 16bit depth buffer enable w-buffering.
            assert( autoWindow );
            if ( PixelUtil::getUnitSize( autoWindow->getFormat() ) * 8u == 16u )
            {
                mWBuffer = true;
            }
            else
            {
                mWBuffer = false;
            }
        }

        LogManager::getSingleton().logMessage("***************************************");
        LogManager::getSingleton().logMessage("*** D3D11: Subsystem Initialized OK ***");
        LogManager::getSingleton().logMessage("***************************************");

        // call superclass method
        RenderSystem::init( autoCreateWindow );
        this->fireDeviceEvent(&mDevice, "DeviceCreated");
        return autoWindow;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::reinit()
    {
        LogManager::getSingleton().logMessage( "D3D11: Reinitializing" );
        this->shutdown();
    //  this->initialise( true );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::shutdown()
    {
        RenderSystem::shutdown();

        mRenderSystemWasInited = false;

        mPrimaryWindow = NULL; // primary window deleted by base class.
        freeDevice();
        SAFE_DELETE( mDriverList );
        mActiveD3DDriver = D3D11Driver();
        mDevice.ReleaseAll();
        LogManager::getSingleton().logMessage("D3D11: Shutting down cleanly.");
        SAFE_DELETE( mHardwareBufferManager );
        SAFE_DELETE( mGpuProgramManager );
    }
    //---------------------------------------------------------------------
    Window* D3D11RenderSystem::createWindow( const String &name,
                                                    uint32 width, uint32 height, bool fullScreen,
                                                    const PropertyData *miscParams )
    {

        // Check we're not creating a secondary window when the primary
        // was fullscreen
        if (mPrimaryWindow && mPrimaryWindow->isFullMode() && fullScreen == false)
        {
            N_EXCEPT(Exception::ERR_INVALID_STATE, "Cannot create secondary windows not in full screen when the primary is full screen");
        }

        // Log a message
        StringStream ss;
        ss << "D3D11RenderSystem::createWindow \"" << name << "\", " <<
            width << "x" << height << " ";
        if (fullScreen)
            ss << "fullscreen ";
        else
            ss << "windowed ";
        if (miscParams)
        {
            ss << " miscParams: ";
            PropertyData::const_iterator it;
            for (it = miscParams->begin(); it != miscParams->end(); ++it)
            {
                ss << it->first << "=" << it->second << " ";
            }
            LogManager::getSingleton().logMessage(ss.str());
        }

        String msg;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        D3D11Window* win = new D3D11WindowHwnd(  name, width, height, fullScreen,
                                                 Texture::DepthFormat,
                                                 miscParams, mDevice, this );
#elif OGRE_PLATFORM == OGRE_PLATFORM_WINRT
        String windowType;
        if(miscParams)
        {
            // Get variable-length params
            PropertyData::const_iterator opt = miscParams->find("windowType");
            if(opt != miscParams->end())
                windowType = opt->second;
        }

        D3D11Window* win = NULL;
#if defined(_WIN32_WINNT_WINBLUE) && _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
        if(win == NULL && windowType == "SwapChainPanel")
            win = new D3D11WindowSwapChainPanel( name, width, height, fullScreen, 
                                                 Texture::DepthFormat,
                                                 miscParams, mDevice, this );
#endif // defined(_WIN32_WINNT_WINBLUE) && _WIN32_WINNT >= _WIN32_WINNT_WINBLUE
        if(win == NULL)
            win = new D3D11WindowCoreWindow(     name, width, height, fullScreen, 
                                                 Texture::DepthFormat,
                                                 miscParams, mDevice, this );
#endif

        mWindows.insert( win );

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        // Must be called after device has been linked to window
        D3D11StereoDriverBridge::getSingleton().addRenderWindow(win);
        win->_validateStereo();
#endif

        // If this is the first window, get the D3D device and create the texture manager
        if (!mPrimaryWindow)
        {
            mPrimaryWindow = win;
            //win->getPlatformData("D3DDEVICE", &mDevice);

            if( miscParams )
            {
                PropertyData::const_iterator itOption = miscParams->find( "reverse_depth" );
                if( itOption != miscParams->end() )
                    mReverseDepth = StringConverter::parseBool( itOption->second, true );
            }

            // Also create hardware buffer manager
            mHardwareBufferManager = new v1::D3D11HardwareBufferManager(mDevice);

            // Create the GPU program manager
            mGpuProgramManager = new D3D11GpuProgramManager();
            // create & register HLSL factory
            if (mHLSLProgramFactory == NULL)
                mHLSLProgramFactory = new D3D11HLSLProgramFactory(mDevice);
            mVendorFeature = setupFeature();

            // if we are using custom capabilities, then
            // mFeature has already been loaded
            if (mSysFeatureValid)
                mFeature = mVendorFeature;

            fireEvent("RenderSystemCapabilitiesCreated");

            initImpl(mFeature, mPrimaryWindow);

            assert( !mBufferManager );
            mBufferManager = OGRE_NEW D3D11VaoManager( false, mDevice, this, miscParams );

            mTextureManager = OGRE_NEW D3D11TextureGpuManager( mBufferManager, this, mDevice );

            mTextureManager->_update( true );
        }
        else
        {
            mSecondaryWindows.push_back(win);
        }

        win->_init( mTextureManager );

        return win;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::fireDeviceEvent( D3D11Device* device, const String & name,
                                             D3D11Window* sendingWindow /* = NULL */)
    {
        PropertyData params;
        params["D3DDEVICE"] =  StringConverter::toString((size_t)device->get());
        if(sendingWindow)
            params["Window"] = StringConverter::toString((size_t)sendingWindow);
        fireEvent( name, &params );
    }
    //---------------------------------------------------------------------
    RenderFeature* D3D11RenderSystem::setupFeature() const
    {
        RenderFeature* rsc = new RenderFeature();
        rsc->setDriverVersion(mVersion);
        rsc->setDeviceName(mActiveD3DDriver.DriverDescription());
        rsc->setRenderSystemName(getName());

        rsc->setCapability(RSC_HWSTENCIL);
        rsc->setStencilBufferBitDepth(8);

        rsc->setCapability(RSC_HW_GAMMA);
        rsc->setCapability(RSC_TEXTURE_SIGNED_INT);

#ifdef _WIN32_WINNT_WIN10
        //Check if D3D11.3 is installed. If so, typed UAV loads are supported
        ComPtr<ID3D11Device3> d3dDeviceVersion113;
        HRESULT hr = mDevice->QueryInterface( d3dDeviceVersion113.GetAddressOf() );
        if( SUCCEEDED( hr ) && d3dDeviceVersion113 )
        {
            rsc->setCapability(CF_Program_StorgeBuffer);
        }
#ifdef NTDDI_WIN10_TH2
        D3D11_FEATURE_DATA_D3D11_OPTIONS3 fOpt3 = {};
        hr = mDevice->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS3, &fOpt3, sizeof( fOpt3 ) );
        if( SUCCEEDED( hr ) && fOpt3.VPAndRTArrayIndexFromAnyShaderFeedingRasterizer )
        {
            rsc->setCapability( CF_Program_ViwportArray );
        }
#endif
#endif

        rsc->setCapability(RSC_VBO);
        UINT formatSupport;
        if( mFeatureLevel >= D3D_FEATURE_LEVEL_9_2 ||
            (SUCCEEDED( mDevice->CheckFormatSupport(DXGI_FORMAT_R32_UINT, &formatSupport)) &&
            0 != (formatSupport & D3D11_FORMAT_SUPPORT_IA_INDEX_BUFFER)) )
        {
            rsc->setCapability(RSC_32BIT_INDEX);
        }

        // Set number of texture units, always 16
        rsc->setMaxTexUnit(16);
        rsc->setCapability(RSC_ANISOTROPY);
        rsc->setCapability(RSC_AUTOMIPMAP);
        rsc->setCapability(RSC_BLENDING);
        rsc->setCapability(RSC_DOT3);
        // Cube map
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->setCapability(RSC_CUBEMAPPING);
            rsc->setCapability(RSC_READ_BACK_AS_TEXTURE);
        }

        rsc->setCapability(RSC_EXPLICIT_FSAA_RESOLVE);

        // We always support compression, D3DX will decompress if device does not support
        rsc->setCapability(RSC_TEXTURE_COMPRESSION);
        rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);

        if(mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            rsc->setCapability(RSC_TWO_SIDED_STENCIL);

        rsc->setCapability(RSC_STENCIL_WRAP);
        rsc->setCapability(RSC_HWOCCLUSION);
        rsc->setCapability(RSC_HWOCCLUSION_ASYNCHRONOUS);

        rsc->setCapability(RSC_TEXTURE_2D_ARRAY);

        if( mFeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
            rsc->setCapability(RSC_MSAA_2D_ARRAY);

        if( mFeatureLevel >= D3D_FEATURE_LEVEL_10_1 )
        {
            rsc->setCapability(CF_Tex_CubeArray);
            rsc->setCapability(GF_Tex_Gather);
        }

        if( mFeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
        {
            rsc->setMaxTexSize( static_cast<ushort>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION),
                                        static_cast<ushort>(D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION),
                                        static_cast<ushort>(D3D11_REQ_TEXTURECUBE_DIMENSION) );
        }
        else if( mFeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
        {
            rsc->setMaxTexSize( static_cast<ushort>(D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION),
                                        static_cast<ushort>(D3D10_REQ_TEXTURE3D_U_V_OR_W_DIMENSION),
                                        static_cast<ushort>(D3D10_REQ_TEXTURECUBE_DIMENSION) );
        }
        /*TODO
        else if( mFeatureLevel >= D3D_FEATURE_LEVEL_9_3 )
        {
            rsc->setMaxTexSize( static_cast<ushort>(D3D_FL9_3_REQ_TEXTURE2D_U_OR_V_DIMENSION),
                                        static_cast<ushort>(D3D_FL9_3_REQ_TEXTURE3D_U_V_OR_W_DIMENSION),
                                        static_cast<ushort>(D3D_FL9_3_REQ_TEXTURECUBE_DIMENSION) );
        }
        else
        {
            rsc->setMaxTexSize( static_cast<ushort>(D3D_FL9_1_REQ_TEXTURE2D_U_OR_V_DIMENSION),
                                        static_cast<ushort>(D3D_FL9_1_REQ_TEXTURE3D_U_V_OR_W_DIMENSION),
                                        static_cast<ushort>(D3D_FL9_1_REQ_TEXTURECUBE_DIMENSION) );
        }*/

        convertVertexShaderCaps(rsc);
        convertPixelShaderCaps(rsc);
        convertGeometryShaderCaps(rsc);
        convertHullShaderCaps(rsc);
        convertDomainShaderCaps(rsc);
        convertComputeShaderCaps(rsc);
        rsc->addShaderProfile("hlsl");

        // Check support for dynamic linkage
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->setCapability(RSC_SHADER_SUBROUTINE);
        }

        rsc->setCapability(RSC_USER_CLIP_PLANES);
        rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);

        rsc->setCapability(RSC_RTT_SEPARATE_DEPTHBUFFER);
        rsc->setCapability(RSC_RTT_MAIN_DEPTHBUFFER_ATTACHABLE);


        // Adapter details
        const DXGI_ADAPTER_DESC1& adapterID = mActiveD3DDriver.getAdapterIdentifier();

        switch(mDriverType) {
        case D3D_DRIVER_TYPE_HARDWARE:
            // determine vendor
            // Full list of vendors here: http://www.pcidatabase.com/vendors.php?sort=id
            switch(adapterID.VendorId)
            {
            case 0x10DE:
                rsc->setVendor(GPU_NVIDIA);
                break;
            case 0x1002:
                rsc->setVendor(GPU_AMD);
                break;
            case 0x163C:
            case 0x8086:
                rsc->setVendor(GPU_INTEL);
                break;
            case 0x5333:
                rsc->setVendor(GPU_S3);
                break;
            case 0x3D3D:
                rsc->setVendor(GPU_3DLABS);
                break;
            case 0x102B:
                rsc->setVendor(GPU_MATROX);
                break;
            default:
                rsc->setVendor(GPU_UNKNOWN);
                break;
            };
            break;
        case D3D_DRIVER_TYPE_SOFTWARE:
            rsc->setVendor(GPU_MS_SOFTWARE);
            break;
        case D3D_DRIVER_TYPE_WARP:
            rsc->setVendor(GPU_MS_WARP);
            break;
        default:
            rsc->setVendor(GPU_UNKNOWN);
            break;
        }

        rsc->setCapability(RSC_INFINITE_FAR_PLANE);

        rsc->setCapability(RSC_TEXTURE_3D);
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
            rsc->setCapability(RSC_HWRENDER_TO_TEXTURE_3D);
            rsc->setCapability(RSC_TEXTURE_1D);
            rsc->setCapability(RSC_TEXTURE_COMPRESSION_BC4_BC5);
            rsc->setCapability(RSC_COMPLETE_TEXTURE_BINDING);
        }

        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->setCapability(RSC_TEXTURE_COMPRESSION_BC6H_BC7);
            rsc->setCapability(CF_Program_StorgeBuffer);
        }

        rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
        rsc->setCapability(RSC_TEXTURE_FLOAT);

#ifdef D3D_FEATURE_LEVEL_9_3
        int numMultiRenderTargets = (mFeatureLevel > D3D_FEATURE_LEVEL_9_3) ? D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT :      // 8
                                    (mFeatureLevel == D3D_FEATURE_LEVEL_9_3) ? 4/*D3D_FL9_3_SIMULTANEOUS_RENDER_TARGET_COUNT*/ :    // 4
                                    1/*D3D_FL9_1_SIMULTANEOUS_RENDER_TARGET_COUNT*/;                                                // 1
#else
        int numMultiRenderTargets = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;     // 8
#endif

        rsc->setNumMultiRenderTargets(std::min(numMultiRenderTargets, (int)NII_MAX_RenderTarget));
        rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);

        rsc->setCapability(RSC_POINT_SPRITES);
        rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS);
        rsc->setMaxPointSize(256); // TODO: guess!

        rsc->setCapability(GF_Program_VertexTexFetch);
        rsc->setNumVertexTextureUnits(4);
        rsc->setVertexTextureUnitsShared(false);

        rsc->setCapability(RSC_MIPMAP_LOD_BIAS);

        // actually irrelevant, but set
        rsc->setCapability(RSC_PERSTAGECONSTANT);

        rsc->setCapability(RSC_VERTEX_BUFFER_INSTANCE_DATA);
        rsc->setCapability(RSC_CAN_GET_COMPILED_SHADER_BUFFER);
        rsc->setCapability(RSC_SEPARATE_SAMPLERS_FROM_TEXTURES);

        rsc->setCapability(CF_Program_ConstBuffer);

        return rsc;

    }
    //-----------------------------------------------------------------------
    void D3D11RenderSystem::initImpl(
        RenderFeature* caps, Window* primary)
    {
        if(caps->getRenderSystemName() != getName())
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Trying to initialize D3D11RenderSystem from RenderFeature that do not support Direct3D11");
        }

        // add hlsl
        HighLevelGpuProgramManager::getSingleton().addFactory(mHLSLProgramFactory);

        Log* defaultLog = LogManager::getSingleton().getDefaultLog();
        if (defaultLog)
        {
            caps->log(defaultLog);
            defaultLog->logMessage(
                " * Using Reverse Z: " + StringConverter::toString( mReverseDepth, true ) );
        }

        mGpuProgramManager->setSaveMicrocodesToCache(true);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertVertexShaderCaps(RenderFeature* rsc) const
    {
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_9_1)
        {
            rsc->addShaderProfile("vs_4_0_level_9_1");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("vs_2_0");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_9_3)
        {
            rsc->addShaderProfile("vs_4_0_level_9_3");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("vs_2_a");
            rsc->addShaderProfile("vs_2_x");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->addShaderProfile("vs_4_0");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("vs_3_0");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
        {
            rsc->addShaderProfile("vs_4_1");
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("vs_5_0");
        }

        rsc->setCapability(RSC_VERTEX_PROGRAM);

        // TODO: constant buffers have no limits but lower models do
        // 16 boolean params allowed
        rsc->setVertexProgramConstantBoolCount(16);
        // 16 integer params allowed, 4D
        rsc->setVertexProgramConstantIntCount(16);
        // float params, always 4D
        rsc->setVertexProgramConstantFloatCount(512);

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertPixelShaderCaps(RenderFeature* rsc) const
    {
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_9_1)
        {
            rsc->addShaderProfile("ps_4_0_level_9_1");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("ps_2_0");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_9_3)
        {
            rsc->addShaderProfile("ps_4_0_level_9_3");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("ps_2_a");
            rsc->addShaderProfile("ps_2_b");
            rsc->addShaderProfile("ps_2_x");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->addShaderProfile("ps_4_0");
#if SUPPORT_SM2_0_HLSL_SHADERS == 1
            rsc->addShaderProfile("ps_3_0");
            rsc->addShaderProfile("ps_3_x");
#endif
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
        {
            rsc->addShaderProfile("ps_4_1");
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("ps_5_0");
        }


        rsc->setCapability(RSC_FRAGMENT_PROGRAM);


        // TODO: constant buffers have no limits but lower models do
        // 16 boolean params allowed
        rsc->setFragmentProgramConstantBoolCount(16);
        // 16 integer params allowed, 4D
        rsc->setFragmentProgramConstantIntCount(16);
        // float params, always 4D
        rsc->setFragmentProgramConstantFloatCount(512);

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertHullShaderCaps(RenderFeature* rsc) const
    {
        // Only for shader model 5.0
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("hs_5_0");

            rsc->setCapability(RSC_TESSELLATION_HULL_PROGRAM);

            // TODO: constant buffers have no limits but lower models do
            // 16 boolean params allowed
            rsc->setTessellationHullProgramConstantBoolCount(16);
            // 16 integer params allowed, 4D
            rsc->setTessellationHullProgramConstantIntCount(16);
            // float params, always 4D
            rsc->setTessellationHullProgramConstantFloatCount(512);
        }

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertDomainShaderCaps(RenderFeature* rsc) const
    {
        // Only for shader model 5.0
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("ds_5_0");

            rsc->setCapability(RSC_TESSELLATION_DOMAIN_PROGRAM);


            // TODO: constant buffers have no limits but lower models do
            // 16 boolean params allowed
            rsc->setTessellationDomainProgramConstantBoolCount(16);
            // 16 integer params allowed, 4D
            rsc->setTessellationDomainProgramConstantIntCount(16);
            // float params, always 4D
            rsc->setTessellationDomainProgramConstantFloatCount(512);
        }

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertComputeShaderCaps(RenderFeature* rsc) const
    {

//        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
//        {
//            rsc->addShaderProfile("cs_4_0");
//        }
//        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
//        {
//            rsc->addShaderProfile("cs_4_1");
//        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("cs_5_0");
            rsc->setCapability(RSC_COMPUTE_PROGRAM);
        }

        // TODO: constant buffers have no limits but lower models do
        // 16 boolean params allowed
        rsc->setComputeProgramConstantBoolCount(16);
        // 16 integer params allowed, 4D
        rsc->setComputeProgramConstantIntCount(16);
        // float params, always 4D
        rsc->setComputeProgramConstantFloatCount(512);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::convertGeometryShaderCaps(RenderFeature* rsc) const
    {
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
        {
            rsc->addShaderProfile("gs_4_0");
            rsc->setCapability(RSC_GEOMETRY_PROGRAM);
            rsc->setCapability(RSC_HWRENDER_TO_VERTEX_BUFFER);
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
        {
            rsc->addShaderProfile("gs_4_1");
        }
        if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
        {
            rsc->addShaderProfile("gs_5_0");
        }

        rsc->setGeometryProgramConstantFloatCount(512);
        rsc->setGeometryProgramConstantIntCount(16);
        rsc->setGeometryProgramConstantBoolCount(16);
        rsc->setGeometryProgramNumOutputVertices(1024);
    }
    //-----------------------------------------------------------------------
    bool D3D11RenderSystem::checkVertexTextureFormats(void)
    {
        return true;
    }
    //-----------------------------------------------------------------------------------
    FrameBufferObject* D3D11RenderSystem::createFBO(void)
    {
        FrameBufferObject * fbo = OGRE_NEW D3D11RenderPassDescriptor( mDevice, this );
        mFBOList.insert(fbo);
        return fbo;
    }
    //-----------------------------------------------------------------------------------
    void D3D11RenderSystem::beginFBO(FrameBufferObject * fbo, const ViewportList & vlist)
    {
        bool warnIfRtvWasFlushed = false;
        if( fbo->mStepping && fbo->isTargetEqual(mActiveFrame2))
            return;

        const int oldWidth = mActiveViewport[0].getPixelWidth();
        const int oldHeight = mActiveViewport[0].getPixelHeight();
        const int oldX = mActiveViewport[0].getPixelLeft();
        const int oldY = mActiveViewport[0].getPixelTop();

        D3D11RenderPassDescriptor * currPassDesc = static_cast<D3D11RenderPassDescriptor*>( mActiveFrame2 );

        RenderSystem::beginFBO(fbo, vlist);

        int x, y, w, h;

        // Calculate the new "lower-left" corner of the viewport to compare with the old one
        w = mActiveViewport[0].getPixelWidth();
        h = mActiveViewport[0].getPixelHeight();
        x = mActiveViewport[0].getPixelLeft();
        y = mActiveViewport[0].getPixelTop();

        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();

        const bool vpChanged = oldX != x || oldY != y || oldWidth != w || oldHeight != h;

        D3D11RenderPassDescriptor * newPassDesc = static_cast<D3D11RenderPassDescriptor*>(fbo);

        //Determine whether:
        //  1. We need to store current active FrameBufferObject
        //  2. We need to perform clears when loading the new FrameBufferObject
        uint32 entriesToFlush = 0;
        if( currPassDesc )
        {
            entriesToFlush = currPassDesc->willSwitchTo( newPassDesc, warnIfRtvWasFlushed );

            if( entriesToFlush != 0 )
            {
                currPassDesc->performStoreActions( entriesToFlush );

                //Set all textures to 0 to prevent the runtime from thinkin we might
                //be sampling from the render target (common when doing shadow map
                //rendering)
                context->VSSetShaderResources( 0, mMaxSrvCount[ST_VS],   mNullViews );
                context->PSSetShaderResources( 0, mMaxSrvCount[ST_FS],    mNullViews );
                context->HSSetShaderResources( 0, mMaxSrvCount[ST_TS],     mNullViews );
                context->DSSetShaderResources( 0, mMaxSrvCount[ST_DS],   mNullViews );
                context->GSSetShaderResources( 0, mMaxSrvCount[ST_GS], mNullViews );
                context->CSSetShaderResources( 0, mMaxComputeShaderSrvCount,    mNullViews );
                memset( mMaxSrvCount, 0, sizeof(mMaxSrvCount) );
                mMaxComputeShaderSrvCount = 0;
            }
        }
        else
        {
            entriesToFlush = FrameBufferObject::TT_All;
        }
        int numViewports = vlist.size();
        if( vpChanged || numViewports > 1u )
        {
            D3D11_VIEWPORT d3dVp[16];
            for( size_t i=0; i<numViewports; ++i )
            {
                d3dVp[i].TopLeftX= static_cast<FLOAT>( mActiveViewport[i].getPixelLeft() );
                d3dVp[i].TopLeftY= static_cast<FLOAT>( mActiveViewport[i].getPixelTop() );
                d3dVp[i].Width   = static_cast<FLOAT>( mActiveViewport[i].getPixelWidth() );
                d3dVp[i].Height  = static_cast<FLOAT>( mActiveViewport[i].getPixelHeight() );
                d3dVp[i].MinDepth= 0.0f;
                d3dVp[i].MaxDepth= 1.0f;
            }
            context->RSSetViewports( numViewports, d3dVp );
        }

        D3D11_RECT scRc[16];
        for( size_t i=0; i<numViewports; ++i )
        {
            scRc[i].left  = mActiveViewport[i].getClipPixelLeft();
            scRc[i].top   = mActiveViewport[i].getClipPixelTop();
            scRc[i].right = scRc[i].left + mActiveViewport[i].getClipPixelWidth();
            scRc[i].bottom= scRc[i].top + mActiveViewport[i].getClipPixelHeight();
        }
        context->RSSetScissorRects( numViewports, scRc );

        newPassDesc->performLoadActions( &mActiveViewport[0], entriesToFlush, mUavStartingSlot, mGpuParamBuffer );
    }
    //-----------------------------------------------------------------------------------
    void D3D11RenderSystem::endFBO(void)
    {
        if( mActiveFrame2 )
        {
            D3D11RenderPassDescriptor * passDesc = static_cast<D3D11RenderPassDescriptor*>( mActiveFrame2 );
            passDesc->performStoreActions( FrameBufferObject::TT_All );
        }

        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();
        //Set all textures to 0 to prevent the runtime from thinkin we might
        //be sampling from the render target (common when doing shadow map
        //rendering)
        context->VSSetShaderResources( 0, mMaxSrvCount[ST_VS],   mNullViews );
        context->PSSetShaderResources( 0, mMaxSrvCount[ST_FS],    mNullViews );
        context->HSSetShaderResources( 0, mMaxSrvCount[ST_TS],     mNullViews );
        context->DSSetShaderResources( 0, mMaxSrvCount[ST_DS],   mNullViews );
        context->GSSetShaderResources( 0, mMaxSrvCount[ST_GS], mNullViews );
        context->CSSetShaderResources( 0, mMaxComputeShaderSrvCount,    mNullViews );
        memset( mMaxSrvCount, 0, sizeof(mMaxSrvCount) );
        mMaxComputeShaderSrvCount = 0;

        RenderSystem::endFBO();
    }
    //-----------------------------------------------------------------------------------
    Texture* D3D11RenderSystem::createDepthImpl( Texture * colourTexture, bool preferDepthTexture, PixelFormat depthBufferFormat )
    {
        if( depthBufferFormat == PF_UNKNOWN )
        {
            //GeForce 8 & 9 series are faster using 24-bit depth buffers. Likely
            //other HW from that era has the same issue. Assume D3D10.1 is old
            //HW that prefers 24-bit.
            depthBufferFormat = Texture::DepthFormat;
        }

        return RenderSystem::createDepthImpl( colourTexture, preferDepthTexture, depthBufferFormat );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_notifyWindowDestroyed( Window *window )
    {
        // Check in specialized lists
        if( mPrimaryWindow == window )
        {
            // We're destroying the primary window, so reset device and window
            mPrimaryWindow = NULL;
        }
    }
    //-----------------------------------------------------------------------
    void D3D11RenderSystem::freeDevice(void)
    {
        if (!mDevice.isNull() && mFeature)
        {
            // Clean up depth stencil surfaces
            mDevice.ReleaseAll();
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createDevice( const String &windowTitle )
    {
        mDevice.ReleaseAll();

        mLastWindowTitlePassedToExtensions = windowTitle;

        D3D11Driver *d3dDriver = getDirect3DDrivers(true)->findByName( mDriverName );
        mActiveD3DDriver = *d3dDriver; // store copy of selected driver, so that it is not
                                       //lost when drivers would be re-enumerated
        LogManager::getSingleton().stream() << "D3D11: Requested \"" << mDriverName <<
                                               "\", selected \"" <<
                                               d3dDriver->DriverDescription() << "\"";

        if( D3D11Driver *nvPerfHudDriver = (mDriverType == D3D_DRIVER_TYPE_HARDWARE && mUseNVPerfHUD) ?
            getDirect3DDrivers()->item("NVIDIA PerfHUD") : NULL )
        {
            d3dDriver = nvPerfHudDriver;
            LogManager::getSingleton().logMessage("D3D11: Actually \"NVIDIA PerfHUD\" is used");
        }

        ComPtr<ID3D11Device> device;
        createD3D11Device( mVendorExtension, windowTitle, d3dDriver, mDriverType,
                           mMinRequestedFeatureLevel, mMaxRequestedFeatureLevel, &mFeatureLevel,
                           device.GetAddressOf() );
        mDevice.TransferOwnership( device );

        LARGE_INTEGER driverVersion = mDevice.GetDriverVersion();
        mVersion.major = HIWORD(driverVersion.HighPart);
        mVersion.minor = LOWORD(driverVersion.HighPart);
        mVersion.release = HIWORD(driverVersion.LowPart);
        mVersion.build = LOWORD(driverVersion.LowPart);

        //On AMD's GCN cards, there is no performance or memory difference between
        //PF_D24_UNORM_S8_UINT & PF_D32_FLOAT_X24_S8_UINT, so prefer the latter
        //on modern cards (GL >= 4.3) and that also claim to support this format.
        //NVIDIA's preference? Dunno, they don't tell. But at least the quality
        //will be consistent.
        if( mFeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
            Texture::DepthFormat = PF_Depth32f_Stencil8ui_64;
    }
    //-----------------------------------------------------------------------
    void D3D11RenderSystem::handleDeviceLost()
    {
        LogManager::getSingleton().logMessage("D3D11: Device was lost, recreating.");

        // release device depended resources
        fireDeviceEvent(&mDevice, "DeviceLost");

        SceneManagerEnumerator::SceneManagerIterator scnIt = SceneManagerEnumerator::getSingleton().getSceneManagerIterator();
        while(scnIt.hasMoreElements())
            scnIt.getNext()->_releaseManualHardwareResources();

        notifyDeviceLost(&mDevice);

        // Release all automatic temporary buffers and free unused
        // temporary buffers, so we doesn't need to recreate them,
        // and they will reallocate on demand.
        v1::HardwareBufferManager::getSingleton()._releaseBufferCopies(true);

        // recreate device
        createDevice( mLastWindowTitlePassedToExtensions );

        // recreate device depended resources
        notifyDeviceRestored(&mDevice);

        v1::MeshManager::getSingleton().reloadAll(Resource::LF_PRESERVE_STATE);

        scnIt = SceneManagerEnumerator::getSingleton().getSceneManagerIterator();
        while(scnIt.hasMoreElements())
            scnIt.getNext()->_restoreManualHardwareResources();

        fireDeviceEvent(&mDevice, "DeviceRestored");

        LogManager::getSingleton().logMessage("D3D11: Device was restored.");
    }
    //---------------------------------------------------------------------
    bool D3D11RenderSystem::checkDevice(bool forceDeviceElection)
    {
        if(mDevice.isNull())
            return false;

        // The D3D Device is no longer valid if the elected adapter changes or if
        // the device has been removed.

        bool anotherIsElected = false;
        if(forceDeviceElection)
        {
            // elect new device
            D3D11Driver* newDriver = getDirect3DDrivers(true)->findByName(mDriverName);

            // check by LUID
            LUID newLUID = newDriver->getAdapterIdentifier().AdapterLuid;
            LUID prevLUID = mActiveD3DDriver.getAdapterIdentifier().AdapterLuid;
            anotherIsElected = (newLUID.LowPart != prevLUID.LowPart) || (newLUID.HighPart != prevLUID.HighPart);
        }

        if(anotherIsElected || mDevice.IsDeviceLost())
        {
            handleDeviceLost();

            return !mDevice.IsDeviceLost();
        }

        return true;
    }
    //---------------------------------------------------------------------
    DataType D3D11RenderSystem::getColourType(void) const
    {
        return VET_COLOUR_ABGR;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setLight(const LightList& lights, unsigned short limit)
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setShadingType( ShadeOptions so )
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setLightingEnabled( bool enabled )
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setView( const Matrix4 &m )
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setProj( const Matrix4 &m )
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setModel( const Matrix4 &m )
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setColour( const Colour &ambient, const Colour &diffuse,
        const Colour &specular, const Colour &emissive, Real shininess,
        TrackVertexColourType tracking )
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setPoint(Real size,
        bool attenuationEnabled, Real constant, Real linear, Real quadratic,
        Real minSize, Real maxSize)
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setPoint(bool enabled)
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_bindTexture( size_t stage, Texture * texPtr )
    {
        if( texPtr )
        {
            D3D11TextureGpu *tex = static_cast<D3D11TextureGpu*>( texPtr );
            ID3D11ShaderResourceView *view = tex->getDefaultDisplaySrv();
            mDevice.GetImmediateContext()->VSSetShaderResources( static_cast<UINT>(stage), 1u, &view );
            mDevice.GetImmediateContext()->PSSetShaderResources( static_cast<UINT>(stage), 1u, &view );
            mMaxSrvCount[ST_VS]  = std::max<uint32>( mMaxSrvCount[ST_VS], stage + 1u );
            mMaxSrvCount[ST_FS]   = std::max<uint32>( mMaxSrvCount[ST_FS], stage + 1u );
        }
        else
        {
            ID3D11ShaderResourceView *nullView = 0;
            mDevice.GetImmediateContext()->VSSetShaderResources( static_cast<UINT>(stage), 1u, &nullView );
            mDevice.GetImmediateContext()->PSSetShaderResources( static_cast<UINT>(stage), 1u, &nullView );
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTexture( uint32 slotStart, const DescriptorSetTexture *set, uint32 hazardousTexIdx )
    {
        ID3D11DeviceContextN * context = mDevice.GetImmediateContext();
        ComPtr<ID3D11ShaderResourceView> * srvList = reinterpret_cast<ComPtr<ID3D11ShaderResourceView>*>( set->mRsData );

        ComPtr<ID3D11ShaderResourceView> hazardousSrv;
        if( hazardousTexIdx < set->mTextures.size() )
        {
            //Is the texture currently bound as RTT?
            if( mActiveFrame2->isTarget( set->mTextures[hazardousTexIdx] ) )
            {
                //Then do not set it!
                srvList[hazardousTexIdx].Swap( hazardousSrv );
            }
        }

        UINT texIdx = 0;
        for( size_t i=0u; i<ST_PCnt; ++i )
        {
            const UINT numTexturesUsed = set->mShaderTypeTexCount[i];
            if( !numTexturesUsed )
                continue;

            switch( i )
            {
            case ST_VS:
                context->VSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf() );
                break;
            case ST_FS:
                context->PSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf());
                break;
            case ST_GS:
                context->GSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf());
                break;
            case ST_TS:
                context->HSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf());
                break;
            case ST_DS:
                context->DSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf());
                break;
            }

            mMaxSrvCount[i] = std::max( mMaxSrvCount[i], slotStart + texIdx + numTexturesUsed );

            texIdx += numTexturesUsed;
        }

        //Restore the SRV with the hazardous texture.
        if( hazardousSrv )
            srvList[hazardousTexIdx].Swap( hazardousSrv );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTexture( uint32 slotStart, const ShaderChTexture *set )
    {
        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();
        ComPtr<ID3D11ShaderResourceView> *srvList = reinterpret_cast<ComPtr<ID3D11ShaderResourceView>*>( set->mRsData );
        UINT texIdx = 0;
        for( size_t i=0u; i<ST_PCnt; ++i )
        {
            const UINT numTexturesUsed = set->mShaderTypeTexCount[i];
            if( !numTexturesUsed )
                continue;

            switch( i )
            {
            case ST_VS:
                context->VSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf() );
                break;
            case ST_FS:
                context->PSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf() );
                break;
            case ST_GS:
                context->GSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf() );
                break;
            case ST_TS:
                context->HSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf() );
                break;
            case ST_DS:
                context->DSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf() );
                break;
            }

            mMaxSrvCount[i] = std::max( mMaxSrvCount[i], slotStart + texIdx + numTexturesUsed );

            texIdx += numTexturesUsed;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setSampler( uint32 slotStart, const TextureSample *set )
    {
        ID3D11SamplerState *samplers[32];

        vectorL<const TextureSampleUnit*>::const_iterator itor = set->mSamplers.begin();

        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();
        UINT samplerIdx = slotStart;
        for( size_t i=0u; i<ST_PCnt; ++i )
        {
            const UINT numSamplersUsed = set->mShaderTypeSamplerCount[i];

            if( !numSamplersUsed )
                continue;

            for( size_t j=0; j<numSamplersUsed; ++j )
            {
                if( *itor )
                {
                    ID3D11SamplerState *samplerState = reinterpret_cast<ID3D11SamplerState*>( (*itor)->mRsData );
                    samplers[j] = samplerState;
                }
                else
                {
                    samplers[j] = 0;
                }
                ++itor;
            }

            switch( i )
            {
            case ST_VS:
                context->VSSetSamplers( samplerIdx, numSamplersUsed, samplers );
                break;
            case ST_FS:
                context->PSSetSamplers( samplerIdx, numSamplersUsed, samplers );
                break;
            case ST_GS:
                context->GSSetSamplers( samplerIdx, numSamplersUsed, samplers );
                break;
            case ST_TS:
                context->HSSetSamplers( samplerIdx, numSamplersUsed, samplers );
                break;
            case ST_DS:
                context->DSSetSamplers( samplerIdx, numSamplersUsed, samplers );
                break;
            }

            samplerIdx += numSamplersUsed;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTextureCS( uint32 slotStart, const DescriptorSetTexture *set )
    {
        const uint32 oldSrvCount = mMaxComputeShaderSrvCount;
        uint32 newSrvCount = 0;

        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();
        ComPtr<ID3D11ShaderResourceView> *srvList = reinterpret_cast<ComPtr<ID3D11ShaderResourceView>*>( set->mRsData );
        UINT texIdx = 0;
        for( size_t i=0u; i<ST_PCnt; ++i )
        {
            const UINT numTexturesUsed = set->mShaderTypeTexCount[i];
            if( !numTexturesUsed )
                continue;

            context->CSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf() );

            mMaxComputeShaderSrvCount = std::max( mMaxComputeShaderSrvCount, slotStart + texIdx + numTexturesUsed );
            texIdx += numTexturesUsed;
        }

        //We must unbound old textures otherwise they could clash with the next _setBufferCS call
        if( newSrvCount < oldSrvCount )
        {
            const uint32 excessSlots = oldSrvCount - newSrvCount;
            context->CSSetShaderResources( newSrvCount, excessSlots, mNullViews );
        }

        mMaxComputeShaderSrvCount = newSrvCount;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTextureCS( uint32 slotStart, const ShaderChTexture *set )
    {
        const uint32 oldSrvCount = mMaxComputeShaderSrvCount;
        uint32 newSrvCount = 0;

        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();
        ComPtr<ID3D11ShaderResourceView> *srvList = reinterpret_cast<ComPtr<ID3D11ShaderResourceView>*>( set->mRsData );
        UINT texIdx = 0;
        for( size_t i=0u; i<ST_PCnt; ++i )
        {
            const UINT numTexturesUsed = set->mShaderTypeTexCount[i];
            if( !numTexturesUsed )
                continue;

            context->CSSetShaderResources( slotStart + texIdx, numTexturesUsed, srvList[texIdx].GetAddressOf() );

            newSrvCount = std::max( newSrvCount, slotStart + texIdx + numTexturesUsed );
            texIdx += numTexturesUsed;
        }

        //We must unbound old textures otherwise they could clash with the next _setBufferCS call
        if( newSrvCount < oldSrvCount )
        {
            const uint32 excessSlots = oldSrvCount - newSrvCount;
            context->CSSetShaderResources( newSrvCount, excessSlots, mNullViews );
        }

        mMaxComputeShaderSrvCount = newSrvCount;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setSamplerCS( uint32 slotStart, const TextureSample *set )
    {
        ID3D11SamplerState *samplers[32];

        vectorL<const TextureSampleUnit*>::const_iterator itor = set->mSamplers.begin();

        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();
        UINT samplerIdx = slotStart;
        for( size_t i=0u; i<ST_PCnt; ++i )
        {
            const UINT numSamplersUsed = set->mShaderTypeSamplerCount[i];

            if( !numSamplersUsed )
                continue;

            for( size_t j=0; j<numSamplersUsed; ++j )
            {
                if( *itor )
                {
                    ID3D11SamplerState *samplerState =
                            reinterpret_cast<ID3D11SamplerState*>( (*itor)->mRsData );
                    samplers[j] = samplerState;
                }
                else
                {
                    samplers[j] = 0;
                }
                ++itor;
            }

            context->CSSetSamplers( samplerIdx, numSamplersUsed, samplers );
            samplerIdx += numSamplersUsed;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setBufferCS( uint32 slotStart, const GpuParamBuffer * set )
    {
        ComPtr<ID3D11UnorderedAccessView> *uavList = reinterpret_cast<ComPtr<ID3D11UnorderedAccessView>*>( set->mRsData );
        ID3D11DeviceContextN *context = mDevice.GetImmediateContext();
        context->CSSetUnorderedAccessViews( slotStart, static_cast<UINT>( set->mUavs.size() ), uavList[0].GetAddressOf(), 0 );

        mMaxBoundUavCS = std::max<uint32>( mMaxBoundUavCS, slotStart + set->mUavs.size() );
    }
    //---------------------------------------------------------------------
    /*void D3D11RenderSystem::_setBindingType(ShaderChTextureUnit::BindingType bindingType)
    {
        mBindingType = bindingType;
    }*/
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_bindVertexTexture(size_t stage, Texture *tex)
    {
        _bindTexture(stage, tex);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_bindGeometryTexture(size_t stage, Texture *tex)
    {
        _bindTexture(stage, tex);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_bindTessControlTexture(size_t stage, Texture *tex)
    {
        _bindTexture(stage, tex);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_bindDomainTexture(size_t stage, Texture *tex)
    {
        _bindTexture(stage, tex);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTextureCoordPrc( size_t stage, TextureCoordPrc m,
        const Frustum* frustum)
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTextureMatrix( size_t stage, const Matrix4& xForm )
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTextureBlend( size_t stage, const TextureBlend& bm )
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setFog( FogMode mode, const Colour& colour, Real densitiy, Real start, Real end )
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setStencil( uint32 refValue, const ShaderChStencil &stencil )
    {
        RenderSystem::setStencil( refValue, stencil );

        mStencilRef = refValue;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createRSO( RenderStateObject *block )
    {
        D3D11HlmsPso *pso = new D3D11HlmsPso();
        memset( pso, 0, sizeof(D3D11HlmsPso) );

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

        ZeroMemory( &depthStencilDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
        depthStencilDesc.DepthEnable        = block->mStencil->mDepthCheck;
        depthStencilDesc.DepthWriteMask     =
                block->mStencil->mDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL :
                                                  D3D11_DEPTH_WRITE_MASK_ZERO;
        CmpMode depthFunc = block->mStencil->mDepthFunc;
        if( mReverseDepth )
            depthFunc = reverse( depthFunc );
        depthStencilDesc.DepthFunc          = D3D11Mappings::get( depthFunc );
        depthStencilDesc.StencilEnable      = block->mSampleObject.mStencil.enabled;
        depthStencilDesc.StencilReadMask    = block->mSampleObject.mStencil.readMask;
        depthStencilDesc.StencilWriteMask   = block->mSampleObject.mStencil.writeMask;
        const StencilStateOp &stateFront = block->mSampleObject.mStencil.stencilFront;
        depthStencilDesc.FrontFace.StencilFunc          = D3D11Mappings::get( stateFront.compareOp );
        depthStencilDesc.FrontFace.StencilDepthFailOp   = D3D11Mappings::get( stateFront.stencilDepthFailOp );
        depthStencilDesc.FrontFace.StencilPassOp        = D3D11Mappings::get( stateFront.stencilPassOp );
        depthStencilDesc.FrontFace.StencilFailOp        = D3D11Mappings::get( stateFront.stencilFailOp );
        const StencilStateOp &stateBack = block->mSampleObject.mStencil.stencilBack;
        depthStencilDesc.BackFace.StencilFunc           = D3D11Mappings::get( stateBack.compareOp );
        depthStencilDesc.BackFace.StencilDepthFailOp    = D3D11Mappings::get( stateBack.stencilDepthFailOp );
        depthStencilDesc.BackFace.StencilPassOp         = D3D11Mappings::get( stateBack.stencilPassOp );
        depthStencilDesc.BackFace.StencilFailOp         = D3D11Mappings::get( stateBack.stencilFailOp );

        HRESULT hr = mDevice->CreateDepthStencilState( &depthStencilDesc, pso->depthStencilState.GetAddressOf() );
        if( FAILED(hr) )
        {
            delete pso;
            pso = 0;

            String errorDescription = mDevice.getErrorString(hr);
            N_EXCEPT_EX(Exception::ERR_RENDERINGAPI_ERROR, hr,
                "Failed to create depth stencil state\nError Description: " + errorDescription,
                "D3D11RenderSystem::createRSO" );
        }

        const bool useTesselation = !block->mDS.isNull();
        const bool useAdjacency   = !block->mGS.isNull() &&
                                    block->mGS->isAdjacencyPrimitive();

        switch( block->mType )
        {
        case OT_POINT_LIST:
            pso->topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        case OT_LINE_LIST:
            if( useTesselation )
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
            else if( useAdjacency )
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
            else
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case OT_LINE_STRIP:
            if( useTesselation )
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
            else if( useAdjacency )
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
            else
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
            break;
        default:
        case OT_TRIANGLE_LIST:
            if( useTesselation )
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
            else if( useAdjacency )
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
            else
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        case OT_TRIANGLE_STRIP:
            if( useTesselation )
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
            else if( useAdjacency )
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
            else
                pso->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            break;
        case OT_TRIANGLE_FAN:
            pso->topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

            delete pso;
            pso = 0;
            N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Error - DX11 render - no support for triangle fan (OT_TRIANGLE_FAN)");
            break;
        }

        //No subroutines for now
        if( !block->mVS.isNull() )
        {
            pso->mVS = static_cast<D3D11HLSLProgram*>( block->mVS->
                                                                getMain() );
        }
        if( !block->mGS.isNull() )
        {
            pso->mGS = static_cast<D3D11HLSLProgram*>( block->mGS->
                                                                  getMain() );
        }
        if( mFeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
        {
            if( !block->mTS.isNull() )
            {
                pso->mTS = static_cast<D3D11HLSLProgram*>( block->mTS->
                                                                  getMain() );
            }
            if( !block->mDS.isNull() )
            {
                pso->mDS = static_cast<D3D11HLSLProgram*>( block->mDS->
                                                                    getMain() );
            }

            // Check consistency of tessellation shaders
            if( block->mTS.isNull() != block->mDS.isNull() )
            {
                delete pso;
                pso = 0;
                if( block->mTS.isNull() )
                {
                    N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Attempted to use tessellation, but domain shader is missing");
                }
                else
                {
                    N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Attempted to use tessellation, but hull shader is missing");
                }
            }
        }
        if( !block->mFS.isNull() )
        {
            pso->mFS = static_cast<D3D11HLSLProgram*>( block->mFS->
                                                               getMain() );
        }

        if( pso->mVS )
        {
            try
            {
                pso->inputLayout = pso->mVS->getLayoutForPso( block->mUnitsList );
            }
            catch( Exception& )
            {
                delete pso;
                pso = 0;
                throw;
            }
        }

        block->mExtData = pso;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::destroyRSO( RenderStateObject *pso )
    {
        D3D11HlmsPso *d3dPso = reinterpret_cast<D3D11HlmsPso*>( pso->mExtData );
        delete d3dPso;
        pso->mExtData = 0;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createStencil( ShaderChStencil *newBlock )
    {
        D3D11_RASTERIZER_DESC rasterDesc;
        switch( newBlock->mCullMode )
        {
        case CM_None:
            rasterDesc.CullMode = D3D11_CULL_NONE;
            break;
        default:
        case CM_Clockwise:
            rasterDesc.CullMode = D3D11_CULL_BACK;
            break;
        case CM_Anticlockwise:
            rasterDesc.CullMode = D3D11_CULL_FRONT;
            break;
        }

        // This should/will be done in a geometry shader like in the FixedFuncEMU sample and the shader needs solid
        rasterDesc.FillMode = newBlock->mPolygonMode == PM_WIREFRAME ? D3D11_FILL_WIREFRAME :
                                                                       D3D11_FILL_SOLID;

        rasterDesc.FrontCounterClockwise = true;

        const float nearFarFactor = 10.0;
        const float biasSign = mReverseDepth ? 1.0f : -1.0f;
        rasterDesc.DepthBias            = static_cast<int>( nearFarFactor * biasSign *
                                                            newBlock->mDepthBiasConstant );
        rasterDesc.SlopeScaledDepthBias = newBlock->mDepthBiasSlopeScale * biasSign;
        rasterDesc.DepthBiasClamp   = 0;

        rasterDesc.DepthClipEnable  = true;
        rasterDesc.ScissorEnable    = newBlock->mScissorTestEnabled;

        rasterDesc.MultisampleEnable     = true;
        rasterDesc.AntialiasedLineEnable = false;

        ID3D11RasterizerState *rasterizerState = 0;

        HRESULT hr = mDevice->CreateRasterizerState( &rasterDesc, &rasterizerState );
        if( FAILED(hr) )
        {
            String errorDescription = mDevice.getErrorString(hr);
            N_EXCEPT_EX(Exception::ERR_RENDERINGAPI_ERROR, hr,
                "Failed to create rasterizer state\nError Description: " + errorDescription,
                "D3D11RenderSystem::createStencil" );
        }

        newBlock->mRsData = rasterizerState;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::destroyStencil( ShaderChStencil *block )
    {
        ID3D11RasterizerState *rasterizerState = reinterpret_cast<ID3D11RasterizerState*>(
                                                                        block->mRsData );
        rasterizerState->Release();
        block->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createBlend( ShaderChBlend *newBlock )
    {
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory( &blendDesc, sizeof(D3D11_BLEND_DESC) );
        blendDesc.IndependentBlendEnable = false;
        blendDesc.RenderTarget[0].BlendEnable = newBlock->mBlendOperation;

        blendDesc.RenderTarget[0].RenderTargetWriteMask = newBlock->mBlendChannelMask;

        if( newBlock->mSeparateBlend )
        {
            if( newBlock->mSourceBlendFactor == SBF_ONE &&
                newBlock->mDestBlendFactor == SBF_ZERO &&
                newBlock->mSourceBlendFactorAlpha == SBF_ONE &&
                newBlock->mDestBlendFactorAlpha == SBF_ZERO )
            {
                blendDesc.RenderTarget[0].BlendEnable = FALSE;
            }
            else
            {
                blendDesc.RenderTarget[0].BlendEnable = TRUE;
                blendDesc.RenderTarget[0].SrcBlend = D3D11Mappings::get(newBlock->mSourceBlendFactor, false);
                blendDesc.RenderTarget[0].DestBlend = D3D11Mappings::get(newBlock->mDestBlendFactor, false);
                blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11Mappings::get(newBlock->mSourceBlendFactorAlpha, true);
                blendDesc.RenderTarget[0].DestBlendAlpha = D3D11Mappings::get(newBlock->mDestBlendFactorAlpha, true);
                blendDesc.RenderTarget[0].BlendOp = blendDesc.RenderTarget[0].BlendOpAlpha =
                        D3D11Mappings::get( newBlock->mBlendOperation );

                blendDesc.RenderTarget[0].RenderTargetWriteMask = newBlock->mBlendChannelMask;
            }
        }
        else
        {
            if( newBlock->mSourceBlendFactor == SBF_ONE && newBlock->mDestBlendFactor == SBF_ZERO )
            {
                blendDesc.RenderTarget[0].BlendEnable = FALSE;
            }
            else
            {
                blendDesc.RenderTarget[0].BlendEnable = TRUE;
                blendDesc.RenderTarget[0].SrcBlend = D3D11Mappings::get(newBlock->mSourceBlendFactor, false);
                blendDesc.RenderTarget[0].DestBlend = D3D11Mappings::get(newBlock->mDestBlendFactor, false);
                blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11Mappings::get(newBlock->mSourceBlendFactor, true);
                blendDesc.RenderTarget[0].DestBlendAlpha = D3D11Mappings::get(newBlock->mDestBlendFactor, true);
                blendDesc.RenderTarget[0].BlendOp = D3D11Mappings::get( newBlock->mBlendOperation );
                blendDesc.RenderTarget[0].BlendOpAlpha = D3D11Mappings::get( newBlock->mBlendOperationAlpha );

                blendDesc.RenderTarget[0].RenderTargetWriteMask = newBlock->mBlendChannelMask;
            }
        }

        // feature level 9 and below does not support alpha to coverage.
        if (mFeatureLevel < D3D_FEATURE_LEVEL_10_0)
            blendDesc.AlphaToCoverageEnable = false;
        else
            blendDesc.AlphaToCoverageEnable = newBlock->mAlphaToCoverageEnabled;

        ID3D11BlendState *blendState = 0;

        HRESULT hr = mDevice->CreateBlendState( &blendDesc, &blendState );
        if( FAILED(hr) )
        {
            String errorDescription = mDevice.getErrorString(hr);
            N_EXCEPT_EX(Exception::ERR_RENDERINGAPI_ERROR, hr,
                "Failed to create blend state\nError Description: " + errorDescription,
                "D3D11RenderSystem::createBlend" );
        }

        newBlock->mRsData = blendState;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::destroyBlend( ShaderChBlend *block )
    {
        ID3D11BlendState *blendState = reinterpret_cast<ID3D11BlendState*>( block->mRsData );
        blendState->Release();
        block->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createTexSample( TextureSampleUnit *newBlock )
    {
        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory( &samplerDesc, sizeof(D3D11_SAMPLER_DESC) );
        samplerDesc.Filter = D3D11Mappings::get( newBlock->mMinFilter, newBlock->mMagFilter,
                                                 newBlock->mMipFilter,
                                                 newBlock->mCompareFunction != NUM_COMPARE_FUNCTIONS );
        if( newBlock->mCompareFunction == NUM_COMPARE_FUNCTIONS )
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        else
            samplerDesc.ComparisonFunc = D3D11Mappings::get( newBlock->mCompareFunction );

        samplerDesc.AddressU = D3D11Mappings::get( newBlock->mU );
        samplerDesc.AddressV = D3D11Mappings::get( newBlock->mV );
        samplerDesc.AddressW = D3D11Mappings::get( newBlock->mW );

        samplerDesc.MipLODBias      = newBlock->mMipLodBias;
        samplerDesc.MaxAnisotropy   = static_cast<UINT>( newBlock->mMaxAnisotropy );
        samplerDesc.BorderColor[0]  = newBlock->mBorderColour.r;
        samplerDesc.BorderColor[1]  = newBlock->mBorderColour.g;
        samplerDesc.BorderColor[2]  = newBlock->mBorderColour.b;
        samplerDesc.BorderColor[3]  = newBlock->mBorderColour.a;
        samplerDesc.MinLOD          = newBlock->mMinLod;
        samplerDesc.MaxLOD          = newBlock->mMaxLod;

        ID3D11SamplerState *samplerState = 0;

        HRESULT hr = mDevice->CreateSamplerState( &samplerDesc, &samplerState ) ;
        if( FAILED(hr) )
        {
            String errorDescription = mDevice.getErrorString(hr);
            N_EXCEPT_EX(Exception::ERR_RENDERINGAPI_ERROR, hr,
                "Failed to create sampler state\nError Description: " + errorDescription,
                "D3D11RenderSystem::createTexSample" );
        }

        newBlock->mRsData = samplerState;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::destroyTexSample( TextureSampleUnit *block )
    {
        ID3D11SamplerState *samplerState = reinterpret_cast<ID3D11SamplerState*>( block->mRsData );
        samplerState->Release();
        block->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_descriptorSetTextureCreated( DescriptorSetTexture *newSet )
    {
        const size_t numElements = newSet->mTextures.size();
        ComPtr<ID3D11ShaderResourceView> *srvList = new ComPtr<ID3D11ShaderResourceView>[numElements];
        newSet->mRsData = srvList;

        size_t texIdx = 0;
        vectorL<const Texture*>::const_iterator itor = newSet->mTextures.begin();

        for( size_t i=0u; i<ST_PCnt; ++i )
        {
            const size_t numTexturesUsed = newSet->mShaderTypeTexCount[i];
            for( size_t j=0u; j<numTexturesUsed; ++j )
            {
                if( *itor )
                {
                    const D3D11TextureGpu *texture = static_cast<const D3D11TextureGpu*>( *itor );
                    srvList[texIdx] = texture->createSrv();
                }

                ++texIdx;
                ++itor;
            }
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_descriptorSetTextureDestroyed( DescriptorSetTexture *set )
    {
        const size_t numElements = set->mTextures.size();
        ComPtr<ID3D11ShaderResourceView> *srvList = reinterpret_cast<ComPtr<ID3D11ShaderResourceView>*>( set->mRsData );

        delete [] srvList;
        set->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createTexture( ShaderChTexture *newSet )
    {
        const size_t numElements = newSet->mTextures.size();
        ComPtr<ID3D11ShaderResourceView> *srvList = new ComPtr<ID3D11ShaderResourceView>[numElements];
        newSet->mRsData = srvList;

        vectorL<ShaderChTexture::Slot>::const_iterator itor = newSet->mTextures.begin();

        for( size_t i=0u; i<numElements; ++i )
        {
            if( itor->empty() )
                ;
            else if( itor->isTexture() )
            {
                const ShaderChTexture::TextureSlot &texSlot = itor->getTexture();
                const D3D11TextureGpu *texture = static_cast<const D3D11TextureGpu*>( texSlot.texture );
                srvList[i] = texture->createSrv( texSlot );
            }
            else
            {
                const ShaderChTexture::BufferSlot &bufferSlot = itor->getBuffer();
                const D3D11TexBufferPacked *texBuffer =
                        static_cast<const D3D11TexBufferPacked*>( bufferSlot.buffer );
                srvList[i] = texBuffer->createSrv( bufferSlot );
            }

            ++itor;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::destroyTexture( ShaderChTexture *set )
    {
        const size_t numElements = set->mTextures.size();
        ComPtr<ID3D11ShaderResourceView> *srvList = reinterpret_cast<ComPtr<ID3D11ShaderResourceView>*>( set->mRsData );

        delete [] srvList;
        set->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createBuffer( GpuParamBuffer *newSet )
    {
        const size_t numElements = newSet->mUavs.size();
        ComPtr<ID3D11UnorderedAccessView> * uavList = new ComPtr<ID3D11UnorderedAccessView>[numElements];
        newSet->mRsData = uavList;

        vectorL<GpuParamBuffer::Slot>::const_iterator itor = newSet->mUavs.begin();

        for( size_t i=0u; i<numElements; ++i )
        {
            if( itor->empty() )
                ;
            else if( itor->isTexture() )
            {
                const GpuParamBuffer::TextureSlot &texSlot = itor->getTexture();
                const D3D11TextureGpu * texture = static_cast<const D3D11TextureGpu*>( texSlot.texture );
                uavList[i] = texture->createUav( texSlot );
            }
            else
            {
                const GpuParamBuffer::BufferSlot &bufferSlot = itor->getBuffer();
                const D3D11UavBufferPacked * uavBuffer = static_cast<const D3D11UavBufferPacked*>( bufferSlot.buffer );
                uavList[i] = uavBuffer->createUav( bufferSlot );
            }

            ++itor;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::destroyBuffer( GpuParamBuffer *set )
    {
        const size_t numElements = set->mUavs.size();
        ComPtr<ID3D11UnorderedAccessView> *uavList = reinterpret_cast<ComPtr<ID3D11UnorderedAccessView>*>( set->mRsData );

        delete [] uavList;
        set->mRsData = 0;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setHlmsDepthblock( const ShaderChStencil * stencil )
    {
        assert( stencil->mRsData &&
                "The block must have been created via RenderSys::getDepthblock!" );

        ID3D11RasterizerState *rasterizerState = reinterpret_cast<ID3D11RasterizerState*>(stencil->mRsData );

        mDevice.GetImmediateContext()->RSSetState( rasterizerState );
        if( mDevice.isError() )
        {
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "D3D11 device cannot set rasterizer state\nError Description: " + errorDescription);
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setHlmsBlendblock( const ShaderChBlend * blend )
    {
        assert( blend->mRsData &&
                "The block must have been created via RenderSys::getBlendblock!" );

        ID3D11BlendState *blendState = reinterpret_cast<ID3D11BlendState*>( blend->mRsData );

        // TODO - Add this functionality to Ogre (what's the GL equivalent?)
        mDevice.GetImmediateContext()->OMSetBlendState( blendState, 0, 0xffffffff );
        if( mDevice.isError() )
        {
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "D3D11 device cannot set blend state\nError Description: " + errorDescription);
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setTextureSample( uint8 texUnit, const TextureSampleUnit *samplerblock )
    {
        assert( samplerblock->mRsData &&
                "The block must have been created via RenderSys::getSamplerblock!" );

        ID3D11SamplerState *samplerState = reinterpret_cast<ID3D11SamplerState*>( samplerblock->mRsData );

        //TODO: Refactor Ogre to:
        //  a. Separate samplerblocks from textures (GL can emulate the merge).
        //  b. Set all of them at once.
        mDevice.GetImmediateContext()->VSSetSamplers( static_cast<UINT>(texUnit), static_cast<UINT>(1),
                                                      &samplerState );
        mDevice.GetImmediateContext()->PSSetSamplers( static_cast<UINT>(texUnit), static_cast<UINT>(1),
                                                      &samplerState );
        if( mDevice.isError() )
        {
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "D3D11 device cannot set pixel shader samplers\nError Description:" + errorDescription);
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setRSO( const RenderStateObject *pso )
    {
        RenderSystem::setRSO( pso );

        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();

        //deviceContext->IASetInputLayout( 0 );
        deviceContext->VSSetShader( 0, 0, 0 );
        deviceContext->GSSetShader( 0, 0, 0 );
        deviceContext->HSSetShader( 0, 0, 0 );
        deviceContext->DSSetShader( 0, 0, 0 );
        deviceContext->PSSetShader( 0, 0, 0 );
        deviceContext->CSSetShader( 0, 0, 0 );

        mBoundComputeProgram = 0;

        if( !pso )
            return;

        _setHlmsDepthblock( pso->mStencil );
        _setHlmsBlendblock( pso->mBlend );

        D3D11HlmsPso *d3dPso = reinterpret_cast<D3D11HlmsPso*>( pso->mExtData );

        mPso = d3dPso;

        deviceContext->OMSetDepthStencilState( d3dPso->depthStencilState.Get(), mStencilRef );
        deviceContext->IASetPrimitiveTopology( d3dPso->topology );
        deviceContext->IASetInputLayout( d3dPso->inputLayout.Get() );

        if( d3dPso->mVS )
        {
            deviceContext->VSSetShader( d3dPso->mVS->getVertexShader(), 0, 0 );
            mVPMode = true;
        }

        if( d3dPso->mGS )
        {
            deviceContext->GSSetShader( d3dPso->mGS->getGeometryShader(), 0, 0 );
            mGPMode = true;
        }

        if( mFeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
        {
            if( d3dPso->mTS )
            {
                deviceContext->HSSetShader( d3dPso->mTS->getHullShader(), 0, 0 );
                mTPMode = true;
            }

            if( d3dPso->mDS )
            {
                deviceContext->DSSetShader( d3dPso->mDS->getDomainShader(), 0, 0 );
                mDPMode = true;
            }
        }

        if( d3dPso->mFS )
        {
            deviceContext->PSSetShader( d3dPso->mFS->getPixelShader(), 0, 0 );
            mFPMode = true;
        }

        if (mDevice.isError())
        {
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "D3D11 device cannot set shaders\nError Description: " + errorDescription);
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setIndirect( IndirectBuffer *ib )
    {
        if( mBufferManager->supportsIndirectBuffers() )
        {
            if( mBoundIndirectBuffer )
            {
                D3D11BufferInterfaceBase *bufferInterface = static_cast<D3D11BufferInterfaceBase*>(ib->getGpuData() );
                mBoundIndirectBuffer = bufferInterface->getVboName();
            }
            else
            {
                mBoundIndirectBuffer = 0;
            }
        }
        else
        {
            if( ib )
                mIndirectPtr = ib->getSwBufferPtr();
            else
                mIndirectPtr = 0;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::createCSO( ComputeStateObject * newPso )
    {
        newPso->mExtData = reinterpret_cast<void *>( static_cast<D3D11HLSLProgram*>(newPso->mCS->getMain() ) );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::destroyCSO( ComputeStateObject * newPso )
    {
        newPso->mExtData = 0;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setCSO( const ComputeStateObject *pso )
    {
        D3D11HLSLProgram *newComputeShader = 0;

        if( pso )
        {
            newComputeShader = reinterpret_cast<D3D11HLSLProgram*>( pso->mExtData );
            if( mBoundComputeProgram == newComputeShader )
                return;
        }

        RenderSystem::setRSO( (RenderStateObject*)0 );

        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();

        //deviceContext->IASetInputLayout( 0 );
        deviceContext->VSSetShader( 0, 0, 0 );
        deviceContext->GSSetShader( 0, 0, 0 );
        deviceContext->HSSetShader( 0, 0, 0 );
        deviceContext->DSSetShader( 0, 0, 0 );
        deviceContext->PSSetShader( 0, 0, 0 );
        deviceContext->CSSetShader( 0, 0, 0 );

        if( !pso )
            return;

        mBoundComputeProgram = newComputeShader;

        deviceContext->CSSetShader( mBoundComputeProgram->getComputeShader(), 0, 0 );
        mActiveCGPP = pso->mParam;
        mCPMode = true;

        if (mDevice.isError())
        {
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "D3D11 device cannot set shaders\nError Description: " + errorDescription );
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_beginFrame()
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_endFrame()
    {
        mBoundComputeProgram = 0;
        mActiveCGPP.setNull();
        mCPMode = false;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_render(const v1::GeometryRaw& op)
    {

        // Exit immediately if there is nothing to render
        if (op.vertexData==0 || op.vertexData->vertexCount == 0)
        {
            return;
        }

        v1::HardwareVertexBufferSharedPtr globalInstanceVertexBuffer = getInstanceBuffer();
        v1::VertexDeclaration* globalVertexDeclaration = getInstanceData();

        bool hasInstanceData = (op.useGlobalInstancingVertexBufferIsAvailable &&
                    !globalInstanceVertexBuffer.isNull() && globalVertexDeclaration != NULL)
                || op.vertexData->vertexBufferBinding->getHasInstanceData();

        size_t numberOfInstances = op.mInstanceCount;

        if (op.useGlobalInstancingVertexBufferIsAvailable)
        {
            numberOfInstances *= getInstanceCount();
        }

        // Call super class
        RenderSystem::_render(op);

        ComPtr<ID3D11Buffer> pSOTarget;
        // Mustn't bind a emulated vertex, pixel shader (see below), if we are rendering to a stream out buffer
        mDevice.GetImmediateContext()->SOGetTargets(1, pSOTarget.GetAddressOf());

        //check consistency of vertex-fragment shaders
        if (!mPso->mVS ||
             (!mPso->mFS && op.mType != OT_POINT_LIST && !pSOTarget )
           )
        {

            N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
                "Attempted to render to a D3D11 device without both vertex and fragment shaders there is no fixed pipeline in d3d11 - use the RTSS or write custom shaders.",
                "D3D11RenderSystem::_render");
        }

        // Check consistency of tessellation shaders
        if( (mPso->mTS && !mPso->mDS) ||
            (!mPso->mTS && mPso->mDS) )
        {
            if (mPso->mTS && !mPso->mDS) {
            N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
                "Attempted to use tessellation, but domain shader is missing",
                "D3D11RenderSystem::_render");
            }
            else {
                N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
                "Attempted to use tessellation, but hull shader is missing",
                "D3D11RenderSystem::_render"); }
        }

        if (mDevice.isError())
        {
            // this will never happen but we want to be consistent with the error checks...
            String errorDescription = mDevice.getErrorString();
            N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
                "D3D11 device cannot set geometry shader to null\nError Description:" + errorDescription,
                "D3D11RenderSystem::_render");
        }

        // Determine rendering operation
        D3D11_PRIMITIVE_TOPOLOGY primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        DWORD primCount = 0;

        // Handle computing
        if(mPso->mTS && mPso->mDS)
        {
            // useful primitives for tessellation
            switch( op.mType )
            {
            case OT_LINE_LIST:
                primType = D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 2;
                break;

            case OT_LINE_STRIP:
                primType = D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 1;
                break;

            case OT_TRIANGLE_LIST:
                primType = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 3;
                break;

            case OT_TRIANGLE_STRIP:
                primType = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 2;
                break;
            }
        }
        else
        {
            //rendering without tessellation.
            bool useAdjacency = (mGPMode && mPso->mGS && mPso->mGS->isAdjacencyPrimitive());
            switch( op.mType )
            {
            case OT_POINT_LIST:
                primType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount);
                break;

            case OT_LINE_LIST:
                primType = useAdjacency ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ : D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 2;
                break;

            case OT_LINE_STRIP:
                primType = useAdjacency ? D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ : D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 1;
                break;

            case OT_TRIANGLE_LIST:
                primType = useAdjacency ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) / 3;
                break;

            case OT_TRIANGLE_STRIP:
                primType = useAdjacency ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 2;
                break;

            case OT_TRIANGLE_FAN:
                N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error - DX11 render - no support for triangle fan (OT_TRIANGLE_FAN)", "D3D11RenderSystem::_render");
                primType = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED; // todo - no TRIANGLE_FAN in DX 11
                primCount = (DWORD)(op.useIndexes ? op.indexData->indexCount : op.vertexData->vertexCount) - 2;
                break;
            }
        }

        if (primCount)
        {
            // Issue the op
            //HRESULT hr;
            if( op.useIndexes  )
            {
                v1::D3D11HardwareIndexBuffer* d3dIdxBuf =
                    static_cast<v1::D3D11HardwareIndexBuffer*>(op.indexData->indexBuffer.get());
                mDevice.GetImmediateContext()->IASetIndexBuffer( d3dIdxBuf->getD3DIndexBuffer(), D3D11Mappings::getFormat(d3dIdxBuf->getType()), 0 );
                if (mDevice.isError())
                {
                    String errorDescription = mDevice.getErrorString();
                    N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
                        "D3D11 device cannot set index buffer\nError Description:" + errorDescription,
                        "D3D11RenderSystem::_render");
                }
            }

            mDevice.GetImmediateContext()->IASetPrimitiveTopology( primType );
            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
                    "D3D11 device cannot set primitive topology\nError Description:" + errorDescription,
                    "D3D11RenderSystem::_render");
            }

            do
            {
                if(op.useIndexes)
                {
                    if(hasInstanceData)
                    {
                        mDevice.GetImmediateContext()->DrawIndexedInstanced(
                            static_cast<UINT>(op.indexData->indexCount),
                            static_cast<UINT>(numberOfInstances),
                            static_cast<UINT>(op.indexData->indexStart),
                            static_cast<INT>(op.vertexData->vertexStart),
                            0);
                    }
                    else
                    {
                        mDevice.GetImmediateContext()->DrawIndexed(
                            static_cast<UINT>(op.indexData->indexCount),
                            static_cast<UINT>(op.indexData->indexStart),
                            static_cast<INT>(op.vertexData->vertexStart));
                    }
                }
                else // non indexed
                {
                    if(op.vertexData->vertexCount == -1) // -1 is a sign to use DrawAuto
                    {
                        mDevice.GetImmediateContext()->DrawAuto();
                    }
                    else if(hasInstanceData)
                    {
                        mDevice.GetImmediateContext()->DrawInstanced(
                            static_cast<UINT>(op.vertexData->vertexCount),
                            static_cast<UINT>(numberOfInstances),
                            static_cast<UINT>(op.vertexData->vertexStart),
                            0);
                    }
                    else
                    {
                        mDevice.GetImmediateContext()->Draw(
                            static_cast<UINT>(op.vertexData->vertexCount),
                            static_cast<UINT>(op.vertexData->vertexStart));
                    }
                }

                if(mDevice.isError())
                {
                    String errorDescription = "D3D11 device cannot draw";
                    if(!op.useIndexes && op.vertexData->vertexCount == -1) // -1 is a sign to use DrawAuto
                        errorDescription.append(" auto");
                    else
                        errorDescription.append(op.useIndexes ? " indexed" : "").append(hasInstanceData ? " instanced" : "");
                    errorDescription.append("\nError Description:").append(mDevice.getErrorString());
                    errorDescription.append("\nActive OGRE shaders:")
                        .append(mPso->mVS ? ("\nVS = " + mPso->mVS->getName()).c_str() : "")
                        .append(mPso->mTS ? ("\nHS = " + mPso->mTS->getName()).c_str() : "")
                        .append(mPso->mDS ? ("\nDS = " + mPso->mDS->getName()).c_str() : "")
                        .append(mPso->mGS ? ("\nGS = " + mPso->mGS->getName()).c_str() : "")
                        .append(mPso->mFS ? ("\nFS = " +mPso->mFS->getName()).c_str() : "")
                        .append(mBoundComputeProgram ? ("\nCS = " + mBoundComputeProgram->getName()).c_str() : "");

                    N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, errorDescription, "D3D11RenderSystem::_render");
                }

            }while(updateParam());
        }


        // Crashy : commented this, 99% sure it's useless but really time consuming
        /*if (true) // for now - clear the render state
        {
            mDevice.GetImmediateContext()->OMSetBlendState(0, 0, 0xffffffff);
            mDevice.GetImmediateContext()->RSSetState(0);
            mDevice.GetImmediateContext()->OMSetDepthStencilState(0, 0);
//          mDevice->PSSetSamplers(static_cast<UINT>(0), static_cast<UINT>(0), 0);

            // Clear class instance storage
            memset(mClassInstances, 0, sizeof(mClassInstances));
            memset(mNumClassInstances, 0, sizeof(mNumClassInstances));
        }*/

    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_execute( const ComputeStateObject &pso )
    {
        mDevice.GetImmediateContext()->Dispatch( pso.mGroupCount[0], pso.mGroupCount[1], pso.mGroupCount[2] );

        assert( mMaxBoundUavCS < 8u );
        ID3D11UnorderedAccessView *nullUavViews[8];
        memset( nullUavViews, 0, sizeof( nullUavViews ) );
        mDevice.GetImmediateContext()->CSSetUnorderedAccessViews( 0, mMaxBoundUavCS + 1u, nullUavViews, NULL );
        mMaxBoundUavCS = 0u;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setGeometry( const GeometryRaw *_vao )
    {
        const D3D11VertexArrayObject *vao = static_cast<const D3D11VertexArrayObject*>( _vao );
        D3D11VertexArrayObjectShared *sharedData = vao->mSharedData;

        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();

        deviceContext->IASetVertexBuffers( 0, vao->mVertexBuffers.size() + 1, //+1 due to DrawId
                                           sharedData->mVertexBuffers[0].GetAddressOf(),
                                           sharedData->mStrides,
                                           sharedData->mOffsets );
        deviceContext->IASetIndexBuffer( sharedData->mIndexBuffer.Get(), sharedData->mIndexFormat, 0 );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_render( const CbDrawCallIndexed *cmd )
    {
        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();

        UINT indirectBufferOffset = reinterpret_cast<UINT>(cmd->indirectBufferOffset);
        for( uint32 i=cmd->numDraws; i--; )
        {
            deviceContext->DrawIndexedInstancedIndirect( mBoundIndirectBuffer, indirectBufferOffset );

            indirectBufferOffset += sizeof( IndexIndirect );
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_render( const CbDrawCallStrip *cmd )
    {
        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();

        UINT indirectBufferOffset = reinterpret_cast<UINT>(cmd->indirectBufferOffset);
        for( uint32 i=cmd->numDraws; i--; )
        {
            deviceContext->DrawInstancedIndirect( mBoundIndirectBuffer, indirectBufferOffset );

            indirectBufferOffset += sizeof( VertexIndirect );
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_renderEmulated( const CbDrawCallIndexed *cmd )
    {
        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();

        IndexIndirect *drawCmd = reinterpret_cast<IndexIndirect*>(mIndirectPtr + (size_t)cmd->indirectBufferOffset );

        for( uint32 i=cmd->numDraws; i--; )
        {
            deviceContext->DrawIndexedInstanced( drawCmd->primCount,
                                                 drawCmd->instanceCount,
                                                 drawCmd->firstVertexIndex,
                                                 drawCmd->baseVertex,
                                                 drawCmd->baseInstance );
            ++drawCmd;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_renderEmulated( const CbDrawCallStrip *cmd )
    {
        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();

        VertexIndirect *drawCmd = reinterpret_cast<VertexIndirect*>(mIndirectPtr + (size_t)cmd->indirectBufferOffset );

        for( uint32 i=cmd->numDraws; i--; )
        {
            deviceContext->DrawInstanced( drawCmd->primCount,
                                          drawCmd->instanceCount,
                                          drawCmd->firstVertexIndex,
                                          drawCmd->baseInstance );
            ++drawCmd;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_setRenderOperation(const v1::GeometryRaw * cmd)
    {
        mCurrentVertexBuffer    = cmd->vertexData;
        mCurrentIndexBuffer     = cmd->indexData;

        ID3D11DeviceContextN *deviceContext = mDevice.GetImmediateContext();

        if( cmd->indexData )
        {
            v1::D3D11HardwareIndexBuffer* indexBuffer = static_cast<v1::D3D11HardwareIndexBuffer*>( cmd->indexData->indexBuffer.get() );
            deviceContext->IASetIndexBuffer( indexBuffer->getD3DIndexBuffer(), D3D11Mappings::getFormat( indexBuffer->getType() ), 0 );
        }
        else
        {
            deviceContext->IASetIndexBuffer( 0, DXGI_FORMAT_UNKNOWN, 0 );
        }

        uint32 usedSlots = 0;
        const v1::VertexBufferBinding::VertexBufferBindingMap & binds = cmd->vertexData->vertexBufferBinding->getBindings();
        v1::VertexBufferBinding::VertexBufferBindingMap::const_iterator i, iend = binds.end();
        for (i = binds.begin(); i != iend; ++i)
        {
            const v1::D3D11HardwareVertexBuffer* d3d11buf =
                static_cast<const v1::D3D11HardwareVertexBuffer*>(i->second.get());

            UINT stride = static_cast<UINT>(d3d11buf->getVertexSize());
            UINT offset = 0; // no stream offset, this is handled in _render instead
            UINT slot = static_cast<UINT>(i->first);
            ID3D11Buffer * pVertexBuffers = d3d11buf->getD3DVertexBuffer();
            mDevice.GetImmediateContext()->IASetVertexBuffers(
                slot, // The first input slot for binding.
                1, // The number of vertex buffers in the array.
                &pVertexBuffers,
                &stride,
                &offset
                );

            if (mDevice.isError())
            {
                String errorDescription = mDevice.getErrorString();
                N_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
                    "D3D11 device cannot set vertex buffers\nError Description:" + errorDescription,
                    "D3D11RenderSystem::setVertexBufferBinding");
            }

            ++usedSlots;
        }

        static_cast<D3D11VaoManager*>(mBufferManager)->bindDrawId(usedSlots);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_render(const v1::CbDrawCallIndexed * cmd)
    {
        mDevice.GetImmediateContext()->DrawIndexedInstanced(
            cmd->primCount,
            cmd->instanceCount,
            cmd->firstVertexIndex,
            static_cast<INT>(mCurrentVertexBuffer->vertexStart),
            cmd->baseInstance );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_render(const v1::CbDrawCallStrip * cmd)
    {
        mDevice.GetImmediateContext()->DrawInstanced(
                    cmd->primCount,
                    cmd->instanceCount,
                    cmd->firstVertexIndex,
                    cmd->baseInstance );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setNormaliseNormals(bool normalise)
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::on(GpuProgramType gptype, GpuProgramParam * params, uint16 mask)
    {
        if (mask & (uint16)GPV_GLOBAL)
        {
            // TODO: Dx11 supports shared constant buffers, so use them
            // check the match to constant buffers & use rendersystem data hooks to store
            // for now, just copy
            params->syncValue();
        }

        // Do everything here in Dx11, since deal with via buffers anyway so number of calls
        // is actually the same whether we categorise the updates or not
        ID3D11Buffer* pBuffers[NumDefaultBufferTypes];
        UINT slotStart, numBuffers;
        switch(gptype)
        {
        case GPT_VERTEX_PROGRAM:
            if( mPso->mVS )
            {
                mPso->mVS->getConstantBuffers( pBuffers, slotStart, numBuffers, params, mask );
                if( numBuffers > 0 )
                {
                    mDevice.GetImmediateContext()->VSSetConstantBuffers( slotStart, numBuffers,
                                                                         pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                                     "D3D11 device cannot set vertex shader constant buffers\n"
                                     "Error Description:" + errorDescription,
                                     "D3D11RenderSystem::on" );
                    }
                }
            }
            break;
        case GPT_FRAGMENT_PROGRAM:
            if( mPso->mFS )
            {
                mPso->mFS->getConstantBuffers( pBuffers, slotStart, numBuffers, params, mask );
                if( numBuffers > 0 )
                {
                    mDevice.GetImmediateContext()->PSSetConstantBuffers( slotStart, numBuffers,
                                                                         pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                                     "D3D11 device cannot set pixel shader constant buffers\n"
                                     "Error Description:" + errorDescription,
                                     "D3D11RenderSystem::on" );
                    }
                }
            }
            break;
        case GPT_GEOMETRY_PROGRAM:
            if( mPso->mGS )
            {
                mPso->mGS->getConstantBuffers( pBuffers, slotStart, numBuffers, params, mask );
                if( numBuffers > 0 )
                {
                    mDevice.GetImmediateContext()->GSSetConstantBuffers( slotStart, numBuffers,
                                                                         pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                                     "D3D11 device cannot set geometry shader constant buffers\n"
                                     "Error Description:" + errorDescription,
                                     "D3D11RenderSystem::on" );
                    }
                }
            }
            break;
        case GPT_HULL_PROGRAM:
            if( mPso->mTS )
            {
                mPso->mTS->getConstantBuffers( pBuffers, slotStart, numBuffers, params, mask );
                if( numBuffers > 0 )
                {
                    mDevice.GetImmediateContext()->HSSetConstantBuffers( slotStart, numBuffers,
                                                                         pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                                     "D3D11 device cannot set hull shader constant buffers\n"
                                     "Error Description:" + errorDescription,
                                     "D3D11RenderSystem::on" );
                    }
                }
            }
            break;
        case GPT_DOMAIN_PROGRAM:
            if( mPso->mDS )
            {
                mPso->mDS->getConstantBuffers( pBuffers, slotStart, numBuffers, params, mask );
                if( numBuffers > 0 )
                {
                    mDevice.GetImmediateContext()->DSSetConstantBuffers( slotStart, numBuffers, pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                                     "D3D11 device cannot set domain shader constant buffers\n"
                                     "Error Description:" + errorDescription,
                                     "D3D11RenderSystem::on" );
                    }
                }
            }
            break;
        case GPT_COMPUTE_PROGRAM:
            if( mBoundComputeProgram )
            {
                mBoundComputeProgram->getConstantBuffers( pBuffers, slotStart, numBuffers, params, mask );
                if( numBuffers > 0 )
                {
                    mDevice.GetImmediateContext()->CSSetConstantBuffers( slotStart, numBuffers, pBuffers );
                    if (mDevice.isError())
                    {
                        String errorDescription = mDevice.getErrorString();
                        N_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                                     "D3D11 device cannot set compute shader constant buffers\n"
                                     "Error Description:" + errorDescription,
                                     "D3D11RenderSystem::on" );
                    }
                }
            }
            break;
        };

        // Now, set class instances
        const GpuProgramParameters::SubroutineMap & subroutineMap = params->getSubroutineMap();
        if (subroutineMap.empty())
            return;

        GpuProgramParameters::SubroutineIterator it;
        GpuProgramParameters::SubroutineIterator end = subroutineMap.end();
        for(it = subroutineMap.begin(); it != end; ++it)
        {
            setSubroutine(gptype, it->first, it->second);
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::bindGpuProgramPassIterationParameters(GpuProgramType gptype)
    {

        switch(gptype)
        {
        case GPT_VERTEX_PROGRAM:
            on(gptype, mActiveVGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;

        case GPT_FRAGMENT_PROGRAM:
            on(gptype, mActiveFGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        case GPT_GEOMETRY_PROGRAM:
            on(gptype, mActiveGGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        case GPT_HULL_PROGRAM:
            on(gptype, mActiveTGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        case GPT_DOMAIN_PROGRAM:
            on(gptype, mActiveDGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        case GPT_COMPUTE_PROGRAM:
            on(gptype, mActiveCGPP, (uint16)GPV_PASS_ITERATION_NUMBER);
            break;
        }
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setSubroutine(GpuProgramType gptype, unsigned int slotIndex, const String& subroutineName)
    {
        ID3D11ClassInstance* instance = 0;

        ClassInstanceIterator it = mInstanceMap.find(subroutineName);
        if (it == mInstanceMap.end())
        {
            // try to get instance already created (must have at least one field)
            HRESULT hr = mDevice.GetClassLinkage()->GetClassInstance(subroutineName.c_str(), 0, &instance);
            if (FAILED(hr) || instance == 0)
            {
                // probably class don't have a field, try create a new
                hr = mDevice.GetClassLinkage()->CreateClassInstance(subroutineName.c_str(), 0, 0, 0, 0, &instance);
                if (FAILED(hr) || instance == 0)
                {
                    N_EXCEPT_EX(Exception::ERR_RENDERINGAPI_ERROR, hr,
                        "Shader subroutine with name " + subroutineName + " doesn't exist.",
                        "D3D11RenderSystem::setSubroutineName");
                }
            }

            // Store class instance
            mInstanceMap.insert(std::make_pair(subroutineName, instance));
        }
        else
        {
            instance = it->second;
        }

        // If already created, store class instance
        mClassInstances[gptype][slotIndex] = instance;
        mNumClassInstances[gptype] = mNumClassInstances[gptype] + 1;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setSubroutine(GpuProgramType gptype, const String& slotName, const String& subroutineName)
    {
        unsigned int slotIdx = 0;
        switch(gptype)
        {
        case GPT_VERTEX_PROGRAM:
            {
                if (mPso->mVS)
                {
                    slotIdx = mPso->mVS->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_FRAGMENT_PROGRAM:
            {
                if (mPso->mFS)
                {
                    slotIdx = mPso->mFS->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_GEOMETRY_PROGRAM:
            {
                if (mPso->mGS)
                {
                    slotIdx = mPso->mGS->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_HULL_PROGRAM:
            {
                if (mPso->mTS)
                {
                    slotIdx = mPso->mTS->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_DOMAIN_PROGRAM:
            {
                if (mPso->mDS)
                {
                    slotIdx = mPso->mDS->getSubroutineSlot(slotName);
                }
            }
            break;
        case GPT_COMPUTE_PROGRAM:
            {
                if (mBoundComputeProgram)
                {
                    slotIdx = mBoundComputeProgram->getSubroutineSlot(slotName);
                }
            }
            break;
        };

        // Set subroutine for slot
        setSubroutine(gptype, slotIdx, subroutineName);
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setClipPlanesImpl(const PlaneList& clipPlanes)
    {
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::clearBuffer( FrameBufferObject *fbobj,
                                              Texture *anyTarget, uint8 mipLevel )
    {
        D3D11RenderPassDescriptor *renderPassDescD3d =
                static_cast<D3D11RenderPassDescriptor*>( fbobj );
        renderPassDescD3d->clearBuffer();
    }
    // ------------------------------------------------------------------
    void D3D11RenderSystem::setClipPlane (ushort index, Real A, Real B, Real C, Real D)
    {
    }

    // ------------------------------------------------------------------
    void D3D11RenderSystem::enableClipPlane (ushort index, bool enable)
    {
    }
    //---------------------------------------------------------------------
    PixelCountQuery* D3D11RenderSystem::create(void)
    {
        D3D11HardwareOcclusionQuery* ret = new D3D11HardwareOcclusionQuery (mDevice);
        mPixelQueryList.push_back(ret);
        return ret;
    }
    //---------------------------------------------------------------------
    Real D3D11RenderSystem::getTexelXOffset(void)
    {
        // D3D11 is now like GL
        return 0.0f;
    }
    //---------------------------------------------------------------------
    Real D3D11RenderSystem::getTexelYOffset(void)
    {
        // D3D11 is now like GL
        return 0.0f;
    }
    //---------------------------------------------------------------------
    Real D3D11RenderSystem::getMinDepth(void)
    {
        // Range [0.0f, 1.0f]
        return 0.0f;
    }
    //---------------------------------------------------------------------
    Real D3D11RenderSystem::getMaxDepth(void)
    {
        // Range [0.0f, 1.0f]
        // D3D inverts even identity view matrices, so maximum INPUT is -1.0
        return -1.0f;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::registerThread()
    {
        // nothing to do - D3D11 shares rendering context already
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::unregisterThread()
    {
        // nothing to do - D3D11 shares rendering context already
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::preExtraThreadsStarted()
    {
        // nothing to do - D3D11 shares rendering context already
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::postExtraThreadsStarted()
    {
        // nothing to do - D3D11 shares rendering context already
    }
    //---------------------------------------------------------------------
    String D3D11RenderSystem::getErrorString(long errorNumber) const
    {
        return mDevice.getErrorString(errorNumber);
    }
    //---------------------------------------------------------------------
    SampleType D3D11RenderSystem::checkSample(const SampleType &sampleDesc, PixelFormat format)
    {
        SampleType res;
        DXGI_FORMAT dxgiFormat = D3D11Mappings::get( format );
        const uint8 samples = sampleDesc.getMaxSampling();
        const bool msaaOnly = sampleDesc.isMsaa();

        // NVIDIA, AMD - prefer CSAA aka EQAA if available. See
        // http://developer.download.nvidia.com/assets/gamedev/docs/CSAA_Tutorial.pdf
        // http://developer.amd.com/wordpress/media/2012/10/EQAA%20Modes%20for%20AMD%20HD%206900%20Series%20Cards.pdf
        // https://www.khronos.org/registry/OpenGL/extensions/NV/NV_framebuffer_multisample_coverage.txt

        // Modes are sorted from high quality to low quality, CSAA aka EQAA are listed first
        // Note, that max(Count, Quality) == MSAA level and (Count >= 8 && Quality != 0) == quality hint
        DXGI_SAMPLE_DESC presets[] = {
            { sampleDesc.getMultiSampling(), sampleDesc.getCoverageSampling() }, // exact match

            { 16, 0 },  // MSAA 16x
            { 8, 16 },  // CSAA 16xQ, EQAA 8f16x
            { 4, 16 },  // CSAA 16x,  EQAA 4f16x

            { 12, 0 },  // MSAA 12x

            { 8, 0 },  // MSAA 8x
            { 4, 8 },  // CSAA 8x,  EQAA 4f8x

            { 6, 0 },  // MSAA 6x
            { 4, 0 },  // MSAA 4x
            { 2, 4 },  // EQAA 2f4x
            { 2, 0 },  // MSAA 2x
            { 1, 0 },  // MSAA 1x
            { NULL, NULL },
        };

        // Use first supported mode
        for( DXGI_SAMPLE_DESC *mode = presets; mode->Count != 0; ++mode )
        {
            // Skip too HQ modes
            unsigned modeSamples = std::max( mode->Count, mode->Quality );
            if( modeSamples > samples )
                continue;

            // Skip CSAA modes if specifically MSAA were requested, but not vice versa
            if( msaaOnly && mode->Quality > 0 )
                continue;

            // Skip unsupported modes
            UINT numQualityLevels;
            HRESULT hr =
                mDevice->CheckMultisampleQualityLevels( dxgiFormat, mode->Count, &numQualityLevels );
            if( FAILED( hr ) || mode->Quality >= numQualityLevels )
                continue;

            // All checks passed
            CoordType::CoordType pattern;
            if( mode->Quality != 0 )
                pattern = CoordType::CT_Unknow;  // CSAA / EQAA
            else
                pattern = sampleDesc.getType();
            res._set( static_cast<uint8>( mode->Count ), static_cast<uint8>( mode->Quality ), pattern );
            break;
        }

        return res;
    }
    //---------------------------------------------------------------------
    unsigned int D3D11RenderSystem::getDisplayMonitorCount() const
    {
        unsigned int monitorCount = 0;
        HRESULT hr;
        ComPtr<IDXGIOutput> pOutput;

        if (!mDriverList)
        {
            return 0;
        }

        for (size_t i = 0; i < mDriverList->count(); ++i)
        {
            for (size_t m = 0;; ++m)
            {
                hr = mDriverList->item(i)->getDeviceAdapter()->EnumOutputs(m, pOutput.ReleaseAndGetAddressOf());
                if (DXGI_ERROR_NOT_FOUND == hr)
                {
                    break;
                }
                else if (FAILED(hr))
                {
                    break;   //Something bad happened.
                }
                else
                {
                    ++monitorCount;
                }
            }
        }
        return monitorCount;
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::initRenderSystem()
    {
        if (mRenderSystemWasInited)
        {
            return;
        }

        mRenderSystemWasInited = true;
        // set pointers to NULL
        mDriverList = NULL;
        mHardwareBufferManager = NULL;
        mGpuProgramManager = NULL;
        mPrimaryWindow = NULL;
        mMinRequestedFeatureLevel = D3D_FEATURE_LEVEL_9_1;
#if __OGRE_WINRT_PHONE // Windows Phone support only FL 9.3, but simulator can create much more capable device, so restrict it artificially here
        mMaxRequestedFeatureLevel = D3D_FEATURE_LEVEL_9_3;
#elif defined(_WIN32_WINNT_WIN8)
        if( IsWindows8OrGreater() )
            mMaxRequestedFeatureLevel = D3D_FEATURE_LEVEL_11_1;
        else
            mMaxRequestedFeatureLevel = D3D_FEATURE_LEVEL_11_0;
#else
        mMaxRequestedFeatureLevel = D3D_FEATURE_LEVEL_11_0;
#endif
        mUseNVPerfHUD = false;
        mHLSLProgramFactory = NULL;

#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        OGRE_DELETE mStereoDriver;
        mStereoDriver = NULL;
#endif

        mPso = NULL;
        mBoundComputeProgram = NULL;

        //mBindingType = ShaderChTextureUnit::BT_FRAGMENT;

        mVendorExtension = D3D11VendorExtension::initializeExtension( GPU_VENDOR_COUNT, 0 );

        ComPtr<ID3D11Device> device;
        createD3D11Device( mVendorExtension, "", NULL, D3D_DRIVER_TYPE_HARDWARE,
                           mMinRequestedFeatureLevel, mMaxRequestedFeatureLevel, 0,
                           device.GetAddressOf() );
        mDevice.TransferOwnership( device );
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::getPlatformData(const String& name, void* pData)
    {
        if( name == "D3DDEVICE" )
        {
            ID3D11DeviceN  **device = (ID3D11DeviceN **)pData;
            *device = mDevice.get();
            return;
        }
        else if( name == "MapNoOverwriteOnDynamicConstantBuffer" )
        {
            *reinterpret_cast<bool*>(pData) = false; //TODO
            return;
        }
        else if( name == "MapNoOverwriteOnDynamicBufferSRV" )
        {
            *reinterpret_cast<bool*>(pData) = false; //TODO
            return;
        }

        N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Attribute not found: " + name, "RenderSystem::getPlatformData");
    }
    //---------------------------------------------------------------------
    D3D11HLSLProgram* D3D11RenderSystem::_getBoundComputeProgram() const
    {
        return mBoundComputeProgram;
    }
    //---------------------------------------------------------------------
    bool D3D11RenderSystem::getBufferType(ViewportType colourBuffer)
    {
#if OGRE_NO_QUAD_BUFFER_STEREO == 0
        return D3D11StereoDriverBridge::getSingleton().getBufferType(colourBuffer);
#else
        return false;
#endif
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::beginProfiler( const String &eventName )
    {
#if OGRE_D3D11_PROFILING
        if(mDevice.GetProfiler())
        {
            wchar_t wideName[256]; // Let avoid heap memory allocation if we are in profiling code.
            bool wideNameOk = !eventName.empty() && 0 != MultiByteToWideChar(CP_ACP, 0, eventName.data(), eventName.length() + 1, wideName, ARRAYSIZE(wideName));
            mDevice.GetProfiler()->BeginEvent(wideNameOk ? wideName : L"<too long or empty event name>");
        }
#endif
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::endProfiler( void )
    {
#if OGRE_D3D11_PROFILING
        if(mDevice.GetProfiler())
            mDevice.GetProfiler()->EndEvent();
#endif
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::markProfiler( const String &eventName )
    {
#if OGRE_D3D11_PROFILING
        if(mDevice.GetProfiler())
        {
            wchar_t wideName[256]; // Let avoid heap memory allocation if we are in profiling code.
            bool wideNameOk = !eventName.empty() && 0 != MultiByteToWideChar(CP_ACP, 0, eventName.data(), eventName.length() + 1, wideName, ARRAYSIZE(wideName));
            mDevice.GetProfiler()->SetMarker(wideNameOk ? wideName : L"<too long or empty event name>");
        }
#endif
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::setupGPUProfiler(void)
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_BindD3D11( (void*)mDevice.get(), (void*)mDevice.GetImmediateContext() );
#endif
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::shutdownGPUProfiler(void)
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_UnbindD3D11();
#endif
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::beginGPUProfiler( const String &name, uint32 *hashCache )
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_BeginD3D11Sample( name.c_str(), hashCache );
#endif
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::endGPUProfiler( const String &name )
    {
#if OGRE_PROFILING == OGRE_PROFILING_REMOTERY
        _rmt_EndD3D11Sample();
#endif
    }
    //---------------------------------------------------------------------
    /*const PixelFormatToShaderType* D3D11RenderSystem::getPixelFormatToShaderType(void) const
    {
        return &mD3D11PixelFormatToShaderType;
    }*/
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_clear(void)
    {
        OgreProfileExhaustive( "D3D11RenderSystem::_clear" );

        mDevice.GetImmediateContext()->ClearState();
        mDevice.GetImmediateContext()->Flush();

        endFBO();
    }
    //---------------------------------------------------------------------
    void D3D11RenderSystem::_flush(void)
    {
        OgreProfileExhaustive( "D3D11RenderSystem::_flush" );
        mDevice.GetImmediateContext()->Flush();
    }
}
