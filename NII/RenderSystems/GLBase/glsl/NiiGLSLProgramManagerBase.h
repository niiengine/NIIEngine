/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org/

  Copyright (c) 2000-2014 Torus Knot Software Ltd

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  -----------------------------------------------------------------------------
*/
#ifndef NII_GLSL_PROGRAMMANAGER_BASE_H
#define NII_GLSL_PROGRAMMANAGER_BASE_H

#include "NiiGLPreInclude.h"
#include "NiiGpuProgramParam.h"
#include "NiiString.h"

namespace NII 
{
    class GLSLProgramBase;
    class GLSLShaderBase;

    class GLSLProgramManagerBase
    {
    public:
        virtual ~GLSLProgramManagerBase();

        /** Populate a list of uniforms based on GLSL source and store
            them in GpuNamedConstants.  
            @param src Reference to the source code.
            @param constantDefs The defs to populate (will
            not be cleared before adding, clear it yourself before
            calling this if that's what you want).  
            @param filename The file name this came from, for logging errors.
        */
        void extractUniformsFromGLSL(const VString & src, GpuParamDefine & def, const String & filename);

        /// Destroy all programs which referencing this shader
        void destroyAllByShader(GLSLShaderBase * shader);
    protected:
        /**  Convert GL uniform size and type to OGRE constant types
             and associate uniform definitions together. */
        virtual void convertGLUniformtoOgreType(Nui32 gltype, GpuParamUnit & unit) = 0;

        /** Parse an individual uniform from a GLSL source file and
            store it in a GpuNamedConstant. */
        void parseGLSLUniform(VString line, GpuParamDefine & defs, const String & filename, GpuParamValue * sharedParams);
    protected:
        typedef map<Nui32, GLSLProgramBase *>::type ProgramMap;
        typedef ProgramMap::iterator ProgramIterator;
        typedef map<VString, Nui32>::type StringToEnumMap;

        StringToEnumMap mTypeEnumMap;
        /// container holding previously created program objects
        ProgramMap mPrograms;
    };

}
#endif