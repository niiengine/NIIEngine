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
#include "OgreCgProgram.h"
#include "OgreGpuProgramManager.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreStringConverter.h"
#include "OgreLogManager.h"

namespace Ogre 
{
    //-----------------------------------------------------------------------
    CgProgram::CmdEntryPoint CgProgram::msCmdEntryPoint;
    CgProgram::CmdProfiles CgProgram::msCmdProfiles;
    CgProgram::CmdArgs CgProgram::msCmdArgs;
    //-----------------------------------------------------------------------
    void CgProgram::selectProfile(void)
    {
        mSelectedProfile.clear();
        mSelectedCgProfile = CG_PROFILE_UNKNOWN;

        StringList::iterator i, iend = mProfiles.end();
        GpuProgramManager& gpuMgr = GpuProgramManager::getSingleton();
        for(i = mProfiles.begin(); i != iend; ++i)
        {
            if(gpuMgr.isSyntaxSupported(*i))
            {
                mSelectedProfile = *i;
                mSelectedCgProfile = cgGetProfile(mSelectedProfile.c_str());
                // Check for errors
                checkForCgError("CgProgram::selectProfile",
                    "Unable to find CG profile enum for program " + mName + ": ", mCgContext);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void CgProgram::buildArgs(void)
    {
        StringList args;
        if(!mCompileArgs.empty())
            args = StringUtil::split(mCompileArgs);

        StringList::const_iterator i;
        if(mSelectedCgProfile == CG_PROFILE_VS_1_1)
        {
            // Need the 'dcls' argument whenever we use this profile
            // otherwise compilation of the assembler will fail
            bool dclsFound = false;
            for(i = args.begin(); i != args.end(); ++i)
            {
                if(*i == "dcls")
                {
                    dclsFound = true;
                    break;
                }
            }
            if(!dclsFound)
            {
                args.push_back("-profileopts");
				args.push_back("dcls");
            }
        }
        // Now split args into that god-awful char** that Cg insists on
        freeCgArgs();
        mCgArguments = OGRE_ALLOC_T(char*, args.size() + 1, MEMCATEGORY_RESOURCE);
        int index = 0;
        for(i = args.begin(); i != args.end(); ++i, ++index)
        {
            mCgArguments[index] = OGRE_ALLOC_T(char, i->length() + 1, MEMCATEGORY_RESOURCE);
            strcpy(mCgArguments[index], i->c_str());
        }
        // Null terminate list
        mCgArguments[index] = 0;
    }
    //-----------------------------------------------------------------------
    void CgProgram::freeCgArgs(void)
    {
        if(mCgArguments)
        {
            size_t index = 0;
            char * current = mCgArguments[index];
            while(current)
            {
                OGRE_FREE(current, MEMCATEGORY_RESOURCE);
				mCgArguments[index] = 0;
                current = mCgArguments[++index];
            }
            OGRE_FREE(mCgArguments, MEMCATEGORY_RESOURCE);
            mCgArguments = 0;
        }
    }
    //-----------------------------------------------------------------------
    void CgProgram::loadCodeImpl(const String & code)
    {
	    selectProfile();

		if(GpuProgramManager::getSingleton().isCacheExist(String("CG_") + mName) )
		{
			getMicrocodeFromCache();
		}
		else
		{
			compileMicrocode();
		}
	}
    //-----------------------------------------------------------------------
    void CgProgram::getMicrocodeFromCache()
    {
		GpuProgramManager::MemDataStream * cacheMicrocode =
			GpuProgramManager::getSingleton().getCache(String("CG_") + mName);

		cacheMicrocode->seek(0);

		// get size of string
		size_t programStringSize = 0;
		cacheMicrocode->read(&programStringSize, sizeof(size_t));

		// get microcode
		mProgramString.resize(programStringSize);
		cacheMicrocode->read(&mProgramString[0], programStringSize);

		// get size of param map
		size_t parametersMapSize = 0;
		cacheMicrocode->read(&parametersMapSize, sizeof(size_t));

		// get params
		for(size_t i = 0 ; i < parametersMapSize ; i++)
		{
			String paramName;
			size_t stringSize = 0;
			GpuParamUnit def;

			// get string size
			cacheMicrocode->read(&stringSize, sizeof(size_t));

			// get string
			paramName.resize(stringSize);
			cacheMicrocode->read(&paramName[0], stringSize);

			// get def
			cacheMicrocode->read( &def, sizeof(GpuParamUnit));

			mParametersMap.insert(GpuParamUnitList::value_type(paramName, def));
		}
	}
    //-----------------------------------------------------------------------
    void CgProgram::compileMicrocode(void)
    {
        // Create Cg Program

        /// Program handle
        CGprogram cgProgram;

		if(mSelectedCgProfile == CG_PROFILE_UNKNOWN)
		{
			LogManager::getSingleton().logMessage(
				"Attempted to load Cg program '" + mName + "', but no suported "
                    "profile was found. ");
			return;
		}
        buildArgs();
		// deal with includes
		String sourceToUse = resolveCgIncludes(mSource, this, mFile);

        cgProgram = cgCreateProgram(mCgContext, CG_SOURCE, sourceToUse.c_str(),
            mSelectedCgProfile, mEntryPoint.c_str(), const_cast<const char**>(mCgArguments));

        // Test
        //LogManager::getSingleton().logMessage(cgGetProgramString(mCgProgram, CG_COMPILED_PROGRAM));

        // Check for errors
        checkForCgError("CgProgram::compileMicrocode",
            "Unable to compile Cg program " + mName + ": ", mCgContext);

        CGerror error = cgGetError();
        if(error == CG_NO_ERROR)
        {
			// get program string (result of cg compile)
			mProgramString = cgGetProgramString(cgProgram, CG_COMPILED_PROGRAM);

			// get params
			mParametersMap.clear();
			recurseParams(cgGetFirstParameter(cgProgram, CG_PROGRAM));
			recurseParams(cgGetFirstParameter(cgProgram, CG_GLOBAL));

			// Unload Cg Program - we don't need it anymore
			cgDestroyProgram(cgProgram);
			checkForCgError("CgProgram::unloadImpl",
				"Error while unloading Cg program " + mName + ": ", mCgContext);
			cgProgram = 0;

			if(GpuProgramManager::getSingleton().isCacheBinary())
			{
				addMicrocodeToCache();
			}
		}
    }
    //-----------------------------------------------------------------------
	void CgProgram::addMicrocodeToCache()
	{
		String name = String("CG_") + mName;
		size_t programStringSize = mProgramString.size();
        size_t sizeOfMicrocode = sizeof(size_t) +   // size of mProgramString
							     programStringSize + // microcode - mProgramString
							     sizeof(size_t) + // size of param map
							     mParametersMapSizeAsBuffer;

		// create microcode
        MemDataStream * newMicrocode =
            GpuProgramManager::getSingleton().createCache(name, sizeOfMicrocode);

		newMicrocode->seek(0);

		// save size of string
		newMicrocode->write(&programStringSize, sizeof(size_t));

		// save microcode
		newMicrocode->write(&mProgramString[0], programStringSize);

		// save size of param map
		size_t parametersMapSize = mParametersMap.size();
		newMicrocode->write(&parametersMapSize, sizeof(size_t));

		// save params
		GpuParamUnitList::const_iterator iter = mParametersMap.begin();
		GpuParamUnitList::const_iterator iterE = mParametersMap.end();
		for(; iter != iterE ; iter++)
		{
			const String & paramName = iter->first;
			const GpuParamUnit & def = iter->second;

			// save string size
			size_t stringSize = paramName.size();
			newMicrocode->write(&stringSize, sizeof(size_t));

			// save string
			newMicrocode->write(&paramName[0], stringSize);

			// save def
			newMicrocode->write(&def, sizeof(GpuParamUnit));
		}
	}
    //-----------------------------------------------------------------------
    void CgProgram::genAssemblerImpl(void)
    {
		// ignore any previous error
		if(mSelectedCgProfile != CG_PROFILE_UNKNOWN && !mErrorCode)
		{
			if(false
// the hlsl 4 profiles are only supported in OGRE from CG 2.2
#if(CG_VERSION_NUM >= 2200)
				|| mSelectedCgProfile == CG_PROFILE_VS_4_0
				 || mSelectedCgProfile == CG_PROFILE_PS_4_0
#endif
				 )
			{
				// Create a high-level program, give it the same name as us
				HighLevelGpuProgram * vp = HighLevelGpuProgramManager::getSingleton().createProgram(
                    mName, mGroup, "hlsl", mType);
				vp->setProgramCode(mProgramString);
				vp->set("target", mSelectedProfile);
				vp->set("entry_point", "main");

				vp->ref();

				mAssemblerCode = vp;
			}
			else
			{
                if(mType == GPT_FRAGMENT_PROGRAM) 
                {
                    //HACK : http://developer.nvidia.com/forums/index.php?showtopic=1063&pid=2378&mode=threaded&start=#entry2378
                    //Still happens in CG 2.2. Remove hack when fixed.
                    mProgramString = StringUtil::replaceAll(mProgramString, "oDepth.z", "oDepth");
                }
				// Create a low-level program, give it the same name as us
				mAssemblerCode = N_Only(GpuProgram).create(mName, mGroup, mType, mSelectedProfile, mProgramString);
                mAssemblerCode->ref(false);
			}
			// Shader params need to be forwarded to low level implementation
			mAssemblerCode->setAdjacencyPrimitive(isAdjacencyPrimitive());
		}
    }
    //-----------------------------------------------------------------------
    void CgProgram::unloadSysCodeImpl()
    {
    }
    //-----------------------------------------------------------------------
    void CgProgram::genParamDefImpl()
    {
        // Derive parameter names from Cg
		createParamMap();

		if(mProgramString.empty())
			return;

		mDefines->mFloatSize = mBindMap->mFloatSize;
		mDefines->mIntSize = mBindMap->mIntSize;

		GpuParamUnitList::const_iterator iter = mParametersMap.begin();
		GpuParamUnitList::const_iterator iterE = mParametersMap.end();
		for(; iter != iterE ; iter++)
		{
			const String & paramName = iter->first;
			const GpuParamUnit & def = iter->second;

			// Record logical / physical mapping
			if(def.isFloat())
			{
				mBindMap->mFloatList.insert(
					GpuParamBlockMap::value_type(def.mIndex,
						GpuParamBlock(def.mIndex, def.mMemIndex, def.mUnitCount * def.mUnitSize, GPT_Render)));
				mBindMap->mFloatSize += def.mUnitCount * def.mUnitSize;
			}
			else
			{
				mBindMap->mIntList.insert(
					GpuParamBlockMap::value_type(def.mIndex,
						GpuParamBlock(def.mIndex, def.mMemIndex, def.mUnitCount * def.mUnitSize, GPT_Render)));
				mBindMap->mIntSize += def.mUnitCount * def.mUnitSize;
			}
			mDefines->add(paramName, def);
		}
	}
	//---------------------------------------------------------------------
	void CgProgram::recurseParams(CGparameter parameter, size_t contextArraySize)
	{
		while(parameter != 0)
        {
            // Look for uniform (non-sampler) parameters only
            // Don't bother enumerating unused parameters, especially since they will
            // be optimised out and therefore not in the indexed versions
            CGtype paramType = cgGetParameterType(parameter);

            if(cgGetParameterVariability(parameter) == CG_UNIFORM &&
                paramType != CG_SAMPLER1D &&
                paramType != CG_SAMPLER2D &&
                paramType != CG_SAMPLER3D &&
                paramType != CG_SAMPLERCUBE &&
                paramType != CG_SAMPLERRECT &&
                cgGetParameterDirection(parameter) != CG_OUT &&
                cgIsParameterReferenced(parameter))
            {
				int arraySize;

				switch(paramType)
				{
				case CG_STRUCT:
					recurseParams(cgGetFirstStructParameter(parameter));
					break;
				case CG_ARRAY:
					// Support only 1-dimensional arrays
					arraySize = cgGetArraySize(parameter, 0);
					recurseParams(cgGetArrayParameter(parameter, 0), (size_t)arraySize);
					break;
				default:
					// Normal path (leaf)
					String paramName = cgGetParameterName(parameter);
					size_t logicalIndex = cgGetParameterResourceIndex(parameter);

					// Get the parameter resource, to calculate the physical index
					CGresource res = cgGetParameterResource(parameter);
					bool isRegisterCombiner = false;
					size_t regCombinerPhysicalIndex = 0;
					switch (res)
					{
					case CG_COMBINER_STAGE_CONST0:
						// register combiner, const 0
						// the index relates to the texture stage; store this as (stage * 2) + 0
						regCombinerPhysicalIndex = logicalIndex * 2;
						isRegisterCombiner = true;
						break;
					case CG_COMBINER_STAGE_CONST1:
						// register combiner, const 1
						// the index relates to the texture stage; store this as (stage * 2) + 1
						regCombinerPhysicalIndex = (logicalIndex * 2) + 1;
						isRegisterCombiner = true;
						break;
					default:
						// normal constant
						break;
					}

					// Trim the '[0]' suffix if it exists, we will add our own indexing later
					if (StringUtil::endsWith(paramName, "[0]", false))
					{
						paramName.erase(paramName.size() - 3);
					}


					GpuParamUnit def;
					def.mUnitCount = contextArraySize;
					mapTypeAndElementSize(paramType, isRegisterCombiner, def);

					if(def.mDataType == GDT_Unknow)
					{
						LogManager::getSingleton().logMessage("Problem parsing the following Cg Uniform: '" + paramName + "' in file " + mName);
						// next uniform
						parameter = cgGetNextParameter(parameter);
						continue;
					}
					if (isRegisterCombiner)
					{
						def.mMemIndex = regCombinerPhysicalIndex;
					}
					else
					{
						// base position on existing buffer contents
						if(def.isFloat())
						{
							def.mMemIndex = mBindMap->mFloatSize;
						}
						else
						{
							def.mMemIndex = mBindMap->mIntSize;
						}
					}

					def.mIndex = logicalIndex;
					if( mParametersMap.find(paramName) == mParametersMap.end())
					{
						mParametersMap.insert(GpuParamUnitList::value_type(paramName, def));
						mParametersMapSizeAsBuffer += sizeof(size_t);
						mParametersMapSizeAsBuffer += paramName.size();
						mParametersMapSizeAsBuffer += sizeof(GpuParamUnit);
					}

					// Record logical / physical mapping
					if(def.isFloat())
					{
						mBindMap->mFloatList.insert(Npair(def.mIndex, GpuParamBlock(def.mIndex, def.mMemIndex, def.mUnitCount * def.mUnitSize, GPT_Render)));
						mBindMap->mFloatSize += def.mUnitCount * def.mUnitSize;
					}
					else
					{
						mBindMap->mIntList.insert(Npair(def.mIndex, GpuParamBlock(def.mIndex, def.mMemIndex, def.mUnitCount * def.mUnitSize, GPT_Render)));
						mBindMap->mIntSize += def.mUnitCount * def.mUnitSize;
					}

					break;
				}
            }
            // Get next
            parameter = cgGetNextParameter(parameter);
        }
    }
	//-----------------------------------------------------------------------
	void CgProgram::mapTypeAndElementSize(CGtype cgType, bool isRegisterCombiner,
		GpuParamUnit& def) const
	{
		if (isRegisterCombiner)
		{
			// register combiners are the only single-float entries in our buffer
			def.mDataType = GDT_Float;
			def.mUnitSize = 1;
		}
		else
		{
			switch(cgType)
			{
			case CG_FLOAT:
			case CG_FLOAT1:
			case CG_HALF:
			case CG_HALF1:
				def.mDataType = GDT_Float;
				break;
			case CG_FLOAT2:
			case CG_HALF2:
				def.mDataType = GDT_Float2X;
				break;
			case CG_FLOAT3:
			case CG_HALF3:
				def.mDataType = GDT_Float3X;
				break;
			case CG_FLOAT4:
			case CG_HALF4:
				def.mDataType = GDT_Float4X;
				break;
			case CG_FLOAT2x2:
			case CG_HALF2x2:
				def.mDataType = GDT_Matrix2X2;
				break;
			case CG_FLOAT2x3:
			case CG_HALF2x3:
				def.mDataType = GDT_Matrix2X3;
				break;
			case CG_FLOAT2x4:
			case CG_HALF2x4:
				def.mDataType = GDT_Matrix2X4;
				break;
			case CG_FLOAT3x2:
			case CG_HALF3x2:
				def.mDataType = GDT_Matrix3X2;
				break;
			case CG_FLOAT3x3:
			case CG_HALF3x3:
				def.mDataType = GDT_Matrix3X3;
				break;
			case CG_FLOAT3x4:
			case CG_HALF3x4:
				def.mDataType = GDT_Matrix3X4;
				break;
			case CG_FLOAT4x2:
			case CG_HALF4x2:
				def.mDataType = GDT_Matrix4X2;
				break;
			case CG_FLOAT4x3:
			case CG_HALF4x3:
				def.mDataType = GDT_Matrix4X3;
				break;
			case CG_FLOAT4x4:
			case CG_HALF4x4:
				def.mDataType = GDT_Matrix4X4;
				break;
			case CG_INT:
			case CG_INT1:
				def.mDataType = GDT_Int;
				break;
			case CG_INT2:
				def.mDataType = GDT_Int2X;
				break;
			case CG_INT3:
				def.mDataType = GDT_Int3X;
				break;
			case CG_INT4:
				def.mDataType = GDT_Int4X;
				break;
			default:
				def.mDataType = GDT_Unknow;
				break;
			}
			// Cg pads
			def.mUnitSize = GpuProgramParam::get32bCount(def.mDataType, true);
		}
	}
    //-----------------------------------------------------------------------
    CgProgram::CgProgram(ResourceManager* creator, const String& name,
        ResourceHandle handle, const String& group, bool isManual,
        ManualResourceLoader* loader, CGcontext context)
        : HighLevelGpuProgram(creator, name, handle, group, isManual, loader),
        mCgContext(context),
        mSelectedCgProfile(CG_PROFILE_UNKNOWN), mCgArguments(0), mParametersMapSizeAsBuffer(0)
    {
        if (createParamDictionary("CgProgram"))
        {
            setupBaseParamDictionary();

            PropertyCmdSet * dict = getCmdObj();

            dict->addParameter(ParameterDef("entry_point",
                "The entry point for the Cg program.",
                PT_STRING),&msCmdEntryPoint);
            dict->addParameter(ParameterDef("profiles",
                "Space-separated list of Cg profiles supported by this profile.",
                PT_STRING),&msCmdProfiles);
            dict->addParameter(ParameterDef("compile_arguments",
                "A string of compilation arguments to pass to the Cg compiler.",
                PT_STRING),&msCmdArgs);
        }

    }
    //-----------------------------------------------------------------------
    CgProgram::~CgProgram()
    {
        freeCgArgs();
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        if(mState == LS_LOADED)
        {
            unload();
        }
        else
        {
            unloadSysCode();
        }
    }
    //-----------------------------------------------------------------------
    bool CgProgram::isValid() const
    {
        if((mErrorCode != 0) || !isSysSupport())
            return false;

		StringList::const_iterator i, iend;
        iend = mProfiles.end();
        // Check to see if any of the profiles are supported
        for (i = mProfiles.begin(); i != iend; ++i)
        {
            if (GpuProgramManager::getSingleton().isSyntaxSupported(*i))
            {
                return true;
            }
        }
        return false;

    }
    //-----------------------------------------------------------------------
    void CgProgram::setProfiles(const StringList& profiles)
    {
        mProfiles.clear();
        StringList::const_iterator i, iend = profiles.end();
        for(i = profiles.begin(); i != iend; ++i)
        {
            mProfiles.push_back(*i);
        }
    }
	//-----------------------------------------------------------------------
	String CgProgram::resolveCgIncludes(const String & inSource, 
        Resource * resourceBeingLoaded, const String & fileName)
	{
		String outSource;
		// output will be at least this big
		outSource.reserve(inSource.length());

		size_t startMarker = 0;
		size_t i = inSource.find("#include");
		while (i != String::npos)
		{
			size_t includePos = i;
			size_t afterIncludePos = includePos + 8;
			size_t newLineBefore = inSource.rfind("\n", includePos);

			// check we're not in a comment
			size_t lineCommentIt = inSource.rfind("//", includePos);
			if (lineCommentIt != String::npos)
			{
				if (newLineBefore == String::npos || lineCommentIt > newLineBefore)
				{
					// commented
					i = inSource.find("#include", afterIncludePos);
					continue;
				}

			}
			size_t blockCommentIt = inSource.rfind("/*", includePos);
			if (blockCommentIt != String::npos)
			{
				size_t closeCommentIt = inSource.rfind("*/", includePos);
				if (closeCommentIt == String::npos || closeCommentIt < blockCommentIt)
				{
					// commented
					i = inSource.find("#include", afterIncludePos);
					continue;
				}

			}

			// find following newline (or EOF)
			size_t newLineAfter = inSource.find("\n", afterIncludePos);
			// find include file string container
			String endDelimeter = "\"";
			size_t startIt = inSource.find("\"", afterIncludePos);
			if (startIt == String::npos || startIt > newLineAfter)
			{
				// try <>
				startIt = inSource.find("<", afterIncludePos);
				if (startIt == String::npos || startIt > newLineAfter)
				{
					N_EXCEPT(Exception::ERR_INTERNAL_ERROR,
						"Badly formed #include directive (expected \" or <) in file "
						+ fileName + ": " + inSource.substr(includePos, newLineAfter-includePos),
						"CgProgram::preprocessor");
				}
				else
				{
					endDelimeter = ">";
				}
			}
			size_t endIt = inSource.find(endDelimeter, startIt+1);
			if (endIt == String::npos || endIt <= startIt)
			{
				N_EXCEPT(Exception::ERR_INTERNAL_ERROR,
					"Badly formed #include directive (expected " + endDelimeter + ") in file "
					+ fileName + ": " + inSource.substr(includePos, newLineAfter-includePos),
					"CgProgram::preprocessor");
			}

			// extract filename
			String filename(inSource.substr(startIt+1, endIt-startIt-1));

			// open included file
			DataStream * resource = N_Only(ResourceScheme).
				openResource(filename, resourceBeingLoaded->getGroup(), true, resourceBeingLoaded);

			// replace entire include directive line
			// copy up to just before include
			if (newLineBefore != String::npos && newLineBefore >= startMarker)
				outSource.append(inSource.substr(startMarker, newLineBefore-startMarker+1));

			size_t lineCount = 0;
			size_t lineCountPos = 0;

			// Count the line number of #include statement
			lineCountPos = outSource.find('\n');
			while(lineCountPos != String::npos)
			{
				lineCountPos = outSource.find('\n', lineCountPos+1);
				lineCount++;
			}

			// Add #line to the start of the included file to correct the line count
			outSource.append("#line 1 \"" + filename + "\"\n");

			outSource.append(resource->getData(), resource->size());
            N_delete resource;
			// Add #line to the end of the included file to correct the line count
			outSource.append("\n#line " + Ogre::StrConv::conv(lineCount) +
				"\"" + fileName + "\"\n");

			startMarker = newLineAfter;

			if(startMarker != String::npos)
				i = inSource.find("#include", startMarker);
			else
				i = String::npos;

		}
		// copy any remaining characters
		outSource.append(inSource.substr(startMarker));

		return outSource;
	}
    //-----------------------------------------------------------------------
    const String & CgProgram::getLanguage(void) const
    {
        static const String language = "cg";

        return language;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String CgProgram::CmdEntryPoint::doGet(const void *target) const
    {
        return static_cast<const CgProgram*>(target)->getEntryPoint();
    }
    void CgProgram::CmdEntryPoint::doSet(void *target, const String& val)
    {
        static_cast<CgProgram *>(target)->setEntryPoint(val);
    }
    //-----------------------------------------------------------------------
    String CgProgram::CmdProfiles::doGet(const void * target) const
    {
        return StrConv::conv(
            static_cast<const CgProgram *>(target)->getProfiles() );
    }
    void CgProgram::CmdProfiles::doSet(void * target, const String & val)
    {
        static_cast<CgProgram *>(target)->setProfiles(StringUtil::split(val));
    }
    //-----------------------------------------------------------------------
    String CgProgram::CmdArgs::doGet(const void * target) const
    {
        return static_cast<const CgProgram *>(target)->getCompileArguments();
    }
    void CgProgram::CmdArgs::doSet(void * target, const String & val)
    {
        static_cast<CgProgram *>(target)->setCompileArguments(val);
    }

}
