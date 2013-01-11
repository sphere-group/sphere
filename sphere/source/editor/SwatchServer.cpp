#include <windows.h>
#include "SwatchServer.hpp"
CSwatchServer* CSwatchServer::s_Server = NULL;
////////////////////////////////////////////////////////////////////////////////
CSwatchServer*
CSwatchServer::Instance()
{
  if (!s_Server) {
    s_Server = new CSwatchServer();
    atexit(OnExitHandler);
  }
  return s_Server;
}
////////////////////////////////////////////////////////////////////////////////
CSwatchServer::CSwatchServer()
{
  // load the swatch
  HKEY key;
  if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\AegisKnight\\Sphere", 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
    return;
  }
  // find out how big the swatch buffer is
  DWORD size = 0;
  RegQueryValueEx(key, "SwatchColors", NULL, NULL, NULL, &size);
  if (size == 0) {
    RegCloseKey(key);
    return;
  }
  // read the swatch buffer
  RGBA* buffer = new RGBA[(size + 3) / 4];
  if (buffer) {
    RegQueryValueEx(key, "SwatchColors", NULL, NULL, (LPBYTE)buffer, &size);
    // parse it and add to the server
    RGBA* p = buffer;
    for (unsigned int i = 0; i < size / 4; i++)
      m_Colors.push_back(*p++);
  
    delete[] buffer;
  }
  RegCloseKey(key);
}
////////////////////////////////////////////////////////////////////////////////
CSwatchServer::~CSwatchServer()
{
  // save the swatch
  HKEY key;
  if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\AegisKnight\\Sphere", 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
    return;
  }
  // make the swatch buffer
  RGBA* buffer = new RGBA[m_Colors.size()];
  if (buffer) {
    for (unsigned int i = 0; i < m_Colors.size(); i++)
      buffer[i] = m_Colors[i];
    
    // write the swatch buffer
    RegSetValueEx(key, "SwatchColors", 0, REG_BINARY, (BYTE*)buffer, m_Colors.size() * sizeof(RGBA));
    delete[] buffer;
  }
  RegCloseKey(key);
}
////////////////////////////////////////////////////////////////////////////////
void __cdecl
CSwatchServer::OnExitHandler()
{
  delete s_Server;
  s_Server = NULL;
}
////////////////////////////////////////////////////////////////////////////////
int
CSwatchServer::GetNumColors() const
{
  return m_Colors.size();
}
////////////////////////////////////////////////////////////////////////////////
RGBA
CSwatchServer::GetColor(int i) const
{
  return m_Colors[i];
}
////////////////////////////////////////////////////////////////////////////////
void
CSwatchServer::Clear()
{
  m_Colors.clear();
}
////////////////////////////////////////////////////////////////////////////////
void
CSwatchServer::SetColor(int i, RGBA color)
{
  if (i >= 0 && (unsigned int) i < m_Colors.size())
    m_Colors[i] = color;
  else
    m_Colors.push_back(color);
}
////////////////////////////////////////////////////////////////////////////////
void
CSwatchServer::InsertColor(int i, RGBA color)
{
  if ((unsigned int) i > m_Colors.size())
    i = m_Colors.size();
  m_Colors.insert(m_Colors.begin() + i, color);
}
////////////////////////////////////////////////////////////////////////////////
void
CSwatchServer::DeleteColor(int i)
{
  if (i >= 0 && (unsigned int) i < m_Colors.size())
    m_Colors.erase(m_Colors.begin() + i);
}
////////////////////////////////////////////////////////////////////////////////
