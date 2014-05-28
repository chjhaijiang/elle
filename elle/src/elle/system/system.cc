#ifndef INFINIT_WINDOWS
# include <unistd.h>
# include <sys/types.h>
#endif

#include <elle/system/system.hh>
#include <elle/os/locale.hh>

#include <boost/filesystem/fstream.hpp>

ELLE_LOG_COMPONENT("elle.system");

namespace elle
{
  namespace system
  {
    void truncate(boost::filesystem::path path,
                  uint64_t size)
    {
#ifdef INFINIT_WINDOWS
      HANDLE h = CreateFileW(
        std::wstring(path.string().begin(), path.string().end()).c_str(),
        GENERIC_WRITE,
        0, 0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
      if (h == INVALID_HANDLE_VALUE)
      {
        throw elle::Exception(
          elle::sprintf("unable to create or open file %s: %s",
                        path.string(), GetLastError()));
      }
      LONG offsetHigh = size >> 32;
      SetFilePointer(h,
                     static_cast<DWORD>(size),
                     &offsetHigh,
                     FILE_BEGIN);
      SetEndOfFile(h);
      CloseHandle(h);
#else
      int err = ::truncate(path.string().c_str(), size);
      if (err)
        throw elle::Exception(elle::sprintf("truncate(): error %s:%s",
                                            err, strerror(err)));
#endif
    }

    Buffer read_file_chunk(boost::filesystem::path path,
                           uint64_t offset,
                           uint64_t size)
    {
      // streamsize is garanteed signed, so this fits
      static const uint64_t MAX_offset{
        std::numeric_limits<std::streamsize>::max()};
      static const size_t MAX_buffer{elle::Buffer::max_size};

      if (size > MAX_buffer)
        throw elle::Exception(
          elle::sprintf("buffer that big (%s) can't be addressed", size));

      elle::os::locale::DefaultImbuer u;
      boost::filesystem::ifstream file{path, std::ios::binary};

      if (!file.good())
        throw elle::Exception("file is broken");

      // If the offset is greater than the machine maximum streamsize, seekg n
      // times to reach the right offset.
      while (offset > MAX_offset)
      {
        file.seekg(MAX_offset, std::ios_base::cur);

        if (!file.good())
        throw elle::Exception(
          elle::sprintf("unable to increment offset by %s", MAX_offset));

        offset -= MAX_offset;
      }

      ELLE_DEBUG("seek to offset %s", offset);
      file.seekg(offset, std::ios_base::cur);

      if (!file.good())
      throw elle::Exception(
        elle::sprintf("unable to seek to pos %s", offset));

      elle::Buffer buffer(size);

      ELLE_DEBUG("read file");
      file.read(reinterpret_cast<char*>(buffer.mutable_contents()), size);
      buffer.size(file.gcount());

      ELLE_DEBUG("buffer resized to %s bytes", buffer.size());

      if (!file.eof() && file.fail() || file.bad())
        throw elle::Exception("unable to reathd");

      ELLE_DUMP("buffer read: %x", buffer);
      return buffer;
    }
  }
}
