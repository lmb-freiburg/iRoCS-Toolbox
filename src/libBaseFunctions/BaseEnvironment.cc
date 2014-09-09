/***************************************************************************
 *   This source code is distributed under the license noted in the        *
 *   LICENSE file you should have received along with this source code.    *
 *   If you do not have received the LICENSE file, you can obtain a copy   *
 *   from http://www.xuvtools.org/license.                                 *
 *   The source code is copyright by its author(s). See the AUTHORS        *
 *   file for the list of authors.                                         *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 *                                                                         *
 *         Title:                                                          *
 *   Description:                                                          *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
#include <BaseEnvironment.hh>
#include <BaseFile.hh>

// standard libraries
#include <string>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <winsock2.h> // for gethostname
#include <windows.h>  // for RegOpenKeyEx
//#include <winsock.h>  // for gethostname
#include <direct.h>   // for mkdir
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#if !defined(_WIN32) && !defined(OS2)
#include <pwd.h>
#endif


std::string
BaseEnvironment::
UserHomeDirectory()
{
  std::string vUserHomeDir;

  char* ptr;
#if defined(__linux__) || defined(__APPLE__)
  ptr = getenv("HOME");
  if( ptr != NULL && BaseFile::IsDirectory(ptr)) {
    vUserHomeDir = std::string(ptr);
  } else {
    ptr = (char*)getpwuid(getuid());
    if( ptr != NULL) {
      ptr = ((struct passwd*)ptr)->pw_dir;
      if( ptr != NULL && *ptr != '\0' && BaseFile::IsDirectory(ptr)) {
        vUserHomeDir = ptr;
      } else {
        ptr = ((struct passwd*)ptr)->pw_name;
        if( ptr != NULL && *ptr != '\0') {
          std::string vTmp = std::string("/home/") + ptr;
          if (BaseFile::IsDirectory(vTmp.c_str())) {
            vUserHomeDir = vTmp;
          }
        }
      }
    } else {
      ptr = getlogin();
      if( ptr != NULL) {
        std::string vTmp = std::string("/home/") + ptr;
        if (BaseFile::IsDirectory(vTmp.c_str())) {
          vUserHomeDir = vTmp;
        }
      }
    }
  }
#elif defined(_WIN32)
  ptr = getenv("USERPROFILE");
  if( ptr != NULL && BaseFile::IsDirectory(ptr)) {
    vUserHomeDir = std::string(ptr);
  } else {
    ptr = getenv("HOMEPATH");
    char* ptr2 = getenv("HOMEDRIVE");
    if( ptr != NULL && ptr2 != 0) {
      std::string vTmp = std::string(ptr2) + std::string(ptr);
      if (BaseFile::IsDirectory(vTmp.c_str())) {
        vUserHomeDir = vTmp;
      }
    }
    /*
     else {
        HKEY   hKey;
        TCHAR  szPath[MAX_PATH];
        TCHAR *szProfilePath;
        ULONG  nPathLen = sizeof(szPath);
        TCHAR *szProfileList = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList";
        if(ERROR_SUCCESS != RegOpenKeyEx(HKLM, szProfileList, 0, KEY_READ, &hKey))
          return FALSE;
        if(ERROR_SUCCESS == RegQueryValueEx(hKey, "ProfilesDirectory", 0, REG_SZ, szPath, &nPathLen)) {
          ExpandEnvironmentStrings(szPath, szProfilePath, nLength);
          RegCloseKey(hKey);
          return TRUE;
        } else {
          RegCloseKey(hKey);
          return FALSE;
        }
      }
    */
  }
#else
#error function not defined for your architecture
#endif

  //if (!vUserHomeDir.empty()) {
  //  Reporter::message(3) << "Found an (existing) user home directory at '" << vUserHomeDir << "'.\n";
  //} else {
  //  Reporter::message(3) << "Could not find an (existing) user home directory.\n";
  //}
  return vUserHomeDir;
}


std::string
BaseEnvironment::
UserTempDirectory()
{
  std::string vUserTempDir;

#if defined(_WIN32)
#if defined(__MINGW32__)
#error test this on Windows with MinGW
#endif
  // Try getting the 'tmp' directory
  char* vPath = NULL;
  size_t vStringLen;
  errno_t vErr = _dupenv_s(&vPath, &vStringLen, "TMP");
  if ((vPath != NULL) && (vErr == 0) && (vStringLen > 0)) {
    vUserTempDir = vPath;
  }
  free(vPath);
  vPath = NULL;
#else
  // Try getting the 'tmp' directory
  char* vPath = getenv("TMP");
  if (vPath != NULL) {
    vUserTempDir = vPath;
  } else if (BaseFile::IsDirectory("/tmp")) {
    vUserTempDir = "/tmp";
  }
  free(vPath);
  vPath = NULL;
#endif

  return vUserTempDir;
}


std::string
BaseEnvironment::
UserAppDataDirectory()
{
  std::string vUserAppDataDir;

#if defined(_WIN32)
#if defined(__MINGW32__)
#error test this on Windows with MinGW
#endif
  // Try getting the 'application data' folder
  char* vPath = NULL;
  size_t vStringLen;
  errno_t vErr = _dupenv_s(&vPath, &vStringLen, "APPDATA");
  if ((vPath != NULL) && (vErr == 0) && (vStringLen > 0)) {
    vUserAppDataDir = vPath;
  }
  free(vPath);
  vPath = NULL;
#endif

  return vUserAppDataDir;
}


std::string
BaseEnvironment::
HostName()
{
  std::string hostName;

  char ptr[1024];
  memset( ptr, 0, 1023);
#if defined(__linux__) || defined(__APPLE__)
  if( gethostname( ptr, 1023) == 0) {
    hostName = std::string(ptr);
  }
#elif defined(_WIN32)
  WSADATA WSAData;
  // Initialize winsock dll
  if( WSAStartup(MAKEWORD(1, 0), &WSAData) == 0) {
    if( gethostname( ptr, 1023) == 0) {
      hostName = std::string(ptr);
    }
  }
  // Cleanup
  WSACleanup();
#else
#error function not defined for your architecture
#endif

  //if (!hostName.empty()) {
  //  Reporter::message(3) << "Found host name is '" << hostName << "'\n";
  //} else {
  //  Reporter::message(3) << "Could not find host name.\n";
  //}
  return hostName;
}


