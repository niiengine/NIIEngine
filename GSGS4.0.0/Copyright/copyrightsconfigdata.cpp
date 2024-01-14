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

#include "copyrightsconfigdata.h"

CopyrightsConfigData::CopyrightsConfigData()
: m_templateFilename(wxEmptyString)
, m_fileMasking(wxT("*.h;*.hpp;*.h++"))
, m_backupFiles(true)
{
}

CopyrightsConfigData::~CopyrightsConfigData()
{
}

void CopyrightsConfigData::DeSerialize(gsgs::Serializer& arch)
{
	arch.Read(wxT("m_templateFilename"), m_templateFilename);
	arch.Read(wxT("m_fileMasking"), m_fileMasking);
	arch.Read(wxT("m_backupFiles"), m_backupFiles);
	arch.Read(wxT("m_ignoreString"), m_ignoreString);
}

void CopyrightsConfigData::Serialize(gsgs::Serializer& arch)
{
	arch.Write(wxT("m_templateFilename"), m_templateFilename);
	arch.Write(wxT("m_fileMasking"), m_fileMasking);
	arch.Write(wxT("m_backupFiles"), m_backupFiles);
	arch.Write(wxT("m_ignoreString"), m_ignoreString);
}
