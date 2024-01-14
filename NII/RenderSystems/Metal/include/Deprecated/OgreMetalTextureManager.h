/*
  -----------------------------------------------------------------------------
  This source file is part of OGRE
  (Object-oriented Graphics Rendering Engine)
  For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2016 Torus Knot Software Ltd

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
#ifndef _OgreMetalTextureManager_H_
#define _OgreMetalTextureManager_H_

#include "OgreMetalPrerequisites.h"
#include "OgreTextureManager.h"

namespace Ogre
{
    class MetalTextureManager : public TextureManager
    {
    protected:
        MetalDevice *mDevice;

        /// @copydoc ResourceManager::createImpl
        virtual Resource* createImpl(const String& name, ResourceHandle handle,
            const String& group, bool isManual, ManualResourceLoader* loader,
            const PropertyData* createParams);

    public:
        MetalTextureManager( MetalDevice *device );
        virtual ~MetalTextureManager();

        /// @copydoc TextureManager::getNativeFormat
        virtual PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage);

        /// @copydoc TextureManager::isHardwareFilteringSupported
        virtual bool isHardwareFilteringSupported( TextureType ttype, PixelFormat format,
                                                   int usage,
                                                   bool preciseFormatOnly = false );
    };
}

#endif
