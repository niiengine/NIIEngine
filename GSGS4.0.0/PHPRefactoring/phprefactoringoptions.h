#ifndef PHPREFACTORINGOPTIONS_H
#define PHPREFACTORINGOPTIONS_H

#include "gsgsConfig.h" // Base class: ObjectData

class PHPRefactoringOptions : public gsgs::ObjectData
{
public:
    PHPRefactoringOptions();
    virtual ~PHPRefactoringOptions();

    virtual void FromJSON(const gsgs::JSONObject& json);
    virtual gsgs::JSONObject ToJSON() const;

    PHPRefactoringOptions& Load();
    PHPRefactoringOptions& Save();

    PHPRefactoringOptions& SetPhprefactoringPhar(const wxFileName& PhprefactoringPhar)
    {
        this->m_phprefactoringPhar = PhprefactoringPhar.GetFullPath();
        return *this;
    }

    const wxString& GetPhprefactoringPhar() const { return m_phprefactoringPhar; }

    PHPRefactoringOptions& SetSkipPreview(const bool& SkipPreview)
    {
        this->m_skipPreview = SkipPreview;
        return *this;
    }

    const bool& GetSkipPreview() const { return m_skipPreview; }
private:
    wxString m_phprefactoringPhar;
    bool m_skipPreview;
};

#endif // PHPREFACTORINGOPTIONS_H
