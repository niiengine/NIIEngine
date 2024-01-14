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

#include "MaterialControls.h"
#include "OgreLogManager.h"
#include "OgreStringList.h"
#include "OgreStringConverter.h"
#include "OgreConfigFile.h"
#include "OgreResourceLoadManager.h"
#include "OgreException.h"
#include "OgreMaterial.h"
#include "OgreTechnique.h"
#include "OgreMaterialManager.h"

/********************************************************************************
            MaterialControls Methods
*********************************************************************************/
void MaterialControls::addControl(const Ogre::String& params)
{
    // params is a string containing using the following format:
    //  "<Control Name>, <Shader parameter name>, <Parameter Type>, <Min Val>, <Max Val>, <Parameter Sub Index>"

    // break up long string into components
    Ogre::StringList vecparams = Ogre::StringUtil::split(params, ",");

    // if there are not five elements then log error and move on
    if (vecparams.size() != 6)
    {
        Ogre::LogManager::getSingleton().logMessage(
            "Incorrect number of parameters passed in params string for MaterialControls::addControl()" );

        return;
    }

    ShaderControl newControl;
    Ogre::StringUtil::trim(vecparams[0]);
    newControl.Name = vecparams[0];

    Ogre::StringUtil::trim(vecparams[1]);
    newControl.ParamName = vecparams[1];

    Ogre::StringUtil::trim(vecparams[2]);
    if (vecparams[2] == "GPU_VERTEX")
        newControl.ValType = GPU_VERTEX;
    else if (vecparams[2] == "GPU_FRAGMENT")
        newControl.ValType = GPU_FRAGMENT;

    newControl.MinVal = Ogre::StringConverter::parseReal(vecparams[3]);
    newControl.MaxVal = Ogre::StringConverter::parseReal(vecparams[4]);
    newControl.ElementIndex = Ogre::StringConverter::parseInt(vecparams[5]);

    mShaderControlsContainer.push_back(newControl);
}

void loadMaterialControlsFile(MaterialControlsContainer & controlsContainer, const Ogre::String& filename)
{
    // Load material controls from config file
    Ogre::ConfigFile cf;

    try
    {
        cf.load(filename, Ogre::GroupDefault, "\t;=", true);

        // Go through all sections & controls in the file
        Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

        Ogre::String secName, typeName, materialName, dataString;

        while(seci.isNextValid())
        {
            secName = seci.getCurrentKey();
            Ogre::ConfigFile::SettingsMultiMap * settings = seci.getAndNext();
            if(!secName.empty() && settings)
            {
                materialName = cf.getSetting("material", secName);

				NII::MaterialPtr curMat = NII::MaterialManager::getSingleton().getByName(materialName);
				curMat->load();
				NII::ShaderFusion * curTec = curMat->getLOD();
				if(!curTec || !curTec->isValid())
				{
					continue;
				}

                MaterialControls newMaaterialControls(secName, materialName);
                controlsContainer.push_back(newMaaterialControls);

                size_t idx = controlsContainer.size() - 1;

                Ogre::ConfigFile::SettingsMultiMap::iterator i;

                for (i = settings->begin(); i != settings->end(); ++i)
                {
                    typeName = i->first;
                    dataString = i->second;
                    if (typeName == "control")
                        controlsContainer[idx].addControl(dataString);
                }
            }
        }
	    Ogre::LogManager::getSingleton().logMessage( "Material Controls setup" );
    }
    catch (Ogre::Exception e)
    {
        // Guess the file didn't exist
    }
}

void loadAllMaterialControlFiles(MaterialControlsContainer & controlsContainer)
{
    Ogre::StringList fileStringList;
    NII::N_Only(ResourceScheme).findResourceNames(fileStringList, "Popular", "*.controls");
	Ogre::StringList::iterator controlsFileNameIterator = fileStringList->begin();

    while(controlsFileNameIterator != fileStringList->end())
	{
        loadMaterialControlsFile(controlsContainer, *controlsFileNameIterator);
        ++controlsFileNameIterator;
	}
}
