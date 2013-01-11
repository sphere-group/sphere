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
    if (filename)
    {

        Load(filename, fs);
    }
}
////////////////////////////////////////////////////////////////////////////////
int
CConfigFile::GetNumSections() const
{
    return (int)m_sections.size();
}
////////////////////////////////////////////////////////////////////////////////
const char*
CConfigFile::GetSectionName(const int index) const
{
    int j;
    std::map<std::string, Section>::const_iterator i;
    for (j = 0, i = m_sections.begin(); i != m_sections.end(); i++, j++)
    {

        if (j == index)
            return i->first.c_str();
    }
    return NULL;
}
////////////////////////////////////////////////////////////////////////////////
int
CConfigFile::GetNumKeys(const int section) const
{
    if (section == -1)
    {
        int num_keys = 0;
        std::map<std::string, Section>::const_iterator i;
        for (i = m_sections.begin(); i != m_sections.end(); i++)
        {

            const Section& s = i->second;
            std::map<std::string, std::string>::const_iterator j;
            for (j = s.entries.begin(); j != s.entries.end(); j++)
            {

                num_keys += 1;
            }
        }
        return num_keys;
    }
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
std::string
CConfigFile::GetKey(const int section, const int index) const
{
    if (section == -1)
    {
        int num_keys = 0;
        std::map<std::string, Section>::const_iterator i;
        for (i = m_sections.begin(); i != m_sections.end(); i++)
        {

            const Section& s = i->second;
            std::map<std::string, std::string>::const_iterator j;
            for (j = s.entries.begin(); j != s.entries.end(); j++)
            {

                if (num_keys == index)
                {

                    return j->first.c_str();
                }
                num_keys += 1;
            }
        }
        return "";
    }
    return "";
}
////////////////////////////////////////////////////////////////////////////////
bool
CConfigFile::RemoveKey(const int section, const int index) const
{
    if (section == -1)
    {
        int num_keys = 0;
        std::map<std::string, Section>::const_iterator i;
        for (i = m_sections.begin(); i != m_sections.end(); i++)
        {

            const Section& s = i->second;
            std::map<std::string, std::string>::const_iterator j;
			std::string keyname;
            for (j = s.entries.begin(); j != s.entries.end(); j++)
            {

                if (num_keys == index)
                {
					//s.entries.get_allocator()
					//j.erase(i);
					std::map<std::string, std::string> SS = s.entries;
					std::map<std::string, std::string>::iterator it;
					it=SS.find(j->first);
					SS.erase(it);
					return true;

					m_sections.find(i->first);

					SS.empty();
					SS.clear();
						
					//SS.erase(SS.begin());
					//SS.erase(SS.end());
					return true;
					SS.erase(j->first);
					return "";
					keyname = j->first;
					std::map<std::string, std::string>::const_iterator ii;
					ii= s.entries.find(keyname);
					if( ii != s.entries.end() )
					{
						 //s.entries.erase (s.entries.find(keyname));
						//s.entries.erase( ii ++,ii );
					}

					//s.entries.
					//	s.entries.erase(j);
					//m_sections.erase(i->first, j->first);

				// return j->first.c_str();
                }
                num_keys += 1;
            }
        }
        return "";
    }
    return "";
}
////////////////////////////////////////////////////////////////////////////////
std::string
CConfigFile::GetValue(const int section, const int index) const
{
	//if(m_sections.find(section) != m_sections.end())
	//	return m_sections[section];
    return "";
}
////////////////////////////////////////////////////////////////////////////////
inline void skip_whitespace(const char*& str)
{
    while (isspace(*str))
    {

        str++;
    }
}
// returns false if eof
inline bool read_line(IFile* file, std::string& s)
{
    char c;
    s = "";
    if (!file)
    {

        return false;
    }
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
CConfigFile::Load(const char* filename, IFileSystem& fs)
{
    m_sections.erase(m_sections.begin(), m_sections.end());
    // open the file
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::read));
    if (!file.get())
    {

        return false;
    }
    std::string current_section = "";
    bool done = false;
    while (!done)
    {

        // read a line
        std::string line;
        done = !read_line(file.get(), line);
        if (line.size() > 0)
        {

            const char* string = line.c_str();
            // parse it
            // eliminate whitespace
            skip_whitespace(string);
            if (strlen(string) == 0)
                break;
            if (string[0] == '[')
            {  // it's a section

                string++;
                current_section = "";
                while (*string && *string != ']')
                {

                    current_section += *string++;
                }
                string++;
            }
            else
            {                 // it's a key=value pair

                // read key
                std::string key;
                while (*string && *string != '=')
                {

                    key += *string++;
                }
                if (*string == 0)
                {

                    continue; // skip lines without equals
                }
                string++; // skip the '='
                std::string value;
                while (*string)
                {

                    value += *string++;
                }
                // add the item
                WriteString(current_section.c_str(), key.c_str(), value.c_str());
            }
        }
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
inline void write_string(IFile* file, const std::string& s)
{
    file->Write(s.c_str(), s.length());
}
////////////////////////////////////////////////////////////////////////////////
bool
CConfigFile::Save(const char* filename, IFileSystem& fs) const
{
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::write));
    if (!file.get())
    {

        return false;
    }
    // find the section without a name and write that first
    std::map<std::string, Section>::const_iterator i;
    for (i = m_sections.begin(); i != m_sections.end(); i++)
    {

        if (i->first == "")
        {

            const Section& s = i->second;
            std::map<std::string, std::string>::const_iterator j;
            for (j = s.entries.begin(); j != s.entries.end(); j++)
            {

                write_string(file.get(), j->first);
                file->Write("=", 1);
                write_string(file.get(), j->second);
                file->Write("\n", 1);
            }
            file->Write("\n", 1);
        }
    }
    // write the rest of the sections
    for (i = m_sections.begin(); i != m_sections.end(); i++)
    {

        if (i->first != "")
        {

            file->Write("[", 1);
            write_string(file.get(), i->first);
            file->Write("]\n", 2);
            const Section& s = i->second;
            std::map<std::string, std::string>::const_iterator j;
            for (j = s.entries.begin(); j != s.entries.end(); j++)
            {

                write_string(file.get(), j->first);
                file->Write("=", 1);
                write_string(file.get(), j->second);
                file->Write("\n", 1);
            }
            file->Write("\n", 1);
        }
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
std::string
CConfigFile::ReadString(const char* section, const char* key, const char* def)
{
    if (m_sections[section].entries.find(key) == m_sections[section].entries.end())
    {

        m_sections[section].entries[key] = def;
        //std::string s = def;
        //return s;
    }
    return m_sections[section].entries[key];
}
////////////////////////////////////////////////////////////////////////////////
int
CConfigFile::ReadInt(const char* section, const char* key, int def)
{
    //std::ostringstream defstr;
    //defstr << def;
    char defstr[1024] = {0};
    sprintf (defstr, "%d", def);
    return atoi(ReadString(section, key, defstr).c_str());
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
    /*
    std::ostringstream os;
    os << value;
    */
    char valstr[1024] = {0};
    sprintf (valstr, "%d", value);
    WriteString(section, key, valstr);
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
