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

#include "OgrePrerequisites.h"
#include "OgreMesh.h"

#include "OgreMaxConfig.h"
#include "OgreMaxExport.h"

#include "max.h"
#include "INode.h"
#include <list>

typedef std::list<INode*> NodeList;

namespace OgreMax {

	class MeshExporter : public OgreMaxExporter
	{
	public:

		MeshExporter(const Config& config);
		virtual ~MeshExporter();

		// take in a list of INode* for assembly into Ogre::Mesh form; returns
		// a MeshPtr pointing to the newly minted Ogre Mesh object.
		Ogre::MeshPtr buildMesh(NodeList nodeList);

	private:

		// Pointer to Mesh object for export
		Ogre::MeshPtr	m_pMesh;
	};
}