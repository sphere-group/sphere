
#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include "DefaultFileSystem.hpp"
#include "DefaultFile.hpp"

DefaultFileSystem g_DefaultFileSystem;
////////////////////////////////////////////////////////////////////////////////
IFile*
DefaultFileSystem::Open(const char* filename, int mode)
{
#ifdef _WIN32
    if (mode == IFileSystem::read)
    {

        // make sure the filename has the correct case
        char old_directory[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, old_directory);

        // convert the filename into a path and name and set it as current
        const char* name = filename;
        char path[MAX_PATH];
        strcpy(path, filename);
        if (strrchr(path, '\\'))
        {
            *strrchr(path, '\\') = 0;
            name = path + strlen(path) + 1;
        }
        else if (strrchr(path, '/'))
        {
            *strrchr(path, '/') = 0;
            name = path + strlen(path) + 1;
        }

        SetCurrentDirectory(path);
        // enumerate the files to make sure 'filename' is in the list
        WIN32_FIND_DATA ffd;
        HANDLE find = FindFirstFile("*", &ffd);
        if (find == INVALID_HANDLE_VALUE)
        {
            SetCurrentDirectory(old_directory);
            return NULL;
        }

        bool found = false;
        do
        {

            if (strcmp(name, ffd.cFileName) == 0)
            {
                found = true;
                break;
            }

        }
        while (FindNextFile(find, &ffd));
        FindClose(find);

        SetCurrentDirectory(old_directory);
        if (found == false)
        {
            return NULL;
        }

    }
#endif

    // build a fopen() format string
    char md[4] = { 0, 0, 0, 0 };
    char* p = md;

    if (mode & IFileSystem::write)
    {
        *p++ = 'w';
    }
    else if (mode & IFileSystem::read)
    {
        *p++ = 'r';
    }
    *p++ = 'b';  // binary

    // open the file
    FILE* file = fopen(filename, md);
    if (file == NULL)
        return NULL;
    else
        return new DefaultFile(file);
}

////////////////////////////////////////////////////////////////////////////////
