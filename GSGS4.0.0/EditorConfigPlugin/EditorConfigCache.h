#ifndef EDITORCONFIGCACHE_H
#define EDITORCONFIGCACHE_H

#include "gsgsOptionsConfig.h"

class EditorConfigCache
{
public:
    EditorConfigCache();
    virtual ~EditorConfigCache();

    /**
     * @brief find cached item for given filename
     */
    bool Get(const wxFileName& filename, gsgs::EditorConfigSection& section);
    
    /**
     * @brief add entry to the cache
     */
    void Add(const wxFileName& filename, const gsgs::EditorConfigSection& section);
    
    /**
     * @brief clear the cache
     */
    void Clear();
private:
    struct CachedItem {
        gsgs::EditorConfigSection section;
        time_t lastModified;
    };
    
    std::map<wxString, CachedItem> m_map;
};

#endif // EDITORCONFIGCACHE_H
