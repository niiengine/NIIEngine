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
// Original author: Tels <http://bloodgate.com>, released as public domain
#include "OgreAreaEmitter.h"
#include "OgreParticle.h"
#include "OgreQuaternion.h"
#include "OgreException.h"
#include "OgreStringConverter.h"



namespace Ogre {

    // Instatiate statics
    AreaEmitter::CmdWidth AreaEmitter::msWidthCmd;
    AreaEmitter::CmdHeight AreaEmitter::msHeightCmd;
    AreaEmitter::CmdDepth AreaEmitter::msDepthCmd;

    //-----------------------------------------------------------------------
    bool AreaEmitter::initDefaults(const String& t)
    {
        // called by the constructor as initDefaults("Type")

        // Defaults
        mDirection = Vector3::Z;
        mUp = Vector3::Y;
        setSize(100,100,100);
        mType = t;

        // Set up parameters
        if(createParamDictionary(mType + "Emitter"))
        {
            PropertyCmdSet * dict = getCmdObj();

            // Custom params
            dict->addParameter(ParameterDef("width",
                "Width of the shape in world coordinates.",
                PT_REAL),&msWidthCmd);
            dict->addParameter(ParameterDef("height",
                "Height of the shape in world coordinates.",
                PT_REAL),&msHeightCmd);
            dict->addParameter(ParameterDef("depth",
                "Depth of the shape in world coordinates.",
                PT_REAL),&msDepthCmd);
            return true;

        }
        return false;
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setDirection(const Vector3 & inDirection)
    {
        SpreadParticle::setDirection(inDirection);

        // Update the ranges
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setSize(const Vector3 & size)
    {
        mSize = size;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setSize(NIIf x, NIIf y, NIIf z)
    {
        mSize.x = x;
        mSize.y = y;
        mSize.z = z;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setWidth(NIIf width)
    {
        mSize.x = width;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    NIIf AreaEmitter::getWidth(void) const
    {
        return mSize.x;
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setHeight(NIIf height)
    {
        mSize.y = height;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    NIIf AreaEmitter::getHeight(void) const
    {
        return mSize.y;
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setDepth(NIIf depth)
    {
        mSize.z = depth;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    NIIf AreaEmitter::getDepth(void) const
    {
        return mSize.z;
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::genAreaAxes(void)
    {
        Vector3 mLeft = mUp.crossProduct(mDirection);

        mXRange = mLeft * (mSize.x * 0.5f);
        mYRange = mUp * (mSize.y * 0.5f);
        mZRange = mDirection * (mSize.z * 0.5f);
    }

    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String AreaEmitter::CmdWidth::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const AreaEmitter*>(target)->getWidth() );
    }
    void AreaEmitter::CmdWidth::doSet(void* target, const String& val)
    {
        static_cast<AreaEmitter*>(target)->setWidth(StringConverter::parseReal(val));
    }
    //-----------------------------------------------------------------------
    String AreaEmitter::CmdHeight::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const AreaEmitter*>(target)->getHeight() );
    }
    void AreaEmitter::CmdHeight::doSet(void* target, const String& val)
    {
        static_cast<AreaEmitter*>(target)->setHeight(StringConverter::parseReal(val));
    }
    //-----------------------------------------------------------------------
    String AreaEmitter::CmdDepth::doGet(const void* target) const
    {
        return StrConv::conv(
            static_cast<const AreaEmitter*>(target)->getDepth() );
    }
    void AreaEmitter::CmdDepth::doSet(void* target, const String& val)
    {
        static_cast<AreaEmitter*>(target)->setDepth(StringConverter::parseReal(val));
    }



}


