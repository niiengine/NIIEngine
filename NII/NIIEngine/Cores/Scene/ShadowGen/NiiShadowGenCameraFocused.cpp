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
#include "NiiCommon.h"
#include "NiiShadowGenCameraFocused.h"
#include "NiiShadowMappingPattern.h"
#include "NiiPlaneSpaceObj.h"
#include "NiiEngine.h"
#include "NiiSpaceManager.h"
#include "NiiCamera.h"
#include "NiiLight.h"
#include "NiiPlane.h"
#include "NiiLogManager.h"
#include "NiiConvexBody.h"
#include "NiiNumerics.h"
#include "NiiViewport.h"

#if N_COMPILER == N_CPP_MSVC
    // we do a lot of PreciseReal -> NIIf in here, casting is messy
    // disable: "conversion from 'NIId' to 'NIIf', possible loss of data
    #pragma warning (disable : 4244)
    #pragma warning (disable : 4305)
#endif

namespace NII
{
    //---------------------------------------------------------------------
    // PlaneOptimalShadowSetup
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    //_______________________________________________________
    //|                                                        |
    //|    getRayForwardIntersect                                |
    //|    -----------------------------                        |
    //|    get the intersections of frustum rays with a plane    |
    //| of interest.  The plane is assumed to have constant    |
    //| z.  If this is not the case, rays                    |
    //| should be rotated beforehand to work in a            |
    //| coordinate system in which this is true.            |
    //|_____________________________________________________|
    //
    vector<Vector4f >::type PlaneOptimalShadowSetup::getRayForwardIntersect(const Vector3f & anchor,
        const Vector3f * dir, NIIf planeOffset) const
    {
        vector<Vector4f >::type res;

        if(!dir)
            return res;

        NIIi infpt[4] = {0, 0, 0, 0}; // 0=finite, 1=infinite, 2=straddles infinity
        Vector3f vec[4];

        // find how much the anchor point must be displaced in the plane's
        // constant variable
        NIIf delta = planeOffset - anchor.z;

        // now set the intersection point and note whether it is a
        // point at infinity or straddles infinity
        Nui32 i;
        for(i = 0; i < 4; ++i)
        {
            NIIf test = dir[i].z * delta;
            if(test == 0.0)
            {
                vec[i] = dir[i];
                infpt[i] = 1;
            }
            else
            {
                NIIf lambda = delta / dir[i].z;
                vec[i] = anchor + (lambda * dir[i]);
                if(test < 0.0)
                    infpt[i] = 2;
            }
        }

        for(i = 0; i < 4; ++i)
        {
            // store the finite intersection points
            if (infpt[i] == 0)
                res.push_back(Vector4f(vec[i].x, vec[i].y, vec[i].z, 1.0));
            else
            {
                // handle the infinite points of intersection;
                // cases split up into the possible frustum planes
                // pieces which may contain a finite intersection point
                NIIi nextind = (i+1) % 4;
                NIIi prevind = (i+3) % 4;
                if ((infpt[prevind] == 0) || (infpt[nextind] == 0))
                {
                    if (infpt[i] == 1)
                        res.push_back(Vector4f(vec[i].x, vec[i].y, vec[i].z, 0.0));
                    else
                    {
                        // handle the intersection points that straddle infinity (back-project)
                        if(infpt[prevind] == 0)
                        {
                            Vector3f temp = vec[prevind] - vec[i];
                            res.push_back(Vector4f(temp.x, temp.y, temp.z, 0.0));
                        }
                        if(infpt[nextind] == 0)
                        {
                            Vector3f temp = vec[nextind] - vec[i];
                            res.push_back(Vector4f(temp.x, temp.y, temp.z, 0.0));
                        }
                    }
                } // end if we need to add an intersection point to the list
            } // end if infinite point needs to be considered
        } // end loop over frustun corners
        // we end up with either 0, 3, 4, or 5 intersection points
        return res;
    }
    //_______________________________________________________
    //|                                                     |
    //|    forwardIntersect                                 |
    //|    -----------------------------                    |
    //|    Forward intersect the camera's frustum rays with |
    //| a specified plane of interest.                      |
    //| Note that if the frustum rays shoot out and would   |
    //| back project onto the plane, this means the forward |
    //| intersection of the frustum would occur at the      |
    //| line at infinity.                                   |
    //|_____________________________________________________|
    //
    void PlaneOptimalShadowSetup::forwardIntersect(const Camera * src, const Plane & worldPlane,
        vector<Vector4f >::type * intersect3d) const
    {
        if(!intersect3d)
            return;

        const Vector3f * cpi = src->getClipPlaneIntersect();

        // need some sort of rotation that will bring the plane normal to the z axis
        Plane pval = worldPlane;
        if(pval.mNormal.z < 0.0)
        {
            pval.mNormal *= -1.0;
            pval.mD *= -1.0;
        }
        Quaternion invPlaneRot = Quaternion::getRotation(pval.mNormal, Vector3f::Z);

        // get rotated light
        Vector3f lPos = invPlaneRot * src->getSpacePos();
        Vector3f vec[4];
        vec[0] = invPlaneRot * cpi[0] - lPos;
        vec[1] = invPlaneRot * cpi[1] - lPos;
        vec[2] = invPlaneRot * cpi[2] - lPos;
        vec[3] = invPlaneRot * cpi[3] - lPos;

        // compute intersection points on plane
        vector<Vector4f >::type iPnt = getRayForwardIntersect(lPos, vec, -pval.mD);

        // return wanted data
        if(intersect3d)
        {
            Quaternion planeRot = invPlaneRot.inverse();
            (*intersect3d).clear();
            for(NCount i = 0; i < iPnt.size(); ++i)
            {
                Vector3f intersection = planeRot * Vector3f(iPnt[i].x, iPnt[i].y, iPnt[i].z);
                (*intersect3d).push_back(Vector4f(intersection.x, intersection.y, intersection.z, iPnt[i].w));
            }
        }
    }
    // --------------------------------------------------------------------
    Matrix4f PlaneOptimalShadowSetup::computeConstrainedProjection(
        const Vector4f & pinhole, const vector<Vector4f >::type & fpoint,
            const vector<Vector2f >::type & constraint) const
    {
        // NOTE: will assume the z coordinates should be decided such that
        // the first 3 points (in fpoint) will have post projective
        // z coordinates of about +1 and the 4th (in fpoint) will have a
        // post projective z coordinate of about -1.

        // TODO: could use SVD to avoid arbitrarily choosing one
        // matrix element to be 1.0 (and thereby fix the scale).

        Matrix4f ret;
        NIIi i;
        bool incrPrecision = false; // use to control numerical solving

        if(fpoint.size() < 4 || constraint.size() < 4)
        {
            return Matrix4f::IDENTITY;
        }

        // allocate memory
        PreciseReal **mat = NULL;
        PreciseReal **backmat = NULL;
        {
            mat = N_alloc_t(PreciseReal*, 11);
            if(incrPrecision)
                backmat = N_alloc_t(PreciseReal*, 11);
            for(i=0; i<11; i++)
            {
                mat[i] = N_alloc_t(PreciseReal, 11);
                if(incrPrecision)
                    backmat[i] = N_alloc_t(PreciseReal, 11);
            }
        }

        // set up linear system to solve for all rows of projective matrix
        // except for the 3rd which corresponds to mapping of z values

        // we choose a nonzero element of the last row to set to the arbitrary
        // constant 1.0.
        NIIi nzind = 3;
        PreciseReal col[11];
        PreciseReal backcol[11];

        // fill in light position constraints
        mat[0][0] = pinhole.x;
        mat[0][1] = pinhole.y;
        mat[0][2] = pinhole.z;
        mat[0][3] = pinhole.w;
        for(i=4; i<11; i++)
            mat[0][i] = 0.0;
        col[0] = 0.0;

        for(i=0; i<11; i++)
            mat[1][i] = 0.0;
        mat[1][4] = pinhole.x;
        mat[1][5] = pinhole.y;
        mat[1][6] = pinhole.z;
        mat[1][7] = pinhole.w;
        col[1] = 0.0;

        PreciseReal larr[4];
        larr[0] = pinhole.x;
        larr[1] = pinhole.y;
        larr[2] = pinhole.z;
        larr[3] = pinhole.w;
        for(i=0; i<8; i++)
            mat[2][i] = 0.0;
        NIIi ind = 8;
        for(i=0; i<4; i++)
        {
            if(nzind == i)
                continue;
            mat[2][ind++] = larr[i];
        }
        col[2] = -larr[nzind];

        // fill in all the other constraints
        NIIi row=3;
        for(i=0; i<4; i++)
        {
            NIIi j;
            larr[0] = fpoint[i].x;
            larr[1] = fpoint[i].y;
            larr[2] = fpoint[i].z;
            larr[3] = fpoint[i].w;

            // lexel s coordinate constraint
            for(j=0; j<4; j++)
                mat[row][j] = larr[j];
            for(j=4; j<8; j++)
                mat[row][j] = 0.0;
            ind=8;
            for(j=0; j<4; j++)
            {
                if(nzind==j)
                    continue;
                mat[row][ind++] = larr[j] * (-constraint[i].x);
            }
            col[row] = larr[nzind] * constraint[i].x;
            ++row;

            // lexel t coordinate constraint
            for(j=0; j<4; j++)
                mat[row][j] = 0.0;
            for(j=4; j<8; j++)
                mat[row][j] = larr[j-4];

            ind=8;
            for(j=0; j<4; j++)
            {
                if(nzind==j)
                    continue;
                mat[row][ind++] = larr[j] * (-constraint[i].y);
            }
            col[row] = larr[nzind] * constraint[i].y;
            ++row;
        }

        // copy the matrix and vector for later computation
        if(incrPrecision)
        {
            for (i=0; i<11; i++)
            {
                for(NIIi j=0; j<11; j++)
                    backmat[i][j] = mat[i][j];
                backcol[i] = col[i];
            }
        }

        // solve for the matrix elements
        if(!NumericSolver::solveNxNLinearSysDestr(11, mat, col))
        {
            // error solving for projective matrix (rows 1,2,4)
        }

        // get a little more precision
        if(incrPrecision)
        {
            for (NIIi k=0; k<3; k++)
            {
                PreciseReal nvec[11];
                for(i=0; i<11; i++)
                {
                    NIIi j;
                    nvec[i] = backmat[i][0] * col[0];
                    mat[i][0] = backmat[i][0];
                    for(j=1; j<11; j++)
                    {
                        nvec[i] += backmat[i][j] * col[j];
                        mat[i][j] = backmat[i][j];
                    }
                    nvec[i] -= backcol[i];
                }
                if(!NumericSolver::solveNxNLinearSysDestr(11, mat, nvec))
                {
                    // error solving for increased precision rows 1,2,4
                }
                for(i=0; i<11; i++)
                    col[i] -= nvec[i];
            }
        }

        PreciseReal row4[4];
        ind = 8;
        for(i=0; i<4; i++)
        {
            if (i == nzind)
                row4[i] = 1.0;
            else
                row4[i] = col[ind++];
        }


        // now solve for the 3rd row which affects depth precision
        PreciseReal zrow[4];

        // we want the affine skew such that isoplanes of constant depth are parallel to
        // the world plane of interest
        // NOTE: recall we perturbed the last fpoint off the plane, so we'll again modify
        // this one since we want 3 points on the plane = far plane, and 1 on the near plane
        NIIi nearind = 3;
        for(i=0; i<3; i++)
        {
            mat[i][0] = fpoint[i].x;
            mat[i][1] = fpoint[i].y;
            mat[i][2] = fpoint[i].z;
            mat[i][3] = 1.0;
            zrow[i] = (row4[0] * fpoint[i].x +
                row4[1] * fpoint[i].y +
                row4[2] * fpoint[i].z +
                row4[3]) * 0.99 ;
        }
        mat[3][0] = fpoint[nearind].x;
        mat[3][1] = fpoint[nearind].y;
        mat[3][2] = fpoint[nearind].z;
        mat[3][3] = 1.0;
        zrow[3] =     -row4[0] * fpoint[nearind].x -
            row4[1] * fpoint[nearind].y -
            row4[2] * fpoint[nearind].z -
            row4[3] ;

        // solve for the z row of the matrix
        if(!NumericSolver::solveNxNLinearSysDestr(4, mat, zrow))
        {
            // error solving for projective matrix (row 3)
        }

        // set projective texture matrix
        ret = Matrix4f(  col[0],  col[1],  col[2],  col[3],
            col[4],  col[5],  col[6],  col[7],
            zrow[0], zrow[1], zrow[2], zrow[3],
            row4[0], row4[1], row4[2], row4[3] );


        // check for clip
        Vector4f testCoord = ret * fpoint[0];
        if(testCoord.w < 0.0)
            ret = ret *  (-1.0);

        // free memory
        for (i=0; i<11; i++)
        {
            if (mat[i])
                N_free(mat[i]);
            if (incrPrecision)
                N_free(backmat[i]);
        }
        N_free(mat);
        if(incrPrecision)
            N_free(backmat);

        return ret;
    }
    //---------------------------------------------------------------------
    // Construct object to consider a specified plane of interest
    PlaneOptimalShadowSetup::PlaneOptimalShadowSetup(PlaneSpaceObj * plane)
    {
        mPlane = plane;
    }
    //---------------------------------------------------------------------
    PlaneOptimalShadowSetup::~PlaneOptimalShadowSetup()
    {
    }
    //---------------------------------------------------------------------
    void PlaneOptimalShadowSetup::setupShadow(Camera * texCam, const SpaceManager * sm, const Camera * cam, 
        const Light * light, NCount level) const
    {
        // get the plane transformed by the parent node(s)
        // Also, make sure the plane is normalized
        Plane worldPlane = mPlane->getSpacePlane();
        worldPlane.normalise();

        // get camera's projection matrix
        Matrix4f camProjection = cam->getProjMatrix() * cam->getViewMatrix();

        // get the world points to constrain
        vector<Vector4f >::type vhull;
        forwardIntersect(cam, worldPlane, &vhull);
        if(vhull.size() < 4)
            return;

        // make sure the last point is a finite point (not point at infinity)
        if(vhull[3].w == 0.0)
        {
            NIIi finiteIndex = -1;
            for (NCount loopIndex = 0; loopIndex < vhull.size(); ++loopIndex)
            {
                if (vhull[loopIndex].w != 0.0)
                {
                    finiteIndex = loopIndex;
                    break;
                }
            }
            if (finiteIndex == -1)
            {
                // there are no finite points, which means camera doesn't see plane of interest.
                // so we don't care what the shadow map matrix is
                // We'll map points off the shadow map so they aren't even stored
                Matrix4f crazyMat(0.0, 0.0, 0.0, 5.0,
                                 0.0, 0.0, 0.0, 5.0,
                                 0.0, 0.0, 0.0, 5.0,
                                 0.0, 0.0, 0.0, 1.0);
                texCam->setCustomViewMatrix(true);
                texCam->setCustomViewMatrix(Matrix4f::IDENTITY);
                texCam->setCustomProjMatrix(true);
                texCam->setCustomProjMatrix(crazyMat);
                return;
            }
            // swap finite point to last point
            std::swap(vhull[3], vhull[finiteIndex]);
        }
        vhull.resize(4);

        // get the post-projective coordinate constraints
        vector<Vector2f >::type constraint;
        for (NIIi i=0; i<4; i++)
        {
            Vector4f postProjPt = camProjection * vhull[i];
            postProjPt *= 1.0 / postProjPt.w;
            constraint.push_back(Vector2f(postProjPt.x, postProjPt.y));
        }

        // perturb one point so we don't have coplanarity
        const Vector4f & pinhole = light->getSpace4();
        const Vector4f & oldPt = vhull.back();
        Vector4f newPt;
        if(pinhole.w == 0)
        {
            // It's directional light
            static const NIIf NEAR_SCALE = 100.0;
            newPt = oldPt + (pinhole * (cam->getNearClipDistance() * NEAR_SCALE));
        }
        else
        {
            // It's point or spotlight
            Vector4f displacement = oldPt - pinhole;
            Vector3f displace3    = Vector3f(displacement.x, displacement.y, displacement.z);
            NIIf dotProd = fabs(displace3.dotProduct(worldPlane.mNormal));
            static const NIIf NEAR_FACTOR = 0.05;
            newPt = pinhole + (displacement * (cam->getNearClipDistance() * NEAR_FACTOR / dotProd));
        }
        vhull.back() = newPt;

        // solve for the matrix that stabilizes the plane
        Matrix4f customMatrix = computeConstrainedProjection(pinhole, vhull, constraint);

        if (pinhole.w == 0)
        {
            // TODO: factor into view and projection pieces.
            // Note: In fact, it's unnecessary to factor into view and projection pieces,
            // but if we do, we will more according with academic requirement :)
            texCam->setCustomViewMatrix(true);
            texCam->setCustomViewMatrix(Matrix4f::IDENTITY);
            texCam->setCustomProjMatrix(true);
            texCam->setCustomProjMatrix(customMatrix);
            return;
        }

        Vector3f tempPos = Vector3f(pinhole.x, pinhole.y, pinhole.z);

        // factor into view and projection pieces
        Matrix4f translation(1.0, 0.0, 0.0,  tempPos.x,
            0.0, 1.0, 0.0,  tempPos.y,
            0.0, 0.0, 1.0,  tempPos.z,
            0.0, 0.0, 0.0,  1.0);
        Matrix4f invTranslation(1.0, 0.0, 0.0, -tempPos.x,
            0.0, 1.0, 0.0, -tempPos.y,
            0.0, 0.0, 1.0, -tempPos.z,
            0.0, 0.0, 0.0,  1.0);
        Matrix4f tempMatrix = customMatrix * translation;
        Vector3f zRow(-tempMatrix[3][0], -tempMatrix[3][1], -tempMatrix[3][2]);
        zRow.normalise();
        Vector3f up;
        if (zRow.y == 1.0)
            up = Vector3f(1,0,0);
        else
            up = Vector3f(0,1,0);
        Vector3f xDir = up.crossProduct(zRow);
        xDir.normalise();
        up = zRow.crossProduct(xDir);
        Matrix4f rotation(xDir.x, up.x, zRow.x, 0.0,
            xDir.y, up.y, zRow.y, 0.0,
            xDir.z, up.z, zRow.z, 0.0,
            0.0,  0.0,    0.0, 1.0 );
        Matrix4f customProj = tempMatrix * rotation;
        Matrix4f customView = rotation.T() * invTranslation;
        // note: now customProj * (0,0,0,1)^t = (0, 0, k, 0)^t for k some constant
        // note: also customProj's 4th row is (0, 0, c, 0) for some negative c.

        // set the shadow map camera
        texCam->setCustomViewMatrix(true);
        texCam->setCustomViewMatrix(customView);
        texCam->setCustomProjMatrix(true);
        texCam->setCustomProjMatrix(customProj);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FocusedShadowSetup::PointListBody
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FocusedShadowSetup::PointListBody::PointListBody()
    {
        // Preallocate some space
        mBodyPoints.reserve(12);
    }
    //-----------------------------------------------------------------------
    FocusedShadowSetup::PointListBody::PointListBody(const ConvexBody & body)
    {
        build(body);
    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::PointListBody::merge(const PointListBody & plb)
    {
        NCount size = plb.getPointCount();
        for(NCount i = 0; i < size; ++i)
        {
            addPoint(plb.getPoint(i));
        }
    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::PointListBody::build(const ConvexBody & body, bool filterDuplicates)
    {
        // erase list
        mBodyPoints.clear();

        // Try to reserve a representative amount of memory
        mBodyPoints.reserve(body.getPolygonCount() * 6);

        // build new list
        for (NCount i = 0; i < body.getPolygonCount(); ++i)
        {
            for (NCount j = 0; j < body.getVertexCount(i); ++j)
            {
                const Vector3f &vInsert = body.getVertex(i, j);

                // duplicates allowed?
                if (filterDuplicates)
                {
                    bool bPresent = false;

                    for(Polygon::VertexList::iterator vit = mBodyPoints.begin();
                        vit != mBodyPoints.end(); ++vit)
                    {
                        const Vector3f& v = *vit;

                        if (vInsert.equal(v))
                        {
                            bPresent = true;
                            break;
                        }
                    }

                    if (bPresent == false)
                    {
                        mBodyPoints.push_back(body.getVertex(i, j));
                    }
                }

                // else insert directly
                else
                {
                    mBodyPoints.push_back(body.getVertex(i, j));
                }
            }
        }
        // update AAB
        // no points altered, so take body AAB
        mAAB = body.getAABB();
    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::PointListBody::buildAndIncludeDirection(
        const ConvexBody & body, NIIf extrudeDist, const Vector3f & dir)
    {
        // reset point list
        this->reset();

        // intersect the rays formed by the points in the list with the given direction and
        // insert them into the list

        const NCount polyCount = body.getPolygonCount();
        for (NCount iPoly = 0; iPoly < polyCount; ++iPoly)
        {

            // store the old inserted point and plane info
            // if the currently processed point hits a different plane than the previous point an
            // intersection point is calculated that lies on the two planes' intersection edge

            // fetch current polygon
            const Polygon & p = body.getPolygon(iPoly);

            NCount pointCount = p.getVertexCount();
            for(NCount iPoint = 0; iPoint < pointCount ; ++iPoint)
            {
                // base point
                const Vector3f & pt = p.getVertex(iPoint);

                // add the base point
                addPoint(pt);

                // intersection ray
                Ray ray(pt, dir);

                const Vector3f ptIntersect = ray.getPoint(extrudeDist);
                addPoint(ptIntersect);
            } // for: polygon point iteration
        } // for: polygon iteration
    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::PointListBody::addPoint(const Vector3f & point)
    {
        // dont check for doubles, simply add
        mBodyPoints.push_back(point);

        // update AAB
        mAAB.merge(point);
    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::PointListBody::addAAB(const AABox & aab)
    {
        const Vector3f & min = aab.getMin();
        const Vector3f & max = aab.getMax();

        Vector3f currentVertex = min;
        // min min min
        addPoint(currentVertex);

        // min min max
        currentVertex.z = max.z;
        addPoint(currentVertex);

        // min max max
        currentVertex.y = max.y;
        addPoint(currentVertex);

        // min max min
        currentVertex.z = min.z;
        addPoint(currentVertex);

        // max max min
        currentVertex.x = max.x;
        addPoint(currentVertex);

        // max max max
        currentVertex.z = max.z;
        addPoint(currentVertex);

        // max min max
        currentVertex.y = min.y;
        addPoint(currentVertex);

        // max min min
        currentVertex.z = min.z;
        addPoint(currentVertex);

    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::PointListBody::reset()
    {
        mBodyPoints.clear();
        mAAB.setNull();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FocusedShadowSetup
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // transform from normal to light space
    const Matrix4f FocusedShadowSetup::msNormalToLightSpace(
        1,  0,  0,  0,        // x
        0,  0, -1,  0,        // y
        0,  1,  0,  0,        // z
        0,  0,  0,  1);    // w
    // transform  from light to normal space
    const Matrix4f FocusedShadowSetup::msLightSpaceToNormal(
        1,  0,  0,  0,        // x
        0,  0,  1,  0,        // y
        0, -1,  0,  0,        // z
        0,  0,  0,  1);    // w
    //-----------------------------------------------------------------------
    FocusedShadowSetup::FocusedShadowSetup()
        : mTempFrustum(N_new Frustum())
        , mLightFrustumCamera(N_new Camera(0/*"TEMP LIGHT INTERSECT CAM" */, NULL))
        , mLightFrustumCameraCalculated(false)
        , mUseAggressiveRegion(true)
    {
        mTempFrustum->setProjType(Frustum::T_Perspective);
    }
    //-----------------------------------------------------------------------
    FocusedShadowSetup::~FocusedShadowSetup()
    {
        N_delete mTempFrustum;
        N_delete mLightFrustumCamera;
    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::calculateShadowMappingMatrix(const SpaceManager & sm,
        const Camera & cam, const Light & light, Matrix4f * out_view, Matrix4f * out_proj,
            Camera * out_cam) const
    {
        // get the shadow frustum's far distance
        NIIf shadowDist = light.getShadowExtent();
        if(!shadowDist)
        {
            // need a shadow distance, make one up
            shadowDist = cam.getNearClipDistance() * 3000;
        }
        NIIf shadowOffset = shadowDist *
            static_cast<ShadowMappingPattern *>(sm.getRenderPattern())->getExtentOffsetFactor();

        if(light.getType() == LT_DIRECTIONAL)
        {
            // generate view matrix if requested
            if(out_view != NULL)
            {
                Vector3f pos;
                if(sm.isCameraOrigin())
                {
                    pos = Vector3f::ZERO;
                }
                else
                {
                    pos = cam.getSpacePos();
                }
                *out_view = buildViewMatrix(pos, light.getSpaceDirection(), cam.getSpaceUp());
            }

            // generate projection matrix if requested
            if(out_proj != NULL)
            {
                *out_proj = ScaleMatrix4(1, 1, -1);
                //*out_proj = Matrix4::IDENTITY;
            }

            // set up camera if requested
            if(out_cam != NULL)
            {
                out_cam->setProjType(Frustum::T_Orthogonal);
                out_cam->setDirection(light.getSpaceDirection());
                out_cam->setPos(cam.getSpacePos());
                out_cam->setFOV(Angle(90));
                out_cam->setNearClipDistance(shadowOffset);
            }
        }
        else if (light.getType() == LT_POINT)
        {
            // target analogue to the default shadow textures
            // Calculate look at position
            // We want to look at a spot shadowOffset away from near plane
            // 0.5 is a little too close for angles
            Vector3f target = cam.getSpacePos() +
                (cam.getSpaceDirection() * shadowOffset);
            Vector3f lightDir = target - light.getSpacePos();
            lightDir.normalise();

            // generate view matrix if requested
            if (out_view != NULL)
            {
                *out_view = buildViewMatrix(light.getSpacePos(),
                    lightDir,
                    cam.getSpaceUp());
            }

            // generate projection matrix if requested
            if (out_proj != NULL)
            {
                // set FOV to 120 degrees
                mTempFrustum->setFOV(Angle(120));

                mTempFrustum->setNearClipDistance(light.getShadowNearClip(cam.getNearClipDistance()));
                mTempFrustum->setFarClipDistance(light.getShadowFarClip(cam.getFarClipDistance()));

                *out_proj = mTempFrustum->getProjMatrix();
            }

            // set up camera if requested
            if (out_cam != NULL)
            {
                out_cam->setProjType(Frustum::T_Perspective);
                out_cam->setDirection(lightDir);
                out_cam->setPos(light.getSpacePos());
                out_cam->setFOV(Angle(120));
                out_cam->setNearClipDistance(light.getShadowNearClip(cam.getNearClipDistance()));
                out_cam->setFarClipDistance(light.getShadowFarClip(cam.getFarClipDistance()));
            }
        }
        else if (light.getType() == LT_SPOT)
        {
            // generate view matrix if requested
            if(out_view != NULL)
            {
                *out_view = buildViewMatrix(light.getSpacePos(),
                    light.getSpaceDirection(),
                    cam.getSpaceUp());
            }

            // generate projection matrix if requested
            if(out_proj != NULL)
            {
                // set FOV slightly larger than spotlight range
                mTempFrustum->setFOV(Clamp<Radian>(light.getSpotlightOuter() * 1.2f, Radian(0), Radian(Math::PI/2.0f)));

                mTempFrustum->setNearClipDistance(light.getShadowNearClip(cam.getNearClipDistance()));
                mTempFrustum->setFarClipDistance(light.getShadowFarClip(cam.getFarClipDistance()));

                *out_proj = mTempFrustum->getProjMatrix();
            }

            // set up camera if requested
            if (out_cam != NULL)
            {
                out_cam->setProjType(Frustum::T_Perspective);
                out_cam->setDirection(light.getSpaceDirection());
                out_cam->setPos(light.getSpacePos());
                out_cam->setFOV(Clamp<Radian>(light.getSpotlightOuter() * 1.2f, Radian(0), Radian(Math::PI/2.0f)));
                out_cam->setNearClipDistance(light.getShadowNearClip(cam.getNearClipDistance()));
                out_cam->setFarClipDistance(light.getShadowFarClip(cam.getFarClipDistance()));
            }
        }
    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::calculateB(const SpaceManager & sm, const Camera& cam,
        const Light & light, const AABox & sceneBB, const AABox & receiverBB,
            PointListBody * out_bodyB) const
    {
        N_assert(out_bodyB != NULL, "bodyB vertex list is NULL");

        // perform convex intersection of the form B = ((V \cap S) + l) \cap S \cap L

        // get V
        mBodyB.define(cam);

        if (light.getType() != LT_DIRECTIONAL)
        {
            // clip bodyB with sceneBB
            /* Note, Matthias' original code states this:
            "The procedure ((V \cap S) + l) \cap S \cap L (Wimmer et al.) leads in some
            cases to disappearing shadows. Valid parts of the scene are clipped, so shadows
            are partly incomplete. The cause may be the    transformation into light space that
            is only done for the corner points which may not contain the whole scene afterwards
            any more. So we fall back to the method of Stamminger et al. (V + l) \cap S \cap L
            which does not show these anomalies."

            .. leading to the commenting out of the below clip. However, ift makes a major
            difference to the quality of the focus, and so far I haven't noticed
            the clipping issues described. Intuitively I would have thought that
            any clipping issue would be due to the findCastListForLight not being
            quite right, since if the sceneBB includes those there is no reason for
            this clip instruction to omit a genuine shadow caster.

            I have made this a user option since the quality effect is major and
            the clipping problem only seems to occur in some specific cases.
            */
            if (mUseAggressiveRegion)
                mBodyB.clip(sceneBB);

            // form a convex hull of bodyB with the light position
            mBodyB.extend(light.getSpacePos());

            // clip bodyB with sceneBB
            mBodyB.clip(sceneBB);

            // clip with the light frustum
            // set up light camera to clip with the resulting frustum planes
            if (!mLightFrustumCameraCalculated)
            {
                calculateShadowMappingMatrix(sm, cam, light, NULL, NULL, mLightFrustumCamera);
                mLightFrustumCameraCalculated = true;
            }
            mBodyB.clip(*mLightFrustumCamera);

            // extract bodyB vertices
            out_bodyB->build(mBodyB);

        }
        else
        {
            // For directional lights, all we care about is projecting the receivers
            // backwards towards the light, clipped by the camera region
            mBodyB.clip(receiverBB);

            // Also clip based on shadow far distance if appropriate
            NIIf farDist = light.getShadowExtent();
            if (farDist)
            {
                Vector3f pointOnPlane = cam.getSpacePos() +
                    (cam.getSpaceDirection() * farDist);
                Plane p(cam.getSpaceDirection(), pointOnPlane);
                mBodyB.clip(p);
            }

            // Extrude the intersection bodyB into the inverted light direction and store
            // the info in the point list.
            // Maximum extrusion extent is to the shadow far distance
            out_bodyB->buildAndIncludeDirection(mBodyB,
                farDist ? farDist : cam.getNearClipDistance() * 3000,
                    -light.getSpaceDirection());
        }
    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::calculateLVS(const SpaceManager& sm, const Camera& cam,
        const Light& light, const AABox& sceneBB, PointListBody *out_LVS) const
    {
        ConvexBody bodyLVS;

        // init body with view frustum
        bodyLVS.define(cam);

        // clip the body with the light frustum (point + spot)
        // for a directional light the space of the intersected
        // view frustum and sceneBB is always lighted and in front
        // of the viewer.
        if (light.getType() != LT_DIRECTIONAL)
        {
            // clip with the light frustum
            // set up light camera to clip the resulting frustum
            if (!mLightFrustumCameraCalculated)
            {
                calculateShadowMappingMatrix(sm, cam, light, NULL, NULL, mLightFrustumCamera);
                mLightFrustumCameraCalculated = true;
            }
            bodyLVS.clip(*mLightFrustumCamera);
        }

        // clip the body with the scene bounding box
        bodyLVS.clip(sceneBB);

        // extract bodyLVS vertices
        out_LVS->build(bodyLVS);
    }
    //-----------------------------------------------------------------------
    Vector3f FocusedShadowSetup::getLSProjViewDir(const Matrix4f& lightSpace,
        const Camera& cam, const PointListBody& bodyLVS) const
    {
        // goal is to construct a view direction
        // because parallel lines are not parallel any more after perspective projection we have to transform
        // a ray to point us the viewing direction

        // fetch a point near the camera
        const Vector3f e_world = getNearCameraPoint_ws(cam.getViewMatrix(), bodyLVS);

        // plus the direction results in a second point
        const Vector3f b_world = e_world + cam.getSpaceDirection();

        // transformation into light space
        const Vector3f e_ls = lightSpace * e_world;
        const Vector3f b_ls = lightSpace * b_world;

        // calculate the projection direction, which is the subtraction of
        // b_ls from e_ls. The y component is set to 0 to project the view
        // direction into the shadow map plane.
        Vector3f projectionDir(b_ls - e_ls);
        projectionDir.y = 0;

        // deal with Y-only vectors
        if(Math::isEqual(projectionDir.length(), 0.0))
            return Vector3f::INV_Z;

        Vector3f norl;
        projectionDir.normalise(norl);
        return norl;
    }
    //-----------------------------------------------------------------------
    Vector3f FocusedShadowSetup::getNearCameraPoint_ws(const Matrix4f& viewMatrix,
        const PointListBody& bodyLVS) const
    {
        if (bodyLVS.getPointCount() == 0)
            return Vector3f(0,0,0);

        Vector3f nearEye = viewMatrix * bodyLVS.getPoint(0),    // for comparison
            nearWorld = bodyLVS.getPoint(0);                // represents the final point

        // store the vertex with the highest z-value which is the nearest point
        for (NCount i = 1; i < bodyLVS.getPointCount(); ++i)
        {
            const Vector3f& vWorld = bodyLVS.getPoint(i);

            // comparison is done from the viewer
            Vector3f vEye = viewMatrix * vWorld;

            if (vEye.z > nearEye.z)
            {
                nearEye        = vEye;
                nearWorld    = vWorld;
            }
        }

        return nearWorld;
    }
    //-----------------------------------------------------------------------
    Matrix4f FocusedShadowSetup::transformToUnitCube(const Matrix4f & m, const PointListBody & body) const
    {
        // map the transformed body AAB points to the unit cube (-1/-1/-1) / (+1/+1/+1) corners
        AABox aab_trans;

        for(NCount i = 0; i < body.getPointCount(); ++i)
        {
            aab_trans.merge(m * body.getPoint(i));
        }

        Vector3f vMin, vMax;

        vMin = aab_trans.getMin();
        vMax = aab_trans.getMax();

        const Vector3f trans(-(vMax.x + vMin.x) / (vMax.x - vMin.x),
            -(vMax.y + vMin.y) / (vMax.y - vMin.y),
            -(vMax.z + vMin.z) / (vMax.z - vMin.z));

        const Vector3f scale(2 / (vMax.x - vMin.x),
            2 / (vMax.y - vMin.y),
            2 / (vMax.z - vMin.z));

        Matrix4f mOut(Matrix4f::IDENTITY);
        mOut.setTrans(trans);
        mOut.setScale(scale);

        return mOut;
    }
    //-----------------------------------------------------------------------
    Matrix4f FocusedShadowSetup::buildViewMatrix(const Vector3f & pos, const Vector3f & dir, const Vector3f & up) const
    {
        Vector3f xN = dir.crossProduct(up);
        xN.normalise();
        Vector3f upN = xN.crossProduct(dir);
        upN.normalise();

        Matrix4f m(xN.x,   xN.y,   xN.z,   -xN.dotProduct(pos),
                    upN.x,  upN.y,  upN.z,  -upN.dotProduct(pos),
                    -dir.x, -dir.y, -dir.z, dir.dotProduct(pos),
                    0.0,    0.0,    0.0,    1.0         );
        return m;
    }
    //-----------------------------------------------------------------------
    void FocusedShadowSetup::setupShadow(Camera * texCam, const SpaceManager * sm, const Camera * cam, 
        const Light * light, NCount level) const
    {
        // check availability - viewport not needed
        N_assert(sm != NULL, "SpaceManager is NULL");
        N_assert(cam != NULL, "Camera (viewer) is NULL");
        N_assert(light != NULL, "Light is NULL");
        N_assert(texCam != NULL, "Camera (texture) is NULL");
        mLightFrustumCameraCalculated = false;

        texCam->setNearClipDistance(light->getShadowNearClip(cam->getNearClipDistance()));
        texCam->setFarClipDistance(light->getShadowFarClip(cam->getFarClipDistance()));

        // calculate standard shadow mapping matrix
        Matrix4f LView, LProj;
        calculateShadowMappingMatrix(*sm, *cam, *light, &LView, &LProj, NULL);

        // build scene bounding box
        AABox sceneBB = texCam->getBound(ST_Normal);
        const AABox & receiverAABB = cam->getBound(ShadowType::ST_Receive);
        sceneBB.merge(receiverAABB);
        sceneBB.merge(cam->getSpacePos());

        // in case the sceneBB is empty (e.g. nothing visible to the cam) simply
        // return the standard shadow mapping matrix
        if (sceneBB.isNull())
        {
            texCam->setCustomViewMatrix(true);
            texCam->setCustomViewMatrix(LView);
            texCam->setCustomProjMatrix(true);
            texCam->setCustomProjMatrix(LProj);
            return;
        }

        // calculate the intersection body B
        mPointListBodyB.reset();
        calculateB(*sm, *cam, *light, sceneBB, receiverAABB, &mPointListBodyB);

        // in case the bodyB is empty (e.g. nothing visible to the light or the cam)
        // simply return the standard shadow mapping matrix
        if(mPointListBodyB.getPointCount() == 0)
        {
            texCam->setCustomViewMatrix(true);
            texCam->setCustomViewMatrix(LView);
            texCam->setCustomProjMatrix(true);
            texCam->setCustomProjMatrix(LProj);
            return;
        }

        // transform to light space: y -> -z, z -> y
        LProj = msNormalToLightSpace * LProj;

        // calculate LVS so it does not need to be calculated twice
        // calculate the body L \cap V \cap S to make sure all returned points are in
        // front of the camera
        mPointListBodyLVS.reset();
        calculateLVS(*sm, *cam, *light, sceneBB, &mPointListBodyLVS);

        // fetch the viewing direction
        const Vector3f viewDir = getLSProjViewDir(LProj * LView, *cam, mPointListBodyLVS);

        // The light space will be rotated in such a way, that the projected light view
        // always points upwards, so the up-vector is the y-axis (we already prepared the
        // light space for this usage).The transformation matrix is set up with the
        // following parameters:
        // - position is the origin
        // - the view direction is the calculated viewDir
        // - the up vector is the y-axis
        LProj = buildViewMatrix(Vector3f::ZERO, viewDir, Vector3f::Y) * LProj;

        // map bodyB to unit cube
        LProj = transformToUnitCube(LProj * LView, mPointListBodyB) * LProj;

        // transform from light space to normal space: y -> z, z -> -y
        LProj = msLightSpaceToNormal * LProj;

        // set the two custom matrices
        texCam->setCustomViewMatrix(true);
        texCam->setCustomViewMatrix(LView);
        texCam->setCustomProjMatrix(true);
        texCam->setCustomProjMatrix(LProj);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // LiSPSMShadowSetup
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    LiSPSMShadowSetup::LiSPSMShadowSetup() :
        mOptAdjustFactor(0.1f),
        mUseSimpleNOpt(true),
        mOptAdjustFactorTweak(1.0),
        mCosCamLightDirThreshold(0.9f)
    {
    }
    //-----------------------------------------------------------------------
    LiSPSMShadowSetup::~LiSPSMShadowSetup()
    {
    }
    //-----------------------------------------------------------------------
    Matrix4f LiSPSMShadowSetup::calculateLiSPSM(const Matrix4f & lightSpace,
        const PointListBody & bodyB, const PointListBody & bodyLVS,
            const SpaceManager & sm, const Camera & cam, const Light & light) const
    {
        // set up bodyB AAB in light space
        AABox bodyBAAB_ls;
        for(NCount i = 0; i < bodyB.getPointCount(); ++i)
        {
            bodyBAAB_ls.merge(lightSpace * bodyB.getPoint(i));
        }

        // near camera point in light space
        const Vector3f e_ls = lightSpace * getNearCameraPoint_ws(cam.getViewMatrix(), bodyLVS);

        // C_start has x and y of e and z from the bodyABB_ls (we look down the negative z axis, so take the maximum z value)
        const Vector3f C_start_ls(e_ls.x, e_ls.y, bodyBAAB_ls.getMax().z);

        // calculate the optimal distance between origin and near plane
        NIIf n_opt;

        if (mUseSimpleNOpt)
            n_opt = calculateNOptSimple(bodyLVS, cam);
        else
            n_opt = calculateNOpt(lightSpace, bodyBAAB_ls, bodyLVS, cam);

        // in case n_opt is null, uniform shadow mapping will be done
        if (n_opt <= 0.0)
        {
            return Matrix4f::IDENTITY;
        }

        // calculate the projection center C which is n units behind the near plane of P
        // we look into the negative z direction so add n
        const Vector3f C(C_start_ls + n_opt * Vector3f::Z);

        // set up a transformation matrix to transform the light space to its new origin
        Matrix4f lightSpaceTranslation(Matrix4f::IDENTITY);
        lightSpaceTranslation.setTrans(-C);

        // range from bMin to bMax; d = |B_z_far - B_z_near|
        NIIf d = Math::Abs(bodyBAAB_ls.getMax().z - bodyBAAB_ls.getMin().z);

        // set up the LiSPSM perspective transformation
        // build up frustum to map P onto the unit cube with (-1/-1/-1) and (+1/+1/+1)
        Matrix4f P = buildFrustumProjection(-1, 1, -1, 1, n_opt + d, n_opt);

        return P * lightSpaceTranslation;
    }
    //-----------------------------------------------------------------------
    NIIf LiSPSMShadowSetup::calculateNOpt(const Matrix4f & lightSpace,
        const AABox & bodyBABB_ls, const PointListBody & bodyLVS, const Camera & cam) const
    {
        // get inverse light space matrix
        Matrix4f invLightSpace = lightSpace.inverse();

        // get view matrix
        const Matrix4f& viewMatrix = cam.getViewMatrix();

        // calculate z0_ls
        const Vector3f e_ws  = getNearCameraPoint_ws(viewMatrix, bodyLVS);
        const Vector3f z0_ls = calculateZ0_ls(lightSpace, e_ws, bodyBABB_ls.getMax().z, cam);

        // z1_ls has the same x and y values as z0_ls and the minimum z values of bodyABB_ls
        const Vector3f z1_ls = Vector3f(z0_ls.x, z0_ls.y, bodyBABB_ls.getMin().z);

        // world
        const Vector3f z0_ws = invLightSpace * z0_ls;
        const Vector3f z1_ws = invLightSpace * z1_ls;

        // eye
        const Vector3f z0_es = viewMatrix * z0_ws;
        const Vector3f z1_es = viewMatrix * z1_ws;

        const NIIf z0 = z0_es.z;
        const NIIf z1 = z1_es.z;

        // check if we have to do uniform shadow mapping
        if ((z0 < 0 && z1 > 0) ||
            (z1 < 0 && z0 > 0))
        {
            // apply uniform shadow mapping
            return 0.0;
        }
        return cam.getNearClipDistance() + Math::sqrt(z0 * z1) * getOptimalAdjustFactor() * mOptAdjustFactorTweak;
    }
    //-----------------------------------------------------------------------
    NIIf LiSPSMShadowSetup::calculateNOptSimple(const PointListBody & bodyLVS, const Camera & cam) const
    {
        // get view matrix
        const Matrix4f& viewMatrix = cam.getViewMatrix();

        // calculate e_es
        const Vector3f e_ws  = getNearCameraPoint_ws(viewMatrix, bodyLVS);
        const Vector3f e_es = viewMatrix * e_ws;

        // according to the new formula (mainly for directional lights)
        // n_opt = zn + sqrt(z0 * z1);
        // zn is set to Abs(near eye point)
        // z0 is set to the near camera clip distance
        // z1 is set to the far camera clip distance
        return (Math::Abs(e_es.z) + Math::sqrt(cam.getNearClipDistance() * cam.getFarClipDistance())) * getOptimalAdjustFactor() * mOptAdjustFactorTweak;
    }
    //-----------------------------------------------------------------------
    Vector3f LiSPSMShadowSetup::calculateZ0_ls(const Matrix4f & lightSpace,
        const Vector3f & e, NIIf bodyB_zMax_ls, const Camera & cam) const
    {
        // z0_ls lies on the intersection point between the planes 'bodyB_ls near plane
        // (z = bodyB_zNear_ls)' and plane with normal X where e_ls lies upon (x = e_ls_x)
        // and the camera's near clipping plane (ls). We are looking towards the negative
        // z-direction, so bodyB_zNear_ls equals bodyB_zMax_ls.

        const Vector3f & camDir = cam.getSpaceDirection();
        const Vector3f e_ls = lightSpace * e;

        // set up a plane with the camera direction as normal and e as a point on the plane
        Plane plane(camDir, e);

        plane.transform(lightSpace);

        // try to intersect plane with a ray from origin V3(e_ls_x, 0.0, bodyB_zNear_ls)T
        // and direction +/- Y
        Ray ray(Vector3f(e_ls.x, 0.0, bodyB_zMax_ls), Vector3f::Y);
        std::pair< bool, NIIf > intersect;
        intersect.first = ray.intersects(plane, intersect.second);

        // we got an intersection point
        if (intersect.first == true)
        {
            return ray.getPoint(intersect.second);
        }
        else
        {
            // try the other direction
            ray = Ray(Vector3f(e_ls.x, 0.0, bodyB_zMax_ls), Vector3f::INV_Y);
            std::pair< bool, NIIf > intersect;
            intersect.first = ray.intersects(plane, intersect.second);

            // we got an intersection point
            if (intersect.first == true)
            {
                return ray.getPoint(intersect.second);
            }
            else
            {
                // failure!
                return Vector3f(0.0, 0.0, 0.0);
            }
        }
    }
    //-----------------------------------------------------------------------
    Matrix4f LiSPSMShadowSetup::buildFrustumProjection(NIIf left, NIIf right,
        NIIf bottom, NIIf top, NIIf nearf, NIIf farf) const
    {
        // Changed to nearf because windows defines near and far as a macros
        NIIf m00 = 2 * nearf / (right - left),
            m02 = (right + left) / (right - left),
            m11 = 2 * nearf / (top - bottom),
            m12 = (top + bottom) / (top - bottom),
            m22 = -(farf + nearf) / (farf - nearf),
            m23 = -2 * farf * nearf / (farf - nearf),
            m32 = -1;

        Matrix4f m(m00, 0.0, m02, 0.0,
            0.0, m11, m12, 0.0,
            0.0, 0.0, m22, m23,
            0.0, 0.0, m32, 0.0);

        return m;
    }
    //-----------------------------------------------------------------------
    void LiSPSMShadowSetup::setupShadow(Camera * texCam, const SpaceManager * sm, const Camera * cam, 
        const Light * light, NCount level) const
    {
        // check availability - viewport not needed
        N_assert(sm != NULL, "SpaceManager is NULL");
        N_assert(cam != NULL, "Camera (viewer) is NULL");
        N_assert(light != NULL, "Light is NULL");
        N_assert(texCam != NULL, "Camera (texture) is NULL");
        mLightFrustumCameraCalculated = false;

        // calculate standard shadow mapping matrix
        Matrix4f LView, LProj;
        calculateShadowMappingMatrix(*sm, *cam, *light, &LView, &LProj, NULL);

        // if the direction of the light and the direction of the camera tend to be parallel,
        // then tweak up the adjust factor
        NIIf dot = Math::Abs(cam->getSpaceDirection().dotProduct(light->getSpaceDirection()));
        if(dot >= mCosCamLightDirThreshold)
        {
            mOptAdjustFactorTweak = 1.0f + (20.0f * ((dot - mCosCamLightDirThreshold) / (1.0f - mCosCamLightDirThreshold)) );
        }
        else
        {
            mOptAdjustFactorTweak = 1.0f;
        }

        // build scene bounding box
        AABox sceneBB = texCam->getBound(ST_Normal);
        const AABox & receiverAABB = cam->getBound(ShadowType::ST_Receive);
        sceneBB.merge(receiverAABB);
        sceneBB.merge(cam->getSpacePos());

        // in case the sceneBB is empty (e.g. nothing visible to the cam) simply
        // return the standard shadow mapping matrix
        if(sceneBB.isNull())
        {
            texCam->setCustomViewMatrix(true);
            texCam->setCustomViewMatrix(LView);
            texCam->setCustomProjMatrix(true);
            texCam->setCustomProjMatrix(LProj);
            return;
        }

        // calculate the intersection body B
        mPointListBodyB.reset();
        calculateB(*sm, *cam, *light, sceneBB, receiverAABB, &mPointListBodyB);

        // in case the bodyB is empty (e.g. nothing visible to the light or the cam)
        // simply return the standard shadow mapping matrix
        if (mPointListBodyB.getPointCount() == 0)
        {
            texCam->setCustomViewMatrix(true);
            texCam->setCustomViewMatrix(LView);
            texCam->setCustomProjMatrix(true);
            texCam->setCustomProjMatrix(LProj);
            return;
        }

        // transform to light space: y -> -z, z -> y
        LProj = msNormalToLightSpace * LProj;

        // calculate LVS so it does not need to be calculated twice
        // calculate the body L \cap V \cap S to make sure all returned points are in
        // front of the camera
        calculateLVS(*sm, *cam, *light, sceneBB, &mPointListBodyLVS);

        // fetch the viewing direction
        const Vector3f viewDir = getLSProjViewDir(LProj * LView, *cam, mPointListBodyLVS);

        // The light space will be rotated in such a way, that the projected light view
        // always points upwards, so the up-vector is the y-axis (we already prepared the
        // light space for this usage).The transformation matrix is set up with the
        // following parameters:
        // - position is the origin
        // - the view direction is the calculated viewDir
        // - the up vector is the y-axis
        LProj = buildViewMatrix(Vector3f::ZERO, viewDir, Vector3f::Y) * LProj;

        // calculate LiSPSM projection
        LProj = calculateLiSPSM(LProj * LView, mPointListBodyB, mPointListBodyLVS, *sm, *cam, *light) * LProj;

        // map bodyB to unit cube
        LProj = transformToUnitCube(LProj * LView, mPointListBodyB) * LProj;

        // transform from light space to normal space: y -> z, z -> -y
        LProj = msLightSpaceToNormal * LProj;

        // LView = Lv^-1
        // LProj = Switch_{-ls} * FocusBody * P * L_r * Switch_{ls} * L_p
        texCam->setCustomViewMatrix(true);
        texCam->setCustomViewMatrix(LView);
        texCam->setCustomProjMatrix(true);
        texCam->setCustomProjMatrix(LProj);
    }
    //---------------------------------------------------------------------
    void LiSPSMShadowSetup::setCameraLightDirectionThreshold(Angle angle)
    {
        mCosCamLightDirThreshold = Math::Cos(angle.getRadian());
    }
    //---------------------------------------------------------------------
    Angle LiSPSMShadowSetup::getCameraLightDirectionThreshold() const
    {
        return Angle(Math::ACos(mCosCamLightDirThreshold));
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // PSSMShadowSetup
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    PSSMShadowSetup::PSSMShadowSetup() : mSplitPadding(1.0f)
    {
        calculateSplitPoints(3, 100, 100000);
        setOptimalAdjustFactor(0, 5);
        setOptimalAdjustFactor(1, 1);
        setOptimalAdjustFactor(2, 0);
    }
    //---------------------------------------------------------------------
    PSSMShadowSetup::~PSSMShadowSetup()
    {
    }
    //---------------------------------------------------------------------
    void PSSMShadowSetup::calculateSplitPoints(NCount splitCount, NIIf nearDist, NIIf farDist, NIIf lambda)
    {
        if(splitCount < 2)
            N_EXCEPT(InvalidParam, _I18n("Cannot specify less than 2 splits"));

        mSplitPoints.resize(splitCount + 1);
        mOptimalAdjustFactors.resize(splitCount);
        mSplitCount = splitCount;

        mSplitPoints[0] = nearDist;
        for (NCount i = 1; i < mSplitCount; i++)
        {
            NIIf fraction = (NIIf)i / (NIIf)mSplitCount;
            NIIf splitPoint = lambda * nearDist * Math::Pow(farDist / nearDist, fraction) +
                (1.0f - lambda) * (nearDist + fraction * (farDist - nearDist));

            mSplitPoints[i] = splitPoint;
        }
        mSplitPoints[splitCount] = farDist;
    }
    //---------------------------------------------------------------------
    void PSSMShadowSetup::setSplitPoints(const SplitPointList & newSplitPoints)
    {
        if (newSplitPoints.size() < 3) // 3, not 2 since splits + 1 points
            N_EXCEPT(InvalidParam, _I18n("Cannot specify less than 2 splits"));
        mSplitCount = newSplitPoints.size() - 1;
        mSplitPoints = newSplitPoints;
        mOptimalAdjustFactors.resize(mSplitCount);
    }
    //---------------------------------------------------------------------
    void PSSMShadowSetup::setOptimalAdjustFactor(NCount splitIndex, NIIf factor)
    {
        if (splitIndex >= mOptimalAdjustFactors.size())
            N_EXCEPT(InvalidParam, _I18n("Split index out of range"));
        mOptimalAdjustFactors[splitIndex] = factor;
    }
    //---------------------------------------------------------------------
    void PSSMShadowSetup::setupShadow(Camera * texCam, const SpaceManager * sm, const Camera * cam, 
        const Light * light, NCount level) const
    {
        // apply the right clip distance.
        NIIf nearDist = mSplitPoints[level];
        NIIf farDist = mSplitPoints[level + 1];

        // Add a padding factor to internal distances so that the connecting split point will 
        // not have bad artifacts.
        if(level > 0)
        {
            nearDist -= mSplitPadding;
        }
        if(level < mSplitCount - 1)
        {
            farDist += mSplitPadding;
        }

        mCurrentIteration = level;

        // Ouch, I know this is hacky, but it's the easiest way to re-use LiSPSM / Focussed
        // functionality right now without major changes
        Camera * _cam = const_cast<Camera *>(cam);
        NIIf oldNear = _cam->getNearClipDistance();
        NIIf oldFar = _cam->getFarClipDistance();
        _cam->setNearClipDistance(nearDist);
        _cam->setFarClipDistance(farDist);

        LiSPSMShadowSetup::setupShadow(texCam, sm, cam, light, level);

        // restore near/far
        _cam->setNearClipDistance(oldNear);
        _cam->setFarClipDistance(oldFar);
    }
    //--------------------------------------------------------------------------------
}