/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#include "memchecksettings.h"

ValgrindSettings::ValgrindSettings():ObjectData(CONFIG_ITEM_NAME_VALGRIND),
    m_binary(VALGRIND_BINARY),
    m_outputInPrivateFolder(VALGRIND_OUTPUT_IN_PRIVATE_FOLDER),
    m_outputFile(VALGRIND_OUTPUT_FILE),
    m_mandatoryOptions(VALGRIND_MANDATORY_OPTIONS),
    m_outputFileOption(VALGRIND_OUTPUT_FILE_OPTION),
    m_suppressionFileOption(VALGRIND_SUPPRESSION_FILE_OPTION),
    m_options(VALGRIND_OPTIONS),
    m_suppFileInPrivateFolder(VALGRIND_SUPP_FILE_IN_PRIVATE_FOLDER),
    m_suppFiles()
{
}


void ValgrindSettings::FromJSON(const JSONObject & json)
{
    if (json.isObjectExist("m_binary")) m_binary = json.getObject("m_binary").toString();
    if (json.isObjectExist("m_outputInPrivateFolder")) m_outputInPrivateFolder = json.getObject("m_outputInPrivateFolder").toBool();
    if (json.isObjectExist("m_outputFile")) m_outputFile = json.getObject("m_outputFile").toString();
    if (json.isObjectExist("m_mandatoryOptions")) m_mandatoryOptions = json.getObject("m_mandatoryOptions").toString();
    if (json.isObjectExist("m_outputFileOption")) m_outputFileOption = json.getObject("m_outputFileOption").toString();
    if (json.isObjectExist("m_suppressionFileOption")) m_suppressionFileOption = json.getObject("m_suppressionFileOption").toString();
    if (json.isObjectExist("m_options")) m_options = json.getObject("m_options").toString();
    if (json.isObjectExist("m_suppFileInPrivateFolder")) m_suppFileInPrivateFolder = json.getObject("m_suppFileInPrivateFolder").toBool();
    if (json.isObjectExist("m_suppFiles")) m_suppFiles = json.getObject("m_suppFiles").toArrayString();
}

JSONObject ValgrindSettings::ToJSON() const
{
    JSONObject element = JSONObject::createObject(GetName());
    element.addProperty("m_binary", m_binary);
    element.addProperty("m_outputInPrivateFolder", m_outputInPrivateFolder);
    element.addProperty("m_outputFile", m_outputFile);
    element.addProperty("m_mandatoryOptions", m_mandatoryOptions);
    element.addProperty("m_outputFileOption", m_outputFileOption);
    element.addProperty("m_suppressionFileOption", m_suppressionFileOption);
    element.addProperty("m_options", m_options);
    element.addProperty("m_suppFileInPrivateFolder", m_suppFileInPrivateFolder);
    element.addProperty("m_suppFiles", m_suppFiles);
    return element;
}



MemCheckSettings::MemCheckSettings():ObjectData(CONFIG_ITEM_NAME_MEMCHECK),
    m_engine(CONFIG_ITEM_NAME_VALGRIND),
    m_availableEngines(),
    m_result_page_size(RESULT_PAGE_SIZE),
    m_result_page_size_max(RESULT_PAGE_SIZE_MAX),
    m_omitNonWorkspace(OMIT_NONWORKSPACE),
    m_omitDuplications(OMIT_DUPLICATIONS),
    m_omitSuppressed(OMIT_SUPPRESSED)
{
    m_availableEngines.Add(CONFIG_ITEM_NAME_VALGRIND);
}


void MemCheckSettings::FromJSON(const JSONObject & json)
{
    if (json.isObjectExist("m_engine")) m_engine = json.getObject("m_engine").toString();
    if (json.isObjectExist("m_result_page_size")) m_result_page_size = json.getObject("m_result_page_size").toSize_t();
    if (json.isObjectExist("m_result_page_size_max")) m_result_page_size_max = json.getObject("m_result_page_size_max").toSize_t();
    if (json.isObjectExist("m_omitNonWorkspace")) m_omitNonWorkspace = json.getObject("m_omitNonWorkspace").toBool();
    if (json.isObjectExist("m_omitDuplications")) m_omitDuplications = json.getObject("m_omitDuplications").toBool();
    if (json.isObjectExist("m_omitSuppressed")) m_omitSuppressed = json.getObject("m_omitSuppressed").toBool();

    m_valgrindSettings.FromJSON(json.getObject(CONFIG_ITEM_NAME_VALGRIND));
}

JSONObject MemCheckSettings::ToJSON() const
{
    JSONObject element = JSONObject::createObject(GetName());
    element.addProperty("m_engine", m_engine);
    element.addProperty("m_result_page_size", m_result_page_size);
    element.addProperty("m_result_page_size_max", m_result_page_size_max);
    element.addProperty("m_omitNonWorkspace", m_omitNonWorkspace);
    element.addProperty("m_omitDuplications", m_omitDuplications);
    element.addProperty("m_omitSuppressed", m_omitSuppressed);

    element.append(m_valgrindSettings.ToJSON());
    return element;
}
