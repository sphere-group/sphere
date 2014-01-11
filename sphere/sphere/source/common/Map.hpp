#ifndef MAP_HPP
#define MAP_HPP

#include <set>
#include "Layer.hpp"
#include "Tileset.hpp"
#include "Entities.hpp"
class CImage32; // #include "Image32.hpp"

void DeleteEntity(sEntity* entity);
class sMap
{
public:
    // for edge scripts
    enum {
        NORTH = 0,
        EAST  = 1,
        SOUTH = 2,
        WEST  = 3,
    };

    // also known as Trigger Zone ;)
    struct sZone
    {
        int x1;
        int y1;
        int x2;
        int y2;
        int layer;

        int reactivate_in_num_steps;
        std::string script;
    };

public:
    sMap();
    sMap(int width, int height, int layers = 1);
    ~sMap();

    bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Save(const char* filename, IFileSystem& fs = g_DefaultFileSystem);

    bool Create(int width, int height, int layers = 1);
    bool BuildFromImage(CImage32& image, int tile_width = 16, int tile_height = 16, bool allow_duplicates = false);

    void PruneTileset(std::set<int>* allowed_tiles = NULL);
    void ValidateTileIndices();

    void SetTileSize(int width, int height, int method = 0, void (*callback)(int tile, int num_tiles) = NULL);
    bool Import_VergeMAP(const char* filename, const char* tilesetFilename, IFileSystem& fs = g_DefaultFileSystem);
    // accessors
    int GetStartX() const;
    int GetStartY() const;
    int GetStartLayer() const;
    int GetStartDirection() const;
    bool IsRepeating() const;

    const char*   GetMusicFile() const;
    const char*   GetTilesetFile() const;

    const char*   GetEntryScript() const;
    const char*   GetExitScript() const;
    const char*   GetEdgeScript(int edge);
    int           GetNumLayers() const;
    sLayer&       GetLayer(int i);
    const sLayer& GetLayer(int i) const;

    int            GetNumEntities() const;
    sEntity&       GetEntity(int i);
    const sEntity& GetEntity(int i) const;

    sTileset&       GetTileset();
    const sTileset& GetTileset() const;

    int  GetNumZones() const;
    sZone&       GetZone(int index);
    const sZone& GetZone(int index) const;

    // mutators
    void SetStartX(int x);
    void SetStartY(int y);
    void SetStartLayer(int layer);
    void SetStartDirection(int direction);

    void SetRepeating(bool repeat);
    void InsertLayer(int where, const sLayer& layer);
    void InsertLayerFromMap(int where, const sMap& map, int layer);
    void AppendLayer(const sLayer& layer);
    void DeleteLayer(int where, bool remove_tiles = false);
    void SwapLayers(int layer1, int layer2);
    void DuplicateLayer(int layer);

    void AddEntity(sEntity* entity);
    void DeleteEntity(int index);
    int  FindEntity(int x, int y, int layer);

    void AddZone(sZone& zone);
    void DeleteZone(int index);
    int  FindZone(int x, int y, int layer);

    std::vector<int> FindZones(int x, int y, int layer);
    void UpdateZone(int index, int x1, int y1, int x2, int y2);

    void SetMusicFile(const char* music);
    void SetTilesetFile(const char* tileset);

    void SetEntryScript(const char* script);
    void SetExitScript(const char* script);
    void SetEdgeScript(int edge, const char* script);

    void Translate(int dx, int dy, int layer_to_translate = -1);
private:
    void ClearEntityList();
private:
    int m_StartX;
    int m_StartY;
    int m_StartLayer;
    int m_StartDirection;

    bool m_Repeating;
    std::string m_MusicFile;
    std::string m_LastTilesetFile;
    std::string m_TilesetFile;

    std::string m_EntryScript;
    std::string m_ExitScript;
    std::string m_EdgeScripts[4];

    std::vector<sLayer> m_Layers;
    std::vector<sEntity*> m_Entities;
    std::vector<sZone> m_Zones;

    sTileset m_Tileset;
};

inline int
sMap::GetNumLayers() const
{
    return m_Layers.size();
}

inline sLayer&
sMap::GetLayer(int i)
{
    return m_Layers[i];
}

inline const sLayer&
sMap::GetLayer(int i) const
{
    return m_Layers[i];
}

inline sTileset&
sMap::GetTileset()
{
    return m_Tileset;
}

inline const sTileset&
sMap::GetTileset() const
{
    return m_Tileset;
}

inline int
sMap::GetNumZones() const
{
    return m_Zones.size();
}

inline sMap::sZone&
sMap::GetZone(int index)
{
    return m_Zones[index];
}

inline const sMap::sZone&
sMap::GetZone(int index) const
{
    return m_Zones[index];
}

inline void
sMap::SetRepeating(bool repeat)
{
    m_Repeating = repeat;
}
inline bool
sMap::IsRepeating() const
{
    return m_Repeating;
}
#endif
