#ifndef CONFCODEDESIGNER_H
#define CONFCODEDESIGNER_H

#include "gsgsSerializer.h" // Base class: ObjectData

class ConfCodeDesigner : public ObjectData {

public:
	ConfCodeDesigner();
	virtual ~ConfCodeDesigner();

public:
	virtual void DeSerialize(Serializer& arch);
	virtual void Serialize(Serializer& arch);
	
	void SetPort(const wxString& Port) {
		this->m_Port = Port;
	}
	void SetPath(const wxString& Path) {
		this->m_Path = Path;
	}
	const wxString& GetPort() const {
		return m_Port;
	}
	const wxString& GetPath() const {
		return m_Path;
	}
	
protected:
	wxString m_Path;
	wxString m_Port;
};

#endif // CONFCODEDESIGNER_H
