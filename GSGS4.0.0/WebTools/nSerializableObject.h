#ifndef NODEJSONSERIALISABLE_H
#define NODEJSONSERIALISABLE_H

#include "gsgsJSON.h"
#include <vector>
#include <wx/sharedptr.h>
#include <wx/string.h>

class nSerializableObject
{
public:
    typedef wxSharedPtr<nSerializableObject> Ptr_t;
    typedef std::vector<nSerializableObject::Ptr_t> Vec_t;

public:
    nSerializableObject();
    virtual ~nSerializableObject();

    virtual gsgs::JSONObject ToJSON(const wxString& name) const = 0;
    virtual void FromJSON(const gsgs::JSONObject& json) = 0;
    template <typename T> T* To() { return reinterpret_cast<T*>(this); }
};

#endif // NODEJSONSERIALISABLE_H
