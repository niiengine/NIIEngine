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


#include "Ogre.h"
#include "OgreMeshSerializer.h"
#include "OgreSkeletonSerializer.h"
#include "OgreMemGBufferManager.h"
#include "OgreProgressiveMesh.h"
#include "NiiVertexBuffer.h"

#include <iostream>
#include <sys/stat.h>

using namespace std;
using namespace Ogre;

void help(void)
{
    // Print help message
    cout << endl << "OgreMeshUpgrader: Upgrades or downgrades .mesh file versions." << endl;
    cout << "Provided for OGRE by Steve Streeting 2004-2012" << endl << endl;
    cout << "Usage: OgreMeshUpgrader [opts] sourcefile [destfile] " << endl;
	cout << "-i             = Interactive mode, prompt for options" << endl;
	cout << "-l lodlevels   = number of LOD levels" << endl;
	cout << "-d loddist     = distance increment to reduce LOD" << endl;
	cout << "-p lodpercent  = Percentage triangle reduction amount per LOD" << endl;
	cout << "-f lodnumtris  = Fixed vertex reduction per LOD" << endl;
    cout << "-e         = DON'T generate edge lists (for stencil shadows)" << endl;
    cout << "-t         = Generate tangents (for normal mapping)" << endl;
	cout << "-td [uvw|tangent]" << endl;
	cout << "           = Tangent vertex semantic destination (default tangent)" << endl;
	cout << "-ts [3|4]      = Tangent size (3 or 4 components, 4 includes parity, default 3)" << endl;
	cout << "-tm            = Split tangent vertices at UV mirror points" << endl;
	cout << "-tr            = Split tangent vertices where basis is rotated > 90 degrees" << endl;
	cout << "-r         = DON'T reorganise buffers to recommended format" << endl;
	cout << "-d3d       = Convert to D3D colour formats" << endl;
	cout << "-gl        = Convert to GL colour formats" << endl;
	cout << "-srcd3d    = Interpret ambiguous colours as D3D style" << endl;
	cout << "-srcgl     = Interpret ambiguous colours as GL style" << endl;
	cout << "-E endian  = Set endian mode 'big' 'little' or 'native' (default)" << endl;
	cout << "-b         = Recalculate bounding box (static meshes only)" << endl;
	cout << "-V version = Specify OGRE version format to write instead of latest" << endl;
	cout << "             Options are: 1.8, 1.7, 1.4, 1.0" << endl;
    cout << "sourcefile = name of file to convert" << endl;
    cout << "destfile   = optional name of file to write to. If you don't" << endl;
    cout << "             specify this OGRE overwrites the existing file." << endl;

    cout << endl;
}

struct UpgradeOptions
{
	bool interactive;
	bool suppressEdgeLists;
	bool generateTangents;
	VertexType tangentSemantic;
	bool tangentUseParity;
	bool tangentSplitMirrored;
	bool tangentSplitRotated;
	bool dontReorganise;
	bool destColourFormatSet;
	DataType destColourFormat;
	bool srcColourFormatSet;
	DataType srcColourFormat;
	unsigned short numLods;
	NIIf lodDist;
	NIIf lodPercent;
	size_t lodFixed;
	bool usePercent;
	Serializer::Endian endian;
	bool recalcBounds;
	SerialVer targetVersion;

};


// Crappy globals
// NB some of these are not directly used, but are required to
//   instantiate the singletons used in the dlls
LogManager* logMgr = 0;
Math* mth = 0;
LodStrategyManager* lodMgr = 0;
MaterialManager* matMgr = 0;
SkeletonManager* skelMgr = 0;
MeshSerializer* meshSerializer = 0;
SkeletonSerializer* skeletonSerializer = 0;
MemGBufferManager *bufferManager = 0;
ResourceSchemeManager* rgm = 0;
MeshManager* meshMgr = 0;
UpgradeOptions opts;

void parseOpts(UnaryOptionList& unOpts, BinaryOptionList& binOpts)
{
	opts.interactive = false;
	opts.suppressEdgeLists = false;
	opts.generateTangents = false;
	opts.tangentSemantic = VT_Tangent;
	opts.tangentUseParity = false;
	opts.tangentSplitMirrored = false;
	opts.tangentSplitRotated = false;
	opts.dontReorganise = false;
	opts.endian = Serializer::ENDIAN_NATIVE;
	opts.destColourFormatSet = false;
	opts.srcColourFormatSet = false;

	opts.lodDist = 500;
	opts.lodFixed = 0;
	opts.lodPercent = 20;
	opts.numLods = 0;
	opts.usePercent = true;
	opts.recalcBounds = false;
	opts.targetVersion = 30000;


	UnaryOptionList::iterator ui = unOpts.find("-e");
	opts.suppressEdgeLists = ui->second;
	ui = unOpts.find("-t");
	opts.generateTangents = ui->second;
	ui = unOpts.find("-tm");
	opts.tangentSplitMirrored = ui->second;
	ui = unOpts.find("-tr");
	opts.tangentSplitRotated = ui->second;

	ui = unOpts.find("-i");
	opts.interactive = ui->second;
	ui = unOpts.find("-r");
	opts.dontReorganise = ui->second;
	ui = unOpts.find("-d3d");
	if (ui->second)
	{
		opts.destColourFormatSet = true;
		opts.destColourFormat = DT_Colour_ARGB;
	}
	ui = unOpts.find("-gl");
	if (ui->second)
	{
		opts.destColourFormatSet = true;
		opts.destColourFormat = DT_Colour_ABGR;
	}
	ui = unOpts.find("-srcd3d");
	if (ui->second)
	{
		opts.srcColourFormatSet = true;
		opts.srcColourFormat = DT_Colour_ARGB;
	}
	ui = unOpts.find("-srcgl");
	if (ui->second)
	{
		opts.srcColourFormatSet = true;
		opts.srcColourFormat = DT_Colour_ABGR;
	}
	ui = unOpts.find("-b");
	if (ui->second)
	{
		opts.recalcBounds = true;
	}


	BinaryOptionList::iterator bi = binOpts.find("-l");
	if (!bi->second.empty())
	{
		opts.numLods = StringConverter::parseInt(bi->second);
	}

	bi = binOpts.find("-d");
	if (!bi->second.empty())
	{
		opts.lodDist = StringConverter::parseReal(bi->second);
	}

	bi = binOpts.find("-p");
	if (!bi->second.empty())
	{
		opts.lodPercent = StringConverter::parseReal(bi->second);
		opts.usePercent = true;
	}


	bi = binOpts.find("-f");
	if (!bi->second.empty())
	{
		opts.lodFixed = StringConverter::parseInt(bi->second);
		opts.usePercent = false;
	}

	bi = binOpts.find("-E");
	if (!bi->second.empty())
	{
	    if (bi->second == "big")
            opts.endian = Serializer::ENDIAN_BIG;
	    else if (bi->second == "little")
            opts.endian = Serializer::ENDIAN_LITTLE;
	    else
            opts.endian = Serializer::ENDIAN_NATIVE;
    }
	bi = binOpts.find("-td");
	if (!bi->second.empty())
	{
		if (bi->second == "uvw")
			opts.tangentSemantic = VT_Tex_Coord;
		else // if (bi->second == "tangent"), or anything else
			opts.tangentSemantic = VT_Tangent;
	}
	bi = binOpts.find("-ts");
	if (!bi->second.empty())
	{
		if (bi->second == "4")
			opts.tangentUseParity = true;
	}

	bi = binOpts.find("-V");
	if (!bi->second.empty())
	{
		if (bi->second == "1.8")
			opts.targetVersion = 30000;
		else if (bi->second == "1.7")
			opts.targetVersion = 17000;
		else if (bi->second == "1.4")
			opts.targetVersion = 14000;
		else if (bi->second == "1.0")
			opts.targetVersion = 10000;
		else
			logMgr->stream() << "Unrecognised target mesh version '" << bi->second << "'";
	}

}

String describeSemantic(VertexType sem)
{
	switch (sem)
	{
	case VT_Pos:
		return "Positions";
	case VT_Normals:
		return "Normals";
	case VT_Matrix_Weight:
		return "Blend Weights";
	case VT_Matrix_Index:
		return "Blend Indices";
	case VT_Diffuse:
		return "Diffuse";
	case VT_Specular:
		return "Specular";
	case VT_Tex_Coord:
		return "Texture coordinates";
	case VT_Binormal:
		return "Binormals";
	case VT_Tangent:
		return "Tangents";
	};
    return "";
}

void displayVertexBuffers(VertexUnitList & elemList)
{
	// Iterate per buffer
	unsigned short currentBuffer = 999;
	unsigned short elemNum = 0;
	VertexUnitList::iterator i, iend = elemList.end();
	for(i = elemList.begin(); i != iend; ++i)
	{
		if(i->mSource != currentBuffer)
		{
			currentBuffer = i->mSource;
			cout << "> Buffer " << currentBuffer << ":" << endl;
		}
		cout << "   - Element " << elemNum++ << ": " << describeSemantic(i->mVType);
		if(i->mVType == VT_Tex_Coord)
		{
			cout << " (index " << i->mIndex << ")";
		}
		cout << endl;
	}
}

// Sort routine for VertexUnit
bool vertexElementLess(const VertexUnit & e1, const VertexUnit & e2)
{
	// Sort by source first
	if(e1.mSource < e2.mSource)
	{
		return true;
	}
	else if(e1.getSource() == e2.getSource())
	{
		// Use ordering of semantics to sort
		if(e1.mVType < e2.mVType)
		{
			return true;
		}
		else if(e1.mVType == e2.mVType)
		{
			// Use index to sort
			if(e1.mIndex < e2.mIndex)
			{
				return true;
			}
		}
	}
	return false;
}
void copyElems(VertexData * obj1, VertexUnitList * obj)
{
	obj->clear();
	const VertexUnitList & origElems = obj1->getUnits();
    VertexUnitList::const_iterator i, iend = origElems.end();
	for(i = origElems.begin(); i != iend; ++i)
	{
		obj->push_back(*i);
	}
	obj->sort(vertexElementLess);
}
// Utility function to allow the user to modify the layout of vertex buffers.
void reorganiseVertexBuffers(const String & desc, Mesh & mesh, SubMesh * sm,
    VertexData * vertexData)
{
	cout << endl << desc << ":- " << endl;
	// Copy elements into a list
    VertexUnitList elemList;
	copyElems(vertexData, &elemList);

	bool finish = false;
	bool anyChanges = false;
	while(!finish)
	{
		displayVertexBuffers(elemList);
		cout << endl;

		cout << "Options: (r)ead element" << endl;
		cout << "         (d)elete element" << endl;
		cout << "         (f)inish" << endl;

		String response = "";
		while(response.empty())
		{
			cin >> response;
			StringUtil::toLowerCase(response);

			if(response == "d")
			{
				String moveResp;
				cout << "Which element do you want to delete (type number): ";
				cin >> moveResp;
				if (!moveResp.empty())
				{
					int eindex = StringConverter::parseInt(moveResp);
                    VertexUnitList::iterator movei = elemList.begin();
					std::advance(movei, eindex);
                    cout << std::endl << "Delete element " << eindex << "(" + describeSemantic(movei->mVType) << ")?: ";
					cin >> moveResp;
					StringUtil::toLowerCase(moveResp);
					if (moveResp == "y")
					{
						elemList.erase(movei);
                        anyChanges = true;
					}
				}
			}
			else if (response == "r")
			{
				// reset
				copyElems(vertexData, &elemList);
				anyChanges = false;
			}
			else if (response == "f")
			{
				// finish
				finish = true;
			}
			else
			{
				response == "";
			}

		}
	}

	if (anyChanges)
	{
		String response;
		while (response.empty())
		{
			displayVertexBuffers(elemList);
			cout << "Really reorganise the vertex buffers this way?";
			cin >> response;
			StringUtil::toLowerCase(response);
			if(response == "y")
			{
                vertexData->removeAll();
                VertexUnitList::iterator i, iend = elemList.end();
				for(i = elemList.begin(); i != iend; ++i)
				{
                    vertexData->getUnits().push_back(*i);
				}

                vertexData->build(
                    (mesh.isSkeletonFusion() ? VertexData::M_Skeleton : VertexData::M_None) |
                    (mesh.isVertexFusion() ? VertexData::M_Animation : VertexData::M_None) |
                    (sm ? sm->isNromalsFusion() : mesh.isNromalsFusion()) ?
                        VertexData::M_Normals : VertexData::M_None,
                    Buffer::Buffer::M_DEV | Buffer::M_WRITE, true);
			}
			else if (response == "n")
			{
				// do nothing
			}
			else
			{
				response = "";
			}
        }
	}
}
// Utility function to allow the user to modify the layout of vertex buffers.
void reorganiseVertexBuffers(Mesh & mesh)
{
	// Make sure animation types up to date
	mesh.getAnimationType();//问题

	if(mesh.getVertexData())
	{
		if(opts.interactive)
			reorganiseVertexBuffers("Shared Geometry", mesh, 0, mesh.getVertexData());
		else
		{
			// Automatic
            mesh.getVertexData()->build(
                (mesh.isSkeletonFusion() ? VertexData::M_Skeleton : VertexData::M_None) |
                (mesh.isVertexFusion() ? VertexData::M_Animation : VertexData::M_None) |
                (mesh.isNromalsFusion() ? VertexData::M_Normals : VertexData::M_None),
                Buffer::Buffer::M_DEV | Buffer::M_WRITE, true);
		}
	}

    SubMeshList::iterator i, iend = mesh.getSubList().end();
	unsigned short idx = 0;
    for(i = mesh.getSubList().begin(); i != iend; ++i)
	{
		SubMesh * sm = *i;
		if(sm->isSelfVertex())
		{
			if(opts.interactive)
			{
				StringUtil::StrStreamType str;
				str << "SubMesh " << idx++;
				reorganiseVertexBuffers(str.str(), mesh, sm, sm->getVertexData());
			}
			else
			{
				const bool hasVertexAnim = sm->getAnimationType() != NII::AM_None;//问题

				// Automatic
                sm->getVertexData()->build(
                    (mesh.isSkeletonFusion() ? VertexData::M_Skeleton : VertexData::M_None) |
                    (hasVertexAnim ? VertexData::M_Animation : VertexData::M_None) |
                    (sm->isNromalsFusion() ? VertexData::M_Normals : VertexData::M_None),
                    Buffer::Buffer::M_DEV | Buffer::M_WRITE, true);
			}
		}
	}
}


void vertexBufferReorg(Mesh& mesh)
{
	String response;

	if (opts.interactive)
	{

		// Check to see whether we would like to reorganise vertex buffers
		std::cout << "\nWould you like to reorganise the vertex buffers for this mesh?";
		while (response.empty())
		{
			cin >> response;
			StringUtil::toLowerCase(response);
			if (response == "y")
			{
				reorganiseVertexBuffers(mesh);
			}
			else if (response == "n")
			{
				// Do nothing
			}
			else
			{
				response = "";
			}
		}
	}
	else if (!opts.dontReorganise)
	{
		reorganiseVertexBuffers(mesh);
	}

}

void buildLod(Mesh* mesh)
{
	String response;

	// Prompt for LOD generation?
	bool genLod = false;
	bool askLodDtls = false;
	if (!opts.interactive) // derive from params if in batch mode
	{
		askLodDtls = false;
		if (opts.numLods == 0)
		{
			genLod = false;
		}
		else
		{
			genLod = true;
		}
	}
	else if(opts.numLods == 0) // otherwise only ask if not specified on command line
	{
		if (mesh->getLodCount() > 1)
		{
			std::cout << "\nXML already contains level-of detail information.\n"
				"Do you want to: (u)se it, (r)eplace it, or (d)rop it?";
			while (response == "")
			{
				cin >> response;
				StringUtil::toLowerCase(response);
				if (response == "u")
				{
					// Do nothing
				}
				else if (response == "d")
				{
					mesh->removeAllLod();
				}
				else if (response == "r")
				{
					genLod = true;
					askLodDtls = true;

				}
				else
				{
					response = "";
				}
			}// while response == ""
		}
		else // no existing LOD
		{
			std::cout << "\nWould you like to generate LOD information? (y/n)";
			while (response == "")
			{
				cin >> response;
				StringUtil::toLowerCase(response);
				if (response == "n")
				{
					// Do nothing
				}
				else if (response == "y")
				{
					genLod = true;
					askLodDtls = true;
				}
			}
		}
	}

	if (genLod)
	{
		unsigned short numLod;
		ProgressiveMesh::VertexReductionQuota quota;
		NIIf reduction;
		vector<NIIf>::type distanceList;

		if (askLodDtls)
		{
			cout << "\nHow many extra LOD levels would you like to generate?";
			cin >> numLod;

			cout << "\nWhat unit of reduction would you like to use:" <<
				"\n(f)ixed or (p)roportional?";
			cin >> response;
			StringUtil::toLowerCase(response);
			if (response == "f")
			{
				quota = ProgressiveMesh::VRQ_CONSTANT;
				cout << "\nHow many vertices should be removed at each LOD?";
			}
			else
			{
				quota = ProgressiveMesh::VRQ_PROPORTIONAL;
				cout << "\nWhat percentage of remaining vertices should be removed "
					"\at each LOD (e.g. 50)?";
			}
			cin >> reduction;
			if (quota == ProgressiveMesh::VRQ_PROPORTIONAL)
			{
				// Percentage -> parametric
				reduction = reduction * 0.01f;
			}

			cout << "\nEnter the distance for each LOD to come into effect.";

			NIIf distance;
			for (unsigned short iLod = 0; iLod < numLod; ++iLod)
			{
				cout << "\nLOD Level " << (iLod+1) << ":";
				cin >> distance;
				distanceList.push_back(distance);
			}
		}
		else
		{
			numLod = opts.numLods;
			quota = opts.usePercent?
				ProgressiveMesh::VRQ_PROPORTIONAL : ProgressiveMesh::VRQ_CONSTANT;
			if (opts.usePercent)
			{
				reduction = opts.lodPercent * 0.01f;
			}
			else
			{
				reduction = opts.lodFixed;
			}
			NIIf currDist = 0;
			for (unsigned short iLod = 0; iLod < numLod; ++iLod)
			{
				currDist += opts.lodDist;
                NIIf currDistSq = Ogre::Math::Sqr(currDist);
				distanceList.push_back(currDistSq);
			}

		}

		ProgressiveMesh::generateLodLevels(mesh, distanceList, quota, reduction);
	}

}

void checkColour(VertexData * vdata, bool & hasColour, bool & hasAmbiguousColour,
	DataType & originalType)
{
	const VertexUnitList & elemList = vdata->getUnits();
	for(VertexUnitList::const_iterator i = elemList.begin(); i != elemList.end(); ++i)
	{
		const VertexUnit & elem = *i;
		switch(elem.mType)
		{
		case DT_Colour:
			hasAmbiguousColour = true;
		case DT_Colour_ABGR:
		case DT_Colour_ARGB:
			hasColour = true;
			originalType = elem.mType;
		default:
			// do nothing
			;
		};
	}
}

void resolveColourAmbiguities(Mesh * mesh)
{
	// Check what we're dealing with
	bool hasColour = false;
	bool hasAmbiguousColour = false;
	DataType originalType = DT_Float1x;
	if(mesh->getVertexData())
	{
		checkColour(mesh->getVertexData(), hasColour, hasAmbiguousColour, originalType);
	}
	for(unsigned short i = 0; i < mesh->getSubCount(); ++i)
	{
		SubMesh * sm = mesh->getSub(i);
		if(sm->isSelfVertex())
		{
			checkColour(sm->getVertexData(), hasColour, hasAmbiguousColour, originalType);
		}
	}

	String response;
	if (hasAmbiguousColour)
	{
		if (opts.srcColourFormatSet)
		{
			originalType = opts.srcColourFormat;
		}
		else
		{
			// unknown input colour, have to ask
			std::cout << "\nYour mesh has vertex colours but I don't know whether they were generated\n"
				<< "using GL or D3D ordering. Please indicate which was used when the mesh was\n"
				<< "created (type 'gl' or 'd3d').\n";
			while (response.empty())
			{
				cin >> response;
				StringUtil::toLowerCase(response);
				if (response == "d3d")
				{
					originalType = DT_Colour_ARGB;
				}
				else if (response == "gl")
				{
					originalType = DT_Colour_ABGR;
				}
				else
				{
					response = "";
				}
			}
		}
	}

	// Ask what format we want to save in
	DataType desiredType = DT_Float1x;
	if (hasColour)
	{
		if (opts.destColourFormatSet)
		{
			desiredType = opts.destColourFormat;
		}
		else
		{
			if (opts.interactive)
			{

				response = "";
				std::cout << "\nYour mesh has vertex colours, which can be stored in one of two layouts,\n"
					<< "each of which will be slightly faster to load in a different render system.\n"
					<< "Do you want to prefer Direct3D (d3d) or OpenGL (gl)?\n";
				while (response.empty())
				{
					cin >> response;
					StringUtil::toLowerCase(response);
					if (response == "d3d")
					{
						desiredType = DT_Colour_ARGB;
					}
					else if (response == "gl")
					{
						desiredType = DT_Colour_ABGR;
					}
					else
					{
						response = "";
					}
				}
			}
			else
			{
				// 'do no harm'
				return;
			}
		}
	}

	if(mesh->getVertexData() && hasColour)
	{
		VertexUnit::conv(mesh->getVertexData(), originalType, desiredType);
	}
	for(unsigned short i = 0; i < mesh->getSubCount(); ++i)
	{
		SubMesh * sm = mesh->getSub(i);
		if(sm->isSelfVertex() && hasColour)
		{
			VertexUnit::conv(sm->getVertexData(), originalType, desiredType);
		}
	}
}

void recalcBounds(const VertexData * vdata, AABox & aabb, NIIf & radius)
{
	const VertexUnit * posElem = vdata->get(VT_Pos);

	const VertexBuffer * buf = vdata->getBuffer(posElem->getSource());
	void * pBase = buf->lock(Buffer::MM_READ);

	for(size_t v = 0; v < vdata->mCount; ++v)
	{
		float * pFloat;
		posElem->getData(pBase, pFloat);

		Vector3 pos(pFloat[0], pFloat[1], pFloat[2]);
		aabb.merge(pos);
		radius = std::max(radius, pos.length());

		pBase = static_cast<void *>(static_cast<char *>(pBase) + buf->getUnitSize());

	}
	buf->unlock();
}

void recalcBounds(Mesh * mesh)
{
	AABox aabb;
	NIIf radius = 0.0f;

	if(mesh->getVertexData())
		recalcBounds(mesh->getVertexData(), aabb, radius);

	for(NCount i = 0; i < mesh->getSubCount(); ++i)
	{
		SubMesh * sm = mesh->getSub(i);
		if(sm->isSelfVertex())
			recalcBounds(sm->getVertexData(), aabb, radius);
	}

	mesh->setAABB(aabb);
	mesh->setCenterRange(radius);
}

int main(int numargs, char** args)
{
    if(numargs < 2)
    {
        help();
        return -1;
    }

	int retCode = 0;
	try
	{
		logMgr = new LogManager();
		logMgr->createLog("OgreMeshUpgrade.log", true);
		rgm = new ResourceSchemeManager();
		mth = new Math();
		lodMgr = new LodStrategyManager();
		matMgr = new MaterialManager();
		matMgr->init();
		skelMgr = new SkeletonManager();
		meshSerializer = new MeshSerializer();
		skeletonSerializer = new SkeletonSerializer();
		bufferManager = new MemGBufferManager(); // needed because we don't have a rendersystem
		meshMgr = new MeshManager();

		UnaryOptionList unOptList;
		BinaryOptionList binOptList;

		unOptList["-i"] = false;
		unOptList["-e"] = false;
		unOptList["-t"] = false;
		unOptList["-tm"] = false;
		unOptList["-tr"] = false;
		unOptList["-r"] = false;
		unOptList["-gl"] = false;
		unOptList["-d3d"] = false;
		unOptList["-srcgl"] = false;
		unOptList["-srcd3d"] = false;
		unOptList["-b"] = false;
		binOptList["-l"] = "";
		binOptList["-d"] = "";
		binOptList["-p"] = "";
		binOptList["-f"] = "";
		binOptList["-E"] = "";
		binOptList["-td"] = "";
		binOptList["-ts"] = "";
		binOptList["-V"] = "";

		int startIdx = findCommandLineOpts(numargs, args, unOptList, binOptList);
		parseOpts(unOptList, binOptList);

		String source(args[startIdx]);

		// Load the mesh
		struct stat tagStat;

		FILE * pFile = fopen( source.c_str(), "rb" );
		if(!pFile)
		{
			N_EXCEPT(Exception::ERR_FILE_NOT_FOUND,
				"File " + source + " not found.", "OgreMeshUpgrade");
		}
		stat(source.c_str(), &tagStat);
		MemStream * memstream = N_new MemStream(source, tagStat.st_size, true);
		fread((void *)memstream->getData(), tagStat.st_size, 1, pFile );
		fclose(pFile);

		Mesh mesh(meshMgr, "conversion", 0, GroupDefault);
        mesh.setRangeFactor(0.0f);
		meshSerializer->importMesh(memstream, &mesh);
        N_delete memstream;
		// Write out the converted mesh
		String dest;
		if (numargs == startIdx + 2)
		{
			dest = args[startIdx + 1];
		}
		else
		{
			dest = source;
		}

		String response;

		vertexBufferReorg(mesh);

		// Deal with DT_Colour ambiguities
		resolveColourAmbiguities(&mesh);

		buildLod(&mesh);

		// Make sure we generate edge lists, provided they are not deliberately disabled
		if(!opts.suppressEdgeLists)
		{
			cout << "\nGenerating edge lists.." << std::endl;
			mesh.buildEdgeList(0);
		}
		else
		{
			mesh.freeEdgeList();
		}

		// Generate tangents?
		if(opts.generateTangents)
		{
			unsigned short srcTex, destTex;
            bool coordtype = (opts.tangentSemantic == VT_Tex_Coord) ? true : false;
			bool existing = mesh.checkTangentBuffer(coordtype);
			if(existing)
			{
				if(opts.interactive)
				{
					std::cout << "\nThis mesh appears to already have a set of tangents, " <<
						"which would suggest tangent vectors have already been calculated. Do you really " <<
						"want to generate new tangent vectors (may duplicate)? (y/n)";
					while (response == "")
					{
						cin >> response;
						StringUtil::toLowerCase(response);
						if (response == "y")
						{
							// Do nothing
						}
						else if (response == "n")
						{
							opts.generateTangents = false;
						}
						else
						{
							response = "";
						}
					}
				}
				else
				{
					// safe
					opts.generateTangents = false;
				}
			}
			if(opts.generateTangents)
			{
				cout << "Generating tangent vectors...." << std::endl;
				mesh.buildTangentBuffer(opts.tangentSemantic, opts.tangentUseParity,
					opts.tangentSplitMirrored, opts.tangentSplitRotated);
			}
		}

		if(opts.recalcBounds)
			recalcBounds(&mesh);

		meshSerializer->exportMesh(&mesh, dest, opts.targetVersion, opts.endian);

	}
	catch(Exception & e)
	{
		cout << "Exception caught: " << e.getVerbose();
		retCode = 1;
	}

    delete meshMgr;
    delete skeletonSerializer;
    delete meshSerializer;
    delete skelMgr;
    delete matMgr;
	delete lodMgr;
    delete mth;
    delete rgm;
    delete logMgr;

    return retCode;
}