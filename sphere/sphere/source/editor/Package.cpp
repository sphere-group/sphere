#pragma warning(disable : 4786)
#include <string.h>
#include <time.h>
#include <zlib.h>
#include "Package.hpp"
#include "../common/spk.hpp"
const int BLOCK_SIZE = 4096;

////////////////////////////////////////////////////////////////////////////////

void
CPackage::AddFile(const char* filename)
{
  m_files.push_back(filename);  
}

////////////////////////////////////////////////////////////////////////////////

void
CPackage::RemoveFile(int index)
{
  std::list<std::string>::iterator iter = m_files.begin();
  for (int i = 0; i < index; i++)
    iter++;
  m_files.erase(iter);
}

////////////////////////////////////////////////////////////////////////////////

int
CPackage::GetNumFiles() const
{
  return int(m_files.size());
}

////////////////////////////////////////////////////////////////////////////////

// this struct needs external linkage
struct index_entry {
  std::string name;
  dword file_offset;
  dword file_size;
  dword compressed_size;
};

////////////////////////////////////////////////////////////////////////////////

bool 
CPackage::Write(const char* filename, PackageFileWritten file_written)
{
  // open file
  FILE* file = fopen(filename, "wb");
  if (file == NULL) {
    return false;
  }
  // write empty header (will be rewritten later)
  SPK_HEADER header;
  memset(&header, 0, sizeof(header));
  memcpy(header.signature, ".spk", 4);
  header.version = 1;
  header.num_files = 0;     // update this
  header.index_offset = 0;  // update this
  if (fwrite(&header, 1, sizeof(header), file) != sizeof(header)) {
    fclose(file);
    return false;
  }
  // make a directory index
  std::list<index_entry> directory;
  // write all files
  std::list<std::string>::iterator i;
  int file_index = 0;
  for (i = m_files.begin(); i != m_files.end(); i++)
  {
    // open file
    FILE* in = fopen(i->c_str(), "rb");
    if (in == NULL) {
      fclose(file);
      return false;
    }
    header.num_files++;
    // fill out as much of the entry as possible
    index_entry entry;
    entry.name        = *i;
    entry.file_offset = ftell(file);
    z_stream_s stream;
    memset(&stream, 0, sizeof(stream));
    deflateInit(&stream, 9);
    // write file to archive
    Byte in_block[BLOCK_SIZE];
    Byte out_block[BLOCK_SIZE];
    stream.next_out = out_block;
    stream.avail_out = BLOCK_SIZE;
    while (!feof(in)) {
      // read block
      if (stream.avail_in == 0) {
        int read = fread(in_block, 1, sizeof(in_block), in);
        stream.next_in  = in_block;
        stream.avail_in = read;
      }
      // add it to the stream (we should check this for errors...)
      deflate(&stream, 0);
      // if output is ready, write it
      if (stream.avail_out == 0) {
        if (fwrite(out_block, 1, BLOCK_SIZE, file) != BLOCK_SIZE) {
          fclose(file);
          deflateEnd(&stream);
          return false;
        }
        stream.next_out  = out_block;
        stream.avail_out = BLOCK_SIZE;
      }
    }
    // flush the output stream
    while (deflate(&stream, Z_FINISH) != Z_STREAM_END) {
      if (fwrite(out_block, 1, BLOCK_SIZE - stream.avail_out, file) != BLOCK_SIZE - stream.avail_out) {
        fclose(file);
        deflateEnd(&stream);
        return false;
      }
      stream.next_out  = out_block;
      stream.avail_out = BLOCK_SIZE;
    }
    // write the *final* bit of compressed data
    if (stream.avail_out != BLOCK_SIZE) {
      if (fwrite(out_block, 1, BLOCK_SIZE - stream.avail_out, file) != BLOCK_SIZE - stream.avail_out) {
        fclose(file);
        deflateEnd(&stream);
        return false;
      }
    }
    deflateEnd(&stream);
    
    // fill out the rest of the entry and add it
    entry.file_size       = ftell(in);
    entry.compressed_size = ftell(file) - entry.file_offset;
    directory.push_back(entry);
    
    fclose(in);
    if (file_written)
      file_written(i->c_str(), file_index++, m_files.size());
  }
  // write file directory
  header.index_offset = ftell(file);
  std::list<index_entry>::iterator j;
  for (j = directory.begin(); j != directory.end(); j++) {
    
    SPK_ENTRY entry;
    entry.entry_version = 1;
    entry.file_name_length = j->name.length() + 1;
    entry.file_offset = j->file_offset;
    entry.file_size = j->file_size;
    entry.compressed_size = j->compressed_size;
    if (fwrite(&entry, 1, sizeof(entry), file) != sizeof(entry)
     || fwrite(j->name.c_str(), 1, j->name.length() + 1, file) != j->name.length() + 1) {
      fclose(file);
      return false;
    }
  }
  // rewrite the header
  rewind(file);
  if (fwrite(&header, 1, sizeof(header), file) != sizeof(header)) {
    fclose(file);
    return false;
  }
  fclose(file);
  // 100% done now
  if (file_written)
    file_written("", file_index++, m_files.size());
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////

/*
bool
CPackage::Write(const char* filename)
{
  zipFile file = zipOpen(filename, 0);
  if (file == NULL) {
    return false;
  }
  // write all files
  std::list<std::string>::iterator i;
  for (i = m_files.begin(); i != m_files.end(); i++) {
    // open file
    FILE* in = fopen(i->c_str(), "rb");
    if (in == NULL) {
      continue;
    }
    // get current time/date
    time_t s = time(NULL);
    tm* t = localtime(&s);
    
    zip_fileinfo fi;
    fi.tmz_date.tm_sec  = t->tm_sec;
    fi.tmz_date.tm_min  = t->tm_min;
    fi.tmz_date.tm_hour = t->tm_hour;
    fi.tmz_date.tm_mday = t->tm_mday;
    fi.tmz_date.tm_mon  = t->tm_mon;
    fi.tmz_date.tm_year = t->tm_year;
    fi.dosDate = 0;
    fi.internal_fa = 0;
    fi.external_fa = 0;
    
    zipOpenNewFileInZip(file, i->c_str(), &fi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    // write file to zip
    char block[BLOCK_SIZE];
    while (!feof(in)) {
      int read = fread(block, 1, sizeof(block), in);
      zipWriteInFileInZip(file, block, read);
    }
    
    zipCloseFileInZip(file);
    fclose(in);
  }
  zipClose(file, "Sphere Package");
  return true;
}
/*
template<int W>
inline void octal(char s[W], int val)
{
  for (int i = W - 1; i >= 0; i--) {
    s[i] = '0' + (val % 8);
    val /= 8;
  }
}
template<typename T>
inline int checksum(T* t) {
  int sum = 0;
  char* c = (char*)t;
  for (int i = 0; i < sizeof(T); i++) {
    sum += c[i];
  }
  return sum;
}
bool
CPackage::Write(const char* filename)
{
  gzFile file = gzopen(filename, "wb");
  // write a tar archive
  block b;
  std::list<std::string>::iterator i;
  for (i = m_files.begin(); i != m_files.end(); i++) {
    // open file
    FILE* in = fopen(i->c_str(), "rb");
    if (in == NULL) {
      continue;
    }
    // calculate size
    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    rewind(in);
    // write header block
    memset(&b.header, 0, sizeof(b.header));
    strncpy(b.header.name, i->c_str(), 100);
    octal<8> (b.header.mode, TUREAD | TUWRITE | TGREAD | TOREAD);
    octal<8> (b.header.uid,  0);
    octal<8> (b.header.gid,  0);
    octal<12>(b.header.size, size);
    octal<12>(b.header.mtime, time(NULL));  // don't worry about modification times
    memset(b.header.chksum, ' ', 8);
    b.header.typeflag = REGTYPE;
    strcpy(b.header.magic, TMAGIC);
    strcpy(b.header.uname, "sphere");
    strcpy(b.header.gname, "games");
    octal<8>(b.header.devmajor, 0);
    octal<8>(b.header.devminor, 0);
    octal<8>(b.header.chksum, checksum(&b));
    gzwrite(file, &b, sizeof(b));
    // write content blocks
    while (!feof(in)) {
      memset(&b, 0, sizeof(b)); // in case the read doesn't finish
      fread(&b, 1, sizeof(b), in);
      gzwrite(file, &b, sizeof(b));
    }
    fclose(in);
  }
  // write an empty block
  memset(&b, 0, sizeof(b));
  gzwrite(file, &b, sizeof(b));
  gzclose(file);
  return true;
}
*/
////////////////////////////////////////////////////////////////////////////////
