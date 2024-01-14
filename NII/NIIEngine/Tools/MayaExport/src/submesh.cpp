////////////////////////////////////////////////////////////////////////////////
// submesh.cpp
// Author       : Francesco Giordana
// Sponsored by : Anygma N.V. (http://www.nazooka.com)
// Start Date   : January 13, 2005
// Copyright    : (C) 2006 by Francesco Giordana
// Email        : fra.giordana@tiscali.it
////////////////////////////////////////////////////////////////////////////////

/*********************************************************************************
*                                                                                *
*   This program is free software; you can redistribute it and/or modify         *
*   it under the terms of the GNU Lesser General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or            *
*   (at your option) any later version.                                          *
*                                                                                *
**********************************************************************************/

#include "submesh.h"

namespace OgreMayaExporter
{
	/***** Class Submesh *****/
	// constructor
	Submesh::Submesh(const MString& name)
	{
		m_pBlendShape = NULL;
		clear();
		m_name = name;
	}

	// destructor
	Submesh::~Submesh()
	{
		clear();
	}

	// clear data
	void Submesh::clear()
	{
		m_name = "";
		m_numTriangles = 0;
		m_pMaterial = NULL;
		m_indices.clear();
		m_vertices.clear();
		m_faces.clear();
		m_uvsets.clear();
		m_use32bitIndexes = false;
		m_pBlendShape = NULL;
	}

	// return number of triangles composing the mesh
	long Submesh::numTriangles()
	{
		return m_numTriangles;
	}

	// return number of vertices composing the mesh
	long Submesh::numVertices()
	{
		return m_numVertices;
	}

	// return submesh name
	MString& Submesh::name()
	{
		return m_name;
	}

/***** load data *****/
	MStatus Submesh::loadMaterial(MObject& shader,MStringArray& uvsets,ParamList& params)
	{
		MPlug plug;
		MPlugArray srcplugarray;
		bool foundShader = false;
		MStatus stat;
		MFnDependencyNode* pShader;
		//get shader from shading group
		MFnDependencyNode shadingGroup(shader);
		plug = shadingGroup.findPlug("surfaceShader");
		plug.connectedTo(srcplugarray,true,false,&stat);
		for (int i=0; i<srcplugarray.length() && !foundShader; i++)
		{
			if (srcplugarray[i].node().hasFn(MFn::kLambert) || srcplugarray[i].node().hasFn(MFn::kSurfaceShader) ||
				srcplugarray[i].node().hasFn(MFn::kPluginHwShaderNode) )
			{
				pShader = new MFnDependencyNode(srcplugarray[i].node());
				foundShader = true;
			}
		}
		if (foundShader)
		{
			std::cout << "Found material: " << pShader->name().asChar() << "\n";
			std::cout.flush();
			//check if this material has already been created
			//fix material name, adding the requested prefix
			MString tmpStr = params.matPrefix;
			if (tmpStr != "")
				tmpStr += "/";
			tmpStr += pShader->name();
			MStringArray tmpStrArray;
			tmpStr.split(':',tmpStrArray);
			MString name = "";
			for (int i=0; i<tmpStrArray.length(); i++)
			{
				name += tmpStrArray[i];
				if(i < tmpStrArray.length() - 1)
					name += "_";
			}
			Material* pMaterial = MaterialSet::getSingleton().getMaterial(name);
			//if the material has already been created, update the pointer
			if (pMaterial)
				m_pMaterial = pMaterial;
			//else create it and add it to the material set
			else
			{
				pMaterial = new Material();
				pMaterial->load(pShader, uvsets, params);
				m_pMaterial = pMaterial;
				MaterialSet::getSingleton().addMaterial(pMaterial);
			}
			//delete temporary shader
			delete pShader;
		}
		else
		{
			std::cout << "Unsupported material, replacing with default lambert\n";
			std::cout.flush();
			m_pMaterial = MaterialSet::getSingleton().getDefaultMaterial();
		}

		//loading complete
		return MS::kSuccess;
	}

	MStatus Submesh::load(const MDagPath& dag,std::vector<face>& faces, std::vector<vertexInfo>& vertInfo, MPointArray& points,
		MFloatVectorArray& normals, MStringArray& texcoordsets,ParamList& params,bool opposite)
	{
		//save the dag path of the maya node from which this submesh will be created
		m_dagPath = dag;
		//create the mesh Fn
		MFnMesh mesh(dag);
		std::cout << "Loading submesh : " << m_name.asChar() << "...";
		std::cout.flush();
		//save uvsets info
		for (int i=m_uvsets.size(); i<texcoordsets.length(); i++)
		{
			uvset uv;
			uv.size = 2;
			m_uvsets.push_back(uv);
		}
		//iterate over faces array, to retrieve vertices info
		std::vector<int> vtx_mapping;
		vtx_mapping.resize(vertInfo.size());
		for (int i=0; i<vtx_mapping.size(); i++)
			vtx_mapping[i] = -1;
		for (int i=0; i<faces.size(); i++)
		{
			face newFace;
			// If we are using shared geometry, indexes refer to the vertex buffer of the whole mesh
			if (params.useSharedGeom)
			{
				if(opposite)
				{	// reverse order of face vertices for correct culling
					newFace.v[0] = faces[i].v[2];
					newFace.v[1] = faces[i].v[1];
					newFace.v[2] = faces[i].v[0];
				}
				else
				{
					newFace.v[0] = faces[i].v[0];
					newFace.v[1] = faces[i].v[1];
					newFace.v[2] = faces[i].v[2];
				}
			}
			// Otherwise we create a vertex buffer for this submesh.
			// First we create a list of the vertices that we need for this submesh
			// then we get the vertex info for those vertices.
			// The indices are stored in a set so we don't get duplicates
			else
			{	// faces are triangles, so retrieve index of the three vertices
				for (int j=0; j<3; j++)
				{
					long new_idx;
					// Store the vertex index, if it hasn't already been stored in the set
					long idx = faces[i].v[j];
					if (vtx_mapping[idx] == -1)
					{
						m_indices.push_back(idx);
						new_idx = m_indices.size() - 1;
						vtx_mapping[idx] = new_idx;
					}
					else
						new_idx = vtx_mapping[idx];
					// Save the index in the set to the face info
					if (opposite)	// reverse order of face vertices to get correct culling
						newFace.v[2-j] = new_idx;
					else
						newFace.v[j] = new_idx;
				}
			}
			m_faces.push_back(newFace);
		}
		// Get the vertices info (if there are vertices to store in this submesh)
		for (int i=0; i<m_indices.size(); i++)
		{
			long vtx_idx = m_indices[i];
			vertex v;
			// get the vertex info
			vertexInfo vInfo = vertInfo[vtx_idx];
			// save vertex coordinates (rescale to desired length unit)
			MPoint point = points[vInfo.pointIdx] * params.lum;
			if (fabs(point.x) < PRECISION)
				point.x = 0;
			if (fabs(point.y) < PRECISION)
				point.y = 0;
			if (fabs(point.z) < PRECISION)
				point.z = 0;
			v.x = point.x;
			v.y = point.y;
			v.z = point.z;
			// save vertex normal
			MFloatVector normal = normals[vInfo.normalIdx];
			if (fabs(normal.x) < PRECISION)
				normal.x = 0;
			if (fabs(normal.y) < PRECISION)
				normal.y = 0;
			if (fabs(normal.z) < PRECISION)
				normal.z = 0;
			if (opposite)
			{
				v.n.x = -normal.x;
				v.n.y = -normal.y;
				v.n.z = -normal.z;
			}
			else
			{
				v.n.x = normal.x;
				v.n.y = normal.y;
				v.n.z = normal.z;
			}
			v.n.normalize();
			// save vertex color
			v.r = vInfo.r;
			v.g = vInfo.g;
			v.b = vInfo.b;
			v.a = vInfo.a;
			// save vertex bone assignements
			for (int k=0; k<vInfo.vba.size(); k++)
			{
				vba newVba;
				newVba.jointIdx = vInfo.jointIds[k];
				newVba.weight = vInfo.vba[k];
				if (newVba.weight > PRECISION)
					v.vbas.push_back(newVba);
			}
			// save texture coordinates
			for (int k=0; k<vInfo.u.size(); k++)
			{
				texcoord newTexCoords;
				newTexCoords.u = vInfo.u[k];
				newTexCoords.v = vInfo.v[k];
				newTexCoords.w = 0;
				v.texcoords.push_back(newTexCoords);
			}
			// save vertex index in maya mesh, to retrieve future positions of the same vertex
			v.index = vInfo.pointIdx;
			// add newly created vertex to vertex list
			m_vertices.push_back(v);
		}
		// set use32bitIndexes flag
		if (params.useSharedGeom || (m_vertices.size() > 65535) || (m_faces.size() > 65535))
			m_use32bitIndexes = true;
		else
			m_use32bitIndexes = false;
		// get submesh bounding box
		MPoint min = mesh.boundingBox().min();
		MPoint max = mesh.boundingBox().max();
		MBoundingBox bbox(min,max);
		if (params.exportWorldCoords)
			bbox.transformUsing(dag.inclusiveMatrix());
		min = bbox.min() * params.lum;
		max = bbox.max() * params.lum;
		MBoundingBox newbbox(min,max);
		m_boundingBox = newbbox;
		// add submesh pointer to parameters list
		params.loadedSubmeshes.push_back(this);

		std::cout << "DONE\n";
		std::cout.flush();
		return MS::kSuccess;
	}


	// Load a keyframe for this submesh
	MStatus Submesh::loadKeyframe(Track& t,float time,ParamList& params)
	{
		// create a new keyframe
		vertexKeyframe k;
		// set keyframe time
		k.time = time;
		// get the mesh Fn
		MFnMesh mesh(m_dagPath);
		// get vertex positions
		MFloatPointArray points;
		if (params.exportWorldCoords)
			mesh.getPoints(points,MSpace::kWorld);
		else
			mesh.getPoints(points,MSpace::kObject);
		// calculate vertex offsets
		for (int i=0; i<m_vertices.size(); i++)
		{
			vertexPosition pos;
			vertex v = m_vertices[i];
			pos.x = points[v.index].x * params.lum;
			pos.y = points[v.index].y * params.lum;
			pos.z = points[v.index].z * params.lum;
			if (fabs(pos.x) < PRECISION)
					pos.x = 0;
			if (fabs(pos.y) < PRECISION)
				pos.y = 0;
			if (fabs(pos.z) < PRECISION)
				pos.z = 0;
			k.positions.push_back(pos);
		}
		// add keyframe to given track
		t.addVertexKeyframe(k);
		if (params.vertBB)
		{
			// update bounding box
			MPoint min = mesh.boundingBox().min();
			MPoint max = mesh.boundingBox().max();
			MBoundingBox bbox(min,max);
			if (params.exportWorldCoords)
				bbox.transformUsing(m_dagPath.inclusiveMatrix());
			min = bbox.min() * params.lum;
			max = bbox.max() * params.lum;
			MBoundingBox newbbox(min,max);
			m_boundingBox.expand(newbbox);
		}
		// keyframe successfully loaded
		return MS::kSuccess;
	}

/***** Export data *****/
	// Write submesh data to an Ogre compatible mesh
	MStatus Submesh::createOgreSubmesh(Ogre::MeshPtr pMesh,const ParamList& params)
	{
		MStatus stat;
		// Create a new submesh
		NII::SubMesh * obj;
		if (m_name != "")
        {
			obj = pMesh->createSub();
            obj->setName(m_name.asChar());
        }
        else
			obj = pMesh->createSub();
		// Set material
        obj->setMaterial(m_pMaterial->name().asChar());
        // Set use shared geometry flag
		obj->setSelfVertex(!params.useSharedGeom);
		// Create vertex data for current submesh
        VertexData * smvd = new Ogre::VertexData();
		obj->setVertexData(smvd);
        // Set number of indexes
        obj->getIndexData(0)->mCount = 3 * m_faces.size();
		smvd->mCount = m_vertices.size();
		// Check if we need to use 32 bit indexes
		bool use32BitIndexes = false;
		if (m_vertices.size() > 65536 || params.useSharedGeom)
		{
			use32BitIndexes = true;
		}
		// Create a new index buffer
        NII::N_Only(GBuffer).create(obj->getIndexData(0)->mBuffer, use32BitIndexes ? 32 : 16, obj->getIndexData(0)->mCount, Buffer::M_NORMAL );
		// Fill the index buffer with faces data
		if(use32BitIndexes)
        {
			Ogre::uint32 * pIdx = static_cast<Ogre::uint32 *>(
				obj->getIndexData(0)->mBuffer->lock(Ogre::Buffer::MM_WHOLE));
			for(int i = 0; i < m_faces.size(); ++i)
			{
				*pIdx++ = static_cast<Ogre::uint32>(m_faces[i].v[0]);
				*pIdx++ = static_cast<Ogre::uint32>(m_faces[i].v[1]);
				*pIdx++ = static_cast<Ogre::uint32>(m_faces[i].v[2]);
			}
			obj->getIndexData(0)->mBuffer->unlock();
        }
        else
        {
            Ogre::uint16 * pIdx = static_cast<Ogre::uint16 *>(
				obj->getIndexData(0)->mBuffer->lock(Ogre::Buffer::MM_WHOLE));
            for(int i = 0; i < m_faces.size(); ++i)
			{
				*pIdx++ = static_cast<Ogre::uint16>(m_faces[i].v[0]);
				*pIdx++ = static_cast<Ogre::uint16>(m_faces[i].v[1]);
				*pIdx++ = static_cast<Ogre::uint16>(m_faces[i].v[2]);
			}
			obj->getIndexData(0)->mBuffer->unlock();
		}
		// Define vertex declaration (only if we're not using shared geometry)
		if(!params.useSharedGeom)
		{
			// Fill the vertex buffer using the newly created vertex declaration
			stat = createOgreVertexBuffer(obj, m_vertices, 
                params.exportVertNorm, params.exportVertCol);
			// Write vertex bone assignements list
			if(params.exportVBA)
			{
				// Create a new vertex bone assignements list
				Ogre::Mesh::SkeletonVertexList vbas;
				// Scan list of shared geometry vertices
				for(int i = 0; i < m_vertices.size(); ++i)
				{
					vertex v = m_vertices[i];
					// Add all bone assignemnts for every vertex to the bone assignements list
					for(int j = 0; j < v.vbas.size(); ++j)
					{
						Ogre::SkeletonVertex vba;
						vba.mVertexIndex = i;
						vba.mBoneIndex = v.vbas[j].jointIdx;
						vba.mWeight = v.vbas[j].weight;
						vbas.insert(NII::Mesh::SkeletonVertexList::value_type(i, vba));
					}
				}
				// Rationalise the bone assignements list
				SkeletonManager::opt(vbas, obj->getVertexData()->mCount);
				// Add bone assignements to the submesh
				for(Ogre::Mesh::SkeletonVertexList::iterator bi = vbas.begin(); bi != vbas.end(); bi++)
				{
					obj->addSkeleton(bi->second);
				}
				obj->buildSkeleton();
			}
            obj->getVertexData()->build(
                (params.exportVBA ? VertexData::M_Skeleton : VertexData::M_None) |
                    ((params.exportBlendShapes || params.exportVertAnims) ? 
                        VertexData::M_Animation : VertexData::M_None), true);
		}
		return MS::kSuccess;
	}

	// Create an Ogre compatible vertex buffer
	MStatus Submesh::createOgreVertexBuffer(NII::SubMesh * obj,
        const std::vector<vertex> & vertices, bool nrls, bool cool)
	{
        Ogre::VertexData * vd = obj->getVertexData();
        size_t offset = 0;
			// Add vertex position
        vd->add(Ogre::DT_Float3x, Ogre::VT_Pos, 0, offset);
        offset += Ogre::VertexUnit::getTypeSize(Ogre::DT_Float3x);
        // Add vertex normal
        if(nrls)
        {
            vd->add(Ogre::DT_Float3x, Ogre::VT_Normals, 0, offset);
            offset += Ogre::VertexUnit::getTypeSize(Ogre::DT_Float3x);
        }
        // Add vertex colour
        if(cool)
        {
            vd->add(NII::DT_Colour, NII::VT_Diffuse, 0, offset);
            offset += NII::VertexUnit::getTypeSize(NII::DT_Colour);
        }
        // Add texture coordinates
        for(int i = 0; i < m_vertices[0].texcoords.size(); ++i)
        {
            NII::DataType uvType = NII::DataTypeUtil::getMultiType(NII::DT_Float1x, 2);
            vd->add(uvType, NII::VT_Tex_Coord, 0, offset, i);
            offset += NII::VertexUnit::getTypeSize(uvType);
        }
            
		NII::VertexBuffer * vbuf;
		NII::N_Only(GBuffer).create(vbuf, vd->getUnitSize(0), obj->getVertexData()->mCount, Buffer::M_NORMAL );
		obj->getVertexData()->attach(0, vbuf);
		size_t vertexSize = vd->getUnitSize(0);
		char * pBase = static_cast<char *>(vbuf->lock(Ogre::Buffer::MM_WHOLE));
        VertexUnitList elems = vd->find(0);
        VertexUnitList::iterator ei, eiend = elems.end();
		float * pFloat;
		Ogre::RGBA * pRGBA;
		// Fill the vertex buffer with shared geometry data
		Ogre::Colour col;
		float ucoord, vcoord;
		for(long vi = 0; vi < vertices.size(); ++vi)
		{
			int iTexCoord = 0;
			vertex v = vertices[vi];
			for(ei = elems.begin(); ei != eiend; ++ei)
			{
				NII::VertexUnit & elem = *ei;
				switch(elem.mVType)
				{
				case NII::VT_Pos:
					elem.getData(pBase, pFloat);
					*pFloat++ = v.x;
					*pFloat++ = v.y;
					*pFloat++ = v.z;
					break;
				case NII::VT_Normals:
					elem.getData(pBase, pFloat);
					*pFloat++ = v.n.x;
					*pFloat++ = v.n.y;
					*pFloat++ = v.n.z;
					break;
				case NII::VT_Diffuse:
					{
						elem.getData(pBase, pRGBA);
						NII::Colour col(v.r, v.g, v.b, v.a);
						*pRGBA = NII::Engine::getColourValue(col,
							NII::Engine::getColourFormat());
					}
					break;
				case NII::VT_Tex_Coord:
					elem.getData(pBase, pFloat);
					ucoord = v.texcoords[iTexCoord].u;
					vcoord = v.texcoords[iTexCoord].v;
					*pFloat++ = ucoord;
					*pFloat++ = vcoord;
					iTexCoord++;
					break;
				}
			}
			pBase += vertexSize;
		}
		vbuf->unlock();
		return MS::kSuccess;
	}

}; //end of namespace
