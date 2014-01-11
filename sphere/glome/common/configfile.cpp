#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <sstream>
#include <stdlib.h>
#include <ctype.h>
#include "configfile.hpp"


////////////////////////////////////////////////////////////////////////////////

CConfigFile::CConfigFile(const char* filename, IFileSystem& fs)
{
  if (filename) {
    Load(filename, fs);
  }
}

////////////////////////////////////////////////////////////////////////////////

inline void skip_whitespace(const char*& str)
{
  while (isspace(*str)) {
    str++;
  }
}

// returns false if eof
inline bool read_line(IFile* file, std::string& s)
{
  s = "";
  
  char c;
  if (file->Read(&c, 1) == 0) {
    return false;
  }

  bool eof = false;
  while (!eof && c != '\n') {
    if (c != '\r') {
      s += c;
    }
    eof = (file->Read(&c, 1) == 0);
  }

  return !eof;
}

bool
CConfigFile::Load(const char* filename, IFileSystem& fs)
{
  m_sections.erase(m_sections.begin(), m_sections.end());

  // open the file
  IFile* file = fs.Open(filename, IFileSystem::read);
  if (file == NULL) {
    return false;
  }

  std::string current_section = "";

  bool eof = false;
  while (!eof) {
    // read a line
    std::string line;
    eof = !read_line(file, line);
    const char* string = line.c_str();

    // parse it
    
    // eliminate whitespace
    skip_whitespace(string);

    if (string[0] == '[') {  // it's a section
      string++;

      current_section = "";
      while (*string != ']') {
        current_section += *string++;
      }
      string++;
    } else {                 // it's a key=value pair

      // read key
      std::string key;
      while (*string != '=' && *string) {
        key += *string++;
      }

      if (*string == 0) {
        continue; // skip lines without equals
      }
      string++; // skip the '='

      std::string value;
      while (*string) {
        value += *string++;
      }

      // add the item
      WriteString(current_section.c_str(), key.c_str(), value.c_str());
    }
  }

  file->Close();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

inline void write_string(IFile* file, const std::string& s)
{
  file->Write(s.c_str(), s.length());
}

bool
CConfigFile::Save(const char* filename, IFileSystem& fs) const
{
  IFile* file = fs.Open(filename, IFileSystem::write);
  if (file == NULL) {
    return false;
  }

  // find the section without a name and write that first
  std::map<std::string, Section>::const_iterator i;
  for (i = m_sections.begin(); i != m_sections.end(); i++) {
    if (i->first == "") {
      const Section& s = i->second;
      
      std::map<std::string, std::string>::const_iterator j;
      for (j = s.entries.begin(); j != s.entries.end(); j++) {
        write_string(file, j->first);
        file->Write("=", 1);
        write_string(file, j->second);
        file->Write("\n", 1);
      }

      file->Write("\n", 1);
    }
  }

  // write the rest of the sections
  for (i = m_sections.begin(); i != m_sections.end(); i++) {
    if (i->first != "") {
      file->Write("[", 1);
      write_string(file, i->first);
      file->Write("]\n", 2);

      const Section& s = i->second;
      std::map<std::string, std::string>::const_iterator j;
      for (j = s.entries.begin(); j != s.entries.end(); j++) {
        write_string(file, j->first);
        file->Write("=", 1);
        write_string(file, j->second);
        file->Write("\n", 1);
      }
      
      file->Write("\n", 1);

    }
  }

  file->Close();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

std::string
CConfigFile::ReadString(const char* section, const char* key, const char* def)
{
  if (m_sections[section].entries.find(key) == m_sections[section].entries.end()) {
    m_sections[section].entries[key] = def;
  }
  return m_sections[section].entries[key];
}

////////////////////////////////////////////////////////////////////////////////

int
CConfigFile::ReadInt(const char* section, const char* key, int def)
{
  std::ostringstream defstr;
  defstr << def;

  return atoi(ReadString(section, key, defstr.str().c_str()).c_str());
}

////////////////////////////////////////////////////////////////////////////////

double
CConfigFile::ReadDouble(const char* section, const char* key, double def)
{
  std::ostringstream defstr;
  defstr << def;
  return atof(ReadString(section, key, defstr.str().c_str()).c_str());
}

////////////////////////////////////////////////////////////////////////////////

bool
CConfigFile::ReadBool(const char* section, const char* key, bool def)
{
  return (ReadInt(section, key, def) != 0);
}

////////////////////////////////////////////////////////////////////////////////

void
CConfigFile::WriteString(const char* section, const char* key, const char* value)
{
  m_sections[section].entries[key] = value;
}

////////////////////////////////////////////////////////////////////////////////

void
CConfigFile::WriteInt(const char* section, const char* key, int value)
{
  std::ostringstream os;
  os << value;
  WriteString(section, key, os.str().c_str());
}

////////////////////////////////////////////////////////////////////////////////

void
CConfigFile::WriteDouble(const char* section, const char* key, double value)
{
  std::ostringstream os;
  os << value;
  WriteString(section, key, os.str().c_str());
}

////////////////////////////////////////////////////////////////////////////////

void
CConfigFile::WriteBool(const char* section, const char* key, bool value)
{
  WriteInt(section, key, value);
}

////////////////////////////////////////////////////////////////////////////////
