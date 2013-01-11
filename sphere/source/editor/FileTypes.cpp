#pragma warning(disable : 4786)
#include <vector>
#include <string>
#include <string.h>
#include "FileTypes.hpp"

#include "Configuration.hpp"
#include "Keys.hpp"

#include <corona.h>
#include <audiere.h>

static const char* Extensions[] = {
  /* maps */         "Map Files:rmp(Sphere Map Files(rmp))",
  /* spriteset */    "Spriteset Files:rss(Sphere Spriteset Files(rss))",
  /* scripts */      "Script Files:js(JavaScript Files(js),Text files(txt),CPP Files(cpp,hpp,c,h,cxx,hxx),Java Files(java),Python Files(py),PHP Files(php))",
  /* sounds */       "", // "Sound Files:ogg(MP3 Files(mp3,mp2),Ogg Vorbis Files(ogg),MOD Files(mod,s3m,xm,it),WAV Files(wav),FLAC Files(flac))",
  /* fonts */        "Font Files:rfn(Sphere Font Files(rfn))",
  /* windowstyles */ "Window Style Files:rws(Sphere Window Styles(rws))",
  /* images */       "", // "Image Files:png(JPEG Images(jpeg,jpg,jpe),PNG Images(png),PCX Images(pcx),Windows Bitmap Images(bmp),Truevision Targa(tga),Gif(gif))",
  /* animations */   "Animation Files:mng,flic(MNG Animations(mng),FLIC Animations(flic,flc,fli))",
  /* tilesets */     "Tileset Files:rts(Sphere Tileset Files(rts))",
  /* playlists */    "Playlist Files:m3u(Winamp Playlist Files(m3u))",
  /* packages */     "Package Files:spk(Sphere Package Files(spk))",
};

CFileTypeLibrary FTL;

////////////////////////////////////////////////////////////////////////////////

CFileTypeLibrary::CFileTypeLibrary()
{
  const int num_extensions = sizeof(Extensions) / sizeof(*Extensions);
  m_FileTypes.resize(num_extensions);

  for (int i = 0; i < int(m_FileTypes.size()); i++) {
    const char* s = Extensions[i];
    FileType& ft = m_FileTypes[i];

    // grab the name
    while (*s != ':') {
      ft.name += *s;
      s++;
    }
    s++; // skip ':'

    // grab the default extension
    while (*s != '(') {
      ft.default_extension += *s;
      s++;
    }
    s++;

    // grab all subtypes
    while (*s != ')') {
      SubType st;

      // get label name
      while (*s != '(') {
        st.label += *s;
        s++;
      }
      s++; // skip '('

      // get extensions
      while (*s != ',' && *s != ')') {
        std::string ext;
        while (*s != ',' && *s != ')') {
          ext += *s;
          s++;
        }
        s++; // skip ',' or ')'

        st.extensions.push_back(ext);
      }
      if (*s == ',') {
        s++;
      }

      ft.sub_types.push_back(st);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

int
CFileTypeLibrary::GetNumFileTypes() {
  return m_FileTypes.size();
}

////////////////////////////////////////////////////////////////////////////////

const char*
CFileTypeLibrary::GetFileTypeLabel(int file_type, bool save)
{
  if (file_type == GT_IMAGES) {
    return "Image Files";
  }

  if (file_type == GT_SOUNDS) {
    return "Sound Files";
  }

  return m_FileTypes[file_type].name.c_str();
}

////////////////////////////////////////////////////////////////////////////////

void
CFileTypeLibrary::GetFileTypeExtensions(int file_type, bool save, std::vector<std::string>& extensions)
{
  if (file_type == GT_IMAGES) {
    corona::FileFormatDesc** formats =
      save ? corona::GetSupportedWriteFormats() : corona::GetSupportedReadFormats();

    for (size_t i = 0; formats[i] != NULL; ++i) {
      for (size_t j = 0; j < formats[i]->getExtensionCount(); ++j) {
        extensions.push_back(formats[i]->getExtension(j));
      }
    }
  }
  else if (file_type == GT_SOUNDS) {
    std::vector<audiere::FileFormatDesc> ffd;
    audiere::GetSupportedFileFormats(ffd);

    for (size_t i = 0; i < ffd.size(); ++i) {
      for (size_t j = 0; j < ffd[i].extensions.size(); ++j) {
        extensions.push_back(ffd[i].extensions[j]);
      }
    }
  }
  else if ((file_type == GT_ANIMATIONS && save)
    || (file_type == GT_SCRIPTS && !Configuration::Get(USE_COMMON_TEXT_FILETYPES))) {
    for (unsigned int i = 0; i < 1 && i < m_FileTypes[file_type].sub_types.size(); i++) {
      for (unsigned int j = 0; j < 1 && j < m_FileTypes[file_type].sub_types[i].extensions.size(); j++) {
        extensions.push_back(m_FileTypes[file_type].sub_types[i].extensions[j]);
      }
    }
  } else {
    for (unsigned int i = 0; i < m_FileTypes[file_type].sub_types.size(); i++) {
      for (unsigned int j = 0; j < m_FileTypes[file_type].sub_types[i].extensions.size(); j++) {
        extensions.push_back(m_FileTypes[file_type].sub_types[i].extensions[j]);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

const char*
CFileTypeLibrary::GetDefaultExtension(int file_type, bool save)
{
  if (file_type == GT_IMAGES) {
    corona::FileFormatDesc** formats =
      save ? corona::GetSupportedWriteFormats() : corona::GetSupportedReadFormats();
    return formats[0]->getExtension(0);
  }

  if (file_type == GT_SOUNDS) {
    return "Ogg";
  }

  return m_FileTypes[file_type].default_extension.c_str();
}

////////////////////////////////////////////////////////////////////////////////

int
CFileTypeLibrary::GetNumSubTypes(int file_type, bool save)
{
  if (file_type == GT_IMAGES) {
    corona::FileFormatDesc** formats =
      save ? corona::GetSupportedWriteFormats() : corona::GetSupportedReadFormats();

    int num_sub_types = 0;
    for (size_t i = 0; formats[i] != NULL; ++i) {
      num_sub_types += 1;
    }

    return num_sub_types;
  }

  if (file_type == GT_SOUNDS) {
    std::vector<audiere::FileFormatDesc> ffd;
    audiere::GetSupportedFileFormats(ffd);
    return ffd.size();
  }

  if (file_type == GT_SCRIPTS && !Configuration::Get(USE_COMMON_TEXT_FILETYPES))
    return 1;

  if (file_type == GT_ANIMATIONS && save)
    return 1;

  return m_FileTypes[file_type].sub_types.size();
}

////////////////////////////////////////////////////////////////////////////////

const char* GetImageSubTypeLabel(const char* ext) {
  if (strcmp(ext, "png") == 0)
    return "PNG Images";
  if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0 || strcmp(ext, "jpe") == 0)
    return "JPEG Images";
  if (strcmp(ext, "pcx") == 0)
    return "PCX Images";
  if (strcmp(ext, "bmp") == 0)
    return "Windows Bitmap Images";
  if (strcmp(ext, "tga") == 0)
    return "Truevision Targa";
  if (strcmp(ext, "gif") == 0)
    return "Gif Images";
  if (strcmp(ext, "pbm") == 0)
    return "Portable Bitmap Images";
  if (strcmp(ext, "pgm") == 0)
    return "Portable Graymap Images";
  if (strcmp(ext, "ppm") == 0)
    return "Portable Pixelmap Images";

  return "Unknown Image";
}

////////////////////////////////////////////////////////////////////////////////

const char* GetSoundSubTypeLabel(const char* ext) {
  if (strcmp(ext, "mp3") == 0 || strcmp(ext, "mp2") == 0)
    return "MP3 Files";
  if (strcmp(ext, "ogg") == 0)
    return "Ogg Vorbis Files";
  if (strcmp(ext, "flac") == 0)
    return "FLAC Files";
  if (strcmp(ext, "mod") == 0 || strcmp(ext, "s3m") == 0 || strcmp(ext, "xm") == 0 || strcmp(ext, "it") == 0)
    return "MOD Files";
  if (strcmp(ext, "wav") == 0)
    return "WAV Files";
  if (strcmp(ext, "spx") == 0)
    return "Speex Files";
  if (strcmp(ext, "aiff") == 0 || strcmp(ext, "aifc") == 0)
    return "AIFF Files";
  if (strcmp(ext, "spc") == 0)
    return "SPC Files";
  if (strcmp(ext, "mid") == 0 || strcmp(ext, "midi") == 0 || strcmp(ext, "rmi") == 0)
    return "MIDI Files";
  return "Unknown Sound";
}

////////////////////////////////////////////////////////////////////////////////

const char*
CFileTypeLibrary::GetSubTypeLabel(int file_type, int sub_type, bool save)
{
  if (file_type == GT_IMAGES) {
    corona::FileFormatDesc** formats =
      save ? corona::GetSupportedWriteFormats() : corona::GetSupportedReadFormats();

    for (size_t i = 0; formats[i] != NULL; ++i) {
      if (sub_type == i && formats[i]->getExtensionCount() > 0) {
        return GetImageSubTypeLabel(formats[i]->getExtension(0));
      }
    }

    return "";
  }

  if (file_type == GT_SOUNDS) {
    std::vector<audiere::FileFormatDesc> ffd;
    audiere::GetSupportedFileFormats(ffd);

    for (size_t i = 0; i < ffd.size(); ++i) {
      if (sub_type == i && ffd[i].extensions.size() > 0) {
        return GetSoundSubTypeLabel(ffd[i].extensions[0].c_str());
      }
    }

    return "";
  }

  return m_FileTypes[file_type].sub_types[sub_type].label.c_str();
}

////////////////////////////////////////////////////////////////////////////////

void
CFileTypeLibrary::GetSubTypeExtensions(int file_type, int sub_type, bool save, std::vector<std::string>& extensions)
{
  if (file_type == GT_IMAGES) {
    corona::FileFormatDesc** formats =
      save ? corona::GetSupportedWriteFormats() : corona::GetSupportedReadFormats();

    for (size_t i = 0; formats[i] != NULL; ++i) {
      if (sub_type == i) {
        for (size_t j = 0; j < formats[i]->getExtensionCount(); ++j) {
          extensions.push_back(formats[i]->getExtension(j));
        }
      }
    }
  }
  else if (file_type == GT_SOUNDS) {
    std::vector<audiere::FileFormatDesc> ffd;
    audiere::GetSupportedFileFormats(ffd);

    for (size_t i = 0; i < ffd.size(); ++i) {
      if (sub_type == i) {
        for (size_t j = 0; j < ffd[i].extensions.size(); ++j) {
          extensions.push_back(ffd[i].extensions[j]);
        }
      }
    }
  }
  else {
    extensions = m_FileTypes[file_type].sub_types[sub_type].extensions;
  }
}

////////////////////////////////////////////////////////////////////////////////
