#ifndef TILESET_HPP
#define TILESET_HPP

#include <vector>
#include <stdio.h>
#include "Tile.hpp"
#include "DefaultFileSystem.hpp"

class sTileset
{
public:
    sTileset();
    sTileset(int num_tiles);
    sTileset(const sTileset& tileset);
    ~sTileset();

    sTileset& operator=(sTileset& tileset);
    bool Create(int num_tiles);
    bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Import_Image(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Import_VSP(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Import_TST(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Save(const char* filename, IFileSystem& fs = g_DefaultFileSystem) const;

    bool LoadFromFile(IFile* file);
    bool SaveToFile(IFile* file) const;

    void Clear();
    bool BuildFromImage(CImage32& image, int tile_width, int tile_height, bool allow_duplicates);

    int GetTileWidth() const;
    int GetTileHeight() const;
    void SetTileSize(int w, int h, int method = 0, void (*callback)(int tile, int num_tiles) = NULL);

    void InsertTiles(int insert_at, int num_tiles);
    void AppendTiles(int num_tiles);
    void DeleteTiles(int delete_at, int num_tiles);

    bool InsertImage(int insert_at, const char* filename);
    bool AppendImage(const char* filename);

    int          GetNumTiles() const;
    sTile&       GetTile(int i);
    const sTile& GetTile(int i) const;

private:
    int m_TileWidth;
    int m_TileHeight;

    std::vector<sTile> m_Tiles;
};

inline int sTileset::GetNumTiles() const
{
    return m_Tiles.size();
}

inline sTile& sTileset::GetTile(int i)
{
    return m_Tiles[i];
}

inline const sTile& sTileset::GetTile(int i) const
{
    return m_Tiles[i];
}

#endif
