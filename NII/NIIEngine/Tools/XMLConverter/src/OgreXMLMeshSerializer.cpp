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

#include "OgreXMLMeshSerializer.h"
#include "OgreSubMesh.h"
#include "OgreLogManager.h"
#include "OgreSkeleton.h"
#include "OgreStringConverter.h"
#include "NiiGBufferManager.h"
#include "OgreException.h"
#include "OgreAnimation.h"
#include "NiiKeyFrameTrack.h"
#include "OgreKeyFrame.h"
#include "OgreLodStrategyManager.h"
#include <cstddef>

namespace Ogre
{
    //---------------------------------------------------------------------
    XMLMeshSerializer::XMLMeshSerializer()
    {
    }
    //---------------------------------------------------------------------
    XMLMeshSerializer::~XMLMeshSerializer()
    {
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::importMesh(const String & filename,
		DataType colourElementType, Mesh * pMesh)
    {
        LogManager::getSingleton().logMessage("XMLMeshSerializer reading mesh data from " + filename + "...");
        mMesh = pMesh;
		mColourElementType = colourElementType;
        mXMLDoc = new TiXmlDocument(filename);
        mXMLDoc->LoadFile();

        TiXmlElement * elem;

        TiXmlElement * rootElem = mXMLDoc->RootElement();

        // shared geometry
        elem = rootElem->FirstChildElement("sharedgeometry");
        if(elem)
        {
            const char * claimedVertexCount_ = elem->Attribute("vertexcount");
            if(!claimedVertexCount_ || StringConverter::parseInt(claimedVertexCount_) > 0)
            {
                N_Only(GBuffer).create(mMesh->getVertexData());
                readGeometry(elem, mMesh->getVertexData());
            }
        }

        // submeshes
        elem = rootElem->FirstChildElement("submeshes");
        if(elem)
            readSubMeshes(elem);

        // skeleton link
        elem = rootElem->FirstChildElement("skeletonlink");
        if(elem)
            readSkeletonLink(elem);

        // bone assignments
        elem = rootElem->FirstChildElement("boneassignments");
        if(elem)
            readBoneAssignments(elem);

		//Lod
		elem = rootElem->FirstChildElement("levelofdetail");
		if(elem)
			readLodInfo(elem);

		// submesh names
		elem = rootElem->FirstChildElement("submeshnames");
		if(elem)
			readSubMeshNames(elem, mMesh);

		// submesh extremes
		elem = rootElem->FirstChildElement("extremes");
		if(elem)
			readExtremes(elem, mMesh);

		// poses
		elem = rootElem->FirstChildElement("poses");
		if(elem)
			readPoses(elem, mMesh);

		// animations
		elem = rootElem->FirstChildElement("animations");
		if(elem)
			readAnimations(elem, mMesh);

		delete mXMLDoc;

        LogManager::getSingleton().logMessage("XMLMeshSerializer import successful.");
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::exportMesh(const Mesh * pMesh, const String & filename)
    {
        LogManager::getSingleton().logMessage("XMLMeshSerializer writing mesh data to " + filename + "...");

        mMesh = const_cast<Mesh *>(pMesh);

        mXMLDoc = new TiXmlDocument();
        mXMLDoc->InsertEndChild(TiXmlElement("mesh"));

        LogManager::getSingleton().logMessage("Populating DOM...");

        // Write to DOM
        writeMesh(pMesh);
        LogManager::getSingleton().logMessage("DOM populated, writing XML file..");

        // Write out to a file
        if(! mXMLDoc->SaveFile(filename) )
        {
            LogManager::getSingleton().logMessage("XMLMeshSerializer failed writing the XML file.", LML_CRITICAL);
        }
        else
        {
            LogManager::getSingleton().logMessage("XMLMeshSerializer export successful.");
        }

        delete mXMLDoc;
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::writeMesh(const Mesh* pMesh)
    {
        TiXmlElement * rootNode = mXMLDoc->RootElement();
        // Write geometry
		if(pMesh->getVertexData())
		{
			TiXmlElement * geomNode = rootNode->InsertEndChild(TiXmlElement("sharedgeometry"))->ToElement();
			writeGeometry(geomNode, pMesh->getVertexData());
		}

        // Write Submeshes
        TiXmlElement * subMeshesNode = rootNode->InsertEndChild(TiXmlElement("submeshes"))->ToElement();
        for(int i = 0; i < pMesh->getSubCount(); ++i)
        {
            LogManager::getSingleton().logMessage("Writing submesh...");
            writeSubMesh(subMeshesNode, pMesh->getSub(i));
            LogManager::getSingleton().logMessage("Submesh exported.");
        }

        // Write skeleton info if required
        if(pMesh->isSkeletonFusion())
        {
            LogManager::getSingleton().logMessage("Exporting skeleton link...");
            // Write skeleton link
            writeSkeletonLink(rootNode, pMesh->getSkeletonID());
            LogManager::getSingleton().logMessage("Skeleton link exported.");

            // Write bone assignments
            Mesh::SkeletonVertexList::iterator bi = const_cast<Mesh *>(pMesh)->getSkeletonList().begin();
            Mesh::SkeletonVertexList::iterator biend = const_cast<Mesh *>(pMesh)->getSkeletonList().end();
            if(bi != biend)
            {
                LogManager::getSingleton().logMessage("Exporting shared geometry bone assignments...");
                TiXmlElement * boneAssignNode =
                    rootNode->InsertEndChild(TiXmlElement("boneassignments"))->ToElement();
                for(; bi != biend; ++bi)
                {
                    const SkeletonVertex & assign = bi->second;
                    writeBoneAssignment(boneAssignNode, &assign);
                }
                LogManager::getSingleton().logMessage("Shared geometry bone assignments exported.");
            }
        }
		if(pMesh->getLodCount() > 1)
		{
            LogManager::getSingleton().logMessage("Exporting LOD information...");
			writeLod(rootNode, pMesh);
            LogManager::getSingleton().logMessage("LOD information exported.");
		}
		// Write poses
		writePoses(rootNode, pMesh);
		// Write animations
		writeAnimations(rootNode, pMesh);
        // Write extremes
        writeExtremes(rootNode, pMesh);
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::writeSubMesh(TiXmlElement * mSubMeshesNode, const SubMesh * s)
    {
        TiXmlElement * subMeshNode = mSubMeshesNode->InsertEndChild(TiXmlElement("submesh"))->ToElement();

        size_t numFaces;
        // Material name
        subMeshNode->SetAttribute("material", s->getMaterial());
        // bool mSelfVertex
        subMeshNode->SetAttribute("usesharedvertices",
            StrConv::conv(!s->mSelfVertex));
        // bool use32BitIndexes
		bool use32BitIndexes = ((s->getIndexData(0)->mBuffer != 0) &&
			s->getIndexData(0)->mBuffer->getUnitSize() == 32);
        subMeshNode->SetAttribute("use32bitindexes",
            StrConv::conv(use32BitIndexes));

        // Operation type
        switch(s->getGeometryType())
        {
        case GeometryRaw::OT_LINE_LIST:
            subMeshNode->SetAttribute("operationtype", "line_list");
            break;
        case GeometryRaw::OT_LINE_STRIP:
            subMeshNode->SetAttribute("operationtype", "line_strip");
            break;
        case GeometryRaw::OT_POINT_LIST:
            subMeshNode->SetAttribute("operationtype", "point_list");
            break;
        case GeometryRaw::OT_TRIANGLE_FAN:
            subMeshNode->SetAttribute("operationtype", "triangle_fan");
            break;
        case GeometryRaw::OT_TRIANGLE_LIST:
            subMeshNode->SetAttribute("operationtype", "triangle_list");
            break;
        case GeometryRaw::OT_TRIANGLE_STRIP:
            subMeshNode->SetAttribute("operationtype", "triangle_strip");
            break;
        }

        if(s->getIndexData(0)->mCount > 0)
        {
            // Faces
            TiXmlElement * facesNode = subMeshNode->InsertEndChild(TiXmlElement("faces"))->ToElement();
            switch(s->getGeometryType())
			{
			case GeometryRaw::OT_TRIANGLE_LIST:
				// tri list
				numFaces = s->getIndexData(0)->mCount / 3;

				break;
			case GeometryRaw::OT_LINE_LIST:
				numFaces = s->getIndexData(0)->mCount / 2;

				break;
			case GeometryRaw::OT_TRIANGLE_FAN:
			case GeometryRaw::OT_TRIANGLE_STRIP:
				// triangle fan or triangle strip
				numFaces = s->getIndexData(0)->mCount - 2;

				break;
			default:
				N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Unsupported render operation type");
			}
            facesNode->SetAttribute("count", StrConv::conv(numFaces));
            // Write each face in turn
            size_t i;
		    unsigned int* pInt = 0;
		    unsigned short* pShort = 0;
		    IndexBuffer * ibuf = s->getIndexData(0)->mBuffer;
		    if (use32BitIndexes)
		    {
			    pInt = static_cast<unsigned int*>(ibuf->lock(Buffer::MM_READ));
		    }
		    else
		    {
			    pShort = static_cast<unsigned short*>(ibuf->lock(Buffer::MM_READ));
		    }
            for(i = 0; i < numFaces; ++i)
            {
                TiXmlElement * faceNode =
                    facesNode->InsertEndChild(TiXmlElement("face"))->ToElement();
			    if(use32BitIndexes)
			    {
				    faceNode->SetAttribute("v1", StrConv::conv(*pInt++));
					if(s->getGeometryType() == GeometryRaw::OT_LINE_LIST)
					{
				        faceNode->SetAttribute("v2", StrConv::conv(*pInt++));
					}
                    /// Only need all 3 vertex indices if trilist or first face
					else if (s->getGeometryType() == GeometryRaw::OT_TRIANGLE_LIST || i == 0)
                    {
				        faceNode->SetAttribute("v2", StrConv::conv(*pInt++));
				        faceNode->SetAttribute("v3", StrConv::conv(*pInt++));
                    }
			    }
			    else
			    {
				    faceNode->SetAttribute("v1", StrConv::conv(*pShort++));
 					if(s->getGeometryType() == GeometryRaw::OT_LINE_LIST)
					{
				        faceNode->SetAttribute("v2", StrConv::conv(*pShort++));
					}
                    /// Only need all 3 vertex indices if trilist or first face
                    else if (s->getGeometryType() == GeometryRaw::OT_TRIANGLE_LIST || i == 0)
                    {
				        faceNode->SetAttribute("v2", StrConv::conv(*pShort++));
				        faceNode->SetAttribute("v3", StrConv::conv(*pShort++));
                    }
			    }
            }
        }

        // M_GEOMETRY chunk (Optional: present only if mSelfVertex = true)
        if(s->isSelfVertex())
        {
            TiXmlElement * geomNode = subMeshNode->InsertEndChild(TiXmlElement("geometry"))->ToElement();
            writeGeometry(geomNode, s->getVertexData());
        }

        // Bone assignments
        if(mMesh->isSkeletonFusion())
        {
            Mesh::SkeletonVertexList::iterator bi, biend = const_cast<SubMesh *>(s)->getSkeletonList().end();
            LogManager::getSingleton().logMessage("Exporting dedicated geometry bone assignments...");

            TiXmlElement * boneAssignNode = subMeshNode->InsertEndChild(TiXmlElement("boneassignments"))->ToElement();
            for(bi = const_cast<SubMesh *>(s)->getSkeletonList().begin(); bi != biend; ++bi)
            {
				const SkeletonVertex & assign = bi->second;
                writeBoneAssignment(boneAssignNode, &assign);
            }
        }
        LogManager::getSingleton().logMessage("Dedicated geometry bone assignments exported.");
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::writeGeometry(TiXmlElement * mParentNode, const VertexData * vertexData)
    {
        // Write a vertex buffer per element

        TiXmlElement *vbNode, *vertexNode, *dataNode;

        // Set num verts on parent
        mParentNode->SetAttribute("vertexcount", StrConv::conv(vertexData->mCount));

		VertexData::BindingList::const_iterator b, bend = vertexData->getAttachs().end();
		for(b = vertexData->getAttachs().begin(); b != bend; ++b)
		{
			vbNode = mParentNode->InsertEndChild(TiXmlElement("vertexbuffer"))->ToElement();
			const VertexBufferSPtr vbuf = b->second;
			unsigned short bufferIdx = b->first;
			// Get all the elements that relate to this buffer
            VertexUnitList elems = vertexData->find(bufferIdx);
            VertexUnitList::iterator i, iend;
			iend = elems.end();

			// Set up the data access for this buffer (lock read-only)
			unsigned char* pVert;
			float* pFloat;
			uint16* pShort;
			uint8* pChar;
			ARGB* pColour;

			pVert = static_cast<unsigned char*>(vbuf->lock(Buffer::MM_READ));

            // Skim over the elements to set up the general data
            unsigned short numTextureCoords = 0;
			for (i = elems.begin(); i != iend; ++i)
			{
				VertexUnit & elem = *i;
				switch(elem.mVType)
				{
				case VT_Pos:
					vbNode->SetAttribute("positions","true");
                    break;
				case VT_Normals:
					vbNode->SetAttribute("normals","true");
                    break;
				case VT_Tangent:
					vbNode->SetAttribute("tangents","true");
					if (elem.mType == DT_Float4x)
					{
						vbNode->SetAttribute("tangent_dimensions", "4");
					}
					break;
				case VT_Binormal:
					vbNode->SetAttribute("binormals","true");
					break;
				case VT_Diffuse:
					vbNode->SetAttribute("colours_diffuse","true");
                    break;
				case VT_Specular:
					vbNode->SetAttribute("colours_specular","true");
                    break;
                case VT_Tex_Coord:
					{
						const char *type = "float2";
						switch (elem.mType)
						{
						case DT_Float1x:
							type = "float1";
							break;
						case DT_Float2x:
							type = "float2";
							break;
						case DT_Float3x:
							type = "float3";
							break;
						case DT_Float4x:
							type = "float4";
							break;
						case DT_Colour:
						case DT_Colour_ARGB:
						case DT_Colour_ABGR:
							type = "colour";
							break;
						case DT_Short1x:
							type = "short1";
							break;
						case DT_Short2x:
							type = "short2";
							break;
						case DT_Short3x:
							type = "short3";
							break;
						case DT_Short4x:
							type = "short4";
							break;
						case DT_U_Byte:
							type = "ubyte4";
							break;
						}
						vbNode->SetAttribute(
							"texture_coord_dimensions_" + StrConv::conv(numTextureCoords), type);
						++numTextureCoords;
					}
					break;

                default:
                    break;
                }
            }
            if (numTextureCoords > 0)
            {
                vbNode->SetAttribute("texture_coords",
                    StrConv::conv(numTextureCoords));
            }

			// For each vertex
			for(size_t v = 0; v < vertexData->mCount; ++v)
			{
                vertexNode = vbNode->InsertEndChild(TiXmlElement("vertex"))->ToElement();
				// Iterate over the elements
				for(i = elems.begin(); i != iend; ++i)
				{
					VertexUnit & elem = *i;
					switch(elem.mVType)
					{
					case VT_Pos:
						elem.getData(pVert, pFloat);
						dataNode =
							vertexNode->InsertEndChild(TiXmlElement("position"))->ToElement();
						dataNode->SetAttribute("x", StrConv::conv(pFloat[0]));
						dataNode->SetAttribute("y", StrConv::conv(pFloat[1]));
						dataNode->SetAttribute("z", StrConv::conv(pFloat[2]));
						break;
					case VT_Normals:
						elem.getData(pVert, pFloat);
						dataNode =
							vertexNode->InsertEndChild(TiXmlElement("normal"))->ToElement();
						dataNode->SetAttribute("x", StrConv::conv(pFloat[0]));
						dataNode->SetAttribute("y", StrConv::conv(pFloat[1]));
						dataNode->SetAttribute("z", StrConv::conv(pFloat[2]));
						break;
					case VT_Tangent:
						elem.getData(pVert, pFloat);
						dataNode =
							vertexNode->InsertEndChild(TiXmlElement("tangent"))->ToElement();
						dataNode->SetAttribute("x", StrConv::conv(pFloat[0]));
						dataNode->SetAttribute("y", StrConv::conv(pFloat[1]));
						dataNode->SetAttribute("z", StrConv::conv(pFloat[2]));
						if (elem.mType == DT_Float4x)
						{
							dataNode->SetAttribute("w", StrConv::conv(pFloat[3]));
						}
						break;
					case VT_Binormal:
						elem.getData(pVert, pFloat);
						dataNode =
							vertexNode->InsertEndChild(TiXmlElement("binormal"))->ToElement();
						dataNode->SetAttribute("x", StrConv::conv(pFloat[0]));
						dataNode->SetAttribute("y", StrConv::conv(pFloat[1]));
						dataNode->SetAttribute("z", StrConv::conv(pFloat[2]));
						break;
					case VT_Diffuse:
						elem.getData(pVert, pColour);
						dataNode =
							vertexNode->InsertEndChild(TiXmlElement("colour_diffuse"))->ToElement();
						{
							ARGB rc = *pColour++;
							Colour cv;
							cv.b = (rc & 0xFF) / 255.0f;		rc >>= 8;
							cv.g = (rc & 0xFF) / 255.0f;		rc >>= 8;
							cv.r = (rc & 0xFF) / 255.0f;		rc >>= 8;
							cv.a = (rc & 0xFF) / 255.0f;
                            dataNode->SetAttribute("value", StrConv::conv(cv));
						}
						break;
					case VT_Specular:
						elem.getData(pVert, pColour);
						dataNode =
							vertexNode->InsertEndChild(TiXmlElement("colour_specular"))->ToElement();
						{
							ARGB rc = *pColour++;
							Colour cv;
							cv.b = (rc & 0xFF) / 255.0f;		rc >>= 8;
							cv.g = (rc & 0xFF) / 255.0f;		rc >>= 8;
							cv.r = (rc & 0xFF) / 255.0f;		rc >>= 8;
							cv.a = (rc & 0xFF) / 255.0f;
							dataNode->SetAttribute("value", StrConv::conv(cv));
						}
						break;
					case VT_Tex_Coord:
						dataNode = vertexNode->InsertEndChild(TiXmlElement("texcoord"))->ToElement();

						switch(elem.mType)
                        {
                        case DT_Float1x:
                            elem.getData(pVert, pFloat);
    						dataNode->SetAttribute("u", StrConv::conv(*pFloat++));
                            break;
                        case DT_Float2x:
                            elem.getData(pVert, pFloat);
    						dataNode->SetAttribute("u", StrConv::conv(*pFloat++));
    						dataNode->SetAttribute("v", StrConv::conv(*pFloat++));
                            break;
                        case DT_Float3x:
                            elem.getData(pVert, pFloat);
    						dataNode->SetAttribute("u", StrConv::conv(*pFloat++));
    						dataNode->SetAttribute("v", StrConv::conv(*pFloat++));
    						dataNode->SetAttribute("w", StrConv::conv(*pFloat++));
                            break;
                        case DT_Float4x:
                            elem.getData(pVert, pFloat);
    						dataNode->SetAttribute("u", StrConv::conv(*pFloat++));
    						dataNode->SetAttribute("v", StrConv::conv(*pFloat++));
    						dataNode->SetAttribute("w", StrConv::conv(*pFloat++));
    						dataNode->SetAttribute("x", StrConv::conv(*pFloat++));
                            break;
                        case DT_Short1x:
                            elem.getData(pVert, pShort);
    						dataNode->SetAttribute("u", StrConv::conv(*pShort++ / 65535.0f));
                            break;
                        case DT_Short2x:
                            elem.getData(pVert, pShort);
    						dataNode->SetAttribute("u", StrConv::conv(*pShort++ / 65535.0f));
    						dataNode->SetAttribute("v", StrConv::conv(*pShort++ / 65535.0f));
                            break;
                        case DT_Short3x:
                            elem.getData(pVert, pShort);
    						dataNode->SetAttribute("u", StrConv::conv(*pShort++ / 65535.0f));
    						dataNode->SetAttribute("v", StrConv::conv(*pShort++ / 65535.0f));
    						dataNode->SetAttribute("w", StrConv::conv(*pShort++ / 65535.0f));
                            break;
                        case DT_Short4x:
                            elem.getData(pVert, pShort);
    						dataNode->SetAttribute("u", StrConv::conv(*pShort++ / 65535.0f));
    						dataNode->SetAttribute("v", StrConv::conv(*pShort++ / 65535.0f));
    						dataNode->SetAttribute("w", StrConv::conv(*pShort++ / 65535.0f));
    						dataNode->SetAttribute("x", StrConv::conv(*pShort++ / 65535.0f));
                            break;
                        case DT_Colour: case DT_Colour_ARGB: case DT_Colour_ABGR:
                            elem.getData(pVert, pColour);
                            {
                                ARGB rc = *pColour++;
                                Colour cv;
                                cv.b = (rc & 0xFF) / 255.0f;		rc >>= 8;
                                cv.g = (rc & 0xFF) / 255.0f;		rc >>= 8;
                                cv.r = (rc & 0xFF) / 255.0f;		rc >>= 8;
                                cv.a = (rc & 0xFF) / 255.0f;
                                dataNode->SetAttribute("u", StrConv::conv(cv));
                            }
                            break;
                        case DT_U_Byte:
                            elem.getData(pVert, pChar);
    						dataNode->SetAttribute("u", StrConv::conv(*pChar++ / 255.0f));
    						dataNode->SetAttribute("v", StrConv::conv(*pChar++ / 255.0f));
    						dataNode->SetAttribute("w", StrConv::conv(*pChar++ / 255.0f));
    						dataNode->SetAttribute("x", StrConv::conv(*pChar++ / 255.0f));
                            break;
                        }
						break;
                    default:
                        break;

					}
				}
				pVert += vbuf->getUnitSize();
			}
			vbuf->unlock();
		}

    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::writeSkeletonLink(TiXmlElement* mMeshNode, const String& skelName)
    {
        TiXmlElement * skelNode = mMeshNode->InsertEndChild(TiXmlElement("skeletonlink"))->ToElement();
        skelNode->SetAttribute("name", skelName);
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::writeBoneAssignment(TiXmlElement * mBoneAssignNode,
        const SkeletonVertex * assign)
    {
        TiXmlElement * assignNode = mBoneAssignNode->InsertEndChild(TiXmlElement("vertexboneassignment"))->ToElement();

        assignNode->SetAttribute("vertexindex", StrConv::conv(assign->mVertexIndex));
        assignNode->SetAttribute("boneindex", StrConv::conv(assign->mBoneIndex));
        assignNode->SetAttribute("weight", StrConv::conv(assign->mWeight));
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::readSubMeshes(TiXmlElement * mSubmeshesNode)
    {
        LogManager::getSingleton().logMessage("Reading submeshes...");

        for (TiXmlElement * smElem = mSubmeshesNode->FirstChildElement();
            smElem != 0; smElem = smElem->NextSiblingElement())
        {
            // All children should be submeshes
            SubMesh * sm = mMesh->createSub();

            const char * mat = smElem->Attribute("material");
            if (mat)
                sm->setMaterial(mat);

            // Read operation type
            bool readFaces = true;
            const char* optype = smElem->Attribute("operationtype");
            if (optype)
            {
                if (!strcmp(optype, "triangle_list"))
                {
                    sm->setGeometryType(GeometryRaw::OT_TRIANGLE_LIST);
                }
                else if (!strcmp(optype, "triangle_fan"))
                {
                    sm->setGeometryType(GeometryRaw::OT_TRIANGLE_FAN);
                }
                else if (!strcmp(optype, "triangle_strip"))
                {
                    sm->setGeometryType(GeometryRaw::OT_TRIANGLE_STRIP);
                }
                else if (!strcmp(optype, "line_strip"))
                {
                    sm->setGeometryType(GeometryRaw::OT_LINE_STRIP);
                    readFaces = false;
                }
                else if (!strcmp(optype, "line_list"))
                {
                    sm->setGeometryType(GeometryRaw::OT_LINE_LIST);
                    //readFaces = false;
                }
                else if (!strcmp(optype, "point_list"))
                {
                    sm->setGeometryType(GeometryRaw::OT_POINT_LIST);
                    readFaces = false;
                }

            }

            const char* tmp = smElem->Attribute("usesharedvertices");
            if (tmp)
                sm->setSelfVertex(!StringConverter::parseBool(tmp));
            tmp = smElem->Attribute("use32bitindexes");
            bool use32BitIndexes = false;
            if (tmp)
                use32BitIndexes = StringConverter::parseBool(tmp);

            // Faces
            if (readFaces)
            {
                TiXmlElement* faces = smElem->FirstChildElement("faces");
                int actualCount = 0;
                for (TiXmlElement *faceElem = faces->FirstChildElement(); faceElem != 0; faceElem = faceElem->NextSiblingElement())
                {
                        actualCount++;
                }
                const char *claimedCount_ = faces->Attribute("count");
                if (claimedCount_ && StringConverter::parseInt(claimedCount_)!=actualCount)
                {
				    LogManager::getSingleton().stream()
					    << "WARNING: face count (" << actualCount << ") " <<
					    "is not as claimed (" << claimedCount_ << ")";
                }


                if (actualCount > 0)
				{
					// Faces
					switch(sm->getGeometryType())
					{
					case GeometryRaw::OT_TRIANGLE_LIST:
						// tri list
						sm->getIndexData(0)->mCount = actualCount * 3;

						break;
					case GeometryRaw::OT_LINE_LIST:
						sm->getIndexData(0)->mCount = actualCount * 2;

						break;
					case GeometryRaw::OT_TRIANGLE_FAN:
					case GeometryRaw::OT_TRIANGLE_STRIP:
						// triangle fan or triangle strip
						sm->getIndexData(0)->mCount = actualCount + 2;

						break;
					default:
						N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "OpType not implemented",
							__FUNCTION__);
					}

					// Allocate space
					IndexBuffer * ibuf;
                    N_Only(GBuffer).create(ibuf, use32BitIndexes ? 32 : 16, sm->getIndexData(0)->mCount, Buffer::M_NORMAL );
					sm->getIndexData(0)->mBuffer = ibuf;
					unsigned int *pInt = 0;
					unsigned short *pShort = 0;
					if (use32BitIndexes)
					{
						pInt = static_cast<unsigned int*>(ibuf->lock(Buffer::MM_WHOLE));
					}
					else
					{
						pShort = static_cast<unsigned short*>(ibuf->lock(Buffer::MM_WHOLE));
					}
					TiXmlElement* faceElem;
					bool firstTri = true;
					for (faceElem = faces->FirstChildElement();
						faceElem != 0; faceElem = faceElem->NextSiblingElement())
					{
						if (use32BitIndexes)
						{
							*pInt++ = StringConverter::parseInt(faceElem->Attribute("v1"));
							if(sm->getGeometryType() == GeometryRaw::OT_LINE_LIST)
							{
								*pInt++ = StringConverter::parseInt(faceElem->Attribute("v2"));
							}
							// only need all 3 vertices if it's a trilist or first tri
							else if (sm->getGeometryType() == GeometryRaw::OT_TRIANGLE_LIST || firstTri)
							{
								*pInt++ = StringConverter::parseInt(faceElem->Attribute("v2"));
								*pInt++ = StringConverter::parseInt(faceElem->Attribute("v3"));
							}
						}
						else
						{
							*pShort++ = StringConverter::parseInt(faceElem->Attribute("v1"));
							if(sm->getGeometryType() == GeometryRaw::OT_LINE_LIST)
							{
								*pShort++ = StringConverter::parseInt(faceElem->Attribute("v2"));
							}
							// only need all 3 vertices if it's a trilist or first tri
							else if (sm->getGeometryType() == GeometryRaw::OT_TRIANGLE_LIST || firstTri)
							{
								*pShort++ = StringConverter::parseInt(faceElem->Attribute("v2"));
								*pShort++ = StringConverter::parseInt(faceElem->Attribute("v3"));
							}
						}
						firstTri = false;
					}
					ibuf->unlock();
				}
            }

            // Geometry
            if(sm->isSelfVertex())
            {
                TiXmlElement * geomNode = smElem->FirstChildElement("geometry");
                if(geomNode)
                {
                    VertexData * smvd;
                    N_Only(GBuffer).create(smvd);
                    sm->setVertexData(smvd);
                    readGeometry(geomNode, smvd);
                }
            }

            // texture aliases
            TiXmlElement * textureAliasesNode = smElem->FirstChildElement("textures");

            // Bone assignments
            TiXmlElement * boneAssigns = smElem->FirstChildElement("boneassignments");
            if(boneAssigns)
                readBoneAssignments(boneAssigns, sm);

        }
        LogManager::getSingleton().logMessage("Submeshes done.");
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::readGeometry(TiXmlElement* mGeometryNode, VertexData* vertexData)
    {
        LogManager::getSingleton().logMessage("Reading geometry...");
        unsigned char *pVert;
        float *pFloat;
        uint16 *pShort;
        uint8 *pChar;
        ARGB *pCol;

        const char *claimedVertexCount_ = mGeometryNode->Attribute("vertexcount");
        ptrdiff_t claimedVertexCount = 0;
        if (claimedVertexCount_)
        {
                claimedVertexCount =
                        StringConverter::parseInt(claimedVertexCount_);
        }
        // Skip empty
        if (claimedVertexCount_ && claimedVertexCount <= 0) return;

        unsigned short bufCount = 0;
        unsigned short totalTexCoords = 0; // across all buffers

        // Information for calculating bounds
        Vector3 min = Vector3::ZERO, max = Vector3::UNIT, pos = Vector3::ZERO;
        NIIf maxSquaredRadius = -1;
        bool first = true;

        // Iterate over all children (vertexbuffer entries)
        for (TiXmlElement* vbElem = mGeometryNode->FirstChildElement();
            vbElem != 0; vbElem = vbElem->NextSiblingElement())
        {
            size_t offset = 0;
            // Skip non-vertexbuffer elems
            if (stricmp(vbElem->Value(), "vertexbuffer")) continue;

            const char* attrib = vbElem->Attribute("positions");
            if (attrib && StringConverter::parseBool(attrib))
            {
                // Add element
                vertexData->add(DT_Float3x, VT_Pos, bufCount, offset);
                offset += VertexUnit::getTypeSize(DT_Float3x);
            }
            attrib = vbElem->Attribute("normals");
            if (attrib && StringConverter::parseBool(attrib))
            {
                // Add element
                vertexData->add(DT_Float3x, VT_Normals, bufCount, offset);
                offset += VertexUnit::getTypeSize(DT_Float3x);
            }
			attrib = vbElem->Attribute("tangents");
			if(attrib && StringConverter::parseBool(attrib))
			{
				DataType tangentType = DT_Float3x;
				attrib = vbElem->Attribute("tangent_dimensions");
				if(attrib)
				{
					unsigned int dims = StringConverter::parseUnsignedInt(attrib);
					if(dims == 4)
						tangentType = DT_Float4x;
				}

				// Add element
				vertexData->add(tangentType, VT_Tangent, bufCount, offset);
				offset += VertexUnit::getTypeSize(tangentType);
			}
			attrib = vbElem->Attribute("binormals");
			if (attrib && StringConverter::parseBool(attrib))
			{
				// Add element
				vertexData->add(DT_Float3x, VT_Binormal, bufCount, offset);
				offset += VertexUnit::getTypeSize(DT_Float3x);
			}
            attrib = vbElem->Attribute("colours_diffuse");
            if (attrib && StringConverter::parseBool(attrib))
            {
                // Add element
                vertexData->add(mColourElementType, VT_Diffuse, bufCount, offset);
                offset += VertexUnit::getTypeSize(mColourElementType);
            }
            attrib = vbElem->Attribute("colours_specular");
            if (attrib && StringConverter::parseBool(attrib))
            {
                // Add element
                vertexData->add(mColourElementType, VT_Specular, bufCount, offset);
                offset += VertexUnit::getTypeSize(mColourElementType);
            }
            attrib = vbElem->Attribute("texture_coords");
            if (attrib && StringConverter::parseInt(attrib))
            {
                unsigned short numTexCoords = StringConverter::parseInt(vbElem->Attribute("texture_coords"));
                for (unsigned short tx = 0; tx < numTexCoords; ++tx)
                {
                    // NB set is local to this buffer, but will be translated into a
                    // global set number across all vertex buffers
					StringUtil::StrStreamType str;
					str << "texture_coord_dimensions_" << tx;
                    attrib = vbElem->Attribute(str.str().c_str());
                    DataType vtype = DT_Float2x; // Default
					if (attrib)
					{
						if (!::strcmp(attrib,"1"))
							vtype = DT_Float1x;
						else if (!::strcmp(attrib,"2"))
							vtype = DT_Float2x;
						else if (!::strcmp(attrib,"3"))
							vtype = DT_Float3x;
						else if (!::strcmp(attrib,"4"))
							vtype = DT_Float4x;
						else if (!::strcmp(attrib,"float1"))
							vtype = DT_Float1x;
						else if (!::strcmp(attrib,"float2"))
							vtype = DT_Float2x;
						else if (!::strcmp(attrib,"float3"))
							vtype = DT_Float3x;
						else if (!::strcmp(attrib,"float4"))
							vtype = DT_Float4x;
						else if (!::strcmp(attrib,"short1"))
							vtype = DT_Short1x;
						else if (!::strcmp(attrib,"short2"))
							vtype = DT_Short2x;
						else if (!::strcmp(attrib,"short3"))
							vtype = DT_Short3x;
						else if (!::strcmp(attrib,"short4"))
							vtype = DT_Short4x;
						else if (!::strcmp(attrib,"ubyte4"))
							vtype = DT_U_Byte;
						else if (!::strcmp(attrib,"colour"))
							vtype = DT_Colour;
						else if (!::strcmp(attrib,"colour_argb"))
							vtype = DT_Colour_ARGB;
						else if (!::strcmp(attrib,"colour_abgr"))
							vtype = DT_Colour_ABGR;
						else
						{
							std::cerr << "ERROR: Did not recognise texture_coord_dimensions value of \""<<attrib<<"\"" << std::endl;
							std::cerr << "Falling back to default of DT_Float2x" << std::endl;
						}
					}
					// Add element
					vertexData->add(vtype, VT_Tex_Coord, bufCount, offset, totalTexCoords++);
					offset += VertexUnit::getTypeSize(vtype);
                }
            }

            // calculate how many vertexes there actually are
            int actualVertexCount = 0;
            for (TiXmlElement * vertexElem = vbElem->FirstChildElement(); vertexElem != 0; vertexElem = vertexElem->NextSiblingElement())
            {
                    actualVertexCount++;
            }
            if (claimedVertexCount_ && actualVertexCount!=claimedVertexCount)
            {
				LogManager::getSingleton().stream()
					<< "WARNING: vertex count (" << actualVertexCount
					<< ") is not as claimed (" << claimedVertexCount_ << ")";
            }

            vertexData->mCount = actualVertexCount;
            // Now create the vertex buffer
            VertexBuffer * vbuf;
            N_Only(GBuffer).create(vbuf, offset, vertexData->mCount, Buffer::Buffer::M_NORMAL );
            vertexData->attach(bufCount, vbuf);
            // Lock it
            pVert = static_cast<unsigned char*>(vbuf->lock(Buffer::MM_WHOLE));

            // Get the element list for this buffer alone
            VertexUnitList elems = vertexData->find(bufCount);
            // Now the buffer is set up, parse all the vertices
            for (TiXmlElement* vertexElem = vbElem->FirstChildElement();
            vertexElem != 0; vertexElem = vertexElem->NextSiblingElement())
            {
                // Now parse the elements, ensure they are all matched
                VertexUnitList::const_iterator ielem, ielemend = elems.end();
                TiXmlElement * xmlElem;
                TiXmlElement * texCoordElem = 0;
                for (ielem = elems.begin(); ielem != ielemend; ++ielem)
                {
                    const VertexUnit & elem = *ielem;
                    // Find child for this element
                    switch(elem.mVType)
                    {
                    case VT_Pos:
                        xmlElem = vertexElem->FirstChildElement("position");
                        if (!xmlElem)
                        {
                            N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Missing <position> element.");
                        }
                        elem.getData(pVert, pFloat);

                        *pFloat++ = StringConverter::parseReal(xmlElem->Attribute("x"));
                        *pFloat++ = StringConverter::parseReal(xmlElem->Attribute("y"));
                        *pFloat++ = StringConverter::parseReal(xmlElem->Attribute("z"));

                        pos.x = StringConverter::parseReal(xmlElem->Attribute("x"));
                        pos.y = StringConverter::parseReal(xmlElem->Attribute("y"));
                        pos.z = StringConverter::parseReal(xmlElem->Attribute("z"));

                        if (first)
                        {
                            min = max = pos;
                            maxSquaredRadius = pos.lengthSquared();
                            first = false;
                        }
                        else
                        {
                            min = min.floor(pos);
                            max = max.ceil(pos);
                            maxSquaredRadius = std::max(pos.lengthSquared(), maxSquaredRadius);
                        }
                        break;
                    case VT_Normals:
                        xmlElem = vertexElem->FirstChildElement("normal");
                        if (!xmlElem)
                        {
                            N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Missing <normal> element.");
                        }
                        elem.getData(pVert, pFloat);

                        *pFloat++ = StringConverter::parseReal(xmlElem->Attribute("x"));
                        *pFloat++ = StringConverter::parseReal(xmlElem->Attribute("y"));
                        *pFloat++ = StringConverter::parseReal(xmlElem->Attribute("z"));
                        break;
					case VT_Tangent:
						xmlElem = vertexElem->FirstChildElement("tangent");
						if(!xmlElem)
						{
							N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Missing <tangent> element.");
						}
						elem.getData(pVert, pFloat);

						*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("x"));
						*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("y"));
						*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("z"));
						if(elem.mType == DT_Float4x)
						{
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("w"));
						}
						break;
					case VT_Binormal:
						xmlElem = vertexElem->FirstChildElement("binormal");
						if(!xmlElem)
						{
							N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Missing <binormal> element.");
						}
						elem.getData(pVert, pFloat);

						*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("x"));
						*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("y"));
						*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("z"));
						break;
                    case VT_Diffuse:
                        xmlElem = vertexElem->FirstChildElement("colour_diffuse");
                        if(!xmlElem)
                        {
                            N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Missing <colour_diffuse> element.");
                        }
                        elem.getData(pVert, pCol);
						{
							Colour cv;
							cv = StringConverter::parseColourValue(xmlElem->Attribute("value"));
							*pCol++ = Engine::getColourValue(cv, mColourElementType);
						}
                        break;
                    case VT_Specular:
                        xmlElem = vertexElem->FirstChildElement("colour_specular");
                        if (!xmlElem)
                        {
                            N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Missing <colour_specular> element.");
                        }
                        elem.getData(pVert, pCol);
						{
							Colour cv;
							cv = StringConverter::parseColourValue(xmlElem->Attribute("value"));
							*pCol++ = Engine::getColourValue(cv, mColourElementType);
						}
                        break;
                    case VT_Tex_Coord:
                        if (!texCoordElem)
                        {
                            // Get first texcoord
                            xmlElem = vertexElem->FirstChildElement("texcoord");
                        }
                        else
                        {
                            // Get next texcoord
                            xmlElem = texCoordElem->NextSiblingElement("texcoord");
                        }
                        if (!xmlElem)
                        {
                            N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Missing <texcoord> element.",
                                "XMLSerializer::readGeometry");
                        }
						// Record the latest texture coord entry
						texCoordElem = xmlElem;

                        if (!xmlElem->Attribute("u"))
                            N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'u' attribute not found.", "XMLMeshSerializer::readGeometry");

                        // depending on type, pack appropriately, can process colour channels separately which is a bonus
						switch(elem.mType)
						{
						case DT_Float1x:
							elem.getData(pVert, pFloat);
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("u"));
							break;

						case DT_Float2x:
							if(!xmlElem->Attribute("v"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'v' attribute not found.", "XMLMeshSerializer::readGeometry");
							elem.getData(pVert, pFloat);
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("u"));
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("v"));
							break;

						case DT_Float3x:
							if (!xmlElem->Attribute("v"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'v' attribute not found.", "XMLMeshSerializer::readGeometry");
							if (!xmlElem->Attribute("w"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'w' attribute not found.", "XMLMeshSerializer::readGeometry");
							elem.getData(pVert, pFloat);
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("u"));
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("v"));
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("w"));
							break;

						case DT_Float4x:
							if (!xmlElem->Attribute("v"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'v' attribute not found.", "XMLMeshSerializer::readGeometry");
							if (!xmlElem->Attribute("w"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'w' attribute not found.", "XMLMeshSerializer::readGeometry");
							if (!xmlElem->Attribute("x"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'x' attribute not found.", "XMLMeshSerializer::readGeometry");
							elem.getData(pVert, pFloat);
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("u"));
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("v"));
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("w"));
							*pFloat++ = StringConverter::parseReal(xmlElem->Attribute("x"));
							break;

						case DT_Short1x:
							elem.getData(pVert, pShort);
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("u")));
							break;

						case DT_Short2x:
							if (!xmlElem->Attribute("v"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'v' attribute not found.", "XMLMeshSerializer::readGeometry");
							elem.getData(pVert, pShort);
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("u")));
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("v")));
							break;

						case DT_Short3x:
							if (!xmlElem->Attribute("v"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'v' attribute not found.", "XMLMeshSerializer::readGeometry");
							if (!xmlElem->Attribute("w"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'w' attribute not found.", "XMLMeshSerializer::readGeometry");
							elem.getData(pVert, pShort);
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("u")));
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("v")));
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("w")));
							break;

						case DT_Short4x:
							if (!xmlElem->Attribute("v"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'v' attribute not found.", "XMLMeshSerializer::readGeometry");
							if (!xmlElem->Attribute("w"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'w' attribute not found.", "XMLMeshSerializer::readGeometry");
							if (!xmlElem->Attribute("x"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'x' attribute not found.", "XMLMeshSerializer::readGeometry");
							elem.getData(pVert, pShort);
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("u")));
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("v")));
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("w")));
							*pShort++ = static_cast<uint16>(65535.0f * StringConverter::parseReal(xmlElem->Attribute("x")));
							break;

						case DT_U_Byte:
							if (!xmlElem->Attribute("v"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'v' attribute not found.", "XMLMeshSerializer::readGeometry");
							if (!xmlElem->Attribute("w"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'w' attribute not found.", "XMLMeshSerializer::readGeometry");
							if (!xmlElem->Attribute("x"))
								N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Texcoord 'x' attribute not found.", "XMLMeshSerializer::readGeometry");
							elem.getData(pVert, pChar);
							// round off instead of just truncating -- avoids magnifying rounding errors
							*pChar++ = static_cast<uint8>(0.5f + 255.0f * StringConverter::parseReal(xmlElem->Attribute("u")));
							*pChar++ = static_cast<uint8>(0.5f + 255.0f * StringConverter::parseReal(xmlElem->Attribute("v")));
							*pChar++ = static_cast<uint8>(0.5f + 255.0f * StringConverter::parseReal(xmlElem->Attribute("w")));
							*pChar++ = static_cast<uint8>(0.5f + 255.0f * StringConverter::parseReal(xmlElem->Attribute("x")));
							break;

						case DT_Colour:
							{
								elem.getData(pVert, pCol);
								Colour cv = StringConverter::parseColourValue(xmlElem->Attribute("u"));
								*pCol++ = Engine::getColourValue(cv, mColourElementType);
							}
							break;

						case DT_Colour_ARGB:
						case DT_Colour_ABGR:
							{
								elem.getData(pVert, pCol);
								Colour cv = StringConverter::parseColourValue(xmlElem->Attribute("u"));
								*pCol++ = Engine::getColourValue(cv, elem.mType);
							}
							break;
						}
                        break;
                    default:
                        break;
                    }
                } // semantic
                pVert += vbuf->getUnitSize();
            } // vertex
            bufCount++;
            vbuf->unlock();
        } // vertexbuffer

        // Set bounds
        const AABox & currBox = mMesh->getAABB();
        NIIf currRadius = mMesh->getCenterRange();
        if(currBox.isNull())
        {
            //do not pad the bounding box
            mMesh->setAABB(AABox(min, max));
            mMesh->setCenterRange(Math::sqrt(maxSquaredRadius));
        }
        else
        {
            AABox newBox(min, max);
            newBox.merge(currBox);
	    //do not pad the bounding box
            mMesh->setAABB(newBox);
            mMesh->setCenterRange(std::max(Math::sqrt(maxSquaredRadius), currRadius));
        }

        LogManager::getSingleton().logMessage("Geometry done...");
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::readSkeletonLink(TiXmlElement * mSkelNode)
    {
        mMesh->setSkeletonID(mSkelNode->Attribute("name"));
    }
    //---------------------------------------------------------------------
    void XMLMeshSerializer::readBoneAssignments(TiXmlElement * mBoneAssignmentsNode)
    {
        LogManager::getSingleton().logMessage("Reading bone assignments...");

        // Iterate over all children (vertexboneassignment entries)
        for(TiXmlElement * elem = mBoneAssignmentsNode->FirstChildElement();
        elem != 0; elem = elem->NextSiblingElement())
        {
            SkeletonVertex vba;
            vba.mVertexIndex = StringConverter::parseInt(elem->Attribute("vertexindex"));
            vba.mBoneIndex = StringConverter::parseInt(elem->Attribute("boneindex"));
            vba.mWeight= StringConverter::parseReal(elem->Attribute("weight"));

            mMesh->addSkeleton(vba);
        }

        LogManager::getSingleton().logMessage("Bone assignments done.");
    }
    //---------------------------------------------------------------------
	void XMLMeshSerializer::readSubMeshNames(TiXmlElement * mMeshNamesNode, Mesh * sm)
	{
		LogManager::getSingleton().logMessage("Reading mesh names...");

		// Iterate over all children (vertexboneassignment entries)
		for(TiXmlElement * elem = mMeshNamesNode->FirstChildElement();
			elem != 0; elem = elem->NextSiblingElement())
		{
			String meshName = elem->Attribute("name");
			int index = StringConverter::parseInt(elem->Attribute("index"));

			sm->getSub(index)->setName(meshName);
		}

		LogManager::getSingleton().logMessage("Mesh names done.");
	}
    //---------------------------------------------------------------------
    void XMLMeshSerializer::readBoneAssignments(TiXmlElement* mBoneAssignmentsNode, SubMesh* sm)
    {
        LogManager::getSingleton().logMessage("Reading bone assignments...");
        // Iterate over all children (vertexboneassignment entries)
        for (TiXmlElement* elem = mBoneAssignmentsNode->FirstChildElement();
        elem != 0; elem = elem->NextSiblingElement())
        {
            SkeletonVertex vba;
            vba.mVertexIndex = StringConverter::parseInt(elem->Attribute("vertexindex"));
            vba.mBoneIndex = StringConverter::parseInt(elem->Attribute("boneindex"));
            vba.mWeight= StringConverter::parseReal(elem->Attribute("weight"));

            sm->addSkeleton(vba);
        }
        LogManager::getSingleton().logMessage("Bone assignments done.");
    }
    //---------------------------------------------------------------------
	void XMLMeshSerializer::writeLod(TiXmlElement* mMeshNode, const Mesh* pMesh)
	{
        TiXmlElement * lodNode =
            mMeshNode->InsertEndChild(TiXmlElement("levelofdetail"))->ToElement();

        MeshLodScheme * strategy = pMesh->getLodScheme();
		unsigned short numLvls = pMesh->getLodCount();
		bool manual = !strategy->isRelation();
        lodNode->SetAttribute("strategy", strategy->getName());
		lodNode->SetAttribute("numlevels", StrConv::conv(numLvls));
		lodNode->SetAttribute("manual", StrConv::conv(manual));

		// Iterate from level 1, not 0 (full detail)
		for(Nindex i = 1; i < numLvls; ++i)
		{
			const MeshLodScheme::MeshLod & usage = pMesh->getLodLevel(i); // 需要改变
			if (manual)
			{
				writeLodUsageManual(lodNode, i, usage);
			}
			else
			{
				writeLodUsageGenerated(lodNode, i, usage, pMesh);
			}
		}
	}
    //---------------------------------------------------------------------
	void XMLMeshSerializer::writeLodUsageManual(TiXmlElement * usageNode,
		unsigned short levelNum, const MeshLodScheme::MeshLod & usage)
	{
		TiXmlElement * manualNode =
			usageNode->InsertEndChild(TiXmlElement("lodmanual"))->ToElement();

		manualNode->SetAttribute("value", StrConv::conv(usage.userValue));
		manualNode->SetAttribute("meshname", usage.manualName);
	}
    //---------------------------------------------------------------------
	void XMLMeshSerializer::writeLodUsageGenerated(TiXmlElement* usageNode,
		unsigned short levelNum,  const MeshLodScheme::MeshLod & usage,
            const Mesh * pMesh)
	{
		TiXmlElement* generatedNode =
			usageNode->InsertEndChild(TiXmlElement("lodgenerated"))->ToElement();
		generatedNode->SetAttribute("value",
			StrConv::conv(usage.userValue));

		// Iterate over submeshes at this level
		unsigned short numsubs = pMesh->getSubCount();

		for(unsigned short subi = 0; subi < numsubs; ++subi)
		{
			TiXmlElement * subNode =
				generatedNode->InsertEndChild(TiXmlElement("lodfacelist"))->ToElement();
			SubMesh * sub = pMesh->getSub(subi);
			subNode->SetAttribute("submeshindex", StrConv::conv(subi));
			// NB level - 1 because SubMeshes don't store the first index in geometry
		    IndexData * facedata = sub->getIndexData(levelNum);
			subNode->SetAttribute("numfaces", StrConv::conv(facedata->mCount / 3));

			if (facedata->mCount > 0)
			{
				// Write each face in turn
				bool use32BitIndexes = (facedata->mBuffer->getUnitSize() == 32);

				// Write each face in turn
				unsigned int* pInt = 0;
				unsigned short* pShort = 0;
				IndexBufferSPtr ibuf = facedata->mBuffer;
				if (use32BitIndexes)
				{
					pInt = static_cast<unsigned int*>(ibuf->lock(Buffer::MM_READ));
				}
				else
				{
					pShort = static_cast<unsigned short*>(ibuf->lock(Buffer::MM_READ));
				}

				for (size_t f = 0; f < facedata->mCount; f += 3)
				{
					TiXmlElement* faceNode =
						subNode->InsertEndChild(TiXmlElement("face"))->ToElement();
					if (use32BitIndexes)
					{
						faceNode->SetAttribute("v1", StrConv::conv(*pInt++));
						faceNode->SetAttribute("v2", StrConv::conv(*pInt++));
						faceNode->SetAttribute("v3", StrConv::conv(*pInt++));
					}
					else
					{
						faceNode->SetAttribute("v1", StrConv::conv(*pShort++));
						faceNode->SetAttribute("v2", StrConv::conv(*pShort++));
						faceNode->SetAttribute("v3", StrConv::conv(*pShort++));
					}

				}
			}
		}
	}
    //---------------------------------------------------------------------
	void XMLMeshSerializer::writeExtremes(TiXmlElement * mMeshNode, const Mesh * m)
	{
        int idx = 0;
		TiXmlElement * extremesNode = NULL;
        SubMeshList::iterator i, iend = ((Mesh &)*m).getSubList().end();
		for(i = ((Mesh &)*m).getSubList().end(); i != iend; ++i, ++idx)
		{
			SubMesh * sm = *i;
			if(sm->getEdgePoints().empty())
				continue; // do nothing

			if(!extremesNode)
				extremesNode = mMeshNode->InsertEndChild(TiXmlElement("extremes"))->ToElement();

			TiXmlElement * submeshNode =
				extremesNode->InsertEndChild(TiXmlElement("submesh_extremes"))->ToElement();

			submeshNode->SetAttribute("index",  StrConv::conv(idx));

			for(vector<Vector3>::type::const_iterator v = sm->getEdgePoints().begin();
				 v != sm->getEdgePoints().end (); ++v)
			{
				TiXmlElement * vert = submeshNode->InsertEndChild(
					TiXmlElement("position"))->ToElement();
				vert->SetAttribute("x", StrConv::conv(v->x));
				vert->SetAttribute("y", StrConv::conv(v->y));
				vert->SetAttribute("z", StrConv::conv(v->z));
			}
		}
	}
	//---------------------------------------------------------------------
	void XMLMeshSerializer::readLodInfo(TiXmlElement * lodNode)
	{
        LogManager::getSingleton().logMessage("Parsing LOD information...");

        const char * val = lodNode->Attribute("strategy");
        // This attribute is optional to maintain backwards compatibility
        if(val)
        {
            String strategyName = val;
            LodStrategy * strategy = LodStrategyManager::getSingleton().get(strategyName);
            mMesh->set(strategy);
        }

		val = lodNode->Attribute("numlevels");
		unsigned short numLevels = static_cast<unsigned short>(
			StringConverter::parseUnsignedInt(val));

		val = lodNode->Attribute("manual");
		bool manual = StringConverter::parseBool(val);

		// Set up the basic structures
        mMesh->setLodCount(numLevels);
        //mMesh->getLodScheme()->setGenEdget(!manual);

		// Parse the detail, start from 1 (the first sub-level of detail)
		unsigned short i = 1;
		TiXmlElement* usageElem;
		if (manual)
		{
			usageElem = lodNode->FirstChildElement("lodmanual");
		}
		else
		{
			usageElem = lodNode->FirstChildElement("lodgenerated");
		}
		while (usageElem)
		{
			if (manual)
			{
				readLodUsageManual(usageElem, i);
				usageElem = usageElem->NextSiblingElement();
			}
			else
			{
				readLodUsageGenerated(usageElem, i);
				usageElem = usageElem->NextSiblingElement();
			}
			++i;
		}

        LogManager::getSingleton().logMessage("LOD information done.");

	}
    //---------------------------------------------------------------------
	void XMLMeshSerializer::readLodUsageManual(TiXmlElement * manualNode, Nindex index)
	{
        MeshLodScheme * scheme = mMesh->getLodScheme();
		const char * val = manualNode->Attribute("value");
        NIIf temp;
        // If value attribute not found check for old name
        if(!val)
        {
            val = manualNode->Attribute("fromdepthsquared");
            if(val)
                LogManager::getSingleton().logMessage("WARNING: 'fromdepthsquared' attribute has been renamed to 'value'.");
			temp = Math::sqrt(StringConverter::parseReal(val));
            // user values are non-squared
            scheme->set(index, temp);
        }
		else
		{
            temp = StringConverter::parseReal(val);
            scheme->set(index, temp);
		}
		scheme->set(index, scheme->getDest(temp));
        scheme->set(index, (ResourceID)manualNode->Attribute("meshname"));
        scheme->set(index, (GeoEdget *)0);
	}
    //---------------------------------------------------------------------
	void XMLMeshSerializer::readLodUsageGenerated(TiXmlElement * genNode, Nindex index)
	{
		const char * val = genNode->Attribute("value");
        NIIf temp;
        MeshLodScheme * scheme = mMesh->getLodScheme();
        // If value attribute not found check for old name
        if(!val)
        {
            val = genNode->Attribute("fromdepthsquared");
            if(val)
                LogManager::getSingleton().logMessage("WARNING: 'fromdepthsquared' attribute has been renamed to 'value'.");
			// user values are non-squared
			temp = Math::sqrt(StringConverter::parseReal(val));
            scheme->set(index, temp);
		}
		else
		{
			temp = StringConverter::parseReal(val);
            scheme->set(index, temp);
		}

		scheme->set(index, scheme->getDest(temp));
        scheme->set(index, (ResourceID)0);
        scheme->set(index, 0);

		// Read submesh face lists
		TiXmlElement * faceListElem = genNode->FirstChildElement("lodfacelist");
		IndexBufferSPtr ibuf;
		while(faceListElem)
		{
			val = faceListElem->Attribute("submeshindex");
			unsigned short subidx = StringConverter::parseUnsignedInt(val);
			val = faceListElem->Attribute("numfaces");
			unsigned short numFaces = StringConverter::parseUnsignedInt(val);
			if(numFaces)
			{
				// use of 32bit indexes depends on submesh
				NCount mtmp = mMesh->getSub(subidx)->getIndexData(0)->mBuffer->getUnitSize();
				bool use32bitindexes = (mtmp == 32);

				// Assign memory: this will be deleted by the submesh
				N_Only(GBuffer).create(ibuf, mtmp, numFaces * 3, Buffer::Buffer::M_NORMAL );

				unsigned short * pShort = 0;
				unsigned int * pInt = 0;
				if(use32bitindexes)
				{
					pInt = static_cast<unsigned int *>(ibuf->lock(Buffer::MM_WHOLE));
				}
				else
				{
					pShort = static_cast<unsigned short *>(ibuf->lock(Buffer::MM_WHOLE));
				}
				TiXmlElement * faceElem = faceListElem->FirstChildElement("face");
				for(unsigned int face = 0; face < numFaces; ++face, faceElem = faceElem->NextSiblingElement())
				{
					if(use32bitindexes)
					{
						val = faceElem->Attribute("v1");
						*pInt++ = StringConverter::parseUnsignedInt(val);
						val = faceElem->Attribute("v2");
						*pInt++ = StringConverter::parseUnsignedInt(val);
						val = faceElem->Attribute("v3");
						*pInt++ = StringConverter::parseUnsignedInt(val);
					}
					else
					{
						val = faceElem->Attribute("v1");
						*pShort++ = StringConverter::parseUnsignedInt(val);
						val = faceElem->Attribute("v2");
						*pShort++ = StringConverter::parseUnsignedInt(val);
						val = faceElem->Attribute("v3");
						*pShort++ = StringConverter::parseUnsignedInt(val);
					}

				}

				ibuf->unlock();
			}
			IndexData * facedata = new IndexData(); // will be deleted by SubMesh
			facedata->mCount = numFaces * 3;
            facedata->mOffset = 0;
            facedata->mBuffer = ibuf;
            SubMesh * sm = mMesh->getSub(subidx);

            sm->setIndexData(index, facedata);

			faceListElem = faceListElem->NextSiblingElement();
		}
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::readExtremes(TiXmlElement * extremesNode, Mesh * m)
	{
		LogManager::getSingleton().logMessage("Reading extremes...");

		// Iterate over all children (submesh_extreme list)
		for(TiXmlElement * elem = extremesNode->FirstChildElement();
			 elem != 0; elem = elem->NextSiblingElement())
		{
			int index = StringConverter::parseInt(elem->Attribute("index"));

			SubMesh * sm = m->getSub(index);
			sm->removeAllEdgePoint();
			for(TiXmlElement* vert = elem->FirstChildElement();
				 vert != 0; vert = vert->NextSiblingElement())
			{
				Vector3 v;
				v.x = StringConverter::parseReal(vert->Attribute("x"));
				v.y = StringConverter::parseReal(vert->Attribute("y"));
				v.z = StringConverter::parseReal(vert->Attribute("z"));
				sm->addEdgePoint(v);
			}
		}

		LogManager::getSingleton().logMessage("Extremes done.");
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::readPoses(TiXmlElement* posesNode, Mesh *m)
	{
		TiXmlElement * poseNode = posesNode->FirstChildElement("pose");

		while(poseNode)
		{
			const char * target = poseNode->Attribute("target");
			if(!target)
			{
				N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
					"Required attribute 'target' missing on pose");
			}
			unsigned short targetID;
			if(String(target) == "mesh")
			{
				targetID = 0;
			}
			else
			{
				// submesh, get index
				const char * val = poseNode->Attribute("index");
				if(!val)
				{
					N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
						"Required attribute 'index' missing on pose");
				}
				unsigned short submeshIndex = static_cast<unsigned short>(
					StringConverter::parseUnsignedInt(val));

				targetID = submeshIndex + 1;
			}

			String name;
			const char * val = poseNode->Attribute("name");
			if(val)
				name = val;
			VertexOffset * pose = m->createOffset(name, targetID);

			TiXmlElement * poseOffsetNode = poseNode->FirstChildElement("poseoffset");
			while(poseOffsetNode)
			{
				uint index = StringConverter::parseUnsignedInt(
					poseOffsetNode->Attribute("index"));
				Vector3 offset;
				offset.x = StringConverter::parseReal(poseOffsetNode->Attribute("x"));
				offset.y = StringConverter::parseReal(poseOffsetNode->Attribute("y"));
				offset.z = StringConverter::parseReal(poseOffsetNode->Attribute("z"));

				if(poseOffsetNode->Attribute("nx") &&
					poseOffsetNode->Attribute("ny") &&
					poseOffsetNode->Attribute("nz"))
				{
					Vector3 normal;
					normal.x = StringConverter::parseReal(poseOffsetNode->Attribute("nx"));
					normal.y = StringConverter::parseReal(poseOffsetNode->Attribute("ny"));
					normal.z = StringConverter::parseReal(poseOffsetNode->Attribute("nz"));
					pose->add(index, offset, normal);
				}
				else
				{
					pose->add(index, offset);
				}

				poseOffsetNode = poseOffsetNode->NextSiblingElement();

			}
			poseNode = poseNode->NextSiblingElement();
		}
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::readAnimations(TiXmlElement * mAnimationsNode, Mesh * pMesh)
	{
		TiXmlElement * animElem = mAnimationsNode->FirstChildElement("animation");
		while(animElem)
		{
			String name = animElem->Attribute("name");
			const char * charLen = animElem->Attribute("length");
			TimeDurMS len = StringConverter::parseReal(charLen);

			Animation * anim = pMesh->createAnimation(name, len);

			TiXmlElement * baseInfoNode = animElem->FirstChildElement("baseinfo");
			if(baseInfoNode)
			{
				String baseName = baseInfoNode->Attribute("baseanimationname");
				TimeDurMS baseTime = StringConverter::parseReal(baseInfoNode->Attribute("basekeyframetime"));
                anim->setBase(baseName, baseTime);
			}

			TiXmlElement * tracksNode = animElem->FirstChildElement("tracks");
			if(tracksNode)
			{
				readTracks(tracksNode, pMesh, anim);
			}

			animElem = animElem->NextSiblingElement();
		}
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::readTracks(TiXmlElement * tracksNode, Mesh * m, Animation * anim)
	{
		TiXmlElement * trackNode = tracksNode->FirstChildElement("track");
		while(trackNode)
		{
			String target = trackNode->Attribute("target");
			unsigned short targetID;
			VertexData * vertexData = 0;
			if(target == "mesh")
			{
				targetID = 0;
				vertexData = m->getVertexData();
			}
			else
			{
				// submesh, get index
				const char * val = trackNode->Attribute("index");
				if(!val)
				{
					N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
						"Required attribute 'index' missing on submesh track");
				}
				unsigned short submeshIndex = static_cast<unsigned short>(
					StringConverter::parseUnsignedInt(val));

				targetID = submeshIndex + 1;
				vertexData = m->getSub(submeshIndex)->getVertexData();

			}

			if(!vertexData)
			{
				N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
					"Track cannot be created for " + target + " since VertexData "
					"does not exist at the specified index");
			}

			// Get type
            VertexKeyFrameTrack * track;
			String strAnimType = trackNode->Attribute("type");
			if(strAnimType == "morph")
			{
                VertexUnitKeyFrameTrack * track = N_new VertexUnitKeyFrameTrack(anim, targetID);
			}
			else if(strAnimType == "pose")
			{
                VertexOffsetKeyFrameTrack * track = N_new VertexOffsetKeyFrameTrack(anim, targetID);
			}
			else
			{
				N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
					"Unrecognised animation track type '" + strAnimType + "'");
			}

			// Create track
            track->setTarget(vertexData);

			TiXmlElement * keyframesNode = trackNode->FirstChildElement("keyframes");
			if(keyframesNode)
			{
				if(track->getAnimationType() == AM_VertexUnit)
				{
					readMorphKeyFrames(keyframesNode, track, vertexData->mCount);
				}
				else // AM_VertexOffset
				{
					readPoseKeyFrames(keyframesNode, track);
				}
			}

			trackNode = trackNode->NextSiblingElement();
		}
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::readMorphKeyFrames(TiXmlElement * keyframesNode,
		VertexKeyFrameTrack * track, size_t vertexCount)
	{
		TiXmlElement * keyNode = keyframesNode->FirstChildElement("keyframe");
		while(keyNode)
		{
			const char * val = keyNode->Attribute("time");
			if(!val)
			{
				N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
					"Required attribute 'time' missing on keyframe");
			}
			NIIf time = StringConverter::parseReal(val);

			VertexUnitKeyFrame * kf = static_cast<VertexUnitKeyFrame *>(track->create(time)));

			bool includesNormals = keyNode->FirstChildElement("normal") != 0;

			size_t vertexSize = sizeof(float) * (includesNormals ? 6 : 3);
			// create a vertex buffer
			VertexBuffer * vbuf;
            N_Only(GBuffer).create(vbuf, vertexSize, vertexCount, Buffer::M_NORMAL );

			float * pFloat = static_cast<float*>(vbuf->lock(Buffer::MM_WHOLE));

			TiXmlElement* posNode = keyNode->FirstChildElement("position");
			TiXmlElement* normNode = keyNode->FirstChildElement("normal");
			for(size_t v = 0; v < vertexCount; ++v)
			{
				if(!posNode)
				{
					N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
						"Not enough 'position' elements under keyframe");
				}

				*pFloat++ = StringConverter::parseReal(posNode->Attribute("x"));
				*pFloat++ = StringConverter::parseReal(posNode->Attribute("y"));
				*pFloat++ = StringConverter::parseReal(posNode->Attribute("z"));

				if(includesNormals)
				{
					if(!normNode)
					{
						N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
							"Not enough 'normal' elements under keyframe");

					}

					*pFloat++ = StringConverter::parseReal(normNode->Attribute("x"));
					*pFloat++ = StringConverter::parseReal(normNode->Attribute("y"));
					*pFloat++ = StringConverter::parseReal(normNode->Attribute("z"));
					normNode = normNode->NextSiblingElement("normal");
				}
				posNode = posNode->NextSiblingElement("position");
			}

			vbuf->unlock();
			kf->setValue(vbuf);
			keyNode = keyNode->NextSiblingElement();
		}
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::readPoseKeyFrames(TiXmlElement* keyframesNode, VertexKeyFrameTrack* track)
	{
		TiXmlElement * keyNode = keyframesNode->FirstChildElement("keyframe");
		while(keyNode)
		{
			const char * val = keyNode->Attribute("time");
			if(!val)
			{
				NII_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
					"Required attribute 'time' missing on keyframe");
			}
			NIIf time = StringConverter::parseReal(val);

			VertexOffsetKeyFrame * kf = static_cast<VertexOffsetKeyFrame *>(track->create(time)));

			// Read all pose references
			TiXmlElement * poseRefNode = keyNode->FirstChildElement("poseref");
			while(poseRefNode)
			{
				const char * attr = poseRefNode->Attribute("poseindex");
				if(!attr)
				{
					N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
						"Required attribute 'poseindex' missing on poseref");
				}
				unsigned short oftIndex = StringConverter::parseUnsignedInt(attr);
				NIIf weight = 1.0f;
				attr = poseRefNode->Attribute("weight");
				if (attr)
				{
					weight = StringConverter::parseReal(attr);
				}

				kf->add(oftIndex, weight);

				poseRefNode = poseRefNode->NextSiblingElement();
			}
			keyNode = keyNode->NextSiblingElement();
		}
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::writePoses(TiXmlElement * meshNode, const Mesh * m)
	{
		if(m->getOffsetCount() == 0)
			return;

		TiXmlElement * posesNode = meshNode->InsertEndChild(TiXmlElement("poses"))->ToElement();

		VertexOffsetList::iterator z, zend = m->getOffsetList().end();
        for(z = m->getOffsetList().begin(); z != zend; ++z)
		{
			const VertexOffset * pose = *z;
			TiXmlElement * poseNode = posesNode->InsertEndChild(TiXmlElement("pose"))->ToElement();
			unsigned short target = pose->getTarget();
			if(target == 0)
			{
				// Main mesh
				poseNode->SetAttribute("target", "mesh");
			}
			else
			{
				// Submesh - rebase index
				poseNode->SetAttribute("target", "submesh");
				poseNode->SetAttribute("index", StrConv::conv(target - 1));
			}
			poseNode->SetAttribute("name", pose->getName());

			bool includesNormals = !pose->getNormals().empty();

			VertexOffset::OffsetMap i, iend = pose->getVertexOffsets().end();
			VertexOffset::NormalsMap j, jend = pose->getNormals().end();
            for(i = pose->getVertexOffsets().begin(), j = pose->getNormals().begin();
                i != iend; ++i, ++j)
			{
				TiXmlElement * poseOffsetElement = poseNode->InsertEndChild(
					TiXmlElement("poseoffset"))->ToElement();

				poseOffsetElement->SetAttribute("index",
					StrConv::conv(i.getCurrentKey()));

				const Vector3 & offset = i->second;
				poseOffsetElement->SetAttribute("x", StrConv::conv(offset.x));
				poseOffsetElement->SetAttribute("y", StrConv::conv(offset.y));
				poseOffsetElement->SetAttribute("z", StrConv::conv(offset.z));

				if (includesNormals)
				{
					const Vector3 & normal = j->second;
					poseOffsetElement->SetAttribute("nx", StrConv::conv(normal.x));
					poseOffsetElement->SetAttribute("ny", StrConv::conv(normal.y));
					poseOffsetElement->SetAttribute("nz", StrConv::conv(normal.z));
				}
			}
		}
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::writeAnimations(TiXmlElement * meshNode, const Mesh * m)
	{
		// Skip if no animation
		if(!m->isVertexFusion())
			return;

		TiXmlElement * animationsNode =
			meshNode->InsertEndChild(TiXmlElement("animations"))->ToElement();

		for(Nindex a = 0; a < m->getAnimationCount(); ++a)
		{
			Animation * anim = m->getAnimation(a);

			TiXmlElement * animNode =
				animationsNode->InsertEndChild(TiXmlElement("animation"))->ToElement();
			animNode->SetAttribute("name", anim->getName());
			animNode->SetAttribute("length", StrConv::conv(anim->getDur()));

			// Optional base keyframe information
			if(anim->getBaseID() != 0)
			{
				TiXmlElement * baseInfoNode =
				animNode->InsertEndChild(TiXmlElement("baseinfo"))->ToElement();
				baseInfoNode->SetAttribute("baseanimationname", anim->getBase());
				baseInfoNode->SetAttribute("basekeyframetime", StrConv::conv(anim->getBaseTime()));
			}

			TiXmlElement * tracksNode = animNode->InsertEndChild(TiXmlElement("tracks"))->ToElement();
			Animation::TrackList i, iend = anim->getTrack().end();
            for(i = anim->getTrack().begin(); i != iend; ++i)
            {
				const VertexKeyFrameTrack * track = *i;

				TiXmlElement * trackNode = tracksNode->InsertEndChild(TiXmlElement("track"))->ToElement();
				KeyFrameTrackID targetID = track->getID();
				if(targetID == 0)
				{
					trackNode->SetAttribute("target", "mesh");
				}
				else
				{
					trackNode->SetAttribute("target", "submesh");
					trackNode->SetAttribute("index", StrConv::conv(targetID-1));
				}

				if(track->getAnimationType() == AM_VertexUnit)
				{
					trackNode->SetAttribute("type", "morph");
					writeMorphKeyFrames(trackNode, track);
				}
				else
				{
					trackNode->SetAttribute("type", "pose");
					writePoseKeyFrames(trackNode, track);
				}
			}
		}
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::writeMorphKeyFrames(TiXmlElement * trackNode, const VertexKeyFrameTrack* track)
	{
		TiXmlElement * keyframesNode =
			trackNode->InsertEndChild(TiXmlElement("keyframes"))->ToElement();

		size_t vertexCount = track->getTarget()->mCount;

		for(NCount k = 0; k < track->getCount(); ++k)
		{
			VertexUnitKeyFrame * kf = static_cast<VertexUnitKeyFrame *>(track->get(k)));
			TiXmlElement * keyNode = keyframesNode->InsertEndChild(TiXmlElement("keyframe"))->ToElement();
			keyNode->SetAttribute("time", StrConv::conv(kf->getTime()));

			VertexBuffer * vbuf = kf->getValue();

			bool includesNormals = vbuf->getUnitSize() > (sizeof(float) * 3);

			float * pFloat = static_cast<float *>(vbuf->lock(Buffer::MM_READ));

			for (size_t v = 0; v < vertexCount; ++v)
			{
				TiXmlElement* posNode =
					keyNode->InsertEndChild(TiXmlElement("position"))->ToElement();
				posNode->SetAttribute("x", StrConv::conv(*pFloat++));
				posNode->SetAttribute("y", StrConv::conv(*pFloat++));
				posNode->SetAttribute("z", StrConv::conv(*pFloat++));

				if (includesNormals)
				{
					TiXmlElement* normNode =
						keyNode->InsertEndChild(TiXmlElement("normal"))->ToElement();
					normNode->SetAttribute("x", StrConv::conv(*pFloat++));
					normNode->SetAttribute("y", StrConv::conv(*pFloat++));
					normNode->SetAttribute("z", StrConv::conv(*pFloat++));
				}
			}

		}
	}
	//-----------------------------------------------------------------------------
	void XMLMeshSerializer::writePoseKeyFrames(TiXmlElement * trackNode, const VertexKeyFrameTrack * track)
	{
		TiXmlElement * keyframesNode = trackNode->InsertEndChild(TiXmlElement("keyframes"))->ToElement();

		for(NCount k = 0; k < track->getCount(); ++k)
		{
			VertexOffsetKeyFrame * kf = static_cast<VertexOffsetKeyFrame *>(track->get(k)));
			TiXmlElement * keyNode =
				keyframesNode->InsertEndChild(TiXmlElement("keyframe"))->ToElement();
			keyNode->SetAttribute("time",
				StrConv::conv(kf->getTime()));

			VertexOffsetKeyFrame::Offsets i, iend = kf->getValue().end();
            for(i = kf->getValue().begin(); i != iend; ++i)
			{
				const VertexOffsetKeyFrame::VertexOffset & oft = *i;
				TiXmlElement* poseRefNode =
					keyNode->InsertEndChild(TiXmlElement("poseref"))->ToElement();

				poseRefNode->SetAttribute("poseindex", StrConv::conv(oft.mTargetIndex));
				poseRefNode->SetAttribute("weight", StrConv::conv(oft.mWeight));
			}
		}
	}
}