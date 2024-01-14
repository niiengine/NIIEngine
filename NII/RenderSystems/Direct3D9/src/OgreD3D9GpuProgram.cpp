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
#include "OgreD3D9GpuProgram.h"
#include "OgreMatrix4.h"
#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreD3D9Mappings.h"
#include "OgreResourceGroupManager.h"
#include "OgreD3D9RenderSystem.h"
#include "OgreGpuProgramManager.h"

namespace Ogre {

    D3D9GpuProgram::CmdColumnMajorMatrices D3D9GpuProgram::msCmdColumnMajorMatrices;
    D3D9GpuProgram::CmdExternalMicrocode D3D9GpuProgram::msCmdExternalMicrocode;

   //-----------------------------------------------------------------------
    String D3D9GpuProgram::CmdColumnMajorMatrices::doGet(const void *target) const
    {
        return StrConv::conv(static_cast<const D3D9GpuProgram*>(target)->getColumnMajorMatrices());
    }
    void D3D9GpuProgram::CmdColumnMajorMatrices::doSet(void *target, const String& val)
    {
        static_cast<D3D9GpuProgram*>(target)->setColumnMajorMatrices(StringConverter::parseBool(val));
    }
    //-----------------------------------------------------------------------
    String D3D9GpuProgram::CmdExternalMicrocode::doGet(const void *target) const
    {
        //D3D9GpuProgram* program=const_cast<D3D9GpuProgram*>(static_cast<const D3D9GpuProgram*>(target));
        //LPD3DXBUFFER ptr=program->getExternalMicrocode();
        //nothing to do
        return String();
    }
    void D3D9GpuProgram::CmdExternalMicrocode::doSet(void *target, const String& val)
    {
        D3D9GpuProgram* program = const_cast<D3D9GpuProgram*>(static_cast<const D3D9GpuProgram*>(target));
        const void* buffer = val.data();
        program->setExternalMicrocode(buffer, val.size());
    }

    //-----------------------------------------------------------------------------
    D3D9GpuProgram::D3D9GpuProgram(ResourceManager* creator, const String& name, ResourceHandle handle,
        const String& group, bool isManual, ManualResourceLoader* loader) 
        : GpuProgram(creator, name, handle, group, isManual, loader), mExternalMicrocode(NULL), mColumnMajorMatrices(false)
    {           
        if (createParamDictionary("D3D9GpuProgram"))
        {
            setupBaseParamDictionary();

            ParamDictionary* dict = getParamDictionary();
            dict->addParameter(ParameterDef("column_major_matrices", 
                "Whether matrix packing in column-major order.",
                PT_BOOL),&msCmdColumnMajorMatrices);
            dict->addParameter(ParameterDef("external_micro_code", 
                "the cached external micro code data.",
                PT_STRING),&msCmdExternalMicrocode);
        }
    }

    //-----------------------------------------------------------------------------
    D3D9GpuProgram::~D3D9GpuProgram()
    {

    }
    
    //-----------------------------------------------------------------------------
    void D3D9GpuProgram::setExternalMicrocode(const void* pMicrocode, size_t size)
    {
        LPD3DXBUFFER pBuffer=0;
        HRESULT hr=D3DXCreateBuffer(size, &pBuffer);
        if(pBuffer)
        {
            memcpy(pBuffer->GetBufferPointer(), pMicrocode, size);
            this->setExternalMicrocode(pBuffer);
            SAFE_RELEASE(pBuffer);
        }
    }
    //-----------------------------------------------------------------------------
    void D3D9GpuProgram::setExternalMicrocode(ID3DXBuffer* pMicrocode)
    { 
        SAFE_RELEASE(mExternalMicrocode);
        mExternalMicrocode = pMicrocode;
        if(mExternalMicrocode)
        {
            mExternalMicrocode->AddRef();   
        }
    }
    //-----------------------------------------------------------------------------
    LPD3DXBUFFER D3D9GpuProgram::getExternalMicrocode(void)
    {
        return mExternalMicrocode;
    }

    //-----------------------------------------------------------------------------
    void D3D9GpuProgram::loadImpl(void)
    {
        D3D9_DEVICE_ACCESS_CRITICAL_SECTION

        for (uint i = 0; i < D3D9RenderSystem::getResourceCreationDeviceCount(); ++i)
        {
            IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getResourceCreationDevice(i);

            loadImpl(d3d9Device);
        }              
    }

    //-----------------------------------------------------------------------------
    void D3D9GpuProgram::loadImpl(IDirect3DDevice9* d3d9Device)
    {
        D3D9_DEVICE_ACCESS_CRITICAL_SECTION

        if (mExternalMicrocode)
        {
            loadFromMicrocode(d3d9Device, mExternalMicrocode);
        }
        else
        {
            // Normal load-from-source approach
            if (mLoadFromFile)
            {
                // find & load source code
                DataStreamPtr stream = 
                    ResourceGroupManager::getSingleton().openResource(
                    mFilename, mGroup, true, this);
                mSource = stream->getAsString();
            }

            // Call polymorphic load
            loadFromSource(d3d9Device);
        }
    }
    //-----------------------------------------------------------------------------
    void D3D9GpuProgram::unloadImpl(void)
    {
        SAFE_RELEASE(mExternalMicrocode);
    }
    //-----------------------------------------------------------------------------
    void D3D9GpuProgram::loadFromSource(void)
    {
        loadFromSource(NULL);
    }
    //-----------------------------------------------------------------------------
   void D3D9GpuProgram::loadFromSource( IDirect3DDevice9* d3d9Device )
    {
        uint32 hash = getHash();
        if ( GpuProgramManager::getSingleton().isMicrocodeAvailableInCache(hash) )
        {
            getMicrocodeFromCache( d3d9Device, hash );
        }
        else
        {
            compileMicrocode( d3d9Device );
        }
    }
    //-----------------------------------------------------------------------
    void D3D9GpuProgram::getMicrocodeFromCache( IDirect3DDevice9* d3d9Device, uint32 id )
    {
        GpuProgramManager::Microcode cacheMicrocode = 
            GpuProgramManager::getSingleton().getMicrocodeFromCache(id);
        
        LPD3DXBUFFER microcode;
        HRESULT hr=D3DXCreateBuffer(cacheMicrocode->size(), &microcode); 

        if(microcode)
        {
            memcpy(microcode->GetBufferPointer(), cacheMicrocode->getPtr(), cacheMicrocode->size());
        }
        
        loadFromMicrocode(d3d9Device, microcode);
    }
    //-----------------------------------------------------------------------
    void D3D9GpuProgram::compileMicrocode( IDirect3DDevice9* d3d9Device )
    {
        D3D9_DEVICE_ACCESS_CRITICAL_SECTION


        // Populate compile flags
        DWORD compileFlags = 0;

        // Create the shader
        // Assemble source into microcode
        LPD3DXBUFFER microcode;
        LPD3DXBUFFER errors;
        HRESULT hr = D3DXAssembleShader(
            mSource.c_str(),
            static_cast<UINT>(mSource.length()),
            NULL,               // no #define support
            NULL,               // no #include support
            compileFlags,       // standard compile options
            &microcode,
            &errors);

        if (FAILED(hr))
        {
            String message = "Cannot assemble D3D9 shader " + mName + " Errors:\n" +
                static_cast<const char*>(errors->GetBufferPointer());
            errors->Release();
            N_EXCEPT(RenderingAPI, message,
                "D3D9GpuProgram::loadFromSource");
        }
        else if ( GpuProgramManager::getSingleton().getSaveMicrocodesToCache() )
        {
            // create microcode
            GpuProgramManager::Microcode newMicrocode = 
                GpuProgramManager::getSingleton().createMicrocode(microcode->GetBufferSize());

            // save microcode
            memcpy(newMicrocode->getPtr(), microcode->GetBufferPointer(), microcode->GetBufferSize());

            // add to the microcode to the cache
            GpuProgramManager::getSingleton().addMicrocodeToCache(getHash(), newMicrocode);
        }

        loadFromMicrocode(d3d9Device, microcode);       
        
        SAFE_RELEASE(microcode);
        SAFE_RELEASE(errors);
    }
    //-----------------------------------------------------------------------
    GpuProgramParam * D3D9GpuProgram::createParameters(void)
    {
        // Call superclass
        GpuProgramParam * params = GpuProgram::createParameters();

        // Need to transpose matrices if compiled with column-major matrices
        params->setTransposeMatrices(mColumnMajorMatrices);

        return params;
    }   
    //-----------------------------------------------------------------------------
    D3D9GpuVertexProgram::D3D9GpuVertexProgram(ResourceManager* creator, 
        const String& name, ResourceHandle handle, const String& group, 
        bool isManual, ManualResourceLoader* loader) 
        : D3D9GpuProgram(creator, name, handle, group, isManual, loader)       
    {
        mType = GPT_VERTEX_PROGRAM;     
    }
    //-----------------------------------------------------------------------------
    D3D9GpuVertexProgram::~D3D9GpuVertexProgram()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        unload();       
    }
    //-----------------------------------------------------------------------------
    void D3D9GpuVertexProgram::loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode)
    {        
        if (d3d9Device == NULL)
        {
            for (uint i = 0; i < D3D9RenderSystem::getResourceCreationDeviceCount(); ++i)
            {
                IDirect3DDevice9* curD3d9Device = D3D9RenderSystem::getResourceCreationDevice(i);

                loadFromMicrocode(curD3d9Device, microcode);        
            }
        }       

        DeviceToVertexShaderIterator it = mMapDeviceToVertexShader.find(d3d9Device);

        if (it != mMapDeviceToVertexShader.end())
            SAFE_RELEASE(it->second);

        if (isSupported())
        {
            // Create the shader
            IDirect3DVertexShader9* pVertexShader;
            HRESULT hr;
            
            hr = d3d9Device->CreateVertexShader( 
                static_cast<DWORD*>(microcode->GetBufferPointer()), 
                &pVertexShader);

            if (FAILED(hr))
            {
                String errMsg = "Cannot create D3D9 vertex shader " + mName + " from microcode ";
                switch (hr)
                {
                case D3DERR_INVALIDCALL:
                    errMsg += "(D3DERR_INVALIDCALL)";
                    break;
                case D3DERR_OUTOFVIDEOMEMORY:
                    errMsg += "(D3DERR_OUTOFVIDEOMEMORY)";
                    break;
                case E_OUTOFMEMORY:
                    errMsg += "(E_OUTOFMEMORY)";
                    break;
                default:
                    break;
                }               

                N_EXCEPT(RenderingAPI, 
                    errMsg,
                    "D3D9GpuVertexProgram::loadFromMicrocode");
                
            }

            mMapDeviceToVertexShader[d3d9Device] = pVertexShader;
        }
        else
        {
            LogManager::getSingleton().logMessage(
                "Unsupported D3D9 vertex shader '" + mName + "' was not loaded.");

            mMapDeviceToVertexShader[d3d9Device] = NULL;
        }
    }
    //-----------------------------------------------------------------------------
    void D3D9GpuVertexProgram::unloadImpl(void)
    {
        D3D9_DEVICE_ACCESS_CRITICAL_SECTION

        DeviceToVertexShaderIterator it = mMapDeviceToVertexShader.begin();

        while (it != mMapDeviceToVertexShader.end())
        {
            SAFE_RELEASE(it->second);
            ++it;
        }
        mMapDeviceToVertexShader.clear();       
        D3D9GpuProgram::unloadImpl();
    }

    //-----------------------------------------------------------------------------
    void D3D9GpuVertexProgram::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
    {
        
    }

    //-----------------------------------------------------------------------------
    void D3D9GpuVertexProgram::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
    {
        D3D9_DEVICE_ACCESS_CRITICAL_SECTION

        DeviceToVertexShaderIterator it;

        // Find the shader of this device.
        it = mMapDeviceToVertexShader.find(d3d9Device);

        // Case shader found -> release it and erase from map.
        if (it != mMapDeviceToVertexShader.end())
        {
            SAFE_RELEASE(it->second);
            mMapDeviceToVertexShader.erase(it);
        }
    }

    //-----------------------------------------------------------------------------
    IDirect3DVertexShader9* D3D9GpuVertexProgram::getVertexShader( void )
    {
        IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
        DeviceToVertexShaderIterator it;

        // Find the shader of this device.
        it = mMapDeviceToVertexShader.find(d3d9Device);
        
        // Shader was not found -> load it.
        if (it == mMapDeviceToVertexShader.end())       
        {
            loadImpl(d3d9Device);       
            it = mMapDeviceToVertexShader.find(d3d9Device);
        }
    
        return it->second;
    }

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    D3D9GpuFragmentProgram::D3D9GpuFragmentProgram(ResourceManager* creator, 
        const String& name, ResourceHandle handle, const String& group, 
        bool isManual, ManualResourceLoader* loader) 
        : D3D9GpuProgram(creator, name, handle, group, isManual, loader)       
    {
        mType = GPT_FRAGMENT_PROGRAM;
    }
    //-----------------------------------------------------------------------------
    D3D9GpuFragmentProgram::~D3D9GpuFragmentProgram()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        unload(); 
    }
    //-----------------------------------------------------------------------------
    void D3D9GpuFragmentProgram::loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode)
    {
        DeviceToPixelShaderIterator it = mMapDeviceToPixelShader.find(d3d9Device);

        if (it != mMapDeviceToPixelShader.end())
            SAFE_RELEASE(it->second);

        if (isSupported())
        {
            // Create the shader
            IDirect3DPixelShader9* pPixelShader;
            HRESULT hr;

            hr = d3d9Device->CreatePixelShader(
                static_cast<DWORD*>(microcode->GetBufferPointer()), 
                &pPixelShader);

            if (FAILED(hr))
            {
                N_EXCEPT(RenderingAPI, 
                    "Cannot create D3D9 pixel shader " + mName + " from microcode.",
                    "D3D9GpuFragmentProgram::loadFromMicrocode");
                
            }

            mMapDeviceToPixelShader[d3d9Device] = pPixelShader;
        }
        else
        {
            LogManager::getSingleton().logMessage(
                "Unsupported D3D9 pixel shader '" + mName + "' was not loaded.");

            mMapDeviceToPixelShader[d3d9Device] = NULL;
        }
    }
    //-----------------------------------------------------------------------------
    void D3D9GpuFragmentProgram::unloadImpl(void)
    {
        D3D9_DEVICE_ACCESS_CRITICAL_SECTION

        DeviceToPixelShaderIterator it = mMapDeviceToPixelShader.begin();

        while (it != mMapDeviceToPixelShader.end())
        {
            SAFE_RELEASE(it->second);
            ++it;
        }
        mMapDeviceToPixelShader.clear();    
        D3D9GpuProgram::unloadImpl();
    }
    //-----------------------------------------------------------------------------
    void D3D9GpuFragmentProgram::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
    {
        
    }

    //-----------------------------------------------------------------------------
    void D3D9GpuFragmentProgram::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
    {
        D3D9_DEVICE_ACCESS_CRITICAL_SECTION

        DeviceToPixelShaderIterator it;

        // Find the shader of this device.
        it = mMapDeviceToPixelShader.find(d3d9Device);

        // Case shader found -> release it and erase from map.
        if (it != mMapDeviceToPixelShader.end())
        {
            SAFE_RELEASE(it->second);
            mMapDeviceToPixelShader.erase(it);
        }
    }

    //-----------------------------------------------------------------------------
    IDirect3DPixelShader9* D3D9GpuFragmentProgram::getPixelShader( void )
    {
        IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
        DeviceToPixelShaderIterator it;

        // Find the shader of this device.
        it = mMapDeviceToPixelShader.find(d3d9Device);

        // Shader was not found -> load it.
        if (it == mMapDeviceToPixelShader.end())        
        {
            loadImpl(d3d9Device);           
            it = mMapDeviceToPixelShader.find(d3d9Device);
        }

        return it->second;
    }
    //-----------------------------------------------------------------------------

}

