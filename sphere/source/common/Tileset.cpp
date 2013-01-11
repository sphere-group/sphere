#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Tileset.hpp"
#include "Image32.hpp"
#include "endian.hpp"
#include "packed.hpp"
#include "common_palettes.hpp"

////////////////////////////////////////////////////////////////////////////////
sTileset::sTileset() : m_TileWidth(16), m_TileHeight(16) {}
////////////////////////////////////////////////////////////////////////////////
sTileset::sTileset(int num_tiles) : m_TileWidth(16), m_TileHeight(16) 
{
    AppendTiles(num_tiles);
}

////////////////////////////////////////////////////////////////////////////////
sTileset::sTileset(const sTileset& tileset)
{
    m_TileWidth  = tileset.m_TileWidth;
    m_TileHeight = tileset.m_TileHeight;
    m_Tiles      = tileset.m_Tiles;
}

////////////////////////////////////////////////////////////////////////////////
sTileset::~sTileset() {}
////////////////////////////////////////////////////////////////////////////////
sTileset& sTileset::operator=(sTileset& tileset)
{
    if (&tileset == this)
    {
        return *this;
    }

    m_TileWidth  = tileset.m_TileWidth;
    m_TileHeight = tileset.m_TileHeight;
    m_Tiles      = tileset.m_Tiles;

    return *this;
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::Create(int num_tiles)
{
    m_TileWidth  = 16;
    m_TileHeight = 16;
    m_Tiles.resize(num_tiles);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
#define STRUCT_NAME TILESET_HEADER
#define STRUCT_BODY                             \
  byte signature[4];                            \
  word version;                                 \
  word num_tiles;                               \
  word tile_width;                              \
  word tile_height;                             \
  word tile_bpp;                                \
  byte compression;                             \
  byte has_obstructions;                        \
  byte reserved[240];
#include "packed_struct.h"

#define STRUCT_NAME TILE_INFORMATION_BLOCK
#define STRUCT_BODY                             \
  byte obsolete1_;                              \
  byte animated;                                \
  word nexttile;                                \
  word delay;                                   \
  byte obsolete2_;                              \
  byte block_type;                              \
  word num_segments;                            \
  word name_length;                             \
  byte terraformed;								\
  byte reserved[19];
#include "packed_struct.h"

////////////////////////////////////////////////////////////////////////////////
// make sure all structs are the correct size

ASSERT_STRUCT_SIZE(TILESET_HEADER,         256)
ASSERT_STRUCT_SIZE(TILE_INFORMATION_BLOCK, 32)

////////////////////////////////////////////////////////////////////////////////
inline std::string ReadTileString(IFile* file, word length)
{
    std::string s;
    for (int i = 0; i < length; i++)
	{
        char c;
        file->Read(&c, 1);
        s += c;
    }
    
	return s;
}

////////////////////////////////////////////////////////////////////////////////
inline void WriteTileString(IFile* file, std::string str)
{
    file->Write(str.c_str(), str.size());
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::Load(const char* filename, IFileSystem& fs)
{
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::read));
    if (!file.get())
	{
        return false;
	}

    return LoadFromFile(file.get());
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::Import_Image(const char* filename, IFileSystem& fs)
{
    // load the new image
    CImage32 image;
    if (image.Load(filename, fs) == false)
	{
        return false;
	}
    
	// check valid size
    if (image.GetWidth() % 16 != 0 || image.GetHeight() % 16 != 0)
	{
        return false;
	}

    int   i_width  = image.GetWidth();
    int   i_height = image.GetHeight();
    RGBA* i_pixels = image.GetPixels();

    // free the old tileset
    m_Tiles.clear();

    // fill the members
    m_Tiles.resize((i_width / 16) * (i_height / 16));

    // set the pixels
    for (int x = 0; x < (int)image.GetWidth(); x++)
	{
        for (int y = 0; y < (int)image.GetHeight(); y++)
        {
            int tilenum = (y / 16) * (i_width / 16) + (x / 16);
            int tilexoffset = x % 16;
            int tileyoffset = y % 16;

            m_Tiles[tilenum].GetPixels()[tileyoffset * 16 + tilexoffset] = i_pixels[y * i_width + x];
        }
	}

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::Import_VSP(const char* filename, IFileSystem& fs)
{
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::read));
    if (!file.get())
	{
        return false;
	}

    word version;
    RGB palette[256];
    word numtiles;

    file->Read(&version,  2);
    file->Read(palette,   3 * 256);
    file->Read(&numtiles, 2);

    version = ltom_w(version);
    numtiles = ltom_w(numtiles);

    m_Tiles.clear();
    m_Tiles.resize(numtiles);

    // decode the file
    if (version == 2)
    {
        for (int i = 0; i < numtiles; i++)
        {
            byte tile[256];

            file->Read(tile, 256);
            for (int x = 0; x < 16; x++)
			{
                for (int y = 0; y < 16; y++)
				{
                    RGBA color;
                    byte c = tile[y * 16 + x];
					if (c == 0)
                    {
                        color.red = 0;
                        color.green = 0;
                        color.blue = 0;
                        color.alpha = 0;
                    }
                    else
                    {
                        color.red   = (byte)(palette[c].red   * 4);
                        color.green = (byte)(palette[c].green * 4);
                        color.blue  = (byte)(palette[c].blue  * 4);
                        color.alpha = 255;
                    }

                    m_Tiles[i].GetPixels()[y * 16 + x] = color;
                }
			}
        }
    }
    else
    {
        // for that wierd thing aen never told me in the vsp code
        dword dw;
        file->Read(&dw, 4);

        dw = (dword)ltom_w((word)dw);
        // create a temporary buffer while setting up the decoding stuff...
        byte* buffer = (byte*)malloc(numtiles * 256);
        int len = numtiles * 256;
        int counter = 0;

        //start decoding!
        while (counter < len)
        {
            byte c;
            file->Read(&c, 1);

            // if the c was 255 then it's a compressed value
            if (c == 255)
            {
                byte run, color;
                file->Read(&run, 1);
                file->Read(&color, 1);

                for (int i = 0; i < run; i++)
                {
                    buffer[counter] = color;
                    counter++;
                }
            }
            else  // it's a normal value
            {
                buffer[counter] = c;
                counter++;
            }
        }

        // now, tranfer the decoded stuff into the tiles' data structure
        for (int i = 0; i < numtiles; i++)
		{
            for (int j = 0; j < 256; j++)
            {
                if (buffer[i * 256 + j] == 0)
                {
                    m_Tiles[i].GetPixels()[j].red   = 0;
                    m_Tiles[i].GetPixels()[j].green = 0;
                    m_Tiles[i].GetPixels()[j].blue  = 0;
                    m_Tiles[i].GetPixels()[j].alpha = 0;
                }
                else
                {
                    m_Tiles[i].GetPixels()[j].red   = (palette[buffer[i * 256 + j]].red)   * 4;
                    m_Tiles[i].GetPixels()[j].green = (palette[buffer[i * 256 + j]].green) * 4;
                    m_Tiles[i].GetPixels()[j].blue  = (palette[buffer[i * 256 + j]].blue)  * 4;
                    m_Tiles[i].GetPixels()[j].alpha = 255;
                }
            }
		}
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::Import_TST(const char* filename, IFileSystem& fs)
{
    // TST file format created by Christoper B. Matthews for the RPG Toolkit Development System
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::read));
    
	if (!file.get())
	{
        return false;
	}

    // read header
    word version;
    word numtiles;
    word detail;
    file->Read(&version,  2);
    file->Read(&numtiles, 2);
    file->Read(&detail,   2);

    version = ltom_w(version);
    numtiles = ltom_w(numtiles);
    detail = ltom_w(detail);

    // check header for errors
    // only support details 2, 4, 6
    if (version != 20)
    {
        return false;
    }

    if (detail == 2 || detail == 4 || detail == 6)
    {
        return false;
    }

    // allocate new tiles
    m_Tiles.clear();
    m_Tiles.resize(numtiles);

    // read them from file
    for (unsigned int i = 0; i < m_Tiles.size(); i++)
    {
		sTile& tile = m_Tiles[i];
		switch (detail)
		{
		case 2: // 16x16, 24-bit color
			{
				for (int ix = 0; ix < 16; ix++)
				{
					for (int iy = 0; iy < 16; iy++)
					{
						RGB rgb;
						file->Read(&rgb, 3);
						byte alpha = 255;

						if (rgb.red == 0 && rgb.green == 1 && rgb.blue == 2)
						{
							alpha = 0;
						}

						tile.GetPixels()[iy * 16 + ix].red   = rgb.red;
						tile.GetPixels()[iy * 16 + ix].green = rgb.green;
						tile.GetPixels()[iy * 16 + ix].blue  = rgb.blue;
						tile.GetPixels()[iy * 16 + ix].alpha = alpha;
					}
				}
				break;
			}
			// these two are effectively the same format
		case 4: // 16x16, 8-bit color
		case 6: // 16x16, 4-bit color
			{
				for (int ix = 0; ix < 16; ix++)
				{
					for (int iy = 0; iy < 16; iy++)
					{
						byte b;
						file->Read(&b, 1);
						RGB rgb = dos_palette[b];
						byte alpha = 255;

						if (b == 255)
						{
							alpha = 0;
						}

						tile.GetPixels()[iy * 16 + ix].red   = rgb.red;
						tile.GetPixels()[iy * 16 + ix].green = rgb.green;
						tile.GetPixels()[iy * 16 + ix].blue  = rgb.blue;
						tile.GetPixels()[iy * 16 + ix].alpha = alpha;
					}
				}
				break;
			}
		}
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::Save(const char* filename, IFileSystem& fs) const
{
    // open the file
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::write));
    
	if (!file.get())
	{
        return false;
	}
    
	return SaveToFile(file.get());
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::LoadFromFile(IFile* file)
{
    unsigned int i;
    RGBA* pixels;

    // load the header
    TILESET_HEADER header;
    if (file->Read(&header, sizeof(header)) != sizeof(header))
    {
        return false;
    }

    header.version = ltom_w(header.version);
    header.num_tiles = ltom_w(header.num_tiles);
    header.tile_width  = ltom_w(header.tile_width);
    header.tile_height = ltom_w(header.tile_height);
    header.tile_bpp = ltom_w(header.tile_bpp);

    // check the header
    if (memcmp(header.signature, ".rts", 4) != 0)
    {
        printf("Invalid signature in tileset header...\n");
        return false;
    }

    if (header.version != 1)
    {
        printf("Invalid version in tileset header... [%d]\n", header.version);
        return false;
    }

    if (header.tile_bpp != 32)
    {
        printf("Invalid tile_bpp in tileset header... [%d]\n", header.tile_bpp);
        return false;
    }

    if (header.num_tiles <= 0)
    {
        printf ("Invalid number of tiles tileset header... [%d]\n", header.num_tiles);
        return false;
    }

    if (header.tile_width <= 0  || header.tile_width > 4096 || header.tile_height <= 0 
		|| header.tile_height > 4096)
    {
        printf ("Invalid tile size in tileset header... [%d x %d]\n", header.tile_width, header.tile_height);
        return false;
    }

    m_TileWidth  = header.tile_width;
    m_TileHeight = header.tile_height;

    // clear out the old tiles
    m_Tiles.clear();
    m_Tiles.resize(header.num_tiles);

    if (m_Tiles.size() != header.num_tiles)
    {
        return false;
    }

    pixels = new RGBA[m_TileWidth * m_TileHeight];
    if (!pixels)
	{
        return false;
    }

    // load the tiles
    for (i = 0; i < m_Tiles.size(); i++)
    {
        if (file->Read(pixels, sizeof(RGBA) * m_TileWidth * m_TileHeight) != sizeof(RGBA) * m_TileWidth * m_TileHeight)
        {
            delete[] pixels;
            pixels = NULL;
            return false;
        }

        m_Tiles[i].Resize(m_TileWidth, m_TileHeight);
        if (m_Tiles[i].GetWidth() != m_TileWidth && m_Tiles[i].GetHeight() != m_TileHeight)
        {
            delete[] pixels;
            pixels = NULL;
            return false;
        }

        memcpy(m_Tiles[i].GetPixels(), pixels, m_TileWidth * m_TileHeight * sizeof(RGBA));
    }

    delete[] pixels;
    pixels = NULL;

    // load the tile info blocks
    for (i = 0; i < m_Tiles.size(); i++)
    {
        TILE_INFORMATION_BLOCK tib;
        if (file->Read(&tib, sizeof(tib)) != sizeof(tib))
        {
            return false;
        }

        tib.nexttile = ltom_w(tib.nexttile);
        tib.delay = ltom_w(tib.delay);
        tib.num_segments = ltom_w(tib.num_segments);
        tib.name_length = ltom_w(tib.name_length);

        m_Tiles[i].SetAnimated(tib.animated ? true : false);
        m_Tiles[i].SetNextTile(tib.nexttile);
        m_Tiles[i].SetTerraformable(tib.terraformed ? true : false);
        m_Tiles[i].SetDelay(tib.delay);
        m_Tiles[i].SetName(ReadTileString(file, tib.name_length));

        if (header.has_obstructions)
        {
            // skip old obstruction data if it exists
            if (tib.block_type == 1)
			{
                file->Seek(file->Tell() + m_TileWidth * m_TileHeight);
            }
            else if (tib.block_type == 2)
            {
                // read per-tile obstruction segments
                for (int j = 0; j < tib.num_segments; j++)
                {
                    word coordinates[4];
                    file->Read(coordinates, 4 * sizeof(word));

                    coordinates[0] = ltom_w(coordinates[0]);
                    coordinates[1] = ltom_w(coordinates[1]);
                    coordinates[2] = ltom_w(coordinates[2]);
                    coordinates[3] = ltom_w(coordinates[3]);

                    m_Tiles[i].GetObstructionMap().AddSegment
					(
                        coordinates[0], // x1
                        coordinates[1], // y1
                        coordinates[2], // x2
                        coordinates[3]  // y2
                    );
                }
            }
        }
    }

	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::SaveToFile(IFile* file) const
{
    // write header
    TILESET_HEADER header;
    memset(&header, 0, sizeof(header));
    memcpy(header.signature, ".rts", 4);
    header.version = mtol_w(1);
    header.num_tiles   = mtol_w(m_Tiles.size());
    header.tile_width  = mtol_w(m_TileWidth);
    header.tile_height = mtol_w(m_TileHeight);
    header.tile_bpp    = mtol_w(32);
    header.compression = 0;
    header.has_obstructions = 1;

    if (file->Write(&header, sizeof(header)) != sizeof(header))
	{
        return false;
	}
    
	unsigned int i;
    
	// write the tiles
    for (i = 0; i < m_Tiles.size(); i++)
	{
        if (file->Write(m_Tiles[i].GetPixels(), (sizeof(RGBA) * m_TileWidth * m_TileHeight)) != (sizeof(RGBA) * m_TileWidth * m_TileHeight))
		{
            return false;
		}
	}

    // write the tile information blocks
    TILE_INFORMATION_BLOCK tib;
    memset(&tib, 0, sizeof(tib));
    for (i = 0; i < m_Tiles.size(); i++)
    {
        const sObstructionMap& obs_map = m_Tiles[i].GetObstructionMap();

        tib.animated     = m_Tiles[i].IsAnimated();
        tib.nexttile     = mtol_w(m_Tiles[i].GetNextTile());
        tib.delay        = mtol_w(m_Tiles[i].GetDelay());
        tib.block_type   = 2;
        tib.num_segments = mtol_w(obs_map.GetNumSegments());
        tib.name_length  = mtol_w(strlen(m_Tiles[i].GetName().c_str()));
        tib.terraformed  = m_Tiles[i].IsTerraformable();

        if (file->Write(&tib, sizeof(tib)) != sizeof(tib))
		{
            return false;
		}

        // write the tile's name
        if (tib.name_length > 0)
        {
            WriteTileString(file, m_Tiles[i].GetName());
        }

        // write the obstruction segments
        for (int j = 0; j < obs_map.GetNumSegments(); j++)
        {
            word coordinates[4] = {
                                      obs_map.GetSegment(j).x1,
                                      obs_map.GetSegment(j).y1,
                                      obs_map.GetSegment(j).x2,
                                      obs_map.GetSegment(j).y2
                                  };

            coordinates[0] = mtol_w(coordinates[0]);
			coordinates[1] = mtol_w(coordinates[1]);
            coordinates[2] = mtol_w(coordinates[2]);
            coordinates[3] = mtol_w(coordinates[3]);

            if (file->Write(coordinates, (4 * sizeof(word))) != (4 * sizeof(word)))
			{
                return false;
			}
        }
    }
    
	return true;
}

////////////////////////////////////////////////////////////////////////////////
void sTileset::Clear()
{
    m_Tiles.clear();
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::BuildFromImage(CImage32& i, int tile_width, int tile_height, bool allow_duplicates)
{
    int num_x_tiles = (i.GetWidth()  + tile_width  - 1) / tile_width;
    int num_y_tiles = (i.GetHeight() + tile_height - 1) / tile_height;

    CImage32 image = i;
    image.Resize(tile_width * num_x_tiles, tile_height * num_y_tiles);

	if (image.GetWidth() != tile_width * num_x_tiles || image.GetHeight() != tile_height * num_y_tiles)
    {
        return false;
    }

    // clear out the old tiles
    m_Tiles.clear();

    m_TileWidth = tile_width;
    m_TileHeight = tile_height;

    // grab each tile from the image
    for (int ty = 0; ty < num_y_tiles; ty++)
	{
        for (int tx = 0; tx < num_x_tiles; tx++)
        {
            int src_x = tx * tile_width;
            int src_y = ty * tile_height;

            // grab a tile
            RGBA* tile = new RGBA[tile_width * tile_height];
            for (int iy = 0; iy < tile_height; iy++)
            {
                memcpy(tile + iy * tile_width, 
					   image.GetPixels() + (src_y + iy) * image.GetWidth() + src_x, 
					   tile_width * sizeof(RGBA));
            }

            // if the tile is not in the tileset already, add it
            bool in_tileset = false;
            if (!allow_duplicates)
			{
                for (unsigned int i = 0; i < m_Tiles.size(); i++)
                {
                    if (memcmp(tile, m_Tiles[i].GetPixels(), tile_width * tile_height * sizeof(RGBA)) == 0)
                    {
                        in_tileset = true;
                        break;
                    }
                }
            }

            if (!in_tileset)
			{
                AppendTiles(1);
                memcpy(m_Tiles[m_Tiles.size() - 1].GetPixels(), tile, tile_width * tile_height * sizeof(RGBA));
            }

            delete[] tile;
        }
	}

    return true;
}

////////////////////////////////////////////////////////////////////////////////
int sTileset::GetTileWidth() const
{
    return m_TileWidth;
}

////////////////////////////////////////////////////////////////////////////////
int sTileset::GetTileHeight() const
{
    return m_TileHeight;
}

////////////////////////////////////////////////////////////////////////////////
void sTileset::SetTileSize(int w, int h, int method, void (*callback)(int tile, int num_tiles))
{
    m_TileWidth  = w;
    m_TileHeight = h;

    unsigned int i;
	switch (method)
	{
	case 0:
		for (i = 0; i < m_Tiles.size(); i++)
		{
			if (callback) callback(i, m_Tiles.size());
			m_Tiles[i].Resize(w, h);
		}
		break;
	case 1:
		for (i = 0; i < m_Tiles.size(); i++)
		{
			if (callback) callback(i, m_Tiles.size());
			m_Tiles[i].Rescale(w, h);
		}
		break;
	case 2:
		for (i = 0; i < m_Tiles.size(); i++)
		{
			if (callback) callback(i, m_Tiles.size());
			m_Tiles[i].Resample(w, h);
		}
		break;
	}

    if (callback) 
	{
		callback(m_Tiles.size(), m_Tiles.size());
	}
}

////////////////////////////////////////////////////////////////////////////////
void sTileset::InsertTiles(int insert_at, int num_tiles)
{
    // resize the tile array
    m_Tiles.insert(m_Tiles.begin() + insert_at, num_tiles, sTile(m_TileWidth, m_TileHeight));
}

////////////////////////////////////////////////////////////////////////////////
void sTileset::AppendTiles(int num_tiles)
{
    m_Tiles.insert(m_Tiles.end(), num_tiles, sTile(m_TileWidth, m_TileHeight));
}

////////////////////////////////////////////////////////////////////////////////
void sTileset::DeleteTiles(int delete_at, int num_tiles)
{
    m_Tiles.erase(m_Tiles.begin() + delete_at, m_Tiles.begin() + delete_at + num_tiles);
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::InsertImage(int insert_at, const char* filename)
{
    sTileset tileset;
    if (!tileset.Import_Image(filename))
	{
        return false;
	}

    InsertTiles(insert_at, tileset.GetNumTiles());
    for (int i = 0; i < tileset.GetNumTiles(); i++)
	{
        m_Tiles[insert_at + i] = tileset.GetTile(i);
	}

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool sTileset::AppendImage(const char* filename)
{
    sTileset tileset;
    if (!tileset.Import_Image(filename))
	{
        return false;
	}

    int iInsertAt = GetNumTiles();
    AppendTiles(tileset.GetNumTiles());

    for (int i = 0; i < tileset.GetNumTiles(); i++)
	{
        m_Tiles[iInsertAt + i] = tileset.GetTile(i);
	}

    return true;
}

////////////////////////////////////////////////////////////////////////////////
