#include "mac_filesystem.h"
#include <errno.h>

using std::string;

std::stack<string> directory_stack;
std::string sphere_directory;

////////////////////////////////////////////////////////////////////////////////
void SetSphereDirectory()
{
    char dir[1024] = { 0 };
    getcwd(dir, 1024);
    sphere_directory = dir;
}

////////////////////////////////////////////////////////////////////////////////
std::string GetSphereDirectory()
{
    return sphere_directory;
}

////////////////////////////////////////////////////////////////////////////////
// return the current working directory
bool GetDirectory (string& directory)
{
    char* dir;

    dir = getcwd(NULL, 0);

    if (dir == NULL)
        return false;

    directory = dir;
    free(dir);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool MakeDirectory (const char* directory)
{
    // we've got to assume a file access mode since sphere isn't aware of these
    if ((mkdir(directory, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0) && (errno != EEXIST))
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool _RemoveDirectory (const char* directory)
{
    if (rmdir(directory) != 0)
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/*
  - enter into a directory
  - This is more analogous to following a link than to cd'ing into a directory
    since you're able to leave the directory later (literally return the previous directory).
    Internally a stack is used to implement this.
*/
bool EnterDirectory (const char* directory)
{
    string current_directory;

    if (!GetDirectory(current_directory))
        return false;

    if (chdir(directory) < 0)
        return false;

    directory_stack.push(current_directory);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/*
  - exit the last entered directory (WARNING: use only after entering a directory)
  - This function returns the program to the directory that it was in
    beforethe last callto EnterDirectory.
*/
bool LeaveDirectory ()
{
    string old_directory;

    if (directory_stack.empty())
        return false; // can't return to a directory when we haven't entered one yet!

    old_directory = directory_stack.top();

    if (chdir(old_directory.c_str()) < 0)
        return false;

    directory_stack.pop();

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/*
  - return a list of directories matching mask
  - The mask is used just like filename globbing in the shell.
*/
DIRECTORYLIST BeginDirectoryList (const char* mask)
{
    DIRECTORYLIST dl = new DIRECTORYLISTimp;
    glob_t matches;
    int lcv;

    memset(&matches, 0, sizeof(matches));
    if (glob(mask, GLOB_ERR | GLOB_MARK, NULL, &matches) != 0)
    {
        delete dl;
        return NULL;
    }

    for (lcv = 0; lcv < matches.gl_pathc; lcv++)
    {
        // we only want to show visible directories
        if ((matches.gl_pathv[lcv][0] != '.') &&
            (matches.gl_pathv[lcv][strlen(matches.gl_pathv[lcv]) - 1] == '/'))
        {
            dl->directories.push_back(matches.gl_pathv[lcv]);
        }
    }

    globfree(&matches);
    dl->current_directory = dl->directories.begin();

    return dl;
}

////////////////////////////////////////////////////////////////////////////////
// clean memory used by the directory list
void EndDirectoryList (DIRECTORYLIST dl)
{
    if (dl != NULL)
    {
        dl->directories.clear();
        delete dl;
    }
}

////////////////////////////////////////////////////////////////////////////////
// determine if the directory list is finished
bool DirectoryListDone (DIRECTORYLIST dl)
{
    if (dl == NULL)
        return true;

    return (dl->current_directory == dl->directories.end());
}

////////////////////////////////////////////////////////////////////////////////
/*
  - get the next directory from the listing
  - Due to GNU's globbing, these are returned alphabetically
*/
void GetNextDirectory (DIRECTORYLIST dl, char directory[FILENAME_MAX])
{
    strcpy(directory, dl->current_directory->c_str());
    dl->current_directory++;
}

////////////////////////////////////////////////////////////////////////////////
/*
  - return a list of files matching mask
  - The mask is used just like filename globbing in the shell.
*/
FILELIST BeginFileList (const char* mask)
{
    FILELIST fl = new FILELISTimp;
    glob_t matches;
    int lcv;

    memset(&matches, 0, sizeof(glob_t));

    if (glob(mask, GLOB_ERR | GLOB_MARK, NULL, &matches) != 0)
        return fl;

    for (lcv = 0; lcv < matches.gl_pathc; lcv++)
    {
        /* we only want to show visible files */
        if ((matches.gl_pathv[lcv][0] != '.') &&
            (matches.gl_pathv[lcv][strlen(matches.gl_pathv[lcv]) - 1] != '/'))
        {
            fl->files.push_back(matches.gl_pathv[lcv]);
        }
    }

    globfree(&matches);
    fl->current_file = fl->files.begin();

    return fl;
}

////////////////////////////////////////////////////////////////////////////////
// clean up the memory used by a file list
void EndFileList (FILELIST fl)
{
    if (fl != NULL)
    {
        fl->files.clear();
        delete fl;
    }
}

////////////////////////////////////////////////////////////////////////////////
// determine if the file list is finished
bool FileListDone (FILELIST fl)
{
    if (fl == NULL)
	    return true;

    return (fl->current_file == fl->files.end());
}

////////////////////////////////////////////////////////////////////////////////
/*
  - get the next directory from the listing
  - Due to GNU's globbing, these are returned alphabetically
*/
void GetNextFile (FILELIST fl, char file[FILENAME_MAX])
{
    strcpy(file, fl->current_file->c_str());
    fl->current_file++;
}

////////////////////////////////////////////////////////////////////////////////
bool _RemoveFile (const char* filename)
{
    if (remove(filename) != 0)
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool _Rename(const char* old_path, const char* new_path)
{
    if (rename(old_path, new_path) != 0)
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
