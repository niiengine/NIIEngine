/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2017 Torus Knot Software Ltd

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

#ifndef _OgreMetalPixelFormatToShaderType_H_
#define _OgreMetalPixelFormatToShaderType_H_

#include "OgreMetalPrerequisites.h"
#include "OgrePixelFormatGpu.h"

namespace Ogre
{
    namespace PixelFormatDataTypes
    {
        enum PixelFormatDataTypes
        {
            Float,
            Half,
            Int,
            Uint,
            Short,
            Ushort,
            Char,
            Uchar,

            NumPixelFormatDataTypes
        };
    }

    class _OgreMetalExport MetalPixelFormatToShaderType : public PixelFormatToShaderType
    {
        static PixelFormatDataTypes::PixelFormatDataTypes getPixelFormatDataType(PixelFormat pf );

    public:
        virtual const char* getPixelFormatType( PixelFormat pf ) const;
        virtual const char* getDataType( PixelFormat pf, uint32 textureType,
                                         bool isMsaa, Buffer::OpType access ) const;
    };
}

#endif
