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

#if !defined(__OGREMAX_MESHXML_EXPORTER_H__)
#define __OGREMAX_MESHXML_EXPORTER_H__

#include "OgreMaxConfig.h"
#include "OgreMaxExport.h"

#include <string>
#include <queue>
#include <map>
#include "tab.h"

class MaterialMap;
class IGameScene;
class IGameNode;
class IGameObject;
class IGameMaterial;

namespace OgreMax {

	class MeshXMLExporter  : public OgreMaxExporter, public ITreeEnumProc
	{
		typedef struct {
			std::string name;
			int start;
			int end;
		} NamedAnimation;

	public:
		typedef std::map< std::string, std::string > OutputMap;

		MeshXMLExporter(const Config& config, MaterialMap& materialMap);
		virtual ~MeshXMLExporter();

		// generates a list of INode* for assembly into Ogre::Mesh form; returns
		// a map of filename --> XML stringstream.
		bool buildMeshXML(OutputMap& output);

	protected:
		// mesh file stream functions
		bool streamFileHeader(std::ostream &of);
		bool streamFileFooter(std::ostream &of);
		bool streamSubmesh(std::ostream &of, IGameObject *obj, std::string &mtlName);
		bool streamBoneAssignments(std::ostream &of, Modifier *mod, IGameNode *node);

		int getBoneIndex(char *name);
		std::string removeSpaces(const std::string &s);

		int callback(INode *node);

	private:

		bool export(OutputMap& output);

		bool m_createSkeletonLink;
		std::string m_exportFilename;			// filename provided by
		std::string m_filename;			// filename provided by
		std::string m_skeletonFilename;
		std::queue< std::string > m_submeshNames;
		MaterialMap& m_materialMap;
		std::map< std::string, int > m_boneIndexMap;
		int m_currentBoneIndex;

		IGameScene*	m_pGame;
		Tab<INode*> m_nodeTab;
	};
}

#endif