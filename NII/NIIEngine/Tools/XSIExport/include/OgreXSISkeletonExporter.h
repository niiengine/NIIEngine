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
#ifndef __XSISKELETONEXPORTER_H__
#define __XSISKELETONEXPORTER_H__

#include "OgrePrerequisites.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreAABox.h"
#include "OgreXSIHelper.h"
#include <xsi_x3dobject.h>
#include <xsi_string.h>
#include <xsi_application.h>
#include <xsi_actionsource.h>
#include <xsi_mixer.h>


namespace Ogre {

	/** Class for performing a skeleton export from XSI.
	*/
	class XsiSkeletonExporter
	{
	public:
		XsiSkeletonExporter();
		virtual ~XsiSkeletonExporter();


		/** Export a skeleton to the provided filename.
		@param skeletonFileName The file name to export to
		@param deformers The list of deformers (bones) found during mesh traversal
		@param framesPerSecond The number of frames per second
		@param animList List of animation splits
		@returns AABB derived from bone animations, should be used to pad mesh bounds
		*/
		const AABox& exportSkeleton(const String& skeletonFileName,
			DeformerMap& deformers, float framesPerSecond,
			AnimationList& animList);
	protected:
		// XSI Objects
		XSI::Application mXsiApp;
		XSI::X3DObject mXsiSceneRoot;
		std::map<String, int> mXSITrackTypeNames;
		// Lower-case version of deformer map (XSI seems to be case insensitive and
		// some animations rely on that!)
		DeformerMap mLowerCaseDeformerMap;
		// Actions created as part of IK sampling, will be deleted afterward
		XSI::CStringArray mIKSampledAnimations;
		AABox mAABB;

		/// Build the bone hierarchy from a simple list of bones
		void buildBoneHierarchy(Skeleton * obj, DeformerMap & deformers,
			AnimationList & animList);
		/** Link the current bone with it's parent
		*/
		void linkBoneWithParent(DeformerEntry * deformer,
			DeformerMap & deformers, std::list<DeformerEntry *>& deformerList);
		/** Validate and create a bone, or eliminate the current bone if it
			has no animated parameters
		*/
		void validateAsBone(Skeleton * obj, DeformerEntry * deformer,
			DeformerMap & deformers, std::list<DeformerEntry *>& deformerList,
			AnimationList & animList);
		/// Process an action source
		void processActionSource(const XSI::ActionSource& source, DeformerMap& deformers);
		/// Bake animations
		void createAnimations(Skeleton* pSkel, DeformerMap& deformers,
			float framesPerSecond, AnimationList& animList, AABox& AABBPadding);
		/// Bake animation tracks by sampling
		void createAnimationTracksSampled(Animation* pAnim, AnimationEntry& animEntry,
			DeformerMap& deformers, float fps, AABox& AABBPadding);

		void cleanup(void);
		void copyDeformerMap(DeformerMap& deformers);
		/// Get deformer from passed in map or lower case version
		DeformerEntry* getDeformer(const String& name, DeformerMap& deformers);
		// Sample all bones, and also sample max global bone position for AABB padding
		void sampleAllBones(DeformerMap& deformers,
			std::vector<NodeKeyFrameTrack*> deformerTracks, double frame,
			NIIf time, float fps, AABox& AABBPadding);
		void establishInitialTransforms(DeformerMap& deformers);
	};
}
#endif