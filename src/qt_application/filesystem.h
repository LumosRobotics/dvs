#ifndef MAIN_APPLICATION_FILESYSTEM_H_
#define MAIN_APPLICATION_FILESYSTEM_H_

#ifdef PLATFORM_LINUX_M
#include <experimental/filesystem>
namespace duoplot
{
namespace filesystem = std::experimental::filesystem;
}
#endif

#ifdef PLATFORM_APPLE_M
#include <filesystem>
namespace duoplot
{
namespace filesystem = std::filesystem;
}
#endif

#endif  // MAIN_APPLICATION_FILESYSTEM_H_
