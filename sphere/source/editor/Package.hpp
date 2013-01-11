#ifndef PACKAGE_HPP
#define PACKAGE_HPP
#include <list>
#include <string>

typedef void (*PackageFileWritten)(const char* filename, int index, int total);

// creates .spk (.tar.gz) Sphere package
class CPackage
{
public:
  void AddFile(const char* filename);
  void RemoveFile(int index);
  int GetNumFiles() const;
  bool Write(const char* filename, PackageFileWritten file_written = NULL);
private:
  std::list<std::string> m_files; // relative paths
};
#endif
