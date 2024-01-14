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

#if !defined(__OGREMAX_VERTEX_H__)
#define __OGREMAX_VERTEX_H__

#include "OgreColourValue.h"
#include "OgreVector3.h"

namespace OgreMax {

	typedef std::map<int, Ogre::Vector3> TexCoordMap;

	class Vertex {
	public:

		Vertex(float x, float y, float z);

		void setNormal(float x, float y, float z);
		void setColour(float r, float g, float b, float a = 1.0);
		void addTexCoord(int mapIndex, float u, float v, float w = 0.0);

		bool operator==(Vertex& other);

		const Ogre::Vector3& getPos() const { return m_position; }
		const Ogre::Vector3& getNormal() const { return m_normal; }
		const Ogre::Colour& getColour() const { return m_colour; }
		const Ogre::Vector3& getUVW(int mapIndex) const { return m_uvwMap.find(mapIndex)->second; }
	
	private:
		bool hasSameTexCoords(std::map<int, Ogre::Vector3>& uvwMap) ;
		Ogre::Vector3					m_position;
		Ogre::Vector3					m_normal;
		Ogre::Colour				m_colour;
		TexCoordMap						m_uvwMap;
	};

	class VertexList {
	public:
		// returns the index into the list for the inserted element
		unsigned int add(Vertex& v);
		size_t size() { return m_vertexList.size(); }
		const Vertex& front() { return m_vertexList.front(); }
		void pop() { m_vertexList.pop_front(); }

	private:
		std::list<Vertex> m_vertexList;

	};

}


#endif