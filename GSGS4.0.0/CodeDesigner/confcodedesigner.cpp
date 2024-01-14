#include "confcodedesigner.h"

ConfCodeDesigner::ConfCodeDesigner()
{
	m_Port = wxT("50000");
}

ConfCodeDesigner::~ConfCodeDesigner()
{
}

void ConfCodeDesigner::DeSerialize(Serializer& arch)
{
	arch.Read(wxT("m_port"), m_Port);
	arch.Read(wxT("m_path"), m_Path);
}

void ConfCodeDesigner::Serialize(Serializer& arch)
{
	arch.Write(wxT("m_port"), m_Port);
	arch.Write(wxT("m_path"), m_Path);
}
