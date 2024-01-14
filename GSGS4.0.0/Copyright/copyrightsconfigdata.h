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

#ifndef __copyrightsconfigdata__
#define __copyrightsconfigdata__

#include "gsgsSerializer.h"

class CopyrightsConfigData : public gsgs::ObjectData
{
public:
	CopyrightsConfigData();
	virtual ~CopyrightsConfigData();

	virtual void DeSerialize(gsgs::Serializer &arch);
	virtual void Serialize(gsgs::Serializer &arch);

	//Setters
	void SetFileMasking(const wxString& fileMasking) {
		this->m_fileMasking = fileMasking;
	}
	void SetTemplateFilename(const wxString& templateFilename) {
		this->m_templateFilename = templateFilename;
	}
	void SetBackupFiles(const bool& backupFiles) {
		this->m_backupFiles = backupFiles;
	}

	//Getters
	const wxString& GetFileMasking() const {
		return m_fileMasking;
	}
	const wxString& GetTemplateFilename() const {
		return m_templateFilename;
	}
	const bool& GetBackupFiles() const {
		return m_backupFiles;
	}
	void SetIgnoreString(const wxString& ignoreString) {
		this->m_ignoreString = ignoreString;
	}
	const wxString& GetIgnoreString() const {
		return m_ignoreString;
	}
protected:
    wxString m_templateFilename;
    wxString m_fileMasking;
    bool m_backupFiles;
    wxString m_ignoreString;
};
#endif // __copyrightsconfigdata__
