#pragma warning(disable: 4786)

#if 0 && defined(_DEBUG)
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "Editor.hpp"
#include "MainWindow.hpp"
//#include "Audio.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "Debug.hpp"
#include "../engine/win32/win32_sphere_config.hpp"
#include "../common/LogWindow.hpp"
#include "translate.hpp"
#include <afxmt.h>

static CEditorApplication g_Application;
static CMainWindow* g_MainWindow = NULL;
static std::string s_SphereDirectory;

////////////////////////////////////////////////////////////////////////////////

int GetEnvironmentVariableStr(const char* name, char* value, int max_len)
{
#ifdef WIN32
  return (int) GetEnvironmentVariable(name, value, max_len);
#else
  return 0;
#endif
}

////////////////////////////////////////////////////////////////////////////////

std::string GetSphereDirectory()
{
  char dir[MAX_PATH] = {0};
  if (GetEnvironmentVariableStr("SPHERE_DIRECTORY", dir, MAX_PATH) != 0)
    return dir;
  return s_SphereDirectory;
}

////////////////////////////////////////////////////////////////////////////////

static CStatusBar* s_StatusBar = NULL;

////////////////////////////////////////////////////////////////////////////////
// the repository of all registred instances
// beware of the mutex!
const int MAX_ENTRIES = 100;
const char* SPHERE_FILE_GUID  = "{BD5C7EF2-BE67-4a47-8202-D8A168EFC65C}";
const char* SPHERE_MUTEX_GUID = "{1C7F921C-C758-49d8-9E9E-B5EE7286D5EE}";

////////////////////////////////////////////////////////////////////////////////

class CInstanceRepository
{
public:
  CInstanceRepository() :
    m_Mutex(FALSE, SPHERE_MUTEX_GUID)
    {		
      m_Size = 0;
      m_Created = false;
      m_hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
        (MAX_ENTRIES * sizeof(HWND)) + sizeof(int), SPHERE_FILE_GUID);

      if (!m_hFileMapping) {
        return;
      }

      m_Size = (int*)MapViewOfFile(m_hFileMapping, FILE_MAP_WRITE, 0, 0,
                                 sizeof(int) + (sizeof(HWND) * MAX_ENTRIES));
      m_Instances = (HWND*)(((char*)m_Size) + sizeof(int));
      
      // ASSERT(m_Size != NULL && m_Instances != NULL);
      if (m_Size == NULL || m_Instances == NULL) {
        m_Size = NULL;
        m_Instances = NULL;
        return;
      }
      m_Created = true;
    }

    ///////////////////////////////////////////////////////

	~CInstanceRepository()
	{
      UnmapViewOfFile(m_Size);
      UnmapViewOfFile(m_Instances);
      CloseHandle(m_hFileMapping);
	}

    ///////////////////////////////////////////////////////

	void lock()
	{
      if (m_Created)
  	    m_Mutex.Lock();
	}

    ///////////////////////////////////////////////////////

	void unlock()
	{
	  if (m_Created)
      m_Mutex.Unlock();
	}

    ///////////////////////////////////////////////////////
	
	BOOL isEmpty()
	{
      if (!m_Size)
        return TRUE;
      return *m_Size == 0;
	}

    ///////////////////////////////////////////////////////

	HWND getFirstInstanceWnd()
	{
		return m_Instances[0];
	}

    ///////////////////////////////////////////////////////

	BOOL registerInstance(HWND hwnd)
    {		
      if (!m_Size)
        return FALSE;

      if (*m_Size == MAX_ENTRIES) return FALSE;
		
      m_HWND = hwnd;
      m_Instances[*m_Size] = hwnd;
      (*m_Size)++;

      return TRUE;
	}

    ///////////////////////////////////////////////////////

	BOOL unregisterInstance()
	{
      if (!m_Size)
        return FALSE;

      for (int i = 0; i < *m_Size; i++)
      {
	    if (m_Instances[i] == m_HWND)
		{
		  for (int a = i; a < *m_Size-1; a++)
		  {
			m_Instances[a] = m_Instances[a+1];		
		  }
		
          (*m_Size)--;
		  return TRUE;
		}
	  }
	
      return FALSE;
	}

private:
	// instances represented by their main window HWND
  HWND*  m_Instances;
  int*	 m_Size;
  HANDLE m_hFileMapping;
  CMutex m_Mutex;
  HWND   m_HWND;
  bool   m_Created;
};

////////////////////////////////////////////////////////////////////////////////

// command-line parsers
class CEditorPrestartCommandLineInfo : public CCommandLineInfo
{
public:
  CEditorPrestartCommandLineInfo(HWND foreignMainWindow)
  : m_ForeignMainWindow(foreignMainWindow),
	m_InstanceMayExit(FALSE)
  {
  }
	BOOL mayInstanceExit()
	{
		return m_InstanceMayExit;
	}
private:
  void ParseParam(LPCTSTR parameter, BOOL /*flag*/, BOOL /*last*/)
  {		
    //ATTENTION: This solution only works if project and document game files are not merged
    // at one command line.
    // Furthermore for project files a new instance is opened in general, even if 
    // there are existing instances with no project opened.
    if (!CMainWindow::IsProjectFile(parameter))		
    {
	  // ASSERT(m_ForeignMainWindow != INVALID_HANDLE_VALUE);	
      if (m_ForeignMainWindow == INVALID_HANDLE_VALUE)
        return;
      
      // delegate it to another instance
      COPYDATASTRUCT cds;
      cds.dwData = CD_OPEN_GAME_FILE;
      cds.cbData = lstrlen(parameter)+1;
      cds.lpData = (LPVOID)parameter;				
      ::SendMessage(m_ForeignMainWindow, WM_COPYDATA, 0, (LPARAM)(LPVOID)&cds);				
      SetForegroundWindow(m_ForeignMainWindow);
      m_InstanceMayExit = TRUE;				
    }
  }

private:  
	HWND m_ForeignMainWindow;
	BOOL m_InstanceMayExit;
};

////////////////////////////////////////////////////////////////////////////////

class CEditorCommandLineInfo : public CCommandLineInfo
{
public:
  CEditorCommandLineInfo(CMainWindow* mainwindow)
  : m_MainWindow(mainwindow)
  {
  }
private:
  void ParseParam(LPCTSTR parameter, BOOL /*flag*/, BOOL /*last*/)
  {
    if (m_MainWindow) m_MainWindow->OpenGameFile(parameter);
  }
private:
  CMainWindow* m_MainWindow;
};

////////////////////////////////////////////////////////////////////////////////

CEditorApplication::CEditorApplication()
: CWinApp("Sphere Editor")	
{
  InitializeLog();
}

////////////////////////////////////////////////////////////////////////////////

BOOL
CEditorApplication::InitInstance()
{	
  srand((unsigned int) time(NULL));

#if 0 && defined(CRTDBG_MAP_ALLOC)
  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  _CrtSetBreakAlloc(0);
#endif

  // look for another instance
  m_Instances = new CInstanceRepository();
  
  if (m_Instances) {
    m_Instances->lock();
    if (!m_Instances->isEmpty())
    {		
      CEditorPrestartCommandLineInfo cli(m_Instances->getFirstInstanceWnd());
      ParseCommandLine(cli);
      if (cli.mayInstanceExit())
      {						
        m_Instances->unlock();
        delete m_Instances;
        m_Instances = NULL;
        return FALSE;
      }		
    }
  }

  // set the configuration directory
  char config_directory[MAX_PATH] = {0};
  GetModuleFileName(m_hInstance, config_directory, MAX_PATH); 
  if (strrchr(config_directory, '\\')) {
    *strrchr(config_directory, '\\') = 0;
  }

  std::string editor_config = std::string(config_directory) + "\\editor.ini";
  std::string engine_config = std::string(config_directory) + "\\engine.ini";
  Configuration::ConfigurationFile = editor_config;
  SetCurrentDirectory(config_directory);
  s_SphereDirectory = config_directory;
  SetLanguage(Configuration::Get(KEY_LANGUAGE).c_str());
  // create the main window
  CMainWindow* main_window = new CMainWindow();
  if (!main_window)
    return FALSE;

  main_window->Create();
  m_pMainWnd = main_window;
  g_MainWindow = main_window;
  
  if (m_Instances) {
    // register this instance 
    m_Instances->registerInstance(main_window->m_hWnd);
    m_Instances->unlock();
  }

  SPHERECONFIG sphere_config;
  LoadSphereConfig(&sphere_config, engine_config.c_str());

  // parse the command line
  CEditorCommandLineInfo cli(main_window);
  ParseCommandLine(cli);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

int
CEditorApplication::ExitInstance()
{
  if (m_Instances)
  {
    m_Instances->lock();
    m_Instances->unregisterInstance();
    m_Instances->unlock();
    delete m_Instances;
    m_Instances = NULL;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

void SetStatusBar(CStatusBar* status_bar)
{
  s_StatusBar = status_bar;
}

////////////////////////////////////////////////////////////////////////////////

CStatusBar* GetStatusBar()
{
  return s_StatusBar;
}

////////////////////////////////////////////////////////////////////////////////

CMainWindow* GetMainWindow()
{
  return g_MainWindow;
}

////////////////////////////////////////////////////////////////////////////////

