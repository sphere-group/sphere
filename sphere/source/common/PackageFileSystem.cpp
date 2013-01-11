
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "PackageFileSystem.hpp"
#include "PackageFile.hpp"
#include "../common/spk.hpp"

///////////////////////////////////////////////////////////////////////////////
CPackageFileSystem::CPackageFileSystem(const char* filename, bool load_from_filesystem)

        : m_LoadFromFileSystem(load_from_filesystem)
{
    // open file
	m_file = fopen(filename, "rb");
    if (m_file == NULL)
    {

        return;
    }
    // read header
    SPK_HEADER header;
    if (fread(&header, 1, sizeof(header), m_file) != sizeof(header))
    {

        fclose(m_file);
        m_file = NULL;
        return;
    }
    // check for errors
    if (memcmp(header.signature, ".spk", 4) != 0 ||
            header.version != 1)
    {

        fclose(m_file);
        m_file = NULL;
        return;
    }
    // read directory
    fseek(m_file, header.index_offset, SEEK_SET);
    m_directory.resize(header.num_files);
    for (unsigned int i = 0; i < m_directory.size(); i++)
    {

        // read entry
        SPK_ENTRY entry;
        if (fread(&entry, 1, sizeof(entry), m_file) != sizeof(entry))
        {

            fclose(m_file);
            m_file = NULL;
            return;
        }
        m_directory[i].file_offset     = entry.file_offset;
        m_directory[i].file_size       = entry.file_size;
        m_directory[i].compressed_size = entry.compressed_size;
        // read name
        char* name = new char[entry.file_name_length];
        if (name)
        {

            fread(name, 1, entry.file_name_length, m_file);
            m_directory[i].name = name;
            delete[] name;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////
CPackageFileSystem::~CPackageFileSystem()
{
    if (m_file)
    {

        fclose(m_file);
    }
}
///////////////////////////////////////////////////////////////////////////////
inline bool path_compare(const char* p1, const char* p2)
{
    do
    {

        if (*p1 != *p2)
        {

            // \ is considered equivalent to /
            if ((*p1 != '/' || *p2 != '\\') &&
                    (*p1 != '\\' || *p2 != '/'))
            {

                return false;
            }
        }
        *p1++;
        *p2++;
    }
    while (*p1 && *p2);

    return (*p1 == *p2);  // they should both be '\0' at this point
}
IFile*
CPackageFileSystem::Open(const char* filename, int mode)
{
    //FILE* debug_file = fopen("c:\\windows\\desktop\\sphere.txt", "a+");
    //if (debug_file) fprintf (debug_file, "filename: '%s'\n", filename);
    /*
    for (unsigned int i = 0; i < m_directory.size(); i++) {
      if (path_compare(filename, m_directory[i].name.c_str())) {
        if (debug_file) fprintf (debug_file, "found filename: '%s'\n", filename);
      }
    }
    */
    // first of all, look in package
    if (mode == IFileSystem::read)
    {

        // however, loading from filesystem preempts the package
        if (m_LoadFromFileSystem)
        {

            FILE* file = fopen(filename, "rb");
            if (file != NULL)
            {

                return new CPackageFile(file);
            }
        }
        // now look in package
        for (unsigned int i = 0; i < m_directory.size(); i++)
        {

            if (path_compare(filename, m_directory[i].name.c_str()))
            {

                return new CPackageFile(
                           m_file,
                           m_directory[i].file_offset,
                           m_directory[i].file_size,
                           m_directory[i].compressed_size
                       );
            }
        }
    }
    // make fopen()-style mode string
    char md[4] = { 0, 0, 0, 0 };
    char* p = md;
    if (mode & IFileSystem::read)
    {

        *p++ = 'r';
    }
    if (mode & IFileSystem::write)
    {

        *p++ = 'w';
    }
    *p++ = 'b';
    // otherwise, open from filesystem
    FILE* file = fopen(filename, md);
    if (file == NULL)
    {

        return NULL;
    }
    return new CPackageFile(file);
}
///////////////////////////////////////////////////////////////////////////////
int
CPackageFileSystem::GetNumFiles() const
{
    return m_directory.size();
}
///////////////////////////////////////////////////////////////////////////////
const char*
CPackageFileSystem::GetFileName(int i) const
{
    return m_directory[i].name.c_str();
}
///////////////////////////////////////////////////////////////////////////////
