#include "Exporter.h"
#include <iostream>
#include <string>
#include "stdafx.h"
#include "SemanticLayer.h"
#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreMeshManager.h"
#include "OgreSkeletonManager.h"
#include "OgreAnimation.h"
#include "NiiKeyFrameTrack.h"
#include "OgreKeyFrame.h"
#include "OgreMesh.h"
#include "OgreSubMesh.h"
#include "OgreSkeleton.h"
#include "OgreBone.h"
#include "NiiMemGBufferManager.h"
#include "OgreMeshSerializer.h"
#include "OgreSkeletonSerializer.h"
#include "OgrePrerequisites.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreVector3.h"
#include "OgreColourValue.h"

using namespace Ogre;

//----------------------- GLOBALS FOR SINGLETONS -------------
LogManager * logMgr;
ResourceSchemeManager * rgm;
MeshManager * meshMgr;
MemGBufferManager * hardwareBufMgr;
SkeletonManager * skelMgr;

//------------------------------------------------------------
Exporter::UniqueVertex::UniqueVertex() :
    initialized(false),
    position(Ogre::Vector3::ZERO),
    normal(Ogre::Vector3::ZERO),
    color(0),
    nextIndex(0)
{
    for (int i = 0; i < OGRE_MAX_TEXTURE_COORD_SETS; ++i)
        uv[i] = Ogre::Vector2::ZERO;
}

//------------------------------------------------------------
bool Exporter::UniqueVertex::operator ==(const UniqueVertex& rhs) const
{
    bool ret = position == rhs.position &&
        normal == rhs.normal &&
        color == rhs.color;
    if (!ret) return ret;
    for (int i = 0; i < OGRE_MAX_TEXTURE_COORD_SETS && ret; ++i)
    {
        ret = ret && (uv[i] == rhs.uv[i]);
    }
    return ret;
}

//------------------------------------------------------------
Exporter::Exporter(CSLModel * Root)
{
    // Initialize Exporter object instance variables
    this->SceneRoot = Root;
    boneCount = 0;
}

//------------------------------------------------------------
Exporter::~Exporter()
{
}

//------------------------------------------------------------
void Exporter::exportMesh(std::string fileName, std::string skelName)
{
    // Construct mesh
    MeshPtr pMesh = N_Only(Mesh).createManual(fileName, GroupDefault);
    pMesh->setSkeletonID(skelName);

    // We'll assume we want to export the entire scene
    exportCSLModel(pMesh.getPointer(), SceneRoot);
    MeshSerializer serializer;
    serializer.exportMesh(pMesh.getPointer(), fileName);
}

//--------------------------------------------------------------------------
void Exporter::exportCSLModel(Mesh* pMesh, CSLModel* XSIModel)
{
    if (XSIModel->GetPrimitiveType() == CSLTemplate::SI_MESH)
        exportSubMesh(pMesh, (CSLMesh *) XSIModel->Primitive());

    CSLModel* *l_childrenList = XSIModel->GetChildrenList();

    // Loop through all children
    for (int i = 0; i < XSIModel->GetChildrenCount(); i++ )
    {
        exportCSLModel (pMesh, l_childrenList[i]);
    }
}

//-------------------------------------------------------------------------
void Exporter::exportSubMesh(Mesh *pMesh, CSLMesh* XSIMesh)
{
    SubMesh * sm = 0;
    sm = pMesh->createSub();
    sm->setName(XSIMesh->GetName());
    // HACK:  No materials exporter yet, I hard coded this, wrong as hell, but did it anyway
    // For now, I'm just creating the materials file manually.
    sm->setMaterial("Examples/Woman");
    CSLTriangleList** triangles = XSIMesh->TriangleLists();

    // Assume only one triangle list for now
    CSLTriangleList* triArray = triangles[0];
    std::cout << "Number of triangles: " << triArray->GetTriangleCount() << "\n";
    CSIBCVector3D* srcPosArray = XSIMesh->Shape()->GetVertexListPtr();
    std::cout << "Number of vertices: " << XSIMesh->Shape()->GetVertexCount() << "\n";
    CSIBCVector3D* srcNormArray = XSIMesh->Shape()->GetNormalListPtr();
    std::cout << "Number of normals: " << XSIMesh->Shape()->GetNormalCount() << "\n";
    CSLShape_35 * uv = ((CSLShape_35 *) XSIMesh->Shape());
    size_t numUVs = uv->UVCoordArrays()[0]->GetUVCoordCount();
    std::cout << "Number of UVs: " << numUVs << "\n";

    // For now, assume only one set of UV's
    CSIBCVector2D * uvValueArray = ((CSLShape_35 *) XSIMesh->Shape())->UVCoordArrays()[0]->GetUVCoordListPtr();

    // Check for colors
    bool hasVertexColors = false;
    if(XSIMesh->Shape()->GetColorCount() > 0)
        hasVertexColors = true;

    // Never use shared geometry
    sm->setSelfVertex(true);
    VertexData * smvd;
    N_Only(GBuffer).create(smvd);
    sm->setVertexData(smvd)
    // Always do triangle list
    sm->getIndexData(0)->mCount = static_cast<size_t>(triArray->GetTriangleCount() * 3);

    // Identify the unique vertices, write to a temp index buffer
    startPolygonMesh(XSIMesh->Shape()->GetVertexCount(), triArray->GetTriangleCount() * 3);

    // Iterate through all the triangles
    // There will often be less positions than normals and UV's
    for (long t = 0; t < triArray->GetTriangleCount(); ++t)
    {
        for (int p = 0; p < 3; ++p)
        {
            UniqueVertex vertex;
            CSIBCVector3D pos = srcPosArray[triArray->GetVertexIndicesPtr()[t*3+p]];
            CSIBCVector3D norm = srcNormArray[triArray->GetNormalIndicesPtr()[t*3+p]];
            CSIBCVector2D uv = uvValueArray[triArray->GetUVIndicesPtr(0)[t*3+p]];
            vertex.position = Vector3(pos.GetX(), pos.GetY(), pos.GetZ());
            vertex.normal = Vector3(norm.GetX(), norm.GetY(), norm.GetZ());

            // We are assuming 1 UV -- in our files, number of UV's = number of Normals
            vertex.uv[0] = Vector2(uv.GetX(), (1 - uv.GetY()));

            if (hasVertexColors)
                vertex.color = triArray->GetColorIndicesPtr()[t*3+p];
            size_t index = createOrRetrieveUniqueVertex(triArray->GetVertexIndicesPtr()[t*3+p], vertex);
            mIndices.push_back(index);
        }
    }
    delete [] uvValueArray;

    // Now bake final geometry
    smvd->mCount = mUniqueVertices.size();

    // Determine index size
    bool use32BitIndexes = false;
    if(mUniqueVertices.size() > 65536)
        use32BitIndexes = true;
    N_Only(GBuffer).create(sm->getIndexData(0)->mBuffer, use32BitIndexes ? 32 : 16, triArray->GetTriangleCount() * 3, Buffer::M_NORMAL );
    if(use32BitIndexes)
    {
        uint32 * pIdx = static_cast<uint32 *>(sm->getIndexData(0)->mBuffer->lock(Buffer::MM_WHOLE));
        writeIndexes(pIdx);
        sm->getIndexData(0)->mBuffer->unlock();
    }
    else
    {
        uint16 * pIdx = static_cast<uint16 *>(sm->getIndexData(0)->mBuffer->lock(Buffer::MM_WHOLE));
        writeIndexes(pIdx);
        sm->getIndexData(0)->mBuffer->unlock();
    }

    // Define vertex declaration
    unsigned buf = 0;
    size_t offset = 0;
    smvd->add(DT_Float3x, VT_Pos, buf, offset);
    offset += VertexUnit::getTypeSize(DT_Float3x);
    smvd->add(DT_Float3x, VT_Normals, buf, offset);
    offset += VertexUnit::getTypeSize(DT_Float3x);
    // TODO:  Split Vertex Data if animated

    if(hasVertexColors)
    {
        smvd->add(DT_Colour, VT_Diffuse, buf, offset);
        offset += VertexUnit::getTypeSize(DT_Colour);
    }

    // Again, assume only 1 uv
    smvd->add(DT_Float2x, VT_Tex_Coord, buf, offset);
    offset += VertexUnit::getTypeSize(DT_Float2x);

    // Create and fill buffer(s)
    for(unsigned short b = 0; b < smvd->mBufferCount; ++b)
    {
        createVertexBuffer(smvd, b);
    }

    // Bounds definitions
    NIIf squaredRadius = 0.0f;
    Vector3 min, max;
    bool first = true;
    for (long i = 0; i < XSIMesh->Shape()->GetVertexCount(); ++i)
    {
        Vector3 position = Vector3(srcPosArray[i].GetX(), srcPosArray[i].GetY(), srcPosArray[i].GetZ());
        if (first)
        {
            squaredRadius = position.lengthSquared();
            min = max = position;
            first = false;
        }
        else
        {
            squaredRadius = std::max(squaredRadius, position.lengthSquared());
            min = min.floor(position);
            max = max.ceil(position);
        }
    }
    AABox box;
    box.setMin(min);
    box.setMax(max);

    box.merge(pMesh->getAABB());
    pMesh->setAABB(box);
    pMesh->setRangeFactor(0.01);
    pMesh->setCenterRange(std::max(pMesh->getCenterRange(), Math::sqrt(squaredRadius)));

    // Get Envelope list for this submesh
    CSLEnvelope** envelopes = XSIMesh->ParentModel()->GetEnvelopeList();
    CSLEnvelope* env = 0;
    int boneIdx;
    bool done;
    int index;
    SkeletonVertex vertAssign;
    for(int e = 0; e < XSIMesh->ParentModel()->GetEnvelopeCount(); ++e)
    {
        env = envelopes[e];
        for(int g = 0; g < boneCount; ++g)
        {
            if (boneArray[g] == env->GetDeformer()->GetName())
                boneIdx = g;
            else
                continue;
            break;
        }

        SLVertexWeight * wtList = env->GetVertexWeightListPtr();

        // Go through all collocated vertices, assigning the same weights to each.
        // All the dotXSI files I've seen normalize the weights to 100, so for now
        // I'm just dividing by 100.  TODO:  Insert code to handle normalization
        // just in case.
        for (int h = 0; h < env->GetVertexWeightCount(); ++h)
        {
            vertAssign.mBoneIndex = boneIdx;
            vertAssign.mVertexIndex = index = (int) wtList[h].m_fVertexIndex;
            vertAssign.mWeight = (NIIf) (wtList[h].m_fWeight / 100);
            done = false;
            while(!done)
            {
                sm->addSkeleton(vertAssign);
                if(mUniqueVertices[index].nextIndex)
                    vertAssign.mVertexIndex = index = mUniqueVertices[index].nextIndex;
                else
                    done = true;
            }
        }
    }
    // Last step here is to reorganise the vertex buffers
    smvd->build(VertexData::M_None, Buffer::Buffer::M_NORMAL , true);
}
//-----------------------------------------------------------------------------
template <typename T>
void Exporter::writeIndexes(T* buf)
{
    IndexList::const_iterator i, iend = mIndices.end();
    for (i = mIndices.begin(); i != iend; ++i)
    {
        *buf++ = static_cast<T>(*i);
    }
}

//-----------------------------------------------------------------------------
void Exporter::createVertexBuffer(VertexData * vd, unsigned short bufIdx)
{
    VertexBuffer * vbuf;
    N_Only(GBuffer).create(vbuf, vd->getUnitSize(bufIdx), vd->mCount, Buffer::M_NORMAL );
    vd->attach(bufIdx, vbuf);
    size_t vertexSize = vd->getUnitSize(bufIdx);

    char * pBase = static_cast<char *>(vbuf->lock(Buffer::MM_WHOLE));

    VertexUnitList elems = vd->find(bufIdx);
    VertexUnitList::iterator ei, eiend = elems.end();
    float * pFloat;
    RGBA * pRGBA;

    Exporter::UniqueVertexList::iterator srci = mUniqueVertices.begin();

    for (size_t v = 0; v < vd->mCount; ++v, ++srci)
    {
        for (ei = elems.begin(); ei != eiend; ++ei)
        {
            VertexUnit & elem = *ei;
            switch(elem.mVType)
            {
            case VT_Pos:
                elem.getData(pBase, pFloat);
                *pFloat++ = srci->position.x;
                *pFloat++ = srci->position.y;
                *pFloat++ = srci->position.z;
                break;
            case VT_Normals:
                elem.getData(pBase, pFloat);
                *pFloat++ = srci->normal.x;
                *pFloat++ = srci->normal.y;
                *pFloat++ = srci->normal.z;
                break;
            case VT_Diffuse:
                elem.getData(pBase, pRGBA);
                *pRGBA = srci->color;
                break;
            case VT_Tex_Coord:
                elem.getData(pBase, pFloat);
                *pFloat++ = srci->uv[elem.mIndex].x;
                *pFloat++ = srci->uv[elem.mIndex].y;
                break;
            }
        }
        pBase += vertexSize;
    }
    vbuf->unlock();
}

//----------------------------------------------------------------------
void Exporter::startPolygonMesh(size_t count, size_t indexCount)
{
    mUniqueVertices.clear();
    mUniqueVertices.resize(count);
    mIndices.clear();
    mIndices.reserve(indexCount); // intentionally reserved, not resized
}

//----------------------------------------------------------------------
size_t Exporter::createOrRetrieveUniqueVertex(size_t originalPositionIndex, const UniqueVertex& vertex)
{
    UniqueVertex& orig = mUniqueVertices[originalPositionIndex];

    if (!orig.initialized)
    {
        orig = vertex;
        orig.initialized = true;
        return originalPositionIndex;
    }
    else if (orig == vertex)
    {
        return originalPositionIndex;
    }
    else
    {
        // no match, go to next or create new
        if (orig.nextIndex)
        {
            // cascade
            return createOrRetrieveUniqueVertex(orig.nextIndex, vertex);
        }
        else
        {
            // get new index
            size_t newindex = mUniqueVertices.size();
            orig.nextIndex = newindex;
            // create new (NB invalidates 'orig' reference)
            mUniqueVertices.push_back(vertex);
            // set initialised
            mUniqueVertices[newindex].initialized = true;

            return newindex;
        }
    }
}

//------------------------------------------------------------------------------
void Exporter::exportBones(std::string fileName)
{
    // Construct skeleton
    SkeletonPtr pSkel = N_Only(Skeleton).create(fileName, GroupDefault, true);

    // Recursively traverse the bone tree
    root = false;
    recurseBones(pSkel.getPointer(), SceneRoot);

    // Export animations
    exportAnim(pSkel.getPointer(), SceneRoot);

    // Call serializer to write .skeleton file
    SkeletonSerializer serializer;
    serializer.exportSkeleton(pSkel.getPointer(), fileName);
}

//-----------------------------------------------------------------
void Exporter::recurseBones(Skeleton* pSkel, CSLModel* XSIModel)
{
    CSIBCVector3D vec3d;

    // A plethora of logical expressions to ensure that the root null and
    // its children are the only ones that will enter this if block.  Eliminates
    // any extraneous nulls not related to the skeleton.

    if ((XSIModel->GetPrimitiveType() == CSLTemplate::SI_NULL_OBJECT) &&
        ((XSIModel->ParentModel()->GetPrimitiveType() == CSLTemplate::SI_NULL_OBJECT) || (!root)))
    {
        boneArray[boneCount] = XSIModel->GetName();
        Bone* ogreBone = pSkel->create(XSIModel->GetName(), boneCount);
        root = true;
        vec3d = XSIModel->Transform()->GetScale();
        ogreBone->setScale(Vector3f(vec3d.GetX(), vec3d.GetY(), vec3d.GetZ()));
        vec3d = XSIModel->Transform()->GetTranslation();
        Vector3 bonePos(vec3d.GetX(), vec3d.GetY(), vec3d.GetZ());
        ogreBone->setPos(bonePos);

        // Yes, we are converting Euler angles to quaternions, at risk of gimbal lock.
        // This is because XSI doesn't export quaternions, except through the animation
        // mixer and action FCurves.  It's possible to get a 3x3 Rotation matrix, which
        // might be a better choice for conversion to quaternion.
        vec3d = XSIModel->Transform()->GetEulerRotation();
        NII::Quaternion qx, qy, qz, qfinal;
        qx.from(NII::Angle(vec3d.GetX()), NII::Vector3::X);
        qy.from(NII::Angle(vec3d.GetY()), NII::Vector3::Y);
        qz.from(NII::Angle(vec3d.GetZ()), NII::Vector3::Z);

        // Assume rotate by x then y then z
        qfinal = qz * qy * qx;
        ogreBone->setOri(qfinal);
        ++boneCount;

        if((boneCount > 1) && (XSIModel->ParentModel()->GetPrimitiveType() == CSLTemplate::SI_NULL_OBJECT))
        {
            pSkel->get(XSIModel->ParentModel()->GetName())->add(ogreBone);
        }
    }

    CSLModel* *l_childrenList = XSIModel->GetChildrenList();

    // Loop through all children
    for (int i = 0; i < XSIModel->GetChildrenCount(); i++ )
    {
        recurseBones (pSkel, l_childrenList[i]);
    }
}

//------------------------------------------------------------------------------

void Exporter::exportAnim(Skeleton* pSkel, CSLModel* XSIModel)
{
    CSLTransform* initial;
    CSLTransform* keyfr = 0;
    CSIBCMatrix4x4 initmat, invinitmat, keyfmat, newmat;

    // Timing conversions from XSI frames to OGRE time in seconds
    float frameRate = XSIModel->Scene()->SceneInfo()->GetFrameRate();
    float lengthInFrames = XSIModel->Scene()->SceneInfo()->GetEnd() -
        XSIModel->Scene()->SceneInfo()->GetStart();
    float realTime = lengthInFrames / frameRate;

    // HACK:  You'd want to assign the correct name to your particular animation.
    Animation * ogreanim = pSkel->createAnimation("Jump", realTime );
    int i, numKeys;

    // Go to each bone and create the animation tracks
    for (i = 0; i < boneCount; ++i)
    {
        Bone* ogrebone = pSkel->get(boneArray[i]);
        CSLModel* XSIbone = XSIModel->Scene()->FindModelRecursively((char *) boneArray[i].c_str(), XSIModel);
        if ((i == 0) || (XSIbone->ParentModel()->GetPrimitiveType() == CSLTemplate::SI_NULL_OBJECT))
        {
            // Create animation tracks for a bone
            KeyFrameTrack *ogretrack = ogreanim->createTrack(i, ogrebone);
            numKeys = XSIbone->Transform()->FCurves()[0]->GetKeyCount();
            CSLLinearKey* scalx = XSIbone->Transform()->GetSpecificFCurve(CSLTemplate::SI_SCALING_X)->GetLinearKeyListPtr();
            CSLLinearKey* scaly = XSIbone->Transform()->GetSpecificFCurve(CSLTemplate::SI_SCALING_Y)->GetLinearKeyListPtr();
            CSLLinearKey* scalz = XSIbone->Transform()->GetSpecificFCurve(CSLTemplate::SI_SCALING_Z)->GetLinearKeyListPtr();
            CSLLinearKey* rotx = XSIbone->Transform()->GetSpecificFCurve(CSLTemplate::SI_ROTATION_X)->GetLinearKeyListPtr();
            CSLLinearKey* roty = XSIbone->Transform()->GetSpecificFCurve(CSLTemplate::SI_ROTATION_Y)->GetLinearKeyListPtr();
            CSLLinearKey* rotz = XSIbone->Transform()->GetSpecificFCurve(CSLTemplate::SI_ROTATION_Z)->GetLinearKeyListPtr();
            CSLLinearKey* tranx = XSIbone->Transform()->GetSpecificFCurve(CSLTemplate::SI_TRANSLATION_X)->GetLinearKeyListPtr();
            CSLLinearKey* trany = XSIbone->Transform()->GetSpecificFCurve(CSLTemplate::SI_TRANSLATION_Y)->GetLinearKeyListPtr();
            CSLLinearKey* tranz = XSIbone->Transform()->GetSpecificFCurve(CSLTemplate::SI_TRANSLATION_Z)->GetLinearKeyListPtr();

            // Set up the bind pose matrix and take inverse
            initial = XSIbone->Transform();
            initial->ComputeLocalMatrix();
            initmat = initial->GetMatrix();
            initmat.GetInverse(invinitmat);

            for (int currKeyIdx = 0; currKeyIdx < numKeys; ++currKeyIdx)
            {
                // Create keyframe
                // Adjust for start time, and for the fact that frames are numbered from 1
                float frameTime = scalx[currKeyIdx].m_fTime - XSIModel->Scene()->SceneInfo()->GetStart();
                realTime = frameTime / frameRate;
                KeyFrame *ogrekey = ogretrack->create(realTime);
                keyfr = XSIbone->Transform();
                keyfr->SetScale(CSIBCVector3D(scalx[currKeyIdx].m_fValue, scaly[currKeyIdx].m_fValue, scalz[currKeyIdx].m_fValue));
                keyfr->SetEulerRotation(CSIBCVector3D(rotx[currKeyIdx].m_fValue, roty[currKeyIdx].m_fValue, rotz[currKeyIdx].m_fValue));
                keyfr->SetTranslation(CSIBCVector3D(tranx[currKeyIdx].m_fValue, trany[currKeyIdx].m_fValue, tranz[currKeyIdx].m_fValue));
                keyfr->ComputeLocalMatrix();
                keyfmat = keyfr->GetMatrix();

                // Inverse bind pose matrix * keyframe transformation matrix
                invinitmat.Multiply(keyfmat, newmat);
                CSIBCVector3D kfSca, kfRot, kfPos;
                newmat.GetTransforms(kfSca, kfRot, kfPos);
                Vector3 kSca(kfSca.GetX(), kfSca.GetY(), kfSca.GetZ());
                Vector3 kPos(kfPos.GetX(), kfPos.GetY(), kfPos.GetZ());
                Quaternion qx, qy, qz, kfQ;
                ogrekey->setScale(kSca);
                ogrekey->setTranslate(kPos);
                qx.from(NII::Radian(kfRot.GetX()), Vector3::X);
                qy.from(NII::Radian(kfRot.GetY()), Vector3::Y);
                qz.from(NII::Radian(kfRot.GetZ()), Vector3::Z);
                kfQ = qz * qy * qx;
                ogrekey->setRotation(kfQ);
            }
        }
    }
}
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Validate command line arguments
    if (argc != 3) {
        std::cout << "XSI Ogre Exporter should be invoked in the format: \n";
        std::cout << "exporter <XSI File> <OGRE Mesh/Skeleton File>\n";
        std::cout << "Ex:  exporter example.xsi example\n";
        return (0);
    }

    // Ogre Singletons
    logMgr = new LogManager();
    logMgr->createLog("XSIOgreExport");
    rgm = new ResourceSchemeManager();
    meshMgr = new MeshManager();
    hardwareBufMgr = new MemGBufferManager();
    skelMgr = new SkeletonManager();

    // Initialize dotXSI Scene
    CSLScene Scene;
    std::string fn(argv[2]);
    std::string meshFileName = fn + ".mesh";
    std::string skelFileName = fn + ".skeleton";

    // Continue if valid dotXSI file, end gracefully if not
    if (Scene.Open(argv[1]) == SI_SUCCESS)
    {
        Scene.Read();
        Exporter * e = new Exporter(Scene.Root());
        e->exportBones(skelFileName);
        e->exportMesh(meshFileName, skelFileName);
        delete e;
        Scene.Close();
    }
    else
        std::cout << "Error opening file " << argv[1] << ".  Please check for validity.\n";

    // Get rid of Ogre Singletons
    delete skelMgr;
    delete meshMgr;
    delete hardwareBufMgr;
    delete rgm;
    delete logMgr;
    return (0);
}
