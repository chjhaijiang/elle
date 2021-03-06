#include <elle/system/self-path.hh>

#if defined(INFINIT_MACOSX)
# include <mach-o/dyld.h>
# include <sys/param.h>
#endif

#include <elle/Exception.hh>
#include <elle/windows.hh>

namespace elle
{
  namespace system
  {
    boost::filesystem::path
    self_path()
    {
#if defined(INFINIT_LINUX)
      return boost::filesystem::read_symlink("/proc/self/exe");
#elif defined(INFINIT_MACOSX)
      uint32_t size = PATH_MAX;
      char result[size];
      if (_NSGetExecutablePath(result, &size) == 0)
        return result;
      else
        throw elle::Exception("unable to get executable path");
#elif defined(INFINIT_WINDOWS)
      char result[1024];
      auto size = GetModuleFileName(0, result, sizeof(result));
      return std::string(result, size);
#else
      throw elle::Exception("unable to get executable path");
#endif
    }
  }
}
