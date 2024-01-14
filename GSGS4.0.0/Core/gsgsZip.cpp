#include "gsgsZip.h"

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // ZipReader
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    ZipReader::ZipReader(const wxFileName& zipfile)
        : m_file(new wxFileInputStream(zipfile.GetFullPath()))
        , m_zip(new wxZipInputStream(*m_file))
    {
    }
    //-----------------------------------------------------------------------------------
    ZipReader::~ZipReader() { Close(); }
    //-----------------------------------------------------------------------------------
    void ZipReader::Close()
    {
        wxDELETE(m_file);
        wxDELETE(m_zip);
    }
    //-----------------------------------------------------------------------------------
    void ZipReader::Extract(const wxString& filename, const wxString& directory)
    {
        wxZipEntry* entry(NULL);
        entry = m_zip->GetNextEntry();
        while(entry) {
            if(::wxMatchWild(filename, entry->GetName())) {
                // Incase the entry name has a directory prefix, remove it
                wxString fullpath;
                fullpath << directory << "/" << entry->GetName();
                
                // Change to posix style
                fullpath.Replace("\\", "/");
                // Remove any duplicate double slashes
                while(fullpath.Replace("//", "/")) {}
                
                if(entry->IsDir()) {
                    // a folder
                    wxFileName::Mkdir(fullpath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
                } else {
                    wxFileName outfile(fullpath);
                    // ensure that the path to the file exists
                    outfile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
                    wxFFileOutputStream out(outfile.GetFullPath());
                    if(out.IsOk()) {
                        m_zip->Read(out);
                        out.Close();
                    }
                }
            }
            wxDELETE(entry);
            entry = m_zip->GetNextEntry();
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // ZipWriter
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    ZipWriter::ZipWriter(const wxFileName& zipfile)
        : m_filename(zipfile)
    {
        m_file = new wxFileOutputStream(zipfile.GetFullPath());
        m_zip = new wxZipOutputStream(*m_file);
    }
    //-----------------------------------------------------------------------------------
    ZipWriter::~ZipWriter() { Close(); }
    //-----------------------------------------------------------------------------------
    void ZipWriter::Add(const wxFileName& file)
    {
        if(!file.FileExists()) {
            return;
        }
        wxZipEntry* entry = new wxZipEntry(file.GetFullName());
        m_zip->PutNextEntry(entry);

        wxFileInputStream fis(file.GetFullPath());
        wxBufferedOutputStream bfs(*m_zip);
        fis.Read(bfs);
        bfs.Sync();
    }
    //-----------------------------------------------------------------------------------
    void ZipWriter::AddDirectory(const wxString& path, const wxString& pattern)
    {
        wxArrayString files;
        wxDir::GetAllFiles(path, &files, pattern);

        for(size_t i = 0; i < files.GetCount(); ++i) {
            Add(wxFileName(files.Item(i)));
        }
    }
    //-----------------------------------------------------------------------------------
    void ZipWriter::Close()
    {
        if(m_zip) {
            m_zip->Close();
        }
        wxDELETE(m_zip);
        wxDELETE(m_file);
    }
    //-----------------------------------------------------------------------------------
}