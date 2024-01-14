#include "fc_fileopener.h"
#include <cstdio>
#include <cctype>
#include <algorithm>
#include <wx/filename.h>
#include <wx/filefn.h>

#ifdef __WXMSW__
#define FC_PATH_SEP "\\"
#else
#define FC_PATH_SEP "/"
#endif

FileOpen* FileOpen::gInstance = 0;

FileOpen::FileOpen()
    : _depth(0)
    , _maxDepth(20)
{
}

FileOpen::~FileOpen()
{
}

FileOpen* FileOpen::Get()
{
    if (gInstance == 0) {
        gInstance = new FileOpen();
    }
    return gInstance;
}

void FileOpen::Release()
{
    if (gInstance) {
        delete gInstance;
    }
    gInstance = 0;
}

void FileOpen::AddSearchPath(const wxString& path)
{
    wxFileName fn( path, "" );
    if ( !wxFileName::DirExists( fn.GetPath() ) )
        return;
    _searchPath.push_back( fn.GetPath() );
}

FILE* FileOpen::OpenFile(const wxString& include_path, wxString &filepath)
{
    filepath.Clear();
    if ( include_path.empty() ) {
        return NULL;
    }

    wxString mod_path ( include_path );

    static wxString trimString("\"<> \t");

    mod_path.erase(0, mod_path.find_first_not_of(trimString));
    mod_path.erase(mod_path.find_last_not_of    (trimString)+1);

    if ( _scannedfiles.find(mod_path) != _scannedfiles.end() ) {
        // we already scanned this file
        filepath.Clear();
        return NULL;
    }

    FILE *fp (NULL);
    
    // first try to cwd
    fp = try_open(_cwd, mod_path, filepath);
    if ( fp ) {
        return fp;
    }
    
    // Now try the search directories
    for (size_t i=0; i<_searchPath.size(); ++i) {
        fp = try_open(_searchPath.at(i), mod_path, filepath);
        if ( fp ) return fp;
    }
    
    _scannedfiles.insert( mod_path );
    filepath.Clear();
    return NULL;
}

FILE* FileOpen::try_open(const wxString &path, const wxString &name, wxString &filepath)
{
    wxString fullpath ( path + FC_PATH_SEP + name );
    wxFileName fn(fullpath);
    
    fullpath = fn.GetFullPath();
    FILE *fp = wxFopen(fullpath, "rb");
    if ( fp ) {

        _scannedfiles.insert( name );
        wxString pathPart = fn.GetPath();

        for(size_t i=0; i<_excludePaths.size(); ++i) {
            if ( pathPart.StartsWith(_excludePaths.at(i) ) ) {
                ::fclose( fp );
                return NULL;
            }
        }

        _matchedfiles.insert( fullpath );
        filepath = fullpath;
        return fp;
    }
    return NULL;
}

void FileOpen::AddExcludePath(const wxString& path)
{
    wxFileName fn( path, "" );
    if ( !wxFileName::DirExists( fn.GetPath() ) )
        return;
    _excludePaths.push_back( fn.GetPath() );
}

void FileOpen::AddNamespace(const char* ns)
{
    _namespaces.insert(ns);
}

void FileOpen::AddIncludeStatement(const wxString& statement)
{
    if( std::find(_includeStatements.begin(), _includeStatements.end(), statement) == _includeStatements.end()) {
        _includeStatements.push_back(statement);
    }
}

BufferState FileOpen::PopBufferState()
{
    if ( _states.empty() ) {
        return NULL;
    }
    
    fcState curstate = _states.back();
    BufferState state = curstate.buffer;
    
    // update the current directory
    _cwd = wxFileName(curstate.filename).GetPath();
    
    _states.pop_back();
    decDepth();
    return state;
}

void FileOpen::PushBufferState(BufferState buffer, const wxString &filename)
{
    fcState curstate;
    curstate.buffer = buffer;
    curstate.filename = filename;
    _states.push_back( curstate );
    
    // update the current directory
    _cwd = wxFileName(curstate.filename).GetPath();
    incDepth();
}
