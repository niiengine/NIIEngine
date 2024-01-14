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

#ifndef _NII_VERTEX_H_
#define _NII_VERTEX_H_

#include "NiiPreInclude.h"
#include "NiiVector2.h"
#include "NiiVector3.h"
#include "NiiColour.h"

namespace NII
{
    /** 2D¶¥µã
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Vertex : public GeometryAlloc
    {
    public:
        Vertex(){}
        ~Vertex(){}
    public:
        Vector3f mPos;
        Vector2f mCoord;
        Colour mColour;
    };

    /** ÏñËØ¶¥µã
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelVertex : public GeometryAlloc
    {
    public:
        PixelVertex(){}
        ~PixelVertex(){}
    public:
        Vector3f mPos;
        Colour mColour;
        NIIf mCoord;
    };

    /** 3D¶¥µã
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ComplexVertex : public GeometryAlloc
    {
    public:
        ComplexVertex(){}
        ~ComplexVertex(){}
    public:
        Vector3f mPos;
        Vector3f mNorl;
        Colour mColour;
        Vector3f mTangent;
        Vector4f mCoord[8];
        Nui8 mCoordDim[8];
    };

    /** 3DÏñËØ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Pixel : public PixelVertex
    {
    public:
        Pixel(){}
        ~Pixel(){}
    public:
        NIIf mDim;
        Quaternion mOri;

    };
    typedef vector<Pixel>::type PixelList;
}

#endif