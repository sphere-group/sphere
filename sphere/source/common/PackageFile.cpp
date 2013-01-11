#include <string.h>
#include "PackageFile.hpp"
////////////////////////////////////////////////////////////////////////////////
void
CPackageFile::Close()
{
    if (!m_in_package)
    {

        fclose(m_file);
        m_file = NULL;
    }
    else
    {

        inflateEnd(&m_stream);
    }
    delete this;
}
////////////////////////////////////////////////////////////////////////////////
template<typename T>
T minimum(T a, T b)
{

    return (a < b ? a : b);
}
template<typename T>
T maximum(T a, T b)
{

    return (a > b ? a : b);
}
int
CPackageFile::Read(void* bytes, int size)
{
    if (m_in_package)
    {

        // if we try to read too far in the file, truncate the read
        if (m_position + size > m_length)
        {

            size = m_length - m_position;
        }
        m_stream.avail_out = size;
        m_stream.next_out = (Byte*)bytes;
        while (m_stream.avail_out != 0)
        {

            // make sure we have input data
            if (m_stream.avail_in == 0)
            {

                fseek(m_file, m_start + m_next_read, SEEK_SET);
                int should_read = minimum<int>(m_compressed_length - m_next_read, PACKAGE_BUFFER_SIZE);
                int amount_read = fread(m_in_buffer, 1, should_read, m_file);
                m_stream.next_in = m_in_buffer;
                m_stream.avail_in = amount_read;
                m_next_read += amount_read;
            }
            inflate(&m_stream, 0);
        }
        m_position += size;
        return size;
    }
    else
    {

        return fread(bytes, 1, size, m_file);
    }
}
////////////////////////////////////////////////////////////////////////////////
int
CPackageFile::Write(const void* bytes, int size)
{
    if (m_in_package)
    {

        return 0;
    }
    else
    {

        return fwrite(bytes, 1, size, m_file);
    }
}
////////////////////////////////////////////////////////////////////////////////
int
CPackageFile::Size()
{
    if (m_in_package)
    {

        return m_length;
    }
    else
    {

        long curpos = ftell(m_file);
        fseek(m_file, 0, SEEK_END);
        long end = ftell(m_file);
        fseek(m_file, curpos, SEEK_SET);
        return end;
    }
}
////////////////////////////////////////////////////////////////////////////////
int
CPackageFile::Tell()
{
    if (m_in_package)
    {

        return m_position;
    }
    else
    {

        return ftell(m_file);
    }
}
////////////////////////////////////////////////////////////////////////////////
void
CPackageFile::Seek(int position)
{
    if (m_in_package)
    {

        // if we're seeking forwards, simply do a read and discard the data
        if (position > m_position)
        {

            byte buffer[PACKAGE_BUFFER_SIZE];
            int bytes_to_read = position - m_position;
            while (bytes_to_read > 0)
            {

                int bytes_read = minimum(PACKAGE_BUFFER_SIZE, bytes_to_read);
                Read(buffer, bytes_read);
                bytes_to_read -= bytes_read;
            }
        }
        // if we're seeking backwards, jump to the beginning of the stream and seek forwards
        else if (position < m_position)
        {

            m_position = 0;
            m_next_read = 0;
            m_stream.avail_in = 0;
            // reset zlib stream
            inflateEnd(&m_stream);
            memset(&m_stream, 0, sizeof(m_stream));
            inflateInit(&m_stream);
            m_stream.next_in = m_in_buffer;
            Seek(position);
        }
    }
    else
    {

        fseek(m_file, m_start + position, SEEK_SET);
    }
}
////////////////////////////////////////////////////////////////////////////////
CPackageFile::CPackageFile(FILE* file)
        : m_in_package(false)
        , m_file(file)
{
}

////////////////////////////////////////////////////////////////////////////////
CPackageFile::CPackageFile(FILE* file, int start, int length, int compressed_length)
        : m_in_package(true)
        , m_file(file)
        , m_start(start)
        , m_length(length)
        , m_compressed_length(compressed_length)
        , m_position(0)
        , m_next_read(0)
{
    // initialize the zlib stream
    memset(&m_stream, 0, sizeof(m_stream));
    inflateInit(&m_stream);
    m_stream.avail_in = 0;
    m_stream.next_in = m_in_buffer;
}
////////////////////////////////////////////////////////////////////////////////
