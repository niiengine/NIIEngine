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

#include "NiiPreProcess.h"
#include "NiiLightFactory.h"
#include "NiiFactoryList.h"
#include "NiiPropertyList.h"
#include "NiiLight.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // LightFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    LightFactory::LightFactory()
    {
    }
    //-----------------------------------------------------------------------
    LightFactory::~LightFactory()
    {
    }
    //-----------------------------------------------------------------------
    FactoryID LightFactory::getID() const
    {
        return N_FACTORY_Light;
    }
    //-----------------------------------------------------------------------
    SpaceObj * LightFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
    {
        Light * light = N_new Light(sid);
        light->setFactory(this);
        light->setManager(mag);
        if(params)
        {
            const PropertyData::PropertyList & plist = params->getList();
            PropertyData::PropertyList::const_iterator ni;
            // Setting the light type first before any property specific to a certain light type
            if((ni = plist.find(N_PropertyLight_Type)) != plist.end())
            {
                if(ni->second == _T("point"))
                    light->setType(LT_POINT);
                else if(ni->second == _T("directional"))
                    light->setType(LT_DIRECTIONAL);
                else if(ni->second == _T("spot"))
                    light->setType(LT_SPOT);
                else
                    N_EXCEPT(InvalidParam, _I18n("Invalid light type '") + ni->second + _T("'."));
            }

            // Common properties
            if((ni = plist.find(N_PropertyLight_Position)) != plist.end())
            {
                Vector3f temp;
                StrConv::conv(ni->second, temp);
                light->setPos(temp);
            }

            if((ni = plist.find(N_PropertyLight_Direction)) != plist.end())
            {
                Vector3f temp;
                StrConv::conv(ni->second, temp);
                light->setDirection(temp);
            }

            if((ni = plist.find(N_PropertyLight_Diffuse)) != plist.end())
            {
                Colour temp;
                StrConv::conv(ni->second, temp);
                light->setDiffuse(temp);
            }

            if((ni = plist.find(N_PropertyLight_Specular)) != plist.end())
            {
                Colour temp;
                StrConv::conv(ni->second, temp);
                light->setSpecular(temp);
            }
            if((ni = plist.find(N_PropertyLight_Attenuation)) != plist.end())
            {
                Vector4f attenuation;
                StrConv::conv(ni->second, attenuation);
                light->setRange(attenuation.x);
                light->setAttenuationConstant(attenuation.y);
                light->setAttenuationLinear(attenuation.z);
                light->setAttenuationQuadric(attenuation.w);
            }

            if((ni = plist.find(N_PropertyLight_CastShadows)) != plist.end())
            {
                bool temp;
                StrConv::conv(ni->second, temp);
                light->setCastShadow(temp);
            }

            if((ni = plist.find(N_PropertyLight_Visible)) != plist.end())
            {
                bool temp;
                StrConv::conv(ni->second, temp);
                light->setVisible(temp);
            }

            NIIf temp;

            if((ni = plist.find(N_PropertyLight_PowerScale)) != plist.end())
            {
                StrConv::conv(ni->second, temp);
                light->setBrightFactor(temp);
            }

            if((ni = plist.find(N_PropertyLight_ShadowFarDistance)) != plist.end())
            {
                StrConv::conv(ni->second, temp);
                light->setShadowExtent(temp);
            }

            Radian temp2;

            // Spotlight properties
            if((ni = plist.find(N_PropertyLight_SpotlightInner)) != plist.end())
            {
                StrConv::conv(ni->second, temp2);
                light->setSpotlightInner(temp2);
            }

            if((ni = plist.find(N_PropertyLight_SpotlightOuter)) != plist.end())
            {
                StrConv::conv(ni->second, temp2);
                light->setSpotlightOuter(temp2);
            }
            if((ni = plist.find(N_PropertyLight_SpotlightFalloff)) != plist.end())
            {
                StrConv::conv(ni->second, temp);
                light->setSpotlightFalloff(temp);
            }
        }

        return light;
    }
    //-----------------------------------------------------------------------
    void LightFactory::destroy(void * obj)
    {
        Light * temp = static_cast<Light *>(obj);
        N_delete temp;
    }
    //------------------------------------------------------------------------
}