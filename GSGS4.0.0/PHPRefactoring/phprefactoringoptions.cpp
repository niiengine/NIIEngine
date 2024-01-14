#include "phprefactoringoptions.h"
#include "gsgsPathManager.h"
#include <wx/filename.h>

PHPRefactoringOptions::PHPRefactoringOptions()
    : ObjectData("phprefactoring")
    , m_phprefactoringPhar("")
    , m_skipPreview(false)
{
    // Set default PHAR file
    wxFileName pharFile(gsgs_Path().GetDataDir(), "phprefactor.phar");
    if(pharFile.FileExists()) {
        m_phprefactoringPhar = pharFile.GetFullPath();
    }
}

PHPRefactoringOptions::~PHPRefactoringOptions() {}

void PHPRefactoringOptions::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("phprefactoringPhar").getValue(m_phprefactoringPhar, m_phprefactoringPhar);
    json.getObject("skipPreview").getValue(m_skipPreview, m_skipPreview);
}

gsgs::JSONObject PHPRefactoringOptions::ToJSON() const
{
    gsgs::JSONObject element = gsgs::JSONObject::createObject(GetName());
    element.addProperty("phprefactoringPhar", m_phprefactoringPhar);
    element.addProperty("skipPreview", m_skipPreview);
    return element;
}

PHPRefactoringOptions& PHPRefactoringOptions::Load()
{
    gsgs::Config config("phprefactoring.conf");
    config.ReadItem(this);
    return *this;
}

PHPRefactoringOptions& PHPRefactoringOptions::Save()
{
    gsgs::Config config("phprefactoring.conf");
    config.WriteItem(this);
    return *this;
}
