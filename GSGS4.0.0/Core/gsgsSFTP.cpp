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

#if USE_SFTP

#include "gsgsSFTP.h"
#include <sys/stat.h>
#include <libssh/sftp.h>
#include "gsgsPathManager.h"

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_SFTP_SAVE_FILE, SFTPEvent);
    wxDEFINE_EVENT(wxEVT_SFTP_RENAME_FILE, SFTPEvent);
    wxDEFINE_EVENT(wxEVT_SFTP_DELETE_FILE, SFTPEvent);
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // SFTPEvent
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    SFTPEvent::~SFTPEvent() {}
    //-----------------------------------------------------------------------------------
    SFTPEvent::SFTPEvent(const SFTPEvent& event) { *this = event; }
    //-----------------------------------------------------------------------------------
    SFTPEvent::SFTPEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    {
    }
    //-----------------------------------------------------------------------------------
    SFTPEvent& SFTPEvent::operator=(const SFTPEvent& src)
    {
        Event::operator=(src);
        
        m_account = src.m_account;
        m_localFile = src.m_localFile;
        m_remoteFile = src.m_remoteFile;
        m_newRemoteFile = src.m_newRemoteFile;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // SFTPAttribute
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    SFTPAttribute::SFTPAttribute(SFTPAttribute_t attr)
        : m_attributes(NULL)
        , m_permissions(0)
    {
        Assign(attr);
    }
    //-----------------------------------------------------------------------------------
    SFTPAttribute::~SFTPAttribute()
    {
        DoClear();
    }
    //-----------------------------------------------------------------------------------
    void SFTPAttribute::Assign(SFTPAttribute_t attr)
    {
        DoClear();
        m_attributes = attr;
        DoConstruct();
    }
    //-----------------------------------------------------------------------------------
    void SFTPAttribute::DoClear()
    {
        if (m_attributes) {
            sftp_attributes_free(m_attributes);
        }
        m_attributes = NULL;
        m_name.Clear();
        m_flags = 0;
        m_size = 0;
        m_permissions = 0;
    }
    //-----------------------------------------------------------------------------------
    void SFTPAttribute::DoConstruct()
    {
        if (!m_attributes)
            return;

        m_name = m_attributes->name;
        m_size = m_attributes->size;
        m_permissions = m_attributes->permissions;
        m_flags = 0;

        switch (m_attributes->type) {
        case SSH_FILEXFER_TYPE_DIRECTORY:
            m_flags |= TYPE_FOLDER;
            break;
        default:
        case SSH_FILEXFER_TYPE_REGULAR:
            m_flags |= TYPE_REGULAR_FILE;
            break;
        case SSH_FILEXFER_TYPE_SPECIAL:
            m_flags |= TYPE_SEPCIAL;
            break;

        case SSH_FILEXFER_TYPE_UNKNOWN:
            m_flags |= TYPE_UNKNOWN;
            break;

        case SSH_FILEXFER_TYPE_SYMLINK:
            m_flags |= TYPE_SYMBLINK;
            break;
        }
    }
    //-----------------------------------------------------------------------------------
    wxString SFTPAttribute::GetTypeAsString() const
    {
        if (IsSpecial()) {
            return "Special";
        }
        else if (IsFolder()) {
            return "Folder";
        }
        else if (IsSymlink()) {
            return "Symlink";
        }
        else if (IsFile()) {
            return "File";
        }
        else {
            return "Unknown";
        }
    }
    //-----------------------------------------------------------------------------------
    bool SFTPAttribute::Compare(SFTPAttribute::Ptr_t one, SFTPAttribute::Ptr_t two)
    {
        if (one->IsFolder() && !two->IsFolder()) {
            return true;
        }
        else if (!one->IsFolder() && two->IsFolder()) {
            return false;
        }
        else {
            return one->GetName() < two->GetName();
        }
    }
    //-----------------------------------------------------------------------------------
    class SFTPDirCloser
    {
        sftp_dir m_dir;

    public:
        SFTPDirCloser(sftp_dir d)
            : m_dir(d)
        {
        }
        ~SFTPDirCloser() { sftp_closedir(m_dir); }
    };
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // SFTP
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    SFTP::SFTP(SSH::Ptr_t ssh)
        : m_ssh(ssh)
        , m_sftp(NULL)
        , m_connected(false)
    {
    }
    //-----------------------------------------------------------------------------------
    SFTP::~SFTP() 
    { 
        Close(); 
    }
    //-----------------------------------------------------------------------------------
    void SFTP::Initialize() 
    {
        if(m_sftp) return;

        m_sftp = sftp_new(m_ssh->GetSession());
        if(m_sftp == NULL) {
            throw Exception(wxString() << "Error allocating SFTP session: " << ssh_get_error(m_ssh->GetSession()));
        }

        int rc = sftp_init(m_sftp);
        if(rc != SSH_OK) {
            throw Exception(wxString() << "Error initializing SFTP session: " << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
        m_connected = true;
    }
    //-----------------------------------------------------------------------------------
    void SFTP::Close()
    {
        if(m_sftp) {
            sftp_free(m_sftp);
        }

        m_connected = false;
        m_sftp = NULL;
    }
    //-----------------------------------------------------------------------------------
    void SFTP::Write(const wxFileName& localFile, const wxString& remotePath, SFTPAttribute::Ptr_t attributes) 
    {
        if(!m_connected) {
            throw Exception("scp is not initialized!");
        }

        if(!localFile.Exists()) {
            throw Exception(wxString() << "scp::Write file '" << localFile.GetFullPath() << "' does not exist!");
        }

        wxFFile fp(localFile.GetFullPath(), "rb");
        if(!fp.IsOpened()) {
            throw Exception(wxString() << "scp::Write could not open file '" << localFile.GetFullPath() << "'. "
                                         << ::strerror(errno));
        }

        char buffer[4096];
        wxMemoryBuffer memBuffer;
        size_t nbytes(0);
        while(!fp.Eof()) {
            nbytes = fp.Read(buffer, sizeof(buffer));
            if(nbytes == 0) break;
            memBuffer.AppendData(buffer, nbytes);
        }
        fp.Close();
        Write(memBuffer, remotePath);
        if(attributes && attributes->GetPermissions()) {
            Chmod(remotePath, attributes->GetPermissions());
        }
    }
    //-----------------------------------------------------------------------------------
    void SFTP::Write(const wxMemoryBuffer& fileContent, const wxString& remotePath, SFTPAttribute::Ptr_t attributes) 
    {
        if(!m_sftp) {
            throw Exception("SFTP is not initialized");
        }

        int access_type = O_WRONLY | O_CREAT | O_TRUNC;
        sftp_file file;
        wxString tmpRemoteFile = remotePath;
        tmpRemoteFile << ".gsgssftp";

        file = sftp_open(m_sftp, tmpRemoteFile.mb_str(wxConvUTF8).data(), access_type, 0644);
        if(file == NULL) {
            throw Exception(wxString() << _("Can't open file: ") << tmpRemoteFile << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }

        char* p = (char*)fileContent.GetData();
        const int maxChunkSize = 65536;
        wxInt64 bytesLeft = fileContent.GetDataLen();

        while(bytesLeft > 0) {
            wxInt64 chunkSize = bytesLeft > maxChunkSize ? maxChunkSize : bytesLeft;
            wxInt64 bytesWritten = sftp_write(file, p, chunkSize);
            if(bytesWritten < 0) {
                sftp_close(file);
                throw Exception(wxString() << _("Can't write data to file: ") << tmpRemoteFile << ". "
                                             << ssh_get_error(m_ssh->GetSession()),
                                  sftp_get_error(m_sftp));
            }
            bytesLeft -= bytesWritten;
            p += bytesWritten;
        }
        sftp_close(file);

        // Unlink the original file if it exists
        bool needUnlink = false;
        {
            // Check if the file exists
            sftp_attributes attr = sftp_stat(m_sftp, remotePath.mb_str(wxConvISO8859_1).data());
            if(attr) {
                needUnlink = true;
                sftp_attributes_free(attr);
            }
        }

        if(needUnlink && sftp_unlink(m_sftp, remotePath.mb_str(wxConvUTF8).data()) < 0) {
            throw Exception(wxString() << _("Failed to unlink file: ") << remotePath << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }

        // Rename the file
        if(sftp_rename(m_sftp, tmpRemoteFile.mb_str(wxConvUTF8).data(), remotePath.mb_str(wxConvUTF8).data()) < 0) {
            throw Exception(wxString() << _("Failed to rename file: ") << tmpRemoteFile << " -> " << remotePath << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }

        if(attributes && attributes->GetPermissions()) {
            Chmod(remotePath, attributes->GetPermissions());
        }
    }
    //-----------------------------------------------------------------------------------
    SFTPAttribute::List_t SFTP::List(const wxString& folder, size_t flags, const wxString& filter) 
    {
        sftp_dir dir;
        sftp_attributes attributes;

        if(!m_sftp) {
            throw Exception("SFTP is not initialized");
        }

        dir = sftp_opendir(m_sftp, folder.mb_str(wxConvUTF8).data());
        if(!dir) {
            throw Exception(wxString() << _("Failed to list directory: ") << folder << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }

        // Keep the current folder name
        m_currentFolder = dir->name;

        // Ensure the directory is closed
        SFTPDirCloser dc(dir);
        SFTPAttribute::List_t files;

        attributes = sftp_readdir(m_sftp, dir);
        while(attributes) {

            SFTPAttribute::Ptr_t attr(new SFTPAttribute(attributes));
            attributes = sftp_readdir(m_sftp, dir);

            // Don't show files ?
            if(!(flags & SFTP_BROWSE_FILES) && !attr->IsFolder()) {
                continue;

            } else if((flags & SFTP_BROWSE_FILES) && !attr->IsFolder() // show files
                      &&
                      filter.IsEmpty()) { // no filter is given
                files.push_back(attr);

            } else if((flags & SFTP_BROWSE_FILES) && !attr->IsFolder() // show files
                      &&
                      !::wxMatchWild(filter, attr->GetName())) { // but the file does not match the filter
                continue;

            } else {
                files.push_back(attr);
            }
        }
        files.sort(SFTPAttribute::Compare);
        return files;
    }
    //-----------------------------------------------------------------------------------
    SFTPAttribute::List_t SFTP::CdUp(size_t flags, const wxString& filter) 
    {
        wxString curfolder = m_currentFolder;
        curfolder << "/../"; // Force a cd up

        wxFileName fn(curfolder, "", wxPATH_UNIX);
        fn.Normalize();
        return List(fn.GetPath(false, wxPATH_UNIX), flags, filter);
    }
    //-----------------------------------------------------------------------------------
    SFTPAttribute::Ptr_t SFTP::Read(const wxString& remotePath, wxMemoryBuffer& buffer) 
    {
        if(!m_sftp) {
            throw Exception("SFTP is not initialized");
        }

        sftp_file file = sftp_open(m_sftp, remotePath.mb_str(wxConvUTF8).data(), O_RDONLY, 0);
        if(file == NULL) {
            throw Exception(wxString() << _("Failed to open remote file: ") << remotePath << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }

        SFTPAttribute::Ptr_t fileAttr = Stat(remotePath);
        if(!fileAttr) {
            throw Exception(wxString() << _("Could not stat file:") << remotePath << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
        wxInt64 fileSize = fileAttr->GetSize();
        if(fileSize == 0) return fileAttr;

        // Allocate buffer for the file content
        char pBuffer[65536]; // buffer

        // Read the entire file content
        wxInt64 bytesLeft = fileSize;
        wxInt64 bytesRead = 0;
        while(bytesLeft > 0) {
            wxInt64 nbytes = sftp_read(file, pBuffer, sizeof(pBuffer));
            bytesRead += nbytes;
            bytesLeft -= nbytes;
            buffer.AppendData(pBuffer, nbytes);
        }

        if(bytesRead != fileSize) {
            sftp_close(file);
            buffer.Clear();
            throw Exception(wxString() << _("Could not read file:") << remotePath << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
        sftp_close(file);
        return fileAttr;
    }
    //-----------------------------------------------------------------------------------
    void SFTP::CreateDir(const wxString& dirname) 
    {
        if(!m_sftp) {
            throw Exception("SFTP is not initialized");
        }
    #define S_IRWXU 00700
        int rc;
        rc = sftp_mkdir(m_sftp, dirname.mb_str(wxConvISO8859_1).data(), S_IRWXU);

        if(rc != SSH_OK) {
            throw Exception(wxString() << _("Failed to create directory: ") << dirname << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
    }
    //-----------------------------------------------------------------------------------
    void SFTP::Rename(const wxString& oldpath, const wxString& newpath) 
    {
        if(!m_sftp) {
            throw Exception("SFTP is not initialized");
        }

        int rc;
        rc = sftp_rename(m_sftp, oldpath.mb_str(wxConvISO8859_1).data(), newpath.mb_str(wxConvISO8859_1).data());

        if(rc != SSH_OK) {
            throw Exception(wxString() << _("Failed to rename path. ") << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
    }
    //-----------------------------------------------------------------------------------
    void SFTP::RemoveDir(const wxString& dirname) 
    {
        if(!m_sftp) {
            throw Exception("SFTP is not initialized");
        }

        int rc;
        rc = sftp_rmdir(m_sftp, dirname.mb_str(wxConvISO8859_1).data());

        if(rc != SSH_OK) {
            throw Exception(wxString() << _("Failed to remove directory: ") << dirname << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
    }
    //-----------------------------------------------------------------------------------
    void SFTP::UnlinkFile(const wxString& path) 
    {
        if(!m_sftp) {
            throw Exception("SFTP is not initialized");
        }

        int rc;
        rc = sftp_unlink(m_sftp, path.mb_str(wxConvISO8859_1).data());

        if(rc != SSH_OK) {
            throw Exception(wxString() << _("Failed to unlink path: ") << path << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
    }
    //-----------------------------------------------------------------------------------
    SFTPAttribute::Ptr_t SFTP::Stat(const wxString& path) 
    {
        if(!m_sftp) {
            throw Exception("SFTP is not initialized");
        }

        sftp_attributes attr = sftp_stat(m_sftp, path.mb_str(wxConvISO8859_1).data());
        if(!attr) {
            throw Exception(wxString() << _("Could not stat: ") << path << ". " << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
        SFTPAttribute::Ptr_t pattr(new SFTPAttribute(attr));
        return pattr;
    }
    //-----------------------------------------------------------------------------------
    void SFTP::CreateRemoteFile(const wxString& remoteFullPath, const wxString& content,
        SFTPAttribute::Ptr_t attr) 
    {
        // Create the path to the file
        Mkpath(wxFileName(remoteFullPath).GetPath());
        Write(content, remoteFullPath, attr);
    }
    //-----------------------------------------------------------------------------------
    void SFTP::Mkpath(const wxString& remoteDirFullpath) 
    {
        if(!m_sftp) {
            throw Exception("SFTP is not initialized");
        }

        wxString tmpPath = remoteDirFullpath;
        tmpPath.Replace("\\", "/");
        if(!tmpPath.StartsWith("/")) {
            throw Exception("Mkpath: path must be absolute");
        }

        wxFileName fn(remoteDirFullpath, "");
        const wxArrayString& dirs = fn.GetDirs();
        wxString curdir;

        curdir << "/";
        for(size_t i = 0; i < dirs.GetCount(); ++i) {
            curdir << dirs.Item(i);
            sftp_attributes attr = sftp_stat(m_sftp, curdir.mb_str(wxConvISO8859_1).data());
            if(!attr) {
                // directory does not exists
                CreateDir(curdir);

            } else {
                // directory already exists
                sftp_attributes_free(attr);
            }
            curdir << "/";
        }
    }
    //-----------------------------------------------------------------------------------
    void SFTP::CreateRemoteFile(const wxString& remoteFullPath, const wxFileName& localFile,
        SFTPAttribute::Ptr_t attr) 
    {
        Mkpath(wxFileName(remoteFullPath).GetPath());
        Write(localFile, remoteFullPath, attr);
    }
    //-----------------------------------------------------------------------------------
    void SFTP::Chmod(const wxString& remotePath, size_t permissions) 
    {
        if(!m_sftp) 
        {
            throw Exception("SFTP is not initialized");
        }
        
        if(permissions == 0) 
            return;
        
        int rc = sftp_chmod(m_sftp, remotePath.mb_str(wxConvUTF8).data(), permissions);
        if(rc != SSH_OK) {
            throw Exception(wxString() << _("Failed to chmod file: ") << remotePath << ". "
                                         << ssh_get_error(m_ssh->GetSession()),
                              sftp_get_error(m_sftp));
        }
    }
    //-----------------------------------------------------------------------------------
    wxString SFTP::GetDefaultDownloadFolder()
    {
        wxFileName path(gsgs_Path().GetUserDataDir(), "");
        path.AppendDir("sftp");
        path.AppendDir("tmp");
        return path.GetPath();
    }
    //-----------------------------------------------------------------------------------
}
#endif // USE_SFTP
