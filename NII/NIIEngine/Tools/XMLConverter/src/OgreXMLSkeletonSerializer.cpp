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

#include "OgreXMLSkeletonSerializer.h"
#include "OgreSkeleton.h"
#include "OgreAnimation.h"
#include "NiiKeyFrameTrack.h"
#include "OgreKeyFrame.h"
#include "OgreBone.h"
#include "OgreString.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "Ogre.h"

#include <map>

namespace Ogre {


    //---------------------------------------------------------------------
    XMLSkeletonSerializer::XMLSkeletonSerializer()
    {
    }
    //---------------------------------------------------------------------
    XMLSkeletonSerializer::~XMLSkeletonSerializer()
    {
    }

    //---------------------------------------------------------------------
    void XMLSkeletonSerializer::importSkeleton(const String & filename, Skeleton* pSkeleton)
    {
		LogManager::getSingleton().logMessage("XMLSkeletonSerializer: reading XML data from " + filename + "...");

		mXMLDoc = new TiXmlDocument(filename);
        mXMLDoc->LoadFile();

		TiXmlElement * elem;

        TiXmlElement * rootElem = mXMLDoc->RootElement();

		// Optional blend mode
		const char * blendModeStr = rootElem->Attribute("blendmode");
		if (blendModeStr)
		{
			if (String(blendModeStr) == "cumulative")
				pSkeleton->setBlendMode(M_MUL);
			else
				pSkeleton->setBlendMode(M_AVG);

		}


        // Bones
        elem = rootElem->FirstChildElement("bones");
        if (elem)
		{
            readBones(pSkeleton, elem);
			elem = rootElem->FirstChildElement("bonehierarchy");

			if (elem)
			{
				createHierarchy(pSkeleton, elem) ;
				elem = rootElem->FirstChildElement("bones");
				if (elem)
				{
					readBones2(pSkeleton, elem);
					elem = rootElem->FirstChildElement("animations");
					if (elem)
					{
						readAnimations(pSkeleton, elem);
					}
					elem = rootElem->FirstChildElement("animationlinks");
					if (elem)
					{
						readSkeletonAnimationLinks(pSkeleton, elem);
					}
				}
			}
		}
		LogManager::getSingleton().logMessage("XMLSkeletonSerializer: Finished. Running SkeletonSerializer..." );
    }


	//---------------------------------------------------------------------
	// sets names
	void XMLSkeletonSerializer::readBones(Skeleton* skel, TiXmlElement* mBonesNode)
    {
        LogManager::getSingleton().logMessage("XMLSkeletonSerializer: Reading Bones name...");

		Quaternion quat ;

        for (TiXmlElement* bonElem = mBonesNode->FirstChildElement();
            bonElem != 0; bonElem = bonElem->NextSiblingElement())
        {
            String name = bonElem->Attribute("name");
			int id = StringConverter::parseInt(bonElem->Attribute("id"));
			skel->create(name, id) ;
        }
    }
	// ---------------------------------------------------------
	// set positions and orientations.
	void XMLSkeletonSerializer::readBones2(Skeleton* skel, TiXmlElement* mBonesNode)
    {
        LogManager::getSingleton().logMessage("XMLSkeletonSerializer: Reading Bones data...");

		Bone* btmp ;
		Quaternion quat ;

        for (TiXmlElement* bonElem = mBonesNode->FirstChildElement();
            bonElem != 0; bonElem = bonElem->NextSiblingElement())
        {
            String name = bonElem->Attribute("name");
//			int id = StringConverter::parseInt(bonElem->Attribute("id"));

			TiXmlElement* posElem = bonElem->FirstChildElement("position");
			TiXmlElement* rotElem = bonElem->FirstChildElement("rotation");
			TiXmlElement* axisElem = rotElem->FirstChildElement("axis");
            TiXmlElement* scaleElem = bonElem->FirstChildElement("scale");

			Vector3 pos;
			Vector3 axis;
			Radian angle;
            Vector3 scale;

			pos.x = StringConverter::parseReal(posElem->Attribute("x"));
			pos.y = StringConverter::parseReal(posElem->Attribute("y"));
			pos.z = StringConverter::parseReal(posElem->Attribute("z"));

			angle = Radian(StringConverter::parseReal(rotElem->Attribute("angle")));

			axis.x = StringConverter::parseReal(axisElem->Attribute("x"));
			axis.y = StringConverter::parseReal(axisElem->Attribute("y"));
			axis.z = StringConverter::parseReal(axisElem->Attribute("z"));

            // Optional scale
            if (scaleElem)
            {
                // Uniform scale or per axis?
                const char* factorAttrib = scaleElem->Attribute("factor");
                if (factorAttrib)
                {
                    // Uniform scale
                    NIIf factor = StringConverter::parseReal(factorAttrib);
                    scale = Vector3(factor, factor, factor);
                }
                else
                {
                    // axis scale
                    scale = Vector3::UNIT;
                    const char* factorString = scaleElem->Attribute("x");
                    if (factorString)
                    {
                        scale.x = StringConverter::parseReal(factorString);
                    }
                    factorString = scaleElem->Attribute("y");
                    if (factorString)
                    {
                        scale.y = StringConverter::parseReal(factorString);
                    }
                    factorString = scaleElem->Attribute("z");
                    if (factorString)
                    {
                        scale.z = StringConverter::parseReal(factorString);
                    }
                }
            }
            else
            {
                scale = Vector3::UNIT;
            }

			/*LogManager::getSingleton().logMessage("bone " + name + " : position("
				+ StrConv::conv(pos.x) + "," + StrConv::conv(pos.y) + "," + StrConv::conv(pos.z) + ")"
				+ " - angle: " + StrConv::conv(angle) +" - axe: "
				+ StrConv::conv(axis.x) + "," + StrConv::conv(axis.y) + "," + StrConv::conv(axis.z) );
			*/

			btmp = skel->get(name) ;

			btmp ->setPos(pos);
			quat.from(angle, axis);
			btmp ->setOri(quat) ;
            btmp ->setScale(scale);

        } // bones
    }
	//-------------------------------------------------------------------
	void XMLSkeletonSerializer::createHierarchy(Skeleton * skel, TiXmlElement * mHierNode)

		LogManager::getSingleton().logMessage("XMLSkeletonSerializer: Reading Hierarchy data...");

		Bone* bone ;
		Bone* parent ;
		String boneName ;
		String parentName ;

		for (TiXmlElement* hierElem = mHierNode->FirstChildElement() ; hierElem != 0; hierElem = hierElem->NextSiblingElement())
        {
			boneName = hierElem->Attribute("bone");
			parentName = hierElem->Attribute("parent");
			bone = skel->get(boneName);
			parent = skel->get(parentName);
			parent ->add(bone) ;
			//LogManager::getSingleton().logMessage("XMLSkeletonSerialiser: lien: " + parent->getName() + "->" + bone->getName());

		}
	}
	//---------------------------------------------------------------------
	void XMLSkeletonSerializer::readAnimations(Skeleton * skel, TiXmlElement * mAnimNode)
    {
		Animation * anim ;
		NodeKeyFrameTrack * track ;
		LogManager::getSingleton().logMessage("XMLSkeletonSerializer: Reading Animations data...");

		for(TiXmlElement * animElem = mAnimNode->FirstChildElement("animation"); animElem != 0; animElem = animElem->NextSiblingElement())
        {
            String name = animElem->Attribute("name");
			TimeDurMS length = StringConverter::parseReal(animElem->Attribute("length"));
			anim = skel->createAnimation(name, length);
			anim->setMode(NodeFusion::M_LINEAR) ; //< ¸Ä³ÉÊµÀý×´Ì¬

			//LogManager::getSingleton().logMessage("Animation: nom: " + name + " et longueur: "
			//	+ StrConv::conv(length) );
			TiXmlElement* baseInfoNode = animElem->FirstChildElement("baseinfo");
			if (baseInfoNode)
			{
				String baseName = baseInfoNode->Attribute("baseanimationname");
				TimeDurMS baseTime = StringConverter::parseReal(baseInfoNode->Attribute("basekeyframetime"));
                anim->setBase(baseName, baseTime);
			}

			// lecture des tracks
			int trackIndex = 0;
			TiXmlElement* tracksNode = animElem->FirstChildElement("tracks");

			for (TiXmlElement* trackElem = tracksNode->FirstChildElement("track"); trackElem != 0; trackElem = trackElem->NextSiblingElement())
			{
				String boneName = trackElem->Attribute("bone");

				//LogManager::getSingleton().logMessage("Track sur le bone: " + boneName );

                NodeKeyFrameTrack * track = N_new NodeKeyFrameTrack(anim, trackIndex++);
                track->setTarget(skel->get(boneName));
				readKeyFrames(track, trackElem->FirstChildElement("keyframes"));
			}
		}
	}
	//---------------------------------------------------------------------
	void XMLSkeletonSerializer::readKeyFrames(NodeKeyFrameTrack* track, TiXmlElement* mKeyfNode) {

		NodeKeyFrame* kf ;
		Quaternion q ;

		for (TiXmlElement* keyfElem = mKeyfNode->FirstChildElement("keyframe"); keyfElem != 0; keyfElem = keyfElem->NextSiblingElement())
        {
			Vector3 trans;
			Vector3 axis;
			Radian angle;
			NIIf time;

            // Get time and create keyframe
			time = StringConverter::parseReal(keyfElem->Attribute("time"));
			kf = static_cast<NodeKeyFrame *>(track->create(time));
            // Optional translate
			TiXmlElement* transElem = keyfElem->FirstChildElement("translate");
            if (transElem)
            {
			    trans.x = StringConverter::parseReal(transElem->Attribute("x"));
			    trans.y = StringConverter::parseReal(transElem->Attribute("y"));
			    trans.z = StringConverter::parseReal(transElem->Attribute("z"));
			    kf->setTranslate(trans) ;
            }
            // Optional rotate
			TiXmlElement* rotElem = keyfElem->FirstChildElement("rotate");
            if (rotElem)
            {
                TiXmlElement* axisElem = rotElem->FirstChildElement("axis");
                if (!axisElem)
                {
                    N_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Missing 'axis' element "
                    "expected under parent 'rotate'", "MXLSkeletonSerializer::readKeyFrames");
                }
			    angle = Radian(StringConverter::parseReal(rotElem->Attribute("angle")));

			    axis.x = StringConverter::parseReal(axisElem->Attribute("x"));
			    axis.y = StringConverter::parseReal(axisElem->Attribute("y"));
			    axis.z = StringConverter::parseReal(axisElem->Attribute("z"));

			    q.from(angle, axis);
			    kf->setRotation(q) ;

            }
            // Optional scale
			TiXmlElement* scaleElem = keyfElem->FirstChildElement("scale");
            if (scaleElem)
            {
                // Uniform scale or per axis?
				const char* factorAttrib = scaleElem->Attribute("factor");
				if (factorAttrib)
				{
					// Uniform scale
					NIIf factor = StringConverter::parseReal(factorAttrib);
					kf->setScale(Vector3(factor, factor, factor));
				}
				else
				{
					// axis scale
                    NIIf xs = 1.0f, ys = 1.0f, zs=1.0f;
                    const char* factorString = scaleElem->Attribute("x");
                    if(factorString)
                    {
                        xs = StringConverter::parseReal(factorString);
                    }
                    factorString = scaleElem->Attribute("y");
                    if(factorString)
                    {
                        ys = StringConverter::parseReal(factorString);
                    }
                    factorString = scaleElem->Attribute("z");
                    if(factorString)
                    {
                        zs = StringConverter::parseReal(factorString);
                    }
					kf->setScale(Vector3(xs, ys, zs));

				}
            }


			/*
			LogManager::getSingleton().logMessage("Keyframe: translation("
				+ StrConv::conv(trans.x) + "," + StrConv::conv(trans.y) + "," + StrConv::conv(trans.z) + ")"
				+ " - angle: " + StrConv::conv(angle) +" - axe: "
				+ StrConv::conv(axis.x) + "," + StrConv::conv(axis.y) + "," + StrConv::conv(axis.z) );
			*/
		}
        track->buildSpline();
	}
    //---------------------------------------------------------------------
    void XMLSkeletonSerializer::exportSkeleton(const Skeleton* pSkeleton,
        const String& filename)
    {

        LogManager::getSingleton().logMessage("XMLSkeletonSerializer writing "
            " skeleton data to " + filename + "...");

        mXMLDoc = new TiXmlDocument();
        mXMLDoc->InsertEndChild(TiXmlElement("skeleton"));
        TiXmlElement* rootNode = mXMLDoc->RootElement();

        LogManager::getSingleton().logMessage("Populating DOM...");


        // Write main skeleton data
        LogManager::getSingleton().logMessage("Exporting bones..");
        writeSkeleton(pSkeleton);
        LogManager::getSingleton().logMessage("Bones exported.");

        // Write all animations
        NCount numAnims = pSkeleton->getAnimationCount();
		String msg = "Exporting animations, count=" + StrConv::conv(numAnims);
        LogManager::getSingleton().logMessage(msg);

        TiXmlElement* animsNode =
            rootNode->InsertEndChild(TiXmlElement("animations"))->ToElement();

        for(Nindex i = 0; i < numAnims; ++i)
        {
            Animation* pAnim = pSkeleton->getAnimation(i);
            msg = "Exporting animation: " + pAnim->getName();
            LogManager::getSingleton().logMessage(msg);
            writeAnimation(animsNode, pAnim);
            LogManager::getSingleton().logMessage("Animation exported.");

        }

		// Write links
		Skeleton::ExtList::iterator i = pSkeleton->getExtList().begin();
        Skeleton::ExtList::iterator iend = pSkeleton->getExtList().end();
		if(i != iend)
		{
			LogManager::getSingleton().logMessage("Exporting animation links.");
			TiXmlElement * linksNode = rootNode->InsertEndChild(TiXmlElement("animationlinks"))->ToElement();
			for(; i != iend; ++i)
			{
				const Skeleton::Ext & link = *i;
				writeSkeletonAnimationLink(linksNode, link);
			}
		}

        LogManager::getSingleton().logMessage("DOM populated, writing XML file..");

        // Write out to a file
        if(! mXMLDoc->SaveFile(filename) )
        {
            LogManager::getSingleton().logMessage("XMLSkeletonSerializer failed writing the XML file.", LML_CRITICAL);
        }
        else
        {
            LogManager::getSingleton().logMessage("XMLSkeletonSerializer export successful.");
        }

        delete mXMLDoc;

    }
    //---------------------------------------------------------------------
    void XMLSkeletonSerializer::writeSkeleton(const Skeleton * pSkel)
    {
        TiXmlElement * rootNode = mXMLDoc->RootElement();

		// Blend mode
		String blendModeStr = pSkel->getBlendMode() == M_MUL ? "cumulative" : "average";
		rootNode->SetAttribute("blendmode", blendModeStr);

        TiXmlElement * bonesElem = rootNode->InsertEndChild(TiXmlElement("bones"))->ToElement();

        unsigned short numBones = pSkel->getCount();
		LogManager::getSingleton().logMessage("There are " + StrConv::conv(numBones) + " bones.");
        unsigned short i;
        for (i = 0; i < numBones; ++i)
        {
			LogManager::getSingleton().logMessage("   Exporting Bone number " + StrConv::conv(i));
            Bone * pBone = pSkel->get(i);
            writeBone(bonesElem, pBone);
        }

        // Write parents
        TiXmlElement * hierElem = rootNode->InsertEndChild(TiXmlElement("bonehierarchy"))->ToElement();
        for(i = 0; i < numBones; ++i)
        {
            Bone* pBone = pSkel->get(i);
			String name = pBone->getName() ;

			if ((pBone->getParent())!=NULL) // root bone
            {
                Bone* pParent = (Bone*)pBone->getParent();
                writeBoneParent(hierElem, name, pParent->getName());
            }
        }
    }
    //---------------------------------------------------------------------
    void XMLSkeletonSerializer::writeBone(TiXmlElement * bonesElement, const Bone * pBone)
    {
        TiXmlElement * boneElem = bonesElement->InsertEndChild(TiXmlElement("bone"))->ToElement();
        // Bone name & handle
        boneElem->SetAttribute("id", StrConv::conv(pBone->getID()));
        boneElem->SetAttribute("name", pBone->getName());

        // Position
        TiXmlElement * subNode = boneElem->InsertEndChild(TiXmlElement("position"))->ToElement();
        Vector3 pos = pBone->getPos();
        subNode->SetAttribute("x", StrConv::conv(pos.x));
        subNode->SetAttribute("y", StrConv::conv(pos.y));
        subNode->SetAttribute("z", StrConv::conv(pos.z));

        // Orientation
        subNode = boneElem->InsertEndChild(TiXmlElement("rotation"))->ToElement();
        // Show Quaternion as angle / axis
        Radian angle;
        Vector3 axis;
        pBone->getOri().to(angle, axis);

        TiXmlElement * axisNode = subNode->InsertEndChild(TiXmlElement("axis"))->ToElement();
        subNode->SetAttribute("angle", StrConv::conv(angle.getValue()));
        axisNode->SetAttribute("x", StrConv::conv(axis.x));
        axisNode->SetAttribute("y", StrConv::conv(axis.y));
        axisNode->SetAttribute("z", StrConv::conv(axis.z));

        // Scale optional
        Vector3 scale = pBone->getScale();
        if(scale != Vector3::UNIT)
        {
            TiXmlElement * scaleNode = boneElem->InsertEndChild(TiXmlElement("scale"))->ToElement();
            scaleNode->SetAttribute("x", StrConv::conv(scale.x));
            scaleNode->SetAttribute("y", StrConv::conv(scale.y));
            scaleNode->SetAttribute("z", StrConv::conv(scale.z));
        }
    }
    //---------------------------------------------------------------------
	//
	// Modifications effectuées:
	//
	// on stoque les noms et pas les Id. c'est plus lisibles.


    void XMLSkeletonSerializer::writeBoneParent(TiXmlElement* boneHierarchyNode,
        String boneName, String parentName)
    {
        TiXmlElement* boneParentNode =
            boneHierarchyNode->InsertEndChild(TiXmlElement("boneparent"))->ToElement();
		/*
	    boneParentNode->SetAttribute("boneid", StrConv::conv(boneId));
        boneParentNode->SetAttribute("parentid", StrConv::conv(parentId));
		*/
		// Modifications: on stoque les noms./
		boneParentNode->SetAttribute("bone", boneName);
        boneParentNode->SetAttribute("parent", parentName);

    }
    //---------------------------------------------------------------------
    void XMLSkeletonSerializer::writeAnimation(TiXmlElement * animsNode, const Animation * anim)
    {
        TiXmlElement * animNode = animsNode->InsertEndChild(TiXmlElement("animation"))->ToElement();

        animNode->SetAttribute("name", anim->getName());
        animNode->SetAttribute("length", StrConv::conv(anim->getDur()));

		// Optional base keyframe information
		if(anim->getBaseID() != 0)
		{
			TiXmlElement * baseInfoNode = animNode->InsertEndChild(TiXmlElement("baseinfo"))->ToElement();
			baseInfoNode->SetAttribute("baseanimationname", anim->getBase());
			baseInfoNode->SetAttribute("basekeyframetime", StrConv::conv(anim->getBaseTime()));
		}

        // Write all tracks
        TiXmlElement * tracksNode = animNode->InsertEndChild(TiXmlElement("tracks"))->ToElement();

        Animation::TrackList i, iend = anim->getTrack().end();
        for(i = anim->getTrack().begin(); i != iend; ++i)
        {
            writeAnimationTrack(tracksNode, *i);
        }
    }
    //---------------------------------------------------------------------
    void XMLSkeletonSerializer::writeAnimationTrack(TiXmlElement * tracksNode,
        const NodeKeyFrameTrack * track)
    {
        TiXmlElement * trackNode = tracksNode->InsertEndChild(TiXmlElement("track"))->ToElement();

        // unsigned short boneIndex     : Index of bone to apply to
        Bone * bone = (Bone *)track->getTarget();
        //unsigned short boneid = bone->getID();
		String boneName = bone->getName();
        trackNode->SetAttribute("bone", boneName);

        // Write all keyframes
        TiXmlElement * keysNode = trackNode->InsertEndChild(TiXmlElement("keyframes"))->ToElement();
        for(NCount i = 0; i < track->getCount(); ++i)
        {
            writeKeyFrame(keysNode, static_cast<NodeKeyFrame *>(track->get(i)));
        }
    }
    //---------------------------------------------------------------------
    void XMLSkeletonSerializer::writeKeyFrame(TiXmlElement * keysNode,
		const NodeKeyFrame * key)
    {
        TiXmlElement * keyNode = keysNode->InsertEndChild(TiXmlElement("keyframe"))->ToElement();

        keyNode->SetAttribute("time", StrConv::conv(key->getTime()));

        TiXmlElement * transNode = keyNode->InsertEndChild(TiXmlElement("translate"))->ToElement();
        Vector3 trans = key->getTranslate();
        transNode->SetAttribute("x", StrConv::conv(trans.x));
        transNode->SetAttribute("y", StrConv::conv(trans.y));
        transNode->SetAttribute("z", StrConv::conv(trans.z));

        TiXmlElement * rotNode = keyNode->InsertEndChild(TiXmlElement("rotate"))->ToElement();
        // Show Quaternion as angle / axis
        Radian angle;
        Vector3 axis;
        key->getRotation().to(angle, axis);

        TiXmlElement * axisNode = rotNode->InsertEndChild(TiXmlElement("axis"))->ToElement();
        rotNode->SetAttribute("angle", StrConv::conv(angle.getValue()));
        axisNode->SetAttribute("x", StrConv::conv(axis.x));
        axisNode->SetAttribute("y", StrConv::conv(axis.y));
        axisNode->SetAttribute("z", StrConv::conv(axis.z));

        // Scale optional
        if(key->getScale() != Vector3::UNIT)
        {
            TiXmlElement * scaleNode = keyNode->InsertEndChild(TiXmlElement("scale"))->ToElement();
            scaleNode->SetAttribute("x", StrConv::conv(key->getScale().x));
            scaleNode->SetAttribute("y", StrConv::conv(key->getScale().y));
            scaleNode->SetAttribute("z", StrConv::conv(key->getScale().z));
        }

    }
    //---------------------------------------------------------------------
	void XMLSkeletonSerializer::writeSkeletonAnimationLink(TiXmlElement* linksNode,
		const Skeleton::Ext & link)
	{
		TiXmlElement* linkNode =
			linksNode->InsertEndChild(TiXmlElement("animationlink"))->ToElement();
		linkNode->SetAttribute("skeletonName", link.mSkeletonID);
		linkNode->SetAttribute("scale", StrConv::conv(link.mScale));

	}
	//---------------------------------------------------------------------
	void XMLSkeletonSerializer::readSkeletonAnimationLinks(Skeleton* skel,
		TiXmlElement* linksNode)
	{
		LogManager::getSingleton().logMessage("XMLSkeletonSerializer: Reading Animations links...");

		for (TiXmlElement* linkElem = linksNode->FirstChildElement("animationlink");
			linkElem != 0; linkElem = linkElem->NextSiblingElement())
		{
			String skelName = linkElem->Attribute("skeletonName");
			const char* strScale = linkElem->Attribute("scale");
			NIIf scale;
			// Scale optional
			if (strScale == 0)
			{
				scale = 1.0f;
			}
			else
			{
				scale = StringConverter::parseReal(strScale);
			}
			skel->addExt(skelName, scale);

		}
	}
}


