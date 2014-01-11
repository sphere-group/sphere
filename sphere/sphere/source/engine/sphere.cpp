
// identifier too long
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "sphere.hpp"
#include "benchmark.hpp"
#include "engine.hpp"
#include "render.hpp"
#include "inputx.hpp"

#include "audio.hpp"
#include "filesystem.hpp"
#include "input.hpp"
#include "system.hpp"
#include "time.hpp"

#include "../common/DefaultFileSystem.hpp"
#include "../common/PackageFileSystem.hpp"
#include "../common/configfile.hpp"
#include "../common/sphere_version.h"

#include "../common/strcmp_ci.hpp"
static void RunPackage(IFileSystem& fs);
static void RunGame(const char* game, const char* parameters);
static void GetGameList(std::vector<Game>& games);
static void LoadSystemObjects();
static void DestroySystemObjects();

static SSystemObjects    s_SystemObjects;
static std::vector<Game> s_GameList;
static std::string       s_ScriptDirectory;

////////////////////////////////////////////////////////////////////////////////
void RunSphere(int argc, const char** argv)
{
    // local functions
    struct Local
    {
        static inline bool extension_compare(const char* path, const char* extension)
        {
            int path_length = strlen(path);
            int ext_length  = strlen(extension);
            return (
                       path_length >= ext_length &&
                       strcmp_ci(path + path_length - ext_length, extension) == 0
                   );
        }
    };
    // populate the game list
    GetGameList(s_GameList);

    // load system objects (and store script directory)
    LoadSystemObjects();

    bool show_menu = true;
    int package_name_index = -1;
    int game_name_index = -1;
    // check for manual game selection
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-benchmark") == 0)
        {
            // run video benchmarks
            BeginBenchmarks(s_SystemObjects);
            show_menu = false;
        }
        else if (strcmp(argv[i], "-game") == 0 &&
                 i < argc - 1)
        {  // if last parameter is a command, it doesn't mean anything
            // look for a parameters string
            const char* parameters = "";
            for (int j = 0; j < argc - 1; j++)
            {
                if (strcmp(argv[j], "-parameters") == 0)
                {
                    parameters = argv[j + 1];
                    break;
                }
            }

            // run the game
            if (EnterDirectory("games"))
            {
                RunGame(argv[i + 1], parameters);
                LeaveDirectory();
            }
            else
            {
                QuitMessage("Could not enter 'games' directory");
                return;
            }
            show_menu = false;

        }
        else if (strcmp(argv[i], "-sgm") == 0 &&
                 i < argc - 1)
        {  // if last parameter is a command, it doesn't mean anything
			
			// run the sgm
			if ( ! Local::extension_compare(argv[i+1], ".sgm")){
				QuitMessage("file needs to be '.sgm'");
				return;
			}

			game_name_index = i + 1;

//////////////////////////////

        }
        else if (strcmp(argv[i], "-package") == 0 &&
                 i < argc - 1)
        {  // if last parameter is a command, it doesn't mean anything

            package_name_index = i + 1;
        }

        else if (strcmp(argv[i], "-version") == 0)
        {

            printf ("sphere engine %s [%s]\n", SPHERE_VERSION, __DATE__);
            show_menu = false;
            i = argc;
        }
        else if (strcmp(argv[i], "-help") == 0)
        {
            printf ("engine.exe -benchmark\n");
            printf ("engine.exe -game \"game_folder\"\n");
            printf ("engine.exe -game \"game_folder\" -parameters=\"...\"\n");
            printf ("engine.exe -package \"game_package.spk\"\n");
            printf ("engine.exe -sgm \"spheregame_with_path.sgm\"\n");
            printf ("engine.exe -version\n");
            printf ("engine.exe -help\n");
            show_menu = false;
            i = argc;
        }
        else
        {
            if (i == 1 && Local::extension_compare(argv[i], ".spk"))
                package_name_index = i;
            if (i == 1 && Local::extension_compare(argv[i], ".sgm"))
                game_name_index = i;
        }
    }
    
    if (package_name_index != -1)
    {
        const char* package_name = argv[package_name_index];
        if (strrchr(package_name, '/'))
            package_name = strrchr(package_name, '/') + 1;
        else if (strrchr(package_name, '\\'))
            package_name = strrchr(package_name, '\\') + 1;
	
        // open the package
        CPackageFileSystem fs(argv[package_name_index]);
        if (fs.GetNumFiles() == 0)
        {
            std::ostringstream os;
            os << "Could not open package '" << argv[package_name_index] << "'";
            QuitMessage(os.str().c_str());
            return;
        }
	
        // make sure "packages" directory exists
        MakeDirectory("packages");
        EnterDirectory("packages");
	
        // now make a directory based on the name of the package
        MakeDirectory(package_name);
        EnterDirectory(package_name);
	
        // run the game
        RunPackage(fs);
        show_menu = false;
        LeaveDirectory();
        LeaveDirectory();
    }

    if (game_name_index != -1)

	{
		int pos = strlen(argv[game_name_index]) - 1;

		//We're not interested in the sgm, we want the parent directory, lets find it:
		while( (pos>0) && (argv[game_name_index][pos] != '/' ) && (argv[game_name_index][pos] != '\\' ) ){
			--pos;
		};
		if(pos == 0){
			pos = strlen(argv[game_name_index]) - 1;
		}

		char path[256];
		strncpy(path, argv[game_name_index],pos);
		path[pos] = '\0';
			
		--pos;
		while( (pos>0) && (argv[game_name_index][pos] != '/' ) && (argv[game_name_index][pos] != '\\' ) ){
			--pos;
		};
		if(pos == 0){
			pos = strlen(argv[game_name_index]) - 1;
		}
		path[pos] = '\0';

			// look for a parameters string
			const char* parameters = "";
            for (int j = 0; j < argc - 1; j++)
            {
                if (strcmp(argv[j], "-parameters") == 0)
                {
                    parameters = argv[j + 1];
                    break;
                }
            }

            if (EnterDirectory(path))
            {
                RunGame(path +pos+1, parameters);
                LeaveDirectory();
            }
            else
            {
                QuitMessage("Could not enter 'games' directory");
                return;
            }
            show_menu = false;
    }
    
    // start the game specified on the command line
    if (show_menu)
    {
        if (s_GameList.size() == 1)
        {
            // run the game
            if (EnterDirectory("games"))
            {
                RunGame(s_GameList[0].directory.c_str(), "");
                LeaveDirectory();
            }
            else
            {
                QuitMessage("Could not enter 'games' directory");
                return;
            }
        }
        else
        {
            RunGame("startup", "");
        }
    }

    DestroySystemObjects();
}

////////////////////////////////////////////////////////////////////////////////

void RunPackage(IFileSystem& fs)
{
    CGameEngine(fs, s_SystemObjects, s_GameList, s_ScriptDirectory.c_str(), "").Run();
    ClearKeyQueue();
    ResetInput();

}

////////////////////////////////////////////////////////////////////////////////

std::string DoRunGame(const char* game, const char* parameters)
{
    printf("Game: \"%s\"\n", game);
    printf("Parameters: \"%s\"\n", parameters);

    std::string result;
    if (EnterDirectory(game))
    {
        CGameEngine engine(
            g_DefaultFileSystem,
            s_SystemObjects,
            s_GameList,
            s_ScriptDirectory.c_str(),
            parameters
        );

        bool restarted;
        do
        {
            result = engine.Run();
            restarted = engine.Restarted();

            ClearKeyQueue();
            ResetInput();
        }
        while (restarted);

        LeaveDirectory();
    }
    else
    {

        std::string message = "Could not open game...\n'";
        message += game;
        message += "'";

        QuitMessage(message.c_str());
        return "";
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
Game ReadGameInfo(const char* directory)
{
    Game g;
    if (EnterDirectory(directory))
    {
        // read the game name
        CConfigFile file;
        file.Load("game.sgm");
        std::string gamename    = file.ReadString("", "name", "");
        std::string author      = file.ReadString("", "author", "Unknown");
        std::string description = file.ReadString("", "description", "");

        LeaveDirectory();
        // if the game name is empty, the game doesn't exist
        if (gamename.length() != 0)
        {
            g.name = gamename;
            g.author = author;
            g.description = description;
            g.directory = directory;
            if (g.directory[strlen(g.directory.c_str()) - 1] == '/')
            	g.directory[strlen(g.directory.c_str()) - 1] = '\0';
        }
    }

    return g;
}

///////////////////////////////////////////////////////////////////////////////
void RunGame(const char* game, const char* parameters)
{

    // first = game directory, second = parameters
    typedef std::pair<std::string, std::string> GamePair;
    std::stack<GamePair> games;
    games.push(GamePair(game, parameters));

    while (!games.empty())
    {
        GamePair g = games.top();
        games.pop();

        std::string result = DoRunGame(g.first.c_str(), g.second.c_str());
        if (!result.empty())
        {
            // add the original game back to the stack
            games.push(g);
            games.push(GamePair(result, ""));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void GetGameList(std::vector<Game>& games)
{
    if (!EnterDirectory("games"))
    {
        return;
    }

    // add game directories
    DIRECTORYLIST dl = BeginDirectoryList("*");

    while (!DirectoryListDone(dl))
    {
        char directory[520];
        GetNextDirectory(dl, directory);
        Game g = ReadGameInfo(directory);

        // if the game name is empty, the game doesn't exist
        if (g.name.length() != 0)
        {
            games.push_back(g);
        }

    }
    EndDirectoryList(dl);
    // add game packages
    // alphabetize the menu
    std::sort(games.begin(), games.end());

    LeaveDirectory();
}

////////////////////////////////////////////////////////////////////////////////
void LoadSystemObjects()
{
    if (!EnterDirectory("system"))
    {
        QuitMessage("System directory not found");

        return;
    }

    // store system scripts directory
    if (!EnterDirectory("scripts"))
    {
        QuitMessage("System scripts directory not found");
        return;

    }
    GetDirectory(s_ScriptDirectory);
    LeaveDirectory();

    CConfigFile file;
    if (!file.Load("system.ini"))
    {
        QuitMessage("Unable to open system.ini");

        return;
    }

    std::string font         = file.ReadString("", "Font",        "unknown");
    std::string window_style = file.ReadString("", "WindowStyle", "unknown");
    std::string arrow        = file.ReadString("", "Arrow",       "unknown");
    std::string up_arrow     = file.ReadString("", "UpArrow",     "unknown");
    std::string down_arrow   = file.ReadString("", "DownArrow",   "unknown");

    char error_string[4096 + 1024];
    // system font
    if (!s_SystemObjects.font.Load(font.c_str(), g_DefaultFileSystem))
    {
        sprintf(error_string, "Error: Could not load system font: '%s'", font.c_str());

        QuitMessage(error_string);
        return;

    }
    // system window style
    if (!s_SystemObjects.window_style.Load(window_style.c_str()))
    {
        sprintf(error_string, "Error: Could not load system window style: '%s'", window_style.c_str());

        QuitMessage(error_string);
        return;
    }
    // system arrow
    if (!s_SystemObjects.arrow.Load(arrow.c_str()))
    {

        sprintf(error_string, "Error: Could not load system arrow: '%s'", arrow.c_str());
        QuitMessage(error_string);
        return;
    }
    // system up arrow
    if (!s_SystemObjects.up_arrow.Load(up_arrow.c_str()))
    {
        sprintf(error_string, "Error: Could not load system up arrow: '%s'", up_arrow.c_str());
        QuitMessage(error_string);
        return;

    }
    // system down arrow
    if (!s_SystemObjects.down_arrow.Load(down_arrow.c_str()))
    {
        sprintf(error_string, "Error: Could not load system down arrow: '%s'", down_arrow.c_str());
        QuitMessage(error_string);
        return;

    }
    LeaveDirectory();
}

////////////////////////////////////////////////////////////////////////////////
void DestroySystemObjects()
{}

////////////////////////////////////////////////////////////////////////////////
