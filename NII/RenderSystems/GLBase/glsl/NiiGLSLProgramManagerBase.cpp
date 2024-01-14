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

#include "NiiGLSLProgramManagerBase.h"
#include "NiiGLSLProgramBase.h"
#include "NiiGpuProgramParam.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"

namespace NII 
{
    //------------------------------------------------------------------------
    GLSLProgramManagerBase::~GLSLProgramManagerBase()
    {
        // iterate through map container and delete link programs
        for (ProgramIterator cp = mPrograms.begin(); cp != mPrograms.end(); ++cp)
        {
            N_delete cp->second;
        }
    }
    //------------------------------------------------------------------------
    void GLSLProgramManagerBase::destroyAllByShader(GLSLShaderBase * shader)
    {
        std::vector<Nui32> keysToErase;
        for (ProgramIterator cp = mPrograms.begin(); cp != mPrograms.end(); ++cp)
        {
            GLSLProgramBase * prgm = cp->second;
            if(prgm->isUsingShader(shader))
            {
                N_delete prgm;
                keysToErase.push_back(cp->first);
            }
        }

        for(size_t i = 0; i < keysToErase.size(); ++i)
        {
            mPrograms.erase(mPrograms.find(keysToErase[i]));
        }
    }
    //------------------------------------------------------------------------
    void GLSLProgramManagerBase::parseGLSLUniform(VString line, GpuParamDefine & defs, const String & filename, GpuParamValue * sharedParams)
    {
        GpuParamUnit def;
        VString paramName = "";

        // Remove spaces before opening square braces, otherwise
        // the following split() can split the line at inappropriate
        // places (e.g. "vec3 something [3]" won't work).
        //FIXME What are valid ways of including spaces in GLSL
        // variable declarations?  May need regex.
        for (VString::size_type sqp = line.find(" ["); sqp != String::npos; sqp = line.find(" ["))
            line.erase(sqp, 1);

        // Split into tokens
        VStringList parts;
        StrUtil::split(line, parts, ", \t\r\n");
        for (VStringList::iterator i = parts.begin(); i != parts.end(); ++i)
        {
            // Is this a type?
            StringToEnumMap::iterator typei = mTypeEnumMap.find(*i);
            if (typei != mTypeEnumMap.end())
            {
                convertGLUniformtoOgreType(typei->second, def);
            }
            else
            {
                // if this is not a type, and not empty, it should be a name
                StrUtil::trim(*i);
                if (i->empty()) 
                    continue;

                // Skip over precision keywords
                if(StrUtil::match((*i), "lowp") || StrUtil::match((*i), "mediump") || StrUtil::match((*i), "highp"))
                    continue;

                VString::size_type arrayStart = i->find("[", 0);
                if (arrayStart != VString::npos)
                {
                    // potential name (if butted up to array)
                    VString name = i->substr(0, arrayStart);
                    StrUtil::trim(name);
                    if (!name.empty())
                        paramName = name;

                    def.mUnitCount = 1;

                    // N-dimensional arrays
                    while (arrayStart != String::npos) 
                    {
                        VString::size_type arrayEnd = i->find("]", arrayStart);
                        VString arrayDimTerm = i->substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                        StrUtil::trim(arrayDimTerm);
                        //TODO
                        // the array term might be a simple number or it might be
                        // an expression (e.g. 24*3) or refer to a constant expression
                        // we'd have to evaluate the expression which could get nasty
                        Ni32 tempi;
                        StrConv::conv(arrayDimTerm, tempi);
                        def.mUnitCount *= tempi;
                        arrayStart = i->find("[", arrayEnd);
                    }
                }
                else
                {
                    paramName = *i;
                    def.mUnitCount = 1;
                }

                // Name should be after the type, so complete def and add
                // We do this now so that comma-separated params will do
                // this part once for each name mentioned
                if (def.mDataType == GDT_Unknow)
                {
                    String templine;
                    StrConv::conv(line, templine);
                    N_Only(Log).log(_T("Problem parsing the following GLSL Uniform: '") + templine + _T("' in file ") + filename, LL_Fatal);
                    // next uniform
                    break;
                }

                // Special handling for shared parameters
                if(sharedParams)
                {
                    const GpuParamUnitList & map = sharedParams->getDefList();
                    if(map.find(paramName) == map.end()) 
                    {
                        // This constant doesn't exist so we'll create a new one
                        sharedParams->add(paramName, def.mDataType);
                    }
                }
                else
                {
                    // Complete def and add
                    // increment physical buffer location
                    def.mIndex = 0; // not valid in GLSL
                    def.mMemIndex = defs.m32bSize;
                    defs.m32bSize += def.mUnitCount * def.mUnitSize;
                    defs.mDefineList.emplace(paramName, def);

                    defs.add(paramName, def);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void GLSLProgramManagerBase::extractUniformsFromGLSL(const VString& src, GpuParamDefine & defs, const String& filename)
    {
        // Parse the output string and collect all uniforms
        // NOTE this relies on the source already having been preprocessed
        // which is done in GLSLESProgram::loadFromSource
        VString line;
        VString::size_type currPos = src.find("uniform");
        while (currPos != String::npos)
        {
            // Now check for using the word 'uniform' in a larger string & ignore
            bool inLargerString = false;
            if (currPos != 0)
            {
                char prev = src.at(currPos - 1);
                if (prev != ' ' && prev != '\t' && prev != '\r' && prev != '\n' && prev != ';')
                    inLargerString = true;
            }
            if (!inLargerString && currPos + 7 < src.size())
            {
                char next = src.at(currPos + 7);
                if (next != ' ' && next != '\t' && next != '\r' && next != '\n')
                    inLargerString = true;
            }

            // skip 'uniform'
            currPos += 7;

            if (!inLargerString)
            {
                VString::size_type endPos;
                VString typeString;
                GpuParamValue * blockSharedParams = 0;

                // Check for a type. If there is one, then the semicolon is missing
                // otherwise treat as if it is a uniform block
                String::size_type lineEndPos = src.find_first_of("\n\r", currPos);
                line = src.substr(currPos, lineEndPos - currPos);
                VStringList parts;
                StrUtil::split(line, parts, " \t");

                // Skip over precision keywords
                if(StrUtil::match((parts.front()), "lowp") || StrUtil::match((parts.front()), "mediump") ||
                   StrUtil::match((parts.front()), "highp"))
                    typeString = parts[1];
                else
                    typeString = parts[0];

                StringToEnumMap::iterator typei = mTypeEnumMap.find(typeString);
                if (typei == mTypeEnumMap.end())
                {
                    // Gobble up the external name
                    VString externalName = parts.front();

                    // Now there should be an opening brace
                    VString::size_type openBracePos = src.find('{', currPos);
                    if (openBracePos != VString::npos)
                    {
                        currPos = openBracePos + 1;
                    }
                    else
                    {
                        N_Only(Log).log(_T("Missing opening brace in GLSL Uniform Block in file ") + filename);
                        break;
                    }

                    // First we need to find the internal name for the uniform block
                    VString::size_type endBracePos = src.find('}', currPos);

                    // Find terminating semicolon
                    currPos = endBracePos + 1;
                    endPos = src.find(';', currPos);
                    if (endPos == String::npos)
                    {
                        // problem, missing semicolon, abort
                        break;
                    }
                }
                else
                {
                    // find terminating semicolon
                    endPos = src.find(';', currPos);
                    if (endPos == String::npos)
                    {
                        // problem, missing semicolon, abort
                        break;
                    }

                    parseGLSLUniform(src.substr(currPos, endPos - currPos), defs, filename, blockSharedParams);
                }
                line = src.substr(currPos, endPos - currPos);
            } // not commented or a larger symbol

            // Find next one
            currPos = src.find("uniform", currPos);
        }
    }
    //------------------------------------------------------------------------
}
