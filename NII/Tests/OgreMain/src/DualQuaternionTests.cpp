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
#include "DualQuaternionTests.h"
#include "OgreDualQuaternion.h"
#include "OgreVector3.h"
#include "OgreMatrix4.h"

// Register the suite
CPPUNIT_TEST_SUITE_REGISTRATION( DualQuaternionTests );

using namespace Ogre;

void DualQuaternionTests::setUp()
{
}

void DualQuaternionTests::tearDown()
{
}

void DualQuaternionTests::testConversion()
{
	DualQuaternion dQuat;
	Quaternion quat(Radian(Angle(60)), Vector3::Y);
	Vector3 translation(0, 0, 10);
	dQuat.from(quat, translation);
		
	Quaternion result;
	Vector3 resTrans;
	dQuat.to(result, resTrans);

	CPPUNIT_ASSERT_EQUAL(result, quat);
	CPPUNIT_ASSERT(resTrans.equal(translation));
}

void DualQuaternionTests::testDefaultValue()
{
	DualQuaternion dQuatDefault;
	
	Quaternion quatDefault;
	Vector3 transDefault;
	
	dQuatDefault.to(quatDefault, transDefault);

	CPPUNIT_ASSERT_EQUAL(quatDefault, Quaternion::IDENTITY); 
	CPPUNIT_ASSERT(transDefault.equal(Vector3::ZERO));
}

void DualQuaternionTests::testMatrix()
{
    Matrix4 transform;
	Vector3 translation(10, 4, 0);
	Vector3 scale = Vector3::UNIT;
	Quaternion rotation;
	rotation.from(Radian(Math::PI), Vector3::Z);
	transform.set(translation, scale, rotation);

	DualQuaternion dQuat;
	dQuat.from(transform);
	Matrix4 transformResult;
	dQuat.to(transformResult);

	Vector3 translationResult;
	Vector3 scaleResult;
	Quaternion rotationResult;
	transformResult.get(translationResult, scaleResult, rotationResult);

	CPPUNIT_ASSERT(translationResult.equal(translation));
	CPPUNIT_ASSERT(scaleResult.equal(scale));
	CPPUNIT_ASSERT(rotationResult.equals(rotation, Radian(0.001)));
}