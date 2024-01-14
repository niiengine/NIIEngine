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
#include "OgreXMLMeshSerializer.h"
#include "OgreMeshSerializer.h"
#include "OgreXMLSkeletonSerializer.h"
#include "OgreSkeletonSerializer.h"
#include "OgreXMLPrerequisites.h"
#include "NiiMemGBufferManager.h"
#include "OgreProgressiveMesh.h"
#include <iostream>
#include <sys/stat.h>

using namespace std;
using namespace Ogre;

struct XmlOptions
{
    String source;
    String dest;
    String sourceExt;
    String destExt;
    String logFile;
    bool interactiveMode;
    unsigned short numLods;
    NIIf lodValue;
    String lodStrategy;
    NIIf lodPercent;
    size_t lodFixed;
    size_t nuextremityPoints;
    bool usePercent;
    bool generateEdgeLists;
    bool generateTangents;
	VertexType tangentSemantic;
	bool tangentUseParity;
	bool tangentSplitMirrored;
	bool tangentSplitRotated;
    bool reorganiseBuffers;
	bool optimiseAnimations;
	bool quietMode;
	bool d3d;
	bool gl;
	Serializer::Endian endian;
};

void help(void)
{
    // Print help message
    cout << endl << "OgreXMLConvert: Converts data between XML and OGRE binary formats." << endl;
    cout << "Provided for OGRE by Steve Streeting" << endl << endl;
    cout << "Usage: OgreXMLConverter [options] sourcefile [destfile] " << endl;
	cout << endl << "Available options:" << endl;
    cout << "-i             = interactive mode - prompt for options" << endl;
    cout << "(The next 4 options are only applicable when converting XML to Mesh)" << endl;
    cout << "-l lodlevels   = number of LOD levels" << endl;
    cout << "-v lodvalue     = value increment to reduce LOD" << endl;
    cout << "-s lodstrategy = LOD strategy to use for this mesh" << endl;
    cout << "-p lodpercent  = Percentage triangle reduction amount per LOD" << endl;
    cout << "-f lodnumtris  = Fixed vertex reduction per LOD" << endl;
    cout << "-e             = DON'T generate edge lists (for stencil shadows)" << endl;
    cout << "-r             = DON'T reorganise vertex buffers to OGRE recommended format." << endl;
    cout << "-t             = Generate tangents (for normal mapping)" << endl;
	cout << "-td [uvw|tangent]" << endl;
	cout << "           = Tangent vertex semantic destination (default tangent)" << endl;
	cout << "-ts [3|4]      = Tangent size (3 or 4 components, 4 includes parity, default 3)" << endl;
	cout << "-tm            = Split tangent vertices at UV mirror points" << endl;
	cout << "-tr            = Split tangent vertices where basis is rotated > 90 degrees" << endl;
    cout << "-o             = DON'T optimise out redundant tracks & keyframes" << endl;
	cout << "-d3d           = Prefer D3D packed colour formats (default on Windows)" << endl;
	cout << "-gl            = Prefer GL packed colour formats (default on non-Windows)" << endl;
	cout << "-E endian      = Set endian mode 'big' 'little' or 'native' (default)" << endl;
	cout << "-x num         = Generate no more than num eXtremes for every submesh (default 0)" << endl;
	cout << "-q             = Quiet mode, less output" << endl;
    cout << "-log filename  = name of the log file (default: 'OgreXMLConverter.log')" << endl;
    cout << "sourcefile     = name of file to convert" << endl;
    cout << "destfile       = optional name of file to write to. If you don't" << endl;
    cout << "                 specify this OGRE works it out through the extension " << endl;
    cout << "                 and the XML contents if the source is XML. For example" << endl;
    cout << "                 test.mesh becomes test.xml, test.xml becomes test.mesh " << endl;
    cout << "                 if the XML document root is <mesh> etc."  << endl;

    cout << endl;
}


XmlOptions parseArgs(int numArgs, char **args)
{
    XmlOptions opts;

    opts.interactiveMode = false;
    opts.lodValue = 250000;
    opts.lodFixed = 0;
    opts.lodPercent = 20;
    opts.numLods = 0;
    opts.nuextremityPoints = 0;
    opts.usePercent = true;
    opts.generateEdgeLists = true;
    opts.generateTangents = false;
	opts.tangentSemantic = VT_Tangent;
	opts.tangentUseParity = false;
	opts.tangentSplitMirrored = false;
	opts.tangentSplitRotated = false;
    opts.reorganiseBuffers = true;
	opts.optimiseAnimations = true;
    opts.quietMode = false;
	opts.endian = Serializer::ENDIAN_NATIVE;

    // ignore program name
    char * source = 0;
    char * dest = 0;

    // Set up options
    UnaryOptionList unOpt;
    BinaryOptionList binOpt;

    unOpt["-i"] = false;
    unOpt["-e"] = false;
    unOpt["-r"] = false;
    unOpt["-t"] = false;
	unOpt["-tm"] = false;
	unOpt["-tr"] = false;
    unOpt["-o"] = false;
	unOpt["-q"] = false;
	unOpt["-d3d"] = false;
	unOpt["-gl"] = false;
	unOpt["-h"] = false;
    binOpt["-l"] = "";
    binOpt["-v"] = "";
    binOpt["-s"] = "Distance";
    binOpt["-p"] = "";
    binOpt["-f"] = "";
    binOpt["-E"] = "";
    binOpt["-x"] = "";
    binOpt["-log"] = "OgreXMLConverter.log";
	binOpt["-td"] = "";
	binOpt["-ts"] = "";

    int startIndex = findCommandLineOpts(numArgs, args, unOpt, binOpt);
    UnaryOptionList::iterator ui;
    BinaryOptionList::iterator bi;

    ui = unOpt.find("-h");
	if (ui->second)
	{
		help();
        exit(1);
	}

	ui = unOpt.find("-q");
	if (ui->second)
	{
		opts.quietMode = true;
	}

	ui = unOpt.find("-i");
    if (ui->second)
    {
        opts.interactiveMode = true;
    }
    else
    {
        ui = unOpt.find("-e");
        if (ui->second)
        {
            opts.generateEdgeLists = false;
        }

        ui = unOpt.find("-r");
        if (ui->second)
        {
            opts.reorganiseBuffers = false;
        }

        ui = unOpt.find("-t");
        if (ui->second)
        {
            opts.generateTangents = true;
        }
		ui = unOpt.find("-tm");
		if (ui->second)
		{
			opts.tangentSplitMirrored = true;
		}
		ui = unOpt.find("-tr");
		if(ui->second)
		{
			opts.tangentSplitRotated = true;
		}

		bi = binOpt.find("-td");
		if(!bi->second.empty())
		{
			if(bi->second == "uvw")
				opts.tangentSemantic = VT_Tex_Coord;
			else
				opts.tangentSemantic = VT_Tangent;
		}
		bi = binOpt.find("-ts");
		if (!bi->second.empty())
		{
			if (bi->second == "4")
				opts.tangentUseParity = true;
		}

        ui = unOpt.find("-o");
        if (ui->second)
        {
            opts.optimiseAnimations = false;
        }

		bi = binOpt.find("-l");
        if (!bi->second.empty())
        {
            opts.numLods = StringConverter::parseInt(bi->second);
        }

        bi = binOpt.find("-v");
        if (!bi->second.empty())
        {
            opts.lodValue = StringConverter::parseReal(bi->second);
        }

        bi = binOpt.find("-s");
        if (!bi->second.empty())
        {
            opts.lodStrategy = bi->second;
        }

        bi = binOpt.find("-p");
        if (!bi->second.empty())
        {
            opts.lodPercent = StringConverter::parseReal(bi->second);
            opts.usePercent = true;
        }


        bi = binOpt.find("-f");
        if (!bi->second.empty())
        {
            opts.lodFixed = StringConverter::parseInt(bi->second);
            opts.usePercent = false;
        }

        bi = binOpt.find("-x");
        if (!bi->second.empty())
        {
            opts.nuextremityPoints = StringConverter::parseInt(bi->second);
        }

        bi = binOpt.find("-log");
        if (!bi->second.empty())
        {
            opts.logFile = bi->second;
        }

        bi = binOpt.find("-E");
        if (!bi->second.empty())
        {
            if (bi->second == "big")
                opts.endian = Serializer::ENDIAN_BIG;
            else if (bi->second == "little")
                opts.endian = Serializer::ENDIAN_LITTLE;
            else
                opts.endian = Serializer::ENDIAN_NATIVE;
        }

		ui = unOpt.find("-d3d");
		if (ui->second)
		{
			opts.d3d = true;
		}

		ui = unOpt.find("-gl");
		if (ui->second)
		{
			opts.gl = true;
			opts.d3d = false;
		}

	}
    // Source / dest
    if (numArgs > startIndex)
        source = args[startIndex];
    if (numArgs > startIndex+1)
        dest = args[startIndex+1];
    if (numArgs > startIndex+2) {
        cout << "Too many command-line arguments supplied - abort. " << endl;
        help();
        exit(1);
    }

    if (!source)
    {
        cout << "Missing source file - abort. " << endl;
        help();
        exit(1);
    }
    // Work out what kind of conversion this is
    opts.source = source;
	Ogre::StringList srcparts = StringUtil::split(opts.source, ".");
    String& ext = srcparts.back();
	StringUtil::toLowerCase(ext);
    opts.sourceExt = ext;

    if (!dest)
    {
        if (opts.sourceExt == "xml")
        {
            // dest is source minus .xml
            opts.dest = opts.source.substr(0, opts.source.size() - 4);
        }
        else
        {
            // dest is source + .xml
            opts.dest = opts.source;
            opts.dest.append(".xml");
        }

    }
    else
    {
        opts.dest = dest;
    }
	Ogre::StringList dstparts = StringUtil::split(opts.dest, ".");
    ext = dstparts.back();
	StringUtil::toLowerCase(ext);
    opts.destExt = ext;

    if(!opts.quietMode)
	{
        cout << endl;
        cout << "-- OPTIONS --" << endl;
        cout << "source file      = " << opts.source << endl;
        cout << "destination file = " << opts.dest << endl;
            cout << "log file         = " << opts.logFile << endl;
        cout << "interactive mode = " << StrConv::conv(opts.interactiveMode) << endl;
        if (opts.numLods == 0)
        {
            cout << "lod levels       = none (or use existing)" << endl;
        }
        else
        {
            cout << "lod levels       = " << opts.numLods << endl;
            cout << "lod value     = " << opts.lodValue << endl;
            cout << "lod strategy     = " << opts.lodStrategy << endl;
            if (opts.usePercent)
            {
                cout << "lod reduction    = " << opts.lodPercent << "%" << endl;
            }
            else
            {
                cout << "lod reduction    = " << opts.lodFixed << " verts" << endl;
            }
        }
        if (opts.nuextremityPoints)
            cout << "Generate extremes per submesh = " << opts.nuextremityPoints << endl;
        cout << "Generate edge lists  = " << opts.generateEdgeLists << endl;
        cout << "Generate tangents = " << opts.generateTangents << endl;
		cout << " semantic = " << (opts.tangentSemantic == VT_Tangent ? "TANGENT" : "TEXCOORD") << endl;
		cout << " parity = " << opts.tangentUseParity << endl;
		cout << " split mirror = " << opts.tangentSplitMirrored << endl;
		cout << " split rotated = " << opts.tangentSplitRotated << endl;
        cout << "Reorganise vertex buffers = " << opts.reorganiseBuffers << endl;
    	cout << "Optimise animations = " << opts.optimiseAnimations << endl;

        cout << "-- END OPTIONS --" << endl;
        cout << endl;
    }


    return opts;
}

// Crappy globals
// NB some of these are not directly used, but are required to
//   instantiate the singletons used in the dlls
LogManager* logMgr = 0;
Math* mth = 0;
LodStrategyManager *lodMgr = 0;
MaterialManager* matMgr = 0;
SkeletonManager* skelMgr = 0;
MeshSerializer* meshSerializer = 0;
XMLMeshSerializer* xmlMeshSerializer = 0;
SkeletonSerializer* skeletonSerializer = 0;
XMLSkeletonSerializer* xmlSkeletonSerializer = 0;
MemGBufferManager *bufferManager = 0;
MeshManager* meshMgr = 0;
ResourceSchemeManager* rgm = 0;

void meshToXML(XmlOptions opts)
{
    std::ifstream ifs;
    ifs.open(opts.source.c_str(), std::ios_base::in | std::ios_base::binary);

    if(ifs.bad())
    {
        cout << "Unable to load file " << opts.source << endl;
        exit(1);
    }

    // pass false for freeOnClose to FileDataStream since ifs is created on stack
    DataStream * stream = N_new FileDataStream(opts.source, &ifs, false);

    MeshPtr mesh = MeshManager::getSingleton().create("conversion", GroupDefault);

    meshSerializer->importMesh(stream, mesh.getPointer());

    N_delete stream;
    xmlMeshSerializer->exportMesh(mesh.getPointer(), opts.dest);
}

void XMLToBinary(XmlOptions opts)
{
    // Read root element and decide from there what type
    String response;
    TiXmlDocument* doc = new TiXmlDocument(opts.source);
    // Some double-parsing here but never mind
    if (!doc->LoadFile())
    {
        cout << "Unable to open file " << opts.source << " - fatal error." << endl;
        delete doc;
        exit (1);
    }
    TiXmlElement* root = doc->RootElement();
    if (!stricmp(root->Value(), "mesh"))
    {
        delete doc;
        MeshPtr newMesh = N_Only(Mesh).createManual("conversion", GroupDefault);
		DataType colourElementType;
		if (opts.d3d)
			colourElementType = DT_Colour_ARGB;
		else
			colourElementType = DT_Colour_ABGR;

        xmlMeshSerializer->importMesh(opts.source, colourElementType, newMesh.getPointer());

        // Re-jig the buffers?
		// Make sure animation types are up to date first
		newMesh->getAnimationType(); //问题
        if(opts.reorganiseBuffers)
        {
            logMgr->logMessage("Reorganising vertex buffers to automatic layout..");
            // Shared geometry
            if(newMesh->getVertexData())
            {
                // Automatic
                newMesh->getVertexData()->build(
                    (newMesh->isSkeletonFusion() ? VertexData::M_Skeleton: VertexData::M_None) |
                    (newMesh->isVertexFusion() ? VertexData::M_Animation : VertexData::M_None) |
                    (newMesh->isNromalsFusion() ? VertexData::M_Normals : VertexData::M_None),
                    Buffer::Buffer::M_DEV | Buffer::M_WRITE, true);
            }
            // Dedicated geometry
            SubMeshList::iterator sui, suiend = newMesh->getSubList().end();
            for(sui = newMesh->getSubList().begin(); sui != suiend; ++sui)
            {
                SubMesh * sm = *sui;
                if(sm->isSelfVertex())
                {
					const bool hasVertexAnim = sm->getAnimationType() != NII::AM_None; //问题
                    // Automatic
                    sm->getVertexData()->build(
                        (newMesh->isSkeletonFusion() ? VertexData::M_Skeleton : VertexData::M_None) |
                            (hasVertexAnim ? VertexData::M_Animation : VertexData::M_None) |
                                (sm->isNromalsFusion() ? VertexData:: : VertexData::M_None),
                                    Buffer::Buffer::M_DEV | Buffer::M_WRITE, true);
                }
            }

        }

        // Prompt for LOD generation?
        bool genLod = false;
        bool askLodDtls = false;
        if (!opts.interactiveMode) // derive from params if in batch mode
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
            if (newMesh->getLodCount() > 1)
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
                        newMesh->removeAllLod();
                    }
                    else if (response == "r")
                    {
                        genLod = true;
                        askLodDtls = true;

                    }
                    else
                    {
                        std::cout << "Did not understand \"" << response << "\" please try again:" << std::endl;
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
                    else
                    {
                        std::cout << "Did not understand \"" << response << "\" please try again:" << std::endl;
                        response = "";
                    }
                }
            }
        }

        if (genLod)
        {
            unsigned short numLod;
            ProgressiveMesh::VertexReductionQuota quota = ProgressiveMesh::VRQ_PROPORTIONAL;
            NIIf reduction;
            vector<NIIf>::type valueList;

            if (askLodDtls)
            {
                cout << "\nHow many extra LOD levels would you like to generate?";
                cin >> numLod;

                cout << "\nWhat lod strategy should be used?";
                cin >> opts.lodStrategy;

                cout << "\nWhat unit of reduction would you like to use:" <<
                    "\n(f)ixed or (p)roportional?";
                response = "";
                while (response == "") {
                    cin >> response;
                    StringUtil::toLowerCase(response);
                    if (response == "f")
                    {
                        quota = ProgressiveMesh::VRQ_CONSTANT;
                        cout << "\nHow many vertices should be removed at each LOD?";
                    }
                    else if (response == "p")
                    {
                        quota = ProgressiveMesh::VRQ_PROPORTIONAL;
                        cout << "\nWhat percentage of remaining vertices should be removed "
                            "\at each LOD (e.g. 50)?";
                    }
                    else {
                            std::cout << "Did not understand \"" << response << "\" please try again:" << std::endl;
                            response = "";
                    }
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
                    valueList.push_back(distance);
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
                    currDist += opts.lodValue;
                    NIIf currDistSq = Ogre::Math::Sqr(currDist);
                    valueList.push_back(currDistSq);
                }

            }

            newMesh->set(N_Only(LodStrategy).get(opts.lodStrategy));
			ProgressiveMesh::generateLodLevels(newMesh.get(), valueList, quota, reduction);
        }

        if (opts.interactiveMode)
        {
            std::cout << "\nWould you like to include edge lists to enable stencil shadows with this mesh? (y/n)";
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
                    opts.generateEdgeLists = false;
                }
                else
                {
                    std::cout << "Did not understand \"" << response << "\" please try again:" << std::endl;
                    response = "";
                }
            }


            std::cout << "\nWould you like to generate tangents to enable normal mapping with this mesh? (y/n)";
            while (response == "")
            {
                cin >> response;
                StringUtil::toLowerCase(response);
                if (response == "y")
                {
                    opts.generateTangents = true;
                }
                else if (response == "n")
                {
                    // Do nothing
                }
                else
                {
                    std::cout << "Did not understand \"" << response << "\" please try again:" << std::endl;
                    response = "";
                }
            }
        }

        if (opts.generateEdgeLists)
        {
            if (!opts.quietMode)
			{
                std::cout << "Generating edge lists...." << std::endl;
            }
            newMesh->buildEdgeList(0);
        }

        if (opts.generateTangents)
        {
            unsigned short srcTex, destTex;
            if (!opts.quietMode)
            {
                std::cout << "Checking if we already have tangent vectors...." << std::endl;
            }
            bool existing;
            try
            {
                bool coordtype = (opts.tangentSemantic == VT_Tex_Coord) ? true : false;
                existing = newMesh->checkTangentBuffer(coordtype);
            }
            catch (Exception & e)
            {
                std::cerr << "WARNING: While checking for existing tangents: " << e.getFullDescription() << std::endl;
                std::cerr << "NOTE: Tangents were NOT generated for this mesh!" << std::endl;
                existing = false;
                opts.generateTangents = false;
            }
            if(existing)
            {
                std::cout << "\nThis mesh appears to already have a set of 3D texture coordinates, " <<
                    "which would suggest tangent vectors have already been calculated. Do you really " <<
                    "want to generate new tangent vectors (may duplicate)? (y/n): ";
                while(response == "")
                {
                    cin >> response;
                    StringUtil::toLowerCase(response);
                    if(response == "y")
                    {
                        // Do nothing
                    }
                    else if(response == "n")
                    {
                        opts.generateTangents = false;
                    }
                    else
                    {
                        std::cout << "Did not understand \"" << response << "\" please try again:" << std::endl;
                        response = "";
                    }
                }
            }
            if(opts.generateTangents)
            {
                if(!opts.quietMode)
				{
                    std::cout << "Generating tangent vectors...." << std::endl;
                }
                newMesh->buildTangentBuffer(opts.tangentSemantic, opts.tangentUseParity,
					opts.tangentSplitMirrored, opts.tangentSplitRotated);
            }
        }

        if(opts.nuextremityPoints)
        {
            SubMeshList::iterator su, suend = newMesh->getSubList().end();
            for(su = newMesh->getSubList().begin(); su != suend; ++su)
            {
                (*su)->buildEdge(opts.nuextremityPoints);
            }
        }
        meshSerializer->exportMesh(newMesh.getPointer(), opts.dest, opts.endian);
    }
    else if(!stricmp(root->Value(), "skeleton"))
    {
        delete doc;
        SkeletonPtr newSkel = SkeletonManager::getSingleton().create("conversion", GroupDefault);
        xmlSkeletonSerializer->importSkeleton(opts.source, newSkel.getPointer());
		if (opts.optimiseAnimations)
		{
			newSkel->optimise();
		}
        skeletonSerializer->exportSkeleton(newSkel.getPointer(), opts.dest, 30000, opts.endian);
    }
    else
    {
        delete doc;
    }
}

void skeletonToXML(XmlOptions opts)
{
    std::ifstream ifs;
    ifs.open(opts.source.c_str(), std::ios_base::in | std::ios_base::binary);
    if (ifs.bad())
	{
		cout << "Unable to load file " << opts.source << endl;
		exit(1);
	}

    SkeletonPtr skel = SkeletonManager::getSingleton().create("conversion", GroupDefault);

    // pass false for freeOnClose to FileDataStream since ifs is created locally on stack
    DataStream * stream = N_new FileDataStream(opts.source, &ifs, false);
    skeletonSerializer->importSkeleton(stream, skel.getPointer());
    N_delete stream;
    xmlSkeletonSerializer->exportSkeleton(skel.getPointer(), opts.dest);
}

int main(int numargs, char ** args)
{
    if(numargs < 2)
    {
        help();
        return -1;
    }

	// Assume success
	int retCode = 0;

	try
	{
		logMgr = new LogManager();

        // this log catches output from the parseArgs call and routes it to stdout only
		logMgr->createLog("Temporary log", false, true, true);

		XmlOptions opts = parseArgs(numargs, args);
        // use the log specified by the cmdline params
        logMgr->setDefaultLog(logMgr->createLog(opts.logFile, false, !opts.quietMode));
        // get rid of the temporary log as we use the new log now
		logMgr->destroyLog("Temporary log");

		rgm = new ResourceSchemeManager();
		mth = new Math();
        lodMgr = new LodStrategyManager();
		meshMgr = new MeshManager();
		matMgr = new MaterialManager();
		matMgr->init();
		skelMgr = new SkeletonManager();
		meshSerializer = new MeshSerializer();
		xmlMeshSerializer = new XMLMeshSerializer();
		skeletonSerializer = new SkeletonSerializer();
		xmlSkeletonSerializer = new XMLSkeletonSerializer();
		bufferManager = new MemGBufferManager(); // needed because we don't have a rendersystem



		if(opts.sourceExt == "mesh")
		{
			meshToXML(opts);
		}
		else if(opts.sourceExt == "skeleton")
		{
			skeletonToXML(opts);
		}
		else if(opts.sourceExt == "xml")
		{
			XMLToBinary(opts);
		}
		else
		{
			cout << "Unknown input type.\n";
			retCode = 1;
		}
	}
	catch(Exception & e)
	{
		cerr << "FATAL ERROR: " << e.getVerbose() << std::endl;
		cerr << "ABORTING!" << std::endl;
		retCode = 1;
	}

    delete xmlSkeletonSerializer;
    delete skeletonSerializer;
    delete xmlMeshSerializer;
    delete meshSerializer;
    delete skelMgr;
    delete matMgr;
    delete meshMgr;
	delete bufferManager;
    delete lodMgr;
    delete mth;
    delete rgm;
    delete logMgr;

    return retCode;
}