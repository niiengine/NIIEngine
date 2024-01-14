#include "EditorConfigCache.h"
#include "gsgsFileUtil.h"

EditorConfigCache::EditorConfigCache() {}

EditorConfigCache::~EditorConfigCache() {}

bool EditorConfigCache::Get(const wxFileName& filename, gsgs::EditorConfigSection& section)
{
    wxString key = filename.GetFullPath();
    if(m_map.count(key) == 0) return false;

    CachedItem& entry = m_map[key];
    time_t lastModTimeOnDisk = FileUtil::GetFileModificationTime(entry.section.GetFilename());
    if(lastModTimeOnDisk != entry.lastModified) {
        // remove this entry and return false
        m_map.erase(key);
        return false;
    }

    section = entry.section;
    return true;
}

void EditorConfigCache::Add(const wxFileName& filename, const gsgs::EditorConfigSection& section)
{
    wxString key = filename.GetFullPath();
    if(m_map.count(key)) {
        m_map.erase(key);
    }

    CachedItem entry;
    entry.lastModified = FileUtil::GetFileModificationTime(section.GetFilename());
    entry.section = section;
    m_map.insert(std::make_pair(key, entry));
}

void EditorConfigCache::Clear() { m_map.clear(); }
