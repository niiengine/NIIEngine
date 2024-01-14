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
#include "OgreD3D11VertexDeclaration.h"
#include "OgreD3D11Mappings.h"
#include "OgreD3D11HLSLProgram.h"
#include "OgreD3D11Device.h"


namespace Ogre {

    //-----------------------------------------------------------------------
    D3D11VertexDeclaration::D3D11VertexDeclaration(D3D11Device &  device) 
        : mlpD3DDevice(device)
    {
    }
    //-----------------------------------------------------------------------
    D3D11VertexDeclaration::~D3D11VertexDeclaration()
    {
    }
    //-----------------------------------------------------------------------
    void D3D11VertexDeclaration::notifyChanged()
    {
        clearCache();
    }
    //-----------------------------------------------------------------------
    void D3D11VertexDeclaration::notifyDeviceLost(D3D11Device* device)
    {
        clearCache();
    }
    //-----------------------------------------------------------------------
    void D3D11VertexDeclaration::notifyDeviceRestored(D3D11Device* device)
    {
    }
    //-----------------------------------------------------------------------
    void D3D11VertexDeclaration::clearCache()
    {
        mD3delems.clear();
        mShaderToILayoutMap.clear();
    }
    //-----------------------------------------------------------------------
    D3D11_INPUT_ELEMENT_DESC * D3D11VertexDeclaration::getD3DVertexDeclaration(D3D11HLSLProgram* boundVertexProgram, VertexBufferBinding* binding)
    {
        // Create D3D elements
        size_t iNumElements = boundVertexProgram->getNumInputs();

        if (mD3delems.find(boundVertexProgram) == mD3delems.end())
        {
            std::vector<D3D11_INPUT_ELEMENT_DESC> D3delems;

            unsigned int idx;
            for (idx = 0; idx < iNumElements; ++idx)
            {
                D3D11_SIGNATURE_PARAMETER_DESC inputDesc = boundVertexProgram->getInputParamDesc(idx);
                VertexElementList::const_iterator i, iend;
                iend = mElementList.end();
                bool found = false;
                for (i = mElementList.begin(); i != iend; ++i)
                {
                    LPCSTR semanticName         = D3D11Mappings::get(i->getSemantic());
                    UINT semanticIndex          = i->getIndex();
                    if(
                        strcmp(semanticName, inputDesc.SemanticName) == 0
                        && semanticIndex == inputDesc.SemanticIndex
                      )
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    N_EXCEPT(RenderingAPI,
                                StringUtil::format("No VertexElement for semantic %s in shader %s found",
                                                   inputDesc.SemanticName, boundVertexProgram->getName().c_str()));
                }

                D3D11_INPUT_ELEMENT_DESC elem = {};
                elem.SemanticName          = inputDesc.SemanticName;
                elem.SemanticIndex         = inputDesc.SemanticIndex;
                elem.Format                = D3D11Mappings::get(i->getType());
                elem.InputSlot             = i->getSource();
                elem.AlignedByteOffset     = static_cast<WORD>(i->getOffset());
                elem.InputSlotClass        = D3D11_INPUT_PER_VERTEX_DATA;
                elem.InstanceDataStepRate  = 0;

                VertexBufferBinding::VertexBufferBindingMap::const_iterator foundIter;
                foundIter = binding->getBindings().find(i->getSource());
                if ( foundIter != binding->getBindings().end() )
                {
                    HardwareVertexBufferSharedPtr bufAtSlot = foundIter->second;
                    if ( bufAtSlot->isInstanceData() )
                    {
                        elem.InputSlotClass        = D3D11_INPUT_PER_INSTANCE_DATA;
                        elem.InstanceDataStepRate  = bufAtSlot->getInstanceDataStepRate();
                    }
                }
                else
                {
                    N_EXCEPT(RenderingAPI, 
                        "Unable to found a bound vertex for a slot that is used in the vertex declaration." , 
                        "D3D11VertexDeclaration::getD3DVertexDeclaration");

                }
                D3delems.push_back(elem);
            }

            mD3delems[boundVertexProgram].swap(D3delems);

        }

        return mD3delems[boundVertexProgram].data();
    }
    //-----------------------------------------------------------------------
    ID3D11InputLayout*  D3D11VertexDeclaration::getILayoutByShader(D3D11HLSLProgram* boundVertexProgram, VertexBufferBinding* binding)
    {
        ShaderToILayoutMapIterator foundIter = mShaderToILayoutMap.find(boundVertexProgram);

        ComPtr<ID3D11InputLayout> pVertexLayout;

        if (foundIter == mShaderToILayoutMap.end())
        {
            // if not found - create

            DWORD dwShaderFlags = 0;
            const MicroCode &  vSBuf = boundVertexProgram->getMicroCode();

            D3D11_INPUT_ELEMENT_DESC * pVertexDecl=getD3DVertexDeclaration(boundVertexProgram, binding);

            // bad bug tracing. see what will happen next.
            //if (pVertexDecl->Format == DXGI_FORMAT_R16G16_SINT)
            //  pVertexDecl->Format = DXGI_FORMAT_R16G16_FLOAT;
            HRESULT hr = mlpD3DDevice->CreateInputLayout( 
                pVertexDecl, 
                boundVertexProgram->getNumInputs(), 
                &vSBuf[0], 
                vSBuf.size(),
                pVertexLayout.ReleaseAndGetAddressOf() );

            if (FAILED(hr)|| mlpD3DDevice.isError())
            {
				String errorDescription = mlpD3DDevice.getErrorString(hr);
                errorDescription += "\nBound shader name: " + boundVertexProgram->getName();

				N_EXCEPT_EX(RenderingAPI, hr,
					"Unable to set D3D11 vertex declaration" + errorDescription,
                    "D3D11VertexDeclaration::getILayoutByShader");
            }

            mShaderToILayoutMap[boundVertexProgram] = pVertexLayout;

        }
        else
        {
            pVertexLayout = foundIter->second;
        }

        return pVertexLayout.Get(); // lifetime is determined by map
    }
    //-----------------------------------------------------------------------
    void D3D11VertexDeclaration::bindToShader(D3D11HLSLProgram* boundVertexProgram, VertexBufferBinding* binding)
    {
        // Set the input layout
        ID3D11InputLayout*  pVertexLayout = getILayoutByShader(boundVertexProgram, binding);
        mlpD3DDevice.GetImmediateContext()->IASetInputLayout( pVertexLayout);
    }   
}


