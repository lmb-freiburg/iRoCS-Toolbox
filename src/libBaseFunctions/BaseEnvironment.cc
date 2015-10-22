/**************************************************************************
 *
 * This file is part of the XuV Tools suite. see http://www.xuvtools.org
 * for more information
 *
 * Copyright (C) 2015 Mario Emmenlauer
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/ 

#include <BaseEnvironment.hh>
#include <BaseFile.hh>

#include <iostream>

// standard libraries
#include <string>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <winsock2.h> // for gethostname
#include <windows.h>  // for RegOpenKeyEx
#include <direct.h>   // for mkdir
#ifdef __MINGW32__
#include <Shlobj.h> // For SHGetFolderPath
#endif
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#if !defined(_WIN32) && !defined(OS2)
#include <pwd.h>
#endif


std::string BaseEnvironment::UserHomeDirectory()
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


std::string BaseEnvironment::UserTempDirectory()
{
  std::string vUserTempDir;

#ifdef _WIN32
#ifdef __MINGW32__
  LPTSTR buf = new TCHAR[MAX_PATH];
  if (GetTempPath(MAX_PATH, buf) == 0) return std::string();
  vUserTempDir = buf;
  delete[] buf;
  std::cout << "TMP = " << vUserTempDir << std::endl;
#else
  // Try getting the 'tmp' directory
  char* vPath = NULL;
  size_t vStringLen;
  errno_t vErr = _dupenv_s(&vPath, &vStringLen, "TMP");
  if ((vPath != NULL) && (vErr == 0) && (vStringLen > 0)) {
    vUserTempDir = vPath;
  }
  free(vPath);
  vPath = NULL;
#endif
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


std::string BaseEnvironment::UserAppDataDirectory()
{
  std::string vUserAppDataDir;

#ifdef _WIN32
#ifdef __MINGW32__
  TCHAR *buf = new TCHAR[MAX_PATH];
  if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buf)))
      return std::string();
  vUserAppDataDir = buf;
  delete[] buf;
  std::cout << "APPDATA = " << vUserAppDataDir << std::endl;
#else
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
#endif

  return vUserAppDataDir;
}


std::string BaseEnvironment::HostName()
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


