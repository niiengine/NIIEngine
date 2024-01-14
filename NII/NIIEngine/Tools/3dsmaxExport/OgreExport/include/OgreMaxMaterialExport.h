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

#if !defined(__OGREMAX_MATERIAL_EXPORTER_H__)
#define __OGREMAX_MATERIAL_EXPORTER_H__

#include "OgreMaxConfig.h"
#include "OgreMaxExport.h"

#include <string>
#include <sstream>
#include <queue>
#include <map>
#include <list>
#include <iosfwd>

//class Mtl;
class IGameMaterial;

namespace OgreMax {

	//typedef std::map< std::string, Mtl* > MaterialMap;
	typedef std::map< std::string, IGameMaterial* > MaterialMap;

	class MaterialExporter  : public OgreMaxExporter
	{

	public:
		MaterialExporter(const Config& config, MaterialMap& map);
		virtual ~MaterialExporter();

		// take in a list of INode* for assembly into Ogre::Mesh form; returns
		// a map of filename --> XML stringstream.
		bool buildMaterial(std::string& output);
		bool buildMaterial(IGameMaterial *mtl, const std::string& matName, std::string &of);

	protected:
		// material file stream functions
		bool streamPass(std::ostream &of, IGameMaterial *mtl);
		bool streamMaterial(std::ostream &of);

		std::string removeSpaces(const std::string &s);

	private:

		MaterialMap& m_materialMap;
	};
}

#endif
