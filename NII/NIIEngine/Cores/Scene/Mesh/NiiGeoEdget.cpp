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

#include "NiiPreProcess.h"
#include "NiiGeoEdget.h"
#include "NiiGpuBuffer.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiException.h"
#include "NiiSimdUtil.h"

namespace NII
{
    //---------------------------------------------------------------------
/*     void GeoEdget::log(Log * l)
    {
        EdgeGroupList::iterator i, iend;
        EdgeList::iterator ei, eiend;
        TriangleList::iterator ti, tiend;
        tiend = mTriangleList.end();
        l->log("Edge Data");
        l->log("---------");
        NCount num = 0;
        for (ti = mTriangleList.begin(); ti != tiend; ++ti, ++num)
        {
            Triangle & t = *ti;
            l->log("Triangle " + StrConv::conv(num) + " = {" +
                "indexSet=" + StrConv::conv(t.mIndexDataIndex) + ", " +
                "vertexSet=" + StrConv::conv(t.mVertexDataIndex) + ", " +
                "v0=" + StrConv::conv(t.mVertIndex[0]) + ", " +
                "v1=" + StrConv::conv(t.mVertIndex[1]) + ", " +
                "v2=" + StrConv::conv(t.mVertIndex[2]) + "}");
        }
        iend = mGeoList.end();
        for (i = mGeoList.begin(); i != iend; ++i)
        {
            num = 0;
            eiend = i->mEdgeList.end();
            l->log("Edge Group vertexSet=" + StrConv::conv(i->mVertexDataIndex));
            for (ei = i->mEdgeList.begin(); ei != eiend; ++ei, ++num)
            {
                Edge& e = *ei;
                l->log(
                    "Edge " + StrConv::conv(num) + " = {\n" +
                    "  tri0=" + StrConv::conv(e.mTriIndex[0]) + ", \n" +
                    "  tri1=" + StrConv::conv(e.mTriIndex[1]) + ", \n" +
                    "  v0=" + StrConv::conv(e.mVertIndex[0]) + ", \n" +
                    "  v1=" + StrConv::conv(e.mVertIndex[1]) + ", \n"
                    "  alone =" + StrConv::conv(e.mAlone) + " \n"
                    "}");
            }
        }
    } */
    //---------------------------------------------------------------------
    void GeoEdget::updateView(const Vector4f & lightPos)
    {
        N_assert(mNormalsList.size() == mViewPosList.size(), "error");

        if(!mNormalsList.empty())
        {
            SIMDUtil::getImplementation()->calculateLightFacing(lightPos, 
                &mNormalsList.front(), &mViewPosList.front(), mViewPosList.size());
        }
    }
    //---------------------------------------------------------------------
    void GeoEdget::getEdget(GeometryRaw &)
    {
    
    }
    //---------------------------------------------------------------------
    void GeoEdget::updateNormals(NCount vdidx, const VertexBuffer * pos)
    {
        N_assert (pos->getUnitSize() == sizeof(NIIf) * 3,
            "Position buffer should contain only positions!");

        // Triangle face normals should be 1:1 with triangles
        N_assert(mNormalsList.size() == mTriangleList.size(), "error");

        // Lock buffer for reading
        NIIf * data = (NIIf *)const_cast<VertexBuffer *>(pos)->lock(Buffer::MM_READ);

        // Calculate triangles which are using this vertex set
        const GeoEdget::EdgeGroup & eg = mGeoList[vdidx];
        if (eg.mCount != 0)
        {
            SIMDUtil::getImplementation()->calculateFaceNormals(data, 
                &mTriangleList[eg.mOffset], &mNormalsList[eg.mOffset], eg.mCount);
        }

        // unlock the buffer
        const_cast<VertexBuffer *>(pos)->unlock();
    }
    //---------------------------------------------------------------------------------------
}