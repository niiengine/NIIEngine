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
#include "NiiPolygon.h"

#include "NiiException.h"
#include "NiiVector3.h"

namespace NII
{
    //-----------------------------------------------------------------------
    Polygon::Polygon() :
        mNormal(Vector3f::ZERO),
        mIsNormalSet(false)
    {
        // reserve space for 6 vertices to reduce allocation cost
        mVertexList.reserve(6);
    }
    //-----------------------------------------------------------------------
    Polygon::~Polygon() {}
    //-----------------------------------------------------------------------
    Polygon::Polygon(const Polygon & cpy)
    {
        mVertexList = cpy.mVertexList;
        mNormal = cpy.mNormal;
        mIsNormalSet = cpy.mIsNormalSet;
    }
    //-----------------------------------------------------------------------
    void Polygon::insertVertex(const Vector3f & vertex, NCount idx)
    {
        // TODO: optional: check planarity
        N_assert(idx <= mVertexList.size(), "Insert position out of range");

        VertexList::iterator it = mVertexList.begin();

        std::advance(it, idx);
        mVertexList.insert(it, vertex);
    }
    //-----------------------------------------------------------------------
    void Polygon::insertVertex(const Vector3f & vertex)
    {
        mVertexList.push_back(vertex);
    }
    //-----------------------------------------------------------------------
    const Vector3f & Polygon::getVertex(NCount idx) const
    {
        N_assert(idx < mVertexList.size(), "Search position out of range");

        return mVertexList[idx];
    }
    //-----------------------------------------------------------------------
    void Polygon::setVertex(const Vector3f & vertex, NCount idx)
    {
        // TODO: optional: check planarity
        N_assert(idx < mVertexList.size(), "Search position out of range");

        mVertexList[idx] = vertex;
    }
    //-----------------------------------------------------------------------
    void Polygon::opt(void)
    {
        for (NCount i = 0; i < mVertexList.size(); ++i)
        {
            const Vector3f & a = getVertex(i);
            const Vector3f & b = getVertex((i + 1)%mVertexList.size());

            if (a.equal(b))
            {
                deleteVertex(i);
                --i;
            }
        }
    }
    //-----------------------------------------------------------------------
    NCount Polygon::getVertexCount() const
    {
        return mVertexList.size();
    }
    //-----------------------------------------------------------------------
    const Vector3f & Polygon::getNormal() const
    {
        N_assert(mVertexList.size() >= 3, "Insufficient vertex count!");

        updateNormal();

        return mNormal;
    }
    //-----------------------------------------------------------------------
    void Polygon::updateNormal() const
    {
        N_assert(mVertexList.size() >= 3, "Insufficient vertex count!");

        if (mIsNormalSet)
            return;

        // vertex order is ccw
        const Vector3f & a = getVertex( 0 );
        const Vector3f & b = getVertex( 1 );
        const Vector3f & c = getVertex( 2 );

        // used method: Newell
        mNormal.x = 0.5f * ( (a.y - b.y) * (a.z + b.z) +
                               (b.y - c.y) * (b.z + c.z) + 
                               (c.y - a.y) * (c.z + a.z));

        mNormal.y = 0.5f * ( (a.z - b.z) * (a.x + b.x) +
                               (b.z - c.z) * (b.x + c.x) + 
                               (c.z - a.z) * (c.x + a.x));

        mNormal.z = 0.5f * ( (a.x - b.x) * (a.y + b.y) +
                               (b.x - c.x) * (b.y + c.y) + 
                               (c.x - a.x) * (c.y + a.y));

        mNormal.normalise();

        mIsNormalSet = true;
    }
    //-----------------------------------------------------------------------
    void Polygon::deleteVertex(NCount vertex)
    {
        N_assert(vertex < mVertexList.size(), "Search position out of range");

        VertexList::iterator it = mVertexList.begin();
        std::advance(it, vertex);

        mVertexList.erase(it);
    }
    //-----------------------------------------------------------------------
    void Polygon::getEdge(Polygon::EdgeMap * list) const
    {
        N_assert(list != NULL, "EdgeMap ptr is NULL");

        NCount vertexCount = mVertexList.size();

        for(NCount i = 0; i < vertexCount; ++i)
        {
            list->push_back(Edge(getVertex(i), getVertex((i + 1) % vertexCount)));
        }
    }
    //-----------------------------------------------------------------------
    void Polygon::getClockwiseEdge(EdgeMap * list) const
    {
        N_assert(list != NULL, "EdgeMap ptr is NULL");

        NCount vertexCount = mVertexList.size();

        for(NCount i = vertexCount; i > 0; --i)
        {
            list->push_back(Edge(getVertex(i % vertexCount), getVertex(i - 1)));
        }
    }
    //-----------------------------------------------------------------------
    void Polygon::cleanAll()
    {
        mVertexList.clear();
        mIsNormalSet = false;
    }
    //-----------------------------------------------------------------------
    bool Polygon::operator == (const Polygon & o) const
    {
        if (mVertexList.size() != o.getVertexCount())
            return false;

        // Compare vertices. They may differ in its starting position.
        // find start
        NCount start = 0;
        bool foundStart = false;
        for (NCount i = 0; i < mVertexList.size(); ++i)
        {    
            if (getVertex(0).equal(o.getVertex(i)))
            {
                start = i;
                foundStart = true;
                break;
            }
        }

        if (!foundStart)
            return false;

        for (NCount i = 0; i < mVertexList.size(); ++i)
        {
            const Vector3f & vA = getVertex(i);
            const Vector3f & vB = o.getVertex((i + start) % mVertexList.size());

            if (!vA.equal(vB))
                return false;
        }

        return true;
    }
    //-----------------------------------------------------------------------
    Nostream & operator<< (Nostream & strm, const Polygon & poly)
    {
        strm << "NUM VERTICES: " << poly.getVertexCount() << std::endl;

        for(NCount j = 0; j < poly.getVertexCount(); ++j)
        {
            strm << "VERTEX " << j << ": " << poly.getVertex(j) << std::endl;
        }

        return strm;
    }
    //-----------------------------------------------------------------------
    bool Polygon::isPointInside(const Vector3f & point) const
    {
        // sum the angles 
        NIIf anglesum = 0;
        NCount n = mVertexList.size();
        for (NCount i = 0; i < n; i++) 
        {
            const Vector3f & p1 = getVertex(i);
            const Vector3f & p2 = getVertex((i + 1) % n);

            Vector3f v1 = p1 - point;
            Vector3f v2 = p2 - point;

            NIIf len1 = v1.length();
            NIIf len2 = v2.length();

            if (Math::isEqual(len1 * len2, 0.0f, 1e-4f))
            {
                // We are on a vertex so consider this inside
                return true; 
            }
            else
            {
                NIIf costheta = v1.dotProduct(v2) / (len1 * len2);
                anglesum += acos(costheta);
            }
        }
        // result should be 2*PI if point is inside poly
        return Math::isEqual(anglesum, Math::PI2X, 1e-4f);
    }
    //---------------------------------------------------------
}
