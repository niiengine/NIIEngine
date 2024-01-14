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
#include "PlayPenSamples.h"


//---------------------------------------------------------------------
PlayPen_testManualBlend::PlayPen_testManualBlend()
{
	mInfo["Title"] = "PlayPen: Manual Blend";
	mInfo["Description"] = "Manual blending";

}
void PlayPen_testManualBlend::setupContent()
{
	// create material
	MaterialPtr mat = MaterialManager::getSingleton().create("TestMat", TRANSIENT_RESOURCE_GROUP);
	Pass * p = mat->get(0)->get(0);
	p->setLight(false);
	p->getTexture().create("Dirt.jpg");
	ShaderChTextureUnit * t = p->getTexture().create("ogrelogo.png");
    t->getColourBlend()->setBlend(TBS_TEXTURE, TBS_PRE_TEXTURE, 0.75);
	GeometrySpace *planeEnt = mSceneMgr->createGeo("Plane", GSP_Plane);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(planeEnt);
	planeEnt->setMaterialName("TestMat");

	mCamera->setPos(NII::Vector3f(0,0,600));
	mCamera->lookAt(Vector3::ZERO);
}
//---------------------------------------------------------------------
PlayPen_testProjectSphere::PlayPen_testProjectSphere()
{
	mInfo["Title"] = "PlayPen: Project Sphere";
	mInfo["Description"] = "Projecting a sphere's bounds onto the camera";

}
void PlayPen_testProjectSphere::setupContent()
{
	mSceneMgr->setAmbient(Colour::White);

	Plane plane;
	plane.normal = Vector3::Y;
	plane.d = 0;
	MeshManager::getSingleton().createPlane("Myplane",
		TRANSIENT_RESOURCE_GROUP, plane,
		4500,4500,10,10,true,1,5,5,Vector3::Z);
	GeometrySpace* pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("Examples/GrassFloor");
	pPlaneEnt->setCastShadow(false);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(pPlaneEnt);

	mProjectionSphere = new Sphere(Vector3(0, 2000, 0), 1500.0);

	CustomSpaceObj* debugSphere = mSceneMgr->createCustomGeo("debugSphere");
	debugSphere->begin(GeometryRaw::OT_LINE_STRIP, MaterialManager::WhiteColour->getOriginID());
	for (int i = 0; i <= 20; ++i)
	{
		Vector3 basePos(mProjectionSphere->getRadius(), 0, 0);
		Quaternion quat;
		quat.from(Radian(((float)i/(float)20)*Math::PI2X), Vector3::Y);
		basePos = quat * basePos;
		debugSphere->position(basePos);
	}
	for (int i = 0; i <= 20; ++i)
	{
		Vector3 basePos(mProjectionSphere->getRadius(), 0, 0);
		Quaternion quat;
		quat.from(Radian(((float)i/(float)20)*Math::PI2X), Vector3::Z);
		basePos = quat * basePos;
		debugSphere->position(basePos);
	}
	debugSphere->end();

	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(0,2000,0)))->attachObject(debugSphere);

	MaterialPtr mat = MaterialManager::getSingleton().create("scissormat", TRANSIENT_RESOURCE_GROUP);
	Pass * p = mat->get(0)->get(0);
	p->setStencil(ShaderCh::SOT_Set)->setDepthWrite(false);
	p->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_INV_ALPHA);
	ShaderChTextureUnit * t = p->getTexture().create();
    t->getColourBlend()->setBlend(TBO_OPERAND1, Colour::Red, TBS_PRE_TEXTURE);
    t->getAlphaBlend()->setBlend(TBO_OPERAND1, 0.5f, TBS_PRE_TEXTURE);
	mScissorRect = mSceneMgr->createCustomGeo("mScissorRect");
	mScissorRect->on(GeometryObj::M_IdentityProj);
	mScissorRect->on(GeometryObj::M_IdentityView);
	mScissorRect->setCullType(CT_Always);
	mScissorRect->begin(GeometryRaw::OT_TRIANGLE_LIST, mat->getOriginID());
	mScissorRect->position(Vector3::ZERO);
	mScissorRect->position(Vector3::ZERO);
	mScissorRect->position(Vector3::ZERO);
	mScissorRect->quad(0, 1, 2, 3);
	mScissorRect->end();
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(mScissorRect);

	mCamera->setPos(NII::Vector3f(0,3000,5000));
	mCamera->lookAt(mProjectionSphere->getCenter());
}

bool PlayPen_testProjectSphere::onPreRender(const Ogre::RenderFrameArgs * evt)
{
	Rectf rect;
	mCamera->project(*mProjectionSphere, rect);

	mScissorRect->beginModify(0);
	mScissorRect->position(rect.mLeft, rect.mTop, 0);
	mScissorRect->position(rect.mLeft, rect.mBottom, 0);
	mScissorRect->position(rect.mRight, rect.mBottom, 0);
	mScissorRect->position(rect.mRight, rect.mTop, 0);
	mScissorRect->quad(0,1,2,3);
	mScissorRect->end();

	return PlayPenBase::onPreRender(evt);
}

//---------------------------------------------------------------------
PlayPen_testCameraSetDirection::PlayPen_testCameraSetDirection()
: mUseParentNode(false)
, mUseFixedYaw(true)
, mFocus(100,200,-300)
{
	mInfo["Title"] = "PlayPen: Camera Set Direction";
	mInfo["Description"] = "Testing various settings for Camera::setDirection";

}
void PlayPen_testCameraSetDirection::setupContent()
{
	mSceneMgr->setAmbient(Colour::White);

	GeometrySpace* e = mSceneMgr->createGeo("1", "knot.mesh");
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(mFocus))->attachObject(e);

	mCamera->setPos(NII::Vector3f(200,1000,1000));
	mCamera->lookAt(mFocus);

	mTrayMgr->createButton(OgreBites::TL_BOTTOM, "Look At", "Look At");
	mTrayMgr->createCheckBox(OgreBites::TL_BOTTOM, "tglParent", "Use Parent Node");
	OgreBites::CheckBox* chk = mTrayMgr->createCheckBox(OgreBites::TL_BOTTOM, "tglFixedYaw", "Use Fixed Yaw");
	chk->setChecked(true, false);
	mTrayMgr->showCursor();
	setDragLook(true);

	mParentNode = mSceneMgr->getOriginNode()->create(Vector3(1000, 2000, -1000));

}
void PlayPen_testCameraSetDirection::buttonHit(OgreBites::Button* button)
{
	mCamera->lookAt(mFocus);
}

void PlayPen_testCameraSetDirection::checkBoxToggled(OgreBites::CheckBox* box)
{
	if (box->getName() == "tglParent")
	{
		mUseParentNode = !mUseParentNode;

		if (mUseParentNode)
			mParentNode->attachObject(mCamera);
		else
			mParentNode->detachAll();
	}
	else if (box->getName() == "tglFixedYaw")
	{
		mUseFixedYaw = !mUseFixedYaw;
		if (mUseFixedYaw)
			mCamera->setCustomYaw(true);
		else
			mCamera->setCustomYaw(false);

	}
}
//---------------------------------------------------------------------
PlayPen_testManualLOD::PlayPen_testManualLOD()
{
	mInfo["Title"] = "PlayPen: Test Manual LOD";
	mInfo["Description"] = "Testing meshes with manual LODs assigned";
}
//---------------------------------------------------------------------
String PlayPen_testManualLOD::getLODMesh()
{
	MeshPtr msh1 = (MeshPtr)MeshManager::getSingleton().load("robot.mesh",
		ResourceSchemeManager::GroupDefault);
        
    MeshLodScheme * mls = msh1->getLodScheme();
    mls->add(200, mls->getDest(200), "razor.mesh", GroupUnknow);
    mls->add(500, mls->getDest(500), "sphere.mesh", GroupUnknow);
    mls->sort();

	return msh1->getName();
}
//---------------------------------------------------------------------
void PlayPen_testManualLOD::setupContent()
{
	String meshName = getLODMesh();

	GeometrySpace *ent;
	for (int i = 0; i < 5; ++i)
	{
		ent = mSceneMgr->createGeo("robot" + StrConv::conv(i), meshName);
		// Add entity to the scene node
		static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(
			Vector3(0,0,(i*50)-(5*50/2))))->attachObject(ent);
	}
	AnimationFusion* anim = ent->getFusion("Walk");
	anim->setEnabled(true);
	mAnimStateList.push_back(anim);


	// Give it a little ambience with lights
	Light* l;
	l = mSceneMgr->createLight("BlueLight");
	l->setPos(-200,-80,-100);
	l->setDiffuse(0.5, 0.5, 1.0);

	l = mSceneMgr->createLight("GreenLight");
	l->setPos(0,0,-100);
	l->setDiffuse(0.5, 1.0, 0.5);

	// Position the camera
	mCamera->setPos(NII::Vector3(100,50,100));
	mCamera->lookAt(NII::Vector3(-50,50,0));

	mSceneMgr->setAmbient(Colour::White);

}
//---------------------------------------------------------------------
PlayPen_testManualLODFromFile::PlayPen_testManualLODFromFile()
{
	mInfo["Title"] = "PlayPen: Test Manual LOD (file)";
	mInfo["Description"] = "Testing meshes with manual LODs assigned, loaded from a file";
}
//---------------------------------------------------------------------
String PlayPen_testManualLODFromFile::getLODMesh()
{
	MeshPtr msh1 = (MeshPtr)MeshManager::getSingleton().load("robot.mesh", GroupDefault);
    MeshLodScheme * mls = msh1->getLodScheme();
    mls->add(200, "razor.mesh", GroupUnknow, 0, false);
    mls->add(500, "sphere.mesh", GroupUnknow, 0, false);
    mls->sort();

	// this time, we save this data to a file and re-load it

	MeshSerializer ser;
	String prefix;
    VFS * vfs = N_Only(ResourceScheme).find("media", GroupDefault);
    if(vfs)
    {
		if(StringUtil::endsWith(vfs->getName(), "media"))
		{
			prefix = vfs->getName();
		}
	}
	ser.exportMesh(msh1.get(), prefix + "/testlod.mesh");

	MeshManager::getSingleton().removeAll();

	return "testlod.mesh";

}
//---------------------------------------------------------------------
PlayPen_testFullScreenSwitch::PlayPen_testFullScreenSwitch()
{
	mInfo["Title"] = "PlayPen: Test full screen";
	mInfo["Description"] = "Testing switching full screen modes without re-initialisation";

}
//---------------------------------------------------------------------
void PlayPen_testFullScreenSwitch::setupContent()
{
	m640x480w = mTrayMgr->createButton(TL_CENTER, "m640x480w", "640 x 480 (windowed)", 300);
	m640x480fs = mTrayMgr->createButton(TL_CENTER, "m640x480fs", "640 x 480 (fullscreen)", 300);
	m800x600w = mTrayMgr->createButton(TL_CENTER, "m800x600w", "800 x 600 (windowed)", 300);
	m800x600fs = mTrayMgr->createButton(TL_CENTER, "m800x600fs", "800 x 600 (fullscreen)", 300);
	m1024x768w = mTrayMgr->createButton(TL_CENTER, "m1024x768w", "1024 x 768 (windowed)", 300);
	m1024x768fs = mTrayMgr->createButton(TL_CENTER, "m1024x768fs", "1024 x 768 (fullscreen)", 300);

	mTrayMgr->showCursor();

}
//---------------------------------------------------------------------
void PlayPen_testFullScreenSwitch::buttonHit(OgreBites::Button * button)
{
	if (button == m640x480w)
		mWindow->setFullMode(false, 640, 480);
	else if (button == m640x480fs)
		mWindow->setFullMode(true, 640, 480);
	else if (button == m800x600w)
		mWindow->setFullMode(false, 800, 600);
	else if (button == m800x600fs)
		mWindow->setFullMode(true, 800, 600);
	else if (button == m1024x768w)
		mWindow->setFullMode(false, 1024, 768);
	else if (button == m1024x768fs)
		mWindow->setFullMode(true, 1024, 768);
}
//---------------------------------------------------------------------
PlayPen_testMorphAnimationWithNormals::PlayPen_testMorphAnimationWithNormals()
{
	mInfo["Title"] = "PlayPen: Morph anim (+normals)";
	mInfo["Description"] = "Testing morph animation with normals";
}
//---------------------------------------------------------------------
void PlayPen_testMorphAnimationWithNormals::setupContent()
{
	// Cannot change this to true, not possible to use software morph animation + normals with stencil shadows
	// because the former requires pos & normals to be in the same buffer, and the
	// latter requires positions to be on their own.
	bool testStencil = false;

	if(testStencil)
		mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);

	mSceneMgr->setAmbient(Colour(0.5, 0.5, 0.5));
	Vector3 dir(-1, -1, 0.5);
	dir.normalise();
	Light * l = mSceneMgr->createLight("light1");
	l->setType(LT_DIRECTIONAL);
	l->setDirection(dir);

	MeshPtr mesh = MeshManager::getSingleton().load("sphere.mesh", GroupDefault);

	String morphName = "testmorphwithnormals.mesh";
	mesh = mesh->clone(morphName);

	SubMesh * sm = mesh->getSub(0);
	// Re-organise geometry since this mesh has no animation and all
	// vertex elements are packed into one buffer
    sm->getVertexData()->build(VertexData::M_Animation | VertexData::M_Normals, true);
	if(testStencil)
		sm->getVertexData()->rebuild(M_ShadowVol); // need to re-prep since reorganised
	// get the position buffer (which should now be separate);
	const VertexUnit * posElem = sm->getVertexData()->get(VT_Pos);
	VertexBufferSPtr origbuf = sm->getVertexData()->getBuffer(posElem->mSource);

	// Create a new position & normal buffer with updated values
	VertexBuffer * newbuf;
    N_Only(GBuffer).create(newbuf, VertexUnit::getTypeSize(DT_Float3x) * 2,
        sm->getVertexData()->mCount, Buffer::Buffer::M_DEV | Buffer::M_HOST);
	float * pSrc = static_cast<float *>(origbuf->lock(Buffer::MM_READ));
	float * pDst = static_cast<float *>(newbuf->lock(Buffer::MM_WHOLE));

	// Make the sphere turn into a cube
	// Do this just by clamping each of the directions (we shrink it)
	float cubeDimension = 0.3f * mesh->getCenterRange();
	size_t srcSkip = origbuf->getUnitSize() / sizeof(float) - 3;
	for (size_t v = 0; v < sm->getVertexData()->mCount; ++v)
	{
		// x/y/z position
		Vector3 pos;

        if(*pSrc >= 0)
        {
            pos.x = std::min(cubeDimension, *pSrc++);
        }
        else
        {
            pos.x = std::max(-cubeDimension, *pSrc++);
        }
        *pDst++ = pos.x;

        if(*pSrc >= 0)
        {
            pos.y = std::min(cubeDimension, *pSrc++);
        }
        else
        {
            pos.y = std::max(-cubeDimension, *pSrc++);
        }
        *pDst++ = pos.y;

        if(*pSrc >= 0)
        {
            pos.z = std::min(cubeDimension, *pSrc++);
        }
        else
        {
            pos.z = std::max(-cubeDimension, *pSrc++);
        }
        *pDst++ = pos.z;

		// normal
		// this should point along the major axis
		// unfortunately since vertices are not duplicated at edges there will be
		// some inaccuracy here but the most important thing is to add sharp edges
		Vector3 norm;
        pos.normalise(norm);

        Real absx = Math::Abs(norm.x);
        Real absy = Math::Abs(norm.y);
        Real absz = Math::Abs(norm.z);
        if(absx > absy)
            if(absx > absz)
                norm = norm.x > 0 ? Vector3::X : Vector3::INV_X;
            else
                norm = norm.z > 0 ? Vector3::Z : Vector3::INV_Z;
        else // absx <= absy
            if(absy > absz)
                norm = norm.y > 0 ? Vector3::Y : Vector3::INV_Y;
            else
                norm = norm.z > 0 ? Vector3::Z : Vector3::INV_Z;

		*pDst++ = norm.x;
		*pDst++ = norm.y;
		*pDst++ = norm.z;

		pSrc += srcSkip;

	}

	origbuf->unlock();
	newbuf->unlock();

	// create a morph animation
	Animation * anim = mesh->createAnimation("testAnim", 10.0f);
    VertexUnitKeyFrameTrack * vt = N_new VertexUnitKeyFrameTrack(anim, 1);
    vt->setTarget(sm->getVertexData());
	// re-use start positions for frame 0
	VertexUnitKeyFrame* kf = vt->create(0);
	kf->setValue(origbuf);

	// Use translated buffer for mid frame
	kf = vt->create(4.0f);
	kf->setValue(newbuf);

	// Pause there
	kf = vt->create(6.0f);
	kf->setValue(newbuf);

	// re-use start positions for final frame
	kf = vt->create(10.0f);
	kf->setValue(origbuf);

	// Export the mesh
	DataStream * stream = N_Engine().createFile(morphName, GroupDefault, true);
	MeshSerializer ser;
	ser.exportMesh(mesh.get(), stream);
	stream->close();
    N_delete stream;

	// Unload old mesh to force reload
	MeshManager::getSingleton().remove(mesh->getPrc());
	mesh->unload();
	mesh.setNull();

	GeometrySpace* e = mSceneMgr->createGeo("test", morphName);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(e);
	AnimationFusion* animState = e->getFusion("testAnim");
	animState->setEnabled(true);
	animState->setWeight(1.0f);
	mAnimStateList.push_back(animState);

	e = mSceneMgr->createGeo("test2", morphName);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(200,0,0)))->attachObject(e);
	// test hardware morph
	e->setMaterialName("Examples/HardwareMorphAnimationWithNormals");
	animState = e->getFusion("testAnim");
	animState->setEnabled(true);
	animState->setWeight(1.0f);
	mAnimStateList.push_back(animState);

	mCamera->setNearClipDistance(0.5);
	mCamera->setPos(NII::Vector3f(0,100,-400));
	mCamera->lookAt(Vector3::ZERO);
	//mSceneMgr->getRenderPattern()->setDebugShadow(true);

	Plane plane;
	plane.normal = Vector3::Y;
	plane.d = 200;
	MeshManager::getSingleton().createPlane("Myplane",
		GroupDefault, plane,
		1500,1500,10,10,true,1,5,5,Vector3::Z);
	GeometrySpace* pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("2 - Default");
	pPlaneEnt->setCastShadow(false);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(pPlaneEnt);

}

//---------------------------------------------------------------------
PlayPen_testMorphAnimationWithoutNormals::PlayPen_testMorphAnimationWithoutNormals()
{
	mInfo["Title"] = "PlayPen: Morph anim (-normals)";
	mInfo["Description"] = "Testing morph animation without normals";
}
//---------------------------------------------------------------------
void PlayPen_testMorphAnimationWithoutNormals::setupContent()
{
	bool testStencil = false;

	if(testStencil)
		mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);

	mSceneMgr->setAmbient(Colour(0.5, 0.5, 0.5));
	Vector3 dir(-1, -1, 0.5);
	dir.normalise();
	Light* l = mSceneMgr->createLight("light1");
	l->setType(LT_DIRECTIONAL);
	l->setDirection(dir);

	MeshPtr mesh = MeshManager::getSingleton().load("sphere.mesh", GroupDefault);

	String morphName = "testmorphnonormals.mesh";
	mesh = mesh->clone(morphName);

	SubMesh * sm = mesh->getSub(0);
	// Re-organise geometry since this mesh has no animation and all
	// vertex elements are packed into one buffer
    sm->getVertexData()->build(VertexData::M_Animation, true);
	if(testStencil)
		sm->getVertexData()->rebuild(M_ShadowVol); // need to re-prep since reorganised
	// get the position buffer (which should now be separate);
	const VertexUnit * posElem = sm->getVertexData()->get(VT_Pos);
	VertexBufferSPtr origbuf = sm->getVertexData()->getBuffer(posElem->mSource);

	// Create a new position & normal buffer with updated values
	VertexBuffer * newbuf;
    N_Only(GBuffer).create(newbuf, VertexUnit::getTypeSize(DT_Float3x),
		sm->getVertexData()->mCount, Buffer::Buffer::M_DEV | Buffer::M_HOST);
	float * pSrc = static_cast<float*>(origbuf->lock(Buffer::MM_READ));
	float * pDst = static_cast<float*>(newbuf->lock(Buffer::MM_WHOLE));

	// Make the sphere turn into a cube
	// Do this just by clamping each of the directions (we shrink it)
	float cubeDimension = 0.3f * mesh->getCenterRange();
	for(size_t v = 0; v < sm->getVertexData()->mCount; ++v)
	{
		// x/y/z position
		Vector3 pos;
        
        if(*pSrc >= 0)
        {
            pos.x = std::min(cubeDimension, *pSrc++);
        }
        else
        {
            pos.x = std::max(-cubeDimension, *pSrc++);
        }
        *pDst++ = pos.x;
        
        if(*pSrc >= 0)
        {
            pos.y = std::min(cubeDimension, *pSrc++);
        }
        else
        {
            pos.y = std::max(-cubeDimension, *pSrc++);
        }
        *pDst++ = pos.y;
        
        if(*pSrc >= 0)
        {
            pos.z = std::min(cubeDimension, *pSrc++);
        }
        else
        {
            pos.z = std::max(-cubeDimension, *pSrc++);
        }
        *pDst++ = pos.z;
	}

	origbuf->unlock();
	newbuf->unlock();

	// create a morph animation
	Animation * anim = mesh->createAnimation("testAnim", 10.0f);
    VertexUnitKeyFrameTrack * vt = N_new VertexUnitKeyFrameTrack(anim, 1);
    vt->setTarget(sm->getVertexData());
	// re-use start positions for frame 0
	VertexUnitKeyFrame * kf = vt->create(0);
	kf->setValue(origbuf);

	// Use translated buffer for mid frame
	kf = vt->create(4.0f);
	kf->setValue(newbuf);

	// Pause there
	kf = vt->create(6.0f);
	kf->setValue(newbuf);

	// re-use start positions for final frame
	kf = vt->create(10.0f);
	kf->setValue(origbuf);

	// Export the mesh
	DataStream * stream = Root::getSingleton().createFile(morphName, GroupDefault, true);
	MeshSerializer ser;
	ser.exportMesh(mesh.get(), stream);
	stream->close();
    N_delete stream;

	// Unload old mesh to force reload
	MeshManager::getSingleton().remove(mesh->getPrc());
	mesh->unload();
	mesh.setNull();

	GeometrySpace* e = mSceneMgr->createGeo("test", morphName);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(e);
	AnimationFusion* animState = e->getFusion("testAnim");
	animState->setEnabled(true);
	animState->setWeight(1.0f);
	mAnimStateList.push_back(animState);

	e = mSceneMgr->createGeo("test2", morphName);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(200,0,0)))->attachObject(e);
	// test hardware morph
	e->setMaterialName("Examples/HardwareMorphAnimation");
	animState = e->getFusion("testAnim");
	animState->setEnabled(true);
	animState->setWeight(1.0f);
	mAnimStateList.push_back(animState);

	mCamera->setNearClipDistance(0.5);
	mCamera->setPos(NII::Vector3f(0,100,-400));
	mCamera->lookAt(Vector3::ZERO);
	//mSceneMgr->getRenderPattern()->setDebugShadow(true);

	Plane plane;
	plane.normal = Vector3::Y;
	plane.d = 200;
	MeshManager::getSingleton().createPlane("Myplane",
		GroupDefault, plane, 1500,1500,10,10,true,1,5,5,Vector3::Z);
	GeometrySpace * pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("2 - Default");
	pPlaneEnt->setCastShadow(false);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(pPlaneEnt);
}
//---------------------------------------------------------------------
PlayPen_testPoseAnimationWithNormals::PlayPen_testPoseAnimationWithNormals()
{
	mInfo["Title"] = "PlayPen: VertexOffset anim (+normals)";
	mInfo["Description"] = "Testing pose animation with normals";

}
//---------------------------------------------------------------------
void PlayPen_testPoseAnimationWithNormals::setupContent()
{
	mSceneMgr->setAmbient(Colour(0.5, 0.5, 0.5));
	Vector3 dir(-1, -1, 0.5);
	dir.normalise();
	Light* l = mSceneMgr->createLight("light1");
	l->setType(LT_DIRECTIONAL);
	l->setDirection(dir);

	MeshPtr mesh = MeshManager::getSingleton().load("cube.mesh", GroupDefault);

	String newName = "testposewithnormals.mesh";
	mesh = mesh->clone(newName);

	SubMesh* sm = mesh->getSub(0);
	// Re-organise geometry since this mesh has no animation and all
	// vertex elements are packed into one buffer
    sm->getVertexData()->build(VertexData::M_Animation | VertexData::M_Normals, true);
	// create 2 poses
	VertexOffset* pose = mesh->createOffset("pose1", 1);
	// Pose1 moves vertices 0, 1, 2 and 3 upward and pushes normals left
	Vector3 offset1(0, 50, 0);
	pose->add(0, offset1, Vector3::INV_X);
	pose->add(1, offset1, Vector3::INV_X);
	pose->add(2, offset1, Vector3::INV_X);
	pose->add(3, offset1, Vector3::INV_X);

	pose = mesh->createOffset("pose2", 1);
	// Pose2 moves vertices 3, 4, and 5 to the right and pushes normals right
	// Note 3 gets affected by both
	Vector3 offset2(100, 0, 0);
	pose->add(3, offset2, Vector3::X);
	pose->add(4, offset2, Vector3::X);
	pose->add(5, offset2, Vector3::X);

	Animation* anim = mesh->createAnimation("poseanim", 20.0f);
    VertexOffsetKeyFrameTrack * vt = N_new VertexOffsetKeyFrameTrack(anim, 1);
    vt->setTarget(sm->getVertexData());

	// Frame 0 - no effect
	VertexPoseKeyFrame* kf = vt->createVertexPoseKeyFrame(0);

	// Frame 1 - bring in pose 1 (index 0)
	kf = vt->createVertexPoseKeyFrame(3);
	kf->add(0, 1.0f);

	// Frame 2 - remove all
	kf = vt->createVertexPoseKeyFrame(6);

	// Frame 3 - bring in pose 2 (index 1)
	kf = vt->createVertexPoseKeyFrame(9);
	kf->add(1, 1.0f);

	// Frame 4 - remove all
	kf = vt->createVertexPoseKeyFrame(12);


	// Frame 5 - bring in pose 1 at 50%, pose 2 at 100%
	kf = vt->createVertexPoseKeyFrame(15);
	kf->add(0, 0.5f);
	kf->add(1, 1.0f);

	// Frame 6 - bring in pose 1 at 100%, pose 2 at 50%
	kf = vt->createVertexPoseKeyFrame(18);
	kf->add(0, 1.0f);
	kf->add(1, 0.5f);

	// Frame 7 - reset
	kf = vt->createVertexPoseKeyFrame(20);


	// Export the mesh
	DataStream * stream = Root::getSingleton().createFile(newName, GroupDefault, true);
	MeshSerializer ser;
	ser.exportMesh(mesh.get(), stream);
	stream->close();
    N_delete stream;
	// Unload old mesh to force reload
	MeshManager::getSingleton().remove(mesh->getPrc());
	mesh->unload();
	mesh.setNull();

	GeometrySpace*  e;
	AnimationFusion* animState;
	// software pose
	e = mSceneMgr->createGeo("test2", newName);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(150,0,0)))->attachObject(e);
	animState = e->getFusion("poseanim");
	animState->setEnabled(true);
	animState->setWeight(1.0f);
	mAnimStateList.push_back(animState);

	// test hardware pose
	e = mSceneMgr->createGeo("test", newName);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(e);
	e->setMaterialName("Examples/HardwarePoseAnimationWithNormals");
	animState = e->getFusion("poseanim");
	animState->setEnabled(true);
	animState->setWeight(1.0f);
	mAnimStateList.push_back(animState);


	mCamera->setNearClipDistance(0.5);
	mSceneMgr->getRenderPattern()->setDebugShadow(true);

	Plane plane;
	plane.normal = Vector3::Y;
	plane.d = 200;
	MeshManager::getSingleton().createPlane("Myplane",
		GroupDefault, plane,
		1500,1500,10,10,true,1,5,5,Vector3::Z);
	GeometrySpace* pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("2 - Default");
	pPlaneEnt->setCastShadow(false);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(pPlaneEnt);

	mCamera->setPos(NII::Vector3f(0, -200, -300));
	mCamera->lookAt(NII::Vector3f(0, 0, 0));
}
//---------------------------------------------------------------------
PlayPen_testPoseAnimationWithoutNormals::PlayPen_testPoseAnimationWithoutNormals()
{
	mInfo["Title"] = "PlayPen: VertexOffset anim (-normals)";
	mInfo["Description"] = "Testing pose animation without normals";

}
//---------------------------------------------------------------------
void PlayPen_testPoseAnimationWithoutNormals::setupContent()
{
	mSceneMgr->setAmbient(Colour(0.5, 0.5, 0.5));
	Vector3 dir(-1, -1, 0.5);
	dir.normalise();
	Light * l = mSceneMgr->createLight("light1");
	l->setType(LT_DIRECTIONAL);
	l->setDirection(dir);

	MeshPtr mesh = MeshManager::getSingleton().load("cube.mesh", GroupDefault);

	String newName = "testposenonormals.mesh";
	mesh = mesh->clone(newName);

	SubMesh * sm = mesh->getSub(0);
	// Re-organise geometry since this mesh has no animation and all
	// vertex elements are packed into one buffer
    sm->getVertexData()->build(VertexData::M_Animation, true);
	// create 2 poses
	VertexOffset * pose = mesh->createOffset("pose1", 1);
	// Pose1 moves vertices 0, 1, 2 and 3 upward
	Vector3 offset1(0, 50, 0);
	pose->add(0, offset1);
	pose->add(1, offset1);
	pose->add(2, offset1);
	pose->add(3, offset1);

	pose = mesh->createOffset("pose2", 1);
	// Pose2 moves vertices 3, 4, and 5 to the right
	// Note 3 gets affected by both
	Vector3 offset2(100, 0, 0);
	pose->add(3, offset2);
	pose->add(4, offset2);
	pose->add(5, offset2);

	Animation * anim = mesh->createAnimation("poseanim", 20.0f);
    VertexOffsetKeyFrameTrack * vt = N_new VertexOffsetKeyFrameTrack(anim, 1);
    vt->setTarget(sm->getVertexData());

	// Frame 0 - no effect
	VertexPoseKeyFrame* kf = vt->createVertexPoseKeyFrame(0);

	// Frame 1 - bring in pose 1 (index 0)
	kf = vt->createVertexPoseKeyFrame(3);
	kf->add(0, 1.0f);

	// Frame 2 - remove all
	kf = vt->createVertexPoseKeyFrame(6);

	// Frame 3 - bring in pose 2 (index 1)
	kf = vt->createVertexPoseKeyFrame(9);
	kf->add(1, 1.0f);

	// Frame 4 - remove all
	kf = vt->createVertexPoseKeyFrame(12);


	// Frame 5 - bring in pose 1 at 50%, pose 2 at 100%
	kf = vt->createVertexPoseKeyFrame(15);
	kf->add(0, 0.5f);
	kf->add(1, 1.0f);

	// Frame 6 - bring in pose 1 at 100%, pose 2 at 50%
	kf = vt->createVertexPoseKeyFrame(18);
	kf->add(0, 1.0f);
	kf->add(1, 0.5f);

	// Frame 7 - reset
	kf = vt->createVertexPoseKeyFrame(20);

	// Export the mesh
	DataStream * stream = Root::getSingleton().createFile(newName, GroupDefault, true);
	MeshSerializer ser;
	ser.exportMesh(mesh.get(), stream);
	stream->close();
    N_delete stream;
	// Unload old mesh to force reload
	MeshManager::getSingleton().remove(mesh->getPrc());
	mesh->unload();
	mesh.setNull();

	GeometrySpace*  e;
	AnimationFusion* animState;
	// software pose
	e = mSceneMgr->createGeo("test2", newName);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(150,0,0)))->attachObject(e);
	animState = e->getFusion("poseanim");
	animState->setEnabled(true);
	animState->setWeight(1.0f);
	mAnimStateList.push_back(animState);

	// test hardware pose
	e = mSceneMgr->createGeo("test", newName);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(e);
	e->setMaterialName("Examples/HardwarePoseAnimation");
	animState = e->getFusion("poseanim");
	animState->setEnabled(true);
	animState->setWeight(1.0f);
	mAnimStateList.push_back(animState);


	mCamera->setNearClipDistance(0.5);

	Plane plane;
	plane.normal = Vector3::Y;
	plane.d = 200;
	MeshManager::getSingleton().createPlane("Myplane",
		GroupDefault, plane,
		1500,1500,10,10,true,1,5,5,Vector3::Z);
	GeometrySpace* pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("2 - Default");
	pPlaneEnt->setCastShadow(false);
	static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(pPlaneEnt);

	mCamera->setPos(NII::Vector3f(0, -200, -300));
	mCamera->lookAt(NII::Vector3f(0, 0, 0));
}