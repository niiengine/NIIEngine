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

#ifndef __XMLSkeletonSerializer_H__
#define __XMLSkeletonSerializer_H__

#include "OgreXMLPrerequisites.h"
#include "OgreMaterial.h"
#include "OgreSkeleton.h"

namespace Ogre 
{
    /** Class for serializing a Skeleton ¶Á/Ð´ XML.
    @remarks
        This class behaves the same way as SkeletonSerializer in the main project,
        but is here to allow conversions to / from XML. This class is 
        deliberately not included in the main project because <UL>
        <LI>Dependence on Xerces would unnecessarily bloat the main library</LI>
        <LI>Runtime use of XML is discouraged because of the parsing overhead</LI></UL>
        This class gives people the option of saving out a Skeleton as XML for examination
        and possible editing. It can then be converted back to the native format
        for maximum runtime efficiency.
    */
    class XMLSkeletonSerializer
    {
    public:

        XMLSkeletonSerializer();
        virtual ~XMLSkeletonSerializer();
        /** Imports a Skeleton from the given XML file.
        @param filename The name of the file to import, expected to be in XML format.
        @param obj The pre-created Skeleton object to be populated.
        */
        void importSkeleton(const String & filename, Skeleton * obj);

        /** Exports a skeleton to the named XML file. */
        void exportSkeleton(const Skeleton * obj, const String & filename);
    private:
        // State for export
        TiXmlDocument* mXMLDoc;
        // State for import
        Skeleton* mSkel;

        void writeSkeleton(const Skeleton * pSkel);
        void writeBone(TiXmlElement * bonesElement, const Bone * pBone);
        void writeBoneParent(TiXmlElement * boneHierarchyNode, String boneName , String parentName);
        void writeAnimation(TiXmlElement * animsNode, const Animation * anim);
        void writeAnimationTrack(TiXmlElement * tracksNode, const NodeKeyFrameTrack * track);
        void writeKeyFrame(TiXmlElement * keysNode, const NodeKeyFrame* key);
		void writeSkeletonAnimationLink(TiXmlElement * linksNode, const Skeleton::Ext & link);
		
		void readBones(Skeleton* skel, TiXmlElement* mBonesNode);
		void readBones2(Skeleton* skel, TiXmlElement* mBonesNode);
		void createHierarchy(Skeleton* skel, TiXmlElement* mHierNode);
		void readKeyFrames(NodeKeyFrameTrack* track, TiXmlElement* mKeyfNode);
		void readAnimations(Skeleton* skel, TiXmlElement* mAnimNode) ;
		void readSkeletonAnimationLinks(Skeleton* skel, TiXmlElement* linksNode);
    };
}
#endif