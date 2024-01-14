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
#include "NiiRay.h"
#include "NiiVector3.h"
#include "NiiSphere.h"
#include "NiiAABox.h"

namespace NII
{
    //---------------------------------------------------------------------
    Ray::Ray() :
        mOrigin(Vector3f::ZERO),
        mDirection(Vector3f::Z)
    {
    }
    //---------------------------------------------------------------------
    Ray::Ray(const Vector3f & origin, const Vector3f & direction) :
        mOrigin(origin),
        mDirection(direction)
    {
    }
    //-----------------------------------------------------------------
    bool Ray::intersects(const Plane & p, NIIf & distance) const
    {
        NIIf denom = p.mNormal.dotProduct(mDirection);
        if(Math::Abs(denom) < N_TYPE_EPSILON(NIIf))
        {
            // Parallel
            distance = 0;
            return false;
        }
        else
        {
            NIIf nom = p.mNormal.dotProduct(mOrigin) + p.mD;
            distance = -(nom / denom);
            return distance >= 0;
        }
    }
    //-----------------------------------------------------------------
    bool Ray::intersects(const AABox & box, NIIf & distance) const
    {
        NIIf lowt = 0.0f;
        NIIf t;
        bool hit = false;
        Vector3f hitpoint;
        const Vector3f & min = box.getMin();
        const Vector3f & max = box.getMax();
        const Vector3f & rayorig = mOrigin;
        const Vector3f & raydir = mDirection;

        // Check origin inside first
        if(rayorig > min && rayorig < max)
        {
            distance = 0;
            return true;
        }

        // Check each face in turn, only check closest 3
        // Min x
        if(rayorig.x <= min.x && raydir.x > 0)
        {
            t = (min.x - rayorig.x) / raydir.x;
            if(t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if(hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                        (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max x
        if(rayorig.x >= max.x && raydir.x < 0)
        {
            t = (max.x - rayorig.x) / raydir.x;
            if (t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if(hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                        (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Min y
        if(rayorig.y <= min.y && raydir.y > 0)
        {
            t = (min.y - rayorig.y) / raydir.y;
            if(t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if(hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                        (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max y
        if(rayorig.y >= max.y && raydir.y < 0)
        {
            t = (max.y - rayorig.y) / raydir.y;
            if(t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if(hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                        (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Min z
        if(rayorig.z <= min.z && raydir.z > 0)
        {
            t = (min.z - rayorig.z) / raydir.z;
            if(t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if(hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.y >= min.y && hitpoint.y <= max.y &&
                        (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max z
        if(rayorig.z >= max.z && raydir.z < 0)
        {
            t = (max.z - rayorig.z) / raydir.z;
            if(t >= 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if(hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.y >= min.y && hitpoint.y <= max.y &&
                        (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        distance = lowt;
        return hit;
    }
    //-----------------------------------------------------------------
    bool Ray::intersects(const Sphere & sphere, NIIf & distance, bool insideD) const
    {
        const Vector3f & raydir = mDirection;
        // Adjust ray origin relative to sphere center
        const Vector3f & rayorig = mOrigin - sphere.getCenter();
        NIIf radius = sphere.getRadius();

        // Check origin inside first
        if(rayorig.lengthSquared() <= (radius * radius) && (!insideD))
        {
            distance = 0;
            return true;
        }

        // Mmm, quadratics
        // Build coeffs which can be used with std quadratic solver
        // ie t = (-b +/- sqrt(b*b + 4ac)) / 2a
        NIIf a = raydir.dotProduct(raydir);
        NIIf b = 2 * rayorig.dotProduct(raydir);
        NIIf c = rayorig.dotProduct(rayorig) - (radius * radius);

        // Calc determinant
        NIIf d = (b * b) - (4 * a * c);
        if(d < 0)
        {
            // No intersection
            distance = 0;
            return false;
        }
        else
        {
            // BTW, if d=0 there is one intersection, if d > 0 there are 2
            // But we only want the closest one, so that's ok, just use the
            // '-' version of the solver
            distance = (-b - Math::sqrt(d)) / (2 * a);
            if(distance < 0)
                distance = (-b + Math::sqrt(d)) / (2 * a);

            return true;
        }
    }
    //-----------------------------------------------------------------
    bool Ray::intersects(const PlaneList & planes, NIIf & distance, Plane::PositionType outside) const
    {
        PlaneList::const_iterator planeit, planeitend = planes.end();
        bool allInside = true;
        bool ret = false;
        std::pair<bool, NIIf> end;
        distance = 0.0f;
        end.first = false;
        end.second = 0;

        for(planeit = planes.begin(); planeit != planeitend; ++planeit)
        {
            const Plane & plane = *planeit;
            // is origin outside ?
            if(plane.getSide(mOrigin) == outside)
            {
                allInside = false;
                // Test single plane
                std::pair<bool, NIIf> planeRes;
                planeRes.first = intersects(plane, planeRes.second);
                if(planeRes.first)
                {
                    // Ok, we intersected
                    ret = true;
                    // Use the most distant result since convex volume
                    distance = std::max(distance, planeRes.second);
                }
                else
                {
                    ret = false;
                    distance = 0.0f;
                    return ret;
                }
            }
            else
            {
                std::pair<bool, NIIf> planeRes;
                planeRes.first = intersects(plane, planeRes.second);
                if(planeRes.first)
                {
                    if(!end.first)
                    {
                        end.first = true;
                        end.second = planeRes.second;
                    }
                    else
                    {
                        end.second = std::min(planeRes.second, end.second);
                    }
                }
            }
        }
        if(allInside)
        {
            // Intersecting at 0 distance since inside the volume!
            ret = true;
            distance = 0.0f;
            return ret;
        }
        if(end.first)
        {
            if(end.second < distance)
            {
                ret = false;
                return ret;
            }
        }
        return ret;
    }
    //-----------------------------------------------------------------
    bool Ray::intersects(const AABox & box, NIIf & d1, NIIf & d2) const
    {
        const Vector3f & min = box.getMin();
        const Vector3f & max = box.getMax();
        const Vector3f & rayorig = mOrigin;
        const Vector3f & raydir = mDirection;

        Vector3f absDir;
        absDir.x = Math::Abs(raydir.x);
        absDir.y = Math::Abs(raydir.y);
        absDir.z = Math::Abs(raydir.z);

        // Sort the axis, ensure check minimise floating error axis first
        NIIi imax = 0, imid = 1, imin = 2;
        if (absDir[0] < absDir[2])
        {
            imax = 2;
            imin = 0;
        }
        if (absDir[1] < absDir[imin])
        {
            imid = imin;
            imin = 1;
        }
        else if (absDir[1] > absDir[imax])
        {
            imid = imax;
            imax = 1;
        }

        NIIf start = 0, end = Math::POS_INFINITY;

#define _CALC_AXIS(i)                                       \
    do {                                                    \
        NIIf denom = 1 / raydir[i];                         \
        NIIf newstart = (min[i] - rayorig[i]) * denom;      \
        NIIf newend = (max[i] - rayorig[i]) * denom;        \
        if (newstart > newend) std::swap(newstart, newend); \
        if (newstart > end || newend < start) return false; \
        if (newstart > start) start = newstart;             \
        if (newend < end) end = newend;                     \
    } while(0)

        // Check each axis in turn

        _CALC_AXIS(imax);

        if (absDir[imid] < N_TYPE_EPSILON(NIIf))
        {
            // Parallel with middle and minimise axis, check bounds only
            if (rayorig[imid] < min[imid] || rayorig[imid] > max[imid] ||
                rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
                return false;
        }
        else
        {
            _CALC_AXIS(imid);

            if (absDir[imin] < N_TYPE_EPSILON(NIIf))
            {
                // Parallel with minimise axis, check bounds only
                if (rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
                    return false;
            }
            else
            {
                _CALC_AXIS(imin);
            }
        }
#undef _CALC_AXIS

        d1 = start;
        d2 = end;

        return true;
    }
    //--------------------------------------------------------------------
    bool Ray::intersects(const Vector3f & a, const Vector3f & b, const Vector3f & c,
        const Vector3f & normal, NIIf & distance, bool & positive) const
    {
        //
        // Calculate intersection with plane.
        //
        NIIf t;
        {
            NIIf denom = normal.dotProduct(mDirection);

            // Check intersect side
            if(denom > + N_TYPE_EPSILON(NIIf))
            {
                positive = false;
            }
            else if(denom < - N_TYPE_EPSILON(NIIf))
            {
                positive = true;
            }
            else
            {
                // Parallel or triangle area is close to zero when
                // the plane normal not normalised.
                distance = 0;
                return false;
            }

            t = normal.dotProduct(a - mOrigin) / denom;

            if(t < 0)
            {
                // Intersection is behind origin
                distance = 0;
                return false;
            }
        }

        //
        // Calculate the largest area projection plane in X, Y or Z.
        //
        NCount i0, i1;
        {
            NIIf n0 = Math::Abs(normal.x);
            NIIf n1 = Math::Abs(normal.y);
            NIIf n2 = Math::Abs(normal.z);

            i0 = 1; i1 = 2;
            if(n1 > n2)
            {
                if(n1 > n0)
                    i0 = 0;
            }
            else
            {
                if(n2 > n0)
                    i1 = 0;
            }
        }

        //
        // Check the intersection point is inside the triangle.
        //
        {
            NIIf u1 = b[i0] - a[i0];
            NIIf v1 = b[i1] - a[i1];
            NIIf u2 = c[i0] - a[i0];
            NIIf v2 = c[i1] - a[i1];
            NIIf u0 = t * mDirection[i0] + mOrigin[i0] - a[i0];
            NIIf v0 = t * mDirection[i1] + mOrigin[i1] - a[i1];

            NIIf alpha = u0 * v2 - u2 * v0;
            NIIf beta  = u1 * v0 - u0 * v1;
            NIIf area  = u1 * v2 - u2 * v1;

            // epsilon to avoid NIIf precision error
            const NIIf EPSILON = 1e-6f;

            NIIf tolerance = - EPSILON * area;

            if(area > 0)
            {
                if(alpha < tolerance || beta < tolerance || alpha+beta > area-tolerance)
                {
                    distance = 0;
                    return false;
                }
            }
            else
            {
                if(alpha > tolerance || beta > tolerance || alpha+beta < area-tolerance)
                {
                    distance = 0;
                    return false;
                }
            }
        }
        distance = t;
        return true;
    }
    //----------------------------------------------------------------------
}