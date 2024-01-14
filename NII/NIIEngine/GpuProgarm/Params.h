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
#ifndef _OgreShaderParams_H_
#define _OgreShaderParams_H_

#include "OgreGpuProgramParams.h"
#include "OgreHeaderPrefix.h"

namespace Ogre
{
    /** \addtogroup Component
    *  @{
    */
    /** \addtogroup Material
    *  @{
    */

    /** The purpose of this class is to contain a set of both auto and
        manual parameters that may apply to multiple shaders; without
        having the shader to be created first (the drawback of GpuProgramParam).
    @par
        Useful when loading an RenderPattern material via JSON (e.g. a Compute Job)
        which may generate multiple shaders depending on the property settings.
    @par
        Parameters are kept unsorted in mDefines
    */
    class _OgreExport GpuParamValue : public PassAlloc
    {
    public:
        enum ElementType
        {
            ElementInt,
            ElementUInt,
            ElementFloat
        };

        struct AutoParam
        {
            ElementType extraParamType;
            GpuSyncParam acType;
            double extraParamValue;
        };

        struct ManualParam
        {
            ElementType elementType;
            /// Size in bytes from dataBytes
            uint8 dataSizeBytes;
            /// Enough data to cover up to a 4x4 matrix.
            /// Gets reinterpret_cast'ed based on elementType
            uint8 dataBytes[64];
        };

        struct _OgreExport Param
        {
            String name;
            bool mSyncValue;
            bool mValidValue;

            union
            {
                AutoParam ap;
                ManualParam mp;
            };

            template <typename T>
            void setManualValue(T value, uint32 numValues, ElementType elementType)
            {
                mSyncValue = false;
                mValidValue = false;
                mp.elementType = elementType;
                mp.dataSizeBytes = std::min( 4u * numValues, 64u ); //Don't overflow
                memcpy( mp.dataBytes, value, mp.dataSizeBytes );
            }

            void setManualValue(const Vector2 & value)
            {
                mSyncValue = false;
                mValidValue = false;
                mp.elementType = ElementFloat;
                mp.dataSizeBytes = sizeof(float) * 2u;
        #if !OGRE_DOUBLE_PRECISION
                memcpy( mp.dataBytes, value.ptr(), sizeof(float) * 2u );
        #else
                reinterpret_cast<float *>(mp.dataBytes)[0] = static_cast<float>( value[0] );
                reinterpret_cast<float *>(mp.dataBytes)[1] = static_cast<float>( value[1] );
                reinterpret_cast<float *>(mp.dataBytes)[2] = static_cast<float>( value[2] );
        #endif
            }

            void setManualValue(const Vector3 & value)
            {
                mSyncValue = false;
                mValidValue = false;
                mp.elementType = ElementFloat;
                mp.dataSizeBytes = sizeof(float) * 3u;
        #if !OGRE_DOUBLE_PRECISION
                memcpy( mp.dataBytes, value.ptr(), sizeof(float) * 3u );
        #else
                reinterpret_cast<float*>(mp.dataBytes)[0] = static_cast<float>( value[0] );
                reinterpret_cast<float*>(mp.dataBytes)[1] = static_cast<float>( value[1] );
                reinterpret_cast<float*>(mp.dataBytes)[2] = static_cast<float>( value[2] );
                //reinterpret_cast<float*>(mp.dataBytes)[3] = static_cast<float>( value[3] );
        #endif
            }
            void setManualValue( const Vector4 & value )
            {
                mSyncValue = false;
                mValidValue = false;
                mp.elementType = ElementFloat;
                mp.dataSizeBytes = sizeof(float) * 4u;
        #if !OGRE_DOUBLE_PRECISION
                memcpy( mp.dataBytes, value.ptr(), sizeof(float) * 4u );
        #else
                reinterpret_cast<float*>(mp.dataBytes)[0] = static_cast<float>( value[0] );
                reinterpret_cast<float*>(mp.dataBytes)[1] = static_cast<float>( value[1] );
                reinterpret_cast<float*>(mp.dataBytes)[2] = static_cast<float>( value[2] );
                reinterpret_cast<float*>(mp.dataBytes)[3] = static_cast<float>( value[3] );
        #endif
            }

            void setManualValue( const Matrix3 & value )
            {
                mSyncValue = false;
                mValidValue = false;
                mp.elementType = ElementFloat;
                mp.dataSizeBytes = sizeof(float) * 9u;
                const Real * RESTRICT_ALIAS value = reinterpret_cast<const Real * RESTRICT_ALIAS>( &value );
                float * RESTRICT_ALIAS dstValue = reinterpret_cast<float*RESTRICT_ALIAS>( mp.dataBytes );

                for(int i = 0; i < 9; ++i)
                    dstValue[i] = static_cast<float>(value[i]);
            }

            void setManualValue(const Matrix4 & value)
            {
                mSyncValue = false;
                mValidValue = false;
                mp.elementType = ElementFloat;
                mp.dataSizeBytes = sizeof(float) * 16u;

                const Real * RESTRICT_ALIAS value = reinterpret_cast<const Real * RESTRICT_ALIAS>(&value);
        #if !OGRE_DOUBLE_PRECISION
                memcpy(mp.dataBytes, value, sizeof(float) * 16u);
        #else
                for(int i = 0; i < 16; ++i)
                    reinterpret_cast<float *>(mp.dataBytes)[i] = static_cast<float>(value[i]);
        #endif
            }

            /** Shortcut for setting the given value without dealing with mp manually.
            @remarks
                You can't write more than 64 bytes of data per parameter.
                (i.e. 16 uint32, 16 int32, or 16 floats)
                Remember to call @see GpuParamValue::setStateMark otherwise
                changes won't take effect.
            */
            void setManualValue(float value)                        { setManualValue( &value, 1u, ElementFloat ); }
            void setManualValue(const float * value, uint32 cnt)    { setManualValue( value, cnt, ElementFloat ); }
            void setManualValue(int32 value)                        { setManualValue( &value, 1u, ElementInt ); }
            void setManualValue(const int32 * value, uint32 cnt)    { setManualValue( value, cnt, ElementInt ); }
            void setManualValue(uint32 value)                       { setManualValue( &value, 1u, ElementUInt ); }
            void setManualValue(const uint32 * value, uint32 cnt)   { setManualValue( value, cnt, ElementUInt ); }

            /** Returns the value. Assumes this is a manual value.
                Assumes the param holds enough bytes to fill the type
                of value you want to retrieve.
                Examples:
                    uint32 myVal;
                    param->getManualValue( myVal );
                    Vector4 myVector4;
                    param->getManualValue( myVector4 );
            */
            template <typename T> void getManualValue(T & value) const
            {
                assert(!mSyncValue && sizeof(T) <= mp.dataSizeBytes);
                memcpy(&value, mp.dataBytes, sizeof(T));
            }
            /// See other overload.
            /// Examples:
            ///     uint32 myVal = param->getManualValue();
            ///     Vector4 myVector4 = param->getManualValue();
            template <typename T> T getManualValue(void) const
            {
                T re;
                getManualValue(re);
                return re;
            }
        };
        
        typedef vector<Param>::type ParamVec;
    public:
        GpuParamValue();

        void update(GpuProgramParam * params, bool bForce);
    protected:
        /// Don't log exceptions about missing parameters
        bool mSilenceMissingParameterWarnings;
        ParamVec mDefines;
    };
    /** @} */
    /** @} */
}
#include "OgreHeaderSuffix.h"
#endif