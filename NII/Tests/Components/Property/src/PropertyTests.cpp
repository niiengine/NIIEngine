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
#include "PropertyTests.h"

CPPUNIT_TEST_SUITE_REGISTRATION( PropertyTests );

void PropertyTests::setUp()
{
}

void PropertyTests::tearDown()
{
}

class Foo
{
protected:
	String mName;
public:
	void setName(const String & name) { mName = name; }
	const String & getName() const { return mName; }
};

void PropertyTests::testStringProp()
{
	PropertyCmdMap propertyDefs;
	Foo foo;
	PropertySet props;

	PropertyCmdMap::iterator defi = propertyDefs.insert(PropertyCmdMap::value_type("name", 
        PropertyCmd("name", "The name of the object.", PROP_STRING))).first;

	props.addProperty(
		N_new Property<String>(&(defi->second),
		boost::bind(&Foo::getName, &foo), 
		boost::bind(&Foo::setName, &foo, _1)));

	Ogre::String strName, strTest;
	strTest = "A simple name";
	props.setValue("name", strTest);
	props.getValue("name", strName);

	CPPUNIT_ASSERT_EQUAL(strTest, strName);
}