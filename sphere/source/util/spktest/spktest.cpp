#include <iostream>
#include <memory>
#include <cstdlib>

#include "../../common/system.hpp"
#include "../../common/PackageFileSystem.hpp"

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "spktest <package-file> <package-dir>" << endl;
        return 1;
    }

	CPackageFileSystem fs(argv[1], false);
    SetCurrentDirectory(argv[2]);

    for (int i = 0; i < fs.GetNumFiles(); i++)
    {
		const char* filename = fs.GetFileName(i);
        cout << "Checking " << filename << "...  ";
        
		std::auto_ptr<IFile> ifile(fs.Open(filename, IFileSystem::read));
        if (!ifile.get())
        {
			cout << "File doesn't exist in package!" << endl;
            return 0;
        }

        int isize = ifile->Size();
        char* idata = new char[isize];
        if (!idata)
		{
            return 0;
		}
        
		ifile->Read(idata, isize);

        FILE* cfile = fopen(filename, "rb");
        if (cfile == NULL)
        {
            cout << "File doesn't exist in package!" << endl;
            delete[] idata;
            exit(0);
        }

        fseek(cfile, 0, SEEK_END);
        int csize = ftell(cfile);
        rewind(cfile);
        char* cdata = new char[csize];
        if (!cdata)
        {
            delete[] idata;
            return 0;
        }

        fread(cdata, 1, csize, cfile);
        fclose(cfile);

        if (isize != csize)
        {
            cout << "Incorrect sizes!" << endl;
        }
        else if (memcmp(idata, cdata, isize) != 0)
        {
            cout << "Incorrect data!" << endl;
        }
        else
        {
            cout << "They match!" << endl;
        }

		delete[] idata;
        delete[] cdata;
    }

    return 0;
}
