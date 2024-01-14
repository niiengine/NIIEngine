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

#ifndef MACROMANAGER_H
#define MACROMANAGER_H

#include "gsgsPreInclude.h"

namespace gsgs
{
    class _gsgsAPI MacroManager
    {
    public:
        static MacroManager * Instance();
    public:
        /*
         * The following macro will be expanded into their real values:
         * $(ProjectPath)
         * $(WorkspacePath)
         * $(ProjectName)
         * $(IntermediateDirectory)
         * $(ConfigurationName)
         * $(OutDir)
         * $(ProjectFiles)
         * $(ProjectFiles)
         * $(ProjectFilesAbs)
         * $(CurrentFileName)
         * $(CurrentFilePath)
         * $(CurrentFileExt)
         * $(CurrentFileFullPath)
         * $(User)
         * $(Date)
         * $(gsgsPath)
         * $(CurrentSelection)
         * $(ProjectOutputFile)
         * $(Selection)
         */
        wxString Expand(const wxString& expression, IManager * manager, const wxString & project, const wxString & confToBuild = wxEmptyString);

        /**
         * @brief same as above, but don't apply environment variables
         */
        wxString ExpandNoEnv(const wxString & expression, const wxString & project, const wxString & confToBuild = wxEmptyString);

        /**
         * @brief search for variableName and replace all its occurance with 'replaceWith'
         * This function supports the following formats:
         * $variableName
         * ${variableName}
         * $(variableName)
         * %variableName%
         */
        wxString Replace(const wxString & inString, const wxString& variableName, const wxString & replaceWith, bool bIgnoreCase = false);

        /**
         * @brief search for variable in the inString.
         * @param inString [input]
         * @param name [output] name only
         * @param fullname [output] includes the $ / %%
         */
        bool FindVariable(const wxString & inString, wxString & name, wxString & fullname);
    private:
        MacroManager();
        virtual ~MacroManager();

        wxString DoExpand(const wxString& expression, IManager* manager, const wxString& project, bool applyEnv,
            const wxString& confToBuild = wxEmptyString);
    };
}
#define gsgs_Macro() gsgs::MacroManager::singleton()
#endif
