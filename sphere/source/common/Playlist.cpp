#include "Playlist.hpp"
////////////////////////////////////////////////////////////////////////////////
CPlaylist::CPlaylist()
{
}

////////////////////////////////////////////////////////////////////////////////
CPlaylist::~CPlaylist()
{
    m_Filenames.clear();
}
////////////////////////////////////////////////////////////////////////////////
const char*
CPlaylist::GetFile(int index) const
{
    return m_Filenames[index].c_str();
}
////////////////////////////////////////////////////////////////////////////////
int
CPlaylist::GetNumFiles() const
{
    return m_Filenames.size();
}
////////////////////////////////////////////////////////////////////////////////
bool
CPlaylist::AppendFile(const char* filename)
{
    int size = int(m_Filenames.size());
    m_Filenames.push_back(filename);
    return size + 1 == int(m_Filenames.size());
}
////////////////////////////////////////////////////////////////////////////////
void
CPlaylist::Clear()
{
    m_Filenames.clear();
}
////////////////////////////////////////////////////////////////////////////////
// returns false if eof
inline bool read_line(IFile* file, std::string& s)
{
    s = "";
    char c;
    if (file->Read(&c, 1) == 0)
    {

        return false;
    }
    bool eof = false;
    while (!eof && c != '\n')
    {

        if (c != '\r')
        {

            s += c;
        }
        eof = (file->Read(&c, 1) == 0);
    }
    return !eof;
}
////////////////////////////////////////////////////////////////////////////////
bool
CPlaylist::LoadFromFile(const char* filename, IFileSystem& fs)
{
    // open the file
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::read));
    if (!file.get())
    {

        printf("Could not open playlist file: %s\n", filename);
        return false;
    }
    std::string str;
    bool done = false;
    while (!done)
    {

        if (read_line(file.get(), str) == false)
            done = true;
        if (str.size() > 0)
        {

            if (str[0] != '#')
            {

                AppendFile(str.c_str());
            }
        }
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
