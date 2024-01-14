
/* ************************************************************************ */
/*                                                                          */
/* CMakePlugin for Codelite                                                 */
/* Copyright (C) 2013 Jiří Fatka <ntsfka@gmail.com>                         */
/*                                                                          */
/* This program is free software: you can redistribute it and/or modify     */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This program is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program. If not, see <http://www.gnu.org/licenses/>.     */
/*                                                                          */
/* ************************************************************************ */

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// Declaration
#include "CMakeSettingsManager.h"

// wxWidgets
#include <wx/arrstr.h>

// Codelite
#include "gsgsJSON.h"
#include "gsgsWorkspace.h"

// CMakePlugin
#include "CMakePlugin.h"

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */
CMakeSettingsManager::CMakeSettingsManager(CMakePlugin* plugin)
    : m_plugin(plugin)
{
    // Nothing to do
}
/* ************************************************************************ */
CMakeProjectSettingsMap * CMakeSettingsManager::GetProjectSettings(const wxString& project, bool create)
{
    if (create) {
        return &(m_projectSettings[project]);
    } else {
        std::map<wxString, CMakeProjectSettingsMap>::iterator it = m_projectSettings.find(project);

        if (it == m_projectSettings.end())
            return NULL;

        return &(it->second);
    }
}
/* ************************************************************************ */

const CMakeProjectSettingsMap*
CMakeSettingsManager::GetProjectSettings(const wxString& project) const
{
    std::map<wxString, CMakeProjectSettingsMap>::const_iterator it = m_projectSettings.find(project);

    if (it == m_projectSettings.end())
        return NULL;

    return &(it->second);
}

/* ************************************************************************ */

CMakeProjectSettings*
CMakeSettingsManager::GetProjectSettings(const wxString& project, const wxString& config, bool create)
{
    // Get project settings
    CMakeProjectSettingsMap* settings = GetProjectSettings(project, create);

    if (create) {
        // GetProjectSettings should create the new one
        wxASSERT(settings);

        // Find or create configuration
        return &(*settings)[config];
    } else {
        // Not found
        if (!settings)
            return NULL;

        // Find configuration
        CMakeProjectSettingsMap::iterator it = settings->find(config);

        // Not found
        if (it == settings->end())
            return NULL;

        return &(it->second);
    }
}

/* ************************************************************************ */

const CMakeProjectSettings*
CMakeSettingsManager::GetProjectSettings(const wxString& project, const wxString& config) const
{
    // Get project settings
    const CMakeProjectSettingsMap* settings = GetProjectSettings(project);

    // Not found
    if (!settings)
        return NULL;

    // Find configuration
    CMakeProjectSettingsMap::const_iterator it = settings->find(config);

    // Not found
    if (it == settings->end())
        return NULL;

    return &(it->second);
}

/* ************************************************************************ */

bool
CMakeSettingsManager::IsProjectEnabled(const wxString& project, const wxString& config) const
{
    // Get project settings
    const CMakeProjectSettings* settings = GetProjectSettings(project, config);

    return settings && settings->enabled;
}

/* ************************************************************************ */

void
CMakeSettingsManager::SaveProjects()
{
    CxxWS* workspace = m_plugin->GetManager()->GetWorkspace();
    wxASSERT(workspace);

    wxArrayString projects;
    workspace->GetProjectList(projects);

    for (wxArrayString::const_iterator it = projects.begin(),
        ite = projects.end(); it != ite; ++it) {
        SaveProject(*it);
    }
}

/* ************************************************************************ */

void
CMakeSettingsManager::SaveProject(const wxString& name)
{
    CxxWS* workspace = m_plugin->GetManager()->GetWorkspace();
    wxASSERT(workspace);

    wxString err;
    ProjectPtr project = workspace->FindProjectByName(name, err);

    if (!project)
        return;

    // Find project settings
    std::map<wxString, CMakeProjectSettingsMap>::const_iterator itSettings = m_projectSettings.find(name);

    // Ehm...
    if (itSettings == m_projectSettings.end())
        return;

    // Create JSON object
    JSONObject json = JSONObject::createArray("configurations");

    // Foreach settings
    for (std::map<wxString, CMakeProjectSettings>::const_iterator it = itSettings->second.begin(),
        ite = itSettings->second.end(); it != ite; ++it) {
        // Get settings
        const CMakeProjectSettings& settings = it->second;

        // Create item
        JSONObject item = JSONObject::createObject("configuration");

        // Store name
        item.addProperty("name", it->first);

        // Store settings
        item.addProperty("enabled", settings.enabled);
        item.addProperty("buildDirectory", settings.buildDirectory);
        item.addProperty("sourceDirectory", settings.sourceDirectory);
        item.addProperty("generator", settings.generator);
        item.addProperty("buildType", settings.buildType);
        item.addProperty("arguments", settings.arguments);
        item.addProperty("parentProject", settings.parentProject);

        // Add array
        json.arrayAppend(item);
    }

    // Must be an array
    wxASSERT(json.getType() == cJSON_Array);

    // Store plugin data
    project->SetPluginData("CMakePlugin", json.getString());
}

/* ************************************************************************ */

void
CMakeSettingsManager::LoadProjects()
{
    CxxWS* workspace = m_plugin->GetManager()->GetWorkspace();
    wxASSERT(workspace);

    wxArrayString projects;
    workspace->GetProjectList(projects);

    for (wxArrayString::const_iterator it = projects.begin(),
        ite = projects.end(); it != ite; ++it) {
        LoadProject(*it);
    }
}

/* ************************************************************************ */

void
CMakeSettingsManager::LoadProject(const wxString& name)
{
    CxxWS* workspace = m_plugin->GetManager()->GetWorkspace();
    wxASSERT(workspace);

    wxString err;
    ProjectPtr project = workspace->FindProjectByName(name, err);

    if (!project)
        return;

    // Find for project or create new one
    CMakeProjectSettingsMap* projectSettings = GetProjectSettings(name, true);
    wxASSERT(projectSettings);

    // Alias for map
    CMakeProjectSettingsMap& settingsMap = *projectSettings;

    // Load JSON string
    const wxString jsonStr = project->GetPluginData("CMakePlugin");

    // Create JSON object
    JSON jsonRoot(jsonStr);
    // JSON cannot be temporary, because destructor deletes cJSON object.
    JSONObject json = jsonRoot.getObject();

    // Unable to parse
    if (!json.isValid())
        return;

    // Expected array with config names
    if (json.getType() != cJSON_Array)
        return;

    // Foreach array
    for (int i = 0; i < json.getArraySize(); ++i) {
        // Get item
        JSONObject item = json.getArray(i);

        // Name
        wxString name;
        item.getObject("name").getValue(name);

        // (Create and) get settings
        CMakeProjectSettings& settings = settingsMap[name];
        item.getObject("enabled").getValue(settings.enabled);
        item.getObject("buildDirectory").getValue(settings.buildDirectory, "build");
        item.getObject("sourceDirectory").getValue(settings.sourceDirectory, "build");
        item.getObject("generator").getValue(settings.generator);
        item.getObject("buildType").getValue(settings.buildType);
        item.getObject("arguments").getValue(settings.arguments);
        item.getObject("parentProject").getValue(settings.parentProject);
    }
}

/* ************************************************************************ */
