/**********************************************************************
 * @brief Utilities for getting system information. These functions are mostly
 *untested since I only develop on Mac (usually).
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

/**
 * WARNING: Most of these functions are untested - don't be surprised if they
 * break.
 *
 */

#pragma once

#include <iostream>
#include <string>
#include <thread>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#elif defined(__APPLE__)
// Mac OS
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

namespace utils {

namespace system {

/**
 * Get current operating system.
 * Untested.
 */
inline std::string get_os() {
#ifdef _WIN32
  return "Windows";
#elif __APPLE__
  return "OSX";
#elif __linux__
  return "Linux";
#else
  return "Unknown";
#endif
}

/**
 * Get number of cores
 * Untested.
 */
inline unsigned int get_num_cores() {
#ifdef _WIN32
  SYSTEM_INFO siSysInfo;
  GetSystemInfo(&siSysInfo);
  return siSysInfo.dwNumberOfProcessors;
#elif __APPLE__
  int nm[2];
  size_t len = 4;
  uint32_t count;

  nm[0] = CTL_HW;
  nm[1] = HW_AVAILCPU;
  sysctl(nm, 2, &count, &len, NULL, 0);

  if (count < 1) {
    nm[1] = HW_NCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);
    if (count < 1) {
      count = 1;
    }
  }
  return count;
#elif __linux__
  return sysconf(_SC_NPROCESSORS_ONLN);
#else
  return 0;
#endif
}

/**
 * Get system memory in GB.
 * Untested.
 */
inline unsigned long long get_memory_GB() {
#ifdef _WIN32
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  GlobalMemoryStatusEx(&statex);
  return statex.ullTotalPhys / (1024 * 1024 * 1024);
#elif __APPLE__
  int mib[2];
  int64_t physical_memory;
  size_t length;

  mib[0] = CTL_HW;
  mib[1] = HW_MEMSIZE;
  length = sizeof(int64_t);
  sysctl(mib, 2, &physical_memory, &length, NULL, 0);
  return (unsigned long long)physical_memory / (1024 * 1024 * 1024);
#elif __linux__
  struct sysinfo memInfo;
  sysinfo(&memInfo);
  return (unsigned long long)memInfo.totalram / (1024 * 1024 * 1024);
#else
  return 0;
#endif
}

/**
 * Get processor name
 * Untested.
 */
inline std::string get_processor_name() {
  std::string cpu_name = "Unknown";
#ifdef _WIN32
  // Windows
  HKEY hKey;
  char value[1024];
  DWORD value_length = 1024;
  RegOpenKey(HKEY_LOCAL_MACHINE,
             "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", &hKey);
  RegQueryValueEx(hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)&value,
                  &value_length);
  RegCloseKey(hKey);
  cpu_name = std::string(value);
#elif __APPLE__
  // macOS
  char buffer[128];
  size_t bufferlen = 128;
  sysctlbyname("machdep.cpu.brand_string", &buffer, &bufferlen, NULL, 0);
  cpu_name = std::string(buffer);
#elif __linux__
  // Linux
  FILE *pipe = popen("cat /proc/cpuinfo | grep 'model name' | uniq", "r");
  if (!pipe)
    return "Unknown";
  char buffer[128];
  fgets(buffer, sizeof(buffer), pipe);
  pclose(pipe);
  cpu_name = std::string(buffer);
#endif
  return cpu_name;
}

} // namespace system
} // namespace utils
