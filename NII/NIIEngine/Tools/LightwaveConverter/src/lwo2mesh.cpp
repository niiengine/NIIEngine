#include "Vector3.h"
#include "lwo2mesh.h"
#include "Ogre.h"
#include "OgreMesh.h"
#include "OgreStringConverter.h"
#include "NiiMemGBufferManager.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <math.h>
#include <float.h>  // FLT_MIN, FLT_MAX
#include <libgen.h> // dirname(), basename().
#include <string.h> // strtok();
#include <string>   // string class
#endif

#define POLYLIMIT 0x5555
#define POINTLIMIT 0x5555

extern Mesh::LodDistanceList distanceList;
extern NIIf reduction;
extern bool flags[NUMFLAGS];
extern MaterialSerializer* materialSerializer;
extern char *matPrefix;

extern ostream& nl(ostream& os);

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX

/* We expect the caller to provide an arrays of chars for the output. */
void _splitpath( const char *_fn, char *_drive, char *_dir, char *_node, char *_ext ) {

   /* A crazy mix of both c and c++.. */

   const char *delimiters = ".";
   char buf[ _MAX_DRIVE+_MAX_DIR+_MAX_FNAME+_MAX_EXT + 5 ];
   char *exte, *ddir, *fnam, *_ext_tmp;

   strcpy( buf, _fn );
   strcpy( _drive, "" ); // _drive is always empth on linux.

   if ( String( buf ).empty() ) {
       strcpy( _node, "" );
       strcpy( _dir, "" );
       strcpy( _ext, "" );
       return;
   }

   fnam = basename( buf );
   strcpy( _node, fnam );
   ddir = dirname( buf );
   strcpy( _dir, ddir );

   _ext_tmp = strtok( fnam, delimiters );
   while ( ( _ext_tmp = strtok( NULL, delimiters ) ) != NULL ) exte = _ext_tmp;
   strcpy( _ext, exte );

   _node[ strlen(_node) - strlen(_ext) - 1 ] = '\0';

}

/* We expect the caller to provide an array of chars for the output. */
void _makepath( char *_fn, const char *_drive, const char *_dir,
       const char *_node, const char *_ext ) {

   /* A crazy mix of both c and c++.. */

   std::string buf("");

   if ( _drive != NULL ) // On Linux, this is usually empty.
       buf += _drive;

   if ( _dir != NULL ) { // The directory part.
       if ( std::string(_dir).empty() )
           buf += ".";
       buf += _dir;
       buf += "/";
   }

   if ( _node != NULL ) // The filename without the extension.
       buf += _node;

   if ( _ext != NULL ) { // The extension.
       if ( std::string( _ext ).compare( 0, 1, "." ) != 0 )
           buf += ".";
       buf += _ext;
   }

   strcpy( _fn, buf.c_str() );
}

#endif

void Lwo2MeshWriter::doExportMaterials()
{
	char
		drive[ _MAX_DRIVE ],
		dir[ _MAX_DIR ],
		node[ _MAX_FNAME ],
		ext[ _MAX_EXT ],
		texname [128];

	unsigned int slength = 0;

	if (flags[RenameMaterials])
	{
		if (flags[UseInteractiveMethod])
		{
			for (unsigned int i = 0; i < object->surfaces.size(); ++i)
			{
				lwSurface *surface = object->surfaces[i];
				cout << "Rename surface " << surface->name << " to: ";
				cin >> texname;
				surface->setname(texname);
			}
		} else {
			_splitpath( dest, drive, dir, node, ext );

			for (unsigned int i = 0; i < object->surfaces.size(); ++i)
			{
				lwSurface *surface = object->surfaces[i];
				if (flags[UsePrefixMethod])
					strcpy(texname,matPrefix);
				else
				{
					strcpy(texname,node);
					strcat(texname,"_");
				}

				strcat(texname, surface->name);
				surface->setname(texname);
			}
		}
	}

	for(unsigned int i = 0; i < object->surfaces.size(); ++i)
	{
		lwSurface * surface = object->surfaces[i];

		// Create deferred material so no load
		MaterialPtr ogreMat = MaterialManager::getSingleton().getByName(surface->name);

		if (ogreMat.isNull())
		{
			ogreMat = MaterialManager::getSingleton().create(surface->name,
                GroupDefault);

            ogreMat->setColour(ShaderChColour(Colour(surface->color.rgb[0],
				surface->color.rgb[1], surface->color.rgb[2]),
                Colour(surface->diffuse.val * surface->color.rgb[0],
                surface->diffuse.val * surface->color.rgb[1],
                surface->diffuse.val * surface->color.rgb[2], 1.0f),
                Colour(surface->specularity.val * surface->color.rgb[0],
				surface->specularity.val * surface->color.rgb[1],
				surface->specularity.val * surface->color.rgb[2], 1.0f),
                Colour(surface->luminosity.val * surface->color.rgb[0],
				surface->luminosity.val * surface->color.rgb[1],
				surface->luminosity.val * surface->color.rgb[2]),
                surface->glossiness.val,
                CM_M_AMBIENT | CM_M_DIFFUSE | CM_M_SPECULAR | CM_M_EMISSIVE));

			unsigned int j;
			lwTexture * tex;
			int cindex;
			lwClip * clip;

			for(j = 0; j < surface->color.textures.size(); ++j)
			{
				tex = surface->color.textures[j];
				cindex = tex->param.imap->cindex;
				clip = object->lwFindClip(cindex);

				if(clip)
				{
					_splitpath(clip->source.still->name, drive, dir, node, ext);
					_makepath(texname, 0, 0, node, ext);
					ogreMat->get(0)->get(0)->getTexture().create(texname);
				}
			}

			for (j = 0; j < surface->transparency.val.textures.size(); j++)
			{
				tex = surface->transparency.val.textures[j];
				cindex = tex->param.imap->cindex;
				clip = object->lwFindClip(cindex);

				if (clip)
				{
					_splitpath( clip->source.still->name, drive, dir, node, ext );
					_makepath( texname, 0, 0, node, ext );
					ogreMat->get(0)->get(0)->getTexture().create(texname);
				}
			}
			materialSerializer->queueForExport(ogreMat);
		}
	}
}

Skeleton *Lwo2MeshWriter::doExportSkeleton(const String &skelName, int l)
{
	vpolygons bones;
	bones.clear();
	bones.reserve(256);

	vpoints bonepoints;
	bonepoints.clear();
	bonepoints.reserve(512);

	if (l == -1)
	{
		for (l = 0; l < object->layers.size(); ++l)
		{
			copyPoints(-1, ID_BONE, object->layers[l]->points, bonepoints);
			copyPolygons(-1, ID_BONE, object->layers[l]->polygons, bones);
		}
	}
	else
	{
		copyPoints(-1, ID_BONE, object->layers[l]->points, bonepoints);
		copyPolygons(-1, ID_BONE, object->layers[l]->polygons, bones);
	}

	if (!bones.size()) return NULL; // no bones means no skeleton

	SkeletonPtr ogreskel = Ogre::SkeletonManager::getSingleton().create(skelName, NII::GroupDefault);

	unsigned int i;
	// Create all the bones in turn
	for (i = 0; i < bones.size(); ++i)
	{
		lwPolygon * bone = bones[i];
		if(bone->vertices.size() != 2) 
            continue; // a bone has only 2 sides

		Bone * ogreBone = ogreskel->create("Bone");

		Ogre::Vector3 bonePos(bone->vertices[0]->point->x, bone->vertices[0]->point->y, bone->vertices[0]->point->z);

		ogreBone->setPos(bonePos);
		// Hmm, Milkshape has chosen a Euler angle representation of orientation which is not smart
		// Rotation Matrix or Quaternion would have been the smarter choice
		// Might we have Gimbal lock here? What order are these 3 angles supposed to be applied?
		// Grr, we'll try our best anyway...
		Quaternion qx, qy, qz, qfinal;
/*
		qx.from(msBoneRot[0], Vector3::X);
		qy.from(msBoneRot[1], Vector3::Y);
		qz.from(msBoneRot[2], Vector3::Z);
*/
		// Assume rotate by x then y then z
		qfinal = qz * qy * qx;
		ogreBone->setOri(qfinal);
	}
/*
	for (i = 0; i < numBones; ++i)
	{
		msBone* bone = msModel_GetBoneAt(pModel, i);

		if (strlen(bone->szParentName) == 0)
		{
		}
		else
		{
			Bone* ogrechild = ogreskel->get(bone->szName);
			Bone* ogreparent = ogreskel->get(bone->szParentName);

			if (ogrechild == 0)
			{
				continue;
			}
			if (ogreparent == 0)
			{
				continue;
			}
			// Make child
			ogreparent->add(ogrechild);
		}


	}

	// Create the Animation(s)
	doExportAnimations(pModel, ogreskel);

	// Create skeleton serializer & export
	SkeletonSerializer serializer;
	serializer.exportSkeleton(ogreskel, szFile);

	ogreMesh->setSkeleton(ogreskel);

	return ogreskel;
*/
    if (!ogreskel.isNull())
        Ogre::SkeletonManager::getSingleton().remove(ogreskel->getPrc());

	return NULL;
}

#define POSITION_BINDING 0
#define NORMAL_BINDING 1
#define TEXCOORD_BINDING 2

VertexData *Lwo2MeshWriter::setupVertexData(unsigned short vertexCount, VertexData *oldVertexData, bool deleteOldVertexData)
{
	VertexData * vertexData;
    N_Only(GBuffer).create(vertexData);

	if(oldVertexData)
	{
        // Basic vertex info
        vertexData->mOffset = oldVertexData->mOffset;
		vertexData->mCount = oldVertexData->mCount + vertexCount;

		const VertexData::BindingList & bindings = oldVertexData->getAttachs();
		VertexData::BindingList::const_iterator vbi, vbend = bindings.end();
		for(vbi = bindings.begin(); vbi != vbend; ++vbi)
		{
			VertexBufferSPtr srcbuf = vbi->second;
			// create new buffer with the same settings
			VertexBuffer * dstBuf;
            N_Only(GBuffer).create(dstBuf, srcbuf->getUnitSize(), srcbuf->getUnitCount() + mCount, srcbuf->getModeMark());

			// copy data
			dstBuf->memcpy(srcbuf, 0, 0, srcbuf->getSize());

			// Copy binding
			vertexData->attach(vbi->first, dstBuf);
		}

        // Copy elements
        const VertexUnitList & elems = oldVertexData->getUnits();
        VertexUnitList::const_iterator ei, eiend = elems.end();
        for(ei = elems.begin(); ei != eiend; ++ei)
        {
            vertexData->add(
                ei->mType,
                ei->mVType,            
                ei->mSource,
                ei->mOffset,
                ei->mIndex);
        }
		if (deleteOldVertexData) delete oldVertexData;
	}
	else
	{
		vertexData->mCount = vertexCount;

		vertexData->add(DT_Float3x, VT_Pos, POSITION_BINDING, 0);
		VertexBuffer * pbuf;
        N_Only(GBuffer).create(pbuf, vertexData->getUnitSize(POSITION_BINDING), vertexData->mCount, Buffer::M_NORMAL );
		vertexData->attach(POSITION_BINDING, pbuf);

		vertexData->add(DT_Float3x, VT_Normals, NORMAL_BINDING, 0);
		N_Only(GBuffer).create(pbuf, vertexData->getUnitSize(NORMAL_BINDING), vertexData->mCount, Buffer::M_NORMAL );
		vertexData->attach(NORMAL_BINDING, pbuf);

		vertexData->add(DT_Float2x, VT_Tex_Coord, TEXCOORD_BINDING, 0);
		N_Only(GBuffer).create(pbuf, vertexData->getUnitSize(TEXCOORD_BINDING), vertexData->mCount, Buffer::M_NORMAL );
		vertexData->attach(TEXCOORD_BINDING, pbuf);
	}
	return vertexData;
}

void Lwo2MeshWriter::copyPoints(int surfaceIndex, unsigned long polygontype, vpoints &sourcepoints, vpoints &destpoints)
{
	for (unsigned int i = 0; i < sourcepoints.size(); i++)
	{
		lwPoint *point = sourcepoints[i];

		for (unsigned int j = 0; j < point->polygons.size(); j++)
		{
			lwPolygon *polygon = point->polygons[j];
			if (polygon->type == polygontype)
				if (surfaceIndex == -1 || surfaceIndex == polygon->surfidx)
				{
					destpoints.push_back(point);
					break;
				}
		}
	}
}

void Lwo2MeshWriter::copyPolygons(int surfaceIndex, unsigned long polygontype, vpolygons &sourcepolygons, vpolygons &destpolygons)
{
	for (unsigned int i = 0; i < sourcepolygons.size(); i++)
	{
		lwPolygon *polygon = sourcepolygons[i];
		if (polygon->type == polygontype)
			if (surfaceIndex == -1 || surfaceIndex == polygon->surfidx)
				destpolygons.push_back(polygon);
	}
}

void Lwo2MeshWriter::copyDataToVertexData(vpoints & points,
    vpolygons & polygons, vvmaps & vmaps, IndexData * indexData,
        VertexData * vertexData, unsigned short vertexDataOffset)
{
	lwVMap * vmap = 0;
	unsigned int ni;

	VertexBufferSPtr pbuf = vertexData->getBuffer(POSITION_BINDING);
	VertexBufferSPtr nbuf = vertexData->getBuffer(NORMAL_BINDING);
	VertexBufferSPtr tbuf = vertexData->getBuffer(TEXCOORD_BINDING);
	IndexBufferSPtr ibuf = indexData->mBuffer;

	float * pPos = static_cast<float *>(pbuf->lock(Buffer::MM_WHOLE));
	float * pNor = static_cast<float *>(nbuf->lock(Buffer::MM_WHOLE));
	float * pTex = static_cast<float *>(tbuf->lock(Buffer::MM_WHOLE));
	unsigned short * pIdx = static_cast<unsigned short *>(ibuf->lock(Buffer::MM_WHOLE));

	for(unsigned int p = 0; p < polygons.size(); p++)
	{
		lwPolygon * polygon = polygons[p];

		if (polygon->vertices.size() != 3) continue; // only copy triangles;

		for (unsigned int v = 0; v < polygon->vertices.size(); v++)
		{
			lwVertex *vertex = polygon->vertices[v];
			lwPoint *point = vertex->point;
			unsigned short i = getPointIndex(point, points);

			pIdx[p*3 + v] = vertexDataOffset + i;

			ni = (vertexDataOffset + i) * 3;

			pPos[ni] = vertex->point->x;
			pPos[ni + 1] = vertex->point->y;
			pPos[ni + 2] = vertex->point->z;

			pNor[ni] = vertex->normal.x;
			pNor[ni + 1] = vertex->normal.y;
			pNor[ni + 2] = vertex->normal.z;

			bool found = false;

			ni = (vertexDataOffset + i) * 2;

			for (unsigned int v = 0; v < point->vmaps.size(); v++)
			{
				for (unsigned int vr = 0; vr < vmaps.size(); vr++)
				{
					vmap = vmaps[vr];
					if (point->vmaps[v].vmap == vmap)
					{
						int n = point->vmaps[v].index;

						pTex[ni] = vmap->val[n][0];
						pTex[ni + 1] = 1.0f - vmap->val[n][1];
						found = true;
						break;
					}
				}
				if (found) break;
			}
		}
	}
    pbuf->unlock();
    nbuf->unlock();
    tbuf->unlock();
	ibuf->unlock();
}

void Lwo2MeshWriter::prepLwObject(void)
{
	unsigned int l, p;

	for (l = 0; l < object->layers.size(); l++)
	{
		lwLayer *layer = object->layers[l];

#ifdef _DEBUG
		cout << "Triangulating layer " << l << ", Polygons before: " << layer->polygons.size();
#endif
		layer->triangulatePolygons();
#ifdef _DEBUG
		cout << ", Polygons after: " << layer->polygons.size() << endl;
#endif

		// Mirror x-coord for Ogre;
		for (p = 0; p < layer->points.size(); p++)
		{
			layer->points[p]->x *= -1.0f;
			layer->points[p]->polygons.clear();
		}
		// Unscrew the bounding box
		float x = layer->bboxmin.x * -1.0f;
		layer->bboxmin.x = layer->bboxmax.x * -1.0f;
		layer->bboxmax.x = x;

		for ( p = 0; p < layer->polygons.size(); p++ )
		{
			lwPolygon *polygon = layer->polygons[ p ];
			for (unsigned int j = 0; j < polygon->vertices.size(); j++ )
				polygon->vertices[ j ]->point->polygons.push_back(polygon);
		}

		for (p = 0; p < layer->polygons.size(); p++)
			layer->polygons[p]->flip();

		layer->calculatePolygonNormals();
		layer->calculateVertexNormals();
	}
}

inline int Lwo2MeshWriter::getPointIndex(lwPoint *point, vpoints &points)
{
	for (unsigned int i = 0; i < points.size(); ++i)
		if (points[i] == point) return i;

	return -1;
}

inline String Lwo2MeshWriter::makeLayerFileName(char* dest, unsigned int l, char *layername)
{
	char
		drive[ _MAX_DRIVE ],
		dir[ _MAX_DIR ],
		node[ _MAX_FNAME ],
		ext[ _MAX_EXT ],
		buf[ _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT + 5 ];

	String LayerFileName;
	String TempName;

	_splitpath( dest, drive, dir, node, ext );

	TempName = String( node );

	if (layername) {
		TempName += ".";
		TempName += layername;
	} else {
		TempName += ".layer" + StrConv::conv(l);
	}

	_makepath( buf, drive, dir, TempName.c_str(), ext );
	LayerFileName = String( buf );

	return LayerFileName;
}

inline String Lwo2MeshWriter::makeMaterialFileName(char* dest)
{
	char
		drive[ _MAX_DRIVE ],
		dir[ _MAX_DIR ],
		node[ _MAX_FNAME ],
		ext[ _MAX_EXT ],
		buf[ _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT + 5 ];

	String MaterialFileName;

	_splitpath( dest, drive, dir, node, ext );
	_makepath( buf, drive, dir, node, ".material" );

	const char *test = MaterialFileName.c_str();
	MaterialFileName = String( buf );

	return MaterialFileName;
}

inline void Lwo2MeshWriter::getTextureVMaps(vtextures &textures, vvmaps &svmaps, vvmaps &dvmaps)
{
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		lwTexture *texture = textures[i];

		if (texture->type == ID_IMAP && texture->param.imap)
		{
			char *mapname = texture->param.imap->vmap_name;
			if (mapname)
				for (unsigned int v = 0; v < svmaps.size(); v++)
				{
					lwVMap *vmap = svmaps[v];
					if (strcmp(mapname, vmap->name) == 0) dvmaps.push_back(vmap);
				}
		}
	}
	return;
}

bool Lwo2MeshWriter::writeLwo2Mesh(lwObject *nobject, char *ndest)
{
	object = nobject;
	dest = ndest;

	if (!object) return false;
	if (!object->layers.size()) return false;

	prepLwObject();

	vpoints points;
	vpolygons polygons;
	vvmaps vmaps;

	MeshSerializer meshserializer;

	if (flags[ExportMaterials])
	{
		doExportMaterials();
		materialSerializer->exportQueued(makeMaterialFileName(dest));
	}

	unsigned int ml = object->layers.size();

	bool SeparateLayers = flags[UseSeparateLayers] && ml > 1;

    if (!SeparateLayers) ogreMesh = Ogre::MeshManager::getSingleton().create(ndest, NII::GroupDefault);

	Ogre::Vector3 boundingBoxMin(FLT_MAX, FLT_MAX, FLT_MAX);
	Ogre::Vector3 boundingBoxMax(FLT_MIN, FLT_MIN, FLT_MIN);


	for( unsigned int ol = 0; ol < ml; ++ol )
	{
		if (!object->layers[ol]->polygons.size())
			continue;

		Ogre::Vector3 currentMin(object->layers[ol]->bboxmin.x,
            object->layers[ol]->bboxmin.y, object->layers[ol]->bboxmin.z);
		Ogre::Vector3 currentMax(object->layers[ol]->bboxmax.x,
            object->layers[ol]->bboxmax.y, object->layers[ol]->bboxmax.z);

		if (SeparateLayers)
		{
			ogreMesh = NII::MeshManager::getSingleton().create(ndest, NII::GroupDefault);
			ogreMesh->setAABB(NII::AABox(currentMin, currentMax));
            ogreMesh->setRangeFactor(0.01);
			ogreMesh->setCenterRange(NII::Math::sqrt(std::max(currentMin.lengthSquared(), currentMax.lengthSquared())));
		}
		else
		{
			boundingBoxMin = boundingBoxMin.floor(currentMin);
			boundingBoxMax = boundingBoxMax.ceil(currentMax);
		}

		for (unsigned int s = 0; s < object->surfaces.size(); s++)
		{
			lwSurface *surface = object->surfaces[s];

			points.clear();
			polygons.clear();
			vmaps.clear();

			unsigned int l = ol;

			for( unsigned int il = 0; il < ml; ++il )
			{
				if (!SeparateLayers) l = il;

				copyPoints(s, ID_FACE, object->layers[l]->points, points);
				copyPolygons(s, ID_FACE, object->layers[l]->polygons, polygons);
				getTextureVMaps(surface->color.textures, object->layers[l]->vmaps, vmaps);

				if (SeparateLayers) break;
			}

			if(!polygons.size())
                continue;

			SubMesh * ogreSubMesh = ogreMesh->createSub();
			ogreSubMesh->setSelfVertex(!(flags[UseSharedVertexData] && points.size() < POINTLIMIT));

			ogreSubMesh->getIndexData(0)->mCount = polygons.size() * 3;
            N_Only(GBuffer).create(ogreSubMesh->getIndexData(0)->mBuffer, 16, ogreSubMesh->getIndexData(0)->mCount, Buffer::M_NORMAL );
			ogreSubMesh->setMaterial(surface->name);

			if(ogreSubMesh->mSelfVertex)
			{
				ogreSubMesh->setVertexData(setupVertexData(points.size()));
				copyDataToVertexData(points, polygons, vmaps, ogreSubMesh->getIndexData(0), ogreSubMesh->getVertexData());
            }
			else
			{
				unsigned short vertexDataOffset = 0;
				if(ogreMesh->getVertexData())
                    vertexDataOffset = ogreMesh->getVertexData()->mCount;
				ogreMesh->setVertexData(setupVertexData(points.size(), ogreMesh->getVertexData()));
				copyDataToVertexData(points, polygons, vmaps, ogreSubMesh->getIndexData(0), ogreMesh->getVertexData(), vertexDataOffset);
			}
		}

		if (!SeparateLayers)
		{
			ogreMesh->setAABB(NII::AABox(boundingBoxMin, boundingBoxMax));
            ogreMesh->setRangeFactor(0.01);
			ogreMesh->setCenterRange(NII::Math::sqrt(std::max(boundingBoxMin.lengthSquared(), boundingBoxMax.lengthSquared())));
		}

		String fname = SeparateLayers ? makeLayerFileName(dest, ol, object->layers[ol]->name) : dest;

		Skeleton *skeleton = 0;

		if (flags[ExportSkeleton])
			if (SeparateLayers)
				skeleton = doExportSkeleton(fname, ol);
			else
				if (!ol) skeleton = doExportSkeleton(fname, -1);

		if (flags[GenerateLOD])
		{
			ProgressiveMesh::VertexReductionQuota quota;

			if (flags[UseFixedMethod])
				quota = ProgressiveMesh::VRQ_CONSTANT;
			else
				quota = ProgressiveMesh::VRQ_PROPORTIONAL;

			ogreMesh->generateLodLevels(distanceList, quota, reduction);
		}

		if(flags[GenerateEdgeLists])
		{
			ogreMesh->buildEdgeList(0);
		}

		if(flags[GenerateTangents])
		{
			ogreMesh->buildTangentBuffer();
		}

		try
		{
			meshserializer.exportMesh(ogreMesh.getPointer(), fname);
		}
		catch (...)
		{
			cout << "Could not export to file: " << fname << endl;
		}

		ogreMesh->unload();

		Ogre::MeshManager::getSingleton().remove(ogreMesh->getPrc());
		if (flags[ExportSkeleton] && skeleton) delete skeleton;

		if (!SeparateLayers) break;
	}

	return true;
}
