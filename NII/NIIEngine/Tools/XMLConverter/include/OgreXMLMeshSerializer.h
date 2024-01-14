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

#ifndef __XMLMeshSerializer_H__
#define __XMLMeshSerializer_H__

#include "OgreXMLPrerequisites.h"
#include "OgreMesh.h"

namespace Ogre 
{
    /** Class for serializing a Mesh ¶Á/Ð´ XML.
    @remarks
        This class behaves the same way as MeshSerializer in the main project,
        but is here to allow conversions to / from XML. This class is 
        deliberately not included in the main project because <UL>
        <LI>Dependence on Xerces would unnecessarily bloat the main library</LI>
        <LI>Runtime use of XML is discouraged because of the parsing overhead</LI></UL>
        This class gives people the option of saving out a Mesh as XML for examination
        and possible editing. It can then be converted back to the native format
        for maximum runtime efficiency.
    */
    class XMLMeshSerializer
    {
    public:

        XMLMeshSerializer();
        virtual ~XMLMeshSerializer();
        /** Imports a Mesh from the given XML file.
        @param filename The name of the file to import, expected to be in XML format.
		@param colourElementType The vertex element to use for packed colours
        @param pMesh The pre-created Mesh object to be populated.
        */
        void importMesh(const String& filename, DataType colourElementType, Mesh* pMesh);

        /** Exports a mesh to the named XML file. */
        void exportMesh(const Mesh* pMesh, const String& filename);

    protected:
        // State for export
        TiXmlDocument* mXMLDoc;
        // State for import
        Mesh* mMesh;
		DataType mColourElementType;

        // Internal methods
        void writeMesh(const Mesh * pMesh);
        void writeSubMesh(TiXmlElement * mSubmeshesNode, const SubMesh * s);
        void writeGeometry(TiXmlElement * mParentNode, const VertexData * pData);
        void writeSkeletonLink(TiXmlElement * mMeshNode, const String & skelName);
        void writeBoneAssignment(TiXmlElement * mBoneAssignNode, const SkeletonVertex * assign);
		void writeLod(TiXmlElement* mMeshNode, const Mesh * pMesh);
		void writeLodUsageManual(TiXmlElement * usageNode, unsigned short levelNum, 
			const MeshLodScheme::MeshLod & usage);
		void writeLodUsageGenerated(TiXmlElement * usageNode, unsigned short levelNum,  
			const MeshLodScheme::MeshLod & usage, const Mesh * pMesh);
		void writePoses(TiXmlElement* meshNode, const Mesh* m);
		void writeAnimations(TiXmlElement* meshNode, const Mesh* m);
		void writeMorphKeyFrames(TiXmlElement* trackNode, const VertexKeyFrameTrack* track);
		void writePoseKeyFrames(TiXmlElement* trackNode, const VertexKeyFrameTrack* track);
        void writeExtremes(TiXmlElement* mMeshNode, const Mesh* m);

        void readSubMeshes(TiXmlElement* mSubmeshesNode);
        void readGeometry(TiXmlElement* mGeometryNode, VertexData* pData);
        void readSkeletonLink(TiXmlElement* mSkelNode);
        void readBoneAssignments(TiXmlElement* mBoneAssignmentsNode);
        void readBoneAssignments(TiXmlElement* mBoneAssignmentsNode, SubMesh* sm);
		void readLodInfo(TiXmlElement*  lodNode);
		void readLodUsageManual(TiXmlElement* manualNode, unsigned short index);
		void readLodUsageGenerated(TiXmlElement* genNode, unsigned short index);
		void readSubMeshNames(TiXmlElement* mMeshNamesNode, Mesh* sm);
		void readPoses(TiXmlElement* posesNode, Mesh *m);
		void readAnimations(TiXmlElement* mAnimationsNode, Mesh *m);
		void readTracks(TiXmlElement* tracksNode, Mesh *m, Animation* anim);
		void readMorphKeyFrames(TiXmlElement* keyframesNode, VertexKeyFrameTrack* track, 
			size_t vertexCount);
		void readPoseKeyFrames(TiXmlElement* keyframesNode, VertexKeyFrameTrack* track);
        void readExtremes(TiXmlElement* extremesNode, Mesh *m);
    };
}

#endif
